#!/bin/bash

#
# Variables Setup
#
BASE=./../..
EXEC=$BASE/faultTolerancePass
INCLUDE_DIR=$BASE/includes
LIB_DIR=$BASE/libs
TEST_DIR=./tests
OUT_DIR=./output
EXPECTED_DIR=./expected
ROSE_COMPILE_FLAGS="-rose:skipfinalCompileStep"
ROSE_OUTPUT_FLAGS="-rose:output"
CXX=g++

# Terminate Output
terminate()
{
	echo " ------------ Test Failed ---------------" >&2
	exit 1
}


# Executable Check
if [ ! -f $EXEC ]
then
    echo "Executable $EXEC does not exists" >&2
    terminate
fi

# Clean and Create Output Dir
if [ -d $OUT_DIR ]
then
	rm -rf $OUT_DIR
fi
mkdir $OUT_DIR

# Clear log
rm -rf rose_log.out

# Generate ROSE output files and compile and test them
for file in `dir -d $TEST_DIR/*.C`
do
        echo " Test File Path : $file "
        filename=$(basename ${file})
        #echo " Filename: $filename "
        $EXEC $ROSE_COMPILE_FLAGS -c $file $ROSE_OUTPUT_FLAGS $OUT_DIR/$filename 2>> rose_log.out >> rose_log.out
	# Compile file
	$CXX -I$INCLUDE_DIR $OUT_DIR/$filename -o $OUT_DIR/$filename.exec -lfaultTolerance -L$LIB_DIR
	# execute file
	./$OUT_DIR/$filename.exec > $OUT_DIR/$filename.out 2>&1 &
	sleep 1
	DIFF=$(diff -r $OUT_DIR/$filename.out $EXPECTED_DIR/$filename.out)
        if [[ $DIFF != "" ]]; then
               echo "Test case $filename failed" >&2
               echo "Please look at the $EXPECTED_DIR/$filename.out to see the actual output required." >&2
               terminate
        fi
done



echo " ------------- All Test Passed Successfully --------------- "
