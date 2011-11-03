#!/bin/bash

#
# Variables Setup
#
EXEC=${2}/src/DataFaultToleranceTransformation
MAIN_TEST_DIR=${1}/test/array/transformation
TEST_DIR=${MAIN_TEST_DIR}/tests
OUT_DIR=./output
EXPECTED_DIR=${MAIN_TEST_DIR}/expected
ROSE_COMPILE_FLAGS="-rose:skipfinalCompileStep"
ROSE_OUTPUT_FLAGS="-rose:output"

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

echo ${TEST_DIR}

FILES=${TEST_DIR}/*.C
# Loop through each test case
#for file in `dir -d ${TEST_DIR}/*.C`
for file in ${FILES}
do
	echo " Test File Path : $file "
	filename=$(basename ${file})
	#echo " Filename: $filename "
 	$EXEC $ROSE_COMPILE_FLAGS -c $file $ROSE_OUTPUT_FLAGS $OUT_DIR/$filename 2>> rose_log.out >> rose_log.out	
	DIFF=$(diff -r $OUT_DIR/$filename $EXPECTED_DIR/$filename)
	if [[ $DIFF != "" ]]; then
		echo "Test case $filename failed" >&2
		echo "Please look at the $EXPECTED_DIR/$filename to see the actual output required." >&2
		terminate
	fi
done


echo " ------------- All Test Passed Successfully --------------- "
