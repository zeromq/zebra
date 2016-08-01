/*  =========================================================================
    zeb_request - Representation of a HTTP request.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZEB_REQUEST_H_INCLUDED
#define ZEB_REQUEST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define MODE_DUP 0      // Duplicate request values
#define MODE_OWN 1      // Take ownership of request values

typedef struct _zeb_request_t zeb_request_t;

//  @interface
//  Create a new zeb_request
ZEBRA_EXPORT zeb_request_t *
    zeb_request_new (const char *path, const char *action, const char *version);

//  Destroy the zeb_request
ZEBRA_EXPORT void
    zeb_request_destroy (zeb_request_t **self_p);

//  Get the request ip
ZEBRA_EXPORT const char *
    zeb_request_ip (zeb_request_t *self);

//  Get the request path
ZEBRA_EXPORT const char *
    zeb_request_path (zeb_request_t *self);

//  Get the request action
ZEBRA_EXPORT const char *
    zeb_request_action (zeb_request_t *self);

//  Get the request's HTTP version
ZEBRA_EXPORT const char *
    zeb_request_version (zeb_request_t *self);

//  Get the request's data, maybe NULL
ZEBRA_EXPORT const char *
    zeb_request_data (zeb_request_t *self);

//  Set request data from POST or PUT
ZEBRA_EXPORT void
    zeb_request_set_data (zeb_request_t *self, char *data, size_t data_size);

//  Get the request's data size, if data NULL then returns 0
ZEBRA_EXPORT size_t
    zeb_request_data_size (zeb_request_t *self);

//  Get the request's header, maybe empty
ZEBRA_EXPORT zhash_t *
    zeb_request_header (zeb_request_t *self);

//  Get the request's query, maybe empty
ZEBRA_EXPORT zhash_t *
    zeb_request_query (zeb_request_t *self);

//  Print properties of object
ZEBRA_EXPORT void
    zeb_request_print (zeb_request_t *self);

//  Self test of this class
ZEBRA_EXPORT void
    zeb_request_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
