/*  =========================================================================
    zwr_dispatcher - URL routing and dispating.

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
    zwr_dispatcher - URL routing and dispating.
@discuss
@end
*/

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

//  Structure of our class

struct _zwr_dispatcher_t {
    //  TODO: Declare properties
    int dummy;
};


//  --------------------------------------------------------------------------
//  Create a new zwr_dispatcher.

zwr_dispatcher_t *
zwr_dispatcher_new ()
{
    zwr_dispatcher_t *self = (zwr_dispatcher_t *) zmalloc (sizeof (zwr_dispatcher_t));
    assert (self);

    //  TODO: Initialize properties

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zwr_dispatcher.

void
zwr_dispatcher_destroy (zwr_dispatcher_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_dispatcher_t *self = *self_p;

        //  TODO: Free class properties

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Print properties of the zwr_dispatcher object.

void
zwr_dispatcher_print (zwr_dispatcher_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zwr_dispatcher_test (bool verbose)
{
    printf (" * zwr_dispatcher: ");

    //  @selftest
    //  Simple create/destroy test
    zwr_dispatcher_t *self = zwr_dispatcher_new ();
    assert (self);
    zwr_dispatcher_destroy (&self);
    //  @end

    printf ("OK\n");
}
