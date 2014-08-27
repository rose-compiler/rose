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

psql $OVERALL_DB -c 'create table function_information(id SERIAL NOT NULL, base_name text, opt_level text, program_name text, specimen text, gzipped_size integer, bsdiff_epsilon_size integer)'
psql $OVERALL_DB -c 'create table specimen_comparison(id SERIAL NOT NULL, gzip_combined_size integer, bsdiff_enumerator integer, func_name_1 integer, func_name_2 integer, ncd_distance double precision, reuse_c_distance_1 double precision, reuse_c_distance_2 double precision, reuse_d_distance_1 double precision, reuse_d_distance_2 double precision)'




BETA=1000;

# Find the names of the directories containing target directories. Target directories are O0, O1, O2, or O3
if [ "$#" -gt 0 ]; then

  SEARCH_DIR="$@"
  echo "Target dir is: $SEARCH_DIR"

  INSERT_FILE=`tempfile`



  for TARGET_DIR in $SEARCH_DIR; do
     for DIR in $TARGET_DIR/*/; do
        NAME="$TARGET_DIR/specimen_1"


        EPSILON_FILE_NAME="$TARGET_DIR/epsilon_file"

        touch $EPSILON_FILE_NAME
        D_PATCH="$TARGET_DIR/d_patch"

         
        SPECIMENS=$(find "$DIR" -type f)

        for SPECIMEN in $SPECIMENS; do
           BASE_NAME=$(basename $SPECIMEN)
           OPT_LEVEL=$(basename $(dirname $SPECIMEN))
           PROGRAM_NAME=$(dirname $(dirname $SPECIMEN))

           GZIPPED=`gzip -c "$SPECIMEN" > "$NAME"`
           GZIPPED_SIZE=$(stat -c%s "$NAME")

           bsdiff $EPSILON_FILE_NAME $SPECIMEN  $D_PATCH
           BSDIFF_EPSILON_SIZE=$(stat -c%s "$D_PATCH")

           echo "insert into function_information(base_name, opt_level, program_name, specimen, gzipped_size, bsdiff_epsilon_size ) values('$BASE_NAME', '$OPT_LEVEL', '$PROGRAM_NAME', '$SPECIMEN', $GZIPPED_SIZE, $BSDIFF_EPSILON_SIZE);" >> $INSERT_FILE
        done

      done

  done
 
  echo "Appending to database"
  psql "$OVERALL_DB" -f "$INSERT_FILE" --quiet --single-transaction --set AUTOCOMMIT=off --set ON_ERROR_STOP=on 

  psql $OVERALL_DB -c 'create table distinct_functions as select distinct base_name, program_name from function_information'

  PROGRAMS=`psql $OVERALL_DB -t -c 'select distinct program_name from distinct_functions;'`

  NUM_FUNCTIONS="100"
  typeset -i i END
 
  for PROGRAM_NAME in $PROGRAMS; do
     echo -e "\n\nStarting analysis of $PROGRAM_NAME\n"
     TARGET_DIR=`pwd`
     NAME_1="$TARGET_DIR/specimen_1"
     NAME_2="$TARGET_DIR/specimen_2"
     COMBINED_NAME="$TARGET_DIR/combined"
     EPSILON_FILE_NAME="$TARGET_DIR/epsilon_file"
 
     touch $EPSILON_FILE_NAME
     D_PATCH="$TARGET_DIR/d_patch"


     let i=0

     OUTPUT_FILE="output_file.txt"

     psql $OVERALL_DB -t -A -F"," -c  "select fta1.id, fta1.specimen, fta1.gzipped_size, fta1.bsdiff_epsilon_size, fta2.id, fta2.specimen, fta2.gzipped_size, fta2.bsdiff_epsilon_size  from function_information as fta1 join function_information as fta2 on fta1.id <= fta2.id where fta1.program_name='$PROGRAM_NAME' AND fta2.program_name='$PROGRAM_NAME';" > $OUTPUT_FILE

     LENGTH=`cat $OUTPUT_FILE | wc -l`

     INSERT_FILE=`tempfile`
 
     while IFS=, read col1 col2 col3 col4 col5 col6 col7 col8
     do
       

       FUNC_NAME_1="$col1"
       SPECIMEN_1="$col2"
       GZIPPED_SIZE_1="$col3"
       BSDIFF_EPSILON_SIZE_1="$col4"
      
       FUNC_NAME_2="$col5"
       SPECIMEN_2="$col6"
       GZIPPED_SIZE_2="$col7"
       BSDIFF_EPSILON_SIZE_2="$col8"

       if [ `echo "$i%1000" | bc` -eq 0  ]; then
         PERCENT_DONE=`echo "100.0*$i/$LENGTH" | bc `
         echo -en "\r$PERCENT_DONE% DONE: $i of $LENGTH "
       fi

       let i++

       GZIPPED_COMBINED=`gzip -c $SPECIMEN_1 $SPECIMEN_2 > $COMBINED_NAME`
       GZIPPED_COMBINED_SIZE=$(stat -c%s "$COMBINED_NAME")

       REUSE_C_ENUMERATOR="$GZIPPED_SIZE_1+$GZIPPED_SIZE_2-$GZIPPED_COMBINED_SIZE-$BETA"
       REUSE_C_DISTANCE_1="(1.0*($REUSE_C_ENUMERATOR))/$GZIPPED_SIZE_1"
       REUSE_C_DISTANCE_2="(1.0*($REUSE_C_ENUMERATOR))/$GZIPPED_SIZE_2"

       if [ $GZIPPED_SIZE_1 -eq 0 ]; then
         REUSE_C_DISTANCE_1="0"
       fi
       if [ $GZIPPED_SIZE_2 -eq 0 ]; then
         REUSE_C_DISTANCE_2="0"
       fi



       bsdiff $SPECIMEN_1 $SPECIMEN_2  $D_PATCH
       REUSE_D_ENUMERATOR=$(stat -c%s "$D_PATCH")

       if [ $BSDIFF_EPSILON_SIZE_1 -eq 0 ]; then
         #They are exactly the same but of zero length
         REUSE_D_DISTANCE_1=0.0;
       else
         REUSE_D_DISTANCE_1="1.0-(1.0*$REUSE_D_ENUMERATOR)/$BSDIFF_EPSILON_SIZE_1"
       fi

       if [ $BSDIFF_EPSILON_SIZE_2 -eq 0 ]; then
         #They are exactly the same but of zero length
         REUSE_D_DISTANCE_2=0.0;
       else
         REUSE_D_DISTANCE_2="1.0-(1.0*$REUSE_D_ENUMERATOR)/$BSDIFF_EPSILON_SIZE_2"
       fi


     
       if [ $GZIPPED_SIZE_1 -eq 0 -a $GZIPPED_SIZE_2 -eq 0 ]; then
          NCD_DISTANCE="0.0"
       else
          NCD_ENUMERATOR="$GZIPPED_COMBINED_SIZE-LEAST($GZIPPED_SIZE_1, $GZIPPED_SIZE_2)"
          NCD_DENOMINATOR="GREATEST($GZIPPED_SIZE_1, $GZIPPED_SIZE_2)"
          NCD_DISTANCE="(1.0*($NCD_ENUMERATOR))/$NCD_DENOMINATOR"
       fi

       echo "insert into specimen_comparison(gzip_combined_size, bsdiff_enumerator, func_name_1, func_name_2, ncd_distance, reuse_c_distance_1, reuse_c_distance_2, reuse_d_distance_1, reuse_d_distance_2) values($GZIPPED_COMBINED_SIZE, $REUSE_D_ENUMERATOR,  $FUNC_NAME_1, $FUNC_NAME_2, $NCD_DISTANCE, $REUSE_C_DISTANCE_1, $REUSE_C_DISTANCE_2, $REUSE_D_DISTANCE_1, $REUSE_D_DISTANCE_2 );" >> $INSERT_FILE




     done < $OUTPUT_FILE

     echo "$PROGRAM_NAME: Appending $LENGTH records to database"

     psql "$OVERALL_DB" -f $INSERT_FILE --quiet --single-transaction --set AUTOCOMMIT=off --set ON_ERROR_STOP=on 
     rm $INSERT_FILE


  done

else
  echo "Error: no specimens found"
  exit 1
fi
