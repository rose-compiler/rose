#!/usr/bin/env bash
# a script to convert dot graph file to png file

if [[ $# -eq 1 ]]; then 
	basename="${1%.*}"
	suffix="${1##*.}"
	if [[ $suffix != "dot" ]]; then
		echo "not .dot file"
		exit
	fi
	dot -Tpng $1 > $basename.png
else
	echo "Incorrect number of arguments"
	echo "Usage: $(basename $0) input.dot"
fi

