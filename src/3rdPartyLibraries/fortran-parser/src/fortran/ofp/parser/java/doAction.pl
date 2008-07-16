#!/usr/bin/perl

# This generates some code for actions. I'm using it to
# automate some repetitive tasks.

# Rule number, name argument list. Put a ? in front of the variable name if 
# you want to put an if statement in the print.
$rule=558;
$argstring="Token id, boolean hasShapeSpecList";
$addString="";
$name="common_block_object";

# A couple of derived variables.
@arglist = split /,/, $argstring;
$nametoo=$name;
$nametoo=~s/_/-/g;
$nametoo=~s/--begin/__begin/g;
$argstring=~s/\?//g;

# File names.
$grammFile="../outG";
$printFile="outP";
$interFile="outI";

# Open the files.
open OUTG,  ">$grammFile" or die "Couldn't open $grammFile.";
open OUTP,  ">$printFile" or die "Couldn't open $printFile.";
open OUTI,  ">$interFile" or die "Couldn't open $interFile.";

# Print some easy stuff to the top of the files.
print OUTI "\t/** R$rule\n\t * $name\n\t *\n\t *\n";

print OUTP "\t/** R$rule\n\t * $name\n\t */\n";
print OUTP "\tpublic void $name($argstring) {\n";

# There may be an additional string.
if ($addstring eq "") {
	print OUTP "\t\tprintRuleHeader($rule, \"$nametoo\");\n";
}
else {
	print OUTP "\t\tprintRuleHeader($rule, \"$nametoo\", \"$addString\");\n";
}


print OUTG "\t\t\t{action.$name(";

# Traipse through the argument list.
foreach $curArg (@arglist) {
	
	# Get argument type and name.
	($argType, $argName) = split " ", $curArg;

	# Check to see if we need a conditional.
	print OUTP "\t\t";
	if ($argName=~/\?/) {
		$argName=~s/\?//;
		print OUTP "if ($argName!=null) ";
	}

	# Now print the rest. Note that param descriptions are (usually) nonsense.
	# We nevertheless fill in as much as we can. Be sure to check.
	print OUTI "\t * \@param $argName ";
	if ($argName eq "label") {
		print OUTI "The label.\n";
	}
	elsif ($argName eq "id") { 
		print OUTI "Identifier if present. Otherwise, null.\n";
	}
	else {
		if ($argType eq "boolean") { 
			print OUTI "True if";
		}
		print OUTI " TODO: description\n";
	}

	print OUTP "printParameter($argName, \"$argName\");\n";
	$argName=~s/label/lbl/;

	print OUTG "$argName,";
}

#  Now print the easy stuff at the bottom of the files.
print OUTP "\t\tprintRuleTrailer();\n\t\}\n\n";
print OUTI "\t */\n\tpublic abstract void $name($argstring);\n\n";

if (@arglist>0){
	seek OUTG, -1, 1;
}
print OUTG ");}\n";

# Close the files.
close OUTG or warn "Couldn't close $grammFile.\n";
close OUTP or warn "Couldn't close $printFile.\n";
close OUTI or warn "Couldn't close $interFile.\n";
