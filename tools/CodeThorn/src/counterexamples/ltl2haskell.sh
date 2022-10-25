#/!bin/sh
# transform a list of LTL formulae from stdin into a haskell datatype
echo "formulae = ["
grep '^[\(]' $1 | \
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
        -e "s/ *i\([A-Z]\) */ In '\1' /g" \
        -e "s/true/TTTrue/g" \
        -e "s/false/FFFalse/g"

echo "    None]"
echo "frequencies = ["
#grep -o i[A-G] $1 |sort |uniq -c|sed -e "s/ *\(.\+\) i\(.\) */  (\1, elements ['\2']),/g
grep -o i[A-G] $1 |sort |uniq -c|sed -e "s/i//g" |awk "{ print \"  (\"\$1\", elements ['\"\$2\"']),\" }"
echo "  (0, elements ['Z'])]"
echo
echo "machine = \"./Problem"`echo $1| egrep -o '[0-9]+'`".exe\""
echo