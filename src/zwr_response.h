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
   zwr_response_new (xrap_msg_t *xresponse);

//  Destroy the zwr_response
ZWEBRAP_EXPORT void
    zwr_response_destroy (zwr_response_t **self_p);

//  Get the response data
ZWEBRAP_EXPORT xrap_msg_t *
   zwr_response_xresponse (zwr_response_t *self);

//  Get the rate limit
ZWEBRAP_EXPORT int
   zwr_response_rate_limit (zwr_response_t *self);

//  Get the rate remaining
ZWEBRAP_EXPORT int
   zwr_response_rate_remaining (zwr_response_t *self);

//  Get the rate reset
ZWEBRAP_EXPORT int64_t
zwr_response_rate_reset (zwr_response_t *self);

//  Set parameters for rate limit
ZWEBRAP_EXPORT void
zwr_response_set_ratelimit (zwr_response_t *self, int limit, int remaining, int64_t reset);

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
