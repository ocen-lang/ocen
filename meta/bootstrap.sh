#!/bin/bash

mkdir -p build

set -e

echo "[+] Building stage 1 compiler"
gcc -o build/stage1 bootstrap/stage0.c -lm
echo "[+] Building stage 2 compiler"
./build/stage1 compiler/main.ae -o build/stage2
echo "[+] Building stage 3 compiler"
./build/stage2 compiler/main.ae -o build/stage3
echo "[+] Comparing stage 2 and 3"

if diff build/stage2.c build/stage3.c; then
    ./build/stage3 -s -d compiler/main.ae -o ./build/aecor
    mv build/aecor bootstrap/aecor
    echo "[+] Bootstrap successful: Use ./bootstrap/aecor"
else
    echo "[-] Error: Stage 2 and 3 are different"
    exit 1
fi
