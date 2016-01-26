/*  =========================================================================
    zeb_request - Representation of a HTTP request.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZEBRA.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zeb_request - Representation of a HTTP request.
@discuss
@end
*/

#include "zebra_classes.h"

//  Structure of our class

struct _zeb_request_t {
    char *ip;
    const char *path;
    const char *action;
    const char *version;
    const char *data;
    size_t data_size;
    zhash_t *header;
    zhash_t *query;
};


//  --------------------------------------------------------------------------
//  Create a new zeb_request.

zeb_request_t *
zeb_request_new (const char *path, const char *action, const char *version)
{
    assert (path);
    assert (action);

    zeb_request_t *self = (zeb_request_t *) zmalloc (sizeof (zeb_request_t));
    assert (self);

    self->path = path;
    self->action = action;
    self->version = version;
    self->data = NULL;
    self->data_size = 0;
    self->header = zhash_new ();
    zhash_autofree (self->header);
    self->query = zhash_new ();
    zhash_autofree (self->query);

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zeb_request.

void
zeb_request_destroy (zeb_request_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zeb_request_t *self = *self_p;

        //  Free class properties
        zhash_destroy (&self->header);
        zhash_destroy (&self->query);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Get the request ip

const char *
zeb_request_ip (zeb_request_t *self)
{
    assert (self);
    return self->ip;
}

//  --------------------------------------------------------------------------
//  Get the request path

const char *
zeb_request_path (zeb_request_t *self)
{
    assert (self);
    return self->path;
}

//  --------------------------------------------------------------------------
//  Get the request action

const char *
zeb_request_action (zeb_request_t *self)
{
    assert (self);
    return self->action;
}

//  --------------------------------------------------------------------------
//  Get the request's HTTP version

const char *
zeb_request_version (zeb_request_t *self)
{
    assert (self);
    return self->version;
}

//  --------------------------------------------------------------------------
//  Get the request's data, maybe NULL

const char *
zeb_request_data (zeb_request_t *self)
{
    assert (self);
    return self->data;
}

//  --------------------------------------------------------------------------
//  Set request data from POST or PUT

void
zeb_request_set_data (zeb_request_t *self, const char *data, size_t data_size)
{
    assert (self);
    self->data = data;
    self->data_size = data_size;
}

//  --------------------------------------------------------------------------
//  Get the request's data size, if data NULL then returns 0

size_t
zeb_request_data_size (zeb_request_t *self)
{
    assert (self);
    return self->data_size;
}


//  --------------------------------------------------------------------------
//  Get the request's header, maybe empty

zhash_t *
zeb_request_header (zeb_request_t *self)
{
    assert (self);
    return self->header;
}


//  --------------------------------------------------------------------------
//  Get the request's query, maybe empty

zhash_t *
zeb_request_query (zeb_request_t *self)
{
    assert (self);
    return self->query;
}

//  --------------------------------------------------------------------------
//  Print properties of the zeb_request object.

void
zeb_request_print (zeb_request_t *self)
{
    assert (self);
    printf ("zeb_request {\n");
    printf ("  Path: %s,\n", self->path);
    printf ("  Action: %s,\n", self->action);
    printf ("  Version: %s,\n", self->version);
    //  Print HTTP header
    zlist_t *header_keys = zhash_keys (self->header);
    char *header_key = (char *) zlist_first (header_keys);
    printf ("  Header {\n");
    while (header_key) {
        printf ("    %s: %s,\n", header_key, (char *) zhash_lookup (self->header, header_key));
        header_key = (char *) zlist_next (header_keys);
    }
    zlist_destroy (&header_keys);
    printf ("  }\n");
    //  Print query arguments, if any
    if (zhash_size (self->query)) {
        zlist_t *query_keys = zhash_keys (self->query);
        char *query_key = (char *) zlist_first (query_keys);
        printf ("  Query {\n");
        while (query_key) {
            printf ("    %s: %s,\n", query_key, (char *) zhash_lookup (self->query, query_key));
            query_key = (char *) zlist_next (query_keys);
        }
        zlist_destroy (&query_keys);
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
zeb_request_test (bool verbose)
{
    printf (" * zeb_request: ");

    //  @selftest
    //  Simple create/destroy test
    zeb_request_t *self = zeb_request_new ("/", "GET", "1.1");
    assert (self);
    zeb_request_destroy (&self);
    //  @end

    printf ("OK\n");
}
