Format:         1.0
Source:         zebra
Version:        0.2.0-1
Binary:         libzebra0, zebra-dev
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
    libsodium-dev,
    dh-autoreconf

Package-List:
 libzebra0 deb net optional arch=any
 zebra-dev deb libdevel optional arch=any

