/*  =========================================================================
    zwr_request - Representation of a HTTP request.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZWEBRAP.
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_REQUEST_H_INCLUDED
#define ZWR_REQUEST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define MODE_DUP 0      // Duplicate request values
#define MODE_OWN 1      // Take ownership of request values

typedef struct _zwr_request_t zwr_request_t;

//  @interface
//  Create a new zwr_request
ZWEBRAP_EXPORT zwr_request_t *
    zwr_request_new (const char *path, const char *action, const char *version);

//  Destroy the zwr_request
ZWEBRAP_EXPORT void
    zwr_request_destroy (zwr_request_t **self_p);

//  Get the request ip
ZWEBRAP_EXPORT const char *
    zwr_request_ip (zwr_request_t *self);

//  Get the request path
ZWEBRAP_EXPORT const char *
    zwr_request_path (zwr_request_t *self);

//  Get the request action
ZWEBRAP_EXPORT const char *
    zwr_request_action (zwr_request_t *self);

//  Get the request's HTTP version
ZWEBRAP_EXPORT const char *
    zwr_request_version (zwr_request_t *self);

//  Get the request's data, maybe NULL
ZWEBRAP_EXPORT const char *
    zwr_request_data (zwr_request_t *self);

//  Set request data from POST or PUT
ZWEBRAP_EXPORT void
    zwr_request_set_data (zwr_request_t *self, const char *data, size_t data_size);

//  Get the request's data size, if data NULL then returns 0
ZWEBRAP_EXPORT size_t
    zwr_request_data_size (zwr_request_t *self);

//  Get the request's header, maybe empty
ZWEBRAP_EXPORT zhash_t *
    zwr_request_header (zwr_request_t *self);

//  Get the request's query, maybe empty
ZWEBRAP_EXPORT zhash_t *
    zwr_request_query (zwr_request_t *self);

//  Print properties of object
ZWEBRAP_EXPORT void
    zwr_request_print (zwr_request_t *self);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_request_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
