/*  =========================================================================
    zwr_dispatcher - URL routing and dispating.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_DISPATCHER_H_INCLUDED
#define ZWR_DISPATCHER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zwr_dispatcher_t zwr_dispatcher_t;

//  @interface
//  Create a new zwr_dispatcher
ZWEBRAP_EXPORT zwr_dispatcher_t *
    zwr_dispatcher_new (void);

//  Destroy the zwr_dispatcher
ZWEBRAP_EXPORT void
    zwr_dispatcher_destroy (zwr_dispatcher_t **self_p);

//  Print properties of object
ZWEBRAP_EXPORT void
    zwr_dispatcher_print (zwr_dispatcher_t *self);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_dispatcher_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
