#!/usr/apps/bin/perl -w
#MAXPATHLEN

#test if machine is LINUX. If not exit
use English;
die "exiting" unless($OSNAME eq "linux");

$identity_translator="../../../../../gcc3.3.2-compile_dir/tutorial/identityTranslator";
$output_line="-o output";
$gpp_command_line ="g++";

@command_line_arguments = (
    "test_simple_file_dollar\$.C",
    "test_simple_file'.C",
    "test_simple_file\"C"
#    "test simple file with spaces in name .C",
#    "./test_simple_file\"\'.C", 
#    "./test_simple_file_no_suffix"

);

$length_of_command_line = @command_line_arguments;

print("#######################################################################################################\n");
print("\t\t\tRunning $length_of_command_line files, in two different modes (with and without the -o option)\n");
print("#######################################################################################################\n");

for($i = 0; $i < $length_of_command_line; $i++)
{ 
    print("\n\n");
    print("$i.##########################COMMAND LINE = \"$identity_translator $command_line_arguments[$i]\"#################################\n");
    system("$identity_translator $command_line_arguments[$i]\n");
    print("$i.##########################COMMAND LINE = \"$gpp_command_line $command_line_arguments[$i]\"#################################\n");
    system("$gpp_command_line $command_line_arguments[$i]\n");
}

exit();


#NOW RUN WITH -o option
$j = $i;
for($i = 0; $i < $length_of_command_line; $i++)
{ 
    $j = $i + $length_of_command_line;
    print("\n\n");
    print("$j.##########################COMMAND LINE = \"$identity_translator $output_line $command_line_arguments[$i]\"#################################\n");
    system("$identity_translator $output_line $command_line_arguments[$i]\n");
}
