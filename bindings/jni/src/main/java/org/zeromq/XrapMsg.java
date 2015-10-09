package org.zeromq.zwebrap;

public class XrapMsg {
    public native void create ();
    public native void destroy ();
    public native void print ();
    public native boolean isXrapMsg ();
    public native int sendAgain (void Output);
    public native int id ();
    public native void setId (int Id);
    public native String parent ();
    public native String contentType ();
    public native String contentBody ();
    public native String location ();
    public native String etag ();
    public native String resource ();
    public native String parametersString (String Key, String DefaultValue);
    public native String ifNoneMatch ();
    public native String metadataString (String Key, String DefaultValue);
    public native String ifMatch ();
    public native String statusText ();
    public native void test (boolean Verbose);
}
