/*  =========================================================================
    zwr_curl_client - Set of utilities to send HTTP request and verify the responses

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zwr_curl_client - Set of utilities to send HTTP request and verify the responses
@discuss
@end
*/

#include <curl/curl.h>
#include "../include/zwebrap.h"
#include "zwebrap_classes.h"

//  Structure of our class

typedef struct _zwr_curl_client_t {
    CURLM *multi_handle;
} zwr_curl_client_t;

//  --------------------------------------------------------------------------
//  Create a new zwr_curl_client.

zwr_curl_client_t *
zwr_curl_client_new ()
{
    zwr_curl_client_t *self = (zwr_curl_client_t *) zmalloc (sizeof (zwr_curl_client_t));
    assert (self);

    //  Initialize properties
    curl_global_init(CURL_GLOBAL_ALL);
    self->multi_handle = curl_multi_init ();

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zwr_curl_client.

void
zwr_curl_client_destroy (zwr_curl_client_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zwr_curl_client_t *self = *self_p;

        //  Free class properties
        curl_multi_cleanup (self->multi_handle);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Sends a HTTP GET request to the given URL

void
zwr_curl_client_send_get (zwr_curl_client_t *self, char *url)
{
    int dontcare;
    CURL *curl = curl_easy_init ();
    assert (curl);
    curl_easy_setopt (curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    //  Send GET request
    curl_multi_add_handle (self->multi_handle, curl);
    curl_multi_perform (self->multi_handle, &dontcare);
    curl_easy_cleanup (curl);
}

//  --------------------------------------------------------------------------
//  Sends a HTTP GET request to the given URL

void
zwr_curl_client_send_post (zwr_curl_client_t *self, char *url, char *data)
{
    int dontcare;
    CURL *curl = curl_easy_init ();
    assert (curl);
    curl_easy_setopt (curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt (curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt (curl, CURLOPT_HEADER, true);
    //  Set HTTP header
    struct curl_slist *header = NULL;
    header = curl_slist_append (header, "Content-Type: text/plain");
    size_t len1 = 16;
    size_t len2 = strlen (data);
    char *result = (char *) malloc (len1 + len2 + 1);    //+1 for the zero-terminator
    assert (result);
    sprintf (result, "%s%zu", "Content-Length", len2);
    header = curl_slist_append (header, result);
    curl_easy_setopt (curl, CURLOPT_HTTPHEADER, header);
    //  Send POST request
    curl_multi_add_handle (self->multi_handle, curl);
    curl_multi_perform (self->multi_handle, &dontcare);
    //  Cleanup
    free (result);
    curl_slist_free_all (header);
    curl_easy_cleanup (curl);
}


void
zwr_curl_client_verify_response (zwr_curl_client_t *self, int status, char *content)
{
    int dontcare;
    CURLMsg *msg = NULL;
    while ((msg = curl_multi_info_read (self->multi_handle, &dontcare))) {
        if (msg->msg == CURLMSG_DONE) {
            CURL *curl = msg->easy_handle;

            int actual_status;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &actual_status);
            assert (actual_status == status);

            curl_multi_remove_handle (self->multi_handle, curl);
            curl_easy_cleanup (curl);
        }
    }
}

//  --------------------------------------------------------------------------
//  Print properties of the zwr_curl_client object.

void
zwr_curl_client_print (zwr_curl_client_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zwr_curl_client_test (bool verbose)
{
    printf (" * zwr_curl_client: ");

    //  @selftest
    //  Simple create/destroy test
    zwr_curl_client_t *self = zwr_curl_client_new ();
    assert (self);
    zwr_curl_client_destroy (&self);
    //  @end

    printf ("OK\n");
}
