/*  =========================================================================
    xrap_msg - XRAP serialization over ZMTP

    Codec class for xrap_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: xrap_msg.xml, or
     * The code generation script that built this file: zproto_codec_c_v1
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
    xrap_msg - XRAP serialization over ZMTP
@discuss
@end
*/

#include "../include/xrap_msg.h"

//  Structure of our class

struct _xrap_msg_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  xrap_msg message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    char *parent;                       //  Schema/type/name
    char *content_type;                 //  Content type
    char *content_body;                 //  New resource specification
    uint16_t status_code;               //  Response status code 2xx
    char *location;                     //  Schema/type/name
    char *etag;                         //  Opaque hash tag
    uint64_t date_modified;             //  Date and time modified
    zhash_t *metadata;                  //  Collection total size/version/hypermedia
    size_t metadata_bytes;              //  Collection total size/version/hypermedia
    char *resource;                     //  Schema/type/name
    zhash_t *parameters;                //  Filtering/sorting/selecting/paging
    size_t parameters_bytes;            //  Filtering/sorting/selecting/paging
    uint64_t if_modified_since;         //  GET if more recent
    char *if_none_match;                //  GET if changed
    uint64_t if_unmodified_since;       //  Update if same date
    char *if_match;                     //  Update if same ETag
    char *status_text;                  //  Response status text
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
    if (self->needle + size > self->ceiling) \
        goto malformed; \
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
    if (self->needle + 1 > self->ceiling) \
        goto malformed; \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) \
        goto malformed; \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) \
        goto malformed; \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) \
        goto malformed; \
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
    if (self->needle + string_size > (self->ceiling)) \
        goto malformed; \
    (host) = (char *) malloc (string_size + 1); \
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
    if (self->needle + string_size > (self->ceiling)) \
        goto malformed; \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}


//  --------------------------------------------------------------------------
//  Create a new xrap_msg

xrap_msg_t *
xrap_msg_new (int id)
{
    xrap_msg_t *self = (xrap_msg_t *) zmalloc (sizeof (xrap_msg_t));
    self->id = id;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the xrap_msg

void
xrap_msg_destroy (xrap_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        xrap_msg_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        free (self->parent);
        free (self->content_type);
        free (self->content_body);
        free (self->location);
        free (self->etag);
        zhash_destroy (&self->metadata);
        free (self->resource);
        zhash_destroy (&self->parameters);
        free (self->if_none_match);
        free (self->if_match);
        free (self->status_text);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  Parse a zmsg_t and decides whether it is xrap_msg. Returns
//  true if it is, false otherwise. Doesn't destroy or modify the
//  original message.
bool
is_xrap_msg (zmsg_t *msg)
{
    if (msg == NULL)
        return false;

    zframe_t *frame = zmsg_first (msg);
    if (frame == NULL)
        return false;

    //  Get and check protocol signature
    xrap_msg_t *self = xrap_msg_new (0);
    self->needle = zframe_data (frame);
    self->ceiling = self->needle + zframe_size (frame);
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 5))
        goto fail;             //  Invalid signature

    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case XRAP_MSG_POST:
        case XRAP_MSG_POST_OK:
        case XRAP_MSG_GET:
        case XRAP_MSG_GET_OK:
        case XRAP_MSG_GET_EMPTY:
        case XRAP_MSG_PUT:
        case XRAP_MSG_PUT_OK:
        case XRAP_MSG_DELETE:
        case XRAP_MSG_DELETE_OK:
        case XRAP_MSG_ERROR:
            xrap_msg_destroy (&self);
            return true;
        default:
            goto fail;
    }
    fail:
    malformed:
        xrap_msg_destroy (&self);
        return false;
}

//  --------------------------------------------------------------------------
//  Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and
//  nullifies the msg reference.

xrap_msg_t *
xrap_msg_decode (zmsg_t **msg_p)
{
    assert (msg_p);
    zmsg_t *msg = *msg_p;
    if (msg == NULL)
        return NULL;

    xrap_msg_t *self = xrap_msg_new (0);
    //  Read and parse command in frame
    zframe_t *frame = zmsg_pop (msg);
    if (!frame)
        goto empty;             //  Malformed or empty

    //  Get and check protocol signature
    self->needle = zframe_data (frame);
    self->ceiling = self->needle + zframe_size (frame);
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 5))
        goto empty;             //  Invalid signature

    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case XRAP_MSG_POST:
            GET_STRING (self->parent);
            GET_STRING (self->content_type);
            GET_LONGSTR (self->content_body);
            break;

        case XRAP_MSG_POST_OK:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->location);
            GET_STRING (self->etag);
            GET_NUMBER8 (self->date_modified);
            GET_STRING (self->content_type);
            GET_LONGSTR (self->content_body);
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                self->metadata = zhash_new ();
                zhash_autofree (self->metadata);
                while (hash_size--) {
                    char *key, *value;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->metadata, key, value);
                    free (key);
                    free (value);
                }
            }
            break;

        case XRAP_MSG_GET:
            GET_STRING (self->resource);
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                self->parameters = zhash_new ();
                zhash_autofree (self->parameters);
                while (hash_size--) {
                    char *key, *value;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->parameters, key, value);
                    free (key);
                    free (value);
                }
            }
            GET_NUMBER8 (self->if_modified_since);
            GET_STRING (self->if_none_match);
            GET_STRING (self->content_type);
            break;

        case XRAP_MSG_GET_OK:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->etag);
            GET_NUMBER8 (self->date_modified);
            GET_STRING (self->content_type);
            GET_LONGSTR (self->content_body);
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                self->metadata = zhash_new ();
                zhash_autofree (self->metadata);
                while (hash_size--) {
                    char *key, *value;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->metadata, key, value);
                    free (key);
                    free (value);
                }
            }
            break;

        case XRAP_MSG_GET_EMPTY:
            GET_NUMBER2 (self->status_code);
            break;

        case XRAP_MSG_PUT:
            GET_STRING (self->resource);
            GET_NUMBER8 (self->if_unmodified_since);
            GET_STRING (self->if_match);
            GET_STRING (self->content_type);
            GET_LONGSTR (self->content_body);
            break;

        case XRAP_MSG_PUT_OK:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->location);
            GET_STRING (self->etag);
            GET_NUMBER8 (self->date_modified);
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                self->metadata = zhash_new ();
                zhash_autofree (self->metadata);
                while (hash_size--) {
                    char *key, *value;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->metadata, key, value);
                    free (key);
                    free (value);
                }
            }
            break;

        case XRAP_MSG_DELETE:
            GET_STRING (self->resource);
            GET_NUMBER8 (self->if_unmodified_since);
            GET_STRING (self->if_match);
            break;

        case XRAP_MSG_DELETE_OK:
            GET_NUMBER2 (self->status_code);
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                self->metadata = zhash_new ();
                zhash_autofree (self->metadata);
                while (hash_size--) {
                    char *key, *value;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->metadata, key, value);
                    free (key);
                    free (value);
                }
            }
            break;

        case XRAP_MSG_ERROR:
            GET_NUMBER2 (self->status_code);
            GET_STRING (self->status_text);
            break;

        default:
            goto malformed;
    }
    //  Successful return
    zframe_destroy (&frame);
    zmsg_destroy (msg_p);
    return self;

    //  Error returns
    malformed:
        zsys_error ("malformed message '%d'\n", self->id);
    empty:
        zframe_destroy (&frame);
        zmsg_destroy (msg_p);
        xrap_msg_destroy (&self);
        return (NULL);
}


//  --------------------------------------------------------------------------
//  Encode xrap_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.

zmsg_t *
xrap_msg_encode (xrap_msg_t **self_p)
{
    assert (self_p);
    assert (*self_p);

    xrap_msg_t *self = *self_p;
    zmsg_t *msg = zmsg_new ();

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case XRAP_MSG_POST:
            //  parent is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->parent)
                frame_size += strlen (self->parent);
            //  content_type is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->content_type)
                frame_size += strlen (self->content_type);
            //  content_body is a string with 4-byte length
            frame_size += 4;
            if (self->content_body)
                frame_size += strlen (self->content_body);
            break;

        case XRAP_MSG_POST_OK:
            //  status_code is a 2-byte integer
            frame_size += 2;
            //  location is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->location)
                frame_size += strlen (self->location);
            //  etag is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->etag)
                frame_size += strlen (self->etag);
            //  date_modified is a 8-byte integer
            frame_size += 8;
            //  content_type is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->content_type)
                frame_size += strlen (self->content_type);
            //  content_body is a string with 4-byte length
            frame_size += 4;
            if (self->content_body)
                frame_size += strlen (self->content_body);
            //  metadata is an array of key=value strings
            frame_size += 4;    //  Size is 4 octets
            if (self->metadata) {
                self->metadata_bytes = 0;
                //  Add up size of dictionary contents
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    self->metadata_bytes += 1 + strlen ((const char *) zhash_cursor (self->metadata));
                    self->metadata_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            frame_size += self->metadata_bytes;
            break;

        case XRAP_MSG_GET:
            //  resource is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->resource)
                frame_size += strlen (self->resource);
            //  parameters is an array of key=value strings
            frame_size += 4;    //  Size is 4 octets
            if (self->parameters) {
                self->parameters_bytes = 0;
                //  Add up size of dictionary contents
                char *item = (char *) zhash_first (self->parameters);
                while (item) {
                    self->parameters_bytes += 1 + strlen ((const char *) zhash_cursor (self->parameters));
                    self->parameters_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->parameters);
                }
            }
            frame_size += self->parameters_bytes;
            //  if_modified_since is a 8-byte integer
            frame_size += 8;
            //  if_none_match is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->if_none_match)
                frame_size += strlen (self->if_none_match);
            //  content_type is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->content_type)
                frame_size += strlen (self->content_type);
            break;

        case XRAP_MSG_GET_OK:
            //  status_code is a 2-byte integer
            frame_size += 2;
            //  etag is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->etag)
                frame_size += strlen (self->etag);
            //  date_modified is a 8-byte integer
            frame_size += 8;
            //  content_type is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->content_type)
                frame_size += strlen (self->content_type);
            //  content_body is a string with 4-byte length
            frame_size += 4;
            if (self->content_body)
                frame_size += strlen (self->content_body);
            //  metadata is an array of key=value strings
            frame_size += 4;    //  Size is 4 octets
            if (self->metadata) {
                self->metadata_bytes = 0;
                //  Add up size of dictionary contents
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    self->metadata_bytes += 1 + strlen ((const char *) zhash_cursor (self->metadata));
                    self->metadata_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            frame_size += self->metadata_bytes;
            break;

        case XRAP_MSG_GET_EMPTY:
            //  status_code is a 2-byte integer
            frame_size += 2;
            break;

        case XRAP_MSG_PUT:
            //  resource is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->resource)
                frame_size += strlen (self->resource);
            //  if_unmodified_since is a 8-byte integer
            frame_size += 8;
            //  if_match is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->if_match)
                frame_size += strlen (self->if_match);
            //  content_type is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->content_type)
                frame_size += strlen (self->content_type);
            //  content_body is a string with 4-byte length
            frame_size += 4;
            if (self->content_body)
                frame_size += strlen (self->content_body);
            break;

        case XRAP_MSG_PUT_OK:
            //  status_code is a 2-byte integer
            frame_size += 2;
            //  location is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->location)
                frame_size += strlen (self->location);
            //  etag is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->etag)
                frame_size += strlen (self->etag);
            //  date_modified is a 8-byte integer
            frame_size += 8;
            //  metadata is an array of key=value strings
            frame_size += 4;    //  Size is 4 octets
            if (self->metadata) {
                self->metadata_bytes = 0;
                //  Add up size of dictionary contents
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    self->metadata_bytes += 1 + strlen ((const char *) zhash_cursor (self->metadata));
                    self->metadata_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            frame_size += self->metadata_bytes;
            break;

        case XRAP_MSG_DELETE:
            //  resource is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->resource)
                frame_size += strlen (self->resource);
            //  if_unmodified_since is a 8-byte integer
            frame_size += 8;
            //  if_match is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->if_match)
                frame_size += strlen (self->if_match);
            break;

        case XRAP_MSG_DELETE_OK:
            //  status_code is a 2-byte integer
            frame_size += 2;
            //  metadata is an array of key=value strings
            frame_size += 4;    //  Size is 4 octets
            if (self->metadata) {
                self->metadata_bytes = 0;
                //  Add up size of dictionary contents
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    self->metadata_bytes += 1 + strlen ((const char *) zhash_cursor (self->metadata));
                    self->metadata_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            frame_size += self->metadata_bytes;
            break;

        case XRAP_MSG_ERROR:
            //  status_code is a 2-byte integer
            frame_size += 2;
            //  status_text is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->status_text)
                frame_size += strlen (self->status_text);
            break;

        default:
            zsys_error ("bad message type '%d', not sent\n", self->id);
            //  No recovery, this is a fatal application error
            assert (false);
    }
    //  Now serialize message into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    self->needle = zframe_data (frame);
    PUT_NUMBER2 (0xAAA0 | 5);
    PUT_NUMBER1 (self->id);

    switch (self->id) {
        case XRAP_MSG_POST:
            if (self->parent) {
                PUT_STRING (self->parent);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_type) {
                PUT_STRING (self->content_type);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_body) {
                PUT_LONGSTR (self->content_body);
            }
            else
                PUT_NUMBER4 (0);    //  Empty string
            break;

        case XRAP_MSG_POST_OK:
            PUT_NUMBER2 (self->status_code);
            if (self->location) {
                PUT_STRING (self->location);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->etag) {
                PUT_STRING (self->etag);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            PUT_NUMBER8 (self->date_modified);
            if (self->content_type) {
                PUT_STRING (self->content_type);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_body) {
                PUT_LONGSTR (self->content_body);
            }
            else
                PUT_NUMBER4 (0);    //  Empty string
            if (self->metadata) {
                PUT_NUMBER4 (zhash_size (self->metadata));
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    PUT_STRING ((const char *) zhash_cursor ((zhash_t *) self->metadata));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty dictionary
            break;

        case XRAP_MSG_GET:
            if (self->resource) {
                PUT_STRING (self->resource);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->parameters) {
                PUT_NUMBER4 (zhash_size (self->parameters));
                char *item = (char *) zhash_first (self->parameters);
                while (item) {
                    PUT_STRING ((const char *) zhash_cursor ((zhash_t *) self->parameters));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->parameters);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty dictionary
            PUT_NUMBER8 (self->if_modified_since);
            if (self->if_none_match) {
                PUT_STRING (self->if_none_match);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_type) {
                PUT_STRING (self->content_type);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            break;

        case XRAP_MSG_GET_OK:
            PUT_NUMBER2 (self->status_code);
            if (self->etag) {
                PUT_STRING (self->etag);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            PUT_NUMBER8 (self->date_modified);
            if (self->content_type) {
                PUT_STRING (self->content_type);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_body) {
                PUT_LONGSTR (self->content_body);
            }
            else
                PUT_NUMBER4 (0);    //  Empty string
            if (self->metadata) {
                PUT_NUMBER4 (zhash_size (self->metadata));
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    PUT_STRING ((const char *) zhash_cursor ((zhash_t *) self->metadata));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty dictionary
            break;

        case XRAP_MSG_GET_EMPTY:
            PUT_NUMBER2 (self->status_code);
            break;

        case XRAP_MSG_PUT:
            if (self->resource) {
                PUT_STRING (self->resource);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            PUT_NUMBER8 (self->if_unmodified_since);
            if (self->if_match) {
                PUT_STRING (self->if_match);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_type) {
                PUT_STRING (self->content_type);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->content_body) {
                PUT_LONGSTR (self->content_body);
            }
            else
                PUT_NUMBER4 (0);    //  Empty string
            break;

        case XRAP_MSG_PUT_OK:
            PUT_NUMBER2 (self->status_code);
            if (self->location) {
                PUT_STRING (self->location);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->etag) {
                PUT_STRING (self->etag);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            PUT_NUMBER8 (self->date_modified);
            if (self->metadata) {
                PUT_NUMBER4 (zhash_size (self->metadata));
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    PUT_STRING ((const char *) zhash_cursor ((zhash_t *) self->metadata));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty dictionary
            break;

        case XRAP_MSG_DELETE:
            if (self->resource) {
                PUT_STRING (self->resource);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            PUT_NUMBER8 (self->if_unmodified_since);
            if (self->if_match) {
                PUT_STRING (self->if_match);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            break;

        case XRAP_MSG_DELETE_OK:
            PUT_NUMBER2 (self->status_code);
            if (self->metadata) {
                PUT_NUMBER4 (zhash_size (self->metadata));
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    PUT_STRING ((const char *) zhash_cursor ((zhash_t *) self->metadata));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty dictionary
            break;

        case XRAP_MSG_ERROR:
            PUT_NUMBER2 (self->status_code);
            if (self->status_text) {
                PUT_STRING (self->status_text);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            break;

    }
    //  Now send the data frame
    if (zmsg_append (msg, &frame)) {
        zmsg_destroy (&msg);
        xrap_msg_destroy (self_p);
        return NULL;
    }
    //  Destroy xrap_msg object
    xrap_msg_destroy (self_p);
    return msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a xrap_msg from the socket. Returns new object or
//  NULL if error. Will block if there's no message waiting.

xrap_msg_t *
xrap_msg_recv (void *input)
{
    assert (input);
    zmsg_t *msg = zmsg_recv (input);
    if (!msg)
        return NULL;            //  Interrupted

    //  If message came from a router socket, first frame is routing_id
    zframe_t *routing_id = NULL;
    if (zsock_type (zsock_resolve (input)) == ZMQ_ROUTER) {
        routing_id = zmsg_pop (msg);
        //  If message was not valid, forget about it
        if (!routing_id || !zmsg_next (msg))
            return NULL;        //  Malformed or empty
    }
    xrap_msg_t *xrap_msg = xrap_msg_decode (&msg);
    if (xrap_msg && zsock_type (zsock_resolve (input)) == ZMQ_ROUTER)
        xrap_msg->routing_id = routing_id;

    return xrap_msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a xrap_msg from the socket. Returns new object,
//  or NULL either if there was no input waiting, or the recv was interrupted.

xrap_msg_t *
xrap_msg_recv_nowait (void *input)
{
    assert (input);
    zmsg_t *msg = zmsg_recv_nowait (input);
    if (!msg)
        return NULL;            //  Interrupted
    //  If message came from a router socket, first frame is routing_id
    zframe_t *routing_id = NULL;
    if (zsock_type (zsock_resolve (input)) == ZMQ_ROUTER) {
        routing_id = zmsg_pop (msg);
        //  If message was not valid, forget about it
        if (!routing_id || !zmsg_next (msg))
            return NULL;        //  Malformed or empty
    }
    xrap_msg_t *xrap_msg = xrap_msg_decode (&msg);
    if (xrap_msg && zsock_type (zsock_resolve (input)) == ZMQ_ROUTER)
        xrap_msg->routing_id = routing_id;

    return xrap_msg;
}


//  --------------------------------------------------------------------------
//  Send the xrap_msg to the socket, and destroy it
//  Returns 0 if OK, else -1

int
xrap_msg_send (xrap_msg_t **self_p, void *output)
{
    assert (self_p);
    assert (*self_p);
    assert (output);

    //  Save routing_id if any, as encode will destroy it
    xrap_msg_t *self = *self_p;
    zframe_t *routing_id = self->routing_id;
    self->routing_id = NULL;

    //  Encode xrap_msg message to a single zmsg
    zmsg_t *msg = xrap_msg_encode (self_p);

    //  If we're sending to a ROUTER, send the routing_id first
    if (zsock_type (zsock_resolve (output)) == ZMQ_ROUTER) {
        assert (routing_id);
        zmsg_prepend (msg, &routing_id);
    }
    else
        zframe_destroy (&routing_id);

    if (msg && zmsg_send (&msg, output) == 0)
        return 0;
    else
        return -1;              //  Failed to encode, or send
}


//  --------------------------------------------------------------------------
//  Send the xrap_msg to the output, and do not destroy it

int
xrap_msg_send_again (xrap_msg_t *self, void *output)
{
    assert (self);
    assert (output);
    self = xrap_msg_dup (self);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Encode POST message

zmsg_t *
xrap_msg_encode_post (
    const char *parent,
    const char *content_type,
    const char *content_body)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_POST);
    xrap_msg_set_parent (self, "%s", parent);
    xrap_msg_set_content_type (self, "%s", content_type);
    xrap_msg_set_content_body (self, "%s", content_body);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode POST_OK message

zmsg_t *
xrap_msg_encode_post_ok (
    uint16_t status_code,
    const char *location,
    const char *etag,
    uint64_t date_modified,
    const char *content_type,
    const char *content_body,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_POST_OK);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_location (self, "%s", location);
    xrap_msg_set_etag (self, "%s", etag);
    xrap_msg_set_date_modified (self, date_modified);
    xrap_msg_set_content_type (self, "%s", content_type);
    xrap_msg_set_content_body (self, "%s", content_body);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode GET message

zmsg_t *
xrap_msg_encode_get (
    const char *resource,
    zhash_t *parameters,
    uint64_t if_modified_since,
    const char *if_none_match,
    const char *content_type)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (self, "%s", resource);
    zhash_t *parameters_copy = zhash_dup (parameters);
    xrap_msg_set_parameters (self, &parameters_copy);
    xrap_msg_set_if_modified_since (self, if_modified_since);
    xrap_msg_set_if_none_match (self, "%s", if_none_match);
    xrap_msg_set_content_type (self, "%s", content_type);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode GET_OK message

zmsg_t *
xrap_msg_encode_get_ok (
    uint16_t status_code,
    const char *etag,
    uint64_t date_modified,
    const char *content_type,
    const char *content_body,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_GET_OK);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_etag (self, "%s", etag);
    xrap_msg_set_date_modified (self, date_modified);
    xrap_msg_set_content_type (self, "%s", content_type);
    xrap_msg_set_content_body (self, "%s", content_body);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode GET_EMPTY message

zmsg_t *
xrap_msg_encode_get_empty (
    uint16_t status_code)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_GET_EMPTY);
    xrap_msg_set_status_code (self, status_code);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode PUT message

zmsg_t *
xrap_msg_encode_put (
    const char *resource,
    uint64_t if_unmodified_since,
    const char *if_match,
    const char *content_type,
    const char *content_body)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (self, "%s", resource);
    xrap_msg_set_if_unmodified_since (self, if_unmodified_since);
    xrap_msg_set_if_match (self, "%s", if_match);
    xrap_msg_set_content_type (self, "%s", content_type);
    xrap_msg_set_content_body (self, "%s", content_body);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode PUT_OK message

zmsg_t *
xrap_msg_encode_put_ok (
    uint16_t status_code,
    const char *location,
    const char *etag,
    uint64_t date_modified,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_PUT_OK);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_location (self, "%s", location);
    xrap_msg_set_etag (self, "%s", etag);
    xrap_msg_set_date_modified (self, date_modified);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode DELETE message

zmsg_t *
xrap_msg_encode_delete (
    const char *resource,
    uint64_t if_unmodified_since,
    const char *if_match)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_DELETE);
    xrap_msg_set_resource (self, "%s", resource);
    xrap_msg_set_if_unmodified_since (self, if_unmodified_since);
    xrap_msg_set_if_match (self, "%s", if_match);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode DELETE_OK message

zmsg_t *
xrap_msg_encode_delete_ok (
    uint16_t status_code,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_DELETE_OK);
    xrap_msg_set_status_code (self, status_code);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode ERROR message

zmsg_t *
xrap_msg_encode_error (
    uint16_t status_code,
    const char *status_text)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_ERROR);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_status_text (self, "%s", status_text);
    return xrap_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Send the POST to the socket in one step

int
xrap_msg_send_post (
    void *output,
    const char *parent,
    const char *content_type,
    const char *content_body)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_POST);
    xrap_msg_set_parent (self, parent);
    xrap_msg_set_content_type (self, content_type);
    xrap_msg_set_content_body (self, content_body);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the POST_OK to the socket in one step

int
xrap_msg_send_post_ok (
    void *output,
    uint16_t status_code,
    const char *location,
    const char *etag,
    uint64_t date_modified,
    const char *content_type,
    const char *content_body,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_POST_OK);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_location (self, location);
    xrap_msg_set_etag (self, etag);
    xrap_msg_set_date_modified (self, date_modified);
    xrap_msg_set_content_type (self, content_type);
    xrap_msg_set_content_body (self, content_body);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the GET to the socket in one step

int
xrap_msg_send_get (
    void *output,
    const char *resource,
    zhash_t *parameters,
    uint64_t if_modified_since,
    const char *if_none_match,
    const char *content_type)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_GET);
    xrap_msg_set_resource (self, resource);
    zhash_t *parameters_copy = zhash_dup (parameters);
    xrap_msg_set_parameters (self, &parameters_copy);
    xrap_msg_set_if_modified_since (self, if_modified_since);
    xrap_msg_set_if_none_match (self, if_none_match);
    xrap_msg_set_content_type (self, content_type);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the GET_OK to the socket in one step

int
xrap_msg_send_get_ok (
    void *output,
    uint16_t status_code,
    const char *etag,
    uint64_t date_modified,
    const char *content_type,
    const char *content_body,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_GET_OK);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_etag (self, etag);
    xrap_msg_set_date_modified (self, date_modified);
    xrap_msg_set_content_type (self, content_type);
    xrap_msg_set_content_body (self, content_body);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the GET_EMPTY to the socket in one step

int
xrap_msg_send_get_empty (
    void *output,
    uint16_t status_code)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_GET_EMPTY);
    xrap_msg_set_status_code (self, status_code);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the PUT to the socket in one step

int
xrap_msg_send_put (
    void *output,
    const char *resource,
    uint64_t if_unmodified_since,
    const char *if_match,
    const char *content_type,
    const char *content_body)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_PUT);
    xrap_msg_set_resource (self, resource);
    xrap_msg_set_if_unmodified_since (self, if_unmodified_since);
    xrap_msg_set_if_match (self, if_match);
    xrap_msg_set_content_type (self, content_type);
    xrap_msg_set_content_body (self, content_body);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the PUT_OK to the socket in one step

int
xrap_msg_send_put_ok (
    void *output,
    uint16_t status_code,
    const char *location,
    const char *etag,
    uint64_t date_modified,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_PUT_OK);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_location (self, location);
    xrap_msg_set_etag (self, etag);
    xrap_msg_set_date_modified (self, date_modified);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the DELETE to the socket in one step

int
xrap_msg_send_delete (
    void *output,
    const char *resource,
    uint64_t if_unmodified_since,
    const char *if_match)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_DELETE);
    xrap_msg_set_resource (self, resource);
    xrap_msg_set_if_unmodified_since (self, if_unmodified_since);
    xrap_msg_set_if_match (self, if_match);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the DELETE_OK to the socket in one step

int
xrap_msg_send_delete_ok (
    void *output,
    uint16_t status_code,
    zhash_t *metadata)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_DELETE_OK);
    xrap_msg_set_status_code (self, status_code);
    zhash_t *metadata_copy = zhash_dup (metadata);
    xrap_msg_set_metadata (self, &metadata_copy);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the ERROR to the socket in one step

int
xrap_msg_send_error (
    void *output,
    uint16_t status_code,
    const char *status_text)
{
    xrap_msg_t *self = xrap_msg_new (XRAP_MSG_ERROR);
    xrap_msg_set_status_code (self, status_code);
    xrap_msg_set_status_text (self, status_text);
    return xrap_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Duplicate the xrap_msg message

xrap_msg_t *
xrap_msg_dup (xrap_msg_t *self)
{
    if (!self)
        return NULL;

    xrap_msg_t *copy = xrap_msg_new (self->id);
    if (self->routing_id)
        copy->routing_id = zframe_dup (self->routing_id);
    switch (self->id) {
        case XRAP_MSG_POST:
            copy->parent = self->parent? strdup (self->parent): NULL;
            copy->content_type = self->content_type? strdup (self->content_type): NULL;
            copy->content_body = self->content_body? strdup (self->content_body): NULL;
            break;

        case XRAP_MSG_POST_OK:
            copy->status_code = self->status_code;
            copy->location = self->location? strdup (self->location): NULL;
            copy->etag = self->etag? strdup (self->etag): NULL;
            copy->date_modified = self->date_modified;
            copy->content_type = self->content_type? strdup (self->content_type): NULL;
            copy->content_body = self->content_body? strdup (self->content_body): NULL;
            copy->metadata = self->metadata? zhash_dup (self->metadata): NULL;
            break;

        case XRAP_MSG_GET:
            copy->resource = self->resource? strdup (self->resource): NULL;
            copy->parameters = self->parameters? zhash_dup (self->parameters): NULL;
            copy->if_modified_since = self->if_modified_since;
            copy->if_none_match = self->if_none_match? strdup (self->if_none_match): NULL;
            copy->content_type = self->content_type? strdup (self->content_type): NULL;
            break;

        case XRAP_MSG_GET_OK:
            copy->status_code = self->status_code;
            copy->etag = self->etag? strdup (self->etag): NULL;
            copy->date_modified = self->date_modified;
            copy->content_type = self->content_type? strdup (self->content_type): NULL;
            copy->content_body = self->content_body? strdup (self->content_body): NULL;
            copy->metadata = self->metadata? zhash_dup (self->metadata): NULL;
            break;

        case XRAP_MSG_GET_EMPTY:
            copy->status_code = self->status_code;
            break;

        case XRAP_MSG_PUT:
            copy->resource = self->resource? strdup (self->resource): NULL;
            copy->if_unmodified_since = self->if_unmodified_since;
            copy->if_match = self->if_match? strdup (self->if_match): NULL;
            copy->content_type = self->content_type? strdup (self->content_type): NULL;
            copy->content_body = self->content_body? strdup (self->content_body): NULL;
            break;

        case XRAP_MSG_PUT_OK:
            copy->status_code = self->status_code;
            copy->location = self->location? strdup (self->location): NULL;
            copy->etag = self->etag? strdup (self->etag): NULL;
            copy->date_modified = self->date_modified;
            copy->metadata = self->metadata? zhash_dup (self->metadata): NULL;
            break;

        case XRAP_MSG_DELETE:
            copy->resource = self->resource? strdup (self->resource): NULL;
            copy->if_unmodified_since = self->if_unmodified_since;
            copy->if_match = self->if_match? strdup (self->if_match): NULL;
            break;

        case XRAP_MSG_DELETE_OK:
            copy->status_code = self->status_code;
            copy->metadata = self->metadata? zhash_dup (self->metadata): NULL;
            break;

        case XRAP_MSG_ERROR:
            copy->status_code = self->status_code;
            copy->status_text = self->status_text? strdup (self->status_text): NULL;
            break;

    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
xrap_msg_print (xrap_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case XRAP_MSG_POST:
            zsys_debug ("XRAP_MSG_POST:");
            if (self->parent)
                zsys_debug ("    parent='%s'", self->parent);
            else
                zsys_debug ("    parent=");
            if (self->content_type)
                zsys_debug ("    content_type='%s'", self->content_type);
            else
                zsys_debug ("    content_type=");
            if (self->content_body)
                zsys_debug ("    content_body='%s'", self->content_body);
            else
                zsys_debug ("    content_body=");
            break;

        case XRAP_MSG_POST_OK:
            zsys_debug ("XRAP_MSG_POST_OK:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            if (self->location)
                zsys_debug ("    location='%s'", self->location);
            else
                zsys_debug ("    location=");
            if (self->etag)
                zsys_debug ("    etag='%s'", self->etag);
            else
                zsys_debug ("    etag=");
            zsys_debug ("    date_modified=%ld", (long) self->date_modified);
            if (self->content_type)
                zsys_debug ("    content_type='%s'", self->content_type);
            else
                zsys_debug ("    content_type=");
            if (self->content_body)
                zsys_debug ("    content_body='%s'", self->content_body);
            else
                zsys_debug ("    content_body=");
            zsys_debug ("    metadata=");
            if (self->metadata) {
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->metadata), item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                zsys_debug ("(NULL)");
            break;

        case XRAP_MSG_GET:
            zsys_debug ("XRAP_MSG_GET:");
            if (self->resource)
                zsys_debug ("    resource='%s'", self->resource);
            else
                zsys_debug ("    resource=");
            zsys_debug ("    parameters=");
            if (self->parameters) {
                char *item = (char *) zhash_first (self->parameters);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->parameters), item);
                    item = (char *) zhash_next (self->parameters);
                }
            }
            else
                zsys_debug ("(NULL)");
            zsys_debug ("    if_modified_since=%ld", (long) self->if_modified_since);
            if (self->if_none_match)
                zsys_debug ("    if_none_match='%s'", self->if_none_match);
            else
                zsys_debug ("    if_none_match=");
            if (self->content_type)
                zsys_debug ("    content_type='%s'", self->content_type);
            else
                zsys_debug ("    content_type=");
            break;

        case XRAP_MSG_GET_OK:
            zsys_debug ("XRAP_MSG_GET_OK:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            if (self->etag)
                zsys_debug ("    etag='%s'", self->etag);
            else
                zsys_debug ("    etag=");
            zsys_debug ("    date_modified=%ld", (long) self->date_modified);
            if (self->content_type)
                zsys_debug ("    content_type='%s'", self->content_type);
            else
                zsys_debug ("    content_type=");
            if (self->content_body)
                zsys_debug ("    content_body='%s'", self->content_body);
            else
                zsys_debug ("    content_body=");
            zsys_debug ("    metadata=");
            if (self->metadata) {
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->metadata), item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                zsys_debug ("(NULL)");
            break;

        case XRAP_MSG_GET_EMPTY:
            zsys_debug ("XRAP_MSG_GET_EMPTY:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            break;

        case XRAP_MSG_PUT:
            zsys_debug ("XRAP_MSG_PUT:");
            if (self->resource)
                zsys_debug ("    resource='%s'", self->resource);
            else
                zsys_debug ("    resource=");
            zsys_debug ("    if_unmodified_since=%ld", (long) self->if_unmodified_since);
            if (self->if_match)
                zsys_debug ("    if_match='%s'", self->if_match);
            else
                zsys_debug ("    if_match=");
            if (self->content_type)
                zsys_debug ("    content_type='%s'", self->content_type);
            else
                zsys_debug ("    content_type=");
            if (self->content_body)
                zsys_debug ("    content_body='%s'", self->content_body);
            else
                zsys_debug ("    content_body=");
            break;

        case XRAP_MSG_PUT_OK:
            zsys_debug ("XRAP_MSG_PUT_OK:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            if (self->location)
                zsys_debug ("    location='%s'", self->location);
            else
                zsys_debug ("    location=");
            if (self->etag)
                zsys_debug ("    etag='%s'", self->etag);
            else
                zsys_debug ("    etag=");
            zsys_debug ("    date_modified=%ld", (long) self->date_modified);
            zsys_debug ("    metadata=");
            if (self->metadata) {
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->metadata), item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                zsys_debug ("(NULL)");
            break;

        case XRAP_MSG_DELETE:
            zsys_debug ("XRAP_MSG_DELETE:");
            if (self->resource)
                zsys_debug ("    resource='%s'", self->resource);
            else
                zsys_debug ("    resource=");
            zsys_debug ("    if_unmodified_since=%ld", (long) self->if_unmodified_since);
            if (self->if_match)
                zsys_debug ("    if_match='%s'", self->if_match);
            else
                zsys_debug ("    if_match=");
            break;

        case XRAP_MSG_DELETE_OK:
            zsys_debug ("XRAP_MSG_DELETE_OK:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            zsys_debug ("    metadata=");
            if (self->metadata) {
                char *item = (char *) zhash_first (self->metadata);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->metadata), item);
                    item = (char *) zhash_next (self->metadata);
                }
            }
            else
                zsys_debug ("(NULL)");
            break;

        case XRAP_MSG_ERROR:
            zsys_debug ("XRAP_MSG_ERROR:");
            zsys_debug ("    status_code=%ld", (long) self->status_code);
            if (self->status_text)
                zsys_debug ("    status_text='%s'", self->status_text);
            else
                zsys_debug ("    status_text=");
            break;

    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
xrap_msg_routing_id (xrap_msg_t *self)
{
    assert (self);
    return self->routing_id;
}

void
xrap_msg_set_routing_id (xrap_msg_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the xrap_msg id

int
xrap_msg_id (xrap_msg_t *self)
{
    assert (self);
    return self->id;
}

void
xrap_msg_set_id (xrap_msg_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
xrap_msg_command (xrap_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case XRAP_MSG_POST:
            return ("POST");
            break;
        case XRAP_MSG_POST_OK:
            return ("POST_OK");
            break;
        case XRAP_MSG_GET:
            return ("GET");
            break;
        case XRAP_MSG_GET_OK:
            return ("GET_OK");
            break;
        case XRAP_MSG_GET_EMPTY:
            return ("GET_EMPTY");
            break;
        case XRAP_MSG_PUT:
            return ("PUT");
            break;
        case XRAP_MSG_PUT_OK:
            return ("PUT_OK");
            break;
        case XRAP_MSG_DELETE:
            return ("DELETE");
            break;
        case XRAP_MSG_DELETE_OK:
            return ("DELETE_OK");
            break;
        case XRAP_MSG_ERROR:
            return ("ERROR");
            break;
    }
    return "?";
}

//  --------------------------------------------------------------------------
//  Get/set the parent field

const char *
xrap_msg_parent (xrap_msg_t *self)
{
    assert (self);
    return self->parent;
}

void
xrap_msg_set_parent (xrap_msg_t *self, const char *format, ...)
{
    //  Format parent from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->parent);
    self->parent = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the content_type field

const char *
xrap_msg_content_type (xrap_msg_t *self)
{
    assert (self);
    return self->content_type;
}

void
xrap_msg_set_content_type (xrap_msg_t *self, const char *format, ...)
{
    //  Format content_type from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->content_type);
    self->content_type = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the content_body field

const char *
xrap_msg_content_body (xrap_msg_t *self)
{
    assert (self);
    return self->content_body;
}

void
xrap_msg_set_content_body (xrap_msg_t *self, const char *format, ...)
{
    //  Format content_body from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->content_body);
    self->content_body = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the status_code field

uint16_t
xrap_msg_status_code (xrap_msg_t *self)
{
    assert (self);
    return self->status_code;
}

void
xrap_msg_set_status_code (xrap_msg_t *self, uint16_t status_code)
{
    assert (self);
    self->status_code = status_code;
}


//  --------------------------------------------------------------------------
//  Get/set the location field

const char *
xrap_msg_location (xrap_msg_t *self)
{
    assert (self);
    return self->location;
}

void
xrap_msg_set_location (xrap_msg_t *self, const char *format, ...)
{
    //  Format location from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->location);
    self->location = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the etag field

const char *
xrap_msg_etag (xrap_msg_t *self)
{
    assert (self);
    return self->etag;
}

void
xrap_msg_set_etag (xrap_msg_t *self, const char *format, ...)
{
    //  Format etag from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->etag);
    self->etag = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the date_modified field

uint64_t
xrap_msg_date_modified (xrap_msg_t *self)
{
    assert (self);
    return self->date_modified;
}

void
xrap_msg_set_date_modified (xrap_msg_t *self, uint64_t date_modified)
{
    assert (self);
    self->date_modified = date_modified;
}


//  --------------------------------------------------------------------------
//  Get the metadata field without transferring ownership

zhash_t *
xrap_msg_metadata (xrap_msg_t *self)
{
    assert (self);
    return self->metadata;
}

//  Get the metadata field and transfer ownership to caller

zhash_t *
xrap_msg_get_metadata (xrap_msg_t *self)
{
    zhash_t *metadata = self->metadata;
    self->metadata = NULL;
    return metadata;
}

//  Set the metadata field, transferring ownership from caller

void
xrap_msg_set_metadata (xrap_msg_t *self, zhash_t **metadata_p)
{
    assert (self);
    assert (metadata_p);
    zhash_destroy (&self->metadata);
    self->metadata = *metadata_p;
    *metadata_p = NULL;
}

//  --------------------------------------------------------------------------
//  Get/set a value in the metadata dictionary

const char *
xrap_msg_metadata_string (xrap_msg_t *self, const char *key, const char *default_value)
{
    assert (self);
    const char *value = NULL;
    if (self->metadata)
        value = (const char *) (zhash_lookup (self->metadata, key));
    if (!value)
        value = default_value;

    return value;
}

uint64_t
xrap_msg_metadata_number (xrap_msg_t *self, const char *key, uint64_t default_value)
{
    assert (self);
    uint64_t value = default_value;
    char *string = NULL;
    if (self->metadata)
        string = (char *) (zhash_lookup (self->metadata, key));
    if (string)
        value = atol (string);

    return value;
}

void
xrap_msg_metadata_insert (xrap_msg_t *self, const char *key, const char *format, ...)
{
    //  Format into newly allocated string
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);

    //  Store string in hash table
    if (!self->metadata) {
        self->metadata = zhash_new ();
        zhash_autofree (self->metadata);
    }
    zhash_update (self->metadata, key, string);
    free (string);
}

size_t
xrap_msg_metadata_size (xrap_msg_t *self)
{
    return zhash_size (self->metadata);
}


//  --------------------------------------------------------------------------
//  Get/set the resource field

const char *
xrap_msg_resource (xrap_msg_t *self)
{
    assert (self);
    return self->resource;
}

void
xrap_msg_set_resource (xrap_msg_t *self, const char *format, ...)
{
    //  Format resource from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->resource);
    self->resource = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get the parameters field without transferring ownership

zhash_t *
xrap_msg_parameters (xrap_msg_t *self)
{
    assert (self);
    return self->parameters;
}

//  Get the parameters field and transfer ownership to caller

zhash_t *
xrap_msg_get_parameters (xrap_msg_t *self)
{
    zhash_t *parameters = self->parameters;
    self->parameters = NULL;
    return parameters;
}

//  Set the parameters field, transferring ownership from caller

void
xrap_msg_set_parameters (xrap_msg_t *self, zhash_t **parameters_p)
{
    assert (self);
    assert (parameters_p);
    zhash_destroy (&self->parameters);
    self->parameters = *parameters_p;
    *parameters_p = NULL;
}

//  --------------------------------------------------------------------------
//  Get/set a value in the parameters dictionary

const char *
xrap_msg_parameters_string (xrap_msg_t *self, const char *key, const char *default_value)
{
    assert (self);
    const char *value = NULL;
    if (self->parameters)
        value = (const char *) (zhash_lookup (self->parameters, key));
    if (!value)
        value = default_value;

    return value;
}

uint64_t
xrap_msg_parameters_number (xrap_msg_t *self, const char *key, uint64_t default_value)
{
    assert (self);
    uint64_t value = default_value;
    char *string = NULL;
    if (self->parameters)
        string = (char *) (zhash_lookup (self->parameters, key));
    if (string)
        value = atol (string);

    return value;
}

void
xrap_msg_parameters_insert (xrap_msg_t *self, const char *key, const char *format, ...)
{
    //  Format into newly allocated string
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);

    //  Store string in hash table
    if (!self->parameters) {
        self->parameters = zhash_new ();
        zhash_autofree (self->parameters);
    }
    zhash_update (self->parameters, key, string);
    free (string);
}

size_t
xrap_msg_parameters_size (xrap_msg_t *self)
{
    return zhash_size (self->parameters);
}


//  --------------------------------------------------------------------------
//  Get/set the if_modified_since field

uint64_t
xrap_msg_if_modified_since (xrap_msg_t *self)
{
    assert (self);
    return self->if_modified_since;
}

void
xrap_msg_set_if_modified_since (xrap_msg_t *self, uint64_t if_modified_since)
{
    assert (self);
    self->if_modified_since = if_modified_since;
}


//  --------------------------------------------------------------------------
//  Get/set the if_none_match field

const char *
xrap_msg_if_none_match (xrap_msg_t *self)
{
    assert (self);
    return self->if_none_match;
}

void
xrap_msg_set_if_none_match (xrap_msg_t *self, const char *format, ...)
{
    //  Format if_none_match from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->if_none_match);
    self->if_none_match = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the if_unmodified_since field

uint64_t
xrap_msg_if_unmodified_since (xrap_msg_t *self)
{
    assert (self);
    return self->if_unmodified_since;
}

void
xrap_msg_set_if_unmodified_since (xrap_msg_t *self, uint64_t if_unmodified_since)
{
    assert (self);
    self->if_unmodified_since = if_unmodified_since;
}


//  --------------------------------------------------------------------------
//  Get/set the if_match field

const char *
xrap_msg_if_match (xrap_msg_t *self)
{
    assert (self);
    return self->if_match;
}

void
xrap_msg_set_if_match (xrap_msg_t *self, const char *format, ...)
{
    //  Format if_match from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->if_match);
    self->if_match = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the status_text field

const char *
xrap_msg_status_text (xrap_msg_t *self)
{
    assert (self);
    return self->status_text;
}

void
xrap_msg_set_status_text (xrap_msg_t *self, const char *format, ...)
{
    //  Format status_text from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->status_text);
    self->status_text = zsys_vprintf (format, argptr);
    va_end (argptr);
}



//  --------------------------------------------------------------------------
//  Selftest

void
xrap_msg_test (bool verbose)
{
    printf (" * xrap_msg: ");

    //  Silence an "unused" warning by "using" the verbose variable
    if (verbose) {;}

    //  @selftest
    //  Simple create/destroy test
    xrap_msg_t *self = xrap_msg_new (0);
    assert (self);
    xrap_msg_destroy (&self);

    //  Create pair of sockets we can send through
    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    zsock_connect (input, "inproc://selftest-xrap_msg");

    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    zsock_bind (output, "inproc://selftest-xrap_msg");

    //  Encode/send/decode and verify each message type
    int instance;
    xrap_msg_t *copy;
    self = xrap_msg_new (XRAP_MSG_POST);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_parent (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_type (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_body (self, "Life is short but Now lasts for ever");
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (streq (xrap_msg_parent (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_type (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_body (self), "Life is short but Now lasts for ever"));
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_POST_OK);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_status_code (self, 123);
    xrap_msg_set_location (self, "Life is short but Now lasts for ever");
    xrap_msg_set_etag (self, "Life is short but Now lasts for ever");
    xrap_msg_set_date_modified (self, 123);
    xrap_msg_set_content_type (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_body (self, "Life is short but Now lasts for ever");
    xrap_msg_metadata_insert (self, "Name", "Brutus");
    xrap_msg_metadata_insert (self, "Age", "%d", 43);
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (xrap_msg_status_code (self) == 123);
        assert (streq (xrap_msg_location (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_etag (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_date_modified (self) == 123);
        assert (streq (xrap_msg_content_type (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_body (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_metadata_size (self) == 2);
        assert (streq (xrap_msg_metadata_string (self, "Name", "?"), "Brutus"));
        assert (xrap_msg_metadata_number (self, "Age", 0) == 43);
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_GET);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_resource (self, "Life is short but Now lasts for ever");
    xrap_msg_parameters_insert (self, "Name", "Brutus");
    xrap_msg_parameters_insert (self, "Age", "%d", 43);
    xrap_msg_set_if_modified_since (self, 123);
    xrap_msg_set_if_none_match (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_type (self, "Life is short but Now lasts for ever");
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (streq (xrap_msg_resource (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_parameters_size (self) == 2);
        assert (streq (xrap_msg_parameters_string (self, "Name", "?"), "Brutus"));
        assert (xrap_msg_parameters_number (self, "Age", 0) == 43);
        assert (xrap_msg_if_modified_since (self) == 123);
        assert (streq (xrap_msg_if_none_match (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_type (self), "Life is short but Now lasts for ever"));
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_GET_OK);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_status_code (self, 123);
    xrap_msg_set_etag (self, "Life is short but Now lasts for ever");
    xrap_msg_set_date_modified (self, 123);
    xrap_msg_set_content_type (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_body (self, "Life is short but Now lasts for ever");
    xrap_msg_metadata_insert (self, "Name", "Brutus");
    xrap_msg_metadata_insert (self, "Age", "%d", 43);
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (xrap_msg_status_code (self) == 123);
        assert (streq (xrap_msg_etag (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_date_modified (self) == 123);
        assert (streq (xrap_msg_content_type (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_body (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_metadata_size (self) == 2);
        assert (streq (xrap_msg_metadata_string (self, "Name", "?"), "Brutus"));
        assert (xrap_msg_metadata_number (self, "Age", 0) == 43);
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_GET_EMPTY);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_status_code (self, 123);
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (xrap_msg_status_code (self) == 123);
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_PUT);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_resource (self, "Life is short but Now lasts for ever");
    xrap_msg_set_if_unmodified_since (self, 123);
    xrap_msg_set_if_match (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_type (self, "Life is short but Now lasts for ever");
    xrap_msg_set_content_body (self, "Life is short but Now lasts for ever");
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (streq (xrap_msg_resource (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_if_unmodified_since (self) == 123);
        assert (streq (xrap_msg_if_match (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_type (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_content_body (self), "Life is short but Now lasts for ever"));
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_PUT_OK);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_status_code (self, 123);
    xrap_msg_set_location (self, "Life is short but Now lasts for ever");
    xrap_msg_set_etag (self, "Life is short but Now lasts for ever");
    xrap_msg_set_date_modified (self, 123);
    xrap_msg_metadata_insert (self, "Name", "Brutus");
    xrap_msg_metadata_insert (self, "Age", "%d", 43);
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (xrap_msg_status_code (self) == 123);
        assert (streq (xrap_msg_location (self), "Life is short but Now lasts for ever"));
        assert (streq (xrap_msg_etag (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_date_modified (self) == 123);
        assert (xrap_msg_metadata_size (self) == 2);
        assert (streq (xrap_msg_metadata_string (self, "Name", "?"), "Brutus"));
        assert (xrap_msg_metadata_number (self, "Age", 0) == 43);
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_DELETE);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_resource (self, "Life is short but Now lasts for ever");
    xrap_msg_set_if_unmodified_since (self, 123);
    xrap_msg_set_if_match (self, "Life is short but Now lasts for ever");
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (streq (xrap_msg_resource (self), "Life is short but Now lasts for ever"));
        assert (xrap_msg_if_unmodified_since (self) == 123);
        assert (streq (xrap_msg_if_match (self), "Life is short but Now lasts for ever"));
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_DELETE_OK);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_status_code (self, 123);
    xrap_msg_metadata_insert (self, "Name", "Brutus");
    xrap_msg_metadata_insert (self, "Age", "%d", 43);
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (xrap_msg_status_code (self) == 123);
        assert (xrap_msg_metadata_size (self) == 2);
        assert (streq (xrap_msg_metadata_string (self, "Name", "?"), "Brutus"));
        assert (xrap_msg_metadata_number (self, "Age", 0) == 43);
        xrap_msg_destroy (&self);
    }
    self = xrap_msg_new (XRAP_MSG_ERROR);

    //  Check that _dup works on empty message
    copy = xrap_msg_dup (self);
    assert (copy);
    xrap_msg_destroy (&copy);

    xrap_msg_set_status_code (self, 123);
    xrap_msg_set_status_text (self, "Life is short but Now lasts for ever");
    //  Send twice from same object
    xrap_msg_send_again (self, output);
    xrap_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = xrap_msg_recv (input);
        assert (self);
        assert (xrap_msg_routing_id (self));

        assert (xrap_msg_status_code (self) == 123);
        assert (streq (xrap_msg_status_text (self), "Life is short but Now lasts for ever"));
        xrap_msg_destroy (&self);
    }

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
}
