/*  =========================================================================
    zwr_client - zeb_server client implementation for both clients and handlers

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zwr_client.xml, or
     * The code generation script that built this file: zproto_client_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZWEBRAP.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_CLIENT_H_INCLUDED
#define ZWR_CLIENT_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef ZWR_CLIENT_T_DEFINED
typedef struct _zwr_client_t zwr_client_t;
#define ZWR_CLIENT_T_DEFINED
#endif

//  @interface
//  Create a new zwr_client, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
ZWEBRAP_EXPORT zwr_client_t *
    zwr_client_new (void);

//  Destroy the zwr_client and free all memory used by the object.
ZWEBRAP_EXPORT void
    zwr_client_destroy (zwr_client_t **self_p);

//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.
ZWEBRAP_EXPORT zactor_t *
    zwr_client_actor (zwr_client_t *self);

//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In
//  the low-volume case we can do everything over the actor pipe, if traffic
//  is never ambiguous.
ZWEBRAP_EXPORT zsock_t *
    zwr_client_msgpipe (zwr_client_t *self);

//  Return true if client is currently connected, else false. Note that the
//  client will automatically re-connect if the server dies and restarts after
//  a successful first connection.
ZWEBRAP_EXPORT bool
    zwr_client_connected (zwr_client_t *self);

//  Connect to server endpoint, with specified timeout in msecs (zero means wait    
//  forever). Constructor succeeds if connection is successful. The caller may      
//  specify its address.                                                            
//  Returns >= 0 if successful, -1 if interrupted.
ZWEBRAP_EXPORT int 
    zwr_client_connect (zwr_client_t *self, const char *endpoint, uint32_t timeout, const char *address);

//  Offer to handle particular XRAP requests, where the route matches request's     
//  resource.                                                                       
//  Returns >= 0 if successful, -1 if interrupted.
ZWEBRAP_EXPORT int 
    zwr_client_set_handler (zwr_client_t *self, const char *method, const char *route);

//  No explanation                                                                  
//  Returns >= 0 if successful, -1 if interrupted.
ZWEBRAP_EXPORT int 
    zwr_client_request (zwr_client_t *self, uint32_t timeout, zmsg_t **content_p);

//  Send XRAP DELIVER message to server, takes ownership of message
//  and destroys message when done sending it.
ZWEBRAP_EXPORT int
    zwr_client_deliver (zwr_client_t *self, zuuid_t *sender, zmsg_t **content_p);

//  Receive message from server; caller destroys message when done
ZWEBRAP_EXPORT zmsg_t *
    zwr_client_recv (zwr_client_t *self);

//  Return last received command. Can be one of these values:
//      "XRAP DELIVER"
ZWEBRAP_EXPORT const char *
    zwr_client_command (zwr_client_t *self);

//  Return last received status
ZWEBRAP_EXPORT int 
    zwr_client_status (zwr_client_t *self);

//  Return last received reason
ZWEBRAP_EXPORT const char *
    zwr_client_reason (zwr_client_t *self);

//  Return last received sender
ZWEBRAP_EXPORT zuuid_t *
    zwr_client_sender (zwr_client_t *self);

//  Return last received content
ZWEBRAP_EXPORT zmsg_t *
    zwr_client_content (zwr_client_t *self);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_client_test (bool verbose);

//  To enable verbose tracing (animation) of zwr_client instances, set
//  this to true. This lets you trace from and including construction.
ZWEBRAP_EXPORT extern volatile int
    zwr_client_verbose;
//  @end

#ifdef __cplusplus
}
#endif

#endif
