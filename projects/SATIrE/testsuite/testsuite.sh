#!/bin/sh 

# Basic test script
# (C) 2008 Adrian Prantl, Viktor Pavlu

#set -x

if [ $# = 2 ]; then
  ANALYSIS=$1
	SUITE=$2
else
  if [ $# = 1 ]; then
	  ANALYSIS=$1
		SUITE="."
	else
	  echo "usage: $0 <binary-to-test> <path-to-testcases>"
		exit 1
	fi
fi

# only include the files in success/ and failure/ directories
FILES=`find -L $SUITE -wholename \*success/\*.[cC]\* -or -wholename \*failure/\*.[cC]\* 2>/dev/null`

expected_fails=0
fails_ok=0
expected_succs=0
succs_ok=0

fail_errors=""
succ_errors=""

STATSFILE=runtests.stats
TMPFILE=runtests.tmp
DATFILE=runtests.dat

printf "outfile\ttime_rose_frontend\tresult\ttime_sys\ttime_user\ttime_wall\ttime_pag_run\ttime_pag_init\ttime_pag_iter\ttime_pag_gc\tmem_allocd\tanalysis\toptions\tfile\n" \
	  | sed 's/\_/\\\\\_/g' > $DATFILE
for file in $FILES; do

  for analysis in $ANALYSIS; do

		echo "--- testing $ANALYSIS on file $file ---"
		outfile=`basename $file`
		options="--statistics --no-verbose --analysis-files=all --output-source=$outfile.dfi"
		# execute and time with 'time'
		#/usr/bin/time --format="%S %U %e" $analysis $options $file 1>/dev/null 2>$TMPFILE
		/usr/bin/time --format="%S %U %e" $analysis $options $file &> $TMPFILE
		result=$?

		# Expected FAIL
		if echo $file | grep -q ^$SUITE/failure; then
			expected_fails=$(( $expected_fails + 1 ))
			if [ $result != 0 ]; then
				fails_ok=$(( $fails_ok + 1 ))
			else
				echo "** ERROR: Expected failure succeeded $analysis $options $file"
				fail_errors="$fail_errors $analysis:$file"
			fi
		fi
		# Expected SUCCESS
		if echo $file | grep -q ^$SUITE/success; then    
			expected_succs=$(( $expected_succs + 1 ))
			if [ $result == 0 ]; then
				succs_ok=$(( $succs_ok + 1 ))

				# grep runtime statistics for succ/succ cases
				time_sys=` cat $TMPFILE | awk 'END {print $1}'`
				time_user=`cat $TMPFILE | awk 'END {print $2}'`
				time_wall=`cat $TMPFILE | awk 'END {print $3}'`

				time_pag_run=` cat $TMPFILE | awk '/analyzer done in .* sec/ {print $5;exit}'`
				time_pag_init=`cat $TMPFILE | awk '/initalizing/ {gsub("sec",""); print $1; exit}'`  # initalizing(!)
				time_pag_iter=`cat $TMPFILE | awk '/initalizing/ {gsub("sec",""); print $3; exit}'`  # initalizing(!)
				time_pag_gc=`  cat $TMPFILE | awk '/garbage collection/ {gsub("s garbage",""); print $1; exit}'`
				mem_allocd=`   cat $TMPFILE | awk '/allocated/ {gsub("MB",""); print $1; exit}'`
	# grep ROSE runtime stats
				time_rose_frontend=` cat $TMPFILE | awk '/ROSE frontend... time = .* .sec/ {print $5;exit}'`

				# verbose output for script development (will also be displayed in statistics at end)
				# echo " pag_run = $time_pag_run"
				# echo "pag_init = $time_pag_init"
				# echo "pag_iter = $time_pag_iter"
				# echo "  pag_gc = $time_pag_gc"
				# echo "     mem = $mem_allocd"
				# echo "     sys = $time_sys"
				# echo "    user = $time_user"
				# echo "    wall = $time_wall"

				printf "$outfile\t$time_rose_frontend\t$result\t$time_sys\t$time_user\t$time_wall\t$time_pag_run\t$time_pag_init\t$time_pag_iter\t$time_pag_gc\t$mem_allocd\t$analysis\t$options\t$file\n" >> $STATSFILE
				printf "$outfile\t$time_rose_frontend\t$result\t$time_sys\t$time_user\t$time_wall\t$time_pag_run\t$time_pag_init\t$time_pag_iter\t$time_pag_gc\t$mem_allocd\t$analysis\t$options\t$file\n" \
			| sed 's/\_/\\\\\_/g' >> $DATFILE
			else
				echo "** ERROR: Expected success failed $analysis $options $file"
			  cat $TMPFILE
				succ_errors="$succ_errors $analysis:$file"
			fi
    fi
  done 
done

# create report script
AWKFILE=/tmp/runtests.awk.$RANDOM
cat >$AWKFILE <<EndOfAWK
BEGIN {
    min_time_user  = 9999
    min_time_wall  = 9999
    min_time_pag   = 9999
    min_mem_allocd = 9999
}
{
    time_frontend += \$2

    time_sys      += \$4
    time_user     += \$5
    time_wall     += \$6
    time_pag_run  += \$7
    time_pag_init += \$8
    time_pag_iter += \$9
    time_pag_gc   += \$10
    mem_allocd    += \$11

    min_time_user = min_time_user < \$5 ? min_time_user : \$5
    max_time_user = max_time_user > \$5 ? max_time_user : \$5

    min_time_wall = min_time_wall < \$6 ? min_time_wall : \$6
    max_time_wall = max_time_wall > \$6 ? max_time_wall : \$6

    min_time_pag  = min_time_pag < \$7 ? min_time_pag : \$7
    max_time_pag  = max_time_pag > \$7 ? max_time_pag : \$7

    min_mem_allocd  = min_mem_allocd < \$11 ? min_mem_allocd : \$11
    max_mem_allocd  = max_mem_allocd > \$11 ? max_mem_allocd : \$11

    i++
}
END {
    if (i == 0) {
        min_time_wall  = max_time_wall  = time_wall = 0;
        min_time_user  = max_time_user  = time_user = 0;
        min_time_pag   = max_time_pag   = time_pag_run = 0;
        min_mem_allocd = max_mem_allocd = mem_allocd = 0;
        i = 1;
    }
    printf "%st_wall   min avg max = %f %f %f secs\n", prefix, min_time_wall,  time_wall    / i, max_time_wall
    printf "%st_user   min avg max = %f %f %f secs\n", prefix, min_time_user,  time_user    / i, max_time_user
    printf "%st_pag    min avg max = %f %f %f secs\n", prefix, min_time_pag,   time_pag_run / i, max_time_pag
    printf "%salloc'd  min avg max = %f %f %f MB\n", prefix, min_mem_allocd, mem_allocd   / i, max_mem_allocd
}
EndOfAWK

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
# GNUplot script to generate the timing histogram
# 

# Output:
# Show an X11 window ...
#set terminal x11 persist
# ... or generate an .eps file
set terminal postscript eps enhanced font "CMTI10, 10" color
set output '$HISTOGRAM'

# Description
set key autotitle columnheader reverse 

# Style
set boxwidth 0.7 absolute
set style fill solid 1.00 border -1
set datafile missing ''
set style data histogram # Histogram style
set style histogram rowstacked
set grid y # use a grid

set title "Time spent per stage"  offset character 0, 0, 0 font "" norotate
set ylabel "Seconds"
set xlabel "Benchmark"
set xtics nomirror rotate by -45 # Style of the x axis labels
set auto x
set auto y
#set yrange [0 : 1] # limit y range

# Plot the data:
# "using 2" means "use column 2 from $DATFILE"
plot newhistogram "" lc 2, '$DATFILE' \
        using 2:xtic(1), \
     '' using 7, \
     '' using 8, \
     '' using 9, \
     '' using 10

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


echo "########################################################################"
echo "# SATIrE automagic test report, `date`"
echo "########################################################################"
echo

OUTPUT_COLUMNS="\$6,\$5,\$7,\$11,\$12,\$13,\$14"
OUTPUT_HEADERS="t_wall\tt_user\tt_pag\talloc'd\tcommandline"

echo "[$succs_ok/$expected_succs] expected successes"
printf "$OUTPUT_HEADERS\n"
awk "BEGIN{FS=\"\\t\"; OFS=\"\\t\"}(\$1==0){print $OUTPUT_COLUMNS}" $STATSFILE
awk '($1==0){print $0}' $STATSFILE | awk -v prefix="" -f $AWKFILE
echo 
echo "[$fails_ok/$expected_fails] expected failures"

if [ "x$fail_errors" != "x" ]; then
    echo "** ERROR: Expected failures succeeded"
    for i in $fail_errors; do 
    echo "  $i"
    done
fi
if [ "x$succ_errors" != "x" ]; then
    echo "** ERROR: Expected successes failed"
    for i in $succ_errors; do
    echo "  $i"
    done
fi

rm -f $AWKFILE
rm -f $TMPFILE
#rm -f $STATSFILE

if [ "x$fail_errors$succ_errors" != "x" ]; then
    exit 1
fi
echo
echo "########################################################################"

# vim: ts=2 sts=2 sw=2:
