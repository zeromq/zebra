/*  =========================================================================
    zeb_connection - Wrapper for zeb_request and zeb_response.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZWEBRAP.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZEB_CONNECTION_H_INCLUDED
#define ZEB_CONNECTION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zeb_connection_t zeb_connection_t;

//  @interface
//  Create a new zeb_connection
ZEBRA_EXPORT zeb_connection_t *
    zeb_connection_new (zeb_request_t *request);

//  Destroy the zeb_connection
ZEBRA_EXPORT void
    zeb_connection_destroy (zeb_connection_t **self_p);

//  Get the request for this connection
ZEBRA_EXPORT zeb_request_t *
    zeb_connection_request (zeb_connection_t *self);

//  Get the response for this connection
ZEBRA_EXPORT zeb_response_t *
    zeb_connection_response (zeb_connection_t *self);

//  Set the response for this connection
ZEBRA_EXPORT void
    zeb_connection_set_response (zeb_connection_t *self, zeb_response_t *response);

//  Print properties of object
ZEBRA_EXPORT void
    zeb_connection_print (zeb_connection_t *self);

//  Self test of this class
ZEBRA_EXPORT void
    zeb_connection_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
