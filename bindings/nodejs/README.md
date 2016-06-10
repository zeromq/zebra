# Node.js Binding for zebra

This is a development kit. Note: this README is generated automatically
by zproject from project.xml. Please DO NOT modify by hand except for test
purposes.

## Prerequisites

### Node.js

* You need Python (v2.7 recommended, v3.x not supported)
* You need (I recommend) nvm and Node.js.
* If your Linux has an existing 'node' command, `sudo apt-get remove node`.
* In every terminal, or .bashrc: `nvm use v5.5.0`

To install the necessary Node tools:

```
sudo apt-get update
sudo apt-get install build-essential libssl-dev
curl https://raw.githubusercontent.com/creationix/nvm/v0.11.1/install.sh | bash
# close terminal, re-open
nvm ls-remote
nvm install v5.5.0
npm install -g nan
npm install -g node-ninja
npm install -g prebuild
npm install -g bindings
```

To build:

```
mkdir -p $HOME/temp
cd $HOME/temp
git clone https://github.com/zeromq/zebra
cd zebra/bindings/nodejs
#   Clones dependencies, builds everything
./build.sh
```

## API

This is a wrapping of the native C libzebra library. See binding.cc for the code.

### The ZebClient class - zeb_broker client implementation for both clients and handlers

    Codec class for zeb_client.

Constructor:

```
var zebra = require ('bindings')('zebra')
var my_zeb_client = new zebra.ZebClient ()
```

You *must* call the destructor on every ZebClient instance:

```
my_zeb_client.destroy ()
```

Methods:

```
zactor my_zeb_client.actor ()
```

Return actor, when caller wants to work with multiple actors and/or
input sockets asynchronously.

```
zsock my_zeb_client.msgpipe ()
```

Return message pipe for asynchronous message I/O. In the high-volume case,
we send methods and get replies to the actor, in a synchronous manner, and
we send/recv high volume message data to a second pipe, the msgpipe. In
the low-volume case we can do everything over the actor pipe, if traffic
is never ambiguous.

```
boolean my_zeb_client.connected ()
```

Return true if client is currently connected, else false. Note that the
client will automatically re-connect if the server dies and restarts after
a successful first connection.

```
integer my_zeb_client.connect (String, Number, String)
```

Connect to server endpoint, with specified timeout in msecs (zero means wait    
forever). Constructor succeeds if connection is successful. The caller may      
specify its address.                                                            
Returns >= 0 if successful, -1 if interrupted.

```
integer my_zeb_client.setHandler (String, String)
```

Offer to handle particular XRAP requests, where the route matches request's     
resource.                                                                       
Returns >= 0 if successful, -1 if interrupted.

```
integer my_zeb_client.request (Number, Zmsg)
```

No explanation                                                                  
Returns >= 0 if successful, -1 if interrupted.

```
integer my_zeb_client.deliver (Zuuid, Zmsg)
```

Send XRAP DELIVER message to server, takes ownership of message
and destroys message when done sending it.

```
zmsg my_zeb_client.recv ()
```

Receive message from server; caller destroys message when done

```
string my_zeb_client.command ()
```

Return last received command. Can be one of these values:
    "XRAP DELIVER"

```
integer my_zeb_client.status ()
```

Return last received status

```
string my_zeb_client.reason ()
```

Return last received reason

```
zuuid my_zeb_client.sender ()
```

Return last received sender

```
zmsg my_zeb_client.content ()
```

Return last received content

```
nothing my_zeb_client.setVerbose (Boolean)
```

Enable verbose tracing (animation) of state machine activity.

```
nothing my_zeb_client.test (Boolean)
```

Self test of this class.

### The ZebHandler class - Handler for XRAP requests

Constructor:

```
var zebra = require ('bindings')('zebra')
var my_zeb_handler = new zebra.ZebHandler ()
```

Methods:

```
integer my_zeb_handler.addOffer (Zactor, Number, String, String)
```

Add a new offer this handler will handle. Returns 0 if successful,
otherwise -1.
The content type parameter is optional and is used to
filter requests upon their requested (GET) or provided (POST/PUT)
content's type. The content type parameter may be a regex which is
useful for GET offers that can supply resources in different formats.
If the client did request multiple content types then the first match
will be chosen and applied to the request. All other content types are
drooped. If the request's content type does not match it is automatically
rejected with the error code 406 (Not acceptable).

```
nothing my_zeb_handler.test (Boolean)
```

Self test of this class.

### The XrapMsg class -     XRAP serialization over ZMTP

/*  These are the xrap_msg messages:

    POST - Create a new, dynamically named resource in some parent.
        parent              string      Schema/type/name
        content_type        string      Content type
        content_body        longstr     New resource specification

    POST_OK - Success response for POST.
        status_code         number 2    Response status code 2xx
        location            string      Schema/type/name
        etag                string      Opaque hash tag
        date_modified       number 8    Date and time modified
        content_type        string      Content type
        content_body        longstr     Resource contents

    GET - Retrieve a known resource.
        resource            string      Schema/type/name
        if_modified_since   number 8    GET if more recent
        if_none_match       string      GET if changed
        content_type        string      Desired content type

    GET_OK - Success response for GET.
        status_code         number 2    Response status code 2xx
        content_type        string      Actual content type
        content_body        longstr     Resource specification

    GET_EMPTY - Conditional GET returned 304 Not Modified.
        status_code         number 2    Response status code 3xx

    PUT - Update a known resource.
        resource            string      Schema/type/name
        if_unmodified_since  number 8   Update if same date
        if_match            string      Update if same ETag
        content_type        string      Content type
        content_body        longstr     New resource specification

    PUT_OK - Success response for PUT.
        status_code         number 2    Response status code 2xx
        location            string      Schema/type/name
        etag                string      Opaque hash tag
        date_modified       number 8    Date and time modified

    DELETE - Remove a known resource.
        resource            string      schema/type/name
        if_unmodified_since  number 8   DELETE if same date
        if_match            string      DELETE if same ETag

    DELETE_OK - Success response for DELETE.
        status_code         number 2    Response status code 2xx

    ERROR - Error response for any request.
        status_code         number 2    Response status code, 4xx or 5xx
        status_text         string      Response status text
*/

Constructor:

```
var zebra = require ('bindings')('zebra')
var my_xrap_msg = new zebra.XrapMsg (Number)
```

You *must* call the destructor on every XrapMsg instance:

```
my_xrap_msg.destroy ()
```

Methods:

```
xrap_msg my_xrap_msg.decode (Zmsg)
```

Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
the message could not be parsed, or was NULL. Destroys msg and
nullifies the msg reference.

```
zmsg my_xrap_msg.encode (XrapMsg)
```

Encode xrap_msg into zmsg and destroy it. Returns a newly created
object or NULL if error. Use when not in control of sending the message.

```
integer my_xrap_msg.id ()
```

Get the xrap_msg id and printable command

```
nothing my_xrap_msg.setId (Number)
```



```
string my_xrap_msg.parent ()
```

Get/set the parent field

```
nothing my_xrap_msg.setParent (String)
```



```
string my_xrap_msg.contentType ()
```

Get/set the content_type field

```
nothing my_xrap_msg.setContentType (String)
```



```
string my_xrap_msg.contentBody ()
```

Get/set the parent field

```
nothing my_xrap_msg.setContentBody (String)
```



```
number my_xrap_msg.statusCode ()
```

Get/set the status_code field

```
nothing my_xrap_msg.setStatusCode (Number)
```



```
string my_xrap_msg.location ()
```

Get/set the location field

```
nothing my_xrap_msg.setLocation (String)
```



```
string my_xrap_msg.etag ()
```

Get/set the etag field

```
nothing my_xrap_msg.setEtag (String)
```



```
number my_xrap_msg.dateModified ()
```

Get/set the date_modified field

```
nothing my_xrap_msg.setDateModified (Number)
```



```
string my_xrap_msg.resource ()
```

Get/set the resource field

```
nothing my_xrap_msg.setResource (String)
```



```
zhash my_xrap_msg.parameters ()
```

//  Get/set the parameters field

```
zhash my_xrap_msg.getParameters ()
```

//  Get the parameters field and transfer ownership to caller

```
nothing my_xrap_msg.setParameters (Zhash)
```



```
string my_xrap_msg.parametersString (String, String)
```

Get/set the parameters field

```
nothing my_xrap_msg.parametersInsert (String, String)
```



```
number my_xrap_msg.ifModifiedSince ()
```

Get/set the if_modified_since field

```
nothing my_xrap_msg.setIfModifiedSince (Number)
```



```
string my_xrap_msg.ifNoneMatch ()
```

Get/set the if_none_match field

```
nothing my_xrap_msg.setIfNoneMatch (String)
```



```
zhash my_xrap_msg.metadata ()
```

//  Get/set the metadata field

```
zhash my_xrap_msg.getMetadata ()
```

//  Get the metadata field and transfer ownership to caller

```
nothing my_xrap_msg.setMetadata (Zhash)
```



```
string my_xrap_msg.metadataString (String, String)
```

Get/set a value in the metadata dictionary

```
nothing my_xrap_msg.metadataInsert (String, String)
```



```
number my_xrap_msg.ifUnmodifiedSince ()
```

Get/set the if_unmodified_since field

```
nothing my_xrap_msg.setIfUnmodifiedSince (Number)
```



```
string my_xrap_msg.ifMatch ()
```

Get/set the if_match field

```
nothing my_xrap_msg.setIfMatch (String)
```



```
string my_xrap_msg.statusText ()
```

Get/set the status_text field

```
nothing my_xrap_msg.setStatusText (String)
```



```
nothing my_xrap_msg.test (Boolean)
```

Self test of this class.

### The XrapTraffic class - Set the content field, transferring ownership from caller

Constructor:

```
var zebra = require ('bindings')('zebra')
var my_xrap_traffic = new zebra.XrapTraffic ()
```

You *must* call the destructor on every XrapTraffic instance:

```
my_xrap_traffic.destroy ()
```

Methods:

```
integer my_xrap_traffic.recv (Zsock)
```

Receive a xrap_traffic from the socket. Returns 0 if OK, -1 if
there was an error. Blocks if there is no message waiting.

```
integer my_xrap_traffic.send (Zsock)
```

Send the xrap_traffic to the output socket, does not destroy it

```
nothing my_xrap_traffic.print ()
```

Print contents of message to stdout

```
zframe my_xrap_traffic.routingId ()
```

Get the message routing id, as a frame

```
nothing my_xrap_traffic.setRoutingId (Zframe)
```

Set the message routing id from a frame

```
integer my_xrap_traffic.id ()
```

Get the xrap_traffic message id

```
nothing my_xrap_traffic.setId (Number)
```

Set the xrap_traffic message id

```
string my_xrap_traffic.command ()
```

Get the xrap_traffic message id as printable text

```
string my_xrap_traffic.address ()
```

Get the address field

```
nothing my_xrap_traffic.setAddress (String)
```

Set the address field

```
number my_xrap_traffic.timeout ()
```

Get the timeout field

```
nothing my_xrap_traffic.setTimeout (Number)
```

Set the timeout field

```
zmsg my_xrap_traffic.content ()
```

Get a copy of the content field

```
zmsg my_xrap_traffic.getContent ()
```

Get the content field and transfer ownership to caller

```
nothing my_xrap_traffic.setContent (Zmsg)
```



```
string my_xrap_traffic.route ()
```

Get the route field

```
nothing my_xrap_traffic.setRoute (String)
```

Set the route field

```
string my_xrap_traffic.method ()
```

Get the method field

```
nothing my_xrap_traffic.setMethod (String)
```

Set the method field

```
zuuid my_xrap_traffic.sender ()
```

Get the sender field

```
zuuid my_xrap_traffic.getSender ()
```

Get the sender field and transfer ownership to caller

```
nothing my_xrap_traffic.setSender (Zuuid)
```

Set the sender field

```
number my_xrap_traffic.statusCode ()
```

Get the status_code field

```
nothing my_xrap_traffic.setStatusCode (Number)
```

Set the status_code field

```
string my_xrap_traffic.statusReason ()
```

Get the status_reason field

```
nothing my_xrap_traffic.setStatusReason (String)
```

Set the status_reason field

```
nothing my_xrap_traffic.test (Boolean)
```

Self test of this class.
