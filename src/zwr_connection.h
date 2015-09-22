/*  =========================================================================
    zwr_connection - Wrapper for zwr_request and zwr_response.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_CONNECTION_H_INCLUDED
#define ZWR_CONNECTION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zwr_connection_t zwr_connection_t;

//  @interface
//  Create a new zwr_connection
ZWEBRAP_EXPORT zwr_connection_t *
    zwr_connection_new (zwr_request_t *request);

//  Destroy the zwr_connection
ZWEBRAP_EXPORT void
    zwr_connection_destroy (zwr_connection_t **self_p);

//  Get the request for this connection
ZWEBRAP_EXPORT zwr_request_t *
    zwr_connection_request (zwr_connection_t *self);

//  Get the response for this connection
ZWEBRAP_EXPORT zwr_response_t *
    zwr_connection_response (zwr_connection_t *self);

//  Set the response for this connection
ZWEBRAP_EXPORT void
    zwr_connection_set_response (zwr_connection_t *self, zwr_response_t *response);

//  Print properties of object
ZWEBRAP_EXPORT void
    zwr_connection_print (zwr_connection_t *self);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_connection_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
