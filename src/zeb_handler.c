/*  =========================================================================
    zeb_handler - Handler for XRAP requests

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZWEBRAP.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zeb_handler - Handler for XRAP requests
@discuss
@end
*/

#include "zebra_classes.h"

//  Structure of the actor

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    bool terminated;
    bool verbose;
    //  Declare properties
    zeb_client_t *client;       //  Client that communicates with the zeb_broker
    ztrie_t *offers;            //  Holds the offers the handler handles
    zlistx_t *accepts;          //  Response formats this handler can deliver
} s_handler_t;

//  Local helper functions

static char *
s_xrap_command (int id)
{
    switch (id) {
        case XRAP_MSG_GET:
            return ("/GET");
            break;
        case XRAP_MSG_POST:
            return ("/POST");
            break;
        case XRAP_MSG_PUT:
            return ("/PUT");
            break;
        case XRAP_MSG_DELETE:
            return ("/DELETE");
            break;
        default:
            return ("");
    }
}

static char *
s_concat (int method, const char *s2)
{
    char *s1 = s_xrap_command (method);
    size_t len1 = strlen (s1);
    size_t len2 = strlen (s2);
    char *result = (char *) malloc (len1 + len2 + 1);    //+1 for the zero-terminator
    assert (result);
    memcpy (result, s1, len1);
    memcpy (result + len1, s2, len2 + 1);       //+1 to copy the null-terminator
    return result;
}

static void
s_destroy_accept_item (void **item_p);

//  --------------------------------------------------------------------------
//  Create a new zeb_handler.

static s_handler_t *
s_handler_new (zsock_t *pipe, char *endpoint)
{
    s_handler_t *self = (s_handler_t *) zmalloc (sizeof (s_handler_t));
    assert (self);

    self->pipe = pipe;
    self->poller = zpoller_new (self->pipe, NULL);
    self->terminated = false;
    self->verbose = false;
    //  Initialize properties
    self->client = zeb_client_new ();
    int rc = zeb_client_connect (self->client, endpoint, 1000, "zeb_handler");
    assert (rc == 0);
    self->offers = ztrie_new ('/');
    self->accepts = zlistx_new ();
    zlistx_set_destructor (self->accepts, s_destroy_accept_item);

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zeb_handler.

static void
s_destroy_accept_item (void **item_p)
{
    assert (item_p);
    if (*item_p) {
        zrex_t *self = (zrex_t *) *item_p;
        zrex_destroy (&self);
    }
}

static void
s_handler_destroy (s_handler_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        s_handler_t *self = *self_p;

        zpoller_destroy (&self->poller);
        // Free class properties
        zeb_client_destroy (&self->client);
        ztrie_destroy (&self->offers);
        zlistx_destroy (&self->accepts);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Add a new offer this handler will handle. Returns 0 if successfull,
//  otherwise 1;

int
s_handler_add_offer (s_handler_t *self)
{
    assert (self);

    uint8_t method;
    char *uri = NULL;
    zsock_brecv (self->pipe, "1s", &method, &uri);
    assert (uri);

    char *joined_uri = s_concat (method, uri);
    int rc = ztrie_insert_route (self->offers, joined_uri, NULL, NULL);
    zstr_free (&joined_uri);

    if (rc == 0)
        rc = zeb_client_set_handler (self->client, s_xrap_command (method) + 1, uri);
    return rc == -1? 1: rc;
}


//  --------------------------------------------------------------------------
//  Add a new accept type that this handler can deliver. May be a regular
//  expression.

int
s_handler_add_accept (s_handler_t *self)
{
    assert (self);

    char *accept = zstr_recv (self->pipe);
    assert (accept);
    zrex_t *rex = zrex_new (accept);
    assert (rex);

    zstr_free (&accept);

    //  Invalid expression
    if (!zrex_valid (rex))
        return 1;

    zlistx_add_end (self->accepts, rex);
    return 0;
}


//  --------------------------------------------------------------------------
//  Deliver an XRAP response back to the caller.

static void
s_handler_recv_client (s_handler_t *self)
{
    assert (self);
    zmsg_t *request = zeb_client_recv (self->client);
    xrap_msg_t *xrequest = xrap_msg_decode (&request);

    //  Check if method is valid
    int xrap_id = xrap_msg_id (xrequest);
    if (! (xrap_id == XRAP_MSG_POST || xrap_id == XRAP_MSG_GET ||
           xrap_id == XRAP_MSG_PUT || xrap_id == XRAP_MSG_DELETE)) {
        xrap_msg_t *xresponse = xrap_msg_new (XRAP_MSG_ERROR);
        xrap_msg_set_status_code (xresponse, XRAP_TRAFFIC_METHOD_NOT_ALLOWED);
        xrap_msg_set_status_text (xresponse, "Valid request methods are POST, GET, PUT and DELETE.");
        zmsg_t *response = xrap_msg_encode (&xresponse);
        zeb_client_deliver (self->client, zeb_client_sender (self->client), &response);
        goto cleanup;
    }

    {   // Jump scoping needed for c++
        //  Check if content type is valid
        bool matching_accept = false;
        const char *content_type = xrap_msg_content_type (xrequest);
        zrex_t *accept_expr = (zrex_t *) zlistx_first (self->accepts);
        while (accept_expr) {
            if (zrex_matches (accept_expr, content_type))
                matching_accept = true;
            accept_expr = (zrex_t *) zlistx_next (self->accepts);
        }
        if (!matching_accept) {
            xrap_msg_t *xresponse = xrap_msg_new (XRAP_MSG_ERROR);
            xrap_msg_set_status_code (xresponse, XRAP_TRAFFIC_NOT_ACCEPTABLE);
            xrap_msg_set_status_text (xresponse, "Ressource accept format is not valid!");
            zmsg_t *response = xrap_msg_encode (&xresponse);
            zeb_client_deliver (self->client, zeb_client_sender (self->client), &response);
            goto cleanup;
        }
    }

    {   // Jump scoping needed for c++

        //  Check if request is conditional
        if (xrap_msg_id (xrequest) == XRAP_MSG_GET) {
            const char *etag = xrap_msg_if_none_match (xrequest);
            uint64_t last_modified = xrap_msg_if_modified_since (xrequest);
            //  If neither is provided request is not conditional
            if ((etag && strneq (etag, "")) || last_modified > 0) {
                bool etag_matches = true;
                bool last_modified_matches = true;
                //  Check etag and last modified
                if (etag && strneq (etag, "")) {
                    zsock_send (self->pipe, "ss", "CHECK ETAG", etag);
                    etag_matches = zsock_wait (self->pipe) == 0;
                }
                if (last_modified > 0) {
                    zsock_send (self->pipe, "s8", "CHECK LAST MODIFIED", last_modified);
                    last_modified_matches = zsock_wait (self->pipe) == 0;
                }
                //  If etag and last modified matches send GET_EMPTY
                if (etag_matches && last_modified_matches) {
                    xrap_msg_t *xresponse = xrap_msg_new (XRAP_MSG_GET_EMPTY);
                    xrap_msg_set_status_code (xresponse, 304);
                    zmsg_t *response = xrap_msg_encode (&xresponse);
                    zeb_client_deliver (self->client, zeb_client_sender (self->client), &response);
                    goto cleanup;
                }
            }
        }
        else
        if (xrap_msg_id (xrequest) == XRAP_MSG_PUT || xrap_msg_id (xrequest) == XRAP_MSG_DELETE) {
            const char *etag = xrap_msg_if_match (xrequest);
            uint64_t last_modified = xrap_msg_if_unmodified_since (xrequest);
            //  If neither is provided request is not conditional
            if ((etag && strneq (etag, "")) || last_modified > 0) {
                bool etag_matches = true;
                bool last_modified_matches = true;
                //  Check etag and last modified
                if (etag && strneq (etag, "")) {
                    zsock_send (self->pipe, "ss", "CHECK ETAG", etag);
                    etag_matches = zsock_wait (self->pipe) == 0;
                }
                if (last_modified > 0) {
                    zsock_send (self->pipe, "s8", "CHECK LAST MODIFIED", last_modified);
                    last_modified_matches = zsock_wait (self->pipe) == 0;
                }
                //  If either etag or last modified mismatches send precondition failed
                if (! (etag_matches && last_modified_matches)) {
                    xrap_msg_t *xresponse = xrap_msg_new (XRAP_MSG_ERROR);
                    xrap_msg_set_status_code (xresponse, XRAP_TRAFFIC_PRECONDITION_FAILED);
                    xrap_msg_set_status_text (xresponse, "Resource has been modified by someone else.");
                    zmsg_t *response = xrap_msg_encode (&xresponse);
                    zeb_client_deliver (self->client, zeb_client_sender (self->client), &response);
                    goto cleanup;
                }
            }
        }

        //  Dispatch request to handle_request_fn
        xrap_msg_t *xrequest_dup = xrap_msg_dup (xrequest);
        request = xrap_msg_encode (&xrequest_dup);
        zsock_send (self->pipe, "sm", "HANDLE REQUEST", request);
        zmsg_destroy (&request);
        zmsg_t *response = zmsg_recv (self->pipe);
        zeb_client_deliver (self->client, zeb_client_sender (self->client), &response);
    }

cleanup:
    xrap_msg_destroy (&xrequest);
    zuuid_t *sender = zeb_client_sender (self->client);
    zuuid_destroy (&sender);
}

//  Here we handle incoming message from the node

static void
s_handler_recv_api (s_handler_t *self)
{
//  Get the whole message of the pipe in one go
    char *command = zstr_recv (self->pipe);
    if (!command)
       return;        //  Interrupted

    if (streq (command, "VERBOSE")) {
        self->verbose = true;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "ADD ACCEPT"))
        zsock_signal (self->pipe, s_handler_add_accept (self));
    else
    if (streq (command, "ADD OFFER"))
        zsock_signal (self->pipe, s_handler_add_offer (self));
    else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
}


//  --------------------------------------------------------------------------
//  This is the handler actor which runs in its own thread and polls its two
//  sockets to process incoming messages.

void
zeb_handler (zsock_t *pipe, void *args)
{
    assert (args);
    char *endpoint = (char *) args;
    s_handler_t *self = s_handler_new (pipe, endpoint);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    zpoller_add (self->poller, zeb_client_msgpipe (self->client));

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
        if (which == self->pipe)
            s_handler_recv_api (self);
        else
        if (which == zeb_client_msgpipe (self->client))
            s_handler_recv_client (self);
    }

    s_handler_destroy (&self);
}


//  ---------------------------------------------------------------------------
//  Add a new accept type that this handler can deliver. May be a regular
//  expression. Returns 0 if successful, otherwise -1.

int
zeb_handler_add_accept (zactor_t *self, const char *accept)
{
    assert (self);
    assert (accept);
    int rc = zstr_sendx (self, "ADD ACCEPT", accept, NULL);
    if (rc == 0)
        rc = zsock_wait (self);
    return rc == 1? -1: rc;
}


//  ---------------------------------------------------------------------------
//  Add a new offer this handler will handle. Returns 0 if successful,
//  otherwise -1;

int
zeb_handler_add_offer (zactor_t *self, int method, const char *uri)
{
    assert (self);
    assert (uri);
    int rc = -1;
    //  Send message name as first, separate frame
    rc = zstr_sendm (self, "ADD OFFER");
    if (rc == 0)
        rc = zsock_bsend (self, "1s", (uint8_t) method, uri);
    if (rc == 0)
        rc = zsock_wait (self);
    return rc == 1? -1: rc;
}


//  ---------------------------------------------------------------------------
//  Self test of this class.

void
zeb_handler_test (bool verbose)
{
    printf (" * zeb_handler: ");

    //  @selftest
    //  Simple create/destroy test

    //  Start a server to test against, and bind to endpoint
    zactor_t *server = zactor_new (zeb_broker, "zeb_client_test");
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "LOAD", "src/zeb_client.cfg", NULL);

    //  Create a client and connect it to the server
    zeb_client_t *client = zeb_client_new ();
    int rc = zeb_client_connect (client, "tcp://127.0.0.1:9999", 1000, "client");
    assert (rc == 0);
    assert (zeb_client_connected (client) == true);

    //  Create a handler
    zactor_t *handler = zactor_new (zeb_handler, (void *) "tcp://127.0.0.1:9999");
    assert (handler);

    //  Set accepted document formats
    rc = zeb_handler_add_accept (handler, "application/(xml|json)");
    assert (rc == 0);

    //  Offer a service
    rc = zeb_handler_add_offer (handler, XRAP_MSG_GET, "/dummy");
    assert (rc == 0);
    rc = zeb_handler_add_offer (handler, XRAP_MSG_PUT, "/dummy");
    assert (rc == 0);

    //  Provide Rubbish Offering
    rc = zeb_handler_add_offer (handler, XRAP_MSG_GET, "/dummy");
    assert (rc == -1);

    //  ================================
    //  GET Tests

    //  Send Request
    xrap_msg_t *xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/xml");
    zmsg_t *msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Receive request and Echo response
    char *command;
    zsock_recv (handler, "sm", &command, &msg);
    assert (streq (command, "HANDLE REQUEST"));
    zstr_free (&command);
    zmsg_send (&msg, handler);

    //  Receive Response (is echo)
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_GET);
    assert (streq (xrap_msg_resource (xmsg), "/dummy"));
    assert (streq (xrap_msg_content_type (xmsg), "application/xml"));
    xrap_msg_destroy (&xmsg);
    zuuid_t *sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request without ACCEPT
    xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Receive Request without ACCEPT
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_ERROR);
    assert (xrap_msg_status_code (xmsg) == XRAP_TRAFFIC_NOT_ACCEPTABLE);
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request with conditionals (not changed)
    xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/json");
    xrap_msg_set_if_none_match (xmsg, "MATCH");
    xrap_msg_set_if_modified_since (xmsg, 10);
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Check etag (match)
    char *etag;
    zsock_recv (handler, "ss", &command, &etag);
    assert (streq (command, "CHECK ETAG"));
    zstr_free (&command);
    assert (streq (etag, "MATCH"));
    zstr_free (&etag);
    zsock_signal (handler, 0);

    //  Check last modified (not modified)
    uint64_t last_modified;
    zsock_recv (handler, "s8", &command, &last_modified);
    assert (streq (command, "CHECK LAST MODIFIED"));
    zstr_free (&command);
    zsock_signal (handler, 0);

    //  Receive Request with conditionals
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_GET_EMPTY);
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request with conditionals (changed)
    xmsg = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/json");
    xrap_msg_set_if_none_match (xmsg, "NONE MATCH");
    xrap_msg_set_if_modified_since (xmsg, 20);
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Check etag (none match)
    zsock_recv (handler, "ss", &command, &etag);
    assert (streq (command, "CHECK ETAG"));
    zstr_free (&command);
    assert (streq (etag, "NONE MATCH"));
    zstr_free (&etag);
    zsock_signal (handler, 1);

    //  Check last modified (modified)
    zsock_recv (handler, "s8", &command, &last_modified);
    assert (streq (command, "CHECK LAST MODIFIED"));
    zstr_free (&command);
    assert (last_modified == 20);
    zsock_signal (handler, 1);

    //  Receive request and Echo response
    zsock_recv (handler, "sm", &command, &msg);
    assert (streq (command, "HANDLE REQUEST"));
    zstr_free (&command);
    zmsg_send (&msg, handler);

    //  Receive response with conditionals
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_GET);
    assert (streq (xrap_msg_resource (xmsg), "/dummy"));
    assert (streq (xrap_msg_content_type (xmsg), "application/json"));
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  ================================
    //  PUT Tests

    //  Send Request
    xmsg = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/xml");
    xrap_msg_set_content_body (xmsg, "application/xml");
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Receive request and Echo response
    zsock_recv (handler, "sm", &command, &msg);
    assert (streq (command, "HANDLE REQUEST"));
    zstr_free (&command);
    zmsg_send (&msg, handler);

    //  Receive response (is echo)
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_PUT);
    assert (streq (xrap_msg_resource (xmsg), "/dummy"));
    assert (streq (xrap_msg_content_type (xmsg), "application/xml"));
    assert (streq (xrap_msg_content_body (xmsg), "application/xml"));
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request with conditionals (update, both)
    xmsg = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/json");
    xrap_msg_set_if_match (xmsg, "MATCH");
    xrap_msg_set_if_unmodified_since (xmsg, 10);
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Check etag (match)
    zsock_recv (handler, "ss", &command, &etag);
    assert (streq (command, "CHECK ETAG"));
    zstr_free (&command);
    assert (streq (etag, "MATCH"));
    zstr_free (&etag);
    zsock_signal (handler, 0);

    //  Check last modified (not modified)
    zsock_recv (handler, "s8", &command, &last_modified);
    assert (streq (command, "CHECK LAST MODIFIED"));
    zstr_free (&command);
    zsock_signal (handler, 0);

    //  Receive request and Echo response
    zsock_recv (handler, "sm", &command, &msg);
    assert (streq (command, "HANDLE REQUEST"));
    zstr_free (&command);
    zmsg_send (&msg, handler);

    //  Receive response with conditionals (update, both)
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_PUT);
    assert (streq (xrap_msg_resource (xmsg), "/dummy"));
    assert (streq (xrap_msg_content_type (xmsg), "application/json"));
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request with conditionals (update, etag)
    xmsg = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/json");
    xrap_msg_set_if_match (xmsg, "MATCH");
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Check etag (match)
    zsock_recv (handler, "ss", &command, &etag);
    assert (streq (command, "CHECK ETAG"));
    zstr_free (&command);
    assert (streq (etag, "MATCH"));
    zstr_free (&etag);
    zsock_signal (handler, 0);

    //  Receive request and Echo response
    zsock_recv (handler, "sm", &command, &msg);
    assert (streq (command, "HANDLE REQUEST"));
    zstr_free (&command);
    zmsg_send (&msg, handler);

    //  Receive response with conditionals (update, etag)
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_PUT);
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request with conditionals (update, last_modified)
    xmsg = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/json");
    xrap_msg_set_if_unmodified_since (xmsg, 10);
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Check last modified (not modified)
    zsock_recv (handler, "s8", &command, &last_modified);
    assert (streq (command, "CHECK LAST MODIFIED"));
    zstr_free (&command);
    zsock_signal (handler, 0);

    //  Receive request and Echo response
    zsock_recv (handler, "sm", &command, &msg);
    assert (streq (command, "HANDLE REQUEST"));
    zstr_free (&command);
    zmsg_send (&msg, handler);

    //  Receive response with conditionals (update, last_modified)
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_PUT);
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  Send Request with conditionals (no update)
    xmsg = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (xmsg, "%s", "/dummy");
    xrap_msg_set_content_type (xmsg, "application/json");
    xrap_msg_set_if_match (xmsg, "NONE MATCH");
    xrap_msg_set_if_unmodified_since (xmsg, 20);
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == 0);

    //  Check etag (none match)
    zsock_recv (handler, "ss", &command, &etag);
    assert (streq (command, "CHECK ETAG"));
    zstr_free (&command);
    assert (streq (etag, "NONE MATCH"));
    zstr_free (&etag);
    zsock_signal (handler, 1);

    //  Check last modified (modified)
    zsock_recv (handler, "s8", &command, &last_modified);
    assert (streq (command, "CHECK LAST MODIFIED"));
    zstr_free (&command);
    assert (last_modified == 20);
    zsock_signal (handler, 1);

    //  Receive Request with conditionals (no update)
    msg = zeb_client_recv (client);
    xmsg = xrap_msg_decode (&msg);
    assert (xrap_msg_id (xmsg) == XRAP_MSG_ERROR);
    assert (xrap_msg_status_code (xmsg) == XRAP_TRAFFIC_PRECONDITION_FAILED);
    xrap_msg_destroy (&xmsg);
    sender = zeb_client_sender (client);
    zuuid_destroy (&sender);

    //  ================================
    //  Invalid Tests

    //  Send Request
    xmsg = xrap_msg_new (XRAP_MSG_GET_EMPTY);
    msg = xrap_msg_encode (&xmsg);
    rc = zeb_client_request (client, 0, &msg);
    assert (rc == XRAP_TRAFFIC_NOT_FOUND);

    zeb_client_destroy (&client);
    zactor_destroy (&handler);

    //  Done, shut down
    zactor_destroy (&server);
    //  @end

    printf ("OK\n");
}
