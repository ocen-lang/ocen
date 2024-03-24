#!/bin/bash

if [[ $# -eq 0 ]] ; then
    echo 'Usage: ./meta/gen_docs.sh <output json path>'
    exit 1
fi

cat > compiler/doc.oc << EOF
//* Dummy file to include all library documentation
//*
//* Nothing to see here

import std::{ vector, map, compact_map, heap, deque, set, linkedlist, variadic }
import std::{ buffer, bufferio, image, json, math, complex, fft, random, curl, og }
import std::{ value, vec, glut, socket, sort, thread, video_renderer, sv, fs, zlib, time }
import std::traits::{ hash, eq, compare }
import std::{ image::draw, hash::sha1, hash::sha256 }

import std::sdl::{ ttf, image as sdl_img, mixer as sdl_mixer, gfx as sdl_gfx }

// Compiler imports

import .ast::{ nodes, scopes, program }
import .passes
import .parser
import .utils
import .docgen
EOF

./bootstrap/ocen compiler/doc.oc --docs $1
EXIT_CODE=$?

rm compiler/doc.oc

exit $EXIT_CODE