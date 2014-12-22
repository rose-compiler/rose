#!/bin/bash
# Runs intellectual property theft detection.  Give zero or more specimens as arguments.

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


#create table to hold precision, recall etc computing a unified value over all dbs
execute psql  $database -c "drop table IF EXISTS threshold_rates; create table threshold_rates( \
          threshold_type text, threshold double precision, \
          recall double precision, specificity double precision, precision double precision, \
          fscore double precision, true_positives integer, true_negatives integer, false_positives integer, false_negatives integer );"

for THRESHOLD_TYPE in reuse_d_distance_1 reuse_d_distance_2   ncd_distance reuse_c_distance_2 reuse_c_distance_1; 
do
   MIN_DISTANCE=`psql $database -t -c "select min($THRESHOLD_TYPE) from specimen_comparison;"`
   MAX_DISTANCE=`psql $database -t -c "select max($THRESHOLD_TYPE) from specimen_comparison;"`

   SEQ_STEP=`echo "($MAX_DISTANCE-$MIN_DISTANCE)/20" | bc -l`



   for THRESHOLD in $(seq $MIN_DISTANCE $SEQ_STEP `echo "$MAX_DISTANCE+$SEQ_STEP" | bc`); 
   do

     echo "Analyzing $THRESHOLD_TYPE with $THRESHOLD"
     TRUE_POSITIVES=`psql $database -t -c "select count(*) from specimen_comparison as sc join function_information as fi on sc.func_name_1=fi.id join function_information as fi2 on sc.func_name_2=fi2.id where   fi.program_name=fi2.program_name AND fi.base_name=fi2.base_name AND sc.$THRESHOLD_TYPE <= $THRESHOLD;"`
     TRUE_NEGATIVES=`psql $database -t -c "select count(*) from specimen_comparison  as sc join function_information as fi on sc.func_name_1=fi.id join function_information as fi2 on sc.func_name_2=fi2.id where ( fi.program_name!=fi2.program_name  OR fi.base_name!=fi2.base_name ) AND sc.$THRESHOLD_TYPE >  $THRESHOLD;"`
     FALSE_POSITIVES=`psql $database -t -c "select count(*) from specimen_comparison as sc join function_information as fi on sc.func_name_1=fi.id join function_information as fi2 on sc.func_name_2=fi2.id where ( fi.program_name!=fi2.program_name  OR fi.base_name!=fi2.base_name ) AND sc.$THRESHOLD_TYPE <= $THRESHOLD;"`
     FALSE_NEGATIVES=`psql $database -t -c "select count(*) from specimen_comparison as sc join function_information as fi on sc.func_name_1=fi.id join function_information as fi2 on sc.func_name_2=fi2.id where ( fi.program_name=fi2.program_name  AND fi.base_name=fi2.base_name  ) AND sc.$THRESHOLD_TYPE >  $THRESHOLD;"`

     if [ $TRUE_POSITIVES -eq 0 -a  $FALSE_NEGATIVES -eq 0  ];
     then
        RECALL="100.0"
     else 
        RECALL="100.0*$TRUE_POSITIVES/($TRUE_POSITIVES+$FALSE_NEGATIVES)"
     fi

     if [ $TRUE_POSITIVES -eq 0 -a  $FALSE_POSITIVES -eq 0  ];
     then
        PRECISION="100.0"
     else 
        PRECISION="100.0*$TRUE_POSITIVES/($TRUE_POSITIVES+$FALSE_POSITIVES)"
     fi


     if [ $TRUE_NEGATIVES -eq 0 -a  $FALSE_POSITIVES -eq 0  ];
     then
       SPECIFICITY="100.0"
     else 
       SPECIFICITY="100.0*$TRUE_NEGATIVES/($TRUE_NEGATIVES+$FALSE_POSITIVES)"
     fi

     echo "PRECISION IS $PRECISION AND RECALL IS $RECALL"
     cur_precision=`echo "($PRECISION)>0" | bc -l`
     cur_recall=`echo "($RECALL)>0" | bc -l`
     if [  "12$cur_precision" -eq "120" -a "12$cur_recall" -eq "120" ];
     then
        FSCORE="0"
     else 
        FSCORE="2*($PRECISION)*($RECALL)/(($PRECISION)+($RECALL))"
     fi

     execute psql $database -c "insert into threshold_rates(threshold_type, threshold, \
        recall, precision, specificity, fscore, true_positives, true_negatives, false_positives, false_negatives  ) \
        values ('$THRESHOLD_TYPE', $THRESHOLD, $RECALL, $PRECISION, $SPECIFICITY, $FSCORE, $TRUE_POSITIVES, $TRUE_NEGATIVES, $FALSE_POSITIVES, $FALSE_NEGATIVES )" || exit 1
  done                                                 
done
