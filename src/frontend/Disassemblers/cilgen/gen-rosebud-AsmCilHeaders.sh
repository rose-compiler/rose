#!/usr/bin/env bash

# jsonMacro from https://rosecompiler2.llnl.gov/gitlab/pirkelbauer2/jsontools
#~ jsonMacro blueprint/rosebud-AsmCilHeaders_h.txt cil.json >headers.h

# create output directory
mkdir -p asm

csplit headers.h -s --elide-empty-files --prefix="asmcil-" --suffix-format="%03d.h" "/^-- cut:Sg/" {*}

for file in asmcil-*.h
do
  newfile=`head -1 $file | cut -d ':' -f 2`
  tail -n +2 "$file" >"asm/$newfile"
  sed -i 's/[ \t]*$//' "asm/$newfile"
  rm "$file"
done

#~ rm headers.h
