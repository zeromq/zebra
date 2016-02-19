/*  =========================================================================
    zebra - Translates REST HTTP-Request into the XRAP protocol.

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.                                        
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef __PLATFORM_H_INCLUDED__
#define __PLATFORM_H_INCLUDED__

//  This file provides the configuration for Linux, Windows, and OS/X
//  as determined by ZEBRA_HAVE_XXX macros passed from project.gyp

//  Check that we're being called from our gyp makefile
#ifndef ZEBRA_GYP_BUILD
#   error "foreign platform.h detected, please re-configure"
#endif

#define ZEBRA_BUILD_DRAFT_API

#if defined ZEBRA_HAVE_WINDOWS

#elif defined ZEBRA_HAVE_OSX
#   define ZEBRA_HAVE_IFADDRS
#   define HAVE_GETIFADDRS
#   define HAVE_NET_IF_H
#   define HAVE_NET_IF_MEDIA_H

#elif defined ZEBRA_HAVE_LINUX
#   define ZEBRA_HAVE_IFADDRS
#   define HAVE_GETIFADDRS
#   define HAVE_LINUX_WIRELESS_H
#   define HAVE_NET_IF_H
#   define HAVE_NET_IF_MEDIA_H

#else
#   error "No platform defined, abandoning"
#endif

#endif
