#!/usr/bin/env bash

set -xe

./bootstrap/ocen -d -o build/ocen compiler/main.oc
./build/ocen "$@"
