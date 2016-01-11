/*  =========================================================================
    xrap_traffic - zebra protocol

    Codec class for xrap_traffic.

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

/*
@header
    xrap_traffic - zebra protocol
@discuss
@end
*/

#include "../include/zebra.h"
#include "../include/xrap_traffic.h"

//  Structure of our class

struct _xrap_traffic_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  xrap_traffic message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    char address [256];                 //  Client address
    uint32_t timeout;                   //  Timeout, msecs, or zero
    zmsg_t *content;                    //  Message body frames
    char route [256];                   //  Service address
    char method [256];                  //  Match message subjects
    zuuid_t *sender;                    //  Sending client address
    uint16_t status_code;               //  3-digit status code
    char status_reason [256];           //  Printable explanation
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host,size) { \
    memcpy (self->needle, (host), size); \
    self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host,size) { \
    if (self->needle + size > self->ceiling) { \
        zsys_warning ("xrap_traffic: GET_OCTETS failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, size); \
    self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) self->needle = (host); \
    self->needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    self->needle [0] = (byte) (((host) >> 8)  & 255); \
    self->needle [1] = (byte) (((host))       & 255); \
    self->needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    self->needle [0] = (byte) (((host) >> 24) & 255); \
    self->needle [1] = (byte) (((host) >> 16) & 255); \
    self->needle [2] = (byte) (((host) >> 8)  & 255); \
    self->needle [3] = (byte) (((host))       & 255); \
    self->needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    self->needle [0] = (byte) (((host) >> 56) & 255); \
    self->needle [1] = (byte) (((host) >> 48) & 255); \
    self->needle [2] = (byte) (((host) >> 40) & 255); \
    self->needle [3] = (byte) (((host) >> 32) & 255); \
    self->needle [4] = (byte) (((host) >> 24) & 255); \
    self->needle [5] = (byte) (((host) >> 16) & 255); \
    self->needle [6] = (byte) (((host) >> 8)  & 255); \
    self->needle [7] = (byte) (((host))       & 255); \
    self->needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (self->needle + 1 > self->ceiling) { \
        zsys_warning ("xrap_traffic: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("xrap_traffic: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("xrap_traffic: GET_NUMBER4 failed"); \
        goto malformed; \
    } \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) { \
        zsys_warning ("xrap_traffic: GET_NUMBER8 failed"); \
        goto malformed; \
    } \
    (host) = ((uint64_t) (self->needle [0]) << 56) \
           + ((uint64_t) (self->needle [1]) << 48) \
           + ((uint64_t) (self->needle [2]) << 40) \
           + ((uint64_t) (self->needle [3]) << 32) \
           + ((uint64_t) (self->needle [4]) << 24) \
           + ((uint64_t) (self->needle [5]) << 16) \
           + ((uint64_t) (self->needle [6]) << 8) \
           +  (uint64_t) (self->needle [7]); \
    self->needle += 8; \
}

//  Put a string to the frame
#define PUT_STRING(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER1 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a string from the frame
#define GET_STRING(host) { \
    size_t string_size; \
    GET_NUMBER1 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("xrap_traffic: GET_STRING failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  Put a long string to the frame
#define PUT_LONGSTR(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER4 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a long string from the frame
#define GET_LONGSTR(host) { \
    size_t string_size; \
    GET_NUMBER4 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("xrap_traffic: GET_LONGSTR failed"); \
        goto malformed; \
    } \
    free ((host)); \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}


//  --------------------------------------------------------------------------
//  Create a new xrap_traffic

xrap_traffic_t *
xrap_traffic_new (void)
{
    xrap_traffic_t *self = (xrap_traffic_t *) zmalloc (sizeof (xrap_traffic_t));
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the xrap_traffic

void
xrap_traffic_destroy (xrap_traffic_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        xrap_traffic_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        zmsg_destroy (&self->content);
        zuuid_destroy (&self->sender);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive a xrap_traffic from the socket. Returns 0 if OK, -1 if
//  there was an error. Blocks if there is no message waiting.

int
xrap_traffic_recv (xrap_traffic_t *self, zsock_t *input)
{
    assert (input);

    if (zsock_type (input) == ZMQ_ROUTER) {
        zframe_destroy (&self->routing_id);
        self->routing_id = zframe_recv (input);
        if (!self->routing_id || !zsock_rcvmore (input)) {
            zsys_warning ("xrap_traffic: no routing ID");
            return -1;          //  Interrupted or malformed
        }
    }
    zmq_msg_t frame;
    zmq_msg_init (&frame);
    int size = zmq_msg_recv (&frame, zsock_resolve (input), 0);
    if (size == -1) {
        zsys_warning ("xrap_traffic: interrupted");
        goto malformed;         //  Interrupted
    }
    //  Get and check protocol signature
    self->needle = (byte *) zmq_msg_data (&frame);
    self->ceiling = self->needle + zmq_msg_size (&frame);

    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 9)) {
        zsys_warning ("xrap_traffic: invalid signature");
        //  TODO: discard invalid messages and loop, and return
        //  -1 only on interrupt
        goto malformed;         //  Interrupted
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case XRAP_TRAFFIC_CONNECTION_OPEN:
            {
                char protocol [256];
                GET_STRING (protocol);
                if (strneq (protocol, "MALAMUTE")) {
                    zsys_warning ("xrap_traffic: protocol is invalid");
                    goto malformed;
                }
            }
            {
                uint16_t version;
                GET_NUMBER2 (version);
                if (version != 1) {
                    zsys_warning ("xrap_traffic: version is invalid");
                    goto malformed;
                }
            }
            GET_STRING (self->address);
            break;

        case XRAP_TRAFFIC_CONNECTION_PING:
            break;

        case XRAP_TRAFFIC_CONNECTION_PONG:
            break;

        case XRAP_TRAFFIC_CONNECTION_CLOSE:
            break;

        case XRAP_TRAFFIC_XRAP_SEND:
            GET_NUMBER4 (self->timeout);
            //  Get zero or more remaining frames
            zmsg_destroy (&self->content);
            if (zsock_rcvmore (input))
                self->content = zmsg_recv (input);
            else
                self->content = zmsg_new ();
            break;

        case XRAP_TRAFFIC_XRAP_OFFER:
            GET_STRING (self->route);
            GET_STRING (self->method);
            break;

        case XRAP_TRAFFIC_XRAP_DELIVER:
            if (self->needle + ZUUID_LEN > (self->ceiling)) {
                zsys_warning ("xrap_traffic: sender is invalid");
                goto malformed;
            }
            zuuid_destroy (&self->sender);
            self->sender = zuuid_new_from (self->needle);
            self->needle += ZUUID_LEN;
            //  Get zero or more remaining frames
            zmsg_destroy (&self->content);
            if (zsock_rcvmore (input))
                self->content = zmsg_recv (input);
            else
                self->content = zmsg_new ();
            break;

        case XRAP_TRAFFIC_OK:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->status_reason);
            break;

        case XRAP_TRAFFIC_FAIL:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->status_reason);
            break;

        case XRAP_TRAFFIC_ERROR:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->status_reason);
            break;

        default:
            zsys_warning ("xrap_traffic: bad message ID");
            goto malformed;
    }
    //  Successful return
    zmq_msg_close (&frame);
    return 0;

    //  Error returns
    malformed:
        zsys_warning ("xrap_traffic: xrap_traffic malformed message, fail");
        zmq_msg_close (&frame);
        return -1;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Send the xrap_traffic to the socket. Does not destroy it. Returns 0 if
//  OK, else -1.

int
xrap_traffic_send (xrap_traffic_t *self, zsock_t *output)
{
    assert (self);
    assert (output);

    if (zsock_type (output) == ZMQ_ROUTER)
        zframe_send (&self->routing_id, output, ZFRAME_MORE + ZFRAME_REUSE);

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case XRAP_TRAFFIC_CONNECTION_OPEN:
            frame_size += 1 + strlen ("MALAMUTE");
            frame_size += 2;            //  version
            frame_size += 1 + strlen (self->address);
            break;
        case XRAP_TRAFFIC_XRAP_SEND:
            frame_size += 4;            //  timeout
            break;
        case XRAP_TRAFFIC_XRAP_OFFER:
            frame_size += 1 + strlen (self->route);
            frame_size += 1 + strlen (self->method);
            break;
        case XRAP_TRAFFIC_XRAP_DELIVER:
            frame_size += ZUUID_LEN;    //  sender
            break;
        case XRAP_TRAFFIC_OK:
            frame_size += 2;            //  status_code
            frame_size += 1 + strlen (self->status_reason);
            break;
        case XRAP_TRAFFIC_FAIL:
            frame_size += 2;            //  status_code
            frame_size += 1 + strlen (self->status_reason);
            break;
        case XRAP_TRAFFIC_ERROR:
            frame_size += 2;            //  status_code
            frame_size += 1 + strlen (self->status_reason);
            break;
    }
    //  Now serialize message into the frame
    zmq_msg_t frame;
    zmq_msg_init_size (&frame, frame_size);
    self->needle = (byte *) zmq_msg_data (&frame);
    PUT_NUMBER2 (0xAAA0 | 9);
    PUT_NUMBER1 (self->id);
    bool have_content = false;
    size_t nbr_frames = 1;              //  Total number of frames to send

    switch (self->id) {
        case XRAP_TRAFFIC_CONNECTION_OPEN:
            PUT_STRING ("MALAMUTE");
            PUT_NUMBER2 (1);
            PUT_STRING (self->address);
            break;

        case XRAP_TRAFFIC_XRAP_SEND:
            PUT_NUMBER4 (self->timeout);
            nbr_frames += self->content? zmsg_size (self->content): 1;
            have_content = true;
            break;

        case XRAP_TRAFFIC_XRAP_OFFER:
            PUT_STRING (self->route);
            PUT_STRING (self->method);
            break;

        case XRAP_TRAFFIC_XRAP_DELIVER:
            if (self->sender)
                zuuid_export (self->sender, self->needle);
            else
                memset (self->needle, 0, ZUUID_LEN);
            self->needle += ZUUID_LEN;
            nbr_frames += self->content? zmsg_size (self->content): 1;
            have_content = true;
            break;

        case XRAP_TRAFFIC_OK:
            PUT_NUMBER2 (self->status_code);
            PUT_STRING (self->status_reason);
            break;

        case XRAP_TRAFFIC_FAIL:
            PUT_NUMBER2 (self->status_code);
            PUT_STRING (self->status_reason);
            break;

        case XRAP_TRAFFIC_ERROR:
            PUT_NUMBER2 (self->status_code);
            PUT_STRING (self->status_reason);
            break;

    }
    //  Now send the data frame
    zmq_msg_send (&frame, zsock_resolve (output), --nbr_frames? ZMQ_SNDMORE: 0);

    //  Now send the content if necessary
    if (have_content) {
        if (self->content) {
            zframe_t *frame = zmsg_first (self->content);
            while (frame) {
                zframe_send (&frame, output, ZFRAME_REUSE + (--nbr_frames? ZFRAME_MORE: 0));
                frame = zmsg_next (self->content);
            }
        }
        else
            zmq_send (zsock_resolve (output), NULL, 0, 0);
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
xrap_traffic_print (xrap_traffic_t *self)
{
    assert (self);
    switch (self->id) {
        case XRAP_TRAFFIC_CONNECTION_OPEN:
            zsys_debug ("XRAP_TRAFFIC_CONNECTION_OPEN:");
            zsys_debug ("    protocol=malamute");
            zsys_debug ("    version=1");
            zsys_debug ("    address='%s'", self->address);
            break;

        case XRAP_TRAFFIC_CONNECTION_PING:
            zsys_debug ("XRAP_TRAFFIC_CONNECTION_PING:");
            break;

        case XRAP_TRAFFIC_CONNECTION_PONG:
            zsys_debug ("XRAP_TRAFFIC_CONNECTION_PONG:");
            break;

        case XRAP_TRAFFIC_CONNECTION_CLOSE:
            zsys_debug ("XRAP_TRAFFIC_CONNECTION_CLOSE:");
            break;

        case XRAP_TRAFFIC_XRAP_SEND:
            zsys_debug ("XRAP_TRAFFIC_XRAP_SEND:");
            zsys_debug ("    timeout=%ld", (long) self->timeout);
            zsys_debug ("    content=");
            if (self->content)
                zmsg_print (self->content);
            else
                zsys_debug ("(NULL)");
            break;

        case XRAP_TRAFFIC_XRAP_OFFER:
            zsys_debug ("XRAP_TRAFFIC_XRAP_OFFER:");
            zsys_debug ("    route='%s'", self->route);
            zsys_debug ("    method='%s'", self->method);
            break;

        case XRAP_TRAFFIC_XRAP_DELIVER:
            zsys_debug ("XRAP_TRAFFIC_XRAP_DELIVER:");
            zsys_debug ("    sender=");
            if (self->sender)
                zsys_debug ("        %s", zuuid_str_canonical (self->sender));
            else
                zsys_debug ("        (NULL)");
            zsys_debug ("    content=");
            if (self->content)
                zmsg_print (self->content);
            else
                zsys_debug ("(NULL)");
            break;

        case XRAP_TRAFFIC_OK:
            zsys_debug ("XRAP_TRAFFIC_OK:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            zsys_debug ("    status_reason='%s'", self->status_reason);
            break;

        case XRAP_TRAFFIC_FAIL:
            zsys_debug ("XRAP_TRAFFIC_FAIL:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            zsys_debug ("    status_reason='%s'", self->status_reason);
            break;

        case XRAP_TRAFFIC_ERROR:
            zsys_debug ("XRAP_TRAFFIC_ERROR:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            zsys_debug ("    status_reason='%s'", self->status_reason);
            break;

    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
xrap_traffic_routing_id (xrap_traffic_t *self)
{
    assert (self);
    return self->routing_id;
}

void
xrap_traffic_set_routing_id (xrap_traffic_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the xrap_traffic id

int
xrap_traffic_id (xrap_traffic_t *self)
{
    assert (self);
    return self->id;
}

void
xrap_traffic_set_id (xrap_traffic_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
xrap_traffic_command (xrap_traffic_t *self)
{
    assert (self);
    switch (self->id) {
        case XRAP_TRAFFIC_CONNECTION_OPEN:
            return ("CONNECTION_OPEN");
            break;
        case XRAP_TRAFFIC_CONNECTION_PING:
            return ("CONNECTION_PING");
            break;
        case XRAP_TRAFFIC_CONNECTION_PONG:
            return ("CONNECTION_PONG");
            break;
        case XRAP_TRAFFIC_CONNECTION_CLOSE:
            return ("CONNECTION_CLOSE");
            break;
        case XRAP_TRAFFIC_XRAP_SEND:
            return ("XRAP_SEND");
            break;
        case XRAP_TRAFFIC_XRAP_OFFER:
            return ("XRAP_OFFER");
            break;
        case XRAP_TRAFFIC_XRAP_DELIVER:
            return ("XRAP_DELIVER");
            break;
        case XRAP_TRAFFIC_OK:
            return ("OK");
            break;
        case XRAP_TRAFFIC_FAIL:
            return ("FAIL");
            break;
        case XRAP_TRAFFIC_ERROR:
            return ("ERROR");
            break;
    }
    return "?";
}

//  --------------------------------------------------------------------------
//  Get/set the address field

const char *
xrap_traffic_address (xrap_traffic_t *self)
{
    assert (self);
    return self->address;
}

void
xrap_traffic_set_address (xrap_traffic_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->address)
        return;
    strncpy (self->address, value, 255);
    self->address [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the timeout field

uint32_t
xrap_traffic_timeout (xrap_traffic_t *self)
{
    assert (self);
    return self->timeout;
}

void
xrap_traffic_set_timeout (xrap_traffic_t *self, uint32_t timeout)
{
    assert (self);
    self->timeout = timeout;
}


//  --------------------------------------------------------------------------
//  Get the content field without transferring ownership

zmsg_t *
xrap_traffic_content (xrap_traffic_t *self)
{
    assert (self);
    return self->content;
}

//  Get the content field and transfer ownership to caller

zmsg_t *
xrap_traffic_get_content (xrap_traffic_t *self)
{
    zmsg_t *content = self->content;
    self->content = NULL;
    return content;
}

//  Set the content field, transferring ownership from caller

void
xrap_traffic_set_content (xrap_traffic_t *self, zmsg_t **msg_p)
{
    assert (self);
    assert (msg_p);
    zmsg_destroy (&self->content);
    self->content = *msg_p;
    *msg_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the route field

const char *
xrap_traffic_route (xrap_traffic_t *self)
{
    assert (self);
    return self->route;
}

void
xrap_traffic_set_route (xrap_traffic_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->route)
        return;
    strncpy (self->route, value, 255);
    self->route [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the method field

const char *
xrap_traffic_method (xrap_traffic_t *self)
{
    assert (self);
    return self->method;
}

void
xrap_traffic_set_method (xrap_traffic_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->method)
        return;
    strncpy (self->method, value, 255);
    self->method [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the sender field
zuuid_t *
xrap_traffic_sender (xrap_traffic_t *self)
{
    assert (self);
    return self->sender;
}

void
xrap_traffic_set_sender (xrap_traffic_t *self, zuuid_t *uuid)
{
    assert (self);
    zuuid_destroy (&self->sender);
    self->sender = zuuid_dup (uuid);
}

//  Get the sender field and transfer ownership to caller

zuuid_t *
xrap_traffic_get_sender (xrap_traffic_t *self)
{
    zuuid_t *sender = self->sender;
    self->sender = NULL;
    return sender;
}


//  --------------------------------------------------------------------------
//  Get/set the status_code field

uint16_t
xrap_traffic_status_code (xrap_traffic_t *self)
{
    assert (self);
    return self->status_code;
}

void
xrap_traffic_set_status_code (xrap_traffic_t *self, uint16_t status_code)
{
    assert (self);
    self->status_code = status_code;
}


//  --------------------------------------------------------------------------
//  Get/set the status_reason field

const char *
xrap_traffic_status_reason (xrap_traffic_t *self)
{
    assert (self);
    return self->status_reason;
}

void
xrap_traffic_set_status_reason (xrap_traffic_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->status_reason)
        return;
    strncpy (self->status_reason, value, 255);
    self->status_reason [255] = 0;
}



//  --------------------------------------------------------------------------
//  Selftest

void
xrap_traffic_test (bool verbose)
{
    printf (" * xrap_traffic:");

    if (verbose)
        printf ("\n");

    //  @selftest
    //  Simple create/destroy test
    xrap_traffic_t *self = xrap_traffic_new ();
    assert (self);
    xrap_traffic_destroy (&self);
    //  Create pair of sockets we can send through
    //  We must bind before connect if we wish to remain compatible with ZeroMQ < v4
    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    int rc = zsock_bind (output, "inproc://selftest-xrap_traffic");
    assert (rc == 0);

    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    rc = zsock_connect (input, "inproc://selftest-xrap_traffic");
    assert (rc == 0);


    //  Encode/send/decode and verify each message type
    int instance;
    self = xrap_traffic_new ();
    xrap_traffic_set_id (self, XRAP_TRAFFIC_CONNECTION_OPEN);

    xrap_traffic_set_address (self, "Life is short but Now lasts for ever");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (streq (xrap_traffic_address (self), "Life is short but Now lasts for ever"));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_CONNECTION_PING);

    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_CONNECTION_PONG);

    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_CONNECTION_CLOSE);

    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_XRAP_SEND);

    xrap_traffic_set_timeout (self, 123);
    zmsg_t *xrap_send_content = zmsg_new ();
    xrap_traffic_set_content (self, &xrap_send_content);
    zmsg_addstr (xrap_traffic_content (self), "Captcha Diem");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (xrap_traffic_timeout (self) == 123);
        assert (zmsg_size (xrap_traffic_content (self)) == 1);
        char *content = zmsg_popstr (xrap_traffic_content (self));
        assert (streq (content, "Captcha Diem"));
        zstr_free (&content);
        if (instance == 1)
            zmsg_destroy (&xrap_send_content);
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_XRAP_OFFER);

    xrap_traffic_set_route (self, "Life is short but Now lasts for ever");
    xrap_traffic_set_method (self, "Life is short but Now lasts for ever");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (streq (xrap_traffic_route (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_traffic_method (self), "Life is short but Now lasts for ever"));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_XRAP_DELIVER);

    zuuid_t *xrap_deliver_sender = zuuid_new ();
    xrap_traffic_set_sender (self, xrap_deliver_sender);
    zmsg_t *xrap_deliver_content = zmsg_new ();
    xrap_traffic_set_content (self, &xrap_deliver_content);
    zmsg_addstr (xrap_traffic_content (self), "Captcha Diem");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (zuuid_eq (xrap_deliver_sender, zuuid_data (xrap_traffic_sender (self))));
        if (instance == 1)
            zuuid_destroy (&xrap_deliver_sender);
        assert (zmsg_size (xrap_traffic_content (self)) == 1);
        char *content = zmsg_popstr (xrap_traffic_content (self));
        assert (streq (content, "Captcha Diem"));
        zstr_free (&content);
        if (instance == 1)
            zmsg_destroy (&xrap_deliver_content);
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_OK);

    xrap_traffic_set_status_code (self, 123);
    xrap_traffic_set_status_reason (self, "Life is short but Now lasts for ever");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (xrap_traffic_status_code (self) == 123);
        assert (streq (xrap_traffic_status_reason (self), "Life is short but Now lasts for ever"));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_FAIL);

    xrap_traffic_set_status_code (self, 123);
    xrap_traffic_set_status_reason (self, "Life is short but Now lasts for ever");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (xrap_traffic_status_code (self) == 123);
        assert (streq (xrap_traffic_status_reason (self), "Life is short but Now lasts for ever"));
    }
    xrap_traffic_set_id (self, XRAP_TRAFFIC_ERROR);

    xrap_traffic_set_status_code (self, 123);
    xrap_traffic_set_status_reason (self, "Life is short but Now lasts for ever");
    //  Send twice
    xrap_traffic_send (self, output);
    xrap_traffic_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        xrap_traffic_recv (self, input);
        assert (xrap_traffic_routing_id (self));
        assert (xrap_traffic_status_code (self) == 123);
        assert (streq (xrap_traffic_status_reason (self), "Life is short but Now lasts for ever"));
    }

    xrap_traffic_destroy (&self);
    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
}
