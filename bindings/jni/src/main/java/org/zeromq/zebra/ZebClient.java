/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
package org.zeromq.zebra;
import org.zeromq.czmq.*;

public class ZebClient implements AutoCloseable{
    static {
        try {
            System.loadLibrary ("zebrajni");
        }
        catch (Exception e) {
            System.exit (-1);
        }
    }
    public long self;
    /*
    Create a new zeb_client, return the reference if successful,   
    or NULL if construction failed due to lack of available memory.
    */
    native static long __new ();
    public ZebClient () {
        /*  TODO: if __new fails, self is null...            */
        self = __new ();
    }
    public ZebClient (long pointer) {
        self = pointer;
    }
    /*
    Destroy the zeb_client and free all memory used by the object.
    */
    native static void __destroy (long self);
    @Override
    public void close () {
        __destroy (self);
        self = 0;
    }
    /*
    Return actor, when caller wants to work with multiple actors and/or
    input sockets asynchronously.                                      
    */
    native static long __actor (long self);
    public Zactor actor () {
        return new Zactor (__actor (self));
    }
    /*
    Return message pipe for asynchronous message I/O. In the high-volume case,
    we send methods and get replies to the actor, in a synchronous manner, and
    we send/recv high volume message data to a second pipe, the msgpipe. In   
    the low-volume case we can do everything over the actor pipe, if traffic  
    is never ambiguous.                                                       
    */
    native static long __msgpipe (long self);
    public Zsock msgpipe () {
        return new Zsock (__msgpipe (self));
    }
    /*
    Return true if client is currently connected, else false. Note that the   
    client will automatically re-connect if the server dies and restarts after
    a successful first connection.                                            
    */
    native static boolean __connected (long self);
    public boolean connected () {
        return __connected (self);
    }
    /*
    Connect to server endpoint, with specified timeout in msecs (zero means wait    
    forever). Constructor succeeds if connection is successful. The caller may      
    specify its address.                                                            
    Returns >= 0 if successful, -1 if interrupted.                                  
    */
    native static int __connect (long self, String endpoint, int timeout, String address);
    public int connect (String endpoint, int timeout, String address) {
        return __connect (self, endpoint, timeout, address);
    }
    /*
    Offer to handle particular XRAP requests, where the route matches request's     
    resource.                                                                       
    Returns >= 0 if successful, -1 if interrupted.                                  
    */
    native static int __setHandler (long self, String method, String route);
    public int setHandler (String method, String route) {
        return __setHandler (self, method, route);
    }
    /*
    No explanation                                                                  
    Returns >= 0 if successful, -1 if interrupted.                                  
    */
    native static int __request (long self, int timeout, long content);
    public int request (int timeout, Zmsg content) {
        return __request (self, timeout, content.self);
    }
    /*
    Send XRAP DELIVER message to server, takes ownership of message
    and destroys message when done sending it.                     
    */
    native static int __deliver (long self, long sender, long content);
    public int deliver (Zuuid sender, Zmsg content) {
        return __deliver (self, sender.self, content.self);
    }
    /*
    Receive message from server; caller destroys message when done
    */
    native static long __recv (long self);
    public Zmsg recv () {
        return new Zmsg (__recv (self));
    }
    /*
    Return last received command. Can be one of these values:
        "XRAP DELIVER"                                       
    */
    native static String __command (long self);
    public String command () {
        return __command (self);
    }
    /*
    Return last received status
    */
    native static int __status (long self);
    public int status () {
        return __status (self);
    }
    /*
    Return last received reason
    */
    native static String __reason (long self);
    public String reason () {
        return __reason (self);
    }
    /*
    Return last received sender
    */
    native static long __sender (long self);
    public Zuuid sender () {
        return new Zuuid (__sender (self));
    }
    /*
    Return last received content
    */
    native static long __content (long self);
    public Zmsg content () {
        return new Zmsg (__content (self));
    }
    /*
    Enable verbose tracing (animation) of state machine activity.
    */
    native static void __setVerbose (long self, boolean verbose);
    public void setVerbose (boolean verbose) {
        __setVerbose (self, verbose);
    }
    /*
    Self test of this class.
    */
    native static void __test (boolean verbose);
    public static void test (boolean verbose) {
        __test (verbose);
    }
}
