#!/bin/bash
# Computes graphs that shows rates (recall, precision, specificity, fscore) for different thresholds

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command computes the aggregates" >&2
    echo "    like e.g precision, recall, and specificity.  The following" >&2
    echo "    switches are understood:" >&2
    echo "        --database=PREFIX" >&2
    echo "            The database of all databases for which we are to compute the aggregate." >&2
    exit $exit_status
}

# Show an error message and exit
die () {
    echo "$argv0: $*" >&2
    exit 1
}

# Execute the command, and show users what it is
execute () {
    echo "+" "$@" >&2
    "$@"
    return $?
}

# Counts the number of arguments
count_args () {
    echo "$#"
}



###############################################################################################################################
###############################################################################################################################

# Parse command-line
database=""

while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
	--database=*)
	    database=${arg#--database=}
	    ;;
	*)
	    echo "$argv0: unknown command-line switch: $arg" >&2
	    echo "$argv0: see --help for more info" >&2
	    exit 1
	    ;;
    esac
done

if [ "12$database" = "12" ]; then
    echo "Please provide a database" 
    usage 0
fi


for THRESHOLD_TYPE in reuse_d_distance_1 reuse_d_distance_2   ncd_distance reuse_c_distance_2 reuse_c_distance_1; 
do
   MIN_DISTANCE=`psql $database -t -c "select min($THRESHOLD_TYPE) from specimen_comparison;"`
   MAX_DISTANCE=`psql $database -t -c "select max($THRESHOLD_TYPE) from specimen_comparison;"`

   SEQ_STEPS=`psql $database -t -c "select distinct threshold from threshold_rates where threshold_type='$THRESHOLD_TYPE' ORDER BY threshold" `

   prefix='compression'
   file_name="$(echo "$prefix")_$(echo "$THRESHOLD_TYPE").tex"
   echo "\begin{tikzpicture}" > $file_name
   echo "  \begin{axis}[" >> $file_name
   echo "     height=\columnwidth," >> $file_name
   echo "     width=\columnwidth," >> $file_name
   echo "     grid=major," >> $file_name
   echo "     xlabel={threshold}," >> $file_name
   echo "     ylabel={Percent}," >> $file_name
   echo "     legend style={at={(0.98,0.50)},anchor=north east}" >> $file_name
   echo "  ]" >> $file_name



   for rate_type in recall precision specificity fscore;
   do
     echo "  \addplot coordinates {" >> $file_name
   
     OUTPUT_FILE="output_file.txt"

     psql $database -t -A -F"," -c "select threshold, $rate_type from threshold_rates where threshold_type='$THRESHOLD_TYPE' ORDER BY threshold" > $OUTPUT_FILE

     while IFS=, read col1 col2 
     do
       THRESHOLD="$col1"
       rate_value="$col2"
       echo "    ( $THRESHOLD, $rate_value )" >> $file_name
     done < $OUTPUT_FILE
 
     echo "  };" >> $file_name
     echo "  \addlegendentry{$rate_type}" >> $file_name
  done                                                 
  echo "  \end{axis}" >> $file_name


  echo "\end{tikzpicture}" >> $file_name


done
