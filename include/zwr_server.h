/*  =========================================================================
    zwr_server - ZWebRap Server

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zwr_server.xml, or
     * The code generation script that built this file: zproto_server_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZWR_SERVER_H_INCLUDED
#define ZWR_SERVER_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  To work with zwr_server, use the CZMQ zactor API:
//
//  Create new zwr_server instance, passing logging prefix:
//
//      zactor_t *zwr_server = zactor_new (zwr_server, "myname");
//
//  Destroy zwr_server instance
//
//      zactor_destroy (&zwr_server);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (zwr_server, "VERBOSE");
//
//  Bind zwr_server to specified endpoint. TCP endpoints may specify
//  the port number as "*" to aquire an ephemeral port:
//
//      zstr_sendx (zwr_server, "BIND", endpoint, NULL);
//
//  Return assigned port number, specifically when BIND was done using an
//  an ephemeral port:
//
//      zstr_sendx (zwr_server, "PORT", NULL);
//      char *command, *port_str;
//      zstr_recvx (zwr_server, &command, &port_str, NULL);
//      assert (streq (command, "PORT"));
//
//  Specify configuration file to load, overwriting any previous loaded
//  configuration file or options:
//
//  -------------------------- zwr_server.cfg ---------------------------------
//  | 1  | #   Apply to the whole broker
//  | 2  | server
//  | 3  |     timeout = 10000     #   Client connection timeout, msec
//  | 4  |     background = 0      #   Run as background process
//  | 5  |     workdir = .         #   Working directory for daemon
//  | 6  |      verbose = 0         #   Do verbose logging of activity?
//  | 7  |
//  | 8  | #   Apply to the ZWebRap service
//  | 9  | #   Note security settings must come before binds
//  | 10 | zwr_server
//  | 11 |     bind
//  | 12 |         endpoint = tcp://127.0.0.1:9999
//  ---------------------------------------------------------------------------
//
//      zstr_sendx (zwr_server, "LOAD", filename, NULL);
//
//  Set configuration path value:
//
//      zstr_sendx (zwr_server, "SET", path, value, NULL);
//
//  Save configuration data to config file on disk:
//
//      zstr_sendx (zwr_server, "SAVE", filename, NULL);
//
//  Send zmsg_t instance to zwr_server:
//
//      zactor_send (zwr_server, &msg);
//
//  Receive zmsg_t instance from zwr_server:
//
//      zmsg_t *msg = zactor_recv (zwr_server);
//
//  This is the zwr_server constructor as a zactor_fn:
//
ZWEBRAP_EXPORT void
    zwr_server (zsock_t *pipe, void *args);

//  Self test of this class
ZWEBRAP_EXPORT void
    zwr_server_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
