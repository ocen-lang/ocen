#!/bin/bash

initial=$1
if [ -z "$initial" ]; then
    initial=./bootstrap/aecor
fi

mkdir -p build

set -e

echo "[+] Testing 3-stage bootstrap for validity"
$initial selfhost/main.ae -o build/stage1
./build/stage1 selfhost/main.ae -o build/stage2
./build/stage2 selfhost/main.ae -o build/stage3
if diff build/stage2.c build/stage3.c; then
    echo "[+] Verification successful!"
    echo
else
    echo "[-] Error: Stage 2 and 3 are different, pleaes verify manually"
    exit 1
fi

echo "[+] Running test suite"
if python3 meta/test.py -c ./build/stage3; then
    echo
else
    read -p "Test Suite failed. Override? [y/N] " confirm
    if [[ $confirm =~ ^[Yy]$ ]]; then
        echo "[+] Overriding... "
    else
        echo "[-] Aborting"
        exit 1
    fi
fi


read -p "Are you sure you want to replace bootstrap/stage0.c? [y/N] " confirm
if [[ $confirm =~ ^[Yy]$ ]]; then
    echo "[+] cp build/stage3.c bootstrap/stage0.c"
    cp build/stage3.c bootstrap/stage0.c
    echo "[+] Creating debug version into /bootstrap/aecor"
    ./build/stage3 -s -d selfhost/main.ae -o ./build/aecor
    cp build/aecor bootstrap/aecor
    echo "Done."
else
    echo "[-] Aborting"
    exit 1
fi
