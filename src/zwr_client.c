/*  =========================================================================
    zwr_client - Dispatcher client

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZWEBRAP.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    Client implementation to communicate with the dispatcher. This
    implementation is used by both clients (i.e. zwr_microhttpd) and the
    handlers.
@discuss
@end
*/

#include "zwebrap_classes.h"

//  Forward reference to method arguments structure
typedef struct _client_args_t client_args_t;

//  This structure defines the context for a client connection
typedef struct {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine. The cmdpipe gets
    //  messages sent to the actor; the msgpipe may be used for
    //  faster asynchronous message flows.
    zsock_t *cmdpipe;           //  Command pipe to/from caller API
    zsock_t *msgpipe;           //  Message pipe to/from caller API
    zsock_t *dealer;            //  Socket to talk to server
    xrap_traffic_t *message;    //  Message to/from server
    client_args_t *args;        //  Arguments from methods

    //  Add specific properties for your application
    int heartbeat_timer;        //  Timeout for heartbeats to server
    zlistx_t *replays;          //  Replay server-side state set-up
} client_t;

//  Include the generated client engine
#include "zwr_client_engine.inc"

//  Work with server-side state replay
typedef struct {
    char *name;                 //  Replay command
    char *method;               //  Xrap method
    char *route;                //  Xrap route
} replay_t;

static replay_t *
s_replay_new (const char *name, const char *method, const char *route)
{
    replay_t *self = (replay_t *) zmalloc (sizeof (replay_t));
    if (self) {
        self->name = strdup (name);
        self->method = strdup (method);
        self->route = strdup (route);
    }
    return self;
}

static void
s_replay_destroy (replay_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        replay_t *self = *self_p;
        free (self->name);
        free (self->method);
        free (self->route);
        free (self);
        *self_p = NULL;
    }
}


static void
s_replay_execute (client_t *self, replay_t *replay)
{
    if (replay) {
        if (streq (replay->name, "XRAP OFFER")) {
            engine_set_next_event (self, set_handler_event);
            xrap_traffic_set_method (self->message, replay->method);
            xrap_traffic_set_route (self->message, replay->route);
        }
    }
    else
        engine_set_next_event (self, replay_ready_event);
}

//  Allocate properties and structures for a new client instance.
//  Return 0 if OK, -1 if failed

static int
client_initialize (client_t *self)
{
    //  We'll ping the server once every two seconds
    self->heartbeat_timer = 2500;
    self->replays = zlistx_new ();
    zlistx_set_destructor (self->replays, (czmq_destructor *) s_replay_destroy);
    return 0;
}

//  Free properties and structures for a client instance

static void
client_terminate (client_t *self)
{
    //  Destroy properties here
    zlistx_destroy (&self->replays);
}


//  ---------------------------------------------------------------------------
//  connect_to_server_endpoint
//

static void
connect_to_server_endpoint (client_t *self)
{
    if (zsock_connect (self->dealer, "%s", self->args->endpoint)) {
        engine_set_exception (self, bad_endpoint_event);
        zsys_warning ("could not connect to %s", self->args->endpoint);
    }
}


//  ---------------------------------------------------------------------------
//  set_client_address
//

static void
set_client_address (client_t *self)
{
    xrap_traffic_set_address (self->message, self->args->address);
}


//  ---------------------------------------------------------------------------
//  client_is_connected
//

static void
client_is_connected (client_t *self)
{
    engine_set_connected (self, true);
    //  We send a PING to the server on every heartbeat
    engine_set_heartbeat (self, self->heartbeat_timer);
    //  We get an expired event if server sends nothing within 3 heartbeats
    engine_set_expiry (self, self->heartbeat_timer * 4);
}


//  ---------------------------------------------------------------------------
//  server_has_gone_offline
//

static void
server_has_gone_offline (client_t *self)
{
    engine_set_connected (self, false);
}


//  ---------------------------------------------------------------------------
//  check_status_code
//

static void
check_status_code (client_t *self)
{
    if (xrap_traffic_status_code (self->message) == XRAP_TRAFFIC_COMMAND_INVALID)
        engine_set_next_event (self, command_invalid_event);
    else
        engine_set_next_event (self, other_event);
}


//  ---------------------------------------------------------------------------
//  use_connect_timeout
//

static void
use_connect_timeout (client_t *self)
{
    engine_set_expiry (self, self->args->timeout);
}


//  ---------------------------------------------------------------------------
//  signal_bad_endpoint
//

static void
signal_bad_endpoint (client_t *self)
{
    zsock_send (self->cmdpipe, "sis", "FAILURE", -1, "Syntax error in server endpoint");
}


//  ---------------------------------------------------------------------------
//  signal_failure
//

static void
signal_failure (client_t *self)
{
    xrap_traffic_status_code (self->message);
    zsock_send (self->cmdpipe, "sis", "FAILURE",
                xrap_traffic_status_code (self->message),
                xrap_traffic_status_reason (self->message));
}


//  ---------------------------------------------------------------------------
//  signal_server_not_present
//

static void
signal_server_not_present (client_t *self)
{
    zsock_send (self->cmdpipe, "sis", "FAILURE", -1, "Server is not reachable");
}


//  ---------------------------------------------------------------------------
//  signal_success
//

static void
signal_success (client_t *self)
{
    zsock_send (self->cmdpipe, "si", "SUCCESS", 0);
}


//  ---------------------------------------------------------------------------
//  signal_unhandled_error
//

static void
signal_unhandled_error (client_t *self)
{
    zsys_error ("unhandled error code from Malamute server");
}


//  ---------------------------------------------------------------------------
//  pass_xrap_message_to_app
//

static void
pass_xrap_message_to_app (client_t *self)
{
    zstr_sendm (self->msgpipe, "XRAP DELIVER");
    zsock_bsend (self->msgpipe, "up",
                 xrap_traffic_sender (self->message),
                 xrap_traffic_get_content (self->message));
}


//  ---------------------------------------------------------------------------
//  prepare_xrap_offer_command
//

static void
prepare_xrap_offer_command (client_t *self)
{
    zlistx_add_end (self->replays,
        s_replay_new ("XRAP OFFER", self->args->method, self->args->route));
    xrap_traffic_set_method (self->message, self->args->method);
    xrap_traffic_set_route (self->message, self->args->route);
}


//  ---------------------------------------------------------------------------
//  prepare_xrap_send_command
//

static void
prepare_xrap_send_command (client_t *self)
{
    xrap_traffic_set_timeout (self->message, self->args->timeout);
    xrap_traffic_set_content (self->message, &self->args->content);
}


//  ---------------------------------------------------------------------------
//  get_first_replay_command
//

static void
get_first_replay_command (client_t *self)
{
    replay_t *replay = (replay_t *) zlistx_first (self->replays);
    s_replay_execute (self, replay);
}


//  ---------------------------------------------------------------------------
//  get_next_replay_command
//

static void
get_next_replay_command (client_t *self)
{
    replay_t *replay = (replay_t *) zlistx_next (self->replays);
    s_replay_execute (self, replay);
}

//  ---------------------------------------------------------------------------
//  Selftest

void
zwr_client_test (bool verbose)
{
    printf (" * zwr_client: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zwr_client_verbose = verbose;

    //  Start a server to test against, and bind to endpoint
    zactor_t *server = zactor_new (zwr_server, "zwr_client_test");
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "LOAD", "src/zwr_client.cfg", NULL);

    //  Create clients for testing
    zwr_client_t *client = zwr_client_new ();
    zwr_client_t *handler = zwr_client_new ();
    assert (client);
    assert (handler);

    //  Connect clients to server
    int rc = zwr_client_connect (client, "tcp://127.0.0.1:9999", 1000, "client");
    assert (rc == 0);
    assert (zwr_client_connected (client) == true);

    rc = zwr_client_connect (handler, "tcp://127.0.0.1:9999", 1000, "handler");
    assert (rc == 0);
    assert (zwr_client_connected (handler) == true);

    //  Provide Rubbish Offering
    rc = zwr_client_set_handler (handler, "GET", "///");
    assert (rc != 0);
    assert (zwr_client_status (handler) == XRAP_TRAFFIC_CONFLICT);

    //  Provide Offering
    rc = zwr_client_set_handler (handler, "GET", "/foo/{[^/]}");
    assert (rc == 0);

    //  Send Request
    xrap_msg_t *xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/foo/bar");
    zmsg_t *msg = xrap_msg_encode (&xmsg);
    rc = zwr_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Receive Request
    msg = zwr_client_recv (handler);
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
    zwr_client_deliver (handler, handler->sender, &msg);
    zuuid_t *sender = zwr_client_sender (handler);
    zuuid_destroy (&sender);

    //  Receive Response
    msg = zwr_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_GET_OK);
    assert (xrap_msg_status_code (xmsg) == 200);
    assert (streq ("text/hello", xrap_msg_content_type (xmsg)));
    assert (streq ("Hello World!", xrap_msg_content_body (xmsg)));
    xrap_msg_destroy (&xmsg);
    sender = zwr_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request 2
    xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/fou/baz");
    msg = xrap_msg_encode (&xmsg);
    rc = zwr_client_request (client, 0, &msg);
    assert (rc == XRAP_TRAFFIC_NOT_FOUND);

    zwr_client_destroy (&client);
    zwr_client_destroy (&handler);

    //  Done, shut down
    zactor_destroy (&server);
    //  @end
    printf ("OK\n");
}
