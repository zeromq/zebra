/*  =========================================================================
    zeb_response - Representation of a HTTP response.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZEB_RESPONSE_H_INCLUDED
#define ZEB_RESPONSE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zeb_response_t zeb_response_t;

//  @interface
//  Create a new zeb_response
ZEBRA_EXPORT zeb_response_t *
   zeb_response_new (xrap_msg_t *xresponse);

//  Destroy the zeb_response
ZEBRA_EXPORT void
    zeb_response_destroy (zeb_response_t **self_p);

//  Get the response data
ZEBRA_EXPORT xrap_msg_t *
   zeb_response_xresponse (zeb_response_t *self);

//  Get the rate limit
ZEBRA_EXPORT int
   zeb_response_rate_limit (zeb_response_t *self);

//  Get the rate remaining
ZEBRA_EXPORT int
   zeb_response_rate_remaining (zeb_response_t *self);

//  Get the rate reset
ZEBRA_EXPORT int64_t
zeb_response_rate_reset (zeb_response_t *self);

//  Set parameters for rate limit
ZEBRA_EXPORT void
zeb_response_set_ratelimit (zeb_response_t *self, int limit, int remaining, int64_t reset);

//  Print properties of object
ZEBRA_EXPORT void
    zeb_response_print (zeb_response_t *self);

//  Self test of this class
ZEBRA_EXPORT void
    zeb_response_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
