#!/bin/bash

if [[ $# -eq 0 ]] ; then
    echo 'Usage: ./meta/gen_docs.sh <output json path>'
    exit 1
fi

cat > compiler/doc.oc << EOF
//* Dummy file to include all library documentation
//*
//* Nothing to see here

import std::buffer
import std::bufferio
import std::compact_map
import std::complex
import std::deque
import std::fft
import std::glut
import std::hash
import std::heap
import std::image
import std::json
import std::libc
import std::linkedlist
import std::map
import std::math
import std::sdl
import std::socket
import std::sort
import std::span
import std::thread
import std::value
import std::vec
import std::vector

// Compiler imports

import .ast::{ nodes, scopes, program }
import .passes
import .parser
import .utils
import .docgen
EOF

./bootstrap/ocen compiler/doc.oc --docs $1

rm compiler/doc.oc