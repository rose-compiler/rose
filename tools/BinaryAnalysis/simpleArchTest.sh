#!/bin/bash
set -ex

INPUT="$1"
OUTPUT="${INPUT##*/}"

ANALYSIS_FLAGS="--isa=simple --use-semantics"
BINARY_SPECIMEN="map:0x1000=rx::${OUTPUT}.bin"

./simple-arch-assembler <"${INPUT}" >"${OUTPUT}.bin"

./bat-mem 	 --architectures=./libSimpleArch.so --format=hexdump $ANALYSIS_FLAGS "$BINARY_SPECIMEN"
./bat-dis 	 --architectures=./libSimpleArch.so $ANALYSIS_FLAGS "$BINARY_SPECIMEN" >&2
./bat-dis-simple --architectures=./libSimpleArch.so $ANALYSIS_FLAGS "$BINARY_SPECIMEN"

exit 0
