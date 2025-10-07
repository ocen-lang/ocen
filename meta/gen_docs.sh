#!/bin/bash

if [[ $# -eq 0 ]] ; then
    echo 'Usage: ./meta/gen_docs.sh <output json path>'
    exit 1
fi

cat > compiler/doc.oc << EOF
//* Dummy file to include all library documentation
//*
//* Nothing to see here

import std::{
    bencode,
    bitio,
    buffer,
    bump_alloc,
    compact_map,
    complex,
    curl,
    deque,
    disjoint_set,
    fcntl,
    fft,
    fs,
    gc,
    glut,
    hash::sha1,
    hash::sha256,
    heap,
    huffman,
    image::{ draw, png, ppm, qoi },
    json,
    linkedlist,
    logging,
    map,
    math,
    matrix,
    mem,
    midi,
    og,
    option,
    process,
    random,
    readline,
    result,
    set,
    setjmp,
    signal,
    socket,
    sort,
    sv,
    testing,
    thread,
    time,
    traits::{ hash, eq, compare },
    value,
    variadic,
    vec,
    vector,
    video_renderer,
    argparse,
    zlib,
}

import std::sdl::{
    ttf,
    image as sdl_img,
    mixer as sdl_mixer,
    gfx as sdl_gfx
}

// Compiler imports

import .ast::{ nodes, scopes, program }
import .passes
import .parser
import .utils
import .docgen
import .lsp
EOF

export OCEN_ROOT=$(pwd)
if [[ -z "$OCEN" ]]; then
    export OCEN=./bootstrap/ocen
fi
$OCEN compiler/doc.oc --docs $1
EXIT_CODE=$?

rm compiler/doc.oc

exit $EXIT_CODE
