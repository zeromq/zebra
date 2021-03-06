/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/

#include "qzebra.h"

///
//  Copy-construct to return the proper wrapped c types
QXrapMsg::QXrapMsg (xrap_msg_t *self, QObject *qObjParent) : QObject (qObjParent)
{
    this->self = self;
}


///
//  Create a new xrap_msg
QXrapMsg::QXrapMsg (int id, QObject *qObjParent) : QObject (qObjParent)
{
    this->self = xrap_msg_new (id);
}

///
//  Destroy the xrap_msg
QXrapMsg::~QXrapMsg ()
{
    xrap_msg_destroy (&self);
}

///
//  Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and
//  nullifies the msg reference.                                  
QXrapMsg * QXrapMsg::decode (QZmsg *msgP)
{
    QXrapMsg *rv = new QXrapMsg (xrap_msg_decode (&msgP->self));
    return rv;
}

///
//  Encode xrap_msg into zmsg and destroy it. Returns a newly created       
//  object or NULL if error. Use when not in control of sending the message.
QZmsg * QXrapMsg::encode (QXrapMsg *xrapMsgP)
{
    QZmsg *rv = new QZmsg (xrap_msg_encode (&xrapMsgP->self));
    return rv;
}

///
//  Receive and parse a xrap_msg from the socket. Returns new object,
//  or NULL if error. Will block if there's no message waiting.      
QXrapMsg * QXrapMsg::recv (void *input)
{
    QXrapMsg *rv = new QXrapMsg (xrap_msg_recv (input));
    return rv;
}

///
//  Receive and parse a xrap_msg from the socket. Returns new object,         
//  or NULL either if there was no input waiting, or the recv was interrupted.
QXrapMsg * QXrapMsg::recvNowait (void *input)
{
    QXrapMsg *rv = new QXrapMsg (xrap_msg_recv_nowait (input));
    return rv;
}

///
//  Send the xrap_msg to the output, and destroy it
int QXrapMsg::send (void *output)
{
    int rv = xrap_msg_send (&self, output);
    return rv;
}

///
//  Send the xrap_msg to the output, and destroy it
int QXrapMsg::sendAgain (void *output)
{
    int rv = xrap_msg_send_again (self, output);
    return rv;
}

///
//  Get the xrap_msg id and printable command
int QXrapMsg::id ()
{
    int rv = xrap_msg_id (self);
    return rv;
}

///
//  
void QXrapMsg::setId (int id)
{
    xrap_msg_set_id (self, id);
    
}

///
//  Get/set the parent field
const QString QXrapMsg::parent ()
{
    const QString rv = QString (xrap_msg_parent (self));
    return rv;
}

///
//  
void QXrapMsg::setParent (const QString &param)
{
    xrap_msg_set_parent (self, "%s", param.toUtf8().data());
    
}

///
//  Get/set the content_type field
const QString QXrapMsg::contentType ()
{
    const QString rv = QString (xrap_msg_content_type (self));
    return rv;
}

///
//  
void QXrapMsg::setContentType (const QString &param)
{
    xrap_msg_set_content_type (self, "%s", param.toUtf8().data());
    
}

///
//  Get/set the parent field
const QString QXrapMsg::contentBody ()
{
    const QString rv = QString (xrap_msg_content_body (self));
    return rv;
}

///
//  
void QXrapMsg::setContentBody (const QString &param)
{
    xrap_msg_set_content_body (self, "%s", param.toUtf8().data());
    
}

///
//  Get/set the status_code field
quint16 QXrapMsg::statusCode ()
{
    uint16_t rv = xrap_msg_status_code (self);
    return rv;
}

///
//  
void QXrapMsg::setStatusCode (quint16 statusCode)
{
    xrap_msg_set_status_code (self, (uint16_t) statusCode);
    
}

///
//  Get/set the location field
const QString QXrapMsg::location ()
{
    const QString rv = QString (xrap_msg_location (self));
    return rv;
}

///
//  
void QXrapMsg::setLocation (const QString &param)
{
    xrap_msg_set_location (self, "%s", param.toUtf8().data());
    
}

///
//  Get/set the etag field
const QString QXrapMsg::etag ()
{
    const QString rv = QString (xrap_msg_etag (self));
    return rv;
}

///
//  
void QXrapMsg::setEtag (const QString &param)
{
    xrap_msg_set_etag (self, "%s", param.toUtf8().data());
    
}

///
//  Get/set the date_modified field
quint64 QXrapMsg::dateModified ()
{
    uint64_t rv = xrap_msg_date_modified (self);
    return rv;
}

///
//  
void QXrapMsg::setDateModified (quint64 dateModified)
{
    xrap_msg_set_date_modified (self, (uint64_t) dateModified);
    
}

///
//  Get/set the resource field
const QString QXrapMsg::resource ()
{
    const QString rv = QString (xrap_msg_resource (self));
    return rv;
}

///
//  
void QXrapMsg::setResource (const QString &param)
{
    xrap_msg_set_resource (self, "%s", param.toUtf8().data());
    
}

///
//  //  Get/set the parameters field
QZhash * QXrapMsg::parameters ()
{
    QZhash *rv = new QZhash (xrap_msg_parameters (self));
    return rv;
}

///
//  //  Get the parameters field and transfer ownership to caller
QZhash * QXrapMsg::getParameters ()
{
    QZhash *rv = new QZhash (xrap_msg_get_parameters (self));
    return rv;
}

///
//  
void QXrapMsg::setParameters (QZhash *parametersP)
{
    xrap_msg_set_parameters (self, &parametersP->self);
    
}

///
//  Get/set the parameters field
const QString QXrapMsg::parametersString (const QString &key, const QString &defaultValue)
{
    const QString rv = QString (xrap_msg_parameters_string (self, key.toUtf8().data(), defaultValue.toUtf8().data()));
    return rv;
}

///
//  
void QXrapMsg::parametersInsert (const QString &key, const QString &param)
{
    xrap_msg_parameters_insert (self, key.toUtf8().data(), "%s", param.toUtf8().data());
    
}

///
//  Get/set the if_modified_since field
quint64 QXrapMsg::ifModifiedSince ()
{
    uint64_t rv = xrap_msg_if_modified_since (self);
    return rv;
}

///
//  
void QXrapMsg::setIfModifiedSince (quint64 ifModifiedSince)
{
    xrap_msg_set_if_modified_since (self, (uint64_t) ifModifiedSince);
    
}

///
//  Get/set the if_none_match field
const QString QXrapMsg::ifNoneMatch ()
{
    const QString rv = QString (xrap_msg_if_none_match (self));
    return rv;
}

///
//  
void QXrapMsg::setIfNoneMatch (const QString &param)
{
    xrap_msg_set_if_none_match (self, "%s", param.toUtf8().data());
    
}

///
//  //  Get/set the metadata field
QZhash * QXrapMsg::metadata ()
{
    QZhash *rv = new QZhash (xrap_msg_metadata (self));
    return rv;
}

///
//  //  Get the metadata field and transfer ownership to caller
QZhash * QXrapMsg::getMetadata ()
{
    QZhash *rv = new QZhash (xrap_msg_get_metadata (self));
    return rv;
}

///
//  
void QXrapMsg::setMetadata (QZhash *metadataP)
{
    xrap_msg_set_metadata (self, &metadataP->self);
    
}

///
//  Get/set a value in the metadata dictionary
const QString QXrapMsg::metadataString (const QString &key, const QString &defaultValue)
{
    const QString rv = QString (xrap_msg_metadata_string (self, key.toUtf8().data(), defaultValue.toUtf8().data()));
    return rv;
}

///
//  
void QXrapMsg::metadataInsert (const QString &key, const QString &param)
{
    xrap_msg_metadata_insert (self, key.toUtf8().data(), "%s", param.toUtf8().data());
    
}

///
//  Get/set the if_unmodified_since field
quint64 QXrapMsg::ifUnmodifiedSince ()
{
    uint64_t rv = xrap_msg_if_unmodified_since (self);
    return rv;
}

///
//  
void QXrapMsg::setIfUnmodifiedSince (quint64 ifUnmodifiedSince)
{
    xrap_msg_set_if_unmodified_since (self, (uint64_t) ifUnmodifiedSince);
    
}

///
//  Get/set the if_match field
const QString QXrapMsg::ifMatch ()
{
    const QString rv = QString (xrap_msg_if_match (self));
    return rv;
}

///
//  
void QXrapMsg::setIfMatch (const QString &param)
{
    xrap_msg_set_if_match (self, "%s", param.toUtf8().data());
    
}

///
//  Get/set the status_text field
const QString QXrapMsg::statusText ()
{
    const QString rv = QString (xrap_msg_status_text (self));
    return rv;
}

///
//  
void QXrapMsg::setStatusText (const QString &param)
{
    xrap_msg_set_status_text (self, "%s", param.toUtf8().data());
    
}

///
//  Self test of this class.
void QXrapMsg::test (bool verbose)
{
    xrap_msg_test (verbose);
    
}
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
