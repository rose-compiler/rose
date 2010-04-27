#!/usr/bin/env perl
#Test which file names are recognized by gcc, g++, EDG and identityTranslator

#test if machine is LINUX. If not exit
use English;
use Term::ANSIColor;

exit 0 unless($OSNAME eq "linux");

$gcc_command_line="gcc";
$g_plus_plus_command_line="g++";
$identity_translator="../../../../../tutorial/identityTranslator";

@command_line_arguments = (
       # the only extension for c language
       #"test_simple_file_suffix_test.c",

       # Here are the files that are "generally" accepted as files that are C PLUS PLUS source extensions

       "test_simple_file_suffix_test_01.c",
       "test_simple_file_suffix_test_02.c++",
       "test_simple_file_suffix_test_05.cc",
       "test_simple_file_suffix_test_07.cp",
       "test_simple_file_suffix_test_09.cpp",
       "test_simple_file_suffix_test_11.cxx",
       "test_simple_file_suffix_test_13.i",
       "test_simple_file_suffix_test_15.ii",
       "test_simple_file_suffix_test_17.h",
       "test_simple_file_suffix_test_19.hh",
);

$length_of_command_line = @command_line_arguments;

print("#######################################################################################################\n");
print("\t\t\tRunning $length_of_command_line files, in three different modes (with gcc, g++ and identityTranslator)\n");
print("#######################################################################################################\n");

for($i = 0; $i < $length_of_command_line; $i++)
{ 
    $file = $command_line_arguments[$i];
    open (FILE, ">$file") or die ("Cannot open file $file");
    print FILE "int main() {}\n";
    close (FILE);
    #print color("red");
    print("\n\n");
    print("$i.**************************RUNNING WITH FILENAME = $file\"*********************************\n");
    print("\n\n");
    #print color("reset");

    #first run with gcc,
    print("##########################COMMAND LINE = \"$gcc_command_line $file\"#################################\n");
    system("$gcc_command_line $file\n");

    #then with g++
    print("##########################COMMAND LINE = \"$g_plus_plus_command_line $file\"#################################\n");
    system("$g_plus_plus_command_line $file\n");

    #then with sweet old identityTranslator
    print("##########################COMMAND LINE = \"$identity_translator $file\"#################################\n");
    system("$identity_translator $file\n");

    unlink($file);
}
