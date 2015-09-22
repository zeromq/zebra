/*  =========================================================================
    xrap_msg - XRAP serialization over ZMTP
    
    Codec header for xrap_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: xrap_msg.xml, or
     * The code generation script that built this file: zproto_codec_c_v1
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef XRAP_MSG_H_INCLUDED
#define XRAP_MSG_H_INCLUDED

/*  These are the xrap_msg messages:

    POST - Create a new, dynamically named resource in some parent.
        parent              string      Schema/type/name
        content_type        string      Content type
        content_body        longstr     New resource specification

    POST_OK - Success response for POST.
        status_code         number 2    Response status code 2xx
        location            string      Schema/type/name
        etag                string      Opaque hash tag
        date_modified       number 8    Date and time modified
        content_type        string      Content type
        content_body        longstr     Resource contents

    GET - Retrieve a known resource.
        resource            string      Schema/type/name
        if_modified_since   number 8    GET if more recent
        if_none_match       string      GET if changed
        content_type        string      Desired content type

    GET_OK - Success response for GET.
        status_code         number 2    Response status code 2xx
        content_type        string      Actual content type
        content_body        longstr     Resource specification

    GET_EMPTY - Conditional GET returned 304 Not Modified.
        status_code         number 2    Response status code 3xx

    PUT - Update a known resource.
        resource            string      Schema/type/name
        if_unmodified_since  number 8   Update if same date
        if_match            string      Update if same ETag
        content_type        string      Content type
        content_body        longstr     New resource specification

    PUT_OK - Success response for PUT.
        status_code         number 2    Response status code 2xx
        location            string      Schema/type/name
        etag                string      Opaque hash tag
        date_modified       number 8    Date and time modified

    DELETE - Remove a known resource.
        resource            string      schema/type/name
        if_unmodified_since  number 8   DELETE if same date
        if_match            string      DELETE if same ETag

    DELETE_OK - Success response for DELETE.
        status_code         number 2    Response status code 2xx

    ERROR - Error response for any request.
        status_code         number 2    Response status code, 4xx or 5xx
        status_text         string      Response status text
*/


#define XRAP_MSG_POST                       1
#define XRAP_MSG_POST_OK                    2
#define XRAP_MSG_GET                        3
#define XRAP_MSG_GET_OK                     4
#define XRAP_MSG_GET_EMPTY                  5
#define XRAP_MSG_PUT                        6
#define XRAP_MSG_PUT_OK                     7
#define XRAP_MSG_DELETE                     8
#define XRAP_MSG_DELETE_OK                  9
#define XRAP_MSG_ERROR                      10

#include <czmq.h>


#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef XRAP_MSG_T_DEFINED
typedef struct _xrap_msg_t xrap_msg_t;
#define XRAP_MSG_T_DEFINED
#endif

//  @interface
//  Create a new xrap_msg
xrap_msg_t *
    xrap_msg_new (int id);

//  Destroy the xrap_msg
void
    xrap_msg_destroy (xrap_msg_t **self_p);

//  Parse a zmsg_t and decides whether it is xrap_msg. Returns
//  true if it is, false otherwise. Doesn't destroy or modify the
//  original message.
bool
    is_xrap_msg (zmsg_t *msg_p);

//  Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL.
xrap_msg_t *
    xrap_msg_decode (zmsg_t *msg_p);

//  Encode xrap_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.
zmsg_t *
    xrap_msg_encode (xrap_msg_t **self_p);

//  Receive and parse a xrap_msg from the socket. Returns new object, 
//  or NULL if error. Will block if there's no message waiting.
xrap_msg_t *
    xrap_msg_recv (void *input);

//  Receive and parse a xrap_msg from the socket. Returns new object, 
//  or NULL either if there was no input waiting, or the recv was interrupted.
xrap_msg_t *
    xrap_msg_recv_nowait (void *input);

//  Send the xrap_msg to the output, and destroy it
int
    xrap_msg_send (xrap_msg_t **self_p, void *output);

//  Send the xrap_msg to the output, and do not destroy it
int
    xrap_msg_send_again (xrap_msg_t *self, void *output);

//  Encode the POST 
zmsg_t *
    xrap_msg_encode_post (
        const char *parent,
        const char *content_type,
        const char *content_body);

//  Encode the POST_OK 
zmsg_t *
    xrap_msg_encode_post_ok (
        uint16_t status_code,
        const char *location,
        const char *etag,
        uint64_t date_modified,
        const char *content_type,
        const char *content_body);

//  Encode the GET 
zmsg_t *
    xrap_msg_encode_get (
        const char *resource,
        uint64_t if_modified_since,
        const char *if_none_match,
        const char *content_type);

//  Encode the GET_OK 
zmsg_t *
    xrap_msg_encode_get_ok (
        uint16_t status_code,
        const char *content_type,
        const char *content_body);

//  Encode the GET_EMPTY 
zmsg_t *
    xrap_msg_encode_get_empty (
        uint16_t status_code);

//  Encode the PUT 
zmsg_t *
    xrap_msg_encode_put (
        const char *resource,
        uint64_t if_unmodified_since,
        const char *if_match,
        const char *content_type,
        const char *content_body);

//  Encode the PUT_OK 
zmsg_t *
    xrap_msg_encode_put_ok (
        uint16_t status_code,
        const char *location,
        const char *etag,
        uint64_t date_modified);

//  Encode the DELETE 
zmsg_t *
    xrap_msg_encode_delete (
        const char *resource,
        uint64_t if_unmodified_since,
        const char *if_match);

//  Encode the DELETE_OK 
zmsg_t *
    xrap_msg_encode_delete_ok (
        uint16_t status_code);

//  Encode the ERROR 
zmsg_t *
    xrap_msg_encode_error (
        uint16_t status_code,
        const char *status_text);


//  Send the POST to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_post (void *output,
        const char *parent,
        const char *content_type,
        const char *content_body);
    
//  Send the POST_OK to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_post_ok (void *output,
        uint16_t status_code,
        const char *location,
        const char *etag,
        uint64_t date_modified,
        const char *content_type,
        const char *content_body);
    
//  Send the GET to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_get (void *output,
        const char *resource,
        uint64_t if_modified_since,
        const char *if_none_match,
        const char *content_type);
    
//  Send the GET_OK to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_get_ok (void *output,
        uint16_t status_code,
        const char *content_type,
        const char *content_body);
    
//  Send the GET_EMPTY to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_get_empty (void *output,
        uint16_t status_code);
    
//  Send the PUT to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_put (void *output,
        const char *resource,
        uint64_t if_unmodified_since,
        const char *if_match,
        const char *content_type,
        const char *content_body);
    
//  Send the PUT_OK to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_put_ok (void *output,
        uint16_t status_code,
        const char *location,
        const char *etag,
        uint64_t date_modified);
    
//  Send the DELETE to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_delete (void *output,
        const char *resource,
        uint64_t if_unmodified_since,
        const char *if_match);
    
//  Send the DELETE_OK to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_delete_ok (void *output,
        uint16_t status_code);
    
//  Send the ERROR to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    xrap_msg_send_error (void *output,
        uint16_t status_code,
        const char *status_text);
    
//  Duplicate the xrap_msg message
xrap_msg_t *
    xrap_msg_dup (xrap_msg_t *self);

//  Print contents of message to stdout
void
    xrap_msg_print (xrap_msg_t *self);

//  Get/set the message routing id
zframe_t *
    xrap_msg_routing_id (xrap_msg_t *self);
void
    xrap_msg_set_routing_id (xrap_msg_t *self, zframe_t *routing_id);

//  Get the xrap_msg id and printable command
int
    xrap_msg_id (xrap_msg_t *self);
void
    xrap_msg_set_id (xrap_msg_t *self, int id);
const char *
    xrap_msg_command (xrap_msg_t *self);

//  Get/set the parent field
const char *
    xrap_msg_parent (xrap_msg_t *self);
void
    xrap_msg_set_parent (xrap_msg_t *self, const char *format, ...);

//  Get/set the content_type field
const char *
    xrap_msg_content_type (xrap_msg_t *self);
void
    xrap_msg_set_content_type (xrap_msg_t *self, const char *format, ...);

//  Get/set the content_body field
const char *
    xrap_msg_content_body (xrap_msg_t *self);
void
    xrap_msg_set_content_body (xrap_msg_t *self, const char *format, ...);

//  Get/set the status_code field
uint16_t
    xrap_msg_status_code (xrap_msg_t *self);
void
    xrap_msg_set_status_code (xrap_msg_t *self, uint16_t status_code);

//  Get/set the location field
const char *
    xrap_msg_location (xrap_msg_t *self);
void
    xrap_msg_set_location (xrap_msg_t *self, const char *format, ...);

//  Get/set the etag field
const char *
    xrap_msg_etag (xrap_msg_t *self);
void
    xrap_msg_set_etag (xrap_msg_t *self, const char *format, ...);

//  Get/set the date_modified field
uint64_t
    xrap_msg_date_modified (xrap_msg_t *self);
void
    xrap_msg_set_date_modified (xrap_msg_t *self, uint64_t date_modified);

//  Get/set the resource field
const char *
    xrap_msg_resource (xrap_msg_t *self);
void
    xrap_msg_set_resource (xrap_msg_t *self, const char *format, ...);

//  Get/set the if_modified_since field
uint64_t
    xrap_msg_if_modified_since (xrap_msg_t *self);
void
    xrap_msg_set_if_modified_since (xrap_msg_t *self, uint64_t if_modified_since);

//  Get/set the if_none_match field
const char *
    xrap_msg_if_none_match (xrap_msg_t *self);
void
    xrap_msg_set_if_none_match (xrap_msg_t *self, const char *format, ...);

//  Get/set the if_unmodified_since field
uint64_t
    xrap_msg_if_unmodified_since (xrap_msg_t *self);
void
    xrap_msg_set_if_unmodified_since (xrap_msg_t *self, uint64_t if_unmodified_since);

//  Get/set the if_match field
const char *
    xrap_msg_if_match (xrap_msg_t *self);
void
    xrap_msg_set_if_match (xrap_msg_t *self, const char *format, ...);

//  Get/set the status_text field
const char *
    xrap_msg_status_text (xrap_msg_t *self);
void
    xrap_msg_set_status_text (xrap_msg_t *self, const char *format, ...);

//  Self test of this class
int
    xrap_msg_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define xrap_msg_dump       xrap_msg_print

#ifdef __cplusplus
}
#endif

#endif
