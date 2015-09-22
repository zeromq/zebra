#include <jni.h>
#include <stdio.h>
#include "org_zeromq_ZwrClient.h"
#include "zwebrap.h"

JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_destroy (JNIEnv *env, jobject thisObj) {
}

//  Print properties of the zwr_client object.
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_print (JNIEnv *env, jobject thisObj) {
}

//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.                                      
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_actor (JNIEnv *env, jobject thisObj) {
}

//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In   
//  the low-volume case we can do everything over the actor pipe, if traffic  
//  is never ambiguous.                                                       
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_msgpipe (JNIEnv *env, jobject thisObj) {
}

//  Return true if client is currently connected, else false. Note that the   
//  client will automatically re-connect if the server dies and restarts after
//  a successful first connection.                                            
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_connected (JNIEnv *env, jobject thisObj) {
}

//  Connect to server endpoint, with specified timeout in msecs (zero means wait
//  forever). Constructor succeeds if connection is successful. The caller may  
//  specify its address.                                                        
//  Returns >= 0 if successful, -1 if interrupted.                              
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_connect (JNIEnv *env, jobject thisObj) {
}

//  Offer to handle particular XRAP requests, where the route matches request's
//  resource.                                                                  
//  Returns >= 0 if successful, -1 if interrupted.                             
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_setHandler (JNIEnv *env, jobject thisObj) {
}

//  No explanation                                
//  Returns >= 0 if successful, -1 if interrupted.
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_request (JNIEnv *env, jobject thisObj) {
}

//  Send XRAP DELIVER message to server, takes ownership of message
//  and destroys message when done sending it.                     
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_deliver (JNIEnv *env, jobject thisObj) {
}

//  Receive message from server; caller destroys message when done
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_recv (JNIEnv *env, jobject thisObj) {
}

//  Return last received command. Can be one of these values:
//      "XRAP DELIVER"                                       
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_command (JNIEnv *env, jobject thisObj) {
}

//  Return last received status
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_status (JNIEnv *env, jobject thisObj) {
}

//  Return last received reason
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_reason (JNIEnv *env, jobject thisObj) {
}

//  Return last received sender
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_sender (JNIEnv *env, jobject thisObj) {
}

//  Return last received content
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_content (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_ZwrClient_test (JNIEnv *env, jobject thisObj) {
}
