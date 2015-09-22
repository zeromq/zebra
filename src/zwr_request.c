/*  =========================================================================
    zwr_request - Representation of a HTTP request.

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
    zwr_request - Representation of a HTTP request.
@discuss
@end
*/

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

//  Structure of our class

struct _zwr_request_t {
    char *ip;
    const char *path;
    const char *action;
    const char *version;
    const char *data;
    size_t data_size;
    zhashx_t *header;
    zhashx_t *query;
};


//  --------------------------------------------------------------------------
//  Create a new zwr_request.

zwr_request_t *
zwr_request_new (const char *path, const char *action, const char *version)
{
    assert (path);
    assert (action);

    zwr_request_t *self = (zwr_request_t *) zmalloc (sizeof (zwr_request_t));
    assert (self);

    self->path = path;
    self->action = action;
    self->version = version;
    self->data = NULL;
    self->data_size = 0;
    self->header = zhashx_new ();
    self->query = zhashx_new ();

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zwr_request.

void
zwr_request_destroy (zwr_request_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_request_t *self = *self_p;

        //  Free class properties
        zhashx_destroy (&self->header);
        zhashx_destroy (&self->query);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Get the request ip

const char *
zwr_request_ip (zwr_request_t *self)
{
    assert (self);
    return self->ip;
}

//  --------------------------------------------------------------------------
//  Get the request path

const char *
zwr_request_path (zwr_request_t *self)
{
    assert (self);
    return self->path;
}

//  --------------------------------------------------------------------------
//  Get the request action

const char *
zwr_request_action (zwr_request_t *self)
{
    assert (self);
    return self->action;
}

//  --------------------------------------------------------------------------
//  Get the request's HTTP version

const char *
zwr_request_version (zwr_request_t *self)
{
    assert (self);
    return self->version;
}

//  --------------------------------------------------------------------------
//  Get the request's data, maybe NULL

const char *
zwr_request_data (zwr_request_t *self)
{
    assert (self);
    return self->data;
}

//  --------------------------------------------------------------------------
//  Set request data from POST or PUT

void
zwr_request_set_data (zwr_request_t *self, const char *data, size_t data_size)
{
    assert (self);
    self->data = data;
    self->data_size = data_size;
}

//  --------------------------------------------------------------------------
//  Get the request's data size, if data NULL then returns 0

size_t
zwr_request_data_size (zwr_request_t *self)
{
    assert (self);
    return self->data_size;
}


//  --------------------------------------------------------------------------
//  Get the request's header, maybe empty

zhashx_t *
zwr_request_header (zwr_request_t *self)
{
    assert (self);
    return self->header;
}


//  --------------------------------------------------------------------------
//  Get the request's query, maybe empty

zhashx_t *
zwr_request_query (zwr_request_t *self)
{
    assert (self);
    return self->query;
}

//  --------------------------------------------------------------------------
//  Print properties of the zwr_request object.

void
zwr_request_print (zwr_request_t *self)
{
    assert (self);
    printf ("zwr_request {\n");
    printf ("  Path: %s,\n", self->path);
    printf ("  Action: %s,\n", self->action);
    printf ("  Version: %s,\n", self->version);
    //  Print HTTP header
    zlistx_t *header_keys = zhashx_keys (self->header);
    zlistx_sort (header_keys);
    char *header_key = (char *) zlistx_first (header_keys);
    printf ("  Header {\n");
    while (header_key) {
        printf ("    %s: %s,\n", header_key, (char *) zhashx_lookup (self->header, header_key));
        header_key = (char *) zlistx_next (header_keys);
    }
    zlistx_destroy (&header_keys);
    printf ("  }\n");
    //  Print query arguments, if any arguments, if any
    if (zhashx_size (self->query)) {
        zlistx_t *query_keys = zhashx_keys (self->query);
        zlistx_sort (query_keys);
        char *query_key = (char *) zlistx_first (query_keys);
        printf ("  Query {\n");
        while (query_key) {
            printf ("    %s: %s,\n", query_key, (char *) zhashx_lookup (self->query, query_key));
            query_key = (char *) zlistx_next (query_keys);
        }
        zlistx_destroy (&query_keys);
        printf ("  }\n");
    }
    //  Print data, if any
    if (self->data) {
        printf ("  Data:\n");
        printf ("-->\n%.*s\n<--", (int) self->data_size, self->data);
    }
    printf ("}\n");
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zwr_request_test (bool verbose)
{
    printf (" * zwr_request: ");

    //  @selftest
    //  Simple create/destroy test
    zwr_request_t *self = zwr_request_new ("/", "GET", "1.1");
    assert (self);
    zwr_request_destroy (&self);
    //  @end

    printf ("OK\n");
}
