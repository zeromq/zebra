/*  =========================================================================
    zwr_microhttpd - HTTP web server backend using libmicrohttpd.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZWR_MICROHTTPDD_H_INCLUDED
#define ZWR_MICROHTTPDD_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_HEADER_CONTENT_TYPE "Content-Type"

//  Opaque class structure
#ifndef ZWR_MICROHTTPD_T_DEFINED
typedef struct _zwr_microhttpd_t zwr_microhttpd_t;
#define ZWR_MICROHTTPD_T_DEFINED
#endif

//  @interface
//  Create new zwr_microhttpd actor instance.
//  @TODO: Describe the purpose of this actor!
//
//      zactor_t *microhttpd = zactor_new (zwr_microhttpd, NULL);
//
//  Destroy zwr_microhttpd instance.
//
//      zactor_destroy (&microhttpd);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (microhttpd, "VERBOSE");
//      zsock_wait (microhttpd);
//
//  Start zwr_microhttpd actor.
//
//      zstr_sendx (microhttpd, "START", NULL);
//      zsock_wait (microhttpd);
//
//  Stop zwr_microhttpd actor.
//
//      zstr_sendx (microhttpd, "STOP", NULL);
//      zsock_wait (microhttpd);
//
//  Attach the endpoint socket to endpoints, see zsock_attach ().
//
//      zstr_sendx (microhttpd, "ENDPOINT", endpoints, NULL);
//      zsock_wait (microhttpd);
//
//  Set the port on which the http server shall listen for requests.
//  The default port is 8888.
//  Note: The port is passed as string!
//  Note: If the server is already started, the server will be stopped
//        and restarted on the new port.
//
//      zstr_sendx (microhttpd, "PORT", "8888", NULL);
//      zsock_wait (microhttpd);
//
//  This is the zwr_microhttpd constructor as a zactor_fn;
ZWEBRAP_EXPORT void
    zwr_microhttpd_actor (zsock_t *pipe, void *args);

//  Self test of this actor
ZWEBRAP_EXPORT void
    zwr_microhttpd_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
