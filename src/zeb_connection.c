/*  =========================================================================
    zeb_connection - Wrapper for zeb_request and zeb_response.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZWEBRAP.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zeb_connection - Wrapper for zeb_request and zeb_response.
@discuss
@end
*/

#include "../include/zebra.h"
#include "zebra_classes.h"

//  Structure of our class

struct _zeb_connection_t {
    zeb_request_t *request;
    zeb_response_t *response;
};


//  --------------------------------------------------------------------------
//  Create a new zeb_connection.

zeb_connection_t *
zeb_connection_new (zeb_request_t *request)
{
    zeb_connection_t *self = (zeb_connection_t *) zmalloc (sizeof (zeb_connection_t));
    assert (self);

    //  Initialize properties
    self->request = request;
    self->response = NULL;

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zeb_connection.

void
zeb_connection_destroy (zeb_connection_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zeb_connection_t *self = *self_p;

        //  Free class properties
        zeb_request_destroy (&self->request);
        if (self->response)
            zeb_response_destroy (&self->response);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Get the request for this connection

zeb_request_t *
zeb_connection_request (zeb_connection_t *self)
{
    assert (self);
    return self->request;
}

//  --------------------------------------------------------------------------
//  Get the response for this connection

zeb_response_t *
zeb_connection_response (zeb_connection_t *self)
{
    assert (self);
    return self->response;
}

//  --------------------------------------------------------------------------
//  Set the response for this connection

void
zeb_connection_set_response (zeb_connection_t *self, zeb_response_t *response)
{
    assert (self);
    self->response = response;
}

//  --------------------------------------------------------------------------
//  Print properties of the zeb_connection object.

void
zeb_connection_print (zeb_connection_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zeb_connection_test (bool verbose)
{
    printf (" * zeb_connection: ");

    //  @selftest
    //  Simple create/destroy test
    zeb_request_t *request = zeb_request_new ("/", "POST", "text/xml");
    zeb_connection_t *self = zeb_connection_new (request);
    assert (self);
    zeb_connection_destroy (&self);
    //  @end

    printf ("OK\n");
}
