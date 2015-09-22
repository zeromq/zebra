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
    unsigned int status_code;
    char *data;
    int mode;
    zhashx_t *header;
};


//  Internal helper functions

void s_destroy_header_value (void **value_p);

void *
s_duplicate_header_value (const void *item)
{
    assert (item);
    return strdup ((char *) item);
}

//  --------------------------------------------------------------------------
//  Create a new zwr_response. Takes ownership of data.

zwr_response_t *
zwr_response_new (char *data, const char *mime_type, unsigned int status_code, int mode)
{
    assert (data);

    zwr_response_t *self = (zwr_response_t *) zmalloc (sizeof (zwr_response_t));
    assert (self);

    self->status_code = status_code;
    self->mode = mode;
    self->header = zhashx_new ();
    zhashx_set_destructor (self->header, s_destroy_header_value);
    if (MODE_DUP == self->mode) {
        self->data = strdup (data);
        zhashx_set_duplicator (self->header, s_duplicate_header_value);
    }
    if (MODE_OWN == self->mode)
        self->data = data;
    zhashx_insert (self->header, HTTP_HEADER_CONTENT_TYPE, (void *) mime_type);

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the header values

void
s_destroy_header_value (void **value_p)
{
    assert (value_p);
    if (*value_p) {
        char *value = (char *) *value_p;
        free (value);
        *value_p = NULL;
    }
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
        free (self->data);
        zhashx_destroy (&self->header);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Get the response data

const char *
zwr_response_data (zwr_response_t *self)
{
    assert (self);
    return self->data;
}

//  --------------------------------------------------------------------------
//  Get the response mime type

const char *
zwr_response_mime_type (zwr_response_t *self)
{
    assert (self);
    return (const char *) zhashx_lookup (self->header, HTTP_HEADER_CONTENT_TYPE);
}

//  --------------------------------------------------------------------------
//  Get the response staus code

unsigned int
zwr_response_status_code (zwr_response_t *self)
{
    assert (self);
    return self->status_code;
}

//  --------------------------------------------------------------------------
//  Print properties of the zwr_response object.

void
zwr_response_print (zwr_response_t *self)
{
    assert (self);
    printf ("zwr_response {\n");
    printf ("  mime: %s,\n", zwr_response_mime_type (self));
    printf ("  status: %d,\n", self->status_code);
    printf ("  Data:\n");
    printf ("  -->\n%s\n  <--", self->data);
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
    zwr_response_t *self = zwr_response_new ("", "text/plain", 200, MODE_DUP);
    assert (self);
    zwr_response_destroy (&self);
    //  @end

    printf ("OK\n");
}
