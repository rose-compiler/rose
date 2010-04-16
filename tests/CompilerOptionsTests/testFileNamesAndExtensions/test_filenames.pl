#!/usr/bin/env perl
#The cases are the following:
# the file could be present and given as 
#       absolute path
#       relative path
# the file be absent and given as
#       absolute path
#       relative path
#the output file could be specified with -o argument
#

#MAXPATHLEN

#test if machine is LINUX. If not exit
use English;
exit 0 unless($OSNAME eq "linux");

$identity_translator="../../testTranslator";
$output_line="-o output";

@command_line_arguments = (
    "", #EMPTY COMMAND LINE

    #Non existent files
    #No suffix
    "l", #(with no suffix)
    "./l", #(with no suffix)
    "./ls.C",
    "././ls.C",
    "ls.C",

    #Existent, but improper files (directories and like)
    "./",
    ".",
    "././",
    "/",

    #Existent, but empty files in different flavors
    "test_empty_cplusplus_file.C",
    "test_empty_c_file.c",
    "./test_empty_c_file.c",
    "././test_empty_c_file.c",
    "././///////////////////////////////./././././././././/test_empty_c_file.c",
#     "/home/upadrasta1/NEW_ROSE6.4/test_empty_c_file.c",
#     "/home/upadrasta1/./NEW_ROSE6.4/./test_empty_c_file.c",

     #test for a file in a non-existant relative path 
     "./a/b/c/test_empty_c_file.C",

     #file with no read permissions
     "./test_simple_file_no_permissions.C",

     # simple file
     "./test_simple_file.C",
     "test_simple_file.C",
     "/home/upadrasta1/NEW_ROSE6.4/test_simple_file.C",
     "/home/upadrasta1/NEW_ROSE6.4/test_simple_file.C",

     # MULTIPLE FILES

     #Non existent and empty file
    "ls1.C ls2.C", #Two non-existent files
     "test_simple_file.C ./l",
     "/a/b/c/test_empty_file.C",

     #Non existent and improper files
     "ls.C ././", "././ ls.C",
     "././ ./ls.C",
     "/a/b/c/test_empty_file.C /./",
     "/./ /a/b/c/test_empty_file.C",

     #Symbolic links
     "test_empty_c_file.L.c",
     "test_simple_file.L.C",
     "test_symbolic.C",
     "testTranslator"
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
}


#NOW RUN WITH -o option
$j = $i;
for($i = 0; $i < $length_of_command_line; $i++)
{ 
    $j = $i + $length_of_command_line;
    print("\n\n");
    print("$j.##########################COMMAND LINE = \"$identity_translator $output_line $command_line_arguments[$i]\"#################################\n");
#    system("$identity_translator $output_line $command_line_arguments[$i]\n");
    system("$identity_translator -o a$i.out $command_line_arguments[$i]\n");
}
