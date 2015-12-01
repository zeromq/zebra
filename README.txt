.set GIT=https://github.com/zeromq/zwebrap
.sub 0MQ=ØMQ

# zwebrap - REST/HTTP to XRAP gateway.

## Contents

.toc 3

## Overview

### Scope and Goals

zwebrap is designed to take HTTP requests for the common HTTP methods GET, POST, PUT and DELETE and convert them into the [XRAP](http://rfc.zeromq.org/spec:40) format. The converted messages will be passed to the request handlers which compose a response in the XRAP format which will be converted back into HTTP. To allow handler to come and go a they please, they need to register at a dispatcher which will forward XRAP messages both ways.

[diagram]
                   HTTP
                    ^
                    | GET, POST,
                    | PUT, DELETE
                    v
  Converts   +----------------+
  HTTP to    | zwr_microhttpd |
  XRAP.      |  (webserver)   |
             +----------------+
                    ^
                    |
                    v
             +--------------+  Dispatches xrap msg
             |  zwr_server  |  to the handlers, based
             | (dispatcher) |  on their offerings
             +--------------+
                 ^      ^
                 |      |
           +-----+      +-----+
           |                  |
           v                  v
  +----------------+  +----------------+
  | XRAP Handler 1 |  | XRAP Handler 2 |  Offer at least one
  |  (zwr_client)  |  |  (zwr_client)  |  service for at least
  +----------------+  +----------------+  one HTTP method.
[/diagram]

## Ownership and License

The contributors are listed in AUTHORS. This project uses the MPL v2 license, see LICENSE.

zwebrap uses the [C4.1 (Collective Code Construction Contract)](http://rfc.zeromq.org/spec:22) process for contributions.

zwebrap uses the [CLASS (C Language Style for Scalabilty)](http://rfc.zeromq.org/spec:21) guide for code style.

To report an issue, use the [zwebrap issue tracker](https://github.com/zeromq/zwebrap/issues) at github.com.

## Using zwebrap

### Requirements

* libmicrohttpd (>= 0.9.38)
* libzmq (>= 4.1)
* czmq (>= 3.0.3)
* libcurl [optional, to run HTTP tests]

### Building and Installing

Here's how to build zwebrap, including libmicrohttpd, libzmq and czmq:

    wget http://ftpmirror.gnu.org/libmicrohttpd/libmicrohttpd-0.9.38.tar.gz
    tar -xvf libmicrohttpd-0.9.38.tar.gz
    cd libmicrohttpd-0.9.38
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

    git clone git://github.com/zeromq/libzmq.git
    cd libzmq
    ./autogen.sh
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

    git clone git://github.com/zeromq/czmq.git
    cd czmq
    ./autogen.sh
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

    git clone git://github.com/zeromq/zwebrap.git
    cd zwebrap
    ./autogen.sh
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

### User information

#### User Agent Required

All HTTP requests MUST include a valid User-Agent header. Requests with no User-Agent header will be rejected. A good User-Agent header value is the name of your application.

#### Rate Limiting

To allow to compensate hardware limitations or to mitigate DOS attacks zwebrap has a built in rate limiting which allows to limit the number of request within an time interval.

You can check the returned HTTP headers of any HTTP request to see your current rate limit status:

```
HTTP/1.1 200 OK
Status: 200 OK
X-RateLimit-Limit: 10
X-RateLimit-Remaining: 5
X-RateLimit-Reset: 22
```

Once you go over the rate limit you will receive an 403 Forbidden error.

#### Conditional requests

XRAP allows responses to return an ETag header as well as a Last-Modified header. You can use the values of these headers to make subsequent requests to those resources using the If-None-Match and If-Modified-Since headers, respectively. If the resource has not changed, the handler might return a 304 Not Modified. Also note: making a conditional request and receiving a 304 response does not count against the Rate Limit which zwebrap takes automatically care of.

### Usage

This is the API provided by zwebrap v0.x, in alphabetical order.

.pull doc/zeb_handler.doc
.pull doc/zwr_microhttpd.doc
.pull doc/zwr_server.doc
.pull doc/zwr_client.doc

### Hints to Contributors

Read the CLASS style guide please, and write your code to make it indistinguishable from the rest of the code in the library. That is the only real criteria for good style: it's invisible.

Do read your code after you write it and ask, "Can I make this simpler?" We do use a nice minimalist and yet readable style. Learn it, adopt it, use it.

Before opening a pull request read our [contribution guidelines](https://github.com/zeromq/zwebrap/blob/master/CONTRIBUTING.md). Thanks!

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
