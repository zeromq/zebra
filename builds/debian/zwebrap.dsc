Format:         1.0
Source:         zwebrap
Version:        0.2.0-1
Binary:         libzwebrap0, zwebrap-dev
Architecture:   any all
Maintainer:     John Doe <John.Doe@example.com>
Standards-Version: 3.9.5
Build-Depends: bison, debhelper (>= 8),
    pkg-config,
    automake,
    autoconf,
    libtool,
    libzmq4-dev,
    libczmq-dev,
    libmicrohttpd-dev,
    libcurl-dev,
    dh-autoreconf

Package-List:
 libzwebrap0 deb net optional arch=any
 zwebrap-dev deb libdevel optional arch=any

