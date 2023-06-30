#!/bin/bash

initial=$1
if [ -z "$initial" ]; then
    initial=./bootstrap/ocen
fi

mkdir -p build

set -e

echo "[+] Testing 3-stage bootstrap for validity"
$initial compiler/main.oc -o build/stage1
./build/stage1 compiler/main.oc -o build/stage2
./build/stage2 compiler/main.oc -o build/stage3
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
    echo
    echo "[-] Error: Test suite failed"
    exit 1
fi


read -p "Are you sure you want to replace bootstrap/stage0.c? [y/N] " confirm
if [[ $confirm =~ ^[Yy]$ ]]; then
    echo "[+] cp build/stage3.c bootstrap/stage0.c"
    cp build/stage3.c bootstrap/stage0.c
    echo "[+] Creating debug version into /bootstrap/ocen"
    ./build/stage3 -s -d compiler/main.oc -o ./build/ocen
    cp build/ocen bootstrap/ocen
    echo "Done."
else
    echo "[-] Aborting"
    exit 1
fi
