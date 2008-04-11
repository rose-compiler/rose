#!/bin/sh 

# create shapegraphe statistics report 
# (C) 2008 Viktor Pavlu

#set -x

SUITE="shape/"
EXECPATH="../examples/executable-examples/"
ANALYSIS="srw98_shape/srw98_shape nnh99_shape/nnh99_shape"

# only include the files in success/ directories
FILES=`find -L $SUITE -name \*.[cC]\* 2>/dev/null`

TMPFILE=runtests.tmp
DATFILE=runtests

printf "outfile\tsrw_nodes\tnnh_nodes\n" | sed 's/\_/\\\\\_/g' > $DATFILE.nodes
printf "outfile\tsrw_graphs\tnnh_graphs\n" | sed 's/\_/\\\\\_/g' > $DATFILE.graphs
printf "outfile\tsrw_aliases\tnnh_aliases\n" | sed 's/\_/\\\\\_/g' > $DATFILE.aliases

for file in $FILES; do
	
	echo "--- measuring file $file ---"
  outfile=`basename $file`

  # srw called with --no-individualgraphs --summarygraph is ok for graph and nodes (srw)
	# but for nnh we need to count nodes in joined graph to make them compareable
	# with srw, otherwise every {x} would be counted for every graph that comprise
	# only one srw graph.
	# nnh: join graphs, then count nodes.
	# nnh: graph counting is ok
	# srw: graphs and nodes are ok

	cmd="${EXECPATH}srw98_shape/srw98_shape --output-gdl=outfile.gdl --analysis-files=all --no-number-expressions --output-graph-statistics=outfile.stats --no-individualgraphs --summarygraph $file"
	$cmd &> $TMPFILE

	if [ $? == 0 ]; then
	  # srw with --summarygraph
		srw_graphs=`cat outfile.stats | awk '/n_graphs: / {print $2;exit}'`
		srw_nodes=`cat outfile.stats | awk '/n_nodes: / {print $2;exit}'`
		srw_aliases=`cat outfile.stats | awk '/n_may-aliases: / {print $2;exit}'`
  else
		echo "** ERROR: Expected success failed $cmd"
		cat $TMPFILE
		exit 1
  fi

	cmd="${EXECPATH}nnh99_shape/nnh99_shape --output-gdl=outfile.gdl --analysis-files=all --no-number-expressions --output-graph-statistics=outfile.stats --no-individualgraphs --summarygraph $file"
	$cmd &> $TMPFILE

	if [ $? == 0 ]; then
	  # nnh with --summarygraph, get nodecount
		nnh_nodes=`cat outfile.stats | awk '/n_nodes: / {print $2;exit}'`
  else
		echo "** ERROR: Expected success failed $cmd"
		cat $TMPFILE
		exit 1
  fi

  cmd="${EXECPATH}nnh99_shape/nnh99_shape --output-gdl=outfile.gdl --analysis-files=all --no-number-expressions --output-graph-statistics=outfile.stats --individualgraphs --no-summarygraph $file"
	$cmd &> $TMPFILE

	if [ $? == 0 ]; then
	  # srw with --individualgraphs, get alias and graphs
		nnh_graphs=`cat outfile.stats | awk '/n_graphs: / {print $2;exit}'`
		nnh_aliases=`cat outfile.stats | awk '/n_may-aliases: / {print $2;exit}'`
  else
		echo "** ERROR: Expected success failed $cmd"
		cat $TMPFILE
		exit 1
  fi

	rm outfile.gdl outfile.stats

	printf "$outfile\t$srw_nodes\t$nnh_nodes\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.nodes
	printf "$outfile\t$srw_graphs\t$nnh_graphs\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.graphs
	printf "$outfile\t$srw_aliases\t$nnh_aliases\n" | sed 's/\_/\\\\\_/g' >> $DATFILE.aliases
done

exit 0

###################################################################
# Create statistics plot
###################################################################
TODAY=`date +%y-%m-%d`
HISTOGRAM=statistics-$TODAY.eps

# check for GNUplot version >= 4.2
if [ x`gnuplot --version | awk '{ if ($2 >= 4.2) print "yes" }'` = xyes ]
then
    echo "Plotting statistics histogram."
    gnuplot <<EOF
# GNUplot script to generate the graph histogram
# 

# Output:
# Show an X11 window ...
#set terminal x11 persist
# ... or generate an .eps file
set terminal postscript eps enhanced font "CMTI10, 10" color
set output '$HISTOGRAM'

# Description
set key autotitle columnheader 

# Style
set boxwidth 0.7 absolute
set style fill solid 1.00 border -1
set datafile missing ''
set style data histogram # Histogram style
set style histogram clustered 
set grid y # use a grid

set title "May-Alias pairs"  offset character 0, 0, 0 font "" norotate
set ylabel "n"
set xlabel "Benchmark"
set xtics nomirror rotate by -45 # Style of the x axis labels
set auto x
set auto y
#set yrange [0 : 1] # limit y range

# Plot the data:
# "using 2" means "use column 2 from $DATFILE"
plot newhistogram "" lc 2, '$DATFILE' \
        using 2:xtic(1), \
     '' using 12, \
     '' using 13, \
     '' using 14

EOF

#        using 2:xtic(1) ti col lt rgb "#FD8238", \
#     '' using 3 ti col lt rgb "#00B06D", \
#     '' using 4 ti col lt rgb "#0097D6", \
#     '' using 5 ti col lt rgb "#E43887", \
#     '' using 6 ti col lt rgb "#FAE017"

else
    echo "**WARNING: GNUplot version >= 4.2 was NOT found."
    echo "           Statistics plot will not be generated."
fi 

rm -f $AWKFILE
rm -f $TMPFILE

# vim: ts=2 sts=2 sw=2:
