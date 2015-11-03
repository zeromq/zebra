/*  =========================================================================
    xrap_traffic - The ZWebRap Protocol

    Codec header for xrap_traffic.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: xrap_traffic.xml, or
     * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZWEBRAP.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef XRAP_TRAFFIC_H_INCLUDED
#define XRAP_TRAFFIC_H_INCLUDED

/*  These are the xrap_traffic messages:

    CONNECTION_OPEN - Client opens a connection to the server. Client can ask for a mailbox
by specifying an address. If mailbox does not exist, server creates it.
Server replies with OK or ERROR.
        protocol            string      Constant "MALAMUTE"
        version             number 2    Protocol version 1
        address             string      Client address

    CONNECTION_PING - Client pings the server. Server replies with CONNECTION-PONG, or
ERROR with status COMMAND-INVALID if the client is not recognized
(e.g. after a server restart or network recovery).

    CONNECTION_PONG - Server replies to a client connection ping.

    CONNECTION_CLOSE - Client closes the connection. This is polite though not mandatory.
Server will reply with OK or ERROR.

    XRAP_SEND - Client sends a xrap request to a xrap queue. Server replies with
OK when queued, or ERROR if that failed. If the tracker is not
empty, the client can expect a CONFIRM at some later time.
Confirmations are asynchronous. If the message cannot be delivered
within the specified timeout (zero means infinite), the server
discards it and returns CONFIRM with a TIMEOUT-EXPIRED status.
        timeout             number 4    Timeout, msecs, or zero
        content             msg         Message body frames

    XRAP_OFFER - Worker client offers to handle request for a given route and method.
An empty route is not allowed neither is an empty method. A worker
can offer many different services at once. Server replies with OK
or ERROR.
        route               string      Service address
        method              string      Match message subjects

    XRAP_DELIVER - Either server delivers a xrap request to a worker client or worker
client responds with xrap response to previous request.
        sender              uuid        Sending client address
        content             msg         Message body frames

    OK - Server replies with success status. Actual status code provides more
information. An OK always has a 2xx status code.
        status_code         number 2    3-digit status code
        status_reason       string      Printable explanation

    FAIL - Server replies with failure status. Actual status code provides more
information. An FAIL always has 3xx or 4xx status code.
        status_code         number 2    3-digit status code
        status_reason       string      Printable explanation

    ERROR - Server replies with failure status. Actual status code provides more
information. An ERROR always has a 5xx status code.
        status_code         number 2    3-digit status code
        status_reason       string      Printable explanation
*/

#define XRAP_TRAFFIC_SUCCESS                200
#define XRAP_TRAFFIC_STORED                 201
#define XRAP_TRAFFIC_DELIVERED              202
#define XRAP_TRAFFIC_NOT_DELIVERED          300
#define XRAP_TRAFFIC_CONTENT_TOO_LARGE      301
#define XRAP_TRAFFIC_TIMEOUT_EXPIRED        302
#define XRAP_TRAFFIC_CONNECTION_REFUSED     303
#define XRAP_TRAFFIC_BAD_REQUEST            400
#define XRAP_TRAFFIC_ACCESS_REFUSED         401
#define XRAP_TRAFFIC_NOT_FOUND              404
#define XRAP_TRAFFIC_CONFLICT               409
#define XRAP_TRAFFIC_RESOURCE_LOCKED        423
#define XRAP_TRAFFIC_COMMAND_INVALID        500
#define XRAP_TRAFFIC_NOT_IMPLEMENTED        501
#define XRAP_TRAFFIC_INTERNAL_ERROR         502

#define XRAP_TRAFFIC_CONNECTION_OPEN        1
#define XRAP_TRAFFIC_CONNECTION_PING        2
#define XRAP_TRAFFIC_CONNECTION_PONG        3
#define XRAP_TRAFFIC_CONNECTION_CLOSE       4
#define XRAP_TRAFFIC_XRAP_SEND              5
#define XRAP_TRAFFIC_XRAP_OFFER             6
#define XRAP_TRAFFIC_XRAP_DELIVER           7
#define XRAP_TRAFFIC_OK                     8
#define XRAP_TRAFFIC_FAIL                   9
#define XRAP_TRAFFIC_ERROR                  10

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef XRAP_TRAFFIC_T_DEFINED
typedef struct _xrap_traffic_t xrap_traffic_t;
#define XRAP_TRAFFIC_T_DEFINED
#endif

//  @interface
//  Create a new empty xrap_traffic
ZWEBRAP_EXPORT xrap_traffic_t *
    xrap_traffic_new (void);

//  Destroy a xrap_traffic instance
ZWEBRAP_EXPORT void
    xrap_traffic_destroy (xrap_traffic_t **self_p);

//  Receive a xrap_traffic from the socket. Returns 0 if OK, -1 if
//  there was an error. Blocks if there is no message waiting.
ZWEBRAP_EXPORT int
    xrap_traffic_recv (xrap_traffic_t *self, zsock_t *input);

//  Send the xrap_traffic to the output socket, does not destroy it
ZWEBRAP_EXPORT int
    xrap_traffic_send (xrap_traffic_t *self, zsock_t *output);

//  Print contents of message to stdout
ZWEBRAP_EXPORT void
    xrap_traffic_print (xrap_traffic_t *self);

//  Get/set the message routing id
ZWEBRAP_EXPORT zframe_t *
    xrap_traffic_routing_id (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_routing_id (xrap_traffic_t *self, zframe_t *routing_id);

//  Get the xrap_traffic id and printable command
ZWEBRAP_EXPORT int
    xrap_traffic_id (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_id (xrap_traffic_t *self, int id);
ZWEBRAP_EXPORT const char *
    xrap_traffic_command (xrap_traffic_t *self);

//  Get/set the address field
ZWEBRAP_EXPORT const char *
    xrap_traffic_address (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_address (xrap_traffic_t *self, const char *value);

//  Get/set the timeout field
ZWEBRAP_EXPORT uint32_t
    xrap_traffic_timeout (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_timeout (xrap_traffic_t *self, uint32_t timeout);

//  Get a copy of the content field
ZWEBRAP_EXPORT zmsg_t *
    xrap_traffic_content (xrap_traffic_t *self);
//  Get the content field and transfer ownership to caller
ZWEBRAP_EXPORT zmsg_t *
    xrap_traffic_get_content (xrap_traffic_t *self);
//  Set the content field, transferring ownership from caller
ZWEBRAP_EXPORT void
    xrap_traffic_set_content (xrap_traffic_t *self, zmsg_t **msg_p);

//  Get/set the route field
ZWEBRAP_EXPORT const char *
    xrap_traffic_route (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_route (xrap_traffic_t *self, const char *value);

//  Get/set the method field
ZWEBRAP_EXPORT const char *
    xrap_traffic_method (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_method (xrap_traffic_t *self, const char *value);

//  Get/set the sender field
ZWEBRAP_EXPORT zuuid_t *
    xrap_traffic_sender (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_sender (xrap_traffic_t *self, zuuid_t *uuid);
//  Get the sender field and transfer ownership to caller
ZWEBRAP_EXPORT zuuid_t *
    xrap_traffic_get_sender (xrap_traffic_t *self);

//  Get/set the status_code field
ZWEBRAP_EXPORT uint16_t
    xrap_traffic_status_code (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_status_code (xrap_traffic_t *self, uint16_t status_code);

//  Get/set the status_reason field
ZWEBRAP_EXPORT const char *
    xrap_traffic_status_reason (xrap_traffic_t *self);
ZWEBRAP_EXPORT void
    xrap_traffic_set_status_reason (xrap_traffic_t *self, const char *value);

//  Self test of this class
ZWEBRAP_EXPORT void
    xrap_traffic_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define xrap_traffic_dump   xrap_traffic_print

#ifdef __cplusplus
}
#endif

#endif
