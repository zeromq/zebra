/*  =========================================================================
    zebra Node.js binding

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of ZEBRA.                                        
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#define ZEBRA_BUILD_DRAFT_API
#include "zebra.h.h"
#include "nan.h"

using namespace v8;
using namespace Nan;

class XrapMsg: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("XrapMsg").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "decode", decode);
            Nan::SetPrototypeMethod (tpl, "encode", encode);
            Nan::SetPrototypeMethod (tpl, "recv", recv);
            Nan::SetPrototypeMethod (tpl, "recvNowait", recv_nowait);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "sendAgain", send_again);
            Nan::SetPrototypeMethod (tpl, "id", id);
            Nan::SetPrototypeMethod (tpl, "setId", set_id);
            Nan::SetPrototypeMethod (tpl, "parent", parent);
            Nan::SetPrototypeMethod (tpl, "setParent", set_parent);
            Nan::SetPrototypeMethod (tpl, "contentType", content_type);
            Nan::SetPrototypeMethod (tpl, "setContentType", set_content_type);
            Nan::SetPrototypeMethod (tpl, "contentBody", content_body);
            Nan::SetPrototypeMethod (tpl, "setContentBody", set_content_body);
            Nan::SetPrototypeMethod (tpl, "statusCode", status_code);
            Nan::SetPrototypeMethod (tpl, "setStatusCode", set_status_code);
            Nan::SetPrototypeMethod (tpl, "location", location);
            Nan::SetPrototypeMethod (tpl, "setLocation", set_location);
            Nan::SetPrototypeMethod (tpl, "etag", etag);
            Nan::SetPrototypeMethod (tpl, "setEtag", set_etag);
            Nan::SetPrototypeMethod (tpl, "dateModified", date_modified);
            Nan::SetPrototypeMethod (tpl, "setDateModified", set_date_modified);
            Nan::SetPrototypeMethod (tpl, "resource", resource);
            Nan::SetPrototypeMethod (tpl, "setResource", set_resource);
            Nan::SetPrototypeMethod (tpl, "parameters", parameters);
            Nan::SetPrototypeMethod (tpl, "getParameters", get_parameters);
            Nan::SetPrototypeMethod (tpl, "setParameters", set_parameters);
            Nan::SetPrototypeMethod (tpl, "parametersString", parameters_string);
            Nan::SetPrototypeMethod (tpl, "parametersInsert", parameters_insert);
            Nan::SetPrototypeMethod (tpl, "ifModifiedSince", if_modified_since);
            Nan::SetPrototypeMethod (tpl, "setIfModifiedSince", set_if_modified_since);
            Nan::SetPrototypeMethod (tpl, "ifNoneMatch", if_none_match);
            Nan::SetPrototypeMethod (tpl, "setIfNoneMatch", set_if_none_match);
            Nan::SetPrototypeMethod (tpl, "metadata", metadata);
            Nan::SetPrototypeMethod (tpl, "getMetadata", get_metadata);
            Nan::SetPrototypeMethod (tpl, "setMetadata", set_metadata);
            Nan::SetPrototypeMethod (tpl, "metadataString", metadata_string);
            Nan::SetPrototypeMethod (tpl, "metadataInsert", metadata_insert);
            Nan::SetPrototypeMethod (tpl, "ifUnmodifiedSince", if_unmodified_since);
            Nan::SetPrototypeMethod (tpl, "setIfUnmodifiedSince", set_if_unmodified_since);
            Nan::SetPrototypeMethod (tpl, "ifMatch", if_match);
            Nan::SetPrototypeMethod (tpl, "setIfMatch", set_if_match);
            Nan::SetPrototypeMethod (tpl, "statusText", status_text);
            Nan::SetPrototypeMethod (tpl, "setStatusText", set_status_text);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("XrapMsg").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit XrapMsg (arguments) {
            self = new (name);
        }
        ~XrapMsg () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        XrapMsg *xrap_msg;
        xrap_msg = new XrapMsg ();
        if (xrap_msg) {
            xrap_msg->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_destroy (&xrap_msg->self);
    }

    static NAN_METHOD (defined) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (xrap_msg->self != NULL));
    }

    static NAN_METHOD (decode) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_decode (xrap_msg->self);
    }

    static NAN_METHOD (encode) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_encode (xrap_msg->self);
    }

    static NAN_METHOD (recv) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_recv (xrap_msg->self);
    }

    static NAN_METHOD (recv_nowait) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_recv_nowait (xrap_msg->self);
    }

    static NAN_METHOD (send) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_send (xrap_msg->self);
    }

    static NAN_METHOD (send_again) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_send_again (xrap_msg->self);
    }

    static NAN_METHOD (id) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_id (xrap_msg->self);
    }

    static NAN_METHOD (set_id) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_id (xrap_msg->self);
    }

    static NAN_METHOD (parent) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_parent (xrap_msg->self);
    }

    static NAN_METHOD (set_parent) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_parent (xrap_msg->self);
    }

    static NAN_METHOD (content_type) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_content_type (xrap_msg->self);
    }

    static NAN_METHOD (set_content_type) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_content_type (xrap_msg->self);
    }

    static NAN_METHOD (content_body) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_content_body (xrap_msg->self);
    }

    static NAN_METHOD (set_content_body) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_content_body (xrap_msg->self);
    }

    static NAN_METHOD (status_code) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_status_code (xrap_msg->self);
    }

    static NAN_METHOD (set_status_code) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_status_code (xrap_msg->self);
    }

    static NAN_METHOD (location) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_location (xrap_msg->self);
    }

    static NAN_METHOD (set_location) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_location (xrap_msg->self);
    }

    static NAN_METHOD (etag) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_etag (xrap_msg->self);
    }

    static NAN_METHOD (set_etag) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_etag (xrap_msg->self);
    }

    static NAN_METHOD (date_modified) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_date_modified (xrap_msg->self);
    }

    static NAN_METHOD (set_date_modified) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_date_modified (xrap_msg->self);
    }

    static NAN_METHOD (resource) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_resource (xrap_msg->self);
    }

    static NAN_METHOD (set_resource) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_resource (xrap_msg->self);
    }

    static NAN_METHOD (parameters) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_parameters (xrap_msg->self);
    }

    static NAN_METHOD (get_parameters) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_get_parameters (xrap_msg->self);
    }

    static NAN_METHOD (set_parameters) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_parameters (xrap_msg->self);
    }

    static NAN_METHOD (parameters_string) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_parameters_string (xrap_msg->self);
    }

    static NAN_METHOD (parameters_insert) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_parameters_insert (xrap_msg->self);
    }

    static NAN_METHOD (if_modified_since) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_if_modified_since (xrap_msg->self);
    }

    static NAN_METHOD (set_if_modified_since) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_if_modified_since (xrap_msg->self);
    }

    static NAN_METHOD (if_none_match) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_if_none_match (xrap_msg->self);
    }

    static NAN_METHOD (set_if_none_match) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_if_none_match (xrap_msg->self);
    }

    static NAN_METHOD (metadata) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_metadata (xrap_msg->self);
    }

    static NAN_METHOD (get_metadata) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_get_metadata (xrap_msg->self);
    }

    static NAN_METHOD (set_metadata) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_metadata (xrap_msg->self);
    }

    static NAN_METHOD (metadata_string) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_metadata_string (xrap_msg->self);
    }

    static NAN_METHOD (metadata_insert) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_metadata_insert (xrap_msg->self);
    }

    static NAN_METHOD (if_unmodified_since) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_if_unmodified_since (xrap_msg->self);
    }

    static NAN_METHOD (set_if_unmodified_since) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_if_unmodified_since (xrap_msg->self);
    }

    static NAN_METHOD (if_match) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_if_match (xrap_msg->self);
    }

    static NAN_METHOD (set_if_match) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_if_match (xrap_msg->self);
    }

    static NAN_METHOD (status_text) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_status_text (xrap_msg->self);
    }

    static NAN_METHOD (set_status_text) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_set_status_text (xrap_msg->self);
    }

    static NAN_METHOD (test) {
        XrapMsg *xrap_msg = Nan::ObjectWrap::Unwrap <XrapMsg> (info.Holder ());
        xrap_msg_test (xrap_msg->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    xrap_msg_t *self;
    public:
        xrap_msg_t *get_self () {
            return self;
        }
};

class ZebHandler: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("ZebHandler").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "addOffer", add_offer);
            Nan::SetPrototypeMethod (tpl, "addAccept", add_accept);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("ZebHandler").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit ZebHandler () {
        }
        ~ZebHandler () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        ZebHandler *zeb_handler;
        zeb_handler = new ZebHandler ();
        if (zeb_handler) {
            zeb_handler->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (add_offer) {
        ZebHandler *zeb_handler = Nan::ObjectWrap::Unwrap <ZebHandler> (info.Holder ());
        zeb_handler_add_offer (zeb_handler->self);
    }

    static NAN_METHOD (add_accept) {
        ZebHandler *zeb_handler = Nan::ObjectWrap::Unwrap <ZebHandler> (info.Holder ());
        zeb_handler_add_accept (zeb_handler->self);
    }

    static NAN_METHOD (test) {
        ZebHandler *zeb_handler = Nan::ObjectWrap::Unwrap <ZebHandler> (info.Holder ());
        zeb_handler_test (zeb_handler->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class XrapTraffic: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("XrapTraffic").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "recv", recv);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "routingId", routing_id);
            Nan::SetPrototypeMethod (tpl, "setRoutingId", set_routing_id);
            Nan::SetPrototypeMethod (tpl, "id", id);
            Nan::SetPrototypeMethod (tpl, "setId", set_id);
            Nan::SetPrototypeMethod (tpl, "command", command);
            Nan::SetPrototypeMethod (tpl, "address", address);
            Nan::SetPrototypeMethod (tpl, "setAddress", set_address);
            Nan::SetPrototypeMethod (tpl, "timeout", timeout);
            Nan::SetPrototypeMethod (tpl, "setTimeout", set_timeout);
            Nan::SetPrototypeMethod (tpl, "content", content);
            Nan::SetPrototypeMethod (tpl, "getContent", get_content);
            Nan::SetPrototypeMethod (tpl, "setContent", set_content);
            Nan::SetPrototypeMethod (tpl, "route", route);
            Nan::SetPrototypeMethod (tpl, "setRoute", set_route);
            Nan::SetPrototypeMethod (tpl, "method", method);
            Nan::SetPrototypeMethod (tpl, "setMethod", set_method);
            Nan::SetPrototypeMethod (tpl, "sender", sender);
            Nan::SetPrototypeMethod (tpl, "getSender", get_sender);
            Nan::SetPrototypeMethod (tpl, "setSender", set_sender);
            Nan::SetPrototypeMethod (tpl, "statusCode", status_code);
            Nan::SetPrototypeMethod (tpl, "setStatusCode", set_status_code);
            Nan::SetPrototypeMethod (tpl, "statusReason", status_reason);
            Nan::SetPrototypeMethod (tpl, "setStatusReason", set_status_reason);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("XrapTraffic").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit XrapTraffic (arguments) {
            self = new (name);
        }
        ~XrapTraffic () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        XrapTraffic *xrap_traffic;
        xrap_traffic = new XrapTraffic ();
        if (xrap_traffic) {
            xrap_traffic->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_destroy (&xrap_traffic->self);
    }

    static NAN_METHOD (defined) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (xrap_traffic->self != NULL));
    }

    static NAN_METHOD (recv) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_recv (xrap_traffic->self);
    }

    static NAN_METHOD (send) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_send (xrap_traffic->self);
    }

    static NAN_METHOD (print) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_print (xrap_traffic->self);
    }

    static NAN_METHOD (routing_id) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_routing_id (xrap_traffic->self);
    }

    static NAN_METHOD (set_routing_id) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_routing_id (xrap_traffic->self);
    }

    static NAN_METHOD (id) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_id (xrap_traffic->self);
    }

    static NAN_METHOD (set_id) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_id (xrap_traffic->self);
    }

    static NAN_METHOD (command) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_command (xrap_traffic->self);
    }

    static NAN_METHOD (address) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_address (xrap_traffic->self);
    }

    static NAN_METHOD (set_address) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_address (xrap_traffic->self);
    }

    static NAN_METHOD (timeout) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_timeout (xrap_traffic->self);
    }

    static NAN_METHOD (set_timeout) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_timeout (xrap_traffic->self);
    }

    static NAN_METHOD (content) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_content (xrap_traffic->self);
    }

    static NAN_METHOD (get_content) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_get_content (xrap_traffic->self);
    }

    static NAN_METHOD (set_content) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_content (xrap_traffic->self);
    }

    static NAN_METHOD (route) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_route (xrap_traffic->self);
    }

    static NAN_METHOD (set_route) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_route (xrap_traffic->self);
    }

    static NAN_METHOD (method) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_method (xrap_traffic->self);
    }

    static NAN_METHOD (set_method) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_method (xrap_traffic->self);
    }

    static NAN_METHOD (sender) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_sender (xrap_traffic->self);
    }

    static NAN_METHOD (get_sender) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_get_sender (xrap_traffic->self);
    }

    static NAN_METHOD (set_sender) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_sender (xrap_traffic->self);
    }

    static NAN_METHOD (status_code) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_status_code (xrap_traffic->self);
    }

    static NAN_METHOD (set_status_code) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_status_code (xrap_traffic->self);
    }

    static NAN_METHOD (status_reason) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_status_reason (xrap_traffic->self);
    }

    static NAN_METHOD (set_status_reason) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_set_status_reason (xrap_traffic->self);
    }

    static NAN_METHOD (test) {
        XrapTraffic *xrap_traffic = Nan::ObjectWrap::Unwrap <XrapTraffic> (info.Holder ());
        xrap_traffic_test (xrap_traffic->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    xrap_traffic_t *self;
    public:
        xrap_traffic_t *get_self () {
            return self;
        }
};

class ZebBroker: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("ZebBroker").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("ZebBroker").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit ZebBroker () {
        }
        ~ZebBroker () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        ZebBroker *zeb_broker;
        zeb_broker = new ZebBroker ();
        if (zeb_broker) {
            zeb_broker->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        ZebBroker *zeb_broker = Nan::ObjectWrap::Unwrap <ZebBroker> (info.Holder ());
        zeb_broker_test (zeb_broker->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class ZebClient: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("ZebClient").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "actor", actor);
            Nan::SetPrototypeMethod (tpl, "msgpipe", msgpipe);
            Nan::SetPrototypeMethod (tpl, "connected", connected);
            Nan::SetPrototypeMethod (tpl, "connect", connect);
            Nan::SetPrototypeMethod (tpl, "setHandler", set_handler);
            Nan::SetPrototypeMethod (tpl, "request", request);
            Nan::SetPrototypeMethod (tpl, "deliver", deliver);
            Nan::SetPrototypeMethod (tpl, "recv", recv);
            Nan::SetPrototypeMethod (tpl, "command", command);
            Nan::SetPrototypeMethod (tpl, "status", status);
            Nan::SetPrototypeMethod (tpl, "reason", reason);
            Nan::SetPrototypeMethod (tpl, "sender", sender);
            Nan::SetPrototypeMethod (tpl, "content", content);
            Nan::SetPrototypeMethod (tpl, "setVerbose", set_verbose);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("ZebClient").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit ZebClient (arguments) {
            self = new (name);
        }
        ~ZebClient () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        ZebClient *zeb_client;
        zeb_client = new ZebClient ();
        if (zeb_client) {
            zeb_client->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_destroy (&zeb_client->self);
    }

    static NAN_METHOD (defined) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zeb_client->self != NULL));
    }

    static NAN_METHOD (actor) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_actor (zeb_client->self);
    }

    static NAN_METHOD (msgpipe) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_msgpipe (zeb_client->self);
    }

    static NAN_METHOD (connected) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_connected (zeb_client->self);
    }

    static NAN_METHOD (connect) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_connect (zeb_client->self);
    }

    static NAN_METHOD (set_handler) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_set_handler (zeb_client->self);
    }

    static NAN_METHOD (request) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_request (zeb_client->self);
    }

    static NAN_METHOD (deliver) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_deliver (zeb_client->self);
    }

    static NAN_METHOD (recv) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_recv (zeb_client->self);
    }

    static NAN_METHOD (command) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_command (zeb_client->self);
    }

    static NAN_METHOD (status) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_status (zeb_client->self);
    }

    static NAN_METHOD (reason) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_reason (zeb_client->self);
    }

    static NAN_METHOD (sender) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_sender (zeb_client->self);
    }

    static NAN_METHOD (content) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_content (zeb_client->self);
    }

    static NAN_METHOD (set_verbose) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_set_verbose (zeb_client->self);
    }

    static NAN_METHOD (test) {
        ZebClient *zeb_client = Nan::ObjectWrap::Unwrap <ZebClient> (info.Holder ());
        zeb_client_test (zeb_client->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zeb_client_t *self;
    public:
        zeb_client_t *get_self () {
            return self;
        }
};

class ZebMicrohttpd: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("ZebMicrohttpd").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("ZebMicrohttpd").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit ZebMicrohttpd () {
        }
        ~ZebMicrohttpd () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        ZebMicrohttpd *zeb_microhttpd;
        zeb_microhttpd = new ZebMicrohttpd ();
        if (zeb_microhttpd) {
            zeb_microhttpd->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        ZebMicrohttpd *zeb_microhttpd = Nan::ObjectWrap::Unwrap <ZebMicrohttpd> (info.Holder ());
        zeb_microhttpd_test (zeb_microhttpd->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};


extern "C" NAN_MODULE_INIT (zebra_initialize)
{
    XrapMsg::Init (target);
    ZebHandler::Init (target);
    XrapTraffic::Init (target);
    ZebBroker::Init (target);
    ZebClient::Init (target);
    ZebMicrohttpd::Init (target);
}

NODE_MODULE (zebra, zebra_initialize)
