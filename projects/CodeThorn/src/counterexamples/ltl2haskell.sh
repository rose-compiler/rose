#/!bin/sh
# transform a list of LTL formulae from stdin into a haskell datatype
echo "formulae = ["
grep '^[\(]' | \
    sed -e 's/^/    /g' \
        -e 's/$/,/g' \
        -e 's/ *& */) `And` (/g' \
        -e 's/ *| */) `Or` (/g' \
        -e 's/\([^ioW]\)U */\1) `U` (/g' \
        -e 's/\([^io]\)R */\1) `R` (/g' \
        -e 's/\([^io]\)F */\1 F $ /g' \
        -e 's/\([^io]\)G */\1 G $ /g' \
        -e 's/\([^io]\)X */\1 X $ /g' \
        -e 's/ *WU */) `WU` (/g' \
        -e 's/ *! */ Not $ /g' \
        -e "s/ *o\([A-Z]\) */ Out '\1' /g" \
        -e "s/ *i\([A-Z]\) */ In '\1' /g" 

echo "    None]"
