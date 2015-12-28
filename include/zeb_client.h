/*  =========================================================================
    zeb_client - zeb_server client implementation for both clients and handlers

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zeb_client.xml, or
     * The code generation script that built this file: zproto_client_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZWEBRAP.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZEB_CLIENT_H_INCLUDED
#define ZEB_CLIENT_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef ZEB_CLIENT_T_DEFINED
typedef struct _zeb_client_t zeb_client_t;
#define ZEB_CLIENT_T_DEFINED
#endif

//  @interface
//  Create a new zeb_client, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
ZEBRA_EXPORT zeb_client_t *
    zeb_client_new (void);

//  Destroy the zeb_client and free all memory used by the object.
ZEBRA_EXPORT void
    zeb_client_destroy (zeb_client_t **self_p);

//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.
ZEBRA_EXPORT zactor_t *
    zeb_client_actor (zeb_client_t *self);

//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In
//  the low-volume case we can do everything over the actor pipe, if traffic
//  is never ambiguous.
ZEBRA_EXPORT zsock_t *
    zeb_client_msgpipe (zeb_client_t *self);

//  Return true if client is currently connected, else false. Note that the
//  client will automatically re-connect if the server dies and restarts after
//  a successful first connection.
ZEBRA_EXPORT bool
    zeb_client_connected (zeb_client_t *self);

//  Connect to server endpoint, with specified timeout in msecs (zero means wait    
//  forever). Constructor succeeds if connection is successful. The caller may      
//  specify its address.                                                            
//  Returns >= 0 if successful, -1 if interrupted.
ZEBRA_EXPORT int 
    zeb_client_connect (zeb_client_t *self, const char *endpoint, uint32_t timeout, const char *address);

//  Offer to handle particular XRAP requests, where the route matches request's     
//  resource.                                                                       
//  Returns >= 0 if successful, -1 if interrupted.
ZEBRA_EXPORT int 
    zeb_client_set_handler (zeb_client_t *self, const char *method, const char *route);

//  No explanation                                                                  
//  Returns >= 0 if successful, -1 if interrupted.
ZEBRA_EXPORT int 
    zeb_client_request (zeb_client_t *self, uint32_t timeout, zmsg_t **content_p);

//  Send XRAP DELIVER message to server, takes ownership of message
//  and destroys message when done sending it.
ZEBRA_EXPORT int
    zeb_client_deliver (zeb_client_t *self, zuuid_t *sender, zmsg_t **content_p);

//  Receive message from server; caller destroys message when done
ZEBRA_EXPORT zmsg_t *
    zeb_client_recv (zeb_client_t *self);

//  Return last received command. Can be one of these values:
//      "XRAP DELIVER"
ZEBRA_EXPORT const char *
    zeb_client_command (zeb_client_t *self);

//  Return last received status
ZEBRA_EXPORT int 
    zeb_client_status (zeb_client_t *self);

//  Return last received reason
ZEBRA_EXPORT const char *
    zeb_client_reason (zeb_client_t *self);

//  Return last received sender
ZEBRA_EXPORT zuuid_t *
    zeb_client_sender (zeb_client_t *self);

//  Return last received content
ZEBRA_EXPORT zmsg_t *
    zeb_client_content (zeb_client_t *self);

//  Self test of this class
ZEBRA_EXPORT void
    zeb_client_test (bool verbose);

//  To enable verbose tracing (animation) of zeb_client instances, set
//  this to true. This lets you trace from and including construction.
ZEBRA_EXPORT extern volatile int
    zeb_client_verbose;
//  @end

#ifdef __cplusplus
}
#endif

#endif
