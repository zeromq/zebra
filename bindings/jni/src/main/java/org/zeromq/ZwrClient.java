package org.zeromq.zwebrap;

public class ZwrClient {
    public native void create ();
    public native void destroy ();
    public native boolean connected ();
    public native int setHandler (String Method, String Route);
    public native String command ();
    public native int status ();
    public native String reason ();
    public native void test (boolean Verbose);
}
