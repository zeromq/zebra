.set GIT=https://github.com/zeromq/zwebrap
.sub 0MQ=ØMQ

# zwebrap - REST/HTTP to XRAP gateway.

## Contents

.toc 3

## Overview

zwebrap is a REST/HTTP to XRAP gateway.

### Scope and Goals

zwebrap is designed to take HTTP request for the common HTTP methods GET, POST, PUT and DELETE. It will convert these HTTP request into the XRAP format and pass it on to the request handlers which compose a response in the XRAP format which will be converted back into HTTP.

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

### Requirements

* libmicrohttpd (>= 0.9.40)
* libzmq (>= 4.1)
* czmq (>= 3.0.3)

### Installation

```sh
autogen.sh
configure
make && make check
make install
```

### Usage

This is the API provided by zwebrap v0.x, in alphabetical order.

.pull doc/zwr_microhttpd.doc
.pull doc/zwr_server.doc
.pull doc/zwr_client.doc

## Ownership and License

The contributors are listed in AUTHORS. This project uses the MPL v2 license, see LICENSE.

zwebrap uses the [C4.1 (Collective Code Construction Contract)](http://rfc.zeromq.org/spec:22) process for contributions.

To report an issue, use the [zwebrap issue tracker](https://github.com/zeromq/zproject/issues) at github.com.

### Hints to Contributors

Do read your code after you write it and ask, "Can I make this simpler?" We do use a nice minimalist and yet readable style. Learn it, adopt it, use it.

Before opening a pull request read our [contribution guidelines](https://github.com/zeromq/zwebrap/blob/master/CONTRIBUTING.md). Thanks!

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
