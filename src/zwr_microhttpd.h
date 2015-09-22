/*  =========================================================================
    zwr_microhttpd - HTTP web server backend using libmicrohttpd.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_MICROHTTPD_H_INCLUDED
#define ZWR_MICROHTTPD_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_HEADER_CONTENT_TYPE "Content-Type"

typedef struct _zwr_microhttpd_t zwr_microhttpd_t;

//  @interface
// Callback function for zwr_microhttpd http handler method
typedef zwr_response_t * (zwr_microhttpd_handler_fn) (zwr_request_t *request);

//  Create a new zwr_microhttpd
ZWEBRAP_EXPORT zwr_microhttpd_t *
    zwr_microhttpd_new (zwr_microhttpd_handler_fn *get_handler,
                        zwr_microhttpd_handler_fn *post_handler,
                        zwr_microhttpd_handler_fn *put_handler,
                        zwr_microhttpd_handler_fn *delete_handler);

//  Destroy the zwr_microhttpd
ZWEBRAP_EXPORT void
    zwr_microhttpd_destroy (zwr_microhttpd_t **self_p);

//  Start the zwr_microhttpd server. Returns 0 if successful otherwise -1.
ZWEBRAP_EXPORT int
    zwr_microhttpd_start (zwr_microhttpd_t *self);

//  Stops the zwr_microhttpd server. Returns 0 if successful otherwise -1.
ZWEBRAP_EXPORT int
    zwr_microhttpd_stop (zwr_microhttpd_t *self);

//  Print properties of object
ZWEBRAP_EXPORT void
    zwr_microhttpd_print (zwr_microhttpd_t *self);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_microhttpd_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
