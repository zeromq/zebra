/*  =========================================================================
    zeb_response - Representation of a HTTP response.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZWEBRAP.
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

/*
@header
    zeb_response - Representation of a HTTP response.
@discuss
@end
*/

#include "zebra_classes.h"

//  Structure of our class

struct _zeb_response_t {
    int rate_limit;
    int rate_remaining;
    size_t rate_reset;
    xrap_msg_t *xresponse;
};


//  --------------------------------------------------------------------------
//  Create a new zeb_response. Takes ownership of data.

zeb_response_t *
zeb_response_new (xrap_msg_t *xresponse)
{
    assert (xresponse);

    zeb_response_t *self = (zeb_response_t *) zmalloc (sizeof (zeb_response_t));
    assert (self);

    self->xresponse = xresponse;

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zeb_response.

void
zeb_response_destroy (zeb_response_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zeb_response_t *self = *self_p;

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
zeb_response_xresponse (zeb_response_t *self)
{
    assert (self);
    return self->xresponse;
}


//  --------------------------------------------------------------------------
//  Get the rate limit

int
zeb_response_rate_limit (zeb_response_t *self)
{
    assert (self);
    return self->rate_limit;
}


//  --------------------------------------------------------------------------
//  Get the rate remaining

int
zeb_response_rate_remaining (zeb_response_t *self)
{
    assert (self);
    return self->rate_remaining;
}


//  --------------------------------------------------------------------------
//  Get the rate reset

int64_t
zeb_response_rate_reset (zeb_response_t *self)
{
    assert (self);
    return self->rate_reset;
}


//  --------------------------------------------------------------------------
//  Set parameters for rate limit

void
zeb_response_set_ratelimit (zeb_response_t *self, int limit, int remaining, int64_t reset)
{
    assert (self);
    self->rate_limit = limit;
    self->rate_remaining = remaining;
    self->rate_reset = reset;
}

//  --------------------------------------------------------------------------
//  Print properties of the zeb_response object.

void
zeb_response_print (zeb_response_t *self)
{
    assert (self);
    printf ("zeb_response {\n");
    printf ("}\n");
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zeb_response_test (bool verbose)
{
    printf (" * zeb_response: ");

    //  @selftest
    //  Simple create/destroy test
    //  @end

    printf ("OK\n");
}
