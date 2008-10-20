#!/usr/bin/env perl
#MAXPATHLEN

#test if machine is LINUX. If not exit
use English;
exit 0 unless($OSNAME eq "linux");

#$identity_translator="../../../../../gcc3.3.2-compile_dir/tutorial/identityTranslator";
$identity_translator="../../../../tutorial/identityTranslator";
#$output_line="-o output";
$gpp_command_line ="g++";

@command_line_arguments = (
#    "test_simple_file_dollar\$.C",
#    "test_simple_file'.C",
#    "test_simple_file\"C"
#    "test simple file with spaces in name .C",
#    "./test_simple_file\"\'.C", 
#    "./test_simple_file_no_suffix"

#GMY AUG 2008
	"test_simple_file.C",
	"test simple file with spaces in name .C",
	"test_simple_file\"\'.C",
	"test_simple_\'file.C",
	"test_simple_file_dollar\$.C",
	"test_simple_\*.C",
	"test_simple_file_ #1.C",
	"test_simple_file_ending_with_dot.",
	"test_simple_file_no_suffix",

#GMY AUG 2008 Empty Files
	"test_empty_cplusplus_file.C",
	"test_empty_c_file.c"
);

$length_of_command_line = @command_line_arguments;

print("#######################################################################################################\n");
print("\t\t\tRunning $length_of_command_line files, in two different modes (with and without the -o option)\n");
print("#######################################################################################################\n");

for($i = 0; $i < $length_of_command_line; $i++)
{ 
    $file = $command_line_arguments[$i];
    open (FILE, ">$file") or die ("Cannot open file $file");
    print FILE "int main() {}\n";
    close (FILE);
    print("\n\n");
    print("$i.##########################COMMAND LINE = \"$identity_translator $file\"#################################\n");
    system("$identity_translator", "$file");
    print("$i.##########################COMMAND LINE = \"$gpp_command_line $file\"#################################\n");
    system("$gpp_command_line", "$file");
}

#exit();


#NOW RUN WITH -o option
$j = $i;
for($i = 0; $i < $length_of_command_line; $i++)
{ 
    $file = $command_line_arguments[$i];
    $j = $i + $length_of_command_line;
    print("\n\n");
    print("$j.##########################COMMAND LINE = \"$identity_translator $output_line $command_line_arguments[$i]\"#################################\n");
#    system("$identity_translator $output_line $command_line_arguments[$i]\n");
    system("$identity_translator", "-o", "a$i.out", "$command_line_arguments[$i]");

    unlink($file);
}
