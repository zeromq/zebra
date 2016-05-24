/*  =========================================================================
    zebra_classes - private header file

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.                                        
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef ZEBRA_CLASSES_H_INCLUDED
#define ZEBRA_CLASSES_H_INCLUDED

//  Platform definitions, must come first
#include "platform.h"

//  External API
#include "../include/zebra.h"

//  Internal API
#include "zeb_request.h"
#include "zeb_response.h"
#include "zeb_connection.h"

//  *** To avoid double-definitions, only define if building without draft ***
#ifndef ZEBRA_BUILD_DRAFT_API

//  *** Draft method, defined for internal use only ***
//  Self test of this class.
ZEBRA_EXPORT void
    zeb_request_test (bool verbose);

//  *** Draft method, defined for internal use only ***
//  Self test of this class.
ZEBRA_EXPORT void
    zeb_response_test (bool verbose);

//  *** Draft method, defined for internal use only ***
//  Self test of this class.
ZEBRA_EXPORT void
    zeb_connection_test (bool verbose);

#endif // ZEBRA_BUILD_DRAFT_API

#endif
