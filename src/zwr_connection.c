/*  =========================================================================
    zwr_connection - Wrapper for zwr_request and zwr_response.

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
    zwr_connection - Wrapper for zwr_request and zwr_response.
@discuss
@end
*/

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

//  Structure of our class

struct _zwr_connection_t {
    zwr_request_t *request;
    zwr_response_t *response;
};


//  --------------------------------------------------------------------------
//  Create a new zwr_connection.

zwr_connection_t *
zwr_connection_new (zwr_request_t *request)
{
    zwr_connection_t *self = (zwr_connection_t *) zmalloc (sizeof (zwr_connection_t));
    assert (self);

    //  Initialize properties
    self->request = request;
    self->response = NULL;

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zwr_connection.

void
zwr_connection_destroy (zwr_connection_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_connection_t *self = *self_p;

        //  Free class properties
        zwr_request_destroy (&self->request);
        zwr_response_destroy (&self->response);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Get the request for this connection

zwr_request_t *
zwr_connection_request (zwr_connection_t *self)
{
    assert (self);
    return self->request;
}

//  --------------------------------------------------------------------------
//  Get the response for this connection

zwr_response_t *
zwr_connection_response (zwr_connection_t *self)
{
    assert (self);
    return self->response;
}

//  --------------------------------------------------------------------------
//  Set the response for this connection

void
zwr_connection_set_response (zwr_connection_t *self, zwr_response_t *response)
{
    assert (self);
    self->response = response;
}

//  --------------------------------------------------------------------------
//  Print properties of the zwr_connection object.

void
zwr_connection_print (zwr_connection_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zwr_connection_test (bool verbose)
{
    printf (" * zwr_connection: ");

    //  @selftest
    //  Simple create/destroy test
    zwr_request_t *request = zwr_request_new ("/", "POST", "text/xml");
    zwr_connection_t *self = zwr_connection_new (request);
    assert (self);
    zwr_connection_destroy (&self);
    //  @end

    printf ("OK\n");
}
