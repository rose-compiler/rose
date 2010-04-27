
# This is a perl script for converting the java interface to 
# a bunch of stubs.
#
# - Bryan Rasmussen, bryanras@lanl.gov

# File names:
$facefile = "IFortranParserAction";
$nullfile = "FortranParserActionNull";

# Add this stuff to the top of the null file.
$codehead="package fortran.ofp.parser.java;\n";
$codehead="$codehead\nimport org.antlr.runtime.Token;\n";
$codehead="$codehead\nimport fortran.ofp.parser.java.IActionEnums;\n";
$codehead="$codehead\npublic class $nullfile";
$codehead="$codehead implements $facefile {\n";
$codehead="$codehead\n\tpublic FortranParserActionNull";
$codehead="$codehead(String[] args, FortranParser parser,";
$codehead="$codehead String filename) {\n\t\tsuper();\n\t}\n\n";

# Add this stuff to the end of each line to form the stub.
$addline=" {\n\t\t// Auto-generated method stub\n\t}\n";

# Add this stuff to the end.
$codetail="\n}\n";

# Open the files the first time.
open IFACE,  "<$facefile.java" or die "Couldn't open interface file.";
open NULLF,  ">$nullfile.java" or die "Couldn't open java null file.";

# Tack on the stuff at the start of the file.
print NULLF $codehead;

# Interfaces may break across lines. Use the following codes:
# $inline = false => we are not currently building a function.
# $inline = true => we are in the middle of building a function.
$inline=0;
$tline="";

# Read the input file line-by-line.
while ($line=<IFACE>) {


	# See if this is the start of a "function prototype" (or 
	# whatever you call it in java). Perl's conditionals are a little weird.
	if (!$inline) {
		if (($line=~ m/^\s*public/) and not 
				($line=~ /abstract\s*interface/) and not
				($line=~ /public\s*enum/) ) {
		$inline=1;
		}
	}

	# Are we already building a prototype?
	if ($inline) {
		$tline="$tline$line";
	}

	# Are we at the end of a prototype?
	if ($inline and ($line=~m/;/)) {

		# If so, strip out the "abstract" and make a stub.
		$tline =~ s/\s*abstract//;
		$tline =~ s/;/$addline/;
		print NULLF $tline;

		# Reset everything.
		$inline=0;
		$tline="";
	}
}

# Tack on the ending stuff.
print NULLF $codetail;

# Close up shop and gedouddahere.
close IFACE or warn "Couldn't close java file.\n";
close NULLF or warn "Couldn't close C file.\n";

