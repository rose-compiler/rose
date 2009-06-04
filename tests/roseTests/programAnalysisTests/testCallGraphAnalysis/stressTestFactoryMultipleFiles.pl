#!/usr/bin/perl



# Prints forward declarations into a file
# param0 : filehandle to source-file
# rest   : array of function names
sub printForwardDeclarations
{
    my ($STRESSTEST,@function_list) = @_;
    
    print $STRESSTEST "// Forward Declarations to all used functions\n";
    foreach $function (@function_list) 
    {  
        print $STRESSTEST "void $function();\n";
    }
    print $STRESSTEST  "\n\n";
}


# Prints a set of function in a file and updates the graph-dump representation
# param 0 : filehandle to source-file
# param 1 : filehandle to graph-dump
# param 2 : all functions which are called in the generated implementation (ref to array!)
# param 3 : functions for which implementation gets printed (ref to array!)
sub printFunctionsToFile
{
	my $STRESSTEST 	   = $_[0];
	my $STRESSTESTCMP   = $_[1];
	my @call_functions = @{$_[2]};
	my @def_functions  = @{$_[3]};
	

	print $STRESSTEST "// Function-implementations: calls every other function\n";
        foreach $function (@def_functions)
        {
            # create a function defintion
            print $STRESSTEST "void $function()\n{\n";
            
            
            # create a graph-dump-node
            print $STRESSTESTCMP "$function ->";
                
                
            # make a complete cg by calling every other function but the main
            foreach $call (@call_functions)
            {
                print $STRESSTEST "\t$call();\n";
                # add this node to the called list in the cgd
                print $STRESSTESTCMP " $call";
            }
    
            #close the function
            print $STRESSTEST "}\n\n";	
            print $STRESSTESTCMP "\n";
        }
}

# Prints main-function in file and updates the graph-dump representation
# param 0 : filehandle to source-file
# param 1 : filehandle to graph-dump
# param 2 : all functions which are called in the generated implementation (ref to array!)
sub printMainFunction
{
	my $STRESSTEST 	   = $_[0];
	my $STRESSTESTCMP   = $_[1];
	my @call_functions = @{$_[2]};

       print $STRESSTEST "int main(int argc, char**argv)\n{\n";
           
       # create a graph-dump-node
       print $STRESSTESTCMP "main ->";
                
                
       # make a complete cg by calling every other function but the main
       foreach $call (@call_functions)
       {
            print $STRESSTEST "\t$call();\n";
            # add this node to the called list in the cgd
           print $STRESSTESTCMP " $call";
       }

        print $STRESSTEST "\treturn 0;\n";      
        #close the function
        print $STRESSTEST "}\n\n";	
        print $STRESSTESTCMP "\n"; 
}



# Creates files, opens them and puts the filehandles in an array
# the filenames are filenamePrefix. $fileNr . $filenameEnding
# param 0 filenamePrefix
# param 1 filenameEnding
# param 2 number of files
sub generateFileHandleArray
{
	my($prefix, $postfix, $number) = @_;
	
	my @handleArray;
	for ($i = 0; $i < $number; ++$i)
	{
		my $filename = $prefix . "_" . $i. $postfix;
		local * FILEHANDLE;
		open(FILEHANDLE, ">$filename");
		push (@handleArray, *FILEHANDLE );
	}
	
	return @handleArray;
}

# Expects an array of Filehandles as created by generateFileHandleArray
# and closes all files
# all params: all treated as filehandle-pointer
sub closeFiles
{
	foreach( $filehandle, @_ ) {
		close($filehandle);
	}
}




my $dirName;

# ------------- Part 1: get function and file-count from user -----------


my $functionCount = 0;
my $fileCount     = 0;


#print "argv.length =". @ARGV. "and content: ". $ARGV[0] . "\n";
if ( @ARGV >= 1 )
{
	$functionCount= $ARGV[0];
	$fileCount    = $ARGV[1];
        $dirName      = $ARGV[2]; 
}
else
{
	print "This is a stress-test for callgraph factory.\n";
}

while ($functionCount < 2)
{
	print "Enter the number of functions (min 2):";
	$functionCount = <STDIN>;
	chomp($functionCount);
}

while ($fileCount < 1  ||  $fileCount > $functionCount )
{
	print "Enter the number of files (min 1, max $functionCount ):";
	$fileCount = <STDIN>;
	chomp($fileCount);
}

my $funcsPerFile = int( $functionCount / $fileCount );





# ----------------- Part 2: Write out files -----------------------

if(!defined($dirName)) {
    $dirName =  "StressTest_Funcs" . $functionCount . "_Files" . $fileCount;
}

mkdir ( $dirName );

my @sourceFiles = generateFileHandleArray($dirName ."/file",".C",$fileCount);

my $dumpFileName = $dirName . "/dumpfile.cmp.dmp";
open(CMP_FILE, ">$dumpFileName");


# create the list of function-names
my @function_list;
for ($i = 0; $i < $functionCount; ++$i)
{
	my $function="f$i";
	push(@function_list , "f$i");
}



# iterate over each source file and
# write in each file  (functionCount / fileCount) functions except last one (there the remainding funcs are printed)
for( $fileId=0; $fileId <= $#sourceFiles; $fileId++  )
{
	my $curSourceFile = @sourceFiles[$fileId];
	
	print $curSourceFile "// This is an automatically generated file for testing CallGraphAnalysis \n";
	print $curSourceFile "// File $fileId out of $#sourceFiles \n\n\n";
	
	printForwardDeclarations($curSourceFile ,@function_list);
	
	
	my $funcNrBegin = $fileId * $funcsPerFile;
	my $funcNrEnd   = (($fileId+1)* $funcsPerFile) -1;
	
	# Special case for last file:
	# - print all remaining function-implementation
	if( $fileId == $#sourceFiles ) 	{
		$funcNrEnd = $#function_list; 
	}
	
	#print "Subset Range at file $fileId: $funcNrBegin to $funcNrEnd \n";
	my @function_subset = @function_list[ ($funcNrBegin .. $funcNrEnd ) ];
	printFunctionsToFile ( $curSourceFile, *CMP_FILE, \@function_list, \@function_subset);
       
        #print main 
       	if( $fileId == $#sourceFiles ) 	{
		printMainFunction($curSourceFile, *CMP_FILE,\@function_list );
        }  
}


closeFiles(@sourceFiles);
close(CMP_FILE);


