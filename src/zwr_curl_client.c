/*  =========================================================================
    zwr_curl_client - Set of utilities to send HTTP request and verify the responses

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZWEBRAP.

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
    int still_running;
    char *data;
} zwr_curl_client_t;

size_t static
write_callback_func(void *buffer, size_t size, size_t nmemb, void *userp)
{
    char **response = (char **) userp;
    *response = strndup ((char *) buffer, (size_t) (size * nmemb));
    return nmemb;
}

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
        zstr_free (&self->data);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Sends a HTTP GET request to the given URL

void
zwr_curl_client_send_get (zwr_curl_client_t *self, char *url)
{
    CURL *curl = curl_easy_init ();
    assert (curl);
    curl_easy_setopt (curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &self->data);
    curl_easy_setopt (curl, CURLOPT_HEADER, true);
    struct curl_slist *header = NULL;
    header = curl_slist_append (header, "User-Agent: Curl-Test-Client");
    curl_easy_setopt (curl, CURLOPT_HTTPHEADER, header);
    //  Send GET request
    curl_multi_add_handle (self->multi_handle, curl);
    curl_multi_perform (self->multi_handle, &self->still_running);
    //  Cleanup
    curl_slist_free_all (header);
}

//  --------------------------------------------------------------------------
//  Sends a HTTP GET request to the given URL

void
zwr_curl_client_send_post (zwr_curl_client_t *self, char *url, char *data)
{
    CURL *curl = curl_easy_init ();
    assert (curl);
    curl_easy_setopt (curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt (curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt (curl, CURLOPT_HEADER, true);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &self->data);
    //  Set HTTP header
    struct curl_slist *header = NULL;
    header = curl_slist_append (header, "Content-Type: text/plain");
    header = curl_slist_append (header, "User-Agent: curl test client");
    size_t len1 = 16;
    size_t len2 = strlen (data);
    char *result = (char *) malloc (len1 + len2 + 1);    //+1 for the zero-terminator
    assert (result);
    sprintf (result, "%s%zu", "Content-Length", len2);
    header = curl_slist_append (header, result);
    curl_easy_setopt (curl, CURLOPT_HTTPHEADER, header);
    //  Send POST request
    curl_multi_add_handle (self->multi_handle, curl);
    curl_multi_perform (self->multi_handle, &self->still_running);
    //  Cleanup
    free (result);
    curl_slist_free_all (header);
}


void
zwr_curl_client_verify_response (zwr_curl_client_t *self, int status, char *content)
{
    do {
        struct timeval timeout;
        int rc; /* select () return code */
        CURLMcode mc; /* curl_multi_fdset () return code */

        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;

        long curl_timeo = -1;

        FD_ZERO (&fdread);
        FD_ZERO (&fdwrite);
        FD_ZERO (&fdexcep);

        /* set a suitable timeout to play around with */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        curl_multi_timeout (self->multi_handle, &curl_timeo);
        if(curl_timeo >= 0) {
            timeout.tv_sec = curl_timeo / 1000;
            if(timeout.tv_sec > 1)
                timeout.tv_sec = 1;
            else
                timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }

        /* get file descriptors from the transfers */
        mc = curl_multi_fdset (self->multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

        if (mc != CURLM_OK) {
          fprintf (stderr, "curl_multi_fdset () failed, code %d.\n", mc);
          break;
        }

        /* On success the value of maxfd is guaranteed to be >= -1. We call
           select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
           no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
           to sleep 100ms, which is the minimum suggested value in the
           curl_multi_fdset() doc. */

        if (maxfd == -1) {
          /* Portable sleep for platforms other than Windows. */
          struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
          rc = select (0, NULL, NULL, NULL, &wait);
        }
        else {
          /* Note that on some platforms 'timeout' may be modified by select().
             If you need access to the original value save a copy beforehand. */
          rc = select (maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
        }

        switch (rc) {
        case -1:
            /* select error */
            break;
        case 0: /* timeout */
        default: /* action */
            curl_multi_perform (self->multi_handle, &self->still_running);
            break;
        }
    } while (self->still_running);

    int dontcare;
    CURLMsg *msg = NULL;
    while ((msg = curl_multi_info_read (self->multi_handle, &dontcare))) {
        if (msg->msg == CURLMSG_DONE) {
            CURL *curl = msg->easy_handle;

            long actual_status;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &actual_status);
            assert (actual_status == status);
            assert (streq (self->data, content));
            zstr_free (&self->data);

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
