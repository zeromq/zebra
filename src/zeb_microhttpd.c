/*  =========================================================================
    zeb_microhttpdd - Simple HTTP web server

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZEBRA.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    Simple HTTP webserver implementation using the libmicrohttpd library.
    Incomming HTTP request are converted to XRAP and send to the broker.
    Responses from the broker are converted back into HTTP.
@discuss
@end
*/

#include "platform.h"
#include "zebra_classes.h"
#if defined (HAVE_LIBCURL)
#include "zeb_curl_client.c"
#endif

#define X_RATELIMIT_LIMIT 10
#define X_RATELIMIT_INTERVAL 60000

#define PAGE_USER_AGENT_REQUIRED "\
Request forbidden by administrative rules.\n\
Please make sure your request has a User-Agent header."

#define PAGE_CONTENT_TYPE_REQUIRED "\
Problems parsing content.\n\
Please make sure your request has a Content-Type header."

#define PAGE_METHOD_NOT_IMPLEMENTED "\
Request HTTP method is not implemented.\n\
Please make sure your request method is GET, POST, PUT or DELETE"

#define PAGE_NOT_FOUND "\
The requested resource does not exist.\n\
Please make sure to request a valid resource."

#define PAGE_LIMIT_EXCEEDED "\
API rate limit exceeded.\n\
Wait until rate limit resets to make further request"

#define PAGE_BUSY "\
The service is currently busy.\n\
Please try again later."

//  Structure of our actor

struct _zeb_ratelimit_t {
        void *ticket;
        int limit;
        int remaining;
        int interval;
        int64_t reset;
};
typedef struct _zeb_ratelimit_t zeb_ratelimit_t;

struct _zeb_microhttpd_t {
    zsock_t *pipe;              //  Actor command pipe
    zloop_t *reactor;           //  Socket poller and ratelimit timer
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
    //  Declare properties
    zconfig_t *config;          //  Configuration tree
    struct timeval *start_time;
    struct MHD_Daemon *daemon;
    char *endpoints;            //  Address of dispatching endpoint
    int port;
    //  Rate Limiting
    zhashx_t *ratelimit_clients; //  Holds ratelimit for all clients
    size_t ratelimit_limit;      //  Default value for ratelimit
    size_t ratelimit_interval;   //  Default value for ratelimit interval
};


//  Local helper functions

static void
s_destroy_ratelimit (void **self_p);

static int
s_timer_dummy (zloop_t *self, int timer_id, void *arg)
{
    return 0;
}

//  --------------------------------------------------------------------------
//  Create a new zeb_microhttpdd.

static zeb_microhttpd_t *
zeb_microhttpd_new (zsock_t *pipe, void *args)
{
    zeb_microhttpd_t *self = (zeb_microhttpd_t *) zmalloc (sizeof (zeb_microhttpd_t));
    assert (self);

    self->pipe = pipe;
    self->endpoints = NULL;
    self->terminated = false;

    //  Initialize properties
    self->start_time = (struct timeval *) zmalloc (sizeof (struct timeval));
    gettimeofday(self->start_time, 0);
    self->daemon = NULL;
    self->port = 8888;      //  Set 8888 to be the default port
    self->ratelimit_limit = X_RATELIMIT_LIMIT;
    self->ratelimit_interval = X_RATELIMIT_INTERVAL;

    self->reactor = zloop_new ();
    zloop_set_ticket_delay (self->reactor, self->ratelimit_interval);
    zloop_timer (self->reactor, 1000, 0, s_timer_dummy, NULL);
    self->ratelimit_clients = zhashx_new ();
    zhashx_set_destructor (self->ratelimit_clients, s_destroy_ratelimit);

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zeb_microhttpdd.

static void
s_destroy_ratelimit (void **self_p)
{
    assert (self_p);
    if (*self_p) {
        zeb_ratelimit_t *self = (zeb_ratelimit_t * ) *self_p;
        free (self);
    }
}


static void
zeb_microhttpd_destroy (zeb_microhttpd_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zeb_microhttpd_t *self = *self_p;

        //  Free actor properties
        if (self->endpoints)
            free (self->endpoints);
        free (self->start_time);
        zloop_destroy (&self->reactor);
        zhashx_destroy (&self->ratelimit_clients);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

static int
zeb_microhttpd_reset_limit (zloop_t *loop, int timer_id, void *arg)
{
    assert (loop);
    zeb_ratelimit_t *ratelimit = (zeb_ratelimit_t *) arg;
    assert (ratelimit);
    zloop_ticket_delete (loop, ratelimit->ticket);
    ratelimit->remaining = ratelimit->limit;
    ratelimit->reset = (zclock_time () + ratelimit->interval) / 1000;
    ratelimit->ticket = zloop_ticket (loop, zeb_microhttpd_reset_limit, ratelimit);
    return 0;
}

static zeb_ratelimit_t *
zeb_microhttpd_ratelimit_new (zeb_microhttpd_t *self, char *id)
{
    assert (self);
    zeb_ratelimit_t *ratelimit = (zeb_ratelimit_t *) zmalloc (sizeof (zeb_ratelimit_t));
    ratelimit->limit = self->ratelimit_limit;
    ratelimit->remaining = ratelimit->limit;
    ratelimit->interval = self->ratelimit_interval;
    ratelimit->reset = (zclock_time () + ratelimit->interval) / 1000;
    ratelimit->ticket = zloop_ticket (self->reactor, zeb_microhttpd_reset_limit, ratelimit);
    zhashx_insert (self->ratelimit_clients, id, ratelimit);
    return ratelimit;
}

static bool
zeb_microhttpd_limit_exceeded (zeb_microhttpd_t *self, char *id)
{
    assert (self);
    zeb_ratelimit_t *ratelimit = (zeb_ratelimit_t *) zhashx_lookup (self->ratelimit_clients, id);
    if (ratelimit)
       if (ratelimit->remaining == 0)
        return true;
    return false;
}

static int
on_client_connect (void *cls,
                   const struct sockaddr *addr,
                   socklen_t addrlen)
{
    /*zeb_microhttpd_t *self = (zeb_microhttpd_t *) cls;*/
    /*struct sockaddr_in *client_addr = (struct sockaddr_in *) addr;*/
    /*const char *ip = inet_ntoa (client_addr->sin_addr);*/
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
    MHD_destroy_response (http_response);
    return rc;
}


static int
s_send_response (struct MHD_Connection *con, zeb_connection_t *connection, zeb_microhttpd_t *self)
{
    int rc = 0;
    struct MHD_Response *http_response;
    zeb_response_t *zeb_response = zeb_connection_response (connection);
    xrap_msg_t *response = zeb_response_xresponse (zeb_response);
    int status_code = xrap_msg_status_code (response);
    //  Create new http response
    if (XRAP_MSG_GET_OK  == xrap_msg_id (response) ||
        XRAP_MSG_POST_OK == xrap_msg_id (response)) {
        const char *content_type = xrap_msg_content_type (response);
        const char *content_body = xrap_msg_content_body (response);
        size_t content_length = strlen (content_body);
        http_response = MHD_create_response_from_buffer (content_length,
                                                         (void *) content_body,
                                                         MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_ETAG, xrap_msg_etag (response));
    }
    else
    if (XRAP_MSG_PUT_OK  == xrap_msg_id (response)) {
        http_response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header (http_response, MHD_HTTP_HEADER_ETAG, xrap_msg_etag (response));
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
        char modified[64];
        rc = sprintf (modified, "%"PRIu64, xrap_msg_date_modified (response));
        if (rc > 0)
            MHD_add_response_header (http_response, MHD_HTTP_HEADER_LAST_MODIFIED, modified);
    }

    //  Attache custom headers
    zhash_t *metadata = xrap_msg_metadata (response);
    if (metadata && zhash_size (metadata) > 0) {
        zlist_t *keys = zhash_keys (metadata);
        const char *key = (const char *) zlist_first (keys);
        while (key) {
            MHD_add_response_header (http_response, key, (char *) zhash_lookup (metadata, key));
            key = (const char *) zlist_next (keys);
        }
    }

    //  Rate-Limiting
    char limit[10];
    sprintf (limit, "%d", zeb_response_rate_limit (zeb_response));
    char remaining[10];
    sprintf (remaining, "%d", zeb_response_rate_remaining (zeb_response));
    char reset[20];
    sprintf (reset, "%"PRId64, zeb_response_rate_reset (zeb_response));
    MHD_add_response_header (http_response, "X-RateLimit-Limit", limit);
    MHD_add_response_header (http_response, "X-RateLimit-Remaining", remaining);
    MHD_add_response_header (http_response, "X-RateLimit-Reset", reset);

    if (!http_response)
        return MHD_NO;

    rc = MHD_queue_response (con, status_code, http_response);
    //  Cleanup
    MHD_destroy_response (http_response);
    return rc;
}

static int
s_append_connection_value (void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
    assert (cls);
    if (!key || !value)
        return MHD_YES;     //  Ignore connection value if invalid!

    zeb_request_t *request = (zeb_request_t *) cls;
    zhash_t *mapping = NULL;
    if (MHD_HEADER_KIND == kind)
        mapping = zeb_request_header (request);
    else
    if (MHD_GET_ARGUMENT_KIND == kind)
        mapping = zeb_request_query (request);
    else
        return MHD_NO;

    zhash_insert (mapping, (char *) key, (char *) value);
    return MHD_YES;
}

static xrap_msg_t *
s_build_xrap_message (zeb_microhttpd_t *self, zeb_connection_t *con)
{
    zeb_request_t *req = zeb_connection_request (con);
    const char *method = zeb_request_action (req);
    zhash_t *req_header = zeb_request_header (req);
    zhash_t *parameters = zeb_request_query (req);

    xrap_msg_t *xrap_msg = NULL;
    if (streq (MHD_HTTP_METHOD_GET, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_GET);
        xrap_msg_set_resource (xrap_msg, zeb_request_path (req));
        xrap_msg_set_if_modified_since (xrap_msg, 0);
        const char *if_none_match = (const char *) zhash_lookup (req_header, MHD_HTTP_HEADER_IF_NONE_MATCH);
        xrap_msg_set_if_none_match (xrap_msg, if_none_match);
        const char *content_type = (const char *) zhash_lookup (req_header, MHD_HTTP_HEADER_ACCEPT);
        xrap_msg_set_content_type (xrap_msg, content_type);
        if (zhash_size (parameters) > 0) {
            zhash_t *parameters_dup = zhash_dup (parameters);
            xrap_msg_set_parameters (xrap_msg, &parameters_dup);
        }
    }
    else
    if (streq (MHD_HTTP_METHOD_POST, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_POST);
        xrap_msg_set_parent (xrap_msg, zeb_request_path (req));
        const char *content_type = (const char *) zhash_lookup (req_header, MHD_HTTP_HEADER_CONTENT_TYPE);
        xrap_msg_set_content_type (xrap_msg, content_type);
        xrap_msg_set_content_body (xrap_msg, zeb_request_data (req));
    }
    else
    if (streq (MHD_HTTP_METHOD_PUT, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_PUT);
        xrap_msg_set_resource (xrap_msg, zeb_request_path (req));
        xrap_msg_set_if_unmodified_since (xrap_msg, 0);
        const char *if_match = (const char *) zhash_lookup (req_header, MHD_HTTP_HEADER_IF_MATCH);
        xrap_msg_set_if_match (xrap_msg, if_match);
        const char *content_type = (const char *) zhash_lookup (req_header, MHD_HTTP_HEADER_CONTENT_TYPE);
        xrap_msg_set_content_type (xrap_msg, content_type);
        xrap_msg_set_content_body (xrap_msg, zeb_request_data (req));
    }
    else
    if (streq (MHD_HTTP_METHOD_DELETE, method)) {
        xrap_msg = xrap_msg_new (XRAP_MSG_DELETE);
        xrap_msg_set_resource (xrap_msg, zeb_request_path (req));
        xrap_msg_set_if_unmodified_since (xrap_msg, 0);
        const char *if_match = (const char *) zhash_lookup (req_header, MHD_HTTP_HEADER_IF_MATCH);
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
    zeb_microhttpd_t *self = (zeb_microhttpd_t *) cls;
    zeb_connection_t *connection = NULL;

    //  New http request will have nil con_cls
    if (NULL == *con_cls) {
        zeb_request_t *request = zeb_request_new (url, method, version);
        //  Add header and query key/value pairs to request
        MHD_get_connection_values (con, MHD_HEADER_KIND, &s_append_connection_value, request);
        MHD_get_connection_values (con, MHD_GET_ARGUMENT_KIND, &s_append_connection_value, request);
        //  Create a connection wrapper for both request and response.
        connection = zeb_connection_new (request);
        //  The connection wrapper is passed as parameter to this function
        *con_cls = (void *) connection;

        char *user_agent =  (char *) zhash_lookup (zeb_request_header (request), "User-Agent");
        if (!user_agent)
            return s_send_static_response (con, "text/html", PAGE_USER_AGENT_REQUIRED, MHD_HTTP_FORBIDDEN);

        if (zeb_microhttpd_limit_exceeded (self, user_agent))
            return s_send_static_response (con, "text/html", PAGE_LIMIT_EXCEEDED, MHD_HTTP_FORBIDDEN);

        if (streq (method, MHD_HTTP_METHOD_POST) || streq (method, MHD_HTTP_METHOD_PUT)) {
            char *content_type = (char *) zhash_lookup (zeb_request_header (request) , "Content-Type");
            if (content_type) {
                //  POST and PUT data is provided on second call to this method,
                //  thus return MHD_YES which indicates that the request is not yet finished.
                return MHD_YES;
            }
            else {
                //  If Content-Type is not set for POST/PUT something must have went wrong
                return s_send_static_response (con, "text/html",
                                               PAGE_CONTENT_TYPE_REQUIRED, MHD_HTTP_BAD_REQUEST);
            }
        }
    }
    //  Get connection for an already running request
    else {
        connection = (zeb_connection_t *) *con_cls;
    }

    if (self->verbose)
        zeb_request_print (zeb_connection_request (connection));

    //  Start request processing
    if ((0 == strcmp (method, MHD_HTTP_METHOD_POST) ||
         0 == strcmp (method, MHD_HTTP_METHOD_PUT)) && *uploaded_data_size != 0) {
            //  Handle fragments of uploaded data
            zeb_request_t *request = zeb_connection_request (connection);
            char *data = (char *) zeb_request_data (zeb_connection_request (connection));
            if (!data) {
                data = (char *) zmalloc (*uploaded_data_size + 1);  //  +1 for \0
                assert (data);
                //  Copy data fragment to newly allocated data string
                char *data_p = strncat (data, uploaded_data, *uploaded_data_size);
                assert (data_p);
                zeb_request_set_data (request,  data, *uploaded_data_size);
            }
            else {
                data = (char *) realloc (data, zeb_request_data_size (request) * (*uploaded_data_size) + 1); //  +1 for \0
                assert (data);
                //  Copy data fragment to re-allocated data string
                char *data_p = strncat (data, uploaded_data, *uploaded_data_size);
                assert (data_p);
                zeb_request_set_data (request, data, zeb_request_data_size (request) + *uploaded_data_size);
            }
            *uploaded_data_size = 0;
            return MHD_YES;
    }
    xrap_msg_t *xrap_msg = s_build_xrap_message (self, connection);
    if (!xrap_msg)
        return s_send_static_response (con, "text/html",
                                       PAGE_METHOD_NOT_IMPLEMENTED, MHD_HTTP_NOT_IMPLEMENTED);

    //  Create client socket to dispatch request
    zeb_client_t *client = zeb_client_new ();
    assert (client);

    //  Connect client to server
    rc = zeb_client_connect (client, self->endpoints, 1000, "client");
    if (rc == 0) {  //  Interrupted!
        //  Send Request
        zmsg_t *request = xrap_msg_encode (&xrap_msg);
        assert (request);
        rc = zeb_client_request (client, 0, &request);

        //  Parse response
        if (rc == 0) {
            //  Receive Response
            zmsg_t *response = zeb_client_recv (client);
            xrap_msg_t *xresponse = xrap_msg_decode (&response);
            zeb_response_t *zeb_response = zeb_response_new (xresponse);
            //  Update rate limit
            zeb_request_t *zeb_request = zeb_connection_request (connection);
            char *user_agent =  (char *) zhash_lookup (zeb_request_header (zeb_request), "User-Agent");
            zeb_ratelimit_t *ratelimit = (zeb_ratelimit_t *)
                                         zhashx_lookup (self->ratelimit_clients, user_agent);
            if (!ratelimit)
                ratelimit = zeb_microhttpd_ratelimit_new (self, user_agent);

            if (XRAP_MSG_GET_EMPTY != xrap_msg_id (xresponse))
                ratelimit->remaining--;

            zeb_response_set_ratelimit (zeb_response,
                                        ratelimit->limit,
                                        ratelimit->remaining,
                                        ratelimit->reset);
            zeb_connection_set_response (connection, zeb_response);
            //  Cleanup before responding
            zuuid_t *sender = zeb_client_sender (client);
            zuuid_destroy (&sender);
            zeb_client_destroy (&client);
            return s_send_response (con, connection, self);
        }
        else
        if (rc == XRAP_TRAFFIC_NOT_FOUND) {
            zeb_client_destroy (&client);
            //  404 - not found
            return s_send_static_response (con, "text/html", PAGE_NOT_FOUND, MHD_HTTP_NOT_FOUND);
        }
    }
    else {
        xrap_msg_destroy (&xrap_msg);
    }

    zeb_client_destroy (&client);
    //  423 - Busy
    return s_send_static_response (con, "text/html", PAGE_BUSY, MHD_HTTP_SERVICE_UNAVAILABLE);
}


static void
s_request_completed (void *cls, struct MHD_Connection *con,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
    assert (con_cls);
    if (*con_cls) {
        zeb_connection_t *connection = (zeb_connection_t *) *con_cls;
        zeb_connection_destroy (&connection);
    }
    *con_cls = NULL;
}



//  Start this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
zeb_microhttpd_start (zeb_microhttpd_t *self)
{
    assert (self);
    self->daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION, self->port,
                                     &on_client_connect, self,
                                     &answer_to_connection, self,
                                     MHD_OPTION_NOTIFY_COMPLETED, &s_request_completed, self,
                                     MHD_OPTION_END);
    if (self->daemon)
        return 0;
    return -1;
}


//  Stop this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
zeb_microhttpd_stop (zeb_microhttpd_t *self)
{
    assert (self);
    MHD_stop_daemon (self->daemon);
    return 0;
}


//  Create and attach the dispatching endpoint

static int
s_configure_endpoint (zeb_microhttpd_t *self, zmsg_t *request)
{
    char *endpoints = zmsg_popstr (request);
    assert (endpoints);
    if (self->endpoints)
       free (self->endpoints);
    self->endpoints = endpoints;
    return 0;
}


static int
s_configure_port (zeb_microhttpd_t *self, zmsg_t *request)
{
    char *port_str = zmsg_popstr (request);
    assert (port_str);
    sscanf (port_str, "%d", &self->port);
    if (self->daemon) {
        zeb_microhttpd_stop (self);
        zeb_microhttpd_start (self);
    }
    zstr_free (&port_str);
    return 0;
}


static int
s_configure_ratelimit (zeb_microhttpd_t *self, zmsg_t *request)
{
    char *limit_str = zmsg_popstr (request);
    char *interval_str = zmsg_popstr (request);
    assert (limit_str);
    sscanf (limit_str, "%zu", &self->ratelimit_limit);
    assert (interval_str);
    sscanf (interval_str, "%zu", &self->ratelimit_interval);
    zloop_set_ticket_delay (self->reactor, self->ratelimit_interval);
    zstr_free (&limit_str);
    zstr_free (&interval_str);
    return 0;
}

static int
s_server_config_service (zeb_microhttpd_t *self)
{
    //  Apply echo commands and class methods
    zconfig_t *section = zconfig_locate (self->config, "zeb_microhttpd");
    if (section)
        section = zconfig_child (section);

    while (section) {
        if (streq (zconfig_name (section), "echo"))
            zsys_notice ("%s", zconfig_value (section));
        else
        if (streq (zconfig_name (section), "endpoint"))
            self->endpoints = strdup (zconfig_value (section));
        else
        if (streq (zconfig_name (section), "port"))
            self->port = atoi (zconfig_value (section));
        else
        //  If we didn't already set verbose, check if the config tree wants it
        if (streq (zconfig_name (section), "verbose") && atoi (zconfig_value (section)))
            self->verbose = true;
        else
        if (streq (zconfig_name (section), "ratelimit")) {
            self->ratelimit_limit = atoi (
                zconfig_get (section, "limit", "100"));
            self->ratelimit_interval = atoi (
                zconfig_get (section, "interval", "3600000"));
            zloop_set_ticket_delay (self->reactor, self->ratelimit_interval);
        }
        section = zconfig_next (section);
    }
    return 0;
}

//  Here we handle incomming message from the node

static int
zeb_microhttpd_recv_api (zloop_t *loop, zsock_t *pipe, void *arg)
{
    zeb_microhttpd_t *self = (zeb_microhttpd_t *) arg;
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return -1;        //  Interrupted

    int rc = 0;
    char *command = zmsg_popstr (request);
    if (streq (command, "START"))
        zsock_signal (self->pipe, zeb_microhttpd_start (self));
    else
    if (streq (command, "STOP"))
        zsock_signal (self->pipe, zeb_microhttpd_stop (self));
    else
    if (streq (command, "ENDPOINT"))
        zsock_signal (self->pipe, s_configure_endpoint (self, request));
    else
    if (streq (command, "PORT"))
        zsock_signal (self->pipe, s_configure_port (self, request));
    else
    if (streq (command, "RATELIMIT"))
        zsock_signal (self->pipe, s_configure_ratelimit (self, request));
    else
    if (streq (command, "VERBOSE")) {
        self->verbose = true;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "LOAD")) {
        char *filename = zmsg_popstr (request);
        zconfig_destroy (&self->config);
        self->config = zconfig_load (filename);
        if (self->config) {
            zsock_signal (self->pipe, s_server_config_service (self));
        }
        else {
            zsys_warning ("cannot load config file '%s'", filename);
            self->config = zconfig_new ("root", NULL);
        }
        free (filename);
    }
    else
    if (streq (command, "$TERM")) {
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
        zmsg_destroy (&request);
        rc = -1;      //  Tells zloop to return control
    }
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
    return rc;
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
zeb_microhttpd_actor (zsock_t *pipe, void *args)
{
    zeb_microhttpd_t * self = zeb_microhttpd_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    zloop_reader (self->reactor, self->pipe, zeb_microhttpd_recv_api, self);
    while (!self->terminated) {
        zloop_start (self->reactor);
    }

    zeb_microhttpd_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Self test of this actor.

void
zeb_microhttpd_test (bool verbose)
{
    printf (" * zeb_microhttpd: ");
    if (verbose)
        printf ("\n");

#if defined (HAVE_LIBCURL)
    int rc = 0;
    //  @selftest
    //  Start and configure http server and broker
    //  ------------------------------------------
    zactor_t *zeb_microhttpd = zactor_new (zeb_microhttpd_actor, NULL);

    zstr_send (zeb_microhttpd, "START");
    rc = zsock_wait (zeb_microhttpd);             //  Wait until actor started
    assert (rc == 0);

    zstr_sendx (zeb_microhttpd, "PORT", "8081", NULL);
    rc = zsock_wait (zeb_microhttpd);             //  Wait until port is configured
    assert (rc == 0);

    zstr_sendx (zeb_microhttpd, "RATELIMIT", "3", "10000", NULL);
    rc = zsock_wait (zeb_microhttpd);             //  Wait until port is configured
    assert (rc == 0);

    zstr_sendx (zeb_microhttpd, "ENDPOINT", "inproc://http_broker", NULL);
    rc = zsock_wait (zeb_microhttpd);             //  Wait until endpoint configured
    assert (rc == 0);

    zactor_t *broker = zactor_new (zeb_broker, "broker");

    if (verbose)
        zstr_send (broker, "VERBOSE");
    zstr_sendx (broker, "BIND", "inproc://http_broker", NULL);

    //  Create a XRAP hander and connect it to broker
    //  ---------------------------------------------
    zeb_client_t *handler = zeb_client_new ();
    assert (handler);

    //  Connect handler to server
    rc = zeb_client_connect (handler, "inproc://http_broker",  1000, "handler");
    assert (rc == 0);
    assert (zeb_client_connected (handler) == true);

    //  Provide GET Offering
    rc = zeb_client_set_handler (handler, "GET", "/foo/{[^/]}");
    assert (rc == 0);

    //  Simple GET example
    //  ------------------

    //  Send GET Request
    zeb_curl_client_t *curl = zeb_curl_client_new ();
    zeb_curl_client_send_get (curl, "http://localhost:8081/foo/bar?page=10");

    //  Receive Request
    zmsg_t *request = zeb_client_recv (handler);
    assert (request);
    xrap_msg_t *xrap_msg = xrap_msg_decode (&request);
    assert (xrap_msg_id (xrap_msg) == XRAP_MSG_GET);
    assert (streq ("/foo/bar", xrap_msg_resource (xrap_msg)));
    xrap_msg_destroy (&xrap_msg);

    //  Send Response
    xrap_msg = xrap_msg_new (XRAP_MSG_GET_OK);
    xrap_msg_set_status_code (xrap_msg, 200);
    xrap_msg_set_content_type (xrap_msg, "text/hello");
    xrap_msg_set_content_body (xrap_msg, "Hello World!");
    zmsg_t *response = xrap_msg_encode (&xrap_msg);
    zeb_client_deliver (handler, zeb_client_sender (handler), &response);
    zuuid_t *sender = zeb_client_sender (handler);
    zuuid_destroy (&sender);

    //  Receive GET Response
    zeb_curl_client_verify_response (curl, 200, "Hello World!");
    zeb_curl_client_destroy (&curl);

    //  Simple GET not found example
    //  ----------------------------

    //  Send GET Request 2
    curl = zeb_curl_client_new ();
    zeb_curl_client_send_get (curl, "http://localhost:8081/foo/bar/baz");

    //  Receive GET Response 2
    zeb_curl_client_verify_response (curl, 404, PAGE_NOT_FOUND);
    zeb_curl_client_destroy (&curl);

    //  Simple POST example
    //  -------------------

    //  Provide POST Offering
    rc = zeb_client_set_handler (handler, "POST", "/foo/{[^/]}");
    assert (rc == 0);

    curl = zeb_curl_client_new ();
    zeb_curl_client_send_post (curl, "http://localhost:8081/foo/bar", "abc");

    //  Receive Request
    request = zeb_client_recv (handler);
    assert (request);
    xrap_msg = xrap_msg_decode (&request);
    assert (xrap_msg_id (xrap_msg) == XRAP_MSG_POST);
    assert (streq (xrap_msg_content_type (xrap_msg), "text/plain"));
    assert (streq (xrap_msg_content_body (xrap_msg), "abc"));
    assert (streq ("/foo/bar", xrap_msg_parent (xrap_msg)));
    xrap_msg_destroy (&xrap_msg);

    //  Send Response
    xrap_msg = xrap_msg_new (XRAP_MSG_POST_OK);
    xrap_msg_set_status_code (xrap_msg, 201);
    xrap_msg_set_location (xrap_msg, "/foo/bar");
    xrap_msg_set_etag (xrap_msg, "a3fsd3");
    xrap_msg_set_date_modified (xrap_msg, 0);
    xrap_msg_set_content_type (xrap_msg, "text/hello");
    xrap_msg_set_content_body (xrap_msg, "Hello World!");
    response = xrap_msg_encode (&xrap_msg);
    zeb_client_deliver (handler, zeb_client_sender (handler), &response);
    sender = zeb_client_sender (handler);
    zuuid_destroy (&sender);

    //  Give response time to arrive
    zclock_sleep (250);

    zeb_curl_client_verify_response (curl, 201, "Hello World!");
    zeb_curl_client_destroy (&curl);

    //  Simple PUT example
    //  ------------------

    //  Provide PUT Offering
    rc = zeb_client_set_handler (handler, "PUT", "/foo/{[^/]}");
    assert (rc == 0);

    curl = zeb_curl_client_new ();
    zeb_curl_client_send_put (curl, "http://localhost:8081/foo/bar", "abc");

    //  Receive Request
    request = zeb_client_recv (handler);
    assert (request);
    xrap_msg = xrap_msg_decode (&request);
    assert (xrap_msg_id (xrap_msg) == XRAP_MSG_PUT);
    assert (streq (xrap_msg_content_type (xrap_msg), "text/plain"));
    assert (streq (xrap_msg_content_body (xrap_msg), "abc"));
    assert (streq ("/foo/bar", xrap_msg_resource (xrap_msg)));
    xrap_msg_destroy (&xrap_msg);

    //  Send Response
    xrap_msg = xrap_msg_new (XRAP_MSG_POST_OK);
    xrap_msg_set_status_code (xrap_msg, 200);
    xrap_msg_set_location (xrap_msg, "/foo/bar");
    xrap_msg_set_etag (xrap_msg, "a3fsd3");
    xrap_msg_set_date_modified (xrap_msg, 0);
    response = xrap_msg_encode (&xrap_msg);
    zeb_client_deliver (handler, zeb_client_sender (handler), &response);
    sender = zeb_client_sender (handler);
    zuuid_destroy (&sender);

    //  Give response time to arrive
    zclock_sleep (250);

    zeb_curl_client_verify_response (curl, 200, NULL);
    zeb_curl_client_destroy (&curl);

    //  Cleanup
    //  -------
    zeb_client_destroy (&handler);
    zactor_destroy (&broker);

    zstr_send (zeb_microhttpd, "STOP");
    rc = zsock_wait (zeb_microhttpd);             //  Wait until actor stopped
    assert (rc == 0);

    zactor_destroy (&zeb_microhttpd);
    //  @end
#endif
    printf ("OK\n");
}
