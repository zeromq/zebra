/*  =========================================================================
    zebra Node.js binding implementation

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.                                        
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#include "binding.h"

using namespace v8;
using namespace Nan;

NAN_MODULE_INIT (ZebHandler::Init) {
    Nan::HandleScope scope;

    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
    tpl->SetClassName (Nan::New ("ZebHandler").ToLocalChecked ());
    tpl->InstanceTemplate ()->SetInternalFieldCount (1);

    // Prototypes
    Nan::SetPrototypeMethod (tpl, "test", _test);

    constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
    Nan::Set (target, Nan::New ("ZebHandler").ToLocalChecked (),
    Nan::GetFunction (tpl).ToLocalChecked ());
}

ZebHandler::ZebHandler () {
}

ZebHandler::~ZebHandler () {
}

NAN_METHOD (ZebHandler::New) {
    assert (info.IsConstructCall ());
    ZebHandler *zeb_handler = new ZebHandler ();
    if (zeb_handler) {
        zeb_handler->Wrap (info.This ());
        info.GetReturnValue ().Set (info.This ());
    }
}

NAN_METHOD (ZebHandler::_test) {
    if (info [0]->IsUndefined ())
        return Nan::ThrowTypeError ("method requires a `verbose`");
    else
    if (!info [0]->IsBoolean ())
        return Nan::ThrowTypeError ("`verbose` must be a number");
    bool verbose = Nan::To<bool>(info [0]).FromJust ();

    zeb_handler_test (verbose);
}

Nan::Persistent <Function> &ZebHandler::constructor () {
    static Nan::Persistent <Function> my_constructor;
    return my_constructor;
}


extern "C" NAN_MODULE_INIT (zebra_initialize)
{
    ZebHandler::Init (target);
}

NODE_MODULE (zebra, zebra_initialize)
