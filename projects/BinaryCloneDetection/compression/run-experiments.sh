#!/bin/bash

#-------------------------------------------------------------------------------------------------------------------------------
# Looks for pairs of specimens compiled with different optimation levels and creates a database for each pair.  The argument
# for this script is the name of a directory, and that directory is searched recursively for target directories with names
# like <TARGET>/<OPTIM> where <TARGET> is the name of the package (e.g., "nginx") and <OPTIM> is the optimization level (one of
# O0, O1, O2, or O3).  Each <TARGET>/<OPTIM> directory should contain exactly one specimen (executable file or library; see
# the find_specimens function).
#-------------------------------------------------------------------------------------------------------------------------------


# String to prepend to all database names
DB_PREFIX=ncd_

# Try to drop the database ('yes' or 'no')?
DROP_DB=yes

# Create the database ('yes' or 'no')?  If 'no' then the analysis is skipped, which is a convenient way to drop a whole bunch
# of databases that you no longer want.  If 'yes' and the creation fails, then the analysis is skipped--either the database
# already exists and we don't need to re-run, or the database could not be created and running would be pointless.
CREATE_DB=yes


###############################################################################################################################

mydir=${0%/*}

die() {
    echo "$@" >&1
    exit 1
}

usage() {
    die "usage: $0 [TARGETS_DIR]"
}

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command iterates over databases with a prefix and computes the aggregates" >&2
    echo "    like e.g precision, recall, and specificity.  The following" >&2
    echo "    switches are understood:" >&2
    exit $exit_status
}

find_specimens() {
    local dir="$1"
    find $dir -maxdepth 1 -type f \( -perm /ugo+x -o -name '*.a' -o -name '*.so' \)
}



threshold=""
DB_PREFIX=""
while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
  
        *)
	    echo "$argv0: unknown command-line switch: $arg" >&2
	    echo "$argv0: see --help for more info" >&2
	    exit 1
	    ;;
    esac
done

OVERALL_DB="ncd_overall_function"

dropdb $OVERALL_DB
createdb $OVERALL_DB

psql $OVERALL_DB -c 'create table file_information(base_name text, opt_level text, program_name text, specimen text)'
psql $OVERALL_DB -c 'create table specimen_comparison(gzip_1_size integer, gzip_2_size integer, gzip_combined_size integer, bsdiff_enumerator integer, bsdiff_denominator integer, base_name_1 text, opt_level_1 text, program_name_1 text, specimen_1 text, base_name_2 text, opt_level_2 text, program_name_2 text, specimen_2 text, ncd_distance double precision, reuse_c_distance_1 double precision, reuse_c_distance_2 double precision, reuse_d_distance double precision)'


BETA=1000;

if [ "12$REUSE_D_DENOMINATOR" = "12" ]; then
#They are exactly the same but of zero length
REUSE_D_DISTANCE=0.0;
else
REUSE_D_DISTANCE="1.0-(1.0*$REUSE_D_ENUMERATOR)/$REUSE_D_DENOMINATOR"
fi


# Find the names of the directories containing target directories. Target directories are O0, O1, O2, or O3
if [ "$#" -gt 0 ]; then

  SEARCH_DIR="$@"
  echo "Target dir is: $SEARCH_DIR"

  INSERT_FILE=`tempfile`

  for TARGET_DIR in $SEARCH_DIR; do
     for DIR in $TARGET_DIR/*/; do
            
        SPECIMENS=$(find "$DIR" -type f)

        for SPECIMEN in $SPECIMENS; do
           BASE_NAME=$(basename $SPECIMEN)
           OPT_LEVEL=$(basename $(dirname $SPECIMEN))
           PROGRAM_NAME=$(dirname $(dirname $SPECIMEN))

           echo "insert into file_information(base_name, opt_level, program_name, specimen ) values('$BASE_NAME', '$OPT_LEVEL', '$PROGRAM_NAME', '$SPECIMEN');" >> $INSERT_FILE
        done

      done

  done
 
  echo "Appending to database"
  psql "$OVERALL_DB" -f "$INSERT_FILE" \
        --echo-all \
        --single-transaction \
        --set AUTOCOMMIT=off \
        --set ON_ERROR_STOP=on 

  psql $OVERALL_DB -c 'create table distinct_functions as select distinct base_name, program_name from file_information  GROUP BY base_name, program_name HAVING COUNT(*) > 1'

  PROGRAMS=`psql $OVERALL_DB -t -c 'select distinct program_name from distinct_functions;'`

  NUM_FUNCTIONS="100"
  typeset -i i END
 
  for PROGRAM_NAME in $PROGRAMS; do
     echo "Starting analysis of $PROGRAM_NAME"

     psql $OVERALL_DB -t -c "drop table IF EXISTS random_functions; create table random_functions as select * from distinct_functions where program_name='$PROGRAM_NAME' ORDER BY RANDOM() limit $NUM_FUNCTIONS;"
     psql $OVERALL_DB -c "drop table IF EXISTS functions_to_analyze; create table functions_to_analyze as select fi.* from file_information as fi join random_functions as rf on rf.program_name=fi.program_name AND rf.base_name=fi.base_name;"

     FUNCTIONS=`psql $OVERALL_DB -t -c "select distinct specimen from functions_to_analyze;"`

     NAME_1="$TARGET_DIR/specimen_1"
     NAME_2="$TARGET_DIR/specimen_2"
     COMBINED_NAME="$TARGET_DIR/combined"
     EPSILON_FILE_NAME="$TARGET_DIR/epsilon_file"
 
     touch $EPSILON_FILE_NAME
     D_PATCH="$TARGET_DIR/d_patch"


     let i=0

     OUTPUT_FILE="output_file.txt"

     psql $OVERALL_DB -t -A -F"," -c  'select fta1.specimen, fta2.specimen from functions_to_analyze as fta1 join functions_to_analyze as fta2 on fta1.specimen <= fta2.specimen ORDER BY fta1.specimen, fta2.specimen;' > $OUTPUT_FILE

     LENGTH=`cat $OUTPUT_FILE | wc -l`

     INSERT_FILE=`tempfile`


     while IFS=, read col1 col2
     do
       

       SPECIMEN_1="$col1"
       SPECIMEN_2="$col2"

       echo "$i of $LENGTH $SPECIMEN_1 $SPECIMEN_2: Starting analysis"

       let i++

       BASE_NAME_1=$(basename $SPECIMEN_1)
       OPT_LEVEL_1=$(basename $(dirname $SPECIMEN_1))
       PROGRAM_NAME_1=$(dirname $(dirname $SPECIMEN_1))


       GZIPPED_1=`gzip -c "$SPECIMEN_1" > "$NAME_1"`
       GZIPPED_1_SIZE=$(stat -c%s "$NAME_1")


       BASE_NAME_2=$(basename $SPECIMEN_2)
       OPT_LEVEL_2=$(basename $(dirname $SPECIMEN_2))
       PROGRAM_NAME_2=$(dirname $(dirname $SPECIMEN_2))


       GZIPPED_2=`gzip -c "$SPECIMEN_2" > "$NAME_2"`
       GZIPPED_2_SIZE=$(stat -c%s "$NAME_2")

       GZIPPED_COMBINED=`gzip -c $SPECIMEN_1 $SPECIMEN_2 > $COMBINED_NAME`
       GZIPPED_COMBINED_SIZE=$(stat -c%s "$COMBINED_NAME")

       REUSE_C_ENUMERATOR="$GZIPPED_1_SIZE+$GZIPPED_2_SIZE-$GZIPPED_COMBINED_SIZE-$BETA"
       REUSE_C_DISTANCE_1="(1.0*($REUSE_C_ENUMERATOR))/$GZIPPED_1_SIZE"
       REUSE_C_DISTANCE_2="(1.0*($REUSE_C_ENUMERATOR))/$GZIPPED_2_SIZE"

       if [ $GZIPPED_1_SIZE -eq 0 ]; then
         REUSE_C_DISTANCE_1="0"
       fi
       if [ $GZIPPED_2_SIZE -eq 0 ]; then
         REUSE_C_DISTANCE_2="0"
       fi



       bsdiff $SPECIMEN_1 $SPECIMEN_2  $D_PATCH
       REUSE_D_ENUMERATOR=$(stat -c%s "$D_PATCH")

       bsdiff $EPSILON_FILE_NAME $SPECIMEN_2  $D_PATCH
       REUSE_D_DENOMINATOR=$(stat -c%s "$D_PATCH")


       if [ $REUSE_D_DENOMINATOR -eq 0 ]; then
         #They are exactly the same but of zero length
         REUSE_D_DISTANCE=0.0;
       else
         REUSE_D_DISTANCE="1.0-(1.0*$REUSE_D_ENUMERATOR)/$REUSE_D_DENOMINATOR"
       fi
     
       if [ $GZIPPED_1_SIZE -eq 0 -a $GZIPPED_2_SIZE -eq 0 ]; then
          NCD_DISTANCE="0.0"
       else
          NCD_ENUMERATOR="$GZIPPED_COMBINED_SIZE-MINUMUM($GZIPPED_1_SIZE, $GZIPPED_2_SIZE)"
          NCD_DENOMINATOR="MAXIMUM($GZIPPED_1_SIZE, $GZIPPED_2_SIZE)"
          NCD_DISTANCE="(1.0*($NCD_ENUMERATOR))/$NCD_DENOMINATOR"
       fi

       echo "insert into specimen_comparison(gzip_1_size, gzip_2_size, gzip_combined_size, bsdiff_enumerator, bsdiff_denominator, base_name_1, opt_level_1, program_name_1, specimen_1, base_name_2, opt_level_2, program_name_2, specimen_2, ncd_distance, reuse_c_distance_1, reuse_c_distance_2, reuse_d_distance) values($GZIPPED_1_SIZE, $GZIPPED_2_SIZE, $GZIPPED_COMBINED_SIZE, $REUSE_D_ENUMERATOR, $REUSE_D_DENOMINATOR, '$BASE_NAME_1', '$OPT_LEVEL_1', '$PROGRAM_NAME_1', '$SPECIMEN_1' , '$BASE_NAME_2', '$OPT_LEVEL_2', '$PROGRAM_NAME_2', '$SPECIMEN_2', $NCD_DISTANCE, $REUSE_C_DISTANCE_1, $REUSE_C_DISTANCE_2, $REUSE_D_DISTANCE );" >> $INSERT_FILE




     done < $OUTPUT_FILE

     echo "$PROGRAM_NAME: Appending $LENGTH records to database"

     psql "$OVERALL_DB" -f $INSERT_FILE --quiet --single-transaction --set AUTOCOMMIT=off --set ON_ERROR_STOP=on 
     rm $INSERT_FILE


  done

else
  echo "Error: no specimens found"
  exit 1
fi
