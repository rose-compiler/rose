#!/usr/local/bin/perl

# Runs Spear on the given input file, returning 0 if the file's formulas as unsatisfiable and 1 if they are satisfiable
my $usageStr = "Usage: callSpear inputFile.";
if($#ARGV != 0)
{
	die "$usageStr numArgs=$#ARGV";
}

my $inputFile = $ARGV[0];

if(!$ENV{SPEAR_ROOT}) { die "Environment variable SPEAR_ROOT is undefined!"; }
if(!$ENV{machine}) { die "Environment variable machine is undefined!"; }

print "$ENV{SPEAR_ROOT}/bin/$ENV{machine}/Spear_x86_64_Linux --sf $inputFile\n";
$result = `$ENV{SPEAR_ROOT}/bin/$ENV{machine}/Spear_x86_64_Linux --sf $inputFile`;
if($result =~ /UNSATISFIABLE/)
{ exit 0; }
elsif($result =~ /SATISFIABLE/)
{ exit 1; }
else
{ die $result; }