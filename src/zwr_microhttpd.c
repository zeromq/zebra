/*  =========================================================================
    zwr_microhttpd - HTTP web server backend using libmicrohttpd.

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
    zwr_microhttpd - HTTP web server backend using libmicrohttpd.
@discuss
@end
*/

#include <microhttpd.h>
#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

#define PORT 8888

#define errorpage "<html><body>This doesn't seem to be right.</body></html>"

//  Structure of our class

struct _zwr_microhttpd_t {
    struct timeval *start_time;
    struct MHD_Daemon *daemon;
    zwr_microhttpd_handler_fn *get_handler;
    zwr_microhttpd_handler_fn *post_handler;
    zwr_microhttpd_handler_fn *put_handler;
    zwr_microhttpd_handler_fn *delete_handler;
};


//  --------------------------------------------------------------------------
//  Create a new zwr_microhttpd.

zwr_microhttpd_t *
zwr_microhttpd_new (zwr_microhttpd_handler_fn *get_handler,
                    zwr_microhttpd_handler_fn *post_handler,
                    zwr_microhttpd_handler_fn *put_handler,
                    zwr_microhttpd_handler_fn *delete_handler)
{
    //  At least one handler must be registered
    assert (get_handler || post_handler || put_handler || delete_handler);

    zwr_microhttpd_t *self = (zwr_microhttpd_t *) zmalloc (sizeof (zwr_microhttpd_t));
    assert (self);

    //  Initialize properties
    self->start_time = (struct timeval *) zmalloc (sizeof (struct timeval));
    gettimeofday(self->start_time, 0);
    self->daemon = NULL;
    self->get_handler = get_handler;
    self->post_handler = post_handler;
    self->put_handler = put_handler;
    self->delete_handler = delete_handler;

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zwr_microhttpd.

void
zwr_microhttpd_destroy (zwr_microhttpd_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_microhttpd_t *self = *self_p;

        //  Free class properties
        free (self->start_time);

        //  Free object itself
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
s_send_response (struct MHD_Connection *con, zwr_response_t *response_wrapper)
{
    int ret = 0;
    const char *data = zwr_response_data (response_wrapper);
    const char *mime_type = zwr_response_mime_type (response_wrapper);
    int status_code = zwr_response_status_code (response_wrapper);

    struct MHD_Response *response;
    response = MHD_create_response_from_buffer (strlen (data) , (void *) data, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE, mime_type);
    if (!response)
        return MHD_NO;
    ret = MHD_queue_response (con, status_code, response);
    printf ("Send Page: %s, %d\n", mime_type, status_code);
    MHD_destroy_response (response);
    return ret;
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

        if (0 == strcmp (method, MHD_HTTP_METHOD_POST) || 0 == strcmp (method, MHD_HTTP_METHOD_PUT)) {
            char *content_type = (char *) zhashx_lookup (zwr_request_header (request) , "Content-Type");
            if (strcmp ("application/xml", content_type)) {
                //  POST and PUT data is provided on second call to this method,
                //  thus return MHD_YES which indicates that the request is not yet finished.
                return MHD_YES;
            }
        }
    }
    //  Get connection for an already running request
    else {
        connection = (zwr_connection_t *) *con_cls;
    }
    zwr_request_print (zwr_connection_request (connection));

    //  Start request processing
    if (0 == strcmp (method, MHD_HTTP_METHOD_GET)) {
        zwr_connection_set_response (connection, (self->get_handler) (zwr_connection_request (connection)));
        zwr_response_t *response = zwr_connection_response (connection);
        return s_send_response (con, response);
    }
    else
    if (0 == strcmp (method, MHD_HTTP_METHOD_POST)) {
        if (*uploaded_data_size) {
            zwr_request_set_data (zwr_connection_request (connection),  uploaded_data, *uploaded_data_size);
            zwr_connection_set_response (connection,
                                        (self->post_handler) (zwr_connection_request (connection)));
            *uploaded_data_size = 0;
            return MHD_YES;
        }
        else {
            zwr_response_t *response = zwr_connection_response (connection);
            return s_send_response (con, response);
        }
    }
    else
    if (0 == strcmp (method, MHD_HTTP_METHOD_PUT)) {
        if (*uploaded_data_size) {
            zwr_request_set_data (zwr_connection_request (connection),  uploaded_data, *uploaded_data_size);
            zwr_connection_set_response (connection,
                                         (self->put_handler) (zwr_connection_request (connection)));
            *uploaded_data_size = 0;
            return MHD_YES;
        }
        else {
            zwr_response_t *response = zwr_connection_response (connection);
            return s_send_response (con, response);
        }
    }
    else
    if (0 == strcmp (method, MHD_HTTP_METHOD_DELETE)) {
        zwr_connection_set_response (connection,
                                    (self->delete_handler) (zwr_connection_request (connection)));
        zwr_response_t *response = zwr_connection_response (connection);
        return s_send_response (con, response);
    }

    zwr_response_t *response = zwr_response_new (errorpage, "text/html", MHD_HTTP_BAD_REQUEST, MODE_DUP);
    return s_send_response (con, response);
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


//  --------------------------------------------------------------------------
//  Start the zwr_microhttpd server. Returns 0 if successful otherwise -1.

int
zwr_microhttpd_start (zwr_microhttpd_t *self)
{
    assert (self);
    self->daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT,
                                     &on_client_connect, self,
                                     &answer_to_connection, self,
                                     MHD_OPTION_NOTIFY_COMPLETED, &request_completed, self,
                                     MHD_OPTION_END);
    if (self->daemon)
        return 0;
    return -1;
}

//  Stops the zwr_microhttpd server. Returns 0 if successful otherwise -1.
int
zwr_microhttpd_stop (zwr_microhttpd_t *self)
{
    assert (self);
    MHD_stop_daemon (self->daemon);
    return 0;
}

//  --------------------------------------------------------------------------
//  Print properties of the zwr_microhttpd object.

void
zwr_microhttpd_print (zwr_microhttpd_t *self)
{
    assert (self);
}

static zwr_response_t *
s_selftest_dummy_handler (zwr_request_t *request)
{
    return NULL;
}

//  --------------------------------------------------------------------------
//  Self test of this class.

void
zwr_microhttpd_test (bool verbose)
{
    printf (" * zwr_microhttpd: ");

    //  @selftest
    //  Simple create/destroy test
    zwr_microhttpd_t *self = zwr_microhttpd_new (s_selftest_dummy_handler, NULL, NULL, NULL);
    assert (self);
    zwr_microhttpd_start (self);
    zwr_microhttpd_stop (self);
    zwr_microhttpd_destroy (&self);
    //  @end

    printf ("OK\n");
}
