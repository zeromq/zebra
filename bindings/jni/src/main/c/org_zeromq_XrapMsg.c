#include <jni.h>
#include <stdio.h>
#include "org_zeromq_XrapMsg.h"
#include "zwebrap.h"

JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_destroy (JNIEnv *env, jobject thisObj) {
}

//  Print properties of the xrap msg object.
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_print (JNIEnv *env, jobject thisObj) {
}

//  Parse a zmsg_t and decides whether it is xrap_msg. Returns   
//  true if it is, false otherwise. Doesn't destroy or modify the
//  original message.                                            
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_isXrapMsg (JNIEnv *env, jobject thisObj) {
}

//  Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and
//  nullifies the msg reference.                                  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_decode (JNIEnv *env, jobject thisObj) {
}

//  Encode xrap_msg into zmsg and destroy it. Returns a newly created       
//  object or NULL if error. Use when not in control of sending the message.
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_encode (JNIEnv *env, jobject thisObj) {
}

//  Receive and parse a xrap_msg from the socket. Returns new object, 
//  or NULL if error. Will block if there's no message waiting.       
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_recv (JNIEnv *env, jobject thisObj) {
}

//  Receive and parse a xrap_msg from the socket. Returns new object,         
//  or NULL either if there was no input waiting, or the recv was interrupted.
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_recvNowait (JNIEnv *env, jobject thisObj) {
}

//  Send the xrap_msg to the output, and destroy it
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_send (JNIEnv *env, jobject thisObj) {
}

//  Send the xrap_msg to the output, and destroy it
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_sendAgain (JNIEnv *env, jobject thisObj) {
}

//  Get the xrap_msg id and printable command
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_id (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setId (JNIEnv *env, jobject thisObj) {
}

//  Get/set the parent field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_parent (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setParent (JNIEnv *env, jobject thisObj) {
}

//  Get/set the content_type field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_contentType (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setContentType (JNIEnv *env, jobject thisObj) {
}

//  Get/set the parent field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_contentBody (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setContentBody (JNIEnv *env, jobject thisObj) {
}

//  Get/set the status_code field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_statusCode (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setStatusCode (JNIEnv *env, jobject thisObj) {
}

//  Get/set the location field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_location (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setLocation (JNIEnv *env, jobject thisObj) {
}

//  Get/set the etag field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_etag (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setEtag (JNIEnv *env, jobject thisObj) {
}

//  Get/set the date_modified field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_dateModified (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setDataModified (JNIEnv *env, jobject thisObj) {
}

//  Get/set the resource field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_resource (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setResource (JNIEnv *env, jobject thisObj) {
}

//  Get/set the parameters field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_parametersString (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_parametersInsert (JNIEnv *env, jobject thisObj) {
}

//  Get/set the if_modified_since field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_ifModifiedSince (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setIfModifiedSince (JNIEnv *env, jobject thisObj) {
}

//  Get/set the if_none_match field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_ifNoneMatch (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setIfNoneMatch (JNIEnv *env, jobject thisObj) {
}

//  Get/set the parameters field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_metadataString (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_metadataInsert (JNIEnv *env, jobject thisObj) {
}

//  Get/set the if_unmodified_since field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_ifUnmodifiedSince (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setIfUnmodifiedSince (JNIEnv *env, jobject thisObj) {
}

//  Get/set the if_match field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_ifMatch (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setIfMatch (JNIEnv *env, jobject thisObj) {
}

//  Get/set the status_text field
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_statusText (JNIEnv *env, jobject thisObj) {
}

//  
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_setStatusText (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_XrapMsg_test (JNIEnv *env, jobject thisObj) {
}
