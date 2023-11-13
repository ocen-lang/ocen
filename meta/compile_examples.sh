#!/usr/bin/env bash

set -xe

if [ -z $1 ]; then
    OCEN=ocen
else
    OCEN=$1
fi

$OCEN examples/jpegify.oc -o build/jpegify -n
$OCEN examples/raytrace.oc -o build/raytrace -n
$OCEN examples/sdl_raytrace.oc -o build/sdl_raytrace -n
