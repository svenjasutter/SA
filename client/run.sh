#!/bin/bash

XMPP_CLIENT_BINARY=xmppclient

build() {
    if [ ! -d /app/build ]; then
        echo "::: Build directory does not exist. Creating..."
        mkdir -p /app/build
    fi

    if [ "$REBUILD" = "1" ]; then
        cd /app/build
        rm -rf *
    fi

    if [ ! -f "/app/build/$XMPP_CLIENT_BINARY" ]; then
        echo "::: $XMPP_CLIENT_BINARY binary not found. Building..."
        cd /app/build
        rm -rf *
        cmake ..
        make -j$(nproc)
    else
        echo "::: $XMPP_CLIENT_BINARY binary found. No need to rebuild."
    fi
}

build_pqc() {
    if [ "$REBUILD_KYBER" = "1" ] || [ ! -d "/pqc/ref/lib" ]; then
        cd /pqc
        sed -i '/^lib\/libpqcrystals_kyber768_ref\.so:/!b;n;n;s|symmetric-shake.c|& randombytes.c fips202.c|' ref/Makefile
        cd ref
        make clean
        make
        make shared
        ldconfig
    fi
}

build_pqc
build

if [ "$TOR_BG" = "1" ]; then
    echo "::: Tor in Background!..."
    tor > /dev/null 2>&1 &
else
    echo "::: Tor..."
    tor &
fi

cd /app/build
./$XMPP_CLIENT_BINARY
