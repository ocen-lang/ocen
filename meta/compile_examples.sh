#!/usr/bin/env bash

set -xe

ocen examples/jpegify.oc -o build/jpegify -n
ocen examples/raytrace.oc -o build/raytrace -n
ocen examples/sdl_raytrace.oc -o build/sdl_raytrace -n
