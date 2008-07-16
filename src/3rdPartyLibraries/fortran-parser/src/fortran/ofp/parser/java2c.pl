
# This is a perl script for converting a simple java interface to 
# a reasonable approximation of a c header and source file. Nowhere near
# robust---just a simple tool.
#
# - Bryan Rasmussen, bryanras@lanl.gov


###############- Stage 1: Convert main interface to c and header.

# File names:
$outdir = "c";
$javafile = "java/IFortranParserAction.java";
$cfile = "$outdir/FortranParserActionNull.c";
$hfile = "$outdir/FortranParserAction.h";

# Extern C calls.
$externc="\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n";
$endExternc="\n#ifdef __cplusplus\n} /* End extern C. */\n#endif\n";

# Include lines.
$includeEnums = "#include \"ActionEnums.h\"";
$includeTokens = "#include \"token.h\"";
$includeParser = "#include \"FortranParserAction.h\"\n";

# Build a "header top" that goes after copyrights and initial comments.
$headtop="\n#ifndef FORTRANPARSERHEAD\n#define FORTRANPARSERHEAD\n";
$headtop="$headtop\n$externc\n$includeEnums\n$includeTokens\n\n$typedef\n\n";

# Build a tail for both files.
$headtail="\n$endExternc\n#endif\n";


#############################################################
#
#
###############- Stage 2: Convert "enum" interface to header.

# File names:
$javafile = "java/IActionEnums.java";
$hfile = "$outdir/ActionEnums.h";
$tempfile = "BMRtmpBLAHWOOGAumph";

# Build a "header top" that goes after copyrights and initial comments.
# Use the same tail as above.
$headtop="\n#ifndef FORTRANPARSERENUMSHEAD\n#define FORTRANPARSERENUMSHEAD\n";
$headtop="$headtop\n$externc\n$includeTokens\n\n";

# Open the files the first time.
open JAVA,  "<$javafile" or die "Couldn't open interface file $javafile.";
open HEAD,  ">$hfile" or die "Couldn't open header file $hfile.";

# Flags to tell where we are in the file.
$warnend=0;
$incomm=0;

# Read the input file line-by-line.
while ($line=<JAVA>) {

	# Always strip off the leading whitespace.
	$line =~ s/    /\t/g;
	$line =~ s/\A\t//;

	# Copy C-style comments straight up into both files. Strip off the 
	# first tab, though
	if ($line =~ m/\/\*/) {$incomm=1;}

	if ($incomm==1){
		print CODE $line;
		print HEAD $line;
	}

	else {

		# Convert C++ comments to C. Doesn't work if the file has a 
		# bunch of nested comments. If the programmer does that, there is 
		# no hope for him anyway.
		if ($line =~ m/\/\//) {
			$line =~ s/\/\//\/\*/;
			$line =~ s/\n/ \*\/\n/;
		}
	
		# Don't print interfaces and whatnot.
		elsif ($line =~ m/interface/) {}
		elsif ($line =~ m/import/) {}
		elsif ($line =~ m/package/) {}
	
		# Otherwise just make simple substitutions.
		else {
# 			$line =~ s/boolean/bool/g;
			$line =~ s/boolean/ofp_bool/g;
			$line =~ s/final/const/g;
			$line =~ s/public//g;
			$line =~ s/^ //g;
			$line =~ s/const  /const /g;

			# The "bases" must be turned into proprocessor definitions.
			if ($line =~ m/Base\s*=\s*\d+;/) {
				$line =~ s/static const\s*[A-Za-z]+/#define/;
				$line =~ s/=/ /;
				$line =~ s/;//;
			}
	
			# Print function prototype in header. Stub in code.
			print HEAD $line;
			$line =~ s/\)\s*;\n/\)\n\{\n\}\n/g;
		}
	}
	
	# Go back to normal mode if the comment terminates.
	if ($line =~ m/\*\//) {

		$incomm=0;
		# We assume that the first comment is the warning.
		if ($warnend==0){
			$warnend=1;
			print HEAD $headtop;
		}
	}
}

# Close everything up.
close JAVA or warn "Couldn't close interface file.\n";
close HEAD or warn "Couldn't close header file.\n";

# Now fix the file a little bit.
fixit($hfile);

# One last fix. Take advantage of the fact that C is not as verbose as java.
open HEAD, "<$hfile" or die "Couldn't open file $hfile for last fix.\n"; 
open TEMP, ">$tempfile" or die "Couldn't open tmeporary file $tempfile.\n"; 
while ($line=<HEAD>){

	if ($line=~m/=\s*\n/) {
		$line2=<HEAD>;
		$line="$line$line2";
		$line=~s/=\s*\n\s*/=/g;
	}
	print TEMP $line;

}
close HEAD or die "Couldn't close file $hfile for last fix.\n";
close TEMP or die "Couldn't close file $tempfile.\n";
rename $tempfile, $hfile;

# Tack on the tail.
open HEAD, ">>$hfile" or die 
	"Couldn't pin the tail on the header file $hfile\n";
print HEAD $headtail;
close HEAD or die "Couldn't close header file $hfile after pinning tail.\n";



##################################################################
# Slurping subroutine. Used in both modes. 
# We will slurp here because it is not TOO wasteful. The real way 
# to handle this is to use line-by-line with 
# lookahead, but my poor, little brain can't handle that right now.
##################################################################

sub fixit
{
	# Takes file name as an argument.
	$filename = shift(@_);

	# Slurp whole file into babuf.
	$termy = $/;
	undef $/;
	open FNAME, "<$filename" or die "Couldn't open file: $filename\n"; 
	$babuf = <FNAME>;
	$/ = $termy;
	close FNAME or die "Couldn't close file: $filename\n";

	# Collapse multiple blank lines into one.
	$babuf=~s/\n\s*\n\s*\n/\n\n/g;

	# Eliminate the last }.
	$babuf=~s/\s*\}\s*\z/\n\n/g;
	
	# Print back out to a file.
	open FNAME, ">$filename" or die "Couldn't open file: $filename\n"; 
	print FNAME $babuf;
	close FNAME or die "Couldn't close file: $filename\n";

}
