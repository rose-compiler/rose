#!/bin/sh 

# Basic test script
# (C) 2008 Adrian Prantl, Viktor Pavlu

#set -x

# set this to 42 to get a lot of debug output, something else otherwise
# debug=42
debug=0

# work in the 'C' locale to avoid trouble with different numeric formats
LC_ALL=C

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
FILES=`find -L $SUITE -wholename \*success/\*.[cC] -or -wholename \*failure/\*.[cC] 2>/dev/null`

expected_fails=0
fails_ok=0
expected_succs=0
succs_ok=0

fail_errors=""
succ_errors=""

RAND=`date +%N` # necessary for -j
STATSFILE=runtests-$RAND.stats
TMPFILE=runtests-$RAND.tmp
DATFILE=runtests-$RAND.dat

printf "outfile\ttime\_rose\_frontend\tEDG-AST-construction\tROSE-AST-construction\tAST-post-processing\tAST-comment-processing\tICFG-construction\tExpr-numbering\tICFG-checks\tPAG-analysis\tresult\ttime\_sys\ttime\_user\ttime\_wall\ttime\_pag\_run\ttime\_pag\_init\ttime\_pag\_iter\ttime\_pag\_gc\tPAG-analysis\tSATIrE-ICFG\tROSE-AST\ttotal_memory\tanalysis\toptions\tfile\n" \
    > $DATFILE
for file in $FILES; do

  for analysis in $ANALYSIS; do

    echo "--- testing $ANALYSIS on file $file ---"
    outfile=`basename $file`
    options="--statistics --no-verbose --analysis-files=all --output-source=$outfile.dfi"
    # execute and time with 'time'
    /usr/bin/time --format="%S %U %e" $analysis $options $file &> $TMPFILE
    result=$?

    # Expected FAIL
    if echo $file | grep -q "^$SUITE.*failure/"; then
      expected_fails=$(( $expected_fails + 1 ))
      if [ $result != 0 ]; then
        fails_ok=$(( $fails_ok + 1 ))
      # GB (2008-08-23): This is sometimes useful to see *why* expected
      # successes fail. But mostly it should be suppressed.
      # cat $TMPFILE
      else
        echo "** ERROR: Expected failure succeeded $analysis $options $file"
        fail_errors="$fail_errors $analysis:$file"
      fi
    # Expected SUCCESS
    elif echo $file | grep -q "^$SUITE.*success/"; then    
      expected_succs=$(( $expected_succs + 1 ))
      if [ $result == 0 ]; then
        result='success'  # to get more readable statistics
        succs_ok=$(( $succs_ok + 1 ))
    
        # grep runtime statistics for succ/succ cases
        time_sys=` cat $TMPFILE | awk 'END {print $1}'`
        time_user=`cat $TMPFILE | awk 'END {print $2}'`
        time_wall=`cat $TMPFILE | awk 'END {print $3}'`

        time_pag_run=` cat $TMPFILE | awk '/analyzer done in .* sec/ {print $5;exit}'`
        if [ $debug == 42 ]; then echo "time_pag_run = $time_pag_run because: \"`grep 'analyzer done in .* sec' $TMPFILE`\""; fi
        time_pag_init=`cat $TMPFILE | awk '/initalizing/ {gsub("sec",""); print $1; exit}'`  # initalizing(!)
        if [ $debug == 42 ]; then echo "time_pag_init = $time_pag_init because: \"`grep 'initalizing' $TMPFILE`\""; fi
        time_pag_iter=`cat $TMPFILE | awk '/initalizing/ {gsub("sec",""); print $3; exit}'`  # initalizing(!)
        if [ $debug == 42 ]; then echo "time_pag_iter = $time_pag_iter because: \"`grep 'initalizing' $TMPFILE`\""; fi
        time_pag_gc=`  cat $TMPFILE | awk '/garbage collection/ {gsub("s garbage",""); print $1; exit}'`
        if [ $debug == 42 ]; then echo "time_pag_gc = $time_pag_gc because: \"`grep 'garbage collection' $TMPFILE`\""; fi
        pag_mem_allocd=`   cat $TMPFILE | awk '/allocated/ {gsub("MB",""); print $1; exit}'`
        if [ $debug == 42 ]; then echo "pag_mem_allocd = $pag_mem_allocd because: \"`grep 'allocated' $TMPFILE`\""; fi

        # grep ROSE runtime stats
        time_rose_frontend=` cat $TMPFILE | awk '/ROSE frontend... time = .* .sec/ {print $5;exit}'`
        if [ $debug == 42 ]; then echo "time_rose_frontend = $time_rose_frontend because: \"`grep 'ROSE frontend' $TMPFILE`\""; fi
        time_ast_construction=` cat $TMPFILE | awk '/AST Constrution .*: time = .* .sec/ {print $9;exit}'`
        if [ $debug == 42 ]; then echo "time_ast_construction = $time_ast_construction because: \"`grep 'AST Constrution' $TMPFILE`\""; fi
        time_edg_ast=`cat $TMPFILE | awk '/EDG AST Constrution: time = .* .sec/ {print $6;exit}'`
        if [ $debug == 42 ]; then echo "time_edg_ast = $time_edg_ast because: \"`grep 'EDG AST Constrution' $TMPFILE`\""; fi
        time_rose_ast=`cat $TMPFILE | awk '/AST EDG.Sage III Translation:  time = .* .sec/ {print $7;exit}'`
        if [ $debug == 42 ]; then echo "time_rose_ast = $time_rose_ast because: \"`grep 'AST EDG/Sage III Translation' $TMPFILE`\""; fi
        time_ast_postprocess=` cat $TMPFILE | awk '/AST post-processing: time = .* .sec/ {print $5;exit}'`
        if [ $debug == 42 ]; then echo "time_ast_postprocess = $time_ast_postprocess because: \"`grep 'AST post-processing' $TMPFILE`\""; fi
        time_ast_comment=` cat $TMPFILE | awk '/AST Comment.*Wave.: time = .* .sec/ {print $12;exit}'`
        if [ $debug == 42 ]; then echo "time_ast_comment = $time_ast_comment because: \"`grep 'AST Comment' $TMPFILE`\""; fi

        # grep ICFG builder stats
        time_icfg=` cat $TMPFILE | awk '/Traversal to construct/ {print $7;exit}'`
        if [ $debug == 42 ]; then echo "time_icfg = $time_icfg because \"`grep 'Traversal to construct' $TMPFILE`\""; fi
        time_expr_numbering=`cat $TMPFILE | awk '/Numbering of expressions and types/ {print $8; exit}'`
        if [ x$time_expr_numbering == x ]; then time_expr_numbering=0; fi
        if [ $debug == 42 ]; then echo "time_expr_numbering = $time_expr_numbering because \"`grep 'Numbering of expressions and types' $TMPFILE`\""; fi
        time_icfg_check=` cat $TMPFILE | awk '/CFG consistency check: time = .* .sec/ {print $6;exit}'`
        if [ x$time_icfg_check == x ]; then time_icfg_check=0; fi
        if [ $debug == 42 ]; then echo "time_icfg_check = $time_icfg_check because \"`grep 'CFG consistency check' $TMPFILE`\""; fi
        time_analysis=` cat $TMPFILE | awk '/Actual data-flow.*: time = .* .sec/ {print $7;exit}'`
        if [ $debug == 42 ]; then echo "time_analysis = $time_analysis because \"`grep 'Actual data-flow' $TMPFILE`\""; fi

        # grep memory usage stats
        ast_memory=`cat $TMPFILE | awk '/memory used .* memory pool/ {print $21 / 1000; exit}'` # value is in KB, we want it in MB
        if [ $debug == 42 ]; then echo "ast_memory = $ast_memory because \"`grep 'memory used' $TMPFILE`\""; fi
        total_memory=`cat $TMPFILE | awk '/^ +time .* memory usage/ {print $7; exit}'`
        if [ $debug == 42 ]; then echo "total_memory = $total_memory because \"`grep -e '^ \+time .* memory usage' $TMPFILE`\""; fi
        # This is not really only ICFG memory, but it's the best
        # approximation we have. We abuse AWK as our calculator because the
        # shell's built-in calculator doesn't seem to like floats.
        icfg_memory=`echo "" | awk "{print $total_memory - ($ast_memory + $pag_mem_allocd)}"`
        if [ $debug == 42 ]; then echo "icfg_memory = $icfg_memory = $total_memory - ($ast_memory + $pag_mem_allocd)"; fi

            # verbose output for script development (will also be displayed in statistics at end)
        # echo " pag_run = $time_pag_run"
            # echo "pag_init = $time_pag_init"
            # echo "pag_iter = $time_pag_iter"
            # echo "  pag_gc = $time_pag_gc"
            # echo "     mem = $mem_allocd"
            # echo "     sys = $time_sys"
            # echo "    user = $time_user"
            # echo "    wall = $time_wall"

        printf "$outfile\t$time_rose_frontend\t$result\t$time_sys\t$time_user\t$time_wall\t$time_pag_run\t$time_pag_init\t$time_pag_iter\t$time_pag_gc\t$pag_mem_allocd\t$analysis\t$options\t$file\n" >> $STATSFILE
        printf "$outfile\t$time_rose_frontend\t$time_edg_ast\t$time_rose_ast\t$time_ast_postprocess\t$time_ast_comment\t$time_icfg\t$time_expr_numbering\t$time_icfg_check\t$time_analysis\t$result\t$time_sys\t$time_user\t$time_wall\t$time_pag_run\t$time_pag_init\t$time_pag_iter\t$time_pag_gc\t$pag_mem_allocd\t$icfg_memory\t$ast_memory\t$total_memory\t$analysis\t$options\t$file\n" \
          | sed 's/\_/\\\\\_/g' >> $DATFILE
      else
        echo "** ERROR: Expected success failed $analysis $options $file"
        cat $TMPFILE
        succ_errors="$succ_errors $analysis:$file"
      fi
    else
    # This should never be executed, since either the success or the failure
    # path should be taken for every file. Sometimes the grep commands are
    # wrong, and neither of them matches...
      echo "** ERROR: file $file classified as neither success nor failure (internal error)"
      exit 1
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
    pag_mem_allocd    += \$11

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
        min_mem_allocd = max_mem_allocd = pag_mem_allocd = 0;
        i = 1;
    }
    printf "%st_wall   min avg max = %f %f %f secs\n", prefix, min_time_wall,  time_wall    / i, max_time_wall
    printf "%st_user   min avg max = %f %f %f secs\n", prefix, min_time_user,  time_user    / i, max_time_user
    printf "%st_pag    min avg max = %f %f %f secs\n", prefix, min_time_pag,   time_pag_run / i, max_time_pag
    printf "%salloc'd  min avg max = %f %f %f MB\n", prefix, min_mem_allocd, pag_mem_allocd   / i, max_mem_allocd
}
EndOfAWK

###################################################################
# Create statistics plot
###################################################################
TODAY=`date +%Y-%m-%d`
GPLOT_SCRIPT=statistics-$TODAY.gnuplot
HISTOGRAM=statistics-$TODAY.eps
M_GPLOT_SCRIPT=memory-statistics-$TODAY.gnuplot
M_HISTOGRAM=memory-statistics-$TODAY.eps

# check for GNUplot version >= 4.2
if [ x`gnuplot --version | awk '{ if ($2 >= 4.2) print "yes" }'` = xyes ]
then
    echo "Plotting statistics histogram."

    # Setup gnuplot input scripts; these stay around and can be edited by
    # the user, which is quite convenient!
    cat > $GPLOT_SCRIPT <<EOF
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
#set yrange [0 : 0.5] # limit y range

# Plot the data:
# "using 2" means "use column 2 from $DATFILE"
plot newhistogram "" lc 2, '$DATFILE' \
        using 3:xtic(1), \
     '' using 4, \
     '' using 5, \
     '' using 6, \
     '' using 7, \
     '' using 8, \
     '' using 10
EOF

# GB (2008-04-23): Removed "using 7" because the ICFG tests are now not run by
# default.

    cat > $M_GPLOT_SCRIPT <<EOF
# GNUplot script to generate the memory histogram
# 

# Output:
# Show an X11 window ...
#set terminal x11 persist
# ... or generate an .eps file
set terminal postscript eps enhanced font "CMTI10, 10" color
set output '$M_HISTOGRAM'

# Description
set key autotitle columnheader reverse 

# Style
set boxwidth 0.7 absolute
set style fill solid 1.00 border -1
set datafile missing ''
set style data histogram # Histogram style
set style histogram rowstacked
set grid y # use a grid

set title "Memory used per component"  offset character 0, 0, 0 font "" norotate
set ylabel "Megabytes"
set xlabel "Benchmark"
set xtics nomirror rotate by -45 # Style of the x axis labels
set auto x
set auto y
#set yrange [0 : 0.5] # limit y range
    
# Plot the data:
# "using 2" means "use column 2 from $DATFILE"
plot newhistogram "" lc 2, '$DATFILE' \
        using 21:xtic(1), \
     '' using 20, \
     '' using 19
EOF

#        using 2:xtic(1) ti col lt rgb "#FD8238", \
#     '' using 3 ti col lt rgb "#00B06D", \
#     '' using 4 ti col lt rgb "#0097D6", \
#     '' using 5 ti col lt rgb "#E43887", \
#     '' using 6 ti col lt rgb "#FAE017"

    # here now are the actual calls to gnuplot: feed the scripts generated
    # before into the gnuplot program
    gnuplot < $GPLOT_SCRIPT
    gnuplot < $M_GPLOT_SCRIPT

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
# GB (2008-11-14): This used to output an empty and thus meaningless table,
# so I removed it. It can be resurrected at some point if anyone wants it.
#printf "$OUTPUT_HEADERS\n"
#awk "BEGIN{FS=\"\\t\"; OFS=\"\\t\"}(\$1==0){print $OUTPUT_COLUMNS}" $STATSFILE
#awk '($1==0){print $0}' $STATSFILE | awk -v prefix="" -f $AWKFILE
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
