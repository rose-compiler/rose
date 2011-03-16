#!/bin/bash
source scripts/hudson/debuggingPrinters.sh


####################################################################################
#
#  runBuildStep make|make-check|make-distcheck|make-docs [runSpewAnalysis-outputFile]
#
####################################################################################
function runBuildStep {
    if [ $# -lt 1 -o $# -gt 2 -o \
    ! \( $1 == "make" -o \
         $1 == "make-install" -o \
         $1 == "make-installcheck" -o \
         $1 == "make-check" -o \
         $1 == "make-distcheck" -o \
         $1 == "make-docs" \) ] ; then 
        echo "Usage: runBuildStep make|make-install|make-installcheck|make-check|make-distcheck|make-docs [spewAnalysis-outputFile]" 
        exit 1;
    elif [ $# -eq 2 ]; then
        outputFile=$2
    fi
    buildStep=`echo $1 | sed 's/-/ /'`

####################################################################################
# EXECUTE BUILD STEP (with optional spew analysis)
####################################################################################
    
    printBannerToStartStep "$buildStep"
    start_time_seconds="$(date +%s)"


    if [ $# -eq 2 ]; then
        ${buildStep} -j${NUM_PROCESS} 2>&1 | tee $outputFile
    else
        ${buildStep} -j${NUM_PROCESS}
    fi
    # The pipestatus variable is an array that holds the exit status of your last
    # foreground pipeline commands. we check to see if make failes through this command
    if [ ${PIPESTATUS[0]} -ne 0 -o $? -ne 0 ]  ; then
        printBanner "Fatal error during '${buildStep}'. Aborting..."
       exit 3
    fi

    if [ $# -eq 2 ]; then
        runSpewAnalysis $outputFile
    fi


    printBannerToEndStep "$buildStep" $start_time_seconds 
}


####################################################################################
#
#  runSpewAnalysis <filename-to-process> 
#
####################################################################################
function runSpewAnalysis {
    if [ $# -ne 1 ]; then
        echo "Usage: runSpewAnalysis <filename-to-process>"
        exit 1;
    fi

    printBannerToStartStep "Compile time spew"
    start_time_seconds="$(date +%s)"


    inputFile="$1"

    # Now process to count the number of lines of spew.
    echo "Computing compile time spew"
    if [ $inputFile == "make_check_output.txt" ]; then
      grep -n "Making check in " $inputFile | ../scripts/checkMakeSpew.pl | tee makeSpewLineCount.txt
    else      filteredFile="filtered_$inputFile"

      # Filter out the legitimate lines specific to compilation and linking (filter compile, links, and Qt specific tools).
      echo "Building the $filteredFile file"
      grep -v "Entering directory" $inputFile | grep -v "Leaving directory" | grep -v "COMPILE" | grep -v "LINK" | grep -v "Qt-MOC" | grep -v "Qt-RCC" | grep -v "Qt-UIC" > $filteredFile
      echo "Built $filteredFile"
      grep -n "Making all in " $filteredFile | ../scripts/checkMakeSpew.pl | tee makeSpewLineCount.txt
      # DQ (10/2/2010): Added test for parallelism of build system.
      echo "Measure Build System Parallelism"
      ../scripts/checkMakeParallelism.sh ${inputFile}
    fi

    printBannerToEndStep "Compile time spew" $start_time_seconds 
}
