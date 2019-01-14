#!/bin/bash



# Get command line arguments
# http://stackoverflow.com/questions/18003370/script-parameters-in-bash
if [[ $# -lt 1 ]]; then
    echo "Usage: M2Cxx.sh <args> <filename>"
    exit 1
fi

baseDir="$( cd "$(dirname "$0")" ; pwd -P )"
#~ baseDir=$(dirname "$m2cxxloc")

# Location of the file to convert to C++
filePath=${*: -1}
# Directory of the file to convert to C++
fileDir=$(dirname "${filePath}")
# Just the filename to convert
fileName=$(basename "${filePath}")
# What the converted file will be called without the extension
functionName="${fileName%.*}"


# Get the current directory.
#http://unix.stackexchange.com/questions/188182/how-to-get-current-working-directory


echo "$baseDir/src/m2cxx -I $baseDir/support/ $@"
$baseDir/src/m2cxx -I $baseDir/support/ $@

# Get exit status of running m2cxx
# source: http://tldp.org/LDP/abs/html/exit-status.html
if [[ $? -ne 0 ]]; then
    echo "Error running m2cxx on file ${filePath}"
    exit 1
fi


topDir=$PWD

# Make an output directory to put the file in.
outDir="$topDir/output/"
mkdir -p ${outDir}

generatedFile=rose_${fileName}.cc

mv "$topDir/${generatedFile}" ${outDir}

# Copy the Makefile template to out dir
# we use sed below: cp $topDir/template_makefile/Makefile.default ${outDir}

#replace TEMPLATE text by current .cc file
sed -e "s|@TEMPLATE@|${generatedFile}|g" \
    -e "s|@M2CXXBASEDIR@|$baseDir|g" \
    $baseDir/support/blueprint/Makefile.default > ${outDir}/Makefile.${functionName}

# Remove the last line from the generated file, which is an include statement
sed -i '$ d' $outDir/${generatedFile}

# Insert main function in generated file
#~ cat $topDir/template_makefile/main.txt >> ${outDir}/${generatedFile}

#insert the #include statement
echo -e "#include \"armadillo/fastnumerics.h\"\n$(cat ${outDir}/${generatedFile})" > ${outDir}/${generatedFile}

rm "$topDir/${fileName}.cc" "$topDir/${fileName}.ti" "$topDir/builtins.m.cc" "$topDir/builtins.m.ti"
