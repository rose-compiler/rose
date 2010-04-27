#!/bin/bash

# Syntax
# ./testMultipleFiles <srcfolder> <buildfolder> <functionCount> <fileCount>

srcdir=$1
builddir=$2
functionCount=$3
fileCount=$4

#This binary is called with a valid compiler command line and writes data to database
analyseBin="testCG -compare:graph dumpfile.cg.dmp "

#This binary uses the generated database to generate a callgraph compare-file 
# copy compare file->always true
resolveBin="cp dumpfile.cmp.cg.dmp dumpfile.cg.dmp"

#The file which is generated from resolveBin
compareFileName=dumpfile.cg.dmp
dbName="testDB$RANDOM"
echo "functionCount";
echo $3;


res=1;


# First test: call analyseBin with all files at once

    $srcdir/stressTestFactoryMultipleFiles.pl $functionCount $fileCount "testFiles"
    cd $builddir/testFiles;
    ../$analyseBin *.C -db:name $dbName
#$resolveBin
    rm  $dbName 
    diff dumpfile.cmp.dmp $compareFileName;
    res=$? 
    cd .. 
    rm -rf "testFiles"


    if [ $res -ge 1 ]; then
          echo "Test1 failed";
          exit 1; 
    fi
 
     
# Second test: call analyseBin for each single file, then resolve

    $srcdir/stressTestFactoryMultipleFiles.pl $functionCount $fileCount "testFiles"
    cd $builddir/testFiles;
    
    for file in *.C ; do 
        ../$analyseBin $file -o ${file}_out -db:name $dbName
    done 
    rm dbName
    
#    $resolveBin
    diff  dumpfile.cmp.dmp $compareFileName 
    res=$? 
    cd .. 
    rm -rf "testFiles"
    
     
    if [ $res -ge 1 ]; then
          echo "Test2 failed";
          exit 1; 
    fi


