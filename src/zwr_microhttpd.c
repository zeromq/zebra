/*  =========================================================================
    zwr_microhttpdd - HTTP web server backend using libmicrohttpd.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zwr_microhttpdd - HTTP web server backend using libmicrohttpd.
@discuss
@end
*/

#include "platform.h"
#include "../include/zwebrap.h"
#include "zwebrap_classes.h"
#if defined (HAVE_LIBCURL)
#include "zwr_curl_client.c"
#endif

#define errorpage "<html><body>This doesn't seem to be right.</body></html>"
#define busypage "<html><body>Service is busy, try again later.</body></html>"
#define notfoundpage "<html><body>The requested resource does not exist.</body></html>"

//  Structure of our actor

struct _zwr_microhttpd_t {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
    //  Declare properties
    struct timeval *start_time;
    struct MHD_Daemon *daemon;
    char *endpoints;          //  Address of dispatching endpoint
    int port;
};


//  Local helper functions

char *
s_concat (char *s1, char *s2)
{
    size_t len1 = strlen (s1);
    size_t len2 = strlen (s2);
    char *result = (char *) malloc (len1 + len2 + 1);    //+1 for the zero-terminator
    assert (result);
    memcpy (result, s1, len1);
    memcpy (result + len1, s2, len2 + 1);       //+1 to copy the null-terminator
    return result;
}

//  --------------------------------------------------------------------------
//  Create a new zwr_microhttpdd.

static zwr_microhttpd_t *
zwr_microhttpd_new (zsock_t *pipe, void *args)
{
    zwr_microhttpd_t *self = (zwr_microhttpd_t *) zmalloc (sizeof (zwr_microhttpd_t));
    assert (self);

    self->pipe = pipe;
    self->endpoints = NULL;
    self->terminated = false;
    self->poller = zpoller_new (self->pipe, NULL);

    //  Initialize properties
    self->start_time = (struct timeval *) zmalloc (sizeof (struct timeval));
    gettimeofday(self->start_time, 0);
    self->daemon = NULL;
    self->port = 8888;      //  Set 8888 to be the default port

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zwr_microhttpdd.

static void
zwr_microhttpd_destroy (zwr_microhttpd_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_microhttpd_t *self = *self_p;

        //  Free actor properties
        if (self->endpoints)
            free (self->endpoints);
        free (self->start_time);

        //  Free object itself
        zpoller_destroy (&self->poller);
        free (self);
        *self_p = NULL;
    }
}


static int
on_client_connect (void *cls,
                   const struct sockaddr *addr,
                   socklen_t addrlen)
{
    struct sockaddr_in *client_addr = (struct sockaddr_in *) addr;
    printf ("Client-Addr: %s:%d\n", inet_ntoa (client_addr->sin_addr), ntohs (client_addr->sin_port));
    return MHD_YES;
}

static int
s_send_static_response (struct MHD_Connection *con, char *content_type, char *content_body, int status_code)
{
    int rc = 0;
    struct MHD_Response *http_response;
    size_t content_length = strlen (content_body);
    http_response = MHD_create_response_from_buffer (content_length,
                                                     (void *) content_body,
                                                     MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header (http_response, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
    if (!http_response)
        return MHD_NO;
    rc = MHD_queue_response (con, status_code, http_response);
    printf ("Send Page: %d\n", status_code);
    MHD_destroy_response (http_response);
    return rc;
}


static int
s_send_response (struct MHD_Connection *con, xrap_msg_t *response)
{
    int rc = 0;
    struct MHD_Response *http_response;
    int status_code = xrap_msg_status_code (response);
    //  Create new http response
    if (XRAP_MSG_GET_OK  == xrap_msg_id (response) ||
        XRAP_MSG_POST_OK == xrap_msg_id (response) ||
        XRAP_MSG_PUT_OK  == xrap_msg_id (response)) {
        const char *content_type = xrap_msg_content_type (response);
        const char *content_body = xrap_msg_content_body (response);
        size_t content_length = strlen (content_body);
        http_response = MHD_create_response_from_buffer (content_length,
                                                         (void *) content_body,
                                                         MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
    }
    else
    if (XRAP_MSG_ERROR == xrap_msg_id (response)) {
        const char *content_body = xrap_msg_status_text (response);
        size_t content_length = strlen (content_body);
        http_response = MHD_create_response_from_buffer (content_length,
                                                         (void *) content_body,
                                                         MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/plain");
    }
    else {
        http_response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
    }

    //  Attach headers based on response id
    if (XRAP_MSG_POST_OK == xrap_msg_id (response) ||
        XRAP_MSG_PUT_OK  == xrap_msg_id (response)) {
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_LOCATION, xrap_msg_location (response));
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_ETAG, xrap_msg_etag (response));
        char modified[64];
        rc = sprintf (modified, "%"PRIu64, xrap_msg_date_modified (response));
        if (rc > 0)
            MHD_add_response_header (http_response, MHD_HTTP_HEADER_LAST_MODIFIED, modified);
    }

    if (!http_response)
        return MHD_NO;
    rc = MHD_queue_response (con, status_code, http_response);
    printf ("Send Page: %d\n", status_code);
    MHD_destroy_response (http_response);
    return rc;
}

static int
s_append_connection_value (void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
    assert (cls);
    zwr_request_t *request = (zwr_request_t *) cls;
    zhashx_t *mapping = NULL;
    if (MHD_HEADER_KIND == kind)
        mapping = zwr_request_header (request);
    else
    if (MHD_GET_ARGUMENT_KIND == kind)
        mapping = zwr_request_query (request);
    else
        return MHD_NO;
    zhashx_insert (mapping, (char *) key, (char *) value);
    return MHD_YES;
}

static xrap_msg_t *
s_build_xrap_message (zwr_microhttpd_t *self, zwr_connection_t *con)
{
    zwr_request_t *req = zwr_connection_request (con);
    const char *method = zwr_request_action (req);
    zhashx_t *req_header = zwr_request_header (req);

    xrap_msg_t *xrap_msg = NULL;
    if (streq (MHD_HTTP_METHOD_GET, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_GET);
        xrap_msg_set_resource (xrap_msg, zwr_request_path (req));
        xrap_msg_set_if_modified_since (xrap_msg, 0);
        const char *if_none_match = (const char *) zhashx_lookup (req_header, MHD_HTTP_HEADER_IF_NONE_MATCH);
        xrap_msg_set_if_none_match (xrap_msg, if_none_match);
        const char *content_type = (const char *) zhashx_lookup (req_header, MHD_HTTP_HEADER_ACCEPT);
        xrap_msg_set_content_type (xrap_msg, content_type);
    }
    else
    if (streq (MHD_HTTP_METHOD_POST, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_POST);
        xrap_msg_set_parent (xrap_msg, zwr_request_path (req));
        const char *content_type = (const char *) zhashx_lookup (req_header, MHD_HTTP_HEADER_CONTENT_TYPE);
        xrap_msg_set_content_type (xrap_msg, content_type);
        xrap_msg_set_content_body (xrap_msg, zwr_request_data (req));
    }
    else
    if (streq (MHD_HTTP_METHOD_PUT, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_PUT);
        xrap_msg_set_resource (xrap_msg, zwr_request_path (req));
        xrap_msg_set_if_unmodified_since (xrap_msg, 0);
        const char *if_match = (const char *) zhashx_lookup (req_header, MHD_HTTP_HEADER_IF_MATCH);
        xrap_msg_set_if_match (xrap_msg, if_match);
        const char *content_type = (const char *) zhashx_lookup (req_header, MHD_HTTP_HEADER_CONTENT_TYPE);
        xrap_msg_set_content_type (xrap_msg, content_type);
        xrap_msg_set_content_body (xrap_msg, zwr_request_data (req));
    }
    else
    if (streq (MHD_HTTP_METHOD_DELETE, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_DELETE);
        xrap_msg_set_resource (xrap_msg, zwr_request_path (req));
        xrap_msg_set_if_unmodified_since (xrap_msg, 0);
        const char *if_match = (const char *) zhashx_lookup (req_header, MHD_HTTP_HEADER_IF_MATCH);
        xrap_msg_set_if_match (xrap_msg, if_match);
    }

    return xrap_msg;
}


static int
answer_to_connection (void *cls,
                      struct MHD_Connection *con,
                      const char *url,
                      const char *method,
                      const char *version,
                      const char *uploaded_data, size_t *uploaded_data_size,
                      void **con_cls)
{
    assert (cls);
    int rc;
    zwr_microhttpd_t *self = (zwr_microhttpd_t *) cls;
    zwr_connection_t *connection = NULL;

    //  New http request will have nil con_cls
    if (NULL == *con_cls) {
        zwr_request_t *request = zwr_request_new (url, method, version);
        //  Add header and query key/value pairs to request
        MHD_get_connection_values (con, MHD_HEADER_KIND, &s_append_connection_value, request);
        MHD_get_connection_values (con, MHD_GET_ARGUMENT_KIND, &s_append_connection_value, request);
        //  Create a connection wrapper for both request and response.
        connection = zwr_connection_new (request);
        //  The connection wrapper is passed as parameter to this function
        *con_cls = (void *) connection;

        if (streq (method, MHD_HTTP_METHOD_POST) || streq (method, MHD_HTTP_METHOD_PUT)) {
            char *content_type = (char *) zhashx_lookup (zwr_request_header (request) , "Content-Type");
            if (content_type) {
                //  POST and PUT data is provided on second call to this method,
                //  thus return MHD_YES which indicates that the request is not yet finished.
                return MHD_YES;
            }
            else {
                //  If Content-Type is not set for POST/PUT something must have went wrong
                return s_send_static_response (con, "text/html", errorpage, MHD_HTTP_BAD_REQUEST);
            }
        }
    }
    //  Get connection for an already running request
    else {
        connection = (zwr_connection_t *) *con_cls;
    }
    //  DEBUG: debug print of http request
    zwr_request_print (zwr_connection_request (connection));
    //  Start request processing
    if ((0 == strcmp (method, MHD_HTTP_METHOD_POST) ||
         0 == strcmp (method, MHD_HTTP_METHOD_PUT)) && *uploaded_data_size != 0) {
            zwr_request_set_data (zwr_connection_request (connection),  uploaded_data, *uploaded_data_size);
            *uploaded_data_size = 0;
            return MHD_YES;
    }
    //  Create backend socket to dispatch request
    zsock_t *client = zsock_new_dealer (">inproc://http_dispatcher");
    //  Register response dispatch route
    zstr_sendx (client, "REGISTER", s_concat (s_concat (s_concat ("/", (char *) method), "-OK"), (char *) url), NULL);
    rc = zsock_wait (client);
    if (rc != 0) {
        //  423 - Busy
        return s_send_static_response (con, "text/html", busypage, MHD_HTTP_LOCKED);
    }
    //  Send dispatch request in XRAP format
    xrap_msg_t *xrap_msg = s_build_xrap_message (self, connection);
    zmsg_t *request = xrap_msg_encode (&xrap_msg);
    assert (request);
    zmsg_pushstr (request, s_concat (s_concat ("/", (char *) method), (char *) url));
    zmsg_pushstr (request, "DISPATCH");
    zmsg_send (&request, client);
    rc = zsock_wait (client);
    if (rc != 0) {
        //  404 - not found
        return s_send_static_response (con, "text/html", notfoundpage, MHD_HTTP_NOT_FOUND);
    }
    //  Receive response
    xrap_msg_t *xrap_response = xrap_msg_recv (client);
    assert (xrap_response);
    //  Unregister response dispatch route
    zstr_sendx (client, "UNREGISTER", s_concat (s_concat (s_concat ("/", (char *) method), "-OK"), (char *) url), NULL);
    rc = zsock_wait (client);
    assert (rc == 0);
    zsock_destroy (&client);

    return s_send_response (con, xrap_response);
}


static void
request_completed (void *cls, struct MHD_Connection *con,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
    assert (con_cls);
    if (*con_cls) {
        zwr_connection_t *connection = (zwr_connection_t *) *con_cls;
        zwr_connection_destroy (&connection);
    }
    *con_cls = NULL;
}



//  Start this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
zwr_microhttpd_start (zwr_microhttpd_t *self)
{
    assert (self);
    self->daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION, self->port,
                                     &on_client_connect, self,
                                     &answer_to_connection, self,
                                     MHD_OPTION_NOTIFY_COMPLETED, &request_completed, self,
                                     MHD_OPTION_END);
    if (self->daemon)
        return 0;
    return -1;
}


//  Stop this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
zwr_microhttpd_stop (zwr_microhttpd_t *self)
{
    assert (self);
    MHD_stop_daemon (self->daemon);
    return 0;
}


//  Create and attach the dispatching endpoint

static int
s_configure_endpoint (zwr_microhttpd_t *self, zmsg_t *request)
{
    char *endpoints = zmsg_popstr (request);
    assert (endpoints);
    if (self->endpoints)
       free (self->endpoints);
    self->endpoints = endpoints;
    return 0;
}


static int
s_configure_port (zwr_microhttpd_t *self, zmsg_t *request)
{
    char *port_str = zmsg_popstr (request);
    assert (port_str);
    sscanf (port_str, "%d", &self->port);
    if (self->daemon) {
        zwr_microhttpd_stop (self);
        zwr_microhttpd_start (self);
    }
    zstr_free (&port_str);
    return 0;
}


//  Here we handle incomming message from the node

static void
zwr_microhttpd_recv_api (zwr_microhttpd_t *self)
{
//  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "START"))
        zsock_signal (self->pipe, zwr_microhttpd_start (self));
    else
    if (streq (command, "STOP"))
        zsock_signal (self->pipe, zwr_microhttpd_stop (self));
    else
    if (streq (command, "ENDPOINT"))
        zsock_signal (self->pipe, s_configure_endpoint (self, request));
    else
    if (streq (command, "PORT"))
        zsock_signal (self->pipe, s_configure_port (self, request));
    else
    if (streq (command, "VERBOSE")) {
        self->verbose = true;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
zwr_microhttpd_actor (zsock_t *pipe, void *args)
{
    zwr_microhttpd_t * self = zwr_microhttpd_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
       zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 0);
       if (which == self->pipe)
          zwr_microhttpd_recv_api (self);
       //  Add other sockets when you need them.
    }

    zwr_microhttpd_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Self test of this actor.

void
zwr_microhttpd_test (bool verbose)
{
    printf (" * zwr_microhttpd: ");
#if defined (HAVE_LIBCURL)
    int rc = 0;
    //  @selftest
    //  Simple create/destroy test
    zactor_t *zwr_microhttpd = zactor_new (zwr_microhttpd_actor, NULL);

    zstr_send (zwr_microhttpd, "START");
    rc = zsock_wait (zwr_microhttpd);             //  Wait until actor started
    assert (rc == 0);

    zstr_sendx (zwr_microhttpd, "PORT", "8081", NULL);
    rc = zsock_wait (zwr_microhttpd);             //  Wait until port is configured
    assert (rc == 0);

    zstr_sendx (zwr_microhttpd, "ENDPOINT", "inproc://http_dispatcher", NULL);
    rc = zsock_wait (zwr_microhttpd);             //  Wait until endpoint configured
    assert (rc == 0);

    zactor_t *dispatcher = zactor_new (zwr_dispatcher, NULL);

    zstr_sendx (dispatcher, "VERBOSE", NULL);
    zsock_wait (dispatcher);

    zstr_sendx (dispatcher, "ENDPOINT", "inproc://http_dispatcher", NULL);
    rc = zsock_wait (dispatcher);             //  Wait until endpoint configured
    assert (rc == 0);

    zsock_t *handler = zsock_new_dealer (">inproc://http_dispatcher");
    zstr_sendx (handler, "REGISTER", "/GET/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    zwr_curl_client_t *curl = zwr_curl_client_new ();
    zwr_curl_client_send_get (curl, "http://localhost:8081/foo/bar");

    xrap_msg_t *request = xrap_msg_recv (handler);
    assert (request);
    assert (xrap_msg_id (request) == XRAP_MSG_GET);
    xrap_msg_destroy (&request);

    xrap_msg_t *response = xrap_msg_new (XRAP_MSG_GET_OK);
    xrap_msg_set_status_code (response, 200);
    xrap_msg_set_content_type (response, "text/plain");
    xrap_msg_set_content_body (response, "test1234");
    zmsg_t *msg = xrap_msg_encode (&response);
    assert (msg);
    zmsg_pushstr (msg, "/GET-OK/foo/bar");
    zmsg_pushstr (msg, "DISPATCH");
    zmsg_send (&msg, handler);

    zstr_sendx (handler, "UNREGISTER", "/GET/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    zwr_curl_client_verify_response (curl, 200, "");
    zwr_curl_client_destroy (&curl);

    /*zsock_destroy (&handler);*/
    /*handler = zsock_new_dealer (">inproc://http_dispatcher");*/

    zstr_sendx (handler, "REGISTER", "/POST/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    curl = zwr_curl_client_new ();
    zwr_curl_client_send_post (curl, "http://localhost:8081/foo/bar", "abc");

    do {
        msg = zmsg_recv (handler);
        assert (msg);
    } while (!is_xrap_msg (msg));
    request = xrap_msg_decode (&msg);
    assert (request);
    assert (xrap_msg_id (request) == XRAP_MSG_POST);
    xrap_msg_destroy (&request);

    response = xrap_msg_new (XRAP_MSG_POST_OK);
    xrap_msg_set_status_code (response, 201);
    xrap_msg_set_location (response, "/foo/bar");
    xrap_msg_set_etag (response, "a3fsd3");
    xrap_msg_set_date_modified (response, 0);
    xrap_msg_set_content_type (response, "text/plain");
    xrap_msg_set_content_body (response, "test1234");
    msg = xrap_msg_encode (&response);
    assert (msg);
    zmsg_pushstr (msg, "/POST-OK/foo/bar");
    zmsg_pushstr (msg, "DISPATCH");
    zmsg_send (&msg, handler);

    zstr_sendx (handler, "UNREGISTER", "/POST/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    zsock_destroy (&handler);

    //  Give response time to arrive
    usleep (250);
    zactor_destroy (&dispatcher);

    zstr_send (zwr_microhttpd, "STOP");
    rc = zsock_wait (zwr_microhttpd);             //  Wait until actor stopped
    assert (rc == 0);

    zactor_destroy (&zwr_microhttpd);
    //  @end
#endif
    printf ("OK\n");
}
