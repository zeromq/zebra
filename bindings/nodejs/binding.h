/*  =========================================================================
    zebra Node.js binding header file

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.                                        
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZEBRA_BINDING_H_INCLUDED
#define ZEBRA_BINDING_H_INCLUDED

#define ZEBRA_BUILD_DRAFT_API

#include "zebra.h"
#include "nan.h"

using namespace v8;
using namespace Nan;

class ZebHandler: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init);
    private:
        explicit ZebHandler ();
        ~ZebHandler ();
    static Nan::Persistent <Function> &constructor ();

    static NAN_METHOD (New);
    static NAN_METHOD (_test);
};

#endif
