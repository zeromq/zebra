/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/

#include "QmlZebClient.h"


///
//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.                                      
zactor_t *QmlZebClient::actor () {
    return zeb_client_actor (self);
};

///
//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In   
//  the low-volume case we can do everything over the actor pipe, if traffic  
//  is never ambiguous.                                                       
zsock_t *QmlZebClient::msgpipe () {
    return zeb_client_msgpipe (self);
};

///
//  Return true if client is currently connected, else false. Note that the   
//  client will automatically re-connect if the server dies and restarts after
//  a successful first connection.                                            
bool QmlZebClient::connected () {
    return zeb_client_connected (self);
};

///
//  Connect to server endpoint, with specified timeout in msecs (zero means wait    
//  forever). Constructor succeeds if connection is successful. The caller may      
//  specify its address.                                                            
//  Returns >= 0 if successful, -1 if interrupted.                                  
int QmlZebClient::connect (const QString &endpoint, uint32_t timeout, const QString &address) {
    return zeb_client_connect (self, endpoint.toUtf8().data(), timeout, address.toUtf8().data());
};

///
//  Offer to handle particular XRAP requests, where the route matches request's     
//  resource.                                                                       
//  Returns >= 0 if successful, -1 if interrupted.                                  
int QmlZebClient::setHandler (const QString &method, const QString &route) {
    return zeb_client_set_handler (self, method.toUtf8().data(), route.toUtf8().data());
};

///
//  No explanation                                                                  
//  Returns >= 0 if successful, -1 if interrupted.                                  
int QmlZebClient::request (uint32_t timeout, zmsg_t **content) {
    return zeb_client_request (self, timeout, content);
};

///
//  Send XRAP DELIVER message to server, takes ownership of message
//  and destroys message when done sending it.                     
int QmlZebClient::deliver (zuuid_t *sender, zmsg_t **content) {
    return zeb_client_deliver (self, sender, content);
};

///
//  Receive message from server; caller destroys message when done
zmsg_t *QmlZebClient::recv () {
    return zeb_client_recv (self);
};

///
//  Return last received command. Can be one of these values:
//      "XRAP DELIVER"                                       
const QString QmlZebClient::command () {
    return QString (zeb_client_command (self));
};

///
//  Return last received status
int QmlZebClient::status () {
    return zeb_client_status (self);
};

///
//  Return last received reason
const QString QmlZebClient::reason () {
    return QString (zeb_client_reason (self));
};

///
//  Return last received sender
zuuid_t *QmlZebClient::sender () {
    return zeb_client_sender (self);
};

///
//  Return last received content
zmsg_t *QmlZebClient::content () {
    return zeb_client_content (self);
};

///
//  Enable verbose tracing (animation) of state machine activity.
void QmlZebClient::setVerbose (bool verbose) {
    zeb_client_set_verbose (self, verbose);
};


QObject* QmlZebClient::qmlAttachedProperties(QObject* object) {
    return new QmlZebClientAttached(object);
}


///
//  Self test of this class.
void QmlZebClientAttached::test (bool verbose) {
    zeb_client_test (verbose);
};

///
//  Create a new zeb_client, return the reference if successful,   
//  or NULL if construction failed due to lack of available memory.
QmlZebClient *QmlZebClientAttached::construct () {
    QmlZebClient *qmlSelf = new QmlZebClient ();
    qmlSelf->self = zeb_client_new ();
    return qmlSelf;
};

///
//  Destroy the zeb_client and free all memory used by the object.
void QmlZebClientAttached::destruct (QmlZebClient *qmlSelf) {
    zeb_client_destroy (&qmlSelf->self);
};

/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
