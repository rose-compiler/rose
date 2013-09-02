#!/bin/bash

#
# This file is part of Canviz. See http://www.canviz.org/
# $Id: render_example_graph.sh 249 2009-01-05 09:15:06Z ryandesign.com $
#

INPUT_GRAPH_DIR=$1
shift
INPUT_GRAPH=$1
shift
OUTPUT_GRAPH_DIR=$1
shift
GRAPHVIZ_PREFIX=$1
shift
GRAPHVIZ_LAYOUTS=$@

if [ -z "$TMPDIR" ]; then
	TMPDIR=/tmp
fi

TEMP_GRAPH=$(mktemp "$TMPDIR/canviz.graph.XXXXXXXX")
TEMP_HEADER=$(mktemp "$TMPDIR/canviz.header.XXXXXXXX")
for LAYOUT in $GRAPHVIZ_LAYOUTS; do
	GRAPHVIZ_VERSION=$($GRAPHVIZ_PREFIX/bin/$LAYOUT -V 2>&1)
	mkdir -p $OUTPUT_GRAPH_DIR/$LAYOUT/$(dirname $INPUT_GRAPH)
	(cd $INPUT_GRAPH_DIR && time $GRAPHVIZ_PREFIX/bin/$LAYOUT -Txdot $INPUT_GRAPH > $TEMP_GRAPH) 2> $TEMP_HEADER
	if [ -s $TEMP_GRAPH ]; then
		NOW=$(TZ=GMT date)
		OUTPUT_GRAPH=$OUTPUT_GRAPH_DIR/$LAYOUT/$INPUT_GRAPH.txt
		echo "# Generated $NOW by $GRAPHVIZ_VERSION" > $OUTPUT_GRAPH
		echo '#' >> $OUTPUT_GRAPH
		sed 's/^/# /' < $TEMP_HEADER >> $OUTPUT_GRAPH
		echo >> $OUTPUT_GRAPH
		cat $TEMP_GRAPH >> $OUTPUT_GRAPH
	else
		echo "$LAYOUT $INPUT_GRAPH - CRASHED!" 1>&2
	fi
done
rm -f $TEMP_GRAPH $TEMP_HEADER
