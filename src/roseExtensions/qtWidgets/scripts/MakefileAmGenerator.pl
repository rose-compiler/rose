#!/usr/bin/perl

use Cwd;
use File::Basename;


# First argument: filename of .h file
sub usesQObject {

	# Reads in the file
	open(INFO , $_[0] );
	@lines = <INFO>;
	$found=0;
        foreach $line (@lines) 
	{
		if ($line =~ /Q_OBJECT/)
		{
			$found=1;
		} 
	}
	$found;
}

sub mocSourceName
{
    ($name,$path,$suffix) = fileparse($_[0],( ".h" ));
    "moc_" . $name . ".cxx" 
}

sub resSourceName
{
    ($name,$path,$suffix) = fileparse($_[0],(".qrc"));
    "qrc_" . $name . ".cpp" 
}

sub uiHeaderName
{
    ($name,$path,$suffix) = fileparse($_[0],(".ui"));
    "ui_" . $name . ".h" 
}

# prints prefix = val1\
#		 val2
# argument 0: prefix
# argument 1: array
sub printArray
{
	my ( @values ) = @{$_[1]};
	my ( $prefix ) = $_[0];
	

	if(scalar @values >0)
	{			 
		print $prefix . " = "  . $values[0];
	}
			
	$prefixLength = length( $prefix)+3;
	
	for($counter = 1; $counter < (scalar @values) ; $counter++) 
	{
		print "\\\n";	
		for($i = 0; $i < $prefixLength ; $i++)   { print " "; }
		print $values[$counter];
	}
	
	print "\n\n";				
}




$printHeaderTarget= 1; 
$headerInstDir = "";
$printLibTarget  = 1;

$numArgs = $#ARGV + 1;
for ($argnum=0; $argnum < $#ARGV ; $argnum++) 
{
    if( $ARGV[$argnum] eq "headertarget")     
    { 
        $printHeaderTarget=1; 
        $argnum=$argnum+1;
        $headerInstDir=$ARGV[$argnum] ; 
        next; 
    } 
    
   if( $ARGV[$argnum] eq "nolibtarget")      { $printLibTarget=0;    next; }
   
   print "Unknown Parameter: " . $ARGV[$argnum] . "\n";
   print "Possible Params: headertarget <relativeInstallPath>, nolibtarget \n";
   exit(0); 
}


@header_files = <*.h>;
@cpp_files    = <*.cpp>;
@ui_files     = <*.ui>;
@res_files    = <*.qrc>;



$dirName = basename(cwd);
$libName = "lib" . $dirName; 

# Scan the header_files for Q_OBJECT macro, if they use it push them onto moc_header_files
# get the according source file name and push onto moc_source_files
foreach $file (@header_files) { 
   if( usesQObject($file) )  { 
	push(@moc_header_files, $file);
        push(@moc_source_files, mocSourceName($file) );	 
   }
}

foreach $file (@res_files) { 
      push(@qrc_source_files, resSourceName($file) );	 
}

foreach $file (@ui_files) { 
      push(@ui_header_files, uiHeaderName($file) );	 
}


push( @nodist_sources,@moc_source_files);
push( @nodist_sources,@qrc_source_files);
push( @nodist_sources,@ui_header_files);






print "#------------ $dirName Makefile.am -----------\n" ;

print "include \$(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc \n\n";
print "if ROSE_WITH_ROSEQT\n";

if($printLibTarget)
{
    print "\n";
    print "noinst_LTLIBRARIES = ".$libName .".la \n" ;
    print "\n\n";
}


$res = open (PRE_FILE , "Makefile.am.pre" );
@lines = <PRE_FILE>;
if($res) 
{
    print "# Contents of Makefile.am.pre\n";
    foreach $line (@lines) { print $line; }
    print "\n";
    print "# End of Makefile.am.pre Content\n";
}
print "\n";

if($printLibTarget)
{
    printArray( $libName . "_la_SOURCES", \@cpp_files );
    printArray( "nodist_" . $libName . "_la_SOURCES", \@nodist_sources );
    printArray( "BUILT_SOURCES",\@ui_header_files);
}


if($printHeaderTarget)
{
    print "# Target for installing header \n";
    print "headerTargetdir    =  \$(roseQtWidgetsHeaderInstallPath)/$headerInstDir\n";
    printArray("headerTarget_HEADERS",\@header_files);
    print "\n";
}

print "endif\n";
