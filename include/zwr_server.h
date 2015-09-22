/*  =========================================================================
    zwr_server - zwebrap server actor

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_SERVER_H_INCLUDED
#define ZWR_SERVER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


//  @interface
// This is the actor that runs a single node; it uses one thread.
ZWEBRAP_EXPORT void
    zwr_server_actor (zsock_t *pipe, void *args);

//  Self test of this actor
ZWEBRAP_EXPORT void
    zwr_server_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
