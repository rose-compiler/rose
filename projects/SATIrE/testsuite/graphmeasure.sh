#!/bin/sh 

# create srw, nnh comparison diagrams
# (C) 2008 Viktor Pavlu

#set -x

SUITE="shape/"
EXECPATH="../examples/executable-examples/"

if [ ! -e $SUITE ]; then echo "tests in $SUITE not found"; exit 1; fi

FILES=`find -L $SUITE -name \*.[cC]\* 2>/dev/null`
OUTDIR="$0-out"

if [ -e $OUTDIR ]; then rm -rf $OUTDIR; fi
mkdir $OUTDIR

TMPFILE=$OUTDIR/lastoutput
DATFILE=statistics

printf "outfile\tsrw_nodes\tnnh_nodes\n" | sed 's/\_/\\\\\_/g' > $DATFILE.nodes
printf "outfile\tsrw_graphs\tnnh_graphs\n" | sed 's/\_/\\\\\_/g' > $DATFILE.graphs
printf "outfile\tsrw_may_aliases\tnnh_may_aliases\n" | sed 's/\_/\\\\\_/g' > $DATFILE.mayaliases
printf "outfile\tsrw_must_aliases\tnnh_must_aliases\n" | sed 's/\_/\\\\\_/g' > $DATFILE.mustaliases

run_analysis() {
  # $1 := "srw98" | "nnh99"
	# $2 := "--options"
	printf "$1 $2 .. "
	cmd="${EXECPATH}$1_shape/$1_shape --output-gdl=$OUTDIR/$1_$outfile.gdl --output-source=$OUTDIR/$1_$outfile --output-alias-annotation --analysis-files=all --no-number-expressions --output-graph-statistics=outfile.stats $2 $file"
	$cmd &> $TMPFILE

	if [ $? != 0 ]; then
		echo "** ERROR: failed $cmd"
		cat $TMPFILE
		exit 1
  fi
	echo "done"
}

for file in $FILES; do
	
	echo "--- measuring file $file ---"
  outfile=`basename $file`

  # srw
	#  - nunmber of graphs (1 for each statement)
	#  - number of nodes
	#  - number of may-aliases
	#  - number of must-aliases
	run_analysis "srw98" "--summarygraph --no-individualgraphs"
	srw_graphs=`cat outfile.stats | awk '/n_graphs: / {print $2;exit}'`
	srw_nodes=`cat outfile.stats | awk '/n_nodes: / {print $2;exit}'`
	srw_may_aliases=`cat outfile.stats | awk '/n_may-aliases: / {print $2;exit}'`
	srw_must_aliases=`cat outfile.stats | awk '/n_must-aliases: / {print $2;exit}'`

  # nnh
	#  - join individualgraphs into one summarygraph, then count the nodes
	run_analysis "nnh99" "--summarygraph --no-individualgraphs"
  nnh_nodes=`cat outfile.stats | awk '/n_nodes: / {print $2;exit}'`

  # nnh
	#  - nunmber of graphs (at least one per statement)
	#  - number of may-aliases
	#  - number of must-aliases
	run_analysis "nnh99" "--no-summarygraph --individualgraphs"
	nnh_graphs=`cat outfile.stats | awk '/n_graphs: / {print $2;exit}'`
	nnh_may_aliases=`cat outfile.stats | awk '/n_may-aliases: / {print $2;exit}'`
	nnh_must_aliases=`cat outfile.stats | awk '/n_must-aliases: / {print $2;exit}'`

	printf "$outfile\t$srw_nodes\t$nnh_nodes\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.nodes
	printf "$outfile\t$srw_graphs\t$nnh_graphs\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.graphs
	printf "$outfile\t$srw_may_aliases\t$nnh_may_aliases\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.mayaliases
	printf "$outfile\t$srw_must_aliases\t$nnh_must_aliases\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.mustaliases

	rm -f outfile.stats
done

###################################################################
# Create statistics plot
###################################################################
TODAY=`date +%Y-%m-%d`
HISTOGRAM=$0-$TODAY

# check for GNUplot version >= 4.2
if [ x`gnuplot --version | awk '{ if ($2 >= 4.2) print "yes" }'` = xyes ]
then
    printf "Plotting comparison diagrams .. "
    gnuplot <<EOF
# GNUplot script to generate the graph histogram
# 

# Description
set key autotitle columnheader 

# Style
set boxwidth 0.7 absolute
set style fill solid 1.00 border -1
set datafile missing ''
set style data histogram # Histogram style
set style histogram clustered 
set grid y # use a grid

set title "SRW98 vs. NNH99"  offset character 0, 0, 0 font "" norotate
set ylabel "n"
set xlabel "" # Benchmarks
set xtics nomirror rotate by -45 # Style of the x axis labels
set auto x
set auto y
#set yrange [0 : 1] # limit y range

set terminal postscript eps enhanced font "CMTI10, 10" color
set output '$HISTOGRAM.eps'

# Plot the data:
# "using 2" means "use column 2 from file"
plot \
  newhistogram "Number of Nodes", '$DATFILE.nodes' using 2:xtic(1), '' using 3, \
  newhistogram "Number of Graphs", '$DATFILE.graphs' using 2:xtic(1), '' using 3, \
  newhistogram "Number of May-Aliases", '$DATFILE.mayaliases' using 2:xtic(1), '' using 3, \
  newhistogram "Number of Must-Aliases", '$DATFILE.mustaliases' using 2:xtic(1), '' using 3

set output '$HISTOGRAM-nodes.eps'
plot newhistogram "Number of Nodes", '$DATFILE.nodes' using 2:xtic(1), '' using 3

set output '$HISTOGRAM-graphs.eps'
plot newhistogram "Number of Graphs", '$DATFILE.graphs' using 2:xtic(1), '' using 3

set output '$HISTOGRAM-mayaliases.eps'
plot newhistogram "Number of May-Aliases", '$DATFILE.mayaliases' using 2:xtic(1), '' using 3

set output '$HISTOGRAM-mustaliases.eps'
plot newhistogram "Number of Must-Aliases", '$DATFILE.mustaliases' using 2:xtic(1), '' using 3
EOF
echo done.
else
    echo "**WARNING: GNUplot version >= 4.2 was NOT found."
    echo "           Statistics plot will not be generated."
fi 

rm -f $TMPFILE

# vim: ts=2 sts=2 sw=2:
