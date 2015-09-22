/*  =========================================================================
    zwr_server - zwebrap server actor

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
    zwr_server - zwebrap server actor
@discuss
@end
*/

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_ACCEPTED 202

typedef struct _zwr_server_t zwr_server_t;

//  Structure of our actor

struct _zwr_server_t {
    zsock_t *pipe;                //  Pipe back to creator
    bool terminated;              //  Flag indicating termination
    zpoller_t *poller;
    //  Properties
    zwr_microhttpd_t *http_server;
};

static zwr_response_t *
zwr_server_get_handler (zwr_request_t *request)
{
    assert (request);
    zwr_response_t *response = NULL;

    if (0 == strcmp (zwr_request_path (request), "/")) {
        char *page = "<html><body>Hello, Dude!</body></html>";
        response = zwr_response_new (page, "text/html", HTTP_OK, MODE_DUP);
    }
    else
    if (0 == strcmp (zwr_request_path (request), "/countdown")) {
        struct timeval currentTime;
        gettimeofday (&currentTime, 0);
        int countdown = 20 - 5;//(currentTime.tv_sec - startTime.tv_sec);
        if (countdown < 0) {
            char *donepage = "<html><body>Done!</body></html>";
            response = zwr_response_new (donepage, "text/html", HTTP_OK, MODE_DUP);
        }
        else {
            char *waitpage = "<html><body>Countdown %d</body></html>";
            char *waitstr = (char *)  malloc (250);
            sprintf (waitstr, waitpage, countdown);
            response = zwr_response_new (waitstr, "text/html", HTTP_OK, MODE_DUP);
            free (waitstr);
        }
    }
    else {
        char *bla = "<bla><blub></blub></bla>";
        response = zwr_response_new (bla, "text/xml", HTTP_OK, MODE_DUP);
    }
    return response;
}

zwr_response_t *
zwr_server_post_handler (zwr_request_t *request)
{
    assert (request);
    zwr_response_t *response = NULL;

    printf ("%.*s\n", (int) zwr_request_data_size (request), zwr_request_data (request));

    char *page = "<html><body>Thanks, POST!</body></html>";
    response = zwr_response_new (page, "text/html", HTTP_CREATED, MODE_DUP);

    return response;
}

zwr_response_t *
zwr_server_put_handler (zwr_request_t *request)
{
    assert (request);
    zwr_response_t *response = NULL;

    printf ("%.*s\n", (int) zwr_request_data_size (request), zwr_request_data (request));

    char *page = "<html><body>Thanks, PUT!</body></html>";
    response = zwr_response_new (page, "text/html", HTTP_ACCEPTED, MODE_DUP);

    return response;
}

zwr_response_t *
zwr_server_delete_handler (zwr_request_t *request)
{
    assert (request);
    zwr_response_t *response = NULL;

    char *page = "<html><body>Thanks, DELETE!</body></html>";
    response = zwr_response_new (page, "text/html", HTTP_OK, MODE_DUP);

    return response;
}



//  --------------------------------------------------------------------------
//  Create a new zwr_server.

static zwr_server_t *
zwr_server_new (zsock_t *pipe, void *args)
{
    zwr_server_t *self = (zwr_server_t *) zmalloc (sizeof (zwr_server_t));
    assert (self);

    self->pipe = pipe;
    self->terminated = false;
    self->poller = zpoller_new (self->pipe, NULL);

    //  Initialize properties
    self->http_server = zwr_microhttpd_new (zwr_server_get_handler,
                                            zwr_server_post_handler,
                                            zwr_server_put_handler,
                                            zwr_server_delete_handler);

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zwr_server.

static void
zwr_server_destroy (zwr_server_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_server_t *self = *self_p;

        //  Free actor properties
        zwr_microhttpd_destroy (&self->http_server);

        //  Free object itself
        zpoller_destroy (&self->poller);
        free (self);
        *self_p = NULL;
    }
}


//  Start this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
zwr_server_start (zwr_server_t *self)
{
    assert (self);
    zwr_microhttpd_start (self->http_server);

    return 0;
}


//  Stop this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
zwr_server_stop (zwr_server_t *self)
{
    assert (self);
    zwr_microhttpd_stop (self->http_server);

    return 0;
}


//  Here we handle incomming message from the node

static void
zwr_server_recv_api (zwr_server_t *self)
{
//  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "START"))
        zsock_signal (self->pipe, zwr_server_start (self));
    else
    if (streq (command, "STOP"))
        zsock_signal (self->pipe, zwr_server_stop (self));
    else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    free (command);
    zmsg_destroy (&request);
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
zwr_server_actor (zsock_t *pipe, void *args)
{
    zwr_server_t * self = zwr_server_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
       zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 0);
       if (which == self->pipe)
          zwr_server_recv_api (self);
       //  Add other sockets when you need them.
    }

    zwr_server_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Self test of this actor.

void
zwr_server_test (bool verbose)
{
    printf (" * zwr_server: ");

    int rc = 0;
    //  @selftest
    //  Simple create/destroy test
    zactor_t *zwr_server = zactor_new (zwr_server_actor, NULL);

    zstr_send (zwr_server, "START");
    rc = zsock_wait (zwr_server);                  //  Wait until actor started
    assert (rc == 0);

    getchar ();

    zstr_send (zwr_server, "STOP");
    rc = zsock_wait (zwr_server);                  //  Wait until actor stopped
    assert (rc == 0);

    zactor_destroy (&zwr_server);
    //  @end

    printf ("OK\n");
}
