/*  =========================================================================
    zeb_curl_client - Curl client to test the http server

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of ZEBRA.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZEB_CURL_CLIENT_H_INCLUDED
#define ZEB_CURL_CLIENT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zeb_curl_client_t zeb_curl_client_t;

//  @interface
//  Create a new zeb_curl_client
ZEBRA_EXPORT zeb_curl_client_t *
    zeb_curl_client_new (void);

//  Destroy the zeb_curl_client
ZEBRA_EXPORT void
    zeb_curl_client_destroy (zeb_curl_client_t **self_p);

//  Sends a HTTP GET request to the given URL
ZEBRA_EXPORT void
    zeb_curl_client_send_get (zeb_curl_client_t *self, char *url);

//  Sends a HTTP POST request to the given URL
ZEBRA_EXPORT void
    zeb_curl_client_send_post (zeb_curl_client_t *self, char *url, char *data);

//  Sends a HTTP PUT request to the given URL
ZEBRA_EXPORT void
    zeb_curl_client_send_put (zeb_curl_client_t *self, char *url, char *data);

//  Sends a HTTP DELETE request to the given URL
ZEBRA_EXPORT void
    zeb_curl_client_send_delete (zeb_curl_client_t *self, char *url);

ZEBRA_EXPORT void
    zeb_curl_client_verify_response (zeb_curl_client_t *self, int status, char *content);

//  Self test of this class
ZEBRA_EXPORT void
    zeb_curl_client_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
