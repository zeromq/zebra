/*  =========================================================================
    zwr_response - Representation of a HTTP response.

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
    zwr_response - Representation of a HTTP response.
@discuss
@end
*/

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

//  Structure of our class

struct _zwr_response_t {
    int rate_limit;
    int rate_remaining;
    size_t rate_reset;
    xrap_msg_t *xresponse;
};


//  --------------------------------------------------------------------------
//  Create a new zwr_response. Takes ownership of data.

zwr_response_t *
zwr_response_new (xrap_msg_t *xresponse)
{
    assert (xresponse);

    zwr_response_t *self = (zwr_response_t *) zmalloc (sizeof (zwr_response_t));
    assert (self);

    self->xresponse = xresponse;

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zwr_response.

void
zwr_response_destroy (zwr_response_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_response_t *self = *self_p;

        //  Free class properties
        xrap_msg_destroy (&self->xresponse);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Get the response data

xrap_msg_t *
zwr_response_xresponse (zwr_response_t *self)
{
    assert (self);
    return self->xresponse;
}


//  --------------------------------------------------------------------------
//  Get the rate limit

int
zwr_response_rate_limit (zwr_response_t *self)
{
    assert (self);
    return self->rate_limit;
}


//  --------------------------------------------------------------------------
//  Get the rate remaining

int
zwr_response_rate_remaining (zwr_response_t *self)
{
    assert (self);
    return self->rate_remaining;
}


//  --------------------------------------------------------------------------
//  Get the rate reset

int64_t
zwr_response_rate_reset (zwr_response_t *self)
{
    assert (self);
    return self->rate_reset;
}


//  --------------------------------------------------------------------------
//  Set parameters for rate limit

void
zwr_response_set_ratelimit (zwr_response_t *self, int limit, int remaining, int64_t reset)
{
    assert (self);
    self->rate_limit = limit;
    self->rate_remaining = remaining;
    self->rate_reset = reset;
}

//  --------------------------------------------------------------------------
//  Print properties of the zwr_response object.

void
zwr_response_print (zwr_response_t *self)
{
    assert (self);
    printf ("zwr_response {\n");
    printf ("}\n");
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zwr_response_test (bool verbose)
{
    printf (" * zwr_response: ");

    //  @selftest
    //  Simple create/destroy test
    //  @end

    printf ("OK\n");
}
