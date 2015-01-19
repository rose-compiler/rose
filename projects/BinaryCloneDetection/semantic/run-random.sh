#!/bin/bash
# Run programs at random offsets using the "runRandom" tool.

########################################################################################################################
# User configuration follows:

# Where specimens are located
SPECIMEN_DIR=$HOME/GS-CAD/ROSE/CloneDetection/suspects-and-victims/training-set

# Array of all specimens
SPECIMEN_FILES=($(find $SPECIMEN_DIR -type f -perm -u+rx))

# Number of specimens to randomly test
NSPECIMENS=10

# More settings are in the "runRandom" command-line below.


# End of user configuration
########################################################################################################################

echo "found ${#SPECIMEN_FILES[*]} specimens"
if [ ${#SPECIMEN_FILES[*]} -eq 0 ]; then
    echo "error: no specimens found." >&2
    exit 1
fi

for i in $(seq 1 $NSPECIMENS); do
    SPECIMEN_IDX=$[RANDOM % ${#SPECIMEN_FILES[*]}]
    SPECIMEN=${SPECIMEN_FILES[$SPECIMEN_IDX]}

    echo "================================================================================"
    md5sum $SPECIMEN
    echo

    ./runRandom --version
    (  set -x;
 	i386 -R ./runRandom -L 'rose::BinaryAnalysis::Partitioner2(!warn)' \
	    --init=main --functions=100 --instructions=100 --limit=1000000 run:$SPECIMEN
    )
done
    
