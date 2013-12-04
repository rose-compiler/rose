#!/bin/bash

#-------------------------------------------------------------------------------------------------------------------------------
# Looks for pairs of specimens compiled with different optimation levels and creates a database for each pair.  The argument
# for this script is the name of a directory, and that directory is searched recursively for target directories with names
# like <TARGET>/<OPTIM> where <TARGET> is the name of the package (e.g., "nginx") and <OPTIM> is the optimization level (one of
# O0, O1, O2, or O3).  Each <TARGET>/<OPTIM> directory should contain exactly one specimen (executable file or library; see
# the find_specimens function).
#-------------------------------------------------------------------------------------------------------------------------------


# String to prepend to all database names
DB_PREFIX=lsh_

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
    echo "        --threshold=0.0..1.0" >&2
    echo "            Which similarity threshold sould be used, a value between 0.00 and 1.00." >&2
    echo "        --stride=1..INT_MAX" >&2
    echo "            Which stride of instructions should be used between one window and the next." >&2
    echo "        --window-size=1..INT_MAX" >&2
    echo "            Number of instructions that comprise a window. If a function does not have enough" >&2
    echo "            instructions to fill a window then no vectors are generated for the function." >&2
    echo "        --norm=1,2" >&2
    echo "            Use LSH 1 norm (hamming distance) or LSH 2 norm (Euclidean distance)." >&2
    echo "        --prefix=*" >&2
    echo "            Prefix for the database name." >&2
    exit $exit_status
}

find_specimens() {
    local dir="$1"
    find $dir -maxdepth 1 -type f \( -perm /ugo+x -o -name '*.a' -o -name '*.so' \)
}

generate_config() {
    local filename="$1" db_name="$2"
    (
	echo "dbname='postgresql:///$db_name'"
	echo "recreate=yes"
	echo "add_functions_flags='$add_functions_flags'"
	echo "create_vectors_flags='$create_vectors_flags'"
	echo "find_clones_flags='$find_clones_flags'"
    ) >$filename
}

generate_api_config() {
    local filename="$1" db_name="$2"
    (
	echo "dbname='postgresql:///$db_name'"
	echo "api_similarity_worklist_flags='$api_similarity_worklist_flags'"
        echo "api_similarity_flags='$api_similarity_flags'"
    ) >$filename
}

threshold=""
stride=""
window_size=""
norm=""

while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
        --threshold=*)
	    threshold=${arg#--threshold=}
	    ;;
	--stride=*)
	    stride=${arg#--stride=}
	    ;;
	--window-size=*)
	    window_size=${arg#--window-size=}
	    ;;
	--norm=*)
	    norm=${arg#--norm=}
	    ;;
 	--prefix=*)
	    DB_PREFIX=${arg#--prefix=}
	    ;;
   
        *)
	    echo "$argv0: unknown command-line switch: $arg" >&2
	    echo "$argv0: see --help for more info" >&2
	    exit 1
	    ;;
    esac
done

if [ "12$threshold" = "12" ]; then
    echo "Please provide a threshold" 
    usage 0
fi


if [ "12$stride" = "12" ]; then
    echo "Please provide a stride" 
    usage 0
fi


if [ "12$window_size" = "12" ]; then
    echo "Please provide a window size" 
    usage 0
fi


if [ "12$DB_PREFIX" = "12" ]; then
    echo "Please provide a prefix" 
    usage 0
fi


if [ "12$norm" = "12" ]; then
    echo "Please provide a norm" 
    usage 0
fi


# Settings for running the analysis (see $(srcdir)/run-analysis.sh for documentation)
add_functions_flags='--signature-components=total_for_variant,operand_total,ops_for_variant,specific_op,operand_pair'
create_vectors_flags="--stride $stride --windowSize $window_size"
find_clones_flags="-p $norm -k 1000000000 -l 10 -t $threshold"




# Find the names of the directories containing target directories. Target directories are O0, O1, O2, or O3
if [ "$#" -gt 0 ]; then
  SEARCH_DIR="$@"
  echo "Target dir is: $SEARCH_DIR"
  TARGET_DIRS=$(find "$SEARCH_DIR" -type d -name 'O[0123]' |perl -p -e 's((.*)/O[0123]$)($1)' |sort |uniq)
  [ -n "$TARGET_DIRS" ] || die "no target directories found in $SEARCH_DIR"

  for TARGET_DIR in $TARGET_DIRS; do
      TARGET_NAME=$(basename $TARGET_DIR)

      # What optimizations are present?
      OPTIM_DIRS=$(find "$TARGET_DIR" -type d -name 'O[0123]')
      if [ ! "$OPTIM_DIRS" ]; then
	  echo "no optimization directories (O0, O1, O2, O3) in $TARGET_DIR"
  	  continue
      fi

      # Cross product of the optimization levels with itself, avoiding the case where both are the same
      for DIR1 in $OPTIM_DIRS; do
	  OPTIM1=$(basename $DIR1)
	  DIR1_SPECIMENS=($(find_specimens $DIR1))
	  if [ "${#DIR1_SPECIMENS[*]}" -ne 1 ]; then
	      echo "directory should have exactly one specimen (has ${#DIR1_SPECIMENS[*]}): $DIR1"
	      continue
	  fi
	  SPECIMEN1=${DIR1_SPECIMENS[0]}

	  for DIR2 in $OPTIM_DIRS; do

	      # Don't run if DIR1 is lexicographically greater than DIR2 
	      cmp=$(perl -e 'print $ARGV[0] cmp $ARGV[1]' $DIR1 $DIR2)
	      [ "$cmp" -gt 0 ] && continue;

	      OPTIM2=$(basename $DIR2)
	      DIR2_SPECIMENS=($(find_specimens $DIR2))
	      if [ "${#DIR2_SPECIMENS[*]}" -ne 1 ]; then
		  echo "directory should have exactly one specimen (has ${#DIR2_SPECIMENS[*]}): $DIR2"
	  	  continue
	      fi
	      SPECIMEN2=${DIR2_SPECIMENS[0]}

	      # If SPECIMEN1==SPECIMEN2 then copy SPECIMEN2 to a new name and use that instead. Don't copy it into the
	      # same directory it was in or else we violate the one-specimen-per-directory rule described at the top of this
	      # script. Also, don't delete the file after copying it, or else it will exist in the database but not in the file
	      # system, which would cause problems when 25-run-tests tries to analyze it.
	      if [ "$SPECIMEN1" = "$SPECIMEN2" ]; then
		  SPECIMEN2=$(mktemp)
		  rm -f "$SPECIMEN2"
	  	  cp "$SPECIMEN1" "$SPECIMEN2"
	      fi

	      DB_NAME=${DB_PREFIX}${TARGET_NAME}_${OPTIM1}_${OPTIM2}
	      echo
	      echo "==============================================================================================="
	      echo $TARGET_NAME
	      echo
	      echo "database  = $DB_NAME"
	      echo "specimen1 = $SPECIMEN1"
	      echo "specimen2 = $SPECIMEN2"
	      echo

	      # Only run if we don't already have this database and we can create it
              # Ignore error if the db already exists
	      [ "$DROP_DB" = yes ] && ( dropdb $DB_NAME )
	      [ "$CREATE_DB" = yes ] || continue
	      if ! createdb $DB_NAME >/dev/null; then
		  echo "database '$DB_NAME' exists or could not be created; skipping $TARGET_DIR"
	  	  continue
	      fi

	      # Run the analysis
	      CONFIG_FILE=$(mktemp)
	      generate_config $CONFIG_FILE $DB_NAME

              psql $DB_NAME -c "create table fr_run_targets_timings(target text, execution_time float );"
 
              START=$(date +%s.%N)
              $mydir/run-analysis.sh --batch --config=$CONFIG_FILE $SPECIMEN1 $SPECIMEN2 || exit 1
              END=$(date +%s.%N)
              TIME_ELAPSED=$(echo "$END - $START" | bc)

              psql $DB_NAME -c "insert into fr_run_targets_timings(target, execution_time) values('lsh', $TIME_ELAPSED);"

  	done
      done
  done

else
  echo "Error: no specimens found"
  exit 1
fi
