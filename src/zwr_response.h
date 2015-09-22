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

#ifndef ZWR_RESPONSE_H_INCLUDED
#define ZWR_RESPONSE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zwr_response_t zwr_response_t;

//  @interface
//  Create a new zwr_response
ZWEBRAP_EXPORT zwr_response_t *
    zwr_response_new (char *data, const char *mime_type, unsigned int status_code, int mode);

//  Destroy the zwr_response
ZWEBRAP_EXPORT void
    zwr_response_destroy (zwr_response_t **self_p);

//  Get the response data
ZWEBRAP_EXPORT const char *
    zwr_response_data (zwr_response_t *self);

//  Get the response mime type
ZWEBRAP_EXPORT const char *
    zwr_response_mime_type (zwr_response_t *self);

//  Get the response staus code
ZWEBRAP_EXPORT unsigned int
    zwr_response_status_code (zwr_response_t *self);

//  Print properties of object
ZWEBRAP_EXPORT void
    zwr_response_print (zwr_response_t *self);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_response_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
