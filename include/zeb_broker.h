/*  =========================================================================
    zeb_broker - zebra service broker

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zeb_broker.xml, or
     * The code generation script that built this file: zproto_server_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.                                        
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZEB_BROKER_H_INCLUDED
#define ZEB_BROKER_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  To work with zeb_broker, use the CZMQ zactor API:
//
//  Create new zeb_broker instance, passing logging prefix:
//
//      zactor_t *zeb_broker = zactor_new (zeb_broker, "myname");
//
//  Destroy zeb_broker instance
//
//      zactor_destroy (&zeb_broker);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (zeb_broker, "VERBOSE");
//
//  Bind zeb_broker to specified endpoint. TCP endpoints may specify
//  the port number as "*" to aquire an ephemeral port:
//
//      zstr_sendx (zeb_broker, "BIND", endpoint, NULL);
//
//  Return assigned port number, specifically when BIND was done using an
//  an ephemeral port:
//
//      zstr_sendx (zeb_broker, "PORT", NULL);
//      char *command, *port_str;
//      zstr_recvx (zeb_broker, &command, &port_str, NULL);
//      assert (streq (command, "PORT"));
//
//  Specify configuration file to load, overwriting any previous loaded
//  configuration file or options:
//
//      zstr_sendx (zeb_broker, "LOAD", filename, NULL);
//
//  Set configuration path value:
//
//      zstr_sendx (zeb_broker, "SET", path, value, NULL);
//
//  Save configuration data to config file on disk:
//
//      zstr_sendx (zeb_broker, "SAVE", filename, NULL);
//
//  Send zmsg_t instance to zeb_broker:
//
//      zactor_send (zeb_broker, &msg);
//
//  Receive zmsg_t instance from zeb_broker:
//
//      zmsg_t *msg = zactor_recv (zeb_broker);
//
//  This is the zeb_broker constructor as a zactor_fn:
//
ZEBRA_EXPORT void
    zeb_broker (zsock_t *pipe, void *args);

//  Self test of this class
ZEBRA_EXPORT void
    zeb_broker_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
