#!/bin/bash
set -ex

INPUT="$1"
OUTPUT="${INPUT##*/}"

./simple-arch-assembler <"${INPUT}" >"${OUTPUT}.bin"
./bat-ana -o "${OUTPUT}.rba" --architectures=./libSimpleArch.so --isa=simple --use-semantics map:0x1000=rx::"${OUTPUT}.bin"

./bat-mem 	 --architectures=./libSimpleArch.so --format=hexdump "${OUTPUT}.rba"
./bat-dis 	 --architectures=./libSimpleArch.so "${OUTPUT}.rba" >&2
./bat-dis-simple --architectures=./libSimpleArch.so "${OUTPUT}.rba"

exit 0
