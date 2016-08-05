#!/usr/bin/env bash

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  READ THE ZPROJECT/README.MD FOR INFORMATION ABOUT MAKING PERMANENT CHANGES. #
################################################################################

set -x
set -e

if [ "$BUILD_TYPE" == "default" ]; then
    mkdir tmp
    BUILD_PREFIX=$PWD/tmp

    CONFIG_OPTS=()
    CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include")
    CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
    CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include")
    CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
    CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
    CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")
    CONFIG_OPTS+=("--with-docs=no")
    CONFIG_OPTS+=("--quiet")

    # Clone and build dependencies
    wget http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.48.tar.gz
    tar -xzf $(basename "http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.48.tar.gz")
    cd $(basename "http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.48.tar.gz" .tar.gz)
    ./configure "${CONFIG_OPTS[@]}"
    make -j4
    make install
    cd ..
    git clone --quiet --depth 1 https://github.com/zeromq/libzmq libzmq
    cd libzmq
    git --no-pager log --oneline -n1
    if [ -e autogen.sh ]; then
        ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        ./buildconf 2> /dev/null
    fi
    ./configure "${CONFIG_OPTS[@]}"
    make -j4
    make install
    cd ..
    git clone --quiet --depth 1 https://github.com/zeromq/czmq czmq
    cd czmq
    git --no-pager log --oneline -n1
    if [ -e autogen.sh ]; then
        ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        ./buildconf 2> /dev/null
    fi
    ./configure "${CONFIG_OPTS[@]}"
    make -j4
    make install
    cd ..
    git clone --quiet --depth 1 https://github.com/curl/curl libcurl
    cd libcurl
    git --no-pager log --oneline -n1
    if [ -e autogen.sh ]; then
        ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        ./buildconf 2> /dev/null
    fi
    ./configure "${CONFIG_OPTS[@]}"
    make -j4
    make install
    cd ..

    # Build and check this project
    ./autogen.sh 2> /dev/null
    ./configure --enable-drafts=yes "${CONFIG_OPTS[@]}"
    export DISTCHECK_CONFIGURE_FLAGS="--enable-drafts=yes ${CONFIG_OPTS[@]}"
    make VERBOSE=1 distcheck

    # Build and check this project without DRAFT APIs
    make distclean
    git clean -f
    git reset --hard HEAD
    (
        ./autogen.sh 2> /dev/null
        ./configure --enable-drafts=no "${CONFIG_OPTS[@]}"
        export DISTCHECK_CONFIGURE_FLAGS="--enable-drafts=no ${CONFIG_OPTS[@]}" &&
        make VERBOSE=1 distcheck
    ) || exit 1
elif [ "$BUILD_TYPE" == "bindings" ]; then
    pushd "./bindings/${BINDING}" && ./ci_build.sh
else
    pushd "./builds/${BUILD_TYPE}" && REPO_DIR="$(dirs -l +1)" ./ci_build.sh
fi
