/*  =========================================================================
    zeb_broker - zebra service broker

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZEBRA.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The zeb_broker implements the zproto server. This broker connects
    client requests to handler offers.
@discuss
@end
*/

#include "zebra_classes.h"

//  ---------------------------------------------------------------------------
//  Forward declarations for the two main classes we use here

typedef struct _server_t server_t;
typedef struct _client_t client_t;

//  Structure of our class

//  ---------------------------------------------------------------------------
//  Services are lists of messages and workers

typedef struct {
    ztrie_t *get_routes;        //  Registered GET routes
    ztrie_t *post_routes;       //  Registered POST routes
    ztrie_t *put_routes;        //  Registered PUT routes
    ztrie_t *delete_routes;     //  Registered DELETE routes
    zlistx_t *queue;            //  Pending messages
} service_t;

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zsock_t *pipe;              //  Actor pipe back to caller
    zconfig_t *config;          //  Current loaded configuration
    service_t *xrap;            //  Holds registered routes and pending messages
    zhashx_t *clients;          //  Holds clients by address
};

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    xrap_traffic_t *message;    //  Message in and out

    //  These properties are specific for this application
    zuuid_t *id;                //  Address of this client
    int rc;                     //  Return code

    //  Hold currently dispatching service message here
    zmsg_t *msg;            //  Message structure
    client_t * callee;      //  Message source
};

#include "zeb_broker_engine.inc"

//  Work with service instance
//
static void
s_service_destroy (service_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        service_t *self = *self_p;
        ztrie_destroy (&self->get_routes);
        ztrie_destroy (&self->post_routes);
        ztrie_destroy (&self->put_routes);
        ztrie_destroy (&self->delete_routes);
        zlistx_destroy (&self->queue);
        free (self);
        *self_p = NULL;
    }
}

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    self->xrap = (service_t *) zmalloc (sizeof (service_t));
    assert (self->xrap);
    self->xrap->get_routes = ztrie_new ('/');
    self->xrap->post_routes = ztrie_new ('/');
    self->xrap->put_routes = ztrie_new ('/');
    self->xrap->delete_routes = ztrie_new ('/');
    self->clients = zhashx_new ();
    assert (self->clients);
    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    s_service_destroy (&self->xrap);
    zhashx_destroy (&self->clients);
}

//  Process server API method, return reply message if any

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    return NULL;
}



//  Allocate properties and structures for a new client connection and
//  optionally engine_set_next_event (). Return 0 if OK, or -1 on error.

static int
client_initialize (client_t *self)
{
    return 0;
}

//  Free properties and structures for a client connection

static void
client_terminate (client_t *self)
{
    zuuid_destroy (&self->id);
}

//  ---------------------------------------------------------------------------
//  register_new_client
//

static void
register_new_client (client_t *self)
{
    self->id = zuuid_new ();
    zhashx_update (self->server->clients, zuuid_str (self->id), self);
    xrap_traffic_set_status_code (self->message, XRAP_TRAFFIC_SUCCESS);
}


//  ---------------------------------------------------------------------------
//  deregister_the_client
//

static void
deregister_the_client (client_t *self)
{
    zhashx_delete (self->server->clients, zuuid_str (self->id));
}


//  ---------------------------------------------------------------------------
//  signal_command_invalid
//

static void
signal_command_invalid (client_t *self)
{
    xrap_traffic_set_status_code (self->message, XRAP_TRAFFIC_COMMAND_INVALID);
}


//  ---------------------------------------------------------------------------
//  write_message_to_xrap_client
//

static void
write_message_to_xrap_client (client_t *self)
{
    zuuid_t *client_id = xrap_traffic_sender (self->message);
    assert (client_id);
    client_t *client = (client_t *) zhashx_lookup (self->server->clients, zuuid_str (client_id));
    if (client) {
        //  Save message for broker
        client->msg = zmsg_dup (xrap_traffic_content (self->message));
        client->callee = NULL;
        engine_send_event (client, xrap_message_event);
    }
}

//  ---------------------------------------------------------------------------
//  write_message_to_xrap_handler
//

static void
write_message_to_xrap_handler (client_t *self)
{
    service_t *service = self->server->xrap;
    assert (service);
    ztrie_t *routes = NULL;

    zmsg_t *content = zmsg_dup (xrap_traffic_content (self->message));
    xrap_msg_t *msg = xrap_msg_decode (&content);
    char *route;
    int method = xrap_msg_id (msg);
    if (method == XRAP_MSG_GET) {
        route = (char *) xrap_msg_resource (msg);
        routes = service->get_routes;
    }
    else
    if (method == XRAP_MSG_POST) {
        route = (char *) xrap_msg_parent (msg);
        routes = service->post_routes;
    }
    else
    if (method == XRAP_MSG_PUT) {
        route = (char *) xrap_msg_resource (msg);
        routes = service->put_routes;
    }
    else
    if (method == XRAP_MSG_DELETE) {
        route = (char *) xrap_msg_resource (msg);
        routes = service->delete_routes;
    }
    else {
        xrap_traffic_set_status_code (self->message, XRAP_TRAFFIC_BAD_REQUEST);
        engine_set_exception (self, fail_event);
    }

    client_t *target;
    if (routes && ztrie_matches (routes, route)) {
        target = (client_t *) ztrie_hit_data (routes);
        //  Save message for broker
        target->msg = zmsg_dup (xrap_traffic_content (self->message));
        //  Trigger dispatch event
        target->callee = self;
        engine_send_event (target, xrap_message_event);
    }
    else {
        xrap_traffic_set_status_code (self->message, XRAP_TRAFFIC_NOT_FOUND);
        self->rc = -1;
    }

    //  Clean up
    xrap_msg_destroy (&msg);
}


//  ---------------------------------------------------------------------------
//  store_xrap_offer
//

static void
store_xrap_offer (client_t *self)
{
    service_t *service = self->server->xrap;
    assert (service);
    char *route = (char *) xrap_traffic_route (self->message);
    char *method = (char *) xrap_traffic_method (self->message);
    if (streq (method, "GET"))
        self->rc = ztrie_insert_route (service->get_routes, route, self, NULL);
    else
    if (streq (method, "POST"))
        self->rc = ztrie_insert_route (service->post_routes, route, self, NULL);
    else
    if (streq (method, "PUT"))
        self->rc = ztrie_insert_route (service->put_routes, route, self, NULL);
    else
    if (streq (method, "DELETE"))
        self->rc = ztrie_insert_route (service->delete_routes, route, self, NULL);

    if (self->rc != 0)
        xrap_traffic_set_status_code (self->message, XRAP_TRAFFIC_CONFLICT);
}


//  ---------------------------------------------------------------------------
//  send_response
//

static void
send_response (client_t *self)
{
    if (self->rc == 0)
        engine_set_next_event (self, ok_event);
    else
        engine_set_next_event (self, fail_event);
    self->rc = 0;
}


//  ---------------------------------------------------------------------------
//  dispatch_the_xrap_request
//

static void
dispatch_the_xrap_request (client_t *self)
{
}


//  ---------------------------------------------------------------------------
//  allow_time_to_settle
//

static void
allow_time_to_settle (client_t *self)
{
    //  We are still using hard pointers rather than cycled client IDs, so
    //  there may be messages pending from a stream which refer to our client.
    //  Stupid strategy for now is to give the client thread a while to process
    //  these, before killing it.
    engine_set_wakeup_event (self, 1000, settled_event);
}


//  ---------------------------------------------------------------------------
//  get_message_to_deliver
//

static void
get_message_to_deliver (client_t *self)
{
    assert (zmsg_is (self->msg));
    if (self->callee)
        xrap_traffic_set_sender (self->message, self->callee->id);
    xrap_traffic_set_content (self->message, &self->msg);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zeb_broker_test (bool verbose)
{
    printf (" * zeb_broker: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zactor_t *server = zactor_new (zeb_broker, "broker");
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "BIND", "tcp://127.0.0.1:9999", NULL);

    zsock_t* client = zsock_new_dealer (">tcp://127.0.0.1:9999");
    zsock_t* worker = zsock_new_dealer (">tcp://127.0.0.1:9999");
    assert (client);
    assert (worker);

    xrap_traffic_t *traffic = xrap_traffic_new ();

    //  Invalid Command
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_XRAP_OFFER);
    xrap_traffic_send (traffic, client);
    xrap_traffic_recv (traffic, client);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_ERROR);
    assert (xrap_traffic_status_code (traffic) == XRAP_TRAFFIC_COMMAND_INVALID);

    //  Open Connections for client & worker
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_CONNECTION_OPEN);
    xrap_traffic_send (traffic, client);
    xrap_traffic_recv (traffic, client);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_OK);

    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_CONNECTION_OPEN);
    xrap_traffic_send (traffic, worker);
    xrap_traffic_recv (traffic, worker);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_OK);

    //  Provide Rubish Offering
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_XRAP_OFFER);
    xrap_traffic_set_route (traffic, "///");
    xrap_traffic_set_method (traffic, "GET");
    xrap_traffic_send (traffic, worker);
    xrap_traffic_recv (traffic, worker);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_FAIL);
    assert (xrap_traffic_status_code (traffic) == XRAP_TRAFFIC_CONFLICT);

    //  Provide Offering
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_XRAP_OFFER);
    xrap_traffic_set_route (traffic, "/foo/{[^/]}");
    xrap_traffic_set_method (traffic, "GET");
    xrap_traffic_send (traffic, worker);
    xrap_traffic_recv (traffic, worker);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_OK);

    //  Send Request
    xrap_msg_t *xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/foo/bar");
    zmsg_t *msg = xrap_msg_encode (&xmsg);
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_XRAP_SEND);
    xrap_traffic_set_content (traffic, &msg);
    xrap_traffic_send (traffic, client);
    xrap_traffic_recv (traffic, client);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_OK);

    //  Receive Request
    xrap_traffic_recv (traffic, worker);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_XRAP_DELIVER);
    msg = zmsg_dup (xrap_traffic_content (traffic));
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_GET);
    assert (streq ("/foo/bar", xrap_msg_resource (xmsg)));
    xrap_msg_destroy (&xmsg);

    //  Send Response
    xmsg = xrap_msg_new (XRAP_MSG_GET_OK);
    xrap_msg_set_status_code (xmsg, 200);
    xrap_msg_set_content_type (xmsg, "text/hello");
    xrap_msg_set_content_body (xmsg, "Hello World!");
    msg = xrap_msg_encode (&xmsg);
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_XRAP_DELIVER);
    xrap_traffic_set_content (traffic, &msg);
    xrap_traffic_send (traffic, worker);

    //  Receive Response
    xrap_traffic_recv (traffic, client);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_XRAP_DELIVER);
    msg = zmsg_dup (xrap_traffic_content (traffic));
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_GET_OK);
    assert (xrap_msg_status_code (xmsg) == 200);
    assert (streq ("text/hello", xrap_msg_content_type (xmsg)));
    assert (streq ("Hello World!", xrap_msg_content_body (xmsg)));
    xrap_msg_destroy (&xmsg);

    //  Send Request 2
    xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/fou/baz");
    msg = xrap_msg_encode (&xmsg);
    xrap_traffic_set_id (traffic, XRAP_TRAFFIC_XRAP_SEND);
    xrap_traffic_set_content (traffic, &msg);
    xrap_traffic_send (traffic, client);
    xrap_traffic_recv (traffic, client);
    assert (xrap_traffic_id (traffic) == XRAP_TRAFFIC_FAIL);
    assert (xrap_traffic_status_code (traffic) == XRAP_TRAFFIC_NOT_FOUND);

    xrap_traffic_destroy (&traffic);

    //  Finished, we can clean up
    zsock_destroy (&client);
    zsock_destroy (&worker);
    zactor_destroy (&server);
    //  @end
    printf ("OK\n");
}

