#! /bin/bash

PROTOBUF_DIR=$HOME/.protobuf

if [ -z "$(ls -A $PROTOBUF_DIR)" ]; then
    echo "Compiling protobuf"
    # Make sure you grab the latest version
    curl -OL https://github.com/google/protobuf/releases/download/v3.7.1/protobuf-all-3.7.1.zip
    unzip protobuf-all-3.7.1.zip
    cd protobuf-3.7.1
    ./configure --prefix=$HOME/.protobuf
    make
    make check
    sudo make install
    sudo ldconfig
else
    echo "Using cache"
fi

export PATH=$PATH:$PROTOBUF_DIR/bin
protoc --version
