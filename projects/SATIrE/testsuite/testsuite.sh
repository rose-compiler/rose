#!/bin/sh 

# Basic test script
# (C) 2008 Adrian Prantl, Viktor Pavlu

#set -x

if [ x$1 = x ]; then 
    SUITE="."
else
    SUITE="$1"
fi

FILES=`find $SUITE -name \*.[cC]* 2>/dev/null`
CURRENTDIR=`pwd`
ANALYSIS="constprop/constprop"

expected_fails=0
fails_ok=0
expected_succs=0
succs_ok=0

fail_errors=""
succ_errors=""

STATSFILE=runtests.stats
TMPFILE=runtests.tmp
DATFILE=runtests.dat

printf "outfile\ttime_rose_frontend\tresult\ttime_sys\ttime_user\ttime_wall\ttime_pag_run\ttime_pag_init\ttime_pag_iter\ttime_pag_gc\tmem_allocd\tanalysis\toptions\tfile\n" > $DATFILE
for file in $FILES; do

  for analysis in $ANALYSIS; do

    if ! echo $file | grep -q ^$SUITE/dontrun; then

      echo "--- testing $ANALYSIS on file $file ---"
      outfile=`basename $file`
      options="--statistics --no-verbose --analysis-files=all --output-text --output-term=$outfile.pl --output-source=$outfile.dfi"
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
          printf "$outfile\t$time_rose_frontend\t$result\t$time_sys\t$time_user\t$time_wall\t$time_pag_run\t$time_pag_init\t$time_pag_iter\t$time_pag_gc\t$mem_allocd\t$analysis\t$options\t$file\n" >> $DATFILE
        else
          echo "** ERROR: Expected success failed $analysis $options $file"
          succ_errors="$succ_errors $analysis:$file"
        fi
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
if [ "x$fail_errors$succ_errors" != "x" ]; then
    exit 1
fi
echo
echo "########################################################################"

rm -f $AWKFILE
rm -f $TMPFILE
#rm -f $STATSFILE

# vim: ts=2 sts=2 sw=2:
