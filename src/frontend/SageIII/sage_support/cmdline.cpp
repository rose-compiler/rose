/**
 * \file    cmdline.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    April 4, 2012
 */

/*-----------------------------------------------------------------------------
 *  Dependencies
 *---------------------------------------------------------------------------*/
#include "cmdline.h"

/*-----------------------------------------------------------------------------
 *  namespace SageSupport::Cmdline {
 *  namespace Cmdline {
 *---------------------------------------------------------------------------*/
static void
SageSupport::Cmdline::
makeSysIncludeList(const Rose_STL_Container<string>& dirs, Rose_STL_Container<string>& result)
   {
     string includeBase = findRoseSupportPathFromBuild("include-staging", "include");
     for (Rose_STL_Container<string>::const_iterator i = dirs.begin(); i != dirs.end(); ++i)
        {
          ROSE_ASSERT (!i->empty());
          string fullPath = (*i)[0] == '/' ? *i : (includeBase + "/" + *i);
#if 1
       // DQ (11/8/2011): We want to exclude the /usr/include directory since it will be search automatically by EDG.
       // If we include it here it will become part of the -sys_include directories and that will cause it to 
       // be searched before the -I<directory> options (which is incorrect).
          if ( SgProject::get_verbose() >= 1 )
               printf ("In makeSysIncludeList(): Building commandline: --sys_include %s \n",(*i).c_str());
          if (*i == "/usr/include")
             {
               if ( SgProject::get_verbose() >= 1 )
                    printf ("Filtering out from the sys_include paths: *i = %s \n",(*i).c_str());
             }
            else
             {
               result.push_back("--sys_include");
               result.push_back(fullPath);
             }
#else
       // Old code that would include the "/usr/include" path as an -sys_include entry.
       // This whole include paths issue is complex enough that I would like to leave this
       // code in place for a while.
          result.push_back("--sys_include");
          result.push_back(fullPath);
#endif
        }
   }

/*-----------------------------------------------------------------------------
 *  namespace CommandlineProcessing {
 *---------------------------------------------------------------------------*/
#define CASE_SENSITIVE_SYSTEM 1

bool
CommandlineProcessing::isOptionWithParameter ( vector<string> & argv, string optionPrefix, string option, string & optionParameter, bool removeOption )
   {
  // I could not make this work cleanly with valgrind withouth allocatting memory twice
     string localString ="";

     //   printf ("Calling sla for string! removeOption = %s \n",removeOption ? "true" : "false");
     //printf ("   argv %d    optionPrefix %s  option %s   localString  %s \n",argv.size(), optionPrefix.c_str(), option.c_str() , localString.c_str() );
     int optionCount = sla(argv, optionPrefix, "($)^", option, &localString, removeOption ? 1 : -1);
  // printf ("DONE: Calling sla for string! optionCount = %d localString = %s \n",optionCount,localString.c_str());

  // optionCount = sla(argv, optionPrefix, "($)^", option, &localString, removeOption ? 1 : -1);
  // printf ("DONE: Calling sla for string! optionCount = %d localString = %s \n",optionCount,localString.c_str());

     if (optionCount > 0)
          optionParameter = localString;

     return (optionCount > 0);
   }

// Note that moving this function from commandline_processing.C to this file (sageSupport.C)
// permitted the validExecutableFileSuffixes to be initialized properly!
void
CommandlineProcessing::initExecutableFileSuffixList ( )
   {
     static bool first_call = true;

     if ( first_call == true )
        {
       // DQ (1/5/2008): For a binary (executable) file, no suffix is a valid suffix, so allow this case
          validExecutableFileSuffixes.push_back("");

          // printf ("CASE_SENSITIVE_SYSTEM = %d \n",CASE_SENSITIVE_SYSTEM);

#if(CASE_SENSITIVE_SYSTEM == 1)
          validExecutableFileSuffixes.push_back(".exe");
#else
       // it is a case insensitive system
          validExecutableFileSuffixes.push_back(".EXE");
#endif
          first_call = false;
        }
   }

// DQ (1/16/2008): This function was moved from the commandling_processing.C file to support the debugging specific to binary analysis
/* This function not only looks at the file name, but also checks that the file exists, can be opened for reading, and has
 * specific values for its first two bytes. Checking the first two bytes here means that each time we add support for a new
 * magic number in the binary parsers we have to remember to update this list also.  Another problem is that the binary
 * parsers understand a variety of methods for neutering malicious binaries -- transforming them in ways that make them
 * unrecognized by the operating system on which they're intended to run (and thus unrecongizable also by this function).
 * Furthermore, CommandlineProcessing::isBinaryExecutableFile() contains similar magic number checking. [RPM 2010-01-15] */
bool
CommandlineProcessing::isExecutableFilename ( string name )
   {
     initExecutableFileSuffixList();

     if (SgProject::get_verbose() > 0)
        {
          printf ("CommandlineProcessing::isExecutableFilename(): name = %s validExecutableFileSuffixes.size() = %zu \n",name.c_str(),validExecutableFileSuffixes.size());
        }

     ROSE_ASSERT(validExecutableFileSuffixes.empty() == false);

     int length = name.size();
     for ( Rose_STL_Container<string>::iterator j = validExecutableFileSuffixes.begin(); j != validExecutableFileSuffixes.end(); j++ )
        {
          int jlength = (*j).size();

          // printf ("jlength = %d *j = %s \n",jlength,(*j).c_str());

          if ( (length > jlength) && (name.compare(length - jlength, jlength, *j) == 0) )
             {
               bool returnValue = false;

            // printf ("passed test (length > jlength) && (name.compare(length - jlength, jlength, *j) == 0): opening file to double check \n");

            // Open file for reading
               bool firstBase = isValidFileWithExecutableFileSuffixes(name);
               if (firstBase == true)
                  {
                    FILE* f = fopen(name.c_str(), "rb");
                    ROSE_ASSERT(f != NULL);

                 // Check for if this is a binary executable file!
                    int character0 = fgetc(f);
                    int character1 = fgetc(f);

                 // Note that there may be more executable formats that this simple test will not catch.
                 // The first character of an ELF binary is '\177' and for a PE binary it is 'M'
                 // if (character0 == 127)
                 // if ((character0 == 0x7F && character1 == 0x45) ||
                 //     (character0 == 0x4D && character1 == 0x5A))
                    bool secondBase = ( (character0 == 0x7F && character1 == 0x45) || (character0 == 0x4D && character1 == 0x5A) );
                    if (secondBase == true)
                       {
                      // printf ("Found a valid executable file! \n");
                         returnValue = true;
                       }

                 // printf ("First character in file: character0 = %d  (77 == %c) \n",character0,'\77');

                    fclose(f);
                  }

               if (returnValue) return true;
             }
        }

     return false;
   }


bool
CommandlineProcessing::isValidFileWithExecutableFileSuffixes ( string name )
   {
  // DQ (8/20/2008):
  // There may be files that are marked as appearing as an executable but are not
  // (we want to process them so that they fail in the binary file format tests
  // rather then here).  so we need to add them to the list of sourceFiles (executable
  // counts as source for binary analysis in ROSE).

     initExecutableFileSuffixList();

  // printf ("CommandlineProcessing::isValidFileWithExecutableFileSuffixes(): name = %s validExecutableFileSuffixes.size() = %zu \n",name.c_str(),validExecutableFileSuffixes.size());
     ROSE_ASSERT(validExecutableFileSuffixes.empty() == false);

     int length = name.size();
     for ( Rose_STL_Container<string>::iterator j = validExecutableFileSuffixes.begin(); j != validExecutableFileSuffixes.end(); j++ )
        {
          int jlength = (*j).size();

          // printf ("jlength = %d *j = %s \n",jlength,(*j).c_str());

          if ( (length > jlength) && (name.compare(length - jlength, jlength, *j) == 0) )
             {
               bool returnValue = false;

            // printf ("passed test (length > jlength) && (name.compare(length - jlength, jlength, *j) == 0): opening file to double check \n");

            // Open file for reading
               if ( boost::filesystem::exists(name.c_str()) )
                  {
                    returnValue = true;

                 // printf ("This is a valid file: %s \n",name.c_str());
                  }
                 else
                  {
                    printf ("Could not open specified input file: %s \n\n",name.c_str());

                 // DQ (8/20/2008): We need to allow this to pass, since Qing's command line processing
                 // mistakenly treats all the arguments as filenames (and most do not exist as valid files).
                 // If we can't open the file then I think we should end in an error!
                 // ROSE_ASSERT(false);


                 // DQ (1/21/2009): This fails for ./binaryReader /home/dquinlan/ROSE/svn-rose/developersScratchSpace/Dan/Disassembler_tests//home/dquinlan/ROSE/svn-rose/developersScratchSpace/Dan/Disassembler_tests/arm-ctrlaltdel
                    ROSE_ASSERT(false);
                  }

               if (returnValue) return true;
             }
        }

     return false;
   }

// DQ (1/16/2008): This function was moved from the commandling_processing.C file to support the debugging specific to binary analysis
// bool CommandlineProcessing::isOptionTakingFileName( string argument )
bool
CommandlineProcessing::isOptionTakingSecondParameter( string argument )
   {
     bool result = false;
  // printf ("In CommandlineProcessing::isOptionTakingFileName(): argument = %s \n",argument.c_str());

  // List any rose options that take source filenames here, so that they can avoid
  // being confused with the source file name that is to be read by EDG and translated.

  // DQ (1/6/2008): Added another test for a rose option that takes a filename
     if ( argument == "-o" ||                               // Used to specify output file to compiler
          argument == "-opt" ||                             // Used in loopProcessor
       // DQ (1/13/2009): This option should only have a single leading "-", not two.
       // argument == "--include" ||                        // Used for preinclude list (to include some header files before all others, common requirement for compiler)
          argument == "-include" ||                         // Used for preinclude file list (to include some header files before all others, common requirement for compiler)
          argument == "-isystem" ||                         // Used for preinclude directory list (to specify include paths to be search before all others, common requirement for compiler)
          argument == "-rose:output" ||                     // Used to specify output file to ROSE
          argument == "-rose:o" ||                          // Used to specify output file to ROSE (alternative to -rose:output)
          argument == "-rose:compilationPerformanceFile" || // Use to output performance information about ROSE compilation phases
          argument == "-rose:verbose" ||                    // Used to specify output of internal information about ROSE phases
          argument == "-rose:test" ||
          argument == "-rose:backendCompileFormat" ||
          argument == "-rose:outputFormat" ||
          argument == "-edg_parameter:" ||
          argument == "--edg_parameter:" ||
          argument == "-rose:generateSourcePositionCodes" ||
          argument == "-rose:embedColorCodesInGeneratedCode" ||
          argument == "-rose:instantiation" ||
          argument == "-rose:includeCommentsAndDirectives" ||
          argument == "-rose:includeCommentsAndDirectivesFrom" ||
          argument == "-rose:excludeCommentsAndDirectives" ||
          argument == "-rose:excludeCommentsAndDirectivesFrom" ||
          argument == "-rose:includePath" ||
          argument == "-rose:excludePath" ||
          argument == "-rose:includeFile" ||
          argument == "-rose:excludeFile" ||
          argument == "-rose:astMergeCommandFile" ||

          // Support for java options
          argument == "-rose:java:cp" ||
          argument == "-rose:java:classpath" ||
          argument == "-rose:java:sourcepath" ||
          argument == "-rose:java:d" ||

       // negara1 (08/16/2011)
          argument == "-rose:unparseHeaderFilesRootFolder" ||
             
       // DQ (8/20/2008): Add support for Qing's options!
          argument == "-annot" ||
          argument == "-bs" ||
          isOptionTakingThirdParameter(argument) ||

       // DQ (9/30/2008): Added support for java class specification required for Fortran use of OFP.
          argument == "--class" ||

       // AS (02/20/08):  When used with -M or -MM, -MF specifies a file to write
       // the dependencies to. Need to tell ROSE to ignore that output paramater
          argument == "-MF" ||
          argument == "-MT" || argument == "-MQ" ||
          argument == "-outputdir" ||  // FMZ (12/22/1009) added for caf compiler
          argument == "-rose:disassembler_search" ||
          argument == "-rose:partitioner_search" ||
          argument == "-rose:partitioner_config" ||

       // DQ (9/19/2010): UPC support for upc_threads to define the "THREADS" variable.
          argument == "-rose:upc_threads" ||

       // DQ (9/26/2011): Added support for detection of dangling pointers within translators built using ROSE.
          argument == "-rose:detect_dangling_pointers" ||   // Used to specify level of debugging support for optional detection of dangling pointers 
          false)
        {
          result = true;
        }

  // printf ("In CommandlineProcessing::isOptionTakingFileName(): argument = %s result = %s \n",argument.c_str(),result ? "true" : "false");

     return result;
   }

bool
CommandlineProcessing::isOptionTakingThirdParameter( string argument )
   {
     bool result = false;
  // printf ("In CommandlineProcessing::isOptionTakingFileName(): argument = %s \n",argument.c_str());

  // List any rose options that take source filenames here, so that they can avoid
  // being confused with the source file name that is to be read by EDG and translated.

  // DQ (1/6/2008): Added another test for a rose option that takes a filename
     if ( false ||          // Used to specify yet another parameter

       // DQ (8/20/2008): Add support for Qing's options!
          argument == "-unroll" ||
          false )
        {
          result = true;
        }

  // printf ("In CommandlineProcessing::isOptionTakingFileName(): argument = %s result = %s \n",argument.c_str(),result ? "true" : "false");

     return result;
   }

// DQ (1/16/2008): This function was moved from the commandling_processing.C file to support the debugging specific to binary analysis
Rose_STL_Container<string>
CommandlineProcessing::generateSourceFilenames ( Rose_STL_Container<string> argList, bool binaryMode )
   {
     Rose_STL_Container<string> sourceFileList;


     bool isSourceCodeCompiler = false;

     { //Find out if the command line is a source code compile line
       Rose_STL_Container<string>::iterator j = argList.begin();
       // skip the 0th entry since this is just the name of the program (e.g. rose)
       ROSE_ASSERT(argList.size() > 0);
       j++;

       while ( j != argList.end() )
       {

         if ( (*j).size() ==2 && (((*j)[0] == '-') && ( ((*j)[1] == 'o')  ) ) )
         {
           isSourceCodeCompiler = true;
           //std::cout << "Set isSourceCodeCompiler " << *j << std::endl;
           break;
         }
         j++;

       }


     }


     Rose_STL_Container<string>::iterator i = argList.begin();


     if (SgProject::get_verbose() > 0)
        {
          printf ("######################### Inside of CommandlineProcessing::generateSourceFilenames() ############################ \n");
        }

  // skip the 0th entry since this is just the name of the program (e.g. rose)
     ROSE_ASSERT(argList.size() > 0);
     i++;



     while ( i != argList.end() )
        {
       // Count up the number of filenames (if it is ZERO then this is likely a
       // link line called using the compiler (required for template processing
       // in C++ with most compilers)) if there is at least ONE then this is the
       // source file.  Currently their can be up to maxFileNames = 256 files
       // specified.

       // most options appear as -<option>
       // have to process +w2 (warnings option) on some compilers so include +<option>

       // DQ (1/5/2008): Ignore things that would be obvious options using a "-" or "+" prefix.
       // if ( ((*i)[0] != '-') || ((*i)[0] != '+') )
          if ( (*i).empty() || (((*i)[0] != '-') && ((*i)[0] != '+')) )
             {
            // printf ("In CommandlineProcessing::generateSourceFilenames(): Look for file names:  argv[%d] = %s length = %zu \n",counter,(*i).c_str(),(*i).size());

                 if (!isSourceFilename(*i) &&
                     (binaryMode || !isObjectFilename(*i)) &&
                     (binaryMode || isExecutableFilename(*i))) {
                     // printf ("This is an executable file: *i = %s \n",(*i).c_str());
                     // executableFileList.push_back(*i);
                     if(isSourceCodeCompiler == false || binaryMode == true)
                         sourceFileList.push_back(*i);
                     goto incrementPosition;
                  }

            // PC (4/27/2006): Support for custom source file suffixes
            // if ( isSourceFilename(*i) )
               if ( isObjectFilename(*i) == false && isSourceFilename(*i) == true )
                  {
                 // printf ("This is a source file: *i = %s \n",(*i).c_str());
                 // foundSourceFile = true;
                    sourceFileList.push_back(*i);
                    goto incrementPosition;
                  }
#if 1
               if ( isObjectFilename(*i) == false && isSourceFilename(*i) == false && isValidFileWithExecutableFileSuffixes(*i) == true )
                  {
                 // printf ("This is at least an existing file of some kind: *i = %s \n",(*i).c_str());
                 // foundSourceFile = true;
                    if(isSourceCodeCompiler == false || binaryMode == true)
                      sourceFileList.push_back(*i);
                    goto incrementPosition;

                  }
#endif
#if 0
               if ( isObjectFilename(*i) )
                  {
                    objectFileList.push_back(*i);
                  }
#endif
             }

       // DQ (12/8/2007): Looking for rose options that take filenames that would accidentally be considered as source files.
       // if (isOptionTakingFileName(*i) == true)
          if (isOptionTakingSecondParameter(*i) == true)
             {
               if (isOptionTakingThirdParameter(*i) == true)
                  {
                 // Jump over the next argument when such options are identified.
                    i++;
                  }

            // Jump over the next argument when such options are identified.
               i++;
             }

incrementPosition:

          i++;
        }

     if (SgProject::get_verbose() > 0)
        {
          printf ("sourceFileList = = %s \n",StringUtility::listToString(sourceFileList).c_str());
          printf ("######################### Leaving of CommandlineProcessing::generateSourceFilenames() ############################ \n");
        }

     return sourceFileList;
   }

/*-----------------------------------------------------------------------------
 *  namespace SgProject {
 *---------------------------------------------------------------------------*/
void
SgProject::processCommandLine(const vector<string>& input_argv)
   {
  // This functions only copies the command line and extracts information from the
  // command line which is useful at the SgProject level (other information useful
  // at the SgFile level is not extracted).
  // Specifically:
  //      1) --help is processed (along with -help, to be friendly)
  //      2) -o <filename> is processed (since both the compilation and the linking
  //         phases must know the output file name and it makes sense to process that once).
  //      3) Lists of files and libraries are processed (sense they too are required in
  //         both compilation and linking).  (e.g. -l<libname>, -L <directory>, <libname>.a,
  //         <filename>.C, <filename>.c, -I<directory name>, <filename>.h
  // NOTE: there is no side-effect to argc and argv.  Thus the original ROSE translator can
  // see all options.  Any ROSE or EDG specific options can be striped by calling the
  // appropriate functions to strip them out.

  // This function now makes an internal copy of the command line parameters to
  // allow the originals to remain unmodified (SLA modifies the command line).

#if 0
     printf ("Inside of SgProject::processCommandLine() \n");
#endif

  // local copies of argc and argv variables
  // The purpose of building local copies is to avoid
  // the modification of the command line by SLA (to save the original command line)
     vector<string> local_commandLineArgumentList = input_argv;

  // Save a deep copy fo the original command line input the the translator
  // pass in out copies of the argc and argv to make clear that we don't modify argc and argv
     set_originalCommandLineArgumentList( local_commandLineArgumentList );

  // printf ("DONE with copy of command line in SgProject constructor! \n");

  // printf ("SgProject::processCommandLine(): local_commandLineArgumentList.size() = %zu \n",local_commandLineArgumentList.size());

     if (SgProject::get_verbose() > 1)
        {
          printf ("SgProject::processCommandLine(): local_commandLineArgumentList = %s \n",StringUtility::listToString(local_commandLineArgumentList).c_str());
        }

  // Build the empty STL lists
#if ROSE_USING_OLD_PROJECT_FILE_LIST_SUPPORT
     p_fileList.clear();
#else
     ROSE_ASSERT(p_fileList_ptr != NULL);
     p_fileList_ptr->get_listOfFiles().clear();
#endif

  // return value for calls to SLA
     int optionCount = 0;

#if 0
  // DQ (11/1/2009): To be consistant with other tools disable -h and -help options (use --h and --help instead).
  // This is a deprecated option
  //
  // help option (allows alternative -h or -help instead of just -rose:help)
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "-", "($)", "(h|help)",1);
     optionCount = sla_none(local_commandLineArgumentList, "-", "($)", "(h|help)",1);
     if( optionCount > 0 )
        {
       // printf ("option -help found \n");
          printf ("This is a deprecated option in ROSE (use --h or --help instead).\n");
          cout << version_message() << endl;
       // ROSE::usage(0);
          SgFile::usage(0);
          exit(0);
        }

  // printf ("After SgProject processing -help option argc = %d \n",argc);
#endif

  //
  // help option (allows alternative --h or --help)
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "--", "($)", "(h|help)",1);
     optionCount = sla_none(local_commandLineArgumentList, "--", "($)", "(h|help)",1);
     if( optionCount > 0 )
        {
       // printf ("option --help found \n");
       // printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
       // version();
       // ROSE::usage(0);
          cout << version_message() << endl;
          SgFile::usage(0);
          exit(0);
        }

  //
  // version option (allows alternative --version or --V)
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "--", "($)", "(V|version)",1);
     optionCount = sla_none(local_commandLineArgumentList, "--", "($)", "(V|version)",1);
     if ( optionCount > 0 )
        {
       // printf ("SgProject::processCommandLine(): option --version found \n");
       // printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          cout << version_message() << endl;
          exit(0);
        }

#if 0
  // DQ (11/1/2009): To be consistant with other tools disable -h and -help options (use --h and --help instead).
  // This is a deprecated option
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "--", "($)", "(V|version)",1);
     optionCount = sla_none(local_commandLineArgumentList, "-rose:", "($)", "(V|version)",1);
     if ( optionCount > 0 )
        {
       // printf ("SgProject::processCommandLine(): option --version found \n");
       // printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          printf ("This is a deprecated option in ROSE (use --V or --version instead).\n");
          cout << version_message() << endl;
          exit(0);
        }
#endif

#if 0
  // DQ (8/6/2006): Not sure that I want this here!
  //
  // version option (using -rose:version)
  //
     if ( CommandlineProcessing::isOption(argc,argv,"-rose:","(V|version)",true) == true )
        {
       // function in SAGE III to access version number of EDG
          extern std::string edgVersionString();
       // printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          cout << version_message() << endl;
          printf ("     Using C++ and C frontend from EDG (version %s) internally \n",edgVersionString().c_str());
        }
#endif

  // DQ (10/15/2005): Added because verbose was not set when the number of files (source files) was zero (case for linking only)
  //
  // specify verbose setting for projects (should be set even for linking where there are no source files
  //
  // DQ (3/9/2009): valid initial range is 0 ... 10.
     ROSE_ASSERT (get_verbose() >= 0);
     ROSE_ASSERT (get_verbose() <= 10);

     int integerOptionForVerbose = 0;
  // if ( CommandlineProcessing::isOptionWithParameter(argc,argv,"-rose:","(v|verbose)",integerOptionForVerbose,true) == true )
     if ( CommandlineProcessing::isOptionWithParameter(local_commandLineArgumentList,"-rose:","(v|verbose)",integerOptionForVerbose,true) == true )
        {
       // set_verbose(true);
          set_verbose(integerOptionForVerbose);

          if ( SgProject::get_verbose() >= 1 )
               printf ("verbose mode ON (for SgProject)\n");
        }

  //
  // Standard compiler options (allows alternative -E option to just run CPP)
  //
  // if ( CommandlineProcessing::isOption(argc,argv,"-","(E)",false) == true )
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-","(E)",false) == true )
        {
       // printf ("/* option -E found (just run backend compiler with -E to call CPP) */ \n");
          p_C_PreprocessorOnly = true;
        }

  //
  // specify compilation only option (new style command line processing)
  //
  // if ( CommandlineProcessing::isOption(argc,argv,"-","c",false) == true )
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-","c",false) == true )
        {
       // printf ("Option -c found (compile only)! \n");
          set_compileOnly(true);
        }


  // DQ (4/7/2010): This is useful when using ROSE translators as a linker, this permits the SgProject
  // to know what backend compiler to call to do the linking.  This is required when there are no SgFile
  // objects to get this information from.
     set_C_only(false);
     ROSE_ASSERT (get_C_only() == false);
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","(c|C)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("In SgProject: C only mode ON \n");
          set_C_only(true);
        }

  // DQ (4/7/2010): This is useful when using ROSE translators as a linker, this permits the SgProject
  // to know what backend compiler to call to do the linking.  This is required when there are no SgFile
  // objects to get this information from.
     set_Cxx_only(false);
     ROSE_ASSERT (get_Cxx_only() == false);
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","(cxx|Cxx)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("In SgProject: C++ only mode ON \n");
          set_Cxx_only(true);
        }

  // DQ (4/7/2010): This is useful when using ROSE translators as a linker, this permits the SgProject
  // to know what backend compiler to call to do the linking.  This is required when there are no SgFile
  // objects to get this information from.
     set_Fortran_only(false);
     ROSE_ASSERT (get_Fortran_only() == false);
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","(f|F|Fortran)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("In SgProject: Fortran only mode ON \n");
          set_Fortran_only(true);
        }

  // DQ (10/11/2010): Adding initial Java support.
     set_Java_only(false);
     ROSE_ASSERT (get_Java_only() == false);
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","(j|J|java|Java)",true) == true )
        {
          if ( SgProject::get_verbose() > 0 )
               printf ("In SgProject: Java only mode ON \n");
          set_Java_only(true);

       // DQ (4/2/2011): Java code is only compiled, not linked as is C/C++ and Fortran.
          set_compileOnly(true);
        }

     string javaRosePrefix = "-rose:java:";
     // Java classpath option support
     string javaTmpParameter;
     if (CommandlineProcessing::isOptionWithParameter(local_commandLineArgumentList, javaRosePrefix,"(cp|classpath)", javaTmpParameter, true) == true) {
         // Parse and register the java classpath in the project
         std::list<std::string> cpList = StringUtility::tokenize(javaTmpParameter, ':');
         set_Java_classpath(cpList);
     }
     // Java sourcepath option support
     if (CommandlineProcessing::isOptionWithParameter(local_commandLineArgumentList, javaRosePrefix,"(sourcepath)", javaTmpParameter, true) == true) {
         // Parse and register the java sourcepath in the project
         std::list<std::string> cpList = StringUtility::tokenize(javaTmpParameter, ':');
         set_Java_sourcepath(cpList);
     }
     // Java destination dir option support
     if (CommandlineProcessing::isOptionWithParameter(local_commandLineArgumentList, javaRosePrefix,"(d)", javaTmpParameter, true) == true) {
         set_Java_destdir(javaTmpParameter);
     }

     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","wave",false) == true )
        {
       // printf ("Option -c found (compile only)! \n");
          set_wave(true);
        }

     // Liao 6/29/2012: support linking flags for OpenMP lowering when no SgFile is available
     set_openmp_linking(false);
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:OpenMP:","lowering",true) == true
         ||CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:openmp:","lowering",true) == true)
     {
       if ( SgProject::get_verbose() >= 1 )
         printf ("In SgProject: openmp_linking mode ON \n");
       set_openmp_linking(true);
     }


#if 1
  // DQ (10/3/2010): Adding support for CPP directives to be optionally a part of the AST as declarations
  // in global scope instead of handled similar to comments.
     set_addCppDirectivesToAST(false);
     ROSE_ASSERT (get_addCppDirectivesToAST() == false);
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","addCppDirectivesToAST",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("In SgProject: addCppDirectivesToAST mode ON \n");
          set_addCppDirectivesToAST(true);
        }
#else
     printf ("Warning: command line CPP directives not processed \n");
#endif

  //
  // prelink option
  //
  // if ( CommandlineProcessing::isOption(argc,argv,"-rose:","(prelink)",true) == true )
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","(prelink)",true) == true )
        {
       // printf ("prelink mode ON \n");
          set_prelink(true);
        }
       else
        {
       // printf ("-rose:prelink not found: prelink mode OFF \n");
          set_prelink(false);
        }

  //
  // specify output file option
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of argc
     string tempOutputFilename;
  // optionCount = sla(&argc, argv, "-", "($)^", "(o|output)", &tempOutputFilename ,1);
     optionCount = sla(local_commandLineArgumentList, "-", "($)^", "(o|output)", &tempOutputFilename ,1);
     if( optionCount > 0 )
        {
       // Make our own copy of the filename string
          p_outputFileName = tempOutputFilename;
       // printf ("option -o|output found outputFileName = %s \n",p_outputFileName.c_str());
        }
       else
        {
       // Usual default for output executables (comes from the default name for
       // assembler output a long long time ago, way way back in the 1970's).
       // For history of why "a.out" is the default see:
       // http://cm.bell-labs.com/cm/cs/who/dmr/chist.html
          p_outputFileName = "a.out";
        }

  // DQ (1/16/2008): Added include/exclude path lists for use internally by translators.
  // For example in Compass this is the basis of a mechanism to exclude processing of
  // header files from specific directorys (where messages about the properties of the
  // code there are not meaningful since they cannot be changed by the user). These
  // are ment to be used by ROSE translators and as a result have unspecificed semantics
  // in ROSE, it is just a common requirement of many uses of ROSE.
  //
  // DQ (1/16/2008): Added include/exclude path lists for use internally by translators.
  // specify exclude path option
     string tempIncludePath;
     vector<string>::iterator iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( *iter == "-rose:includePath")
             {
               iter++;
               tempIncludePath = *iter;

            // printf ("Adding tempIncludePath = %s to p_includePathList \n",tempIncludePath.c_str());
               p_includePathList.push_back(tempIncludePath);
             }

          iter++;
        }

  // DQ (1/16/2008): Added include/exclude path lists for use internally by translators.
  // specify exclude path option
     string tempExcludePath;
     iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( *iter == "-rose:excludePath")
             {
               iter++;
               tempExcludePath = *iter;

            // printf ("Adding tempExcludePath = %s to p_excludePathList \n",tempExcludePath.c_str());
               p_excludePathList.push_back(tempExcludePath);
             }

          iter++;
        }

  // DQ (1/16/2008): Added include/exclude file lists for use internally by translators.
  // specify exclude path option
     string tempIncludeFile;
     iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( *iter == "-rose:includeFile")
             {
               iter++;
               tempIncludeFile = *iter;

            // DQ (1/17/2008): filename matching should be only done on the filename and not the path
               string filenameWithoutPath = StringUtility::stripPathFromFileName(tempIncludeFile);
            // printf ("Adding tempIncludeFile = %s filenameWithoutPath = %s to p_includeFileList \n",tempIncludeFile.c_str(),filenameWithoutPath.c_str());
               p_includeFileList.push_back(filenameWithoutPath);
             }

          iter++;
        }

  // DQ (1/16/2008): Added include/exclude file lists for use internally by translators.
  // specify exclude path option
     string tempExcludeFile;
     iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( *iter == "-rose:excludeFile")
             {
               iter++;
               tempExcludeFile = *iter;

            // DQ (1/17/2008): filename matching should be only done on the filename and not the path
               string filenameWithoutPath = StringUtility::stripPathFromFileName(tempExcludeFile);
            // printf ("Adding tempExcludeFile = %s filenameWithoutPath = %s to p_excludeFileList \n",tempExcludeFile.c_str(),filenameWithoutPath.c_str());
               p_excludeFileList.push_back(filenameWithoutPath);
             }

          iter++;
        }

  //
  // DQ (5/20/2005): specify template handling options (e.g. -rose:instantiation none)
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of argc
     string tempTemplateHandling;
  // optionCount = sla(&argc, argv, "-rose:", "($)^", "(instantiation)", &tempTemplateHandling ,1);
     optionCount = sla(local_commandLineArgumentList, "-rose:", "($)^", "(instantiation)", &tempTemplateHandling ,1);
     if ( optionCount > 0 )
        {
          std::string templateHandlingString = tempTemplateHandling;
          if (templateHandlingString == "none")
               p_template_instantiation_mode = e_none;
          if (templateHandlingString == "used")
               p_template_instantiation_mode = e_used;
          if (templateHandlingString == "all")
               p_template_instantiation_mode = e_all;
          if (templateHandlingString == "local")
               p_template_instantiation_mode = e_local;

       // printf ("option -rose:instantiation found tempTemplateHandling = %s \n",tempTemplateHandling);
        }
       else
        {
       // usual default for template handling
       // printf ("option -rose:instantiation NOT found setting p_template_instantiation_mode = e_default \n");
          p_template_instantiation_mode = e_default;
        }

  // DQ (1/13/2009): Added support for GNU -include <header_file> option (include this file before all others).
     string tempHeaderFile;
     iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( *iter == "-include")
             {
               iter++;
               tempHeaderFile = *iter;

            // printf ("Adding tempHeaderFile = %s to p_preincludeFileList \n",tempHeaderFile.c_str());
               p_preincludeFileList.push_back(tempHeaderFile);
             }

          iter++;
        }

  // DQ (1/13/2009): Added support for GNU -isystem <directory> option (include this directory before all others).
     string tempDirectory;
     iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Searching for -isystem options iter = %s \n",(*iter).c_str());
          if ( *iter == "-isystem")
             {
               iter++;
               tempDirectory = *iter;

               if ( SgProject::get_verbose() >= 1 )
                    printf ("Adding tempHeaderFile = %s to p_preincludeDirectoryList \n",tempDirectory.c_str());
               p_preincludeDirectoryList.push_back(tempDirectory);
             }

          iter++;
        }

  // DQ (10/16/2005):
  // Build versions of argc and argv that are separate from the input_argc and input_argv
  // (so that we can be clear that there are no side-effects to the original argc and argv
  // that come from the user's ROSE translator.  Mostly we want to use the short names
  // (e.g. "argc" and "argv").
     vector<string> argv = get_originalCommandLineArgumentList();
     ROSE_ASSERT(argv.size() > 0);

  // DQ (12/22/2008): This should only be called once (outside of the loop over all command line arguments!
  // DQ (12/8/2007): This leverages existing support in commandline processing
  // printf ("In SgProject::processCommandLine(): Calling CommandlineProcessing::generateSourceFilenames(argv) \n");

  // Note that we need to process this option before the interpretation of the filenames (below).
  // DQ (2/4/2009): Data member was moved to SgProject from SgFile.
  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(binary|binary_only)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Binary only mode ON \n");
          set_binary_only(true);
        }

  // DQ (2/4/2009): The specification of "-rose:binary" causes filenames to be interpreted
  // differently if they are object files or libary archive files.
  // p_sourceFileNameList = CommandlineProcessing::generateSourceFilenames(argv);
     p_sourceFileNameList = CommandlineProcessing::generateSourceFilenames(argv,get_binary_only());

  // Build a list of source, object, and library files on the command line
  // int sourceFileNameCounter = 0;
     for (unsigned int i = 1; i < argv.size(); i++)
        {
       // find the source code filenames and modify them to be the output filenames
          unsigned int length = argv[i].size();

       // printf ("assemble compiler command line option: argv[%d] = %s length = %d \n",i,argv[i],length);
       // printf ("ROSE::sourceFileNamesWithoutPath[%d] = \n",sourceFileNameCounter,
       //     ROSE::sourceFileNamesWithoutPath[sourceFileNameCounter]);
       // ROSE_ASSERT (ROSE::sourceFileNamesWithoutPath[sourceFileNameCounter] != NULL);

       // DQ (12/8/2007): This leverages existing support in commandline processing
       // p_sourceFileNameList = CommandlineProcessing::generateSourceFilenames(argv);

       // printf ("In SgProject::processCommandLine(): p_sourceFileNameList.size() = %zu \n",p_sourceFileNameList.size());

       // DQ (2/4/2009): Only put *.o files into the objectFileNameList is they are not being
       // processed as binary source files (targets for analysis, as opposed to linking).
       // DQ (1/16/2008): This is a better (simpler) implementation
       // if (CommandlineProcessing::isObjectFilename(argv[i]) == true)
       // printf ("get_binary_only() = %s \n",get_binary_only() ? "true" : "false");
          if ( (get_binary_only() == false) && (CommandlineProcessing::isObjectFilename(argv[i]) == true) )
             {
            // printf ("Adding argv[%u] = %s to p_objectFileNameList \n",i,argv[i].c_str());
               p_objectFileNameList.push_back(argv[i]);
             }

       // look only for .a files (library files)
          if ( (length > 2) &&
               ( (argv[i][0] != '-') && (argv[i][0] != '+') ) &&
               ( (argv[i][length-2] == '.') && (argv[i][length-1] == 'a') ) )
             {
               std::string libraryFile = argv[i];
               p_libraryFileList.push_back(libraryFile);

            // DQ (2/4/2009): Make sure that this is not handled incorrectly is we wanted it to be a target for binary analysis.
            // If so, then is should end up on the source code list (target list for analysis).
               if (get_binary_only() == true)
                  {
                    printf ("This may be an error, since the library archive should be treated as a source file for binary analysis. \n");
                    //ROSE_ASSERT(false);
                  }
             }

       // look only for -l library files (library files)
          if ( (length > 2) && (argv[i][0] == '-') && (argv[i][1] == 'l') )
             {
               std::string librarySpecifier = argv[i].substr(2);
            // librarySpecifier = std::string("lib") + librarySpecifier;
               p_librarySpecifierList.push_back(librarySpecifier);
             }

       // look only for -L directories (directories where -lxxx libraries will be found)
          if ( (length > 2) && (argv[i][0] == '-') && (argv[i][1] == 'L') )
             {
            // AS Changed source code to support absolute paths
               std::string libraryDirectorySpecifier = argv[i].substr(2);
               libraryDirectorySpecifier = StringUtility::getAbsolutePathFromRelativePath(libraryDirectorySpecifier);
               p_libraryDirectorySpecifierList.push_back(libraryDirectorySpecifier);

             }

       // look only for -I include directories (directories where #include<filename> will be found)
          if ( (length > 2) && (argv[i][0] == '-') && (argv[i][1] == 'I') )
             {
            // AS Changed source code to support absolute paths
               std::string includeDirectorySpecifier =  argv[i].substr(2);
               includeDirectorySpecifier = StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
               p_includeDirectorySpecifierList.push_back("-I"+includeDirectorySpecifier);
             }

       // DQ (10/18/2010): Added support to collect "-D" options (assume no space between the "-D" and the option (e.g. "-DmyMacro=8").
       // Note that we want to collect these because we have to process "-D" options more explicitly for Fortran (they are not required
       // for C/C++ because they are processed by the forontend directly.
          if ( (length > 2) && (argv[i][0] == '-') && (argv[i][1] == 'D') )
             {
               std::string macroSpecifier = argv[i].substr(2);
            // librarySpecifier = std::string("lib") + librarySpecifier;
               p_macroSpecifierList.push_back(macroSpecifier);
             }
        }

#if 1
     if ( get_verbose() > 1 )
        {
       // Find out what file we are doing transformations upon
          printf ("In SgProject::processCommandLine() (verbose mode ON): \n");
          display ("In SgProject::processCommandLine()");
        }
#endif

  // DQ (6/17/2005): Added support for AST merging (sharing common parts of the AST most often represented in common header files of a project)
  //
  // specify AST merge option
  //
  // if ( CommandlineProcessing::isOption(argc,argv,"-rose:","(astMerge)",true) == true )
     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","(astMerge)",true) == true )
        {
       // printf ("-rose:astMerge option found \n");
       // set something not yet defined!
          p_astMerge = true;
        }

  // DQ (6/17/2005): Added support for AST merging (sharing common parts of the AST most often represented in common header files of a project)
  //
  // specify AST merge command file option
  //
     std::string astMergeFilenameParameter;
     if ( CommandlineProcessing::isOptionWithParameter(local_commandLineArgumentList,
          "-rose:","(astMergeCommandFile)",astMergeFilenameParameter,true) == true )
        {
          printf ("-rose:astMergeCommandFile %s \n",astMergeFilenameParameter.c_str());
       // Make our own copy of the filename string
       // set_astMergeCommandLineFilename(xxx);
          p_astMergeCommandFile = astMergeFilenameParameter;
        }

  // DQ (8/29/2006): Added support for accumulation of performance data into CSV data file (for later processing to build performance graphs)
     std::string compilationPerformanceFilenameParameter;
     if ( CommandlineProcessing::isOptionWithParameter(local_commandLineArgumentList,
          "-rose:","(compilationPerformanceFile)",compilationPerformanceFilenameParameter,true) == true )
        {
       // printf ("-rose:compilationPerformanceFile = %s \n",compilationPerformanceFilenameParameter.c_str());
          p_compilationPerformanceFile = compilationPerformanceFilenameParameter;
        }

#if 0
     printf ("Exiting after SgProject::processCommandLine() \n");
     display("At base of SgProject::processCommandLine()");
  // ROSE_ASSERT (false);
#endif
   }

/*-----------------------------------------------------------------------------
 *  namespace SgFile {
 *---------------------------------------------------------------------------*/
void
SgFile::usage ( int status )
   {
     if (status != 0)
          fprintf (stderr,"Try option `--help' for more information.\n");
       else
        {
       // it would be nice to insert the version of ROSE being used (using the VERSION macro)
          fputs(
"\n"
"This ROSE translator provides a means for operating on C, C++, Fortran and Java\n"
"source code; as well as on x86, ARM, and PowerPC executable code (plus object files \n"
"and libraries).\n"
"\n"
"Usage: rose [OPTION]... FILENAME...\n"
"\n"
"If a long option shows a mandatory argument, it is mandatory for the equivalent\n"
"short option as well, and similarly for optional arguments.\n"
"\n"
"Main operation mode:\n"
"     -rose:(o|output) FILENAME\n"
"                             file containing final unparsed C++ code\n"
"                             (relative or absolute paths are supported)\n"
"\n"
"Operation modifiers:\n"
"     -rose:output_warnings   compile with warnings mode on\n"
"     -rose:C_only, -rose:C   follow C89 standard, disable C++\n"
"     -rose:C99_only, -rose:C99\n"
"                             follow C99 standard, disable C++\n"
"     -rose:Cxx_only, -rose:Cxx\n"
"                             follow C++ 89 standard\n"
"     -rose:Java, -rose:java, -rose:J, -rose:j\n"
"                             compile Java code (work in progress)\n"
"     -rose:java:cp, -rose:java:classpath\n"
"                             Classpath to look for java classes\n"
"     -rose:java:sourcepath\n"
"                             Sourcepath to look for java sources\n"
"     -rose:java:d\n"
"                             Specifies generated classes destination dir\n"
"     -rose:Python, -rose:python, -rose:py\n"
"                             compile Python code\n"
"     -rose:OpenMP, -rose:openmp\n"
"                             follow OpenMP 3.0 specification for C/C++ and Fortran, perform one of the following actions:\n"
"     -rose:OpenMP:parse_only, -rose:openmp:parse_only\n"
"                             parse OpenMP directives to OmpAttributes, no further actions (default behavior now)\n"
"     -rose:OpenMP:ast_only, -rose:openmp:ast_only\n"
"                             on top of -rose:openmp:parse_only, build OpenMP AST nodes from OmpAttributes, no further actions\n"
"     -rose:OpenMP:lowering, -rose:openmp:lowering\n"
"                             on top of -rose:openmp:ast_only, transform AST with OpenMP nodes into multithreaded code \n"
"                             targeting GCC GOMP runtime library\n"
"     -rose:UPC_only, -rose:UPC\n"
"                             follow Unified Parallel C 1.2 specification\n"
"     -rose:UPCxx_only, -rose:UPCxx\n"
"                             allows C++ within UPC (follows UPC 1.2 but simpily allows \n"
"                             using C++ as the base language) (not a legitimate language, \n"
"                             since there is no backend compiler to support this).\n"
"     -rose:upc_threads n     Enable UPC static threads compilation with n threads\n"
"                             n>=1: static threads; dynamic(default) otherwise\n"
"     -rose:Fortran, -rose:F, -rose:f\n"
"                             compile Fortran code, determining version of\n"
"                             Fortran from file suffix)\n"
"     -rose:CoArrayFortran, -rose:CAF, -rose:caf\n"
"                             compile Co-Array Fortran code (extension of Fortran 2003)\n"
"     -rose:CAF2.0, -rose:caf2.0\n"
"                             compile Co-Array Fortran 2.0 code (Rice CAF extension)\n"
"     -rose:Fortran2003, -rose:F2003, -rose:f2003\n"
"                             compile Fortran 2003 code\n"
"     -rose:Fortran95, -rose:F95, -rose:f95\n"
"                             compile Fortran 95 code\n"
"     -rose:Fortran90, -rose:F90, -rose:f90\n"
"                             compile Fortran 90 code\n"
"     -rose:Fortran77, -rose:F77, -rose:f77\n"
"                             compile Fortran 77 code\n"
"     -rose:Fortran66, -rose:F66, -rose:f66\n"
"                             compile Fortran 66 code\n"
"     -rose:FortranIV, -rose:FIV, -rose:fIV\n"
"                             compile Fortran IV code\n"
"     -rose:FortranII, -rose:FII, -rose:fII\n"
"                             compile Fortran II code (not implemented yet)\n"
"     -rose:FortranI, -rose:FI, -rose:fI\n"
"                             compile Fortran I code (not implemented yet)\n"
"     -rose:strict            strict enforcement of ANSI/ISO standards\n"
"     -rose:binary, -rose:binary_only\n"
"                             assume input file is for binary analysis (this avoids\n"
"                             ambiguity when ROSE might want to assume linking instead)\n"
"     -rose:astMerge          merge ASTs from different files\n"
"     -rose:astMergeCommandFile FILE\n"
"                             filename where compiler command lines are stored\n"
"                             for later processing (using AST merge mechanism)\n"
"     -rose:compilationPerformanceFile FILE\n"
"                             filename where compiler performance for internal\n"
"                             phases (in CSV form) is placed for later\n"
"                             processing (using script/graphPerformance)\n"
"     -rose:exit_after_parser just call the parser (C, C++, and fortran only)\n"
"     -rose:skip_syntax_check skip Fortran syntax checking (required for F2003 and Co-Array Fortran code\n"
"                             when using gfortran versions greater than 4.1)\n"
"     -rose:relax_syntax_check skip Fortran syntax checking (required for some F90 code\n"
"                             when using gfortran based syntax checking)\n"
"     -rose:skip_translation_from_edg_ast_to_rose_ast\n"
"                             skip the translation of the EDG AST into the ROSE AST\n"
"                             (an SgProject, SgFile, and empty SgGlobal will be constructed)\n"
"     -rose:skip_transformation\n"
"                             read input file and skip all transformations\n"
"     -rose:skip_unparse      read and process input file but skip generation of\n"
"                             final C++ output file\n"
"     -rose:skipfinalCompileStep\n"
"                             read and process input file, \n"
"                             but skip invoking the backend compiler\n"
"     -rose:collectAllCommentsAndDirectives\n"
"                             store all comments and CPP directives in header\n"
"                             files into the AST\n"
"     -rose:unparseHeaderFiles\n"
"                             unparse all directly or indirectly modified\n"
"                             header files\n"
"     -rose:excludeCommentsAndDirectives PATH\n"
"                             provide path to exclude when using the\n"
"                             collectAllCommentsAndDirectives option\n"
"     -rose:excludeCommentsAndDirectivesFrom FILENAME\n"
"                             provide filename to file with paths to exclude\n"
"                             when using the collectAllCommentsAndDirectives\n"
"                             option\n"
"     -rose:includeCommentsAndDirectives PATH\n"
"                             provide path to include when using the\n"
"                             collectAllCommentsAndDirectives option\n"
"     -rose:includeCommentsAndDirectivesFrom FILENAME\n"
"                             provide filename to file with paths to include\n"
"                             when using the collectAllCommentsAndDirectives\n"
"                             option\n"
"     -rose:skip_commentsAndDirectives\n"
"                             ignore all comments and CPP directives (can\n"
"                             generate (unparse) invalid code if not used with\n"
"                             -rose:unparse_includes)\n"
"     -rose:prelink           activate prelink mechanism to force instantiation\n"
"                             of templates and assignment to files\n"
"     -rose:instantiation XXX control template instantiation\n"
"                             XXX is one of (none, used, all, local)\n"
"     -rose:read_executable_file_format_only\n"
"                             ignore disassemble of instructions (helps debug binary \n"
"                             file format for binaries)\n"
"\n"
"GNU g++ options recognized:\n"
"     -ansi                   equivalent to -rose:strict\n"
"     -fno-implicit-templates disable output of template instantiations in\n"
"                             generated source\n"
"     -fno-implicit-inline-templates\n"
"                             disable output of inlined template instantiations\n"
"                             in generated source\n"
"\n"
"Informative output:\n"
"     -rose:help, --help, -help, --h\n"
"                             print this help, then exit\n"
"     -rose:version, --version, --V\n"
"                             print ROSE program version number, then exit\n"
"     -rose:markGeneratedFiles\n"
"                             add \"#define ROSE_GENERATED_CODE\" to top of all\n"
"                               generated code\n"
"     -rose:verbose [LEVEL]   verbosely list internal processing (default=0)\n"
"                               Higher values generate more output (can be\n"
"                               applied to individual files and to the project\n"
"                               separately).\n"
"     -rose:output_parser_actions\n"
"                             call parser with --dump option (fortran only)\n"
"     -rose:output_tokens     call parser with --tokens option (fortran only)\n"
"                             (not yet supported for C/C++)\n"
"     -rose:embedColorCodesInGeneratedCode LEVEL\n"
"                             embed color codes into generated output for\n"
"                               visualization of highlighted text using tview\n"
"                               tool for constructs specified by LEVEL\n"
"                             LEVEL is one of:\n"
"                               1: missing position information\n"
"                               2: compiler-generated code\n"
"                               3: other code\n"
"     -rose:generateSourcePositionCodes LEVEL\n"
"                             generate separate file of source position\n"
"                               information for highlighting original source\n"
"                               file using tview tool for constructs specified\n"
"                               by LEVEL\n"
"                             LEVEL is one of:\n"
"                               1: statements, preprocessor directives and\n"
"                                  comments\n"
"                               2: expressions\n"
"\n"
"Control EDG frontend processing:\n"
"     -edg:new_frontend       force use of external EDG front end (disables use\n"
"                               of rest of ROSE/SAGE)\n"
"     -edg:KCC_frontend       for use of KCC (with -c option) as new frontend\n"
"                               (must be specified with -edg:new_frontend)\n"
"     -edg:XXX                pass -XXX to EDG front-end\n"
"    --edg:XXX                pass --XXX to EDG front-end\n"
"     -edg_parameter: XXX YYY pass -XXX YYY to EDG front-end\n"
"    --edg_parameter: XXX YYY pass --XXX YYY to EDG front-end\n"
"\n"
"Control Fortran frontend processing:\n"
"     -rose:cray_pointer_support\n"
"                             turn on internal support for cray pointers\n"
"                             (Note: not implemented in front-end (OFP) yet.)\n"
"     -fortran:XXX            pass -XXX to independent semantic analysis\n"
"                             (useful for turning on specific warnings in front-end)\n"
"\n"
"Control Disassembly:\n"
"     -rose:disassembler_search HOW\n"
"                             Influences how the disassembler searches for instructions\n"
"                             to disassemble. HOW is a comma-separated list of search\n"
"                             specifiers. Each specifier consists of an optional\n"
"                             qualifier followed by either a word or integer. The\n"
"                             qualifier indicates whether the search method should be\n"
"                             added ('+') or removed ('-') from the set. The qualifier\n"
"                             '=' acts like '+' but first clears the set.  The words\n"
"                             are the lower-case versions of the Disassembler::SearchHeuristic\n"
"                             enumerated constants without the leading \"SEARCH_\" (see\n"
"                             doxygen documentation for the complete list and and their\n"
"                             meanings).   An integer (decimal, octal, or hexadecimal using\n"
"                             the usual C notation) can be used to set/clear multiple\n"
"                             search bits at one time. See doxygen comments for the\n"
"                             Disassembler::parse_switches class method for full details.\n"
"     -rose:partitioner_search HOW\n"
"                             Influences how the partitioner searches for functions.\n"
"                             HOW is a comma-separated list of search specifiers. Each\n"
"                             specifier consists of an optional qualifier followed by\n"
"                             either a word or integer. The qualifier indicates whether\n"
"                             the search method should be added ('+') or removed ('-')\n"
"                             from the set. The qualifier '=' acts like '+' but first\n"
"                             clears the set.  The words are the lower-case versions of\n"
"                             most of the SgAsmFunction::FunctionReason\n"
"                             enumerated constants without the leading \"FUNC_\" (see\n"
"                             doxygen documentation for the complete list and and their\n"
"                             meanings).   An integer (decimal, octal, or hexadecimal using\n"
"                             the usual C notation) can be used to set/clear multiple\n"
"                             search bits at one time. See doxygen comments for the\n"
"                             Partitioner::parse_switches class method for full details.\n"
"     -rose:partitioner_config FILENAME\n"
"                             File containing configuration information for the\n"
"                             instruction/block/function partitioner. This config\n"
"                             file can be used to override block successors,\n"
"                             alias two or more blocks that have identical\n"
"                             semantics, assign particular blocks to functions,\n"
"                             override function return analysis, provide or\n"
"                             override function names, etc. See documentation for\n"
"                             the IPDParser class for details.\n"
"\n"
"Control code generation:\n"
"     -rose:unparse_line_directives\n"
"                               unparse statements using #line directives with\n"
"                               reference to the original file and line number\n"
"                               to support view of original source in debuggers\n"
"                               and external tools\n"
"     -rose:unparse_instruction_addresses\n"
"                               Outputs the addresses in left column (output\n"
"                               inappropriate as input to assembler)\n"
"     -rose:unparse_raw_memory_contents\n"
"                               Outputs memory contents in left column\n"
"     -rose:unparse_binary_file_format\n"
"                               Outputs binary executable file format information\n"
"     -rose:unparse_includes\n"
"                               unparse all include files into the source file.\n"
"                               This is a backup option for fail-safe processing\n"
"                               of CPP directives (which can be tricky)\n"
"     -rose:C_output_language\n"
"                             force use of C as output language (currently\n"
"                               generates C/C++)\n"
"     -rose:Cxx_output_language\n"
"                             force use of C++ as output language\n"
"     -rose:Fortran_output_language\n"
"                             force use of Fortran as output language\n"
"     -rose:Promela_output_language\n"
"                             force use of Promela as output language (not\n"
"                               supported)\n"
"     -rose:PHP_output_language\n"
"                             force use of PHP as output language\n"
"     -rose:outputFormat      generate code in either fixed/free format (fortran only)\n"
"                               options are: fixedOutput|fixedFormatOutput or \n"
"                                            freeOutput|freeFormatOutput\n"
"     -rose:backendCompileFormat\n"
"                             use backend compiler option to compile generated code\n"
"                               in either fixed/free format (fortran only)\n"
"                               options are: fixedOutput|fixedFormatOutput or \n"
"                                            freeOutput|freeFormatOutput\n"
"     -rose:unparseHeaderFilesRootFolder FOLDERNAME\n"
"                             A relative or an absolute path to the root folder,\n"
"                             in which unparsed header files are stored.\n"
"                             Note that the folder must be empty (or does not exist).\n"
"                             If not specified, the default relative location _rose_ \n"
"                             is used.\n"                  
"\n"
"Debugging options:\n"
"     -rose:detect_dangling_pointers LEVEL \n"
"                             detects references to previously deleted IR nodes in the AST\n"
"                             (part of AST consistancy tests, default is false since some codes fail this test)\n"
"                             LEVEL is one of:\n"
"                               0: off (does not issue warning)\n"
"                               1: on (issues warning with information)\n"
"                               2: on (issues error and exists)\n"
"\n"
"Testing Options:\n"
"     -rose:negative_test     test ROSE using input that is expected to fail\n"
"                               (returns 0 if input test failed, else error if\n"
"                               passed)\n"
"     -rose:test LEVEL        limit parts of ROSE which are run\n"
"                             LEVEL is one of:\n"
"                               0: transparent (ROSE translator does nothing)\n"
"                               1: run the KCC front end only (assumes it is in\n"
"                                    path)\n"
"                               2: run the newer version of EDG (compiled\n"
"                                    separately from SAGE) 'edgFrontEnd'\n"
"                                    (see\n"
"                                    src/frontend/EDG/EDG_3.3/src/Makefile.am\n"
"                                    for instructions on how to build EDG\n"
"                                    without SAGE III)\n"
"                               3: run internal (older) version of edg front end\n"
"                                    (deprecated option)\n"
"                               4: same as 3 plus parse into Sage III program\n"
"                                    tree\n"
"                               5: same as 4 plus unparse untransformed source\n"
"                                    code\n"
"                               6: same as 5 plus compile generated source code\n"
"                               7: same as 5 plus build higher level grammars\n"
"                                    before unparsing\n"
"                               8: same as 6 plus run midend (transformations)\n"
"                                    before unparsing\n"
"\n"
"Report bugs to <dquinlan@llnl.gov>.\n"
  , stdout);

  // Obsolete options
  // -sage:sage_backend            have EDG call the sage backend
  // -sage:disable_cp_backend      prevent EDG from calling the cp backend
  // -rose:outputGrammarTreeFiles  write out program tree representation in C++ grammar (useful for internal debugging)
  // -rose:outputGrammarTreeFilesForHeaderFiles (include header files in above option (must be specified after above option)

  // DQ (5/20/2005): More obsolete options
  // -edg:disable_edg_backend      prevent EDG from calling the sage backend
  // -sage:disable_sage_backend    prevent EDG from calling the sage backend
  // -sage:enable_cp_backend       have EDG call the cp backend
  // -sage:preinit_il              do a preinit stage between the front-end and
    }

#if 1
  // Comment this out for now while we test!
     exit (status);
#endif
   }

void
SgFile::processRoseCommandLineOptions ( vector<string> & argv )
   {
  // Strip out the rose specific command line options
  // then search for all filenames (options without the "-" prefix)
  // the assume all other arguments are to be passed onto the C or C++ compiler

  // int optionCount = 0;
  // int i = 0;

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

  // Split out the ROSE options first

  // printf ("Before processing ROSE options argc = %d \n",argc);

  //
  // help option (allows alternative -h or -help instead of just -rose:help)
  // This is the only rose options that does not require the "-rose:" prefix
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(h|help)",true) == true ||
          CommandlineProcessing::isOption(argv,"--", "(h|help)",true)    == true ||
          CommandlineProcessing::isOption(argv,"-","(h|help)",true)      == true )
        {
       // printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
       // ROSE::usage(0);
          cout << version_message() << endl;
          usage(0);
       // exit(0);
        }
  //
  // version option
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(V|version)",true) == true )
        {
       // function in SAGE III to access version number of EDG
          extern std::string edgVersionString();
       // printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          cout << version_message() << endl;
          printf ("     Using C++ and C frontend from EDG (version %s) internally \n",edgVersionString().c_str());
        }

  //
  // markGeneratedFiles option
  //
     set_markGeneratedFiles(false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(markGeneratedFiles)",true) == true )
        {
       // Optionally mark the generated files with "#define ROSE_GENERATED_CODE"
          set_markGeneratedFiles(true);
        }

  //
  // embedColorCodesInGeneratedCode option
  //
     int integerOptionForEmbedColorCodesInGeneratedCode = 0;
     if ( CommandlineProcessing::isOptionWithParameter(argv,"-rose:","(embedColorCodesInGeneratedCode)",integerOptionForEmbedColorCodesInGeneratedCode,true) == true )
        {
          printf ("Calling set_embedColorCodesInGeneratedCode(%d) \n",integerOptionForEmbedColorCodesInGeneratedCode);
          set_embedColorCodesInGeneratedCode(integerOptionForEmbedColorCodesInGeneratedCode);
        }

  //
  // generateSourcePositionCodes option
  //
     int integerOptionForGenerateSourcePositionCodes = 0;
     if ( CommandlineProcessing::isOptionWithParameter(argv,"-rose:","(generateSourcePositionCodes)",integerOptionForGenerateSourcePositionCodes,true) == true )
        {
          printf ("Calling set_generateSourcePositionCodes(%d) \n",integerOptionForGenerateSourcePositionCodes);
          set_generateSourcePositionCodes(integerOptionForGenerateSourcePositionCodes);
        }

  //
  // verbose option
  //
  // DQ (4/20/2006): This can already be set (to none zero value) if the SgProject was previously build already (see tutorial/selectedFileTranslation.C)
  // ROSE_ASSERT (get_verbose() == 0);
  // if ( CommandlineProcessing::isOption(argv,"-rose:","(v|verbose)",true) )
     int integerOptionForVerbose = 0;
     if ( CommandlineProcessing::isOptionWithParameter(argv,"-rose:","(v|verbose)",integerOptionForVerbose,true) == true )
        {
       // set_verbose(true);
          set_verbose(integerOptionForVerbose);

       // DQ (8/12/2004): The semantics is to set the global concept of a value
       // for verbose to the maximum of that from the individual files.
          if (SgProject::get_verbose() < integerOptionForVerbose)
               SgProject::set_verbose(integerOptionForVerbose);

          if ( SgProject::get_verbose() >= 1 )
               printf ("verbose mode ON (for SgFile)\n");
        }


  //
  // Turn on warnings (turns on warnings in fronend, for Fortran support this turns on detection of
  // warnings in initial syntax checking using gfortran before passing control to Open Fortran Parser).
  //
     set_output_warnings(false);
     ROSE_ASSERT (get_output_warnings() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(output_warnings)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("output warnings mode ON \n");
          set_output_warnings(true);
        }

  //
  // Turn on warnings (turns on warnings in fronend, for Fortran support this turns on detection of
  // warnings in initial syntax checking using gfortran before passing control to Open Fortran Parser).
  //
     set_cray_pointer_support(false);
     ROSE_ASSERT (get_cray_pointer_support() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(cray_pointer_support)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("cray pointer mode ON \n");
          set_cray_pointer_support(true);
        }

  //
  // Turn on the output of the parser actions for the parser (only applies to Fortran support).
  //
     set_output_parser_actions(false);
     ROSE_ASSERT (get_output_parser_actions() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(output_parser_actions)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("output parser actions mode ON \n");
          set_output_parser_actions(true);
        }

  //
  // Turn on the output of the parser actions for the parser (only applies to Fortran support).
  //
     set_exit_after_parser(false);
     ROSE_ASSERT (get_exit_after_parser() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(exit_after_parser)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("exit after parser mode ON \n");
          set_exit_after_parser(true);
        }

  //
  // DQ (11/20/2010): Added token handling support.
  // Turn on the output of the tokens from the parser (only applies to Fortran support).
  //
     set_output_tokens(false);
     ROSE_ASSERT (get_output_tokens() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(output_tokens)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("output tokens mode ON \n");
          set_output_tokens(true);
        }

  //
  // Turn on the output of the parser actions for the parser (only applies to Fortran support).
  //
     set_skip_syntax_check(false);
     ROSE_ASSERT (get_skip_syntax_check() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_syntax_check)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("skip syntax check mode ON \n");
          set_skip_syntax_check(true);
        }

  //
  // Turn on relaxed syntax checking mode (only applies to Fortran support).
  //
     set_relax_syntax_check(false);
     ROSE_ASSERT (get_relax_syntax_check() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(relax_syntax_check)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("relax syntax check mode ON \n");
          set_relax_syntax_check(true);
        }

  //
  // C only option (turns on EDG "--c" option and g++ "-xc" option)
  //

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
  // DQ (12/2/2006): Note that the filename extension could have set this to C++ mode and we only don't want an explicit specification of "-rose:C" to change this.
  // set_C_only(false);
  // ROSE_ASSERT (get_C_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(C|C_only)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("C only mode ON \n");
          set_C_only(true);

       // I think that explicit specificiation of C mode should turn off C++ mode!
          set_Cxx_only(false);

          if (get_sourceFileUsesCppFileExtension() == true)
             {
               printf ("Warning, C++ source file name specificed with explicit -rose:C C language option! (ignoring explicit option to mimic gcc behavior) \n");
               set_C_only(false);
             }
        }

  //
  // C99 only option (turns on EDG "--c" option and g++ "-xc" option)
  //
     set_C99_only(false);
     ROSE_ASSERT (get_C99_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(C99|C99_only)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("C99 mode ON \n");
          set_C99_only(true);
        }
  //
  // UPC only option , enable UPC mode of ROSE, especially the file suffix is not .upc
  // It is an extension of C, so also set C mode.
  // Liao, 6/19/2008
  //
  // set_UPC_only(false); // invalidate the flag set by SgFile::setupSourceFilename() based on .upc suffix
  // ROSE_ASSERT (get_UPC_only() == false);
     bool hasRoseUpcEnabled = CommandlineProcessing::isOption(argv,"-rose:","(UPC|UPC_only)",true) ;
     bool hasEdgUpcEnabled  = CommandlineProcessing::isOption(argv,"--edg:","(upc)",true) ;
     bool hasEdgUpcEnabled2 = CommandlineProcessing::isOption(argv,"-edg:","(upc)",true) ;

     if (hasRoseUpcEnabled||hasEdgUpcEnabled2||hasEdgUpcEnabled)
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("UPC mode ON \n");
          set_C_only(true);
          set_UPC_only(true);
       // remove edg:restrict since we will add it back in SgFile::build_EDG_CommandLine()
          CommandlineProcessing::isOption(argv,"-edg:","(restrict)",true);
          CommandlineProcessing::isOption(argv,"--edg:","(restrict)",true);
        }

  // DQ (9/19/2010): Added support for UPC++.  This uses the UPC mode and internally processes the code as C++ instead of C.
  // set_UPCpp_only(false); // invalidate the flag set by SgFile::setupSourceFilename() based on .upc suffix
  // ROSE_ASSERT (get_UPCxx_only() == false);
     bool hasRoseUpcppEnabled = CommandlineProcessing::isOption(argv,"-rose:","(UPCxx|UPCxx_only)",true) ;

  // DQ (10/22/2010): Remove specification of edg specific upc++ option (used for testing).
  // bool hasEdgUpcppEnabled  = CommandlineProcessing::isOption(argv,"--edg:","(upc++)",true) ;
  // bool hasEdgUpcppEnabled2 = CommandlineProcessing::isOption(argv,"-edg:","(upc++)",true) ;
  // bool hasEdgUpcppEnabled  = CommandlineProcessing::isOption(argv,"--edg:","(upcxx)",true) ;
  // bool hasEdgUpcppEnabled2 = CommandlineProcessing::isOption(argv,"-edg:","(upcxx)",true) ;
  // if (hasRoseUpcppEnabled||hasEdgUpcppEnabled2||hasEdgUpcppEnabled)

     set_UPCxx_only(false);
     if (hasRoseUpcppEnabled == true)
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("UPC++ mode ON \n");
          set_C_only(false);
          set_Cxx_only(true);
          set_UPCxx_only(true);
       // remove edg:restrict since we will add it back in SgFile::build_EDG_CommandLine()
          CommandlineProcessing::isOption(argv,"-edg:","(restrict)",true);
          CommandlineProcessing::isOption(argv,"--edg:","(restrict)",true);
        }

  // two situations: either of -rose:upc_threads n  and --edg:upc_threads n appears.
  // set flags and remove both.
     int integerOptionForUPCThreads  = 0;
     int integerOptionForUPCThreads2 = 0;
     bool hasRoseUpcThreads = CommandlineProcessing::isOptionWithParameter(argv,"-rose:","(upc_threads)", integerOptionForUPCThreads,true);
     bool hasEDGUpcThreads  = CommandlineProcessing::isOptionWithParameter(argv,"--edg:","(upc_threads)", integerOptionForUPCThreads2,true);

     integerOptionForUPCThreads = (integerOptionForUPCThreads != 0) ? integerOptionForUPCThreads : integerOptionForUPCThreads2;
     if (hasRoseUpcThreads||hasEDGUpcThreads)
        {
       // set ROSE SgFile::upc_threads value, done for ROSE
          set_upc_threads(integerOptionForUPCThreads);
          if ( SgProject::get_verbose() >= 1 )
               printf ("upc_threads is set to %d\n",integerOptionForUPCThreads);
        }
  //
  // C++ only option
  //
  // set_Cxx_only(false);
  // ROSE_ASSERT (get_Cxx_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(Cxx|Cxx_only)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("C++ mode ON \n");
          set_Cxx_only(true);

       // Not clear what behavior I want here!
#if 1
          set_C_only(false);
          set_C99_only(false);
#else
          if (get_sourceFileUsesCFileExtension() == true)
             {
               printf ("Note, C source file name specificed with explicit -rose:Cxx C++ language option! \n");
               set_C_only(false);
             }
#endif
        }

  // DQ (2/5/2009): We now have one at the SgProject and the SgFile levels.
  // DQ (2/4/2009): Moved to SgProject.
  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(binary|binary_only)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Binary only mode ON \n");
          set_binary_only(true);
          if (get_sourceFileUsesBinaryFileExtension() == false)
             {
               printf ("Warning, Non binary file name specificed with explicit -rose:binary option! \n");
               set_binary_only(false);
             }
        }

  // DQ (10/11/2010): Adding initial Java support.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(j|J|Java)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Java only mode ON \n");
          set_Java_only(true);
          if (get_sourceFileUsesJavaFileExtension() == false)
             {
               printf ("Warning, Non Java source file name specified with explicit -rose:Java Java language option! \n");
               set_Java_only(false);

            // DQ (4/2/2011): Java code is only compiled, not linked as is C/C++ and Fortran.
               set_compileOnly(true);
             }
        }

  // driscoll6 (8/8/11): python support
     if ( CommandlineProcessing::isOption(argv,"-rose:","(py|python|Python)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Python only mode ON \n");
          set_Python_only(true);
          if (get_sourceFileUsesPythonFileExtension() == false)
             {
               printf ("Warning, Non Python source file name specificed with explicit -rose:python Python language option! \n");
               set_Python_only(false);
             }
        }

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
  // set_Fortran_only(false);
  // ROSE_ASSERT (get_Fortran_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(f|F|Fortran)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran only mode ON \n");
          set_Fortran_only(true);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with explicit -rose:Fortran Fortran language option! \n");
               set_Fortran_only(false);
             }
        }

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
  // set_F77_only(false);
  // ROSE_ASSERT (get_F77_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(f77|F77|Fortran77)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran77 only mode ON \n");
          set_F77_only(true);
          set_Fortran_only(true);
          if (get_sourceFileUsesFortran77FileExtension() == false)
             {
               printf ("Warning, Non Fortran77 source file name specificed with explicit -rose:Fortran77 Fortran 77 language option! \n");
               set_F77_only(false);
             }
        }

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
  // set_F90_only(false);
  // ROSE_ASSERT (get_F90_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(f90|F90|Fortran90)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran90 only mode ON \n");
          set_F90_only(true);
          set_Fortran_only(true);
          if (get_sourceFileUsesFortran90FileExtension() == false)
             {
               printf ("Warning, Non Fortran90 source file name specificed with explicit -rose:Fortran90 Fortran 90 language option! \n");
               set_F90_only(false);
             }
        }

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
  // set_F95_only(false);
  // ROSE_ASSERT (get_F95_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(f95|F95|Fortran95)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran95 only mode ON \n");
          set_F95_only(true);
          set_Fortran_only(true);
          if (get_sourceFileUsesFortran95FileExtension() == false)
             {
               printf ("Warning, Non Fortran95 source file name specificed with explicit -rose:Fortran95 Fortran 95 language option! \n");
               set_F95_only(false);
             }
        }

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
  // set_F2003_only(false);
  // ROSE_ASSERT (get_F2003_only() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(f2003|F2003|Fortran2003)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran2003 only mode ON \n");
          set_F2003_only(true);
          set_Fortran_only(true);
          if (get_sourceFileUsesFortran2003FileExtension() == false)
             {
               printf ("Warning, Non Fortran2003 source file name specificed with explicit -rose:Fortran2003 Fortran 2003 language option! \n");
               set_F2003_only(false);
             }
        }

     if ( CommandlineProcessing::isOption(argv,"-rose:","(caf|CAF|CoArrayFortran)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Co-Array Fortran only mode ON \n");
          set_CoArrayFortran_only(true);

       // Set this as also being F2003 code since Co-Array Fortran is an extension of Fortran 2003
          set_F2003_only(true);
          set_Fortran_only(true);

       // DQ (12/2/2010): I agree with setting this to true.
       // It is requested (by Laksono at Rice) that CoArray Fortran defaults be to skip the syntax checking
       // Laksono 2009.01.27: I think we should put the boolean to 'true' instead of 'false'
          set_skip_syntax_check(true);

          if (get_sourceFileUsesCoArrayFortranFileExtension() == false)
             {
               printf ("Warning, Non Co-Array Fortran source file name specificed with explicit -rose:CoArrayFortran language option! \n");
               set_CoArrayFortran_only(false);
             }
        }

  // Fixed format v.s. free format option handling (ROSE defaults to fix or free format, depending on the file extension).
  // F77 default is fixed format, F90 and later default is free format.
  // Fortran source file format options for different compilers(for IBM/XL,Intel,Portland,GNU):
  //     IBM/XL           Intel            Portland     GNU
  //    -qfixed          -fixed           -Mfixed      -ffixed-form
  //    -qfree           -free            -Mfree       -free-form
  // GNU gfortran also supports -fno-fixed-form (we could use this to turn off all fixed form
  // formatting independent of the input source).

  // DQ (12/10/2007): Set the defaul value depending on the version of fortran being used.
     if (get_F77_only() == true)
        {
       // For F77 and older versions of Fortran assume fixed format.
       // set_fixedFormat(true);
       // set_freeFormat (false);

       // Use the setting get_F77_only() == true as a default means to set this value
          set_inputFormat(SgFile::e_fixed_form_output_format);
          set_outputFormat(SgFile::e_fixed_form_output_format);
          set_backendCompileFormat(SgFile::e_fixed_form_output_format);
        }
       else
        {
       // For F90 and later versions of Fortran assume free format.
       // set_fixedFormat(false);
       // set_freeFormat (true);

       // Use the setting get_F77_only() == true as a default means to set this value
          set_inputFormat(SgFile::e_free_form_output_format);
          set_outputFormat(SgFile::e_free_form_output_format);
          set_backendCompileFormat(SgFile::e_free_form_output_format);
        }

  // Make sure that only one is true
  // ROSE_ASSERT (get_freeFormat() == false || get_fixedFormat() == false);
  // ROSE_ASSERT (get_freeFormat() == true  || get_fixedFormat() == true);

  // set_fixedFormat(false);
  // ROSE_ASSERT (get_fixedFormat() == false);
     if ( CommandlineProcessing::isOption(argv,"-","(ffixed-form|fixed|Mfixed|qfixed)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran fixed format mode explicitly set: ON \n");
       // set_fixedFormat(true);
          set_inputFormat(SgFile::e_fixed_form_output_format);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Error, Non Fortran source file name specificed with explicit fixed format code generation (unparser) option! \n");
            // set_fixedFormat(false);
               ROSE_ASSERT(false);
             }
        }

  // set_freeFormat(false);
  // ROSE_ASSERT (get_freeFormat() == false);
     if ( CommandlineProcessing::isOption(argv,"-","(ffree-form|free|Mfree|qfree)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran free format mode explicitly set: ON \n");
       // set_freeFormat(true);
          set_inputFormat(SgFile::e_free_form_output_format);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Error, Non Fortran source file name specificed with explicit free format code generation (unparser) option! \n");
            // set_freeFormat(false);
               ROSE_ASSERT(false);
             }
        }

  // DQ (8/19/2007): This option only controls the output format (unparsing) of Fortran code (free or fixed format).
  // It has no effect on C/C++ code generation (unparsing).
  // printf ("########################### Setting the outputFormat ########################### \n");

  // DQ (12/27/2007) These have been assigned default values based on the filename suffix, we only want to override
  // them if the commandline options are explicitly specified.
  // set_outputFormat(SgFile::e_unknown_output_format);
  // ROSE_ASSERT (get_outputFormat() == SgFile::e_unknown_output_format);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(fixedOutput|fixedFormatOutput)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran fixed format output specified \n");
          set_outputFormat(SgFile::e_fixed_form_output_format);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with code generation option: free format! \n");
               ROSE_ASSERT(false);
             }
        }
     if ( CommandlineProcessing::isOption(argv,"-rose:","(freeOutput|freeFormatOutput)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran free format mode explicitly set: ON \n");
          set_outputFormat(SgFile::e_free_form_output_format);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with code generation option: fixed format! \n");
               ROSE_ASSERT(false);
             }
        }

  // DQ (8/19/2007): This option only controls the output format (unparsing) of Fortran code (free or fixed format).
  // It has no effect on C/C++ code generation (unparsing).
  // printf ("########################### Setting the backendCompileFormat ########################### \n");

  // DQ (12/27/2007) These have been assigned default values based on the filename suffix, we only want to override
  // them if the commandline options are explicitly specified.
  // set_backendCompileFormat(SgFile::e_unknown_output_format);
  // ROSE_ASSERT (get_backendCompileFormat() == SgFile::e_unknown_output_format);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(compileFixed|backendCompileFixedFormat)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran fixed format output specified \n");
          set_backendCompileFormat(SgFile::e_fixed_form_output_format);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with code generation option: free format! \n");
               ROSE_ASSERT(false);
             }
        }

     if ( CommandlineProcessing::isOption(argv,"-rose:","(compileFree|backendCompileFreeFormat)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran free format mode explicitly set: ON \n");
          set_backendCompileFormat(SgFile::e_free_form_output_format);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with code generation option: fixed format! \n");
               ROSE_ASSERT(false);
             }
        }

     set_fortran_implicit_none(false);
     ROSE_ASSERT (get_fortran_implicit_none() == false);
     if ( CommandlineProcessing::isOption(argv,"-","fimplicit_none",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran free format mode explicitly set: ON \n");
          set_fortran_implicit_none(true);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with explicit free format code generation (unparser) option! \n");
               set_fortran_implicit_none(false);
             }
        }

  // Liao 10/28/2008: I changed it to a more generic flag to indicate support for either Fortran or C/C++
  // DQ (8/19/2007): I have added the option here so that we can start to support OpenMP for Fortran.
  // Allows handling of OpenMP "!$omp" directives in free form and "c$omp", *$omp and "!$omp" directives in fixed form, enables "!$" conditional
  // compilation sentinels in free form and "c$", "*$" and "!$" sentinels in fixed form and when linking arranges for the OpenMP runtime library
  // to be linked in. (Not implemented yet).
     set_openmp(false);
     ROSE_ASSERT (get_openmp() == false);
     // We parse OpenMP and then stop now since Building OpenMP AST nodes is a work in progress.
     // so the default behavior is to turn on them all
     // TODO turn them to false when parsing-> AST creation -> translation are finished
     ROSE_ASSERT (get_openmp_parse_only() == true);
     ROSE_ASSERT (get_openmp_ast_only() == false);
     ROSE_ASSERT (get_openmp_lowering() == false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(OpenMP|openmp)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("OpenMP option specified \n");
          set_openmp(true);
          /*
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specified with explicit OpenMP option! \n");
               set_fortran_openmp(false);
             }
             */
         //side effect for enabling OpenMP, define the macro as required
         //This new option does not reach the backend compiler
         //But it is enough to reach EDG only.
         //We can later on back end option to turn on their OpenMP handling flags,
         //like -fopenmp for GCC, depending on the version of gcc
         //which will define this macro for GCC
          argv.push_back("-D_OPENMP");
        }

     // Process sub-options for OpenMP handling, Liao 5/31/2009
     // We want to turn on OpenMP if any of its suboptions is used.  Liao , 8/11/2009
     if ( CommandlineProcessing::isOption(argv,"-rose:OpenMP:","parse_only",true) == true
         ||CommandlineProcessing::isOption(argv,"-rose:openmp:","parse_only",true) == true)
     {
       if ( SgProject::get_verbose() >= 1 )
         printf ("OpenMP sub option for parsing specified \n");
       set_openmp_parse_only(true);
       // turn on OpenMP if not set explicitly by standalone -rose:OpenMP
       if (!get_openmp())
       {
         set_openmp(true);
         argv.push_back("-D_OPENMP");
       }
     }

     if ( CommandlineProcessing::isOption(argv,"-rose:OpenMP:","ast_only",true) == true
         ||CommandlineProcessing::isOption(argv,"-rose:openmp:","ast_only",true) == true)
     {
       if ( SgProject::get_verbose() >= 1 )
         printf ("OpenMP option for AST construction specified \n");
       set_openmp_ast_only(true);
       // we don't want to stop after parsing  if we want to proceed to ast creation before stopping
       set_openmp_parse_only(false);
       // turn on OpenMP if not set explicitly by standalone -rose:OpenMP
       if (!get_openmp())
       {
         set_openmp(true);
         argv.push_back("-D_OPENMP");
       }
     }

     if ( CommandlineProcessing::isOption(argv,"-rose:OpenMP:","lowering",true) == true
         ||CommandlineProcessing::isOption(argv,"-rose:openmp:","lowering",true) == true)
     {
       if ( SgProject::get_verbose() >= 1 )
         printf ("OpenMP sub option for AST lowering specified \n");
       set_openmp_lowering(true);
       // we don't want to stop after parsing or ast construction
       set_openmp_parse_only(false);
       set_openmp_ast_only(false);
       // turn on OpenMP if not set explicitly by standalone -rose:OpenMP
       if (!get_openmp())
       {
         set_openmp(true);
         argv.push_back("-D_OPENMP");
       }
     }

  //
  // strict ANSI/ISO mode option
  //
  // set_strict_language_handling(false);
     if ( CommandlineProcessing::isOption(argv,"-rose:","(strict)",true) == true )
        {
       // Optionally specify strict language handling
          set_strict_language_handling(true);
        }

  //
  // specify output file option
  //
  // DQ (10/15/2005): There is a valid default value here, but we want to overwrite it!
     std::string stringParameter;
     if ( CommandlineProcessing::isOptionWithParameter(argv,"-rose:","(o|output)",stringParameter,true) == true )
        {
       // printf ("-rose:output %s \n",stringParameter.c_str());
          if (get_unparse_output_filename().empty() == false)
             {
               printf ("Overwriting value in get_unparse_output_filename() = %s \n",get_unparse_output_filename().c_str());
             }

       // DQ (10/15/2005): This is so much simpler!
          p_unparse_output_filename = stringParameter;
        }

  // printf ("After processing -rose:output option argc = %d \n",argc);
  // ROSE_ABORT();

#if 0
  // DQ (11/1/2011): This option was removed from SgFile at some point in the past (so we don't need the support there).

  // DQ (4/20/2006): Added to support fall through option to be supported by user translators.
  //
  // skip_rose option (just call the backend compiler directly): This causes Rose to act identally
  // to the backend compiler (with no creation of the ROSE AST, translation, code generation, etc.).
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_rose)",true) == true )
        {
          printf ("option -rose:skip_rose found \n");
       // set_skip_rose(true);

       // Need these to be set correctly as well
          p_useBackendOnly = true;
       // p_skip_buildHigherLevelGrammars  = true;
          p_disable_edg_backend  = true; // This variable should be called frontend NOT backend???
          p_skip_transformation  = true;
          p_skip_unparse         = true;
          p_skipfinalCompileStep = false;

       // Skip all processing of comments
          set_skip_commentsAndDirectives(true);
          set_collectAllCommentsAndDirectives(false);
          set_unparseHeaderFiles(false);
        }
#endif

  //
  // skip_translation_from_edg_ast_to_rose_ast option: This variable is checked in the EDG frontend (4.3) 
  // and if set it will cause the translation from the EDG AST to the ROSE AST to be skipped.  A valid
  // SgProject and/or SgFile with with SgGlobal (empty) will be built (as I recall).
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_translation_from_edg_ast_to_rose_ast)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("option -rose:skip_translation_from_edg_ast_to_rose_ast found \n");
          set_skip_translation_from_edg_ast_to_rose_ast(true);
        }

  //
  // skip_transformation option: if transformations of the AST check this variable then the
  // resulting translators can skip the transformatios via this command-line specified option.
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_transformation)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("option -rose:skip_transformation found \n");
          set_skip_transformation(true);
        }

  //
  // skip_unparse option: if analysis only (without transformatio is required, then this can significantly
  // improve the performance since it will also skip the backend compilation, as I recall)
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_unparse)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("option -rose:skip_unparse found \n");
          set_skip_unparse(true);
        }


  // unparser language option
  // DQ (8/27/2007): This option controls the output language (which language unparser is to be used).
  // This allows the code generation of one language to be tested using input of another langauge.  It is
  // mostly a mechanism to test the unparser in the early stages of their development. Unshared language
  // constructs are not supported and would have the be translated.  This step does none of this sort of
  // translation, which might be difficult debendingon the input and output languages selected.
  // Supported options are:
  //      e_default_output_language
  //      e_C_output_language
  //      e_Cxx_output_language
  //      e_Fortran_output_language
  //      e_Promela_output_language
  // set_outputLanguage(SgFile::e_default_output_language);
  // ROSE_ASSERT (get_outputLanguage() == SgFile::e_default_output_language);
     if ( CommandlineProcessing::isOption(argv,"-rose:","C_output_language",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Use the C language code generator (unparser) \n");
          set_outputLanguage(SgFile::e_C_output_language);
        }
     if ( CommandlineProcessing::isOption(argv,"-rose:","Cxx_output_language",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Use the C++ language code generator (unparser) \n");
          set_outputLanguage(SgFile::e_Cxx_output_language);
        }
     if ( CommandlineProcessing::isOption(argv,"-rose:","Fortran_output_language",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Use the Fortran language code generator (unparser) \n");
          set_outputLanguage(SgFile::e_Fortran_output_language);
        }
     if ( CommandlineProcessing::isOption(argv,"-rose:","Promela_output_language",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Use the Promela language code generator (unparser) \n");
          set_outputLanguage(SgFile::e_Promela_output_language);
        }
     if ( CommandlineProcessing::isOption(argv,"-rose:","PHP_output_language",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Use the PHP language code generator (unparser) \n");
          set_outputLanguage(SgFile::e_PHP_output_language);
        }


  //
  // unparse_includes option
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(unparse_includes)",true) == true )
        {
       // printf ("option -rose:unparse_includes found \n");
          set_unparse_includes(true);
        }

  //
  // unparse_line_directives option (added 12/4/2007).
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(unparse_line_directives)",true) == true )
        {
       // printf ("option -rose:unparse_line_directives found \n");
          set_unparse_line_directives(true);
        }

  //
  // unparse_instruction_addresses option (added 8/30/2008).
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(unparse_instruction_addresses)",true) == true )
        {
       // printf ("option -rose:unparse_instruction_addresses found \n");
          set_unparse_instruction_addresses(true);
        }

  //
  // unparse_raw_memory_contents option (added 8/30/2008).
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(unparse_raw_memory_contents)",true) == true )
        {
       // printf ("option -rose:unparse_raw_memory_contents found \n");
          set_unparse_raw_memory_contents(true);
        }

  //
  // unparse_binary_file_format option (added 8/30/2008).
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(unparse_binary_file_format)",true) == true )
        {
       // printf ("option -rose:unparse_binary_file_format found \n");
          set_unparse_binary_file_format(true);
        }

  //
  // collectAllCommentsAndDirectives option: operates across all files (include files) and significantly slows the compilation.
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(collectAllCommentsAndDirectives)",true) == true )
        {
       // printf ("option -rose:collectAllCommentsAndDirectives found \n");
          set_collectAllCommentsAndDirectives(true);
        }

     // negara1 (07/08/2011): Made unparsing of header files optional. 
     if ( CommandlineProcessing::isOption(argv,"-rose:","(unparseHeaderFiles)",true) == true )
        {
          set_unparseHeaderFiles(true);
          //To support header files unparsing, it is always needed to collect all directives.
          set_collectAllCommentsAndDirectives(true);
        }

     // negara1 (08/16/2011): A user may optionally specify the root folder for the unparsed header files.  
     if (CommandlineProcessing::isOptionWithParameter(argv, "-rose:", "(unparseHeaderFilesRootFolder)", stringParameter, true) == true) {
         //Although it is specified per file, it should be the same for the whole project.         
         get_project() -> set_unparseHeaderFilesRootFolder(stringParameter);
     }
     
  //
  // skip_commentsAndDirectives option: if analysis that does not use comments or CPP directives is required
  // then this option can improve the performance of the compilation.
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_commentsAndDirectives)",true) == true )
        {
       // printf ("option -rose:skip_commentsAndDirectives found \n");
          set_skip_commentsAndDirectives(true);

       // If we are skipping comments then we should not be collecting all comments (does not make sense)
          ROSE_ASSERT(get_collectAllCommentsAndDirectives() == false);
       // set_collectAllCommentsAndDirectives(false);
        }

  // DQ (8/16/2008): parse binary executable file format only (some uses of ROSE may only do analysis of
  // the binary executable file format and not the instructions).  This is also useful for testing.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(read_executable_file_format_only)",true) == true )
        {
       // printf ("option -rose:read_executable_file_format_only found \n");
          set_read_executable_file_format_only(true);
        }

  // DQ (11/11/2008): parse binary executable file format only and add attributes to the symbols so that
  // they will not be output in the generation of DOT files.  They will still be present for all other
  // forms of analysis.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(visualize_executable_file_format_skip_symbols)",true) == true )
        {
       // printf ("option -rose:visualize_executable_file_format_skip_symbols found \n");
          set_visualize_executable_file_format_skip_symbols(true);
        }

  // DQ (11/11/2008): parse binary executable file format only and add attributes to the symbols and most
  // other binary file format IR nodes so that they will not be output in the generation of DOT files.
  // They will still be present for all other forms of analysis.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(visualize_dwarf_only)",true) == true )
        {
       // printf ("option -rose:visualize_dwarf_only found \n");
          set_visualize_dwarf_only(true);
        }

  // DQ (1/10/2009): The C language ASM statements are providing significant trouble, they are
  // frequently machine specific and we are compiling then on architectures for which they were
  // not designed.  This option allows then to be read, constructed in the AST to support analysis
  // but not unparsed in the code given to the backend compiler, since this can fail. (See
  // test2007_20.C from Linux Kernel for an example).
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_unparse_asm_commands)",true) == true )
        {
       // printf ("option -rose:skip_unparse_asm_commands found \n");
          set_skip_unparse_asm_commands(true);
        }

  // RPM (12/29/2009): Disassembler aggressiveness.
     if (CommandlineProcessing::isOptionWithParameter(argv, "-rose:", "disassembler_search", stringParameter, true)) {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
         try {
             unsigned heuristics = get_disassemblerSearchHeuristics();
             heuristics = Disassembler::parse_switches(stringParameter, heuristics);
             set_disassemblerSearchHeuristics(heuristics);
         } catch(const Disassembler::Exception &e) {
             fprintf(stderr, "%s in \"-rose:disassembler_search\" switch\n", e.mesg.c_str());
             ROSE_ASSERT(!"error parsing -rose:disassembler_search");
         }
#else
         printf ("Binary analysis not supported in this distribution (turned off in this restricted distribution) \n");
         ROSE_ASSERT(false);
#endif
     }

  // RPM (1/4/2010): Partitioner function search methods
     if (CommandlineProcessing::isOptionWithParameter(argv, "-rose:", "partitioner_search", stringParameter, true)) {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
         try {
             unsigned heuristics = get_partitionerSearchHeuristics();
             heuristics = Partitioner::parse_switches(stringParameter, heuristics);
             set_partitionerSearchHeuristics(heuristics);
         } catch(const std::string &e) {
             fprintf(stderr, "%s in \"-rose:partitioner_search\" switch\n", e.c_str());
             ROSE_ASSERT(!"error parsing -rose:partitioner_search");
         }
#else
         printf ("Binary analysis not supported in this distribution (turned off in this restricted distribution) \n");
         ROSE_ASSERT(false);
#endif
     }

  // RPM (6/9/2010): Partitioner configuration
     if (CommandlineProcessing::isOptionWithParameter(argv, "-rose:", "partitioner_config", stringParameter, true)) {
         set_partitionerConfigurationFileName(stringParameter);
     }


  // DQ (9/26/2011): Adding options to support internal debugging of ROSE based tools and language support.
  // ****************
  // DEBUGING SUPPORT
  // ****************
  //
  // Support for detecting dangling pointers
  //     This is the first level of this sort of support. This level will be fooled by any reuse of 
  //     the memory (from new allocations) previously deleted. A later leve of this option will disable
  //     reuse of previously deleted memory for IR nodes; but it will consume more memory for translators
  //     that delete a lot of IR nodes as part of there functionality.  I expect this to only seriously
  //     make a difference for the AST merge operations which allocate and delete large parts of ASTs
  //     frequently.
  //     Note: this detects only dangling pointers to ROSE IR nodes, nothing more.
  //
#if 0
  // Test ROSE using default level 2 (error that will cause assertion) when not specified via the command line.
     set_detect_dangling_pointers(2);
#endif
     int integerDebugOption = 0;
     if ( CommandlineProcessing::isOptionWithParameter(argv,"-rose:","detect_dangling_pointers",integerDebugOption,true) == true )
        {
       // If this was set and a parameter was not specified or the value set to zero, then let the value be 1.
       // I can't seem to detect if the option was specified without a parameter.
          if (integerDebugOption == 0)
             {
               integerDebugOption = 1;
               if ( SgProject::get_verbose() >= 1 )
                    printf ("option -rose:detect_dangling_pointers found but value not specified or set to 0 default (reset integerDebugOption = %d) \n",integerDebugOption);
             }
          else
             {
               if ( SgProject::get_verbose() >= 1 )
                    printf ("option -rose:detect_dangling_pointers found with level (integerDebugOption = %d) \n",integerDebugOption);
             }

       // Set the level of the debugging to support.
          set_detect_dangling_pointers(integerDebugOption);
        }

  //
  // internal testing option (for internal use only, these may disappear at some point)
  //
     int integerOption = 0;
     if ( CommandlineProcessing::isOptionWithParameter(argv,"-rose:","test",integerOption,true) == true )
        {
       // printf ("option -rose:test %d found \n",integerOption);
          p_testingLevel = integerOption;
          switch (integerOption)
             {
               case 0 :
                 // transparent mode (does nothing)
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_disable_edg_backend  = true; // This variable should be called frontend NOT backend???
                    p_skip_transformation  = true;
                    p_skip_unparse         = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 1 :
                 // run the KCC front end only (can't unparse or compile output)
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_new_frontend         = true;
                    p_KCC_frontend         = true;
                    p_skip_transformation  = true;
                    p_skip_unparse         = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 2 :
                 // run the newer version of EDG (compiled separately from SAGE) "edgFrontEnd"
                 // (can't unparse or compile output)
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_new_frontend         = true;
                    p_skip_transformation  = true;
                    p_skip_unparse         = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 3 :
                 // run internal (older) version of edg front end (compiled with SAGE)
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_skip_transformation  = true;
                    p_skip_unparse         = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 4 :
                 // all of 3 (above) plus parse into SAGE program tree
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_skip_transformation  = true;
                    p_skip_unparse         = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 5 :
                 // all of 4 (above) plus unparse to generate (untransformed source code)
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_skip_transformation  = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 6 :
                 // all of 4 (above) plus compile generated source code
                 // p_skip_buildHigherLevelGrammars  = true;
                    p_skip_transformation  = true;
                    break;
               case 7 :
                 // all of 5 (above) plus parse into higher level grammars before unparsing
                    p_skip_transformation  = true;
                    p_skipfinalCompileStep = true;
                    break;
               case 8 :
               // all of 7 (above) plus compile resulting unparsed code (without transformations)
                    p_skip_transformation  = true;
                    break;
               case 9 :
               // all of 8 (above) plus run transformations before unparsing (do everything)
                    break;
               default:
                 // default mode is an error
                    printf ("Default reached in processing -rose:test # option (use 0-6, input option = %d) \n",integerOption);
                    ROSE_ABORT();
                    break;
             }
        }

#if 0
     printf ("Exiting after test of test option! \n");
     exit (0);
#endif

  // printf ("After processing -rose:test # option argc = %d \n",argc);

  //
  // new_unparser option
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skipfinalCompileStep)",true) == true )
        {
          if (get_verbose()>0) // Liao, 8/29/2008, Only show it in verbose mode.
               printf ("option -rose:skipfinalCompileStep found \n");
          set_skipfinalCompileStep(true);
        }

  //
  // Standard compiler options (allows alternative -E option to just run CPP)
  //
     if ( CommandlineProcessing::isOption(argv,"-","(E)",true) == true )
        {
       // printf ("/* option -E found (just run backend compiler with -E to call CPP) */ \n");
          p_useBackendOnly = true;
       // p_skip_buildHigherLevelGrammars  = true;
          p_disable_edg_backend  = true; // This variable should be called frontend NOT backend???
          p_skip_transformation  = true;
          p_skip_unparse         = true;
          p_skipfinalCompileStep = false;

       // DQ (8/22/2009): Verify that this was set when the command line was processed at the SgProject level.
          SgProject* project = this->get_project();
          ROSE_ASSERT(project != NULL);
          ROSE_ASSERT(project->get_C_PreprocessorOnly() == true);
        }

  //
  // Standard compiler options (allows alternative -H option to just output header file info)
  //
     if ( CommandlineProcessing::isOption(argv,"-","(H)",true) == true )
        {
       // printf ("option -H found (just run backend compiler with -E to call CPP) \n");
          p_useBackendOnly = true;
       // p_skip_buildHigherLevelGrammars  = true;
          p_disable_edg_backend  = true; // This variable should be called frontend NOT backend???
          p_skip_transformation  = true;
          p_skip_unparse         = true;
          p_skipfinalCompileStep = false;
        }

  //
  // negative_test option: allows passing all tests to be treated as an error!
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(negative_test)",true) == true )
        {
          printf ("option -rose:negative_test found \n");
          set_negative_test(true);
        }

#if 1
  //
  // We have processed all rose supported command line options.
  // Anything left in argv now should be a typo or option for either EDG or the C or C++ compiler.
  //
     if ( get_verbose() > 1 )
        {
          cout << "The remaining non-rose options will be passed onto either the parser/front-end or the backend (vendor) the compiler: " << endl;
          for (unsigned int i = 1; i < argv.size(); i++)
                  {
                    cout << "  argv[" << i << "]= " << argv[i] << endl;
                  }
        }

  // debugging aid
  // display("SgFile::processRoseCommandLineOptions()");
#endif
   }

void
SgFile::stripRoseCommandLineOptions ( vector<string> & argv )
   {
  // Strip out the rose specific commandline options
  // the assume all other arguments are to be passed onto the C or C++ compiler

     int optionCount = 0;
  // int i = 0;

// #if ROSE_INTERNAL_DEBUG
#if 1
  // printf ("ROSE_DEBUG = %d \n",ROSE_DEBUG);
  // printf ("get_verbose() = %s value = %d \n",(get_verbose() > 1) ? "true" : "false",get_verbose());

     if ( (ROSE_DEBUG >= 1) || (SgProject::get_verbose() > 2 ))
        {
          printf ("In stripRoseCommandLineOptions (TOP): List ALL arguments: argc = %zu \n",argv.size());
          for (size_t i=0; i < argv.size(); i++)
             printf ("     argv[%zu] = %s \n",i,argv[i].c_str());
        }
#endif

  // Split out the ROSE options first

  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
     optionCount = sla(argv, "-"     , "($)", "(h|help)",1);
     optionCount = sla(argv, "-rose:", "($)", "(h|help)",1);
     optionCount = sla(argv, "-rose:", "($)", "(V|version)", 1);
  // optionCount = sla(argv, "-rose:", "($)", "(v|verbose)",1);
     int integerOption = 0;
     optionCount = sla(argv, "-rose:", "($)^", "(v|verbose)", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)^", "(upc_threads)", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)", "(C|C_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(UPC|UPC_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(OpenMP|openmp)",1);
     optionCount = sla(argv, "-rose:", "($)", "(openmp:parse_only|OpenMP:parse_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(openmp:ast_only|OpenMP:ast_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(openmp:lowering|OpenMP:lowering)",1);
     optionCount = sla(argv, "-rose:", "($)", "(C99|C99_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(Cxx|Cxx_only)",1);

     optionCount = sla(argv, "-rose:", "($)", "(output_warnings)",1);
     optionCount = sla(argv, "-rose:", "($)", "(cray_pointer_support)",1);

     optionCount = sla(argv, "-rose:", "($)", "(output_parser_actions)",1);
     optionCount = sla(argv, "-rose:", "($)", "(output_tokens)",1);
     optionCount = sla(argv, "-rose:", "($)", "(exit_after_parser)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_syntax_check)",1);
     optionCount = sla(argv, "-rose:", "($)", "(relax_syntax_check)",1);

  // DQ (10/12/2010): Added support for Java
     optionCount = sla(argv, "-rose:", "($)", "(j|J|java|Java)",1);

  // DQ (8/11/2007): Support for Fortran and its different flavors
     optionCount = sla(argv, "-rose:", "($)", "(f|F|Fortran)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f77|F77|Fortran77)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f90|F90|Fortran90)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f95|F95|Fortran95)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f2003|F2003|Fortran2003)",1);
     optionCount = sla(argv, "-rose:", "($)", "(caf|CAF|CoArrayFortran)",1);

  // DQ (8/27/2007):Support for Fortran language output format

     optionCount = sla(argv, "-rose:", "($)", "(fixedOutput|fixedFormatOutput)",1);
     optionCount = sla(argv, "-rose:", "($)", "(freeOutput|freeFormatOutput)",1);

     optionCount = sla(argv, "-rose:", "($)", "(compileFixed|backendCompileFixedFormat)",1);
     optionCount = sla(argv, "-rose:", "($)", "(compileFree|backendCompileFreeFormat)",1);

     optionCount = sla(argv, "-rose:", "($)", "(C_output_language|Cxx_output_language|Fortran_output_language|Promela_output_language|PHP_output_language)",1);

  // DQ (5/19/2005): The output file name is constructed from the input source name (as I recall)
  // optionCount = sla(argv, "-rose:", "($)^", "(o|output)", &p_unparse_output_filename ,1);

     optionCount = sla(argv, "-rose:", "($)", "(skip_translation_from_edg_ast_to_rose_ast)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_transformation)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_unparse)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_includes)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_line_directives)",1);

     optionCount = sla(argv, "-rose:", "($)", "(unparse_instruction_addresses)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_raw_memory_contents)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_binary_file_format)",1);

     optionCount = sla(argv, "-rose:", "($)", "(collectAllCommentsAndDirectives)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparseHeaderFiles)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_commentsAndDirectives)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skipfinalCompileStep)",1);
     optionCount = sla(argv, "-rose:", "($)", "(prelink)",1);
     optionCount = sla(argv, "-"     , "($)", "(ansi)",1);
     optionCount = sla(argv, "-rose:", "($)", "(markGeneratedFiles)",1);
     optionCount = sla(argv, "-rose:", "($)", "(wave)",1);
     optionCount = sla(argv, "-rose:", "($)", "(negative_test)",1);
     integerOption = 0;
     optionCount = sla(argv, "-rose:", "($)^", "(embedColorCodesInGeneratedCode)", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)^", "(generateSourcePositionCodes)", &integerOption, 1);

     optionCount = sla(argv, "-rose:", "($)", "(outputGrammarTreeFiles)",1);
     optionCount = sla(argv, "-rose:", "($)", "(outputGrammarTreeFilesForHeaderFiles)",1);
     optionCount = sla(argv, "-rose:", "($)", "(outputGrammarTreeFilesForEDG)",1);
     optionCount = sla(argv, "-rose:", "($)", "(new_unparser)",1);
     optionCount = sla(argv, "-rose:", "($)", "(negative_test)",1);
     optionCount = sla(argv, "-rose:", "($)", "(strict)",1);
     integerOption = 0;
     optionCount = sla(argv, "-rose:", "($)^", "test", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)", "(skipfinalCompileStep)",1);
     optionCount = sla(argv, "-rose:", "($)", "(prelink)",1);

     char* unparseHeaderFilesRootFolderOption = NULL;
     optionCount = sla(argv, "-rose:", "($)^", "(unparseHeaderFilesRootFolder)", unparseHeaderFilesRootFolderOption, 1);

     char* templateInstationationOption = NULL;
     optionCount = sla(argv, "-rose:", "($)^", "(instantiation)",templateInstationationOption,1);

  // DQ (6/17/2005): Added support for AST merging (sharing common parts of the AST most often represented in common header files of a project)
     optionCount = sla(argv, "-rose:", "($)", "(astMerge)",1);
     char* filename = NULL;
     optionCount = sla(argv, "-rose:", "($)^", "(astMergeCommandFile)",filename,1);
     optionCount = sla(argv, "-rose:", "($)^", "(compilationPerformanceFile)",filename,1);

         //AS(093007) Remove paramaters relating to excluding and include comments and directives
     optionCount = sla(argv, "-rose:", "($)^", "(excludeCommentsAndDirectives)", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)^", "(excludeCommentsAndDirectivesFrom)", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)^", "(includeCommentsAndDirectives)", &integerOption, 1);
     optionCount = sla(argv, "-rose:", "($)^", "(includeCommentsAndDirectivesFrom)", &integerOption, 1);

  // DQ (12/8/2007): Strip use of the "-rose:output <filename> option.
     optionCount = sla(argv, "-rose:", "($)^", "(o|output)", filename, 1);

  // Liao 2/26/2009: strip use of -rose:excludePath <pathname> , -rose:excludeFile <filename> ,etc
  // which are introduced in Compass::commandLineProcessing()
     char* pathname = NULL;
     optionCount = sla(argv, "-rose:", "($)^", "(excludePath)", pathname,1);
     optionCount = sla(argv, "-rose:", "($)^", "(excludeFile)", filename,1);
     optionCount = sla(argv, "-rose:", "($)^", "(includeFile)", filename,1);

  // DQ (8/16/2008): parse binary executable file format only (some uses of ROSE may only do analysis of
  // the binary executable file format and not the instructions).  This is also useful for testing.
     optionCount = sla(argv, "-rose:", "($)", "(read_executable_file_format_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(visualize_executable_file_format_skip_symbols)",1);
     optionCount = sla(argv, "-rose:", "($)", "(visualize_dwarf_only)",1);

  // DQ (10/18/2009): Sometimes we need to skip the parsing of the file format
     optionCount = sla(argv, "-rose:", "($)", "(read_instructions_only)",1);

     optionCount = sla(argv, "-rose:", "($)", "(skip_unparse_asm_commands)",1);

  // DQ (8/26/2007): Disassembly support from segments (true) instead of sections (false, default).
     optionCount = sla(argv, "-rose:", "($)", "(aggressive)",1);

  // DQ (2/5/2009): Remove use of "-rose:binary" to prevent it being passed on.
     optionCount = sla(argv, "-rose:", "($)", "(binary|binary_only)",1);

  // DQ (10/3/2010): Adding support for having CPP directives explicitly in the AST (as IR nodes instead of handled similar to comments).
     optionCount = sla(argv, "-rose:", "($)^", "(addCppDirectivesToAST)",filename,1);

  // DQ (9/26/2011): Added support for different levesl of detection for dangling pointers.
     optionCount = sla(argv, "-rose:", "($)^", "detect_dangling_pointers",&integerOption,1);

#if 1
     if ( (ROSE_DEBUG >= 1) || (SgProject::get_verbose() > 2 ))
        {
          printf ("In stripRoseCommandLineOptions (BOTTOM): List ALL arguments: argc = %zu \n",argv.size());
          for (size_t i=0; i < argv.size(); i++)
             printf ("     argv[%zu] = %s \n",i,argv[i].c_str());
        }
#endif
   }

void
SgFile::stripEdgCommandLineOptions ( vector<string> & argv )
   {
  // Strip out the EDG specific commandline options the assume all
  // other arguments are to be passed onto the C or C++ compiler

#if 0
     if ( (ROSE_DEBUG >= 0) || (get_verbose() > 1) )
        {
          Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
          printf ("In SgFile::stripEdgCommandLineOptions: argv = \n%s \n",StringUtility::listToString(l).c_str());
        }
#endif

  // Split out the EDG options (ignore the returned Rose_STL_Container<string> object)
     CommandlineProcessing::removeArgs (argv,"-edg:");
     CommandlineProcessing::removeArgs (argv,"--edg:");
     CommandlineProcessing::removeArgsWithParameters (argv,"-edg_parameter:");
     CommandlineProcessing::removeArgsWithParameters (argv,"--edg_parameter:");

  // DQ (2/20/2010): Remove this option when building the command line for the vendor compiler.
     int optionCount = 0;
     optionCount = sla(argv, "--edg:", "($)", "(no_warnings)",1);

#if 0
     Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("In SgFile::stripEdgCommandLineOptions: argv = \n%s \n",StringUtility::listToString(l).c_str());
#endif
   }

void
SgFile::stripFortranCommandLineOptions ( vector<string> & argv )
   {
  // Strip out the OFP specific commandline options the assume all
  // other arguments are to be passed onto the vendor Fortran compiler

#if 0
     if ( (ROSE_DEBUG >= 0) || (get_verbose() > 1) )
        {
          Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
          printf ("In SgFile::stripFortranCommandLineOptions: argv = \n%s \n",StringUtility::listToString(l).c_str());
        }
#endif

  // No OFP options that we currently filter out.

#if 0
     Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("In SgFile::stripFortranCommandLineOptions: argv = \n%s \n",StringUtility::listToString(l).c_str());
#endif
   }


void
SgFile::stripJavaCommandLineOptions ( vector<string> & argv )
   {
  // Strip out the ECJ specific command line options then assume all
  // other arguments are to be passed onto the vendor Java compiler
     CommandlineProcessing::removeArgs (argv,"-ecj:");
     CommandlineProcessing::removeArgs (argv,"--ecj:");
     CommandlineProcessing::removeArgsWithParameters (argv,"-ecj_parameter:");
     CommandlineProcessing::removeArgsWithParameters (argv,"--ecj_parameter:");

     string javaRosePrefix = "-rose:java:";
     // if destination is not provided, insert rose's default one for java.
     if (!CommandlineProcessing::isOption(argv, javaRosePrefix, "d", false)) {
         // Put *.class files generated from calling that backend compiler (javac) and the ROSE generated code
         // into the current directory.  This makes the semantics for Java similar to all other languages in
         // ROSE but it is different from the default behavior for "javac".  So it is not clear if we really
         // want this semantics, but another reason for this desired behavior is that it will avoid having
         // the source-to-source generated code from overwriting the input source code.
         //
         // Note: we don't use the get_Java_destdir here because we don't have a handle on the SgProject
         // Check get_Java_destdir implementation in Support.code
         argv.push_back("-d");
         argv.push_back(ROSE::getWorkingDirectory());
     }

     // Need to rewrite javac options prefix before handing them to the backend.
     Rose_STL_Container<string> rose_java_options = CommandlineProcessing::generateOptionWithNameParameterList(argv, javaRosePrefix, "-");
     for (Rose_STL_Container<string>::iterator i = rose_java_options.begin(); i != rose_java_options.end(); ++i)
       {
         argv.push_back(*i);
       }
   }

void
SgFile::processBackendSpecificCommandLineOptions ( const vector<string>& argvOrig )
   {
  // DQ (6/21/2005): This function processes commandline options that are specific to the backend
  // but which the front-end or ROSE should know about.  Examples include template options that
  // would influence how ROSE instantiates or outputs templates in the code generation phase.

  // This function leaves all commandline options in place (for use by the backend)

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

  vector<string> argv = argvOrig;

  //
  // -ansi option (fairly general option name across a number of compilers, I think)
  //
     if ( CommandlineProcessing::isOption(argv,"-","ansi",true) == true )
        {
          set_strict_language_handling(true);
        }

  //
  // -fno-implicit-templates option
  //
     p_no_implicit_templates = false;
     if ( CommandlineProcessing::isOption(argv,"-f","no-implicit-templates",true) == true )
        {
       // don't instantiate any inline templates
          printf ("ROSE sees use of -fno-implicit-templates option for use by g++ \n");

       // printf ("ERROR: This g++ specific option is not yet supported \n");
       // ROSE_ASSERT(false);

          p_no_implicit_templates = true;
        }

  //
  // -fno-implicit-inline-templates option
  //
     p_no_implicit_inline_templates = false;
     if ( CommandlineProcessing::isOption(argv,"-f","no-implicit-inline-templates",true) == true )
        {
       // don't instantiate any inline templates
          printf ("ROSE sees use of -fno-implicit-inline-templates option for use by g++ \n");
          p_no_implicit_inline_templates = true;
        }
   }

void SgFile::build_CLANG_CommandLine ( vector<string> & inputCommandLine, vector<string> & argv, int fileNameIndex ) {
    // It filters Rose and Edg specific parameters and fixes the pathes.

    std::vector<std::string> inc_dirs_list;
    std::vector<std::string> define_list;
    std::string input_file;

    for (size_t i = 0; i < argv.size(); i++) {
        std::string current_arg(argv[i]);
        if (current_arg.find("-I") == 0) {
            if (current_arg.length() > 2) {
                inc_dirs_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argv.size())
                    inc_dirs_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-D") == 0) {
            if (current_arg.length() > 2) {
                define_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argv.size())
                    define_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-c") == 0) {}
        else if (current_arg.find("-o") == 0) {
            if (current_arg.length() == 2) {
                i++;
                if (i >= argv.size()) break;
            }
        }
        else if (current_arg.find("-rose") == 0) {}
        else {
            input_file = current_arg;
        }
    }

    std::vector<std::string>::iterator it_str;
    for (it_str = define_list.begin(); it_str != define_list.end(); it_str++)
        inputCommandLine.push_back("-D" + *it_str);
    for (it_str = inc_dirs_list.begin(); it_str != inc_dirs_list.end(); it_str++)
        inputCommandLine.push_back("-I" + StringUtility::getAbsolutePathFromRelativePath(*it_str));

    std::string input_file_path = StringUtility::getPathFromFileName(input_file);
    input_file = StringUtility::stripPathFromFileName(input_file);
    if (input_file_path == "" ) input_file_path = "./";
    input_file_path = StringUtility::getAbsolutePathFromRelativePath(input_file_path);
    input_file = input_file_path + "/" + input_file;
    inputCommandLine.push_back(input_file);

}

void
SgFile::build_EDG_CommandLine ( vector<string> & inputCommandLine, vector<string> & argv, int fileNameIndex )
   {
  // This function builds the command line input for the edg_main (the EDG C++ Front-End)
  // There are numerous options that must be set for different architectures. We can find
  // a more general setup of these options within something like Duct-Tape. In this function
  // argv and argc are the parameters from the user's commandline, and inputCommandLine and
  // numberOfCommandLineArguments are parameters for the new command line that will be build
  // for EDG.

  // printf ("##### Inside of SgFile::build_EDG_CommandLine file = %s \n",get_file_info()->get_filenameString().c_str());
  // printf ("##### Inside of SgFile::build_EDG_CommandLine file = %s = %s \n",get_file_info()->get_filenameString().c_str(),get_sourceFileNameWithPath().c_str());
     ROSE_ASSERT(get_file_info()->get_filenameString() == get_sourceFileNameWithPath());

  // ROSE_ASSERT (p_useBackendOnly == false);

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

  // printf ("Inside of SgFile::build_EDG_CommandLine(): fileNameIndex = %d \n",fileNameIndex);

#if !defined(CXX_SPEC_DEF)
  // Output an error and exit
     cout << "ERROR: The preprocessor definition CXX_SPEC_DEF should have been defined by the configure process!!" << endl;
     ROSE_ABORT();
#endif

     const char* configDefsArray[]          = CXX_SPEC_DEF;
     const char* Cxx_ConfigIncludeDirsRaw[] = CXX_INCLUDE_STRING;
     const char* C_ConfigIncludeDirsRaw[]   = C_INCLUDE_STRING;
     Rose_STL_Container<string> configDefs(configDefsArray, configDefsArray + sizeof(configDefsArray) / sizeof(*configDefsArray));
     Rose_STL_Container<string> Cxx_ConfigIncludeDirs(Cxx_ConfigIncludeDirsRaw, Cxx_ConfigIncludeDirsRaw + sizeof(Cxx_ConfigIncludeDirsRaw) / sizeof(const char*));
     Rose_STL_Container<string> C_ConfigIncludeDirs(C_ConfigIncludeDirsRaw, C_ConfigIncludeDirsRaw + sizeof(C_ConfigIncludeDirsRaw) / sizeof(const char*));

  // const char* boostPath[] = ROSE_BOOST_PATH;

  // Removed reference to __restrict__ so it could be placed into the preinclude vendor specific header file for ROSE.
  // DQ (9/10/2004): Attept to add support for restrict (but I think this just sets it to true, using "-Dxxx=" works)
  // const string roseSpecificDefs    = "-DUSE_RESTRICT_POINTERS_IN_ROSE_TRANSFORMATIONS -DUSE_ROSE -D__restrict__=";
     vector<string> roseSpecificDefs;

  // Communicate that ROSE transformation can use the restrict keyword.
     roseSpecificDefs.push_back("-DUSE_RESTRICT_POINTERS_IN_ROSE_TRANSFORMATIONS");

  // Communicate to the generated program that we are using ROSE (in case there are specific options that the user wants to to invoke.
     roseSpecificDefs.push_back("-DUSE_ROSE");

#ifdef ROSE_USE_NEW_EDG_INTERFACE
  // Allow in internal indicator that EDG version 3.10 or 4.0 (or greater) is in use.
     roseSpecificDefs.push_back("-DROSE_USE_NEW_EDG_INTERFACE");
#endif

     ROSE_ASSERT(configDefs.empty() == false);
     ROSE_ASSERT(Cxx_ConfigIncludeDirs.empty() == false);
     ROSE_ASSERT(C_ConfigIncludeDirs.empty() == false);

  // printf ("configDefsString = %s \n",configDefsString);
#if 0
     printf ("Cxx_ConfigIncludeString = %s \n",Cxx_ConfigIncludeString.c_str());
     printf ("C_ConfigIncludeString   = %s \n",C_ConfigIncludeString.c_str());
#endif

  // JJW (12/11/2008):  add --edg_base_dir as a new ROSE-set flag
     vector<string> commandLine;

#if 1
  // DQ (11/1/2011): This is not enough to support C++ code (e.g. "limits" header file).
#ifdef ROSE_USE_NEW_EDG_INTERFACE

  // Note that the new EDG/Sage interface does not require a generated set of header files specific to ROSE.
     commandLine.push_back("--edg_base_dir");

  // DQ (12/29/2008): Added support for EDG version 4.0 (constains design changes that break a number of things in the pre-version 4.0 work)
#ifdef ROSE_USE_EDG_VERSION_4
  // DQ (2/1/2010): I think this needs to reference the source tree (to pickup src/frontend/CxxFrontend/EDG/EDG_4.0/lib/predefined_macros.txt).
  // DQ (12/21/2009): The locaion of the EDG directory has been changed now that it is a submodule in our git repository.
  // commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG_4.0/lib", "share"));
  // commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG/EDG_4.0/lib", "share"));

  // DQ (11/1/2011): Fix to use EDG 4.3
  // commandLine.push_back(findRoseSupportPathFromSource("src/frontend/CxxFrontend/EDG/EDG_4.0/lib", "share"));
     commandLine.push_back(findRoseSupportPathFromSource("src/frontend/CxxFrontend/EDG/EDG_4.3/lib", "share"));
#else
  // DQ (2/1/2010): I think this needs to reference the source tree (to pickup src/frontend/CxxFrontend/EDG/EDG_4.0/lib/predefined_macros.txt).
  // DQ (12/21/2009): The locaion of the EDG directory has been changed now that it is a submodule in our git repository.
  // commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG_3.10/lib", "share"));
     commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG/EDG_3.10/lib", "share"));
  // commandLine.push_back(findRoseSupportPathFromSource("src/frontend/CxxFrontend/EDG/EDG_3.10/lib", "share"));
#endif
#endif
#endif

  // display("Called from SgFile::build_EDG_CommandLine");

     SgProject* project = isSgProject(this->get_parent());
     ROSE_ASSERT (project != NULL);

     //AS(063006) Changed implementation so that real paths can be found later
     vector<string> includePaths;

  // skip the 0th entry since this is just the name of the program (e.g. rose)
     for (unsigned int i=1; i < argv.size(); i++)
        {
       // most options appear as -<option>
       // have to process +w2 (warnings option) on some compilers so include +<option>
          if ( argv[i].size() >= 2 && (argv[i][0] == '-') && (argv[i][1] == 'I') )
             {
            // int length = strlen(argv[i]);
            // printf ("Look for include path:  argv[%d] = %s length = %d \n",i,argv[i],length);

            // AS: Did changes to get absolute path
               std::string includeDirectorySpecifier =  argv[i].substr(2);
               includeDirectorySpecifier = StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
               includePaths.push_back(includeDirectorySpecifier);
             }
        }

#if 0
  // This functionality has been moved to before source name extraction since the
  // -isystem dir will be extracted as a file name and treated as a source file name
  // and the -isystem will not have an option.

  // AS(02/24/06) Add support for the gcc "-isystem" option (this added a specified directory
  // to the start of the system include path).  This maps to the "--sys_include" in EDG.
     string isystem_string_target = "-isystem";
     for (unsigned int i=1; i < argv.size(); i++)
        {
       // AS (070306) Handle g++ --include directives
          std::string stack_arg(argv[i]);
       // std::cout << "stack arg is: " << stack_arg << std::endl;
          if( stack_arg.find(isystem_string_target) <= 2){
              i++;
              ROSE_ASSERT(i<argv.size());
              std::string currentArgument(argv[i]);
              // std::cout << "Current argument " << currentArgument << std::endl;

              currentArgument = StringUtility::getAbsolutePathFromRelativePath(currentArgument);
              commandLine.push_back("--sys_include");
              commandLine.push_back(currentArgument);
          }
     }
#else
  // DQ (1/13/2009): The preincludeDirectoryList was built if the -isystem <dir> option was used

#ifndef ROSE_USE_NEW_EDG_INTERFACE
  // DQ (11/3/2011): This is only required for the older version of EDG (currently still the default).
  // AS (2/22/08): GCC looks for system headers in '-I' first. We need to support this.
  // PC (10/20/2009): This code was moved from SgProject as it is file-specific (required by AST merge)
     for (vector<string>::iterator i = includePaths.begin(); i != includePaths.end(); ++i)
        {
          commandLine.push_back("--sys_include");
          commandLine.push_back(*i);
        }
#endif

     if ( SgProject::get_verbose() >= 1 )
          printf ("project->get_preincludeDirectoryList().size() = %zu \n",project->get_preincludeDirectoryList().size());

  // This is the list of directories that have been referenced as "-isystem <directory>" on the original command line to the ROSE 
  // translator.  We translate these to "-sys_include <directory>" options to pass to EDG (since that is how EDG understands them).
     for (SgStringList::iterator i = project->get_preincludeDirectoryList().begin(); i != project->get_preincludeDirectoryList().end(); i++)
        {
       // Build the preinclude directory list
          if ( SgProject::get_verbose() >= 1 )
               printf ("Building commandline: --sys_include %s \n",(*i).c_str());

          commandLine.push_back("--sys_include");
          commandLine.push_back(*i);
        }
#endif

     commandLine.insert(commandLine.end(), configDefs.begin(), configDefs.end());

  // DQ (1/13/2009): The preincludeFileList was built if the -include <file> option was used
  // George Vulov (12/8/2010) Include the file rose_edg_required_macros_and_functions.h first, then the other preincludes
     for (SgStringList::iterator i = project->get_preincludeFileList().begin(); i != project->get_preincludeFileList().end(); i++)
        {
       // Build the preinclude file list
          ROSE_ASSERT(project->get_preincludeFileList().empty() == false);

     //  printf ("Building commandline: --preinclude %s \n",(*i).c_str());
          commandLine.push_back("--preinclude");
          commandLine.push_back(*i);
        }

  // DQ (12/2/2006): Both GNU and EDG determine the language mode from the source file name extension.
  // In ROSE we also require that C files be explicitly specified to use the C language mode. Thus
  // C++ source files will be treated as C++ even if the C language rules are specified, however they
  // are restricted to the C subset of C++.
  // bool treatAsCSourceFile = ((get_C_only() == true || get_C99_only() == true) && (get_sourceFileUsesCppFileExtension() == false));
  // if (treatAsCSourceFile == true)

  // Find the C++ sys include path for the rose_edg_required_macros_and_functions.h
     vector<string> roseHeaderDirCPP(1, "--sys_include");

  // This includes a generated header file that defines the __builtin functions and a number
  // of predefined macros obtained from the backend compiler.  The new EDG/Sage interface
  // does not require this function and I think that the file is not generated in this case
  // which is why there is a test for it's existence to see if it should be include.  I would
  // rather see a more direct test.
     for (Rose_STL_Container<string>::iterator i = Cxx_ConfigIncludeDirs.begin(); i != Cxx_ConfigIncludeDirs.end(); i++)
        {
          string file = (*i) + "/rose_edg_required_macros_and_functions.h";
          FILE* testIfFileExist = fopen(file.c_str(),"r");
          if (testIfFileExist)
          {
            roseHeaderDirCPP.push_back(*i);
            fclose(testIfFileExist);
            break;
          }
        }

  // Find the C sys include path for the rose_edg_required_macros_and_functions.h (see comment above for --sys_include use in CPP).
     vector<string> roseHeaderDirC(1, "--sys_include");

     for (Rose_STL_Container<string>::iterator i = C_ConfigIncludeDirs.begin(); i != C_ConfigIncludeDirs.end(); i++)
        {
          string file = (*i) + "/rose_edg_required_macros_and_functions.h";
          FILE* testIfFileExist = fopen(file.c_str(),"r");
          // std::cout << file << std::endl;
          if (testIfFileExist)
          {
            roseHeaderDirC.push_back(*i);
            fclose(testIfFileExist);
            break;
          }
        }


  // TV (05/07/2010): OpenCL and CUDA mode (Caution: we may need both C++ language mode and Cuda)
     bool enable_cuda   = CommandlineProcessing::isOption(argv,"-","cuda",true) || get_Cuda_only();
     bool enable_opencl = CommandlineProcessing::isOption(argv,"-","opencl",true) || get_OpenCL_only();

     string header_path = findRoseSupportPathFromBuild("include-staging", "include-staging");
     
     if (enable_cuda || enable_opencl) {
        SageSupport::Cmdline::makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
        if (enable_cuda && !enable_opencl) {
          commandLine.push_back("--preinclude");
          commandLine.push_back(header_path + "/cuda_HEADERS/preinclude-cuda.h");
        }
        else if (enable_opencl && !enable_cuda) {
          commandLine.push_back("--preinclude");
          commandLine.push_back(header_path + "/opencl_HEADERS/preinclude-opencl.h");
        }
        else {
                printf ("Error: CUDA and OpenCL are mutually exclusive.\n");
                ROSE_ASSERT(false);
        }
     }
     else {
          if (get_C_only() == true || get_C99_only() == true)
             {
            // AS(02/21/07) Add support for the gcc 'nostdinc' and 'nostdinc++' options
            // DQ (11/29/2006): if required turn on the use of the __cplusplus macro
            // if (get_requires_cplusplus_macro() == true)
               if (get_sourceFileUsesCppFileExtension() == true)
                  {
                 // The value here should be 1 to match that of GNU gcc (the C++ standard requires this to be "199711L")
                 // initString += " -D__cplusplus=0 ";
                    commandLine.push_back("-D__cplusplus=1");
                    if ( CommandlineProcessing::isOption(argv,"-","nostdinc",false) == true )
                       {
                         commandLine.insert(commandLine.end(), roseHeaderDirC.begin(), roseHeaderDirC.end());
                      // no standard includes when -nostdinc is specified
                       }
                      else
                       {
                         if ( CommandlineProcessing::isOption(argv,"-","nostdinc++",false) == true )
                            {
                              commandLine.insert(commandLine.end(), roseHeaderDirCPP.begin(), roseHeaderDirCPP.end());
                              SageSupport::Cmdline::makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
                            }
                           else
                            {
                              SageSupport::Cmdline::makeSysIncludeList(Cxx_ConfigIncludeDirs, commandLine);
                            }
                       }

                 // DQ (11/29/2006): Specify C++ mode for handling in rose_edg_required_macros_and_functions.h
                    commandLine.push_back("-DROSE_LANGUAGE_MODE=1");
                  }
                 else
                  {
                    if ( CommandlineProcessing::isOption(argv,"-","nostdinc",false) == true )
                       {
                         commandLine.insert(commandLine.end(), roseHeaderDirC.begin(), roseHeaderDirC.end());
                      // no standard includes when -nostdinc is specified
                       }
                      else
                       {
                         SageSupport::Cmdline::makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
                       }

                 // DQ (11/29/2006): Specify C mode for handling in rose_edg_required_macros_and_functions.h
                    commandLine.push_back("-DROSE_LANGUAGE_MODE=0");
                  }
             }
            else
             {
               if ( CommandlineProcessing::isOption(argv,"-","nostdinc",false) == true )
                  {
                    commandLine.insert(commandLine.end(), roseHeaderDirCPP.begin(), roseHeaderDirCPP.end());
                 // no standard includes when -nostdinc is specified
                  }
                 else
                  {
                    if ( CommandlineProcessing::isOption(argv,"-","nostdinc\\+\\+",false) == true ) // Option name is a RE
                       {
                         commandLine.insert(commandLine.end(), roseHeaderDirCPP.begin(), roseHeaderDirCPP.end());
                         SageSupport::Cmdline::makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
                       }
                      else
                       {
                         SageSupport::Cmdline::makeSysIncludeList(Cxx_ConfigIncludeDirs, commandLine);
                       }
                  }

            // DQ (11/29/2006): Specify C++ mode for handling in rose_edg_required_macros_and_functions.h
               commandLine.push_back("-DROSE_LANGUAGE_MODE=1");
             }
     }

     commandLine.insert(commandLine.end(), roseSpecificDefs.begin(), roseSpecificDefs.end());

  // DQ (9/17/2006): We should be able to build a version of this code which hands a std::string to StringUtility::splitStringIntoStrings()
  // Separate the string into substrings consistent with the structure of argv command line input
     inputCommandLine = commandLine;

  // DQ (11/1/2011): Do we need this for the new EDG 4.3 work?
  // #ifndef ROSE_USE_NEW_EDG_INTERFACE
     inputCommandLine.insert(inputCommandLine.begin(), "dummy_argv0_for_edg");
  // #endif
  // We only provide options to change the default values!

  // Handle option for use of ROSE as a C compiler instead of C++
  // some C code can not be compiled with a C++ compiler.
     if (get_C_only() == true)
        {
       // Add option to indicate use of C code (not C++) to EDG frontend
          inputCommandLine.push_back("--c");
        }

     if (get_C99_only() == true)
        {
       // Add option to indicate use of C code (not C++) to EDG frontend
          inputCommandLine.push_back("--c99");
        }

     if (get_strict_language_handling() == true)
        {
          inputCommandLine.push_back("--strict");
        }

  //
  // edg_new_frontend option
  //
     if ( CommandlineProcessing::isOption(argv,"-edg:","(new_frontend)",true) == true || (get_new_frontend() == true) )
        {
       // printf ("option -edg:new_frontend found \n");
          set_new_frontend(true);

       // if we use the new EDG frontend (not connected to SAGE) then we can't
       // generate C++ code so we don't want to call the C++ compiler
       // set_skipfinalCompileStep(true);
        }

  //
  // edg_KCC_frontend option
  //
     if ( CommandlineProcessing::isOption(argv,"-edg:","(KCC_frontend)",true) == true || (get_KCC_frontend() == true) )
        {
       // printf ("option -edg:KCC_frontend found \n");
          set_KCC_frontend(true);

       // if we use the new EDG frontend (not connected to SAGE) then we can't
       // generate C++ code so we don't want to call the C++ compiler
          set_skipfinalCompileStep(true);
        }

  //
  // edg_backend option
  //
     if ( CommandlineProcessing::isOption(argv,"-edg:","(disable_edg_backend)",true) == true || (get_disable_edg_backend() == true) )
        {
       // printf ("option -edg:disable_edg_backend found \n");
          set_disable_edg_backend(true);
        }

#if 0
  //
  // sage_backend option
  //
     if ( CommandlineProcessing::isOption(argv,"-edg:","(disable_edg_backend)",true) == true || (get_disable_edg_backend() == true) )
        {
       // printf ("option -edg:disable_edg_backend found \n");
          set_disable_edg_backend(true);
        }
#endif
#if 0
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of argc
     optionCount = sla(&argc, argv, "-sage:", "($)", "(disable_sage_backend)",1);
     if( optionCount > 0 || (get_disable_sage_backend() == true) == true )
        {
          printf ("option -sage:disable_sage_backend found \n");
          set_disable_sage_backend(true);
          inputCommandLine.push_back("--disable_sage_backend");

       // We we aren't going to process the code through the backend then there is nothing to compile
          set_skipfinalCompileStep(true);
        }

  // printf ("After processing -sage:disable_sage_backend option argc = %d \n",argc);
#endif

#if 0
  //
  // cp_backend option
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
     optionCount = sla(&argc, argv, "-sage:", "($)", "(enable_cp_backend)",1);
     if ( optionCount > 0 || (get_enable_cp_backend() == true) )
        {
       // printf ("option -sage:enable_cp_backend found \n");
          set_enable_cp_backend(true);
          inputCommandLine.push_back("--enable_cp_backend");
        }

  // printf ("After processing -sage:enable_cp_backend option argc = %d \n",argc);

  //
  // preinit_il option
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
     optionCount = sla(&argc, argv, "-sage:", "($)", "(preinit_il)",1);
     if( optionCount > 0 || (get_preinit_il() == true))
        {
          printf ("option -sage:preinit_il found \n");
          set_preinit_il(true);
          inputCommandLine.push_back("--preinit_il");
        }

  // printf ("After processing -sage:preinit_il option argc = %zu \n",argv.size());
#endif

  // ***********************************************************************
  // Now process the -D options, -I options and the filenames so that these
  // can also be used for the command line passed to the EDG frontend.
  // ***********************************************************************

  // *******************************
  // Add all input macro definitions
  // *******************************

     vector<string> macroDefineOptions;

  // DQ (9/25/2006): Fixed handling of "-D DEFINE_ERROR_WITH_SPACE -DDEFINE_OK_WITHOUT_SPACE"

  // skip the 0th entry since this is just the name of the program (e.g. rose)
     unsigned int argIndex = 1;
     while (argIndex < argv.size())
        {
       // most options appear as -<option>
       // have to process +w2 (warnings option) on some compilers so include +<option>
          if ( (argv[argIndex][0] == '-') && (argv[argIndex][1] == 'D') )
             {
               unsigned int length = argv[argIndex].size();
            // printf ("Look for include path:  argv[%d] = %s length = %d \n",argIndex,argv[argIndex],length);

               macroDefineOptions.push_back(argv[argIndex]);

            // DQ (9/19/2006): There must be an option string associated with each "-D" option
            // ROSE_ASSERT(length > 2);
               if (length == 2)
                  {
                    printf ("Handling the case of \"-D\" with orphened option (unclear if this is legal) \n");
                    macroDefineOptions.push_back("-D" + argv[argIndex + 1]);
                    ++argIndex;
                  }
             }
            else
             {
            // DQ (8/27/2010): Added support for -U to undefine macros (same code as for -D option).
               if ( (argv[argIndex][0] == '-') && (argv[argIndex][1] == 'U') )
                  {
                    unsigned int length = argv[argIndex].size();
                 // printf ("Look for include path:  argv[%d] = %s length = %d \n",argIndex,argv[argIndex],length);

                    macroDefineOptions.push_back(argv[argIndex]);

                    if (length == 2)
                       {
                         printf ("Handling the case of \"-U\" with orphened option (unclear if this is legal) \n");
                         macroDefineOptions.push_back("-U" + argv[argIndex + 1]);
                         ++argIndex;
                       }
                  }
             }

          argIndex++;
        }

#if 0
     for (int i=0; i < macroDefineOptionCounter; i++)
        {
          printf ("     macroDefineOptions[%d] = %s \n",i,macroDefineOptions[i]);
        }
#endif

  // Add filenames (of source files) to so that the EDG front end will know what files to process
     inputCommandLine.insert(inputCommandLine.end(), macroDefineOptions.begin(), macroDefineOptions.end());

  // DQ (9/24/2006): Add -D option so that we can know when to turn on vendor compiler specific macros for ANSI/ISO compatability.
     if (get_strict_language_handling() == true)
        {
          inputCommandLine.push_back("-DROSE_STRICT_LANGUAGE_HANDLING");
        }

  // ********************************************************************************************
  // Now add the include paths to the end so that EDG knows where to find header files to process
  // ********************************************************************************************

  // Now add the include paths to the end so that EDG knows where to find head files to process
  // Add all input include paths so that the EDG front end will know where to find headers

#if 0
     for (int i=0; i < includePathCounter; i++)
        {
          printf ("     includePaths[%d] = %s \n",i,includePaths[i]);
        }
#endif

  // Add the -I definitions to the command line
     for (vector<string>::const_iterator i = includePaths.begin(); i != includePaths.end(); ++i)
        {
          inputCommandLine.push_back("-I" + *i);
        }

  // *******************************************************************
  // Handle general edg options (-xxx)
  // *******************************************************************

  // Strip out all the -edg:xxx options and put them into the edg command line as -xxx

  // Resets modifiedArgc and allocates memory to modifiedArgv
     Rose_STL_Container<string> edgOptionList = CommandlineProcessing::generateOptionList (argv,"-edg:");
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"-",edgOptionList);

  // *******************************************************************
  // Handle general edg options (--xxx)
  // *******************************************************************

  // Strip out all the -edg:xxx options and put them into the edg command line as --xxx

  // Resets modifiedArgc and allocates memory to modifiedArgv
     edgOptionList = CommandlineProcessing::generateOptionList (argv,"--edg:");

  // DQ (8/6/2006): there are a number of options that when specified in their EDG forms
  // should turn on ROSE mechanisms.  "--edg:c" should turn on C mode in ROSE.
  // printf ("--edg option processing: edgOptionList.size() = %zu \n",edgOptionList.size());
     Rose_STL_Container<string>::iterator i = edgOptionList.begin();
     while (i != edgOptionList.end())
        {
       // fprintf (stderr, "edgOptionList: i = %s \n",i->c_str());
          if (*i == "c" || *i == "old_c")
             {
            // This is the EDG option "--c" obtained from the ROSE "--edg:c" option
               set_C_only(true);
             }
          if (*i == "c99")
             {
            // This is the EDG option "--c99" obtained from the ROSE "--edg:c99" option
               set_C99_only(true);
             }
          i++;
        }

     CommandlineProcessing::addListToCommandLine(inputCommandLine,"--",edgOptionList);

  // *******************************************************************
  // Handle general edg options (-xxx abc)
  // *******************************************************************

  // Handle edg options taking a parameter (string or integer)
     edgOptionList = CommandlineProcessing::generateOptionWithNameParameterList (argv,"-edg_parameter:");
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"-",edgOptionList);

  // *******************************************************************
  // Handle general edg options (--xxx abc)
  // *******************************************************************

  // Handle edg options taking a parameter (string or integer)
     edgOptionList = CommandlineProcessing::generateOptionWithNameParameterList (argv,"--edg_parameter:");
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"--",edgOptionList);

  // *******************************************************************
  //                       Handle UPC modes
  // *******************************************************************

  // DQ (9/19/2010): Added UPC++ mode (UPC (C modes) should have UPC++ == false.
  // Liao, 6/20/2008, handle UPC specific EDG options.
  // Generate --upc
  // if (get_UPC_only())
     if ((get_UPC_only() == true) && (get_UPCxx_only() == false))
        {
          inputCommandLine.push_back("--upc");
          inputCommandLine.push_back("--restrict");

#if 0
       // DQ (11/1/2011): This is not enough to support C++ code (e.g. "limits" header file).
#ifdef ROSE_USE_NEW_EDG_INTERFACE
       // DQ (2/17/2011): Added support for UPC (header are placed into include-staging directory).
          inputCommandLine.push_back("--sys_include");
          inputCommandLine.push_back(findRoseSupportPathFromBuild("include-staging", "share"));
#endif
#endif
        }

  // DQ (9/19/2010): Added support for UPC++. Previously the UPC used the C++ language internally this had to
  // be put back to C to provide less strict type-checking and so the new UPC++ option allows us to continue
  // to play with UPC++ as an idea for future work. This modes will also have (UPC == true).
  // Generate --upc++
     if (get_UPCxx_only() == true)
        {
       // DQ (9/19/2010): Testing use of newly added EDG option to control use
       // of C_dialect to allow C++ with UPC (which defines initial UPC++ work).
          inputCommandLine.push_back("--upc++");
          inputCommandLine.push_back("--restrict");

#if 0
       // DQ (11/1/2011): This is not enough to support C++ code (e.g. "limits" header file).
#ifdef ROSE_USE_NEW_EDG_INTERFACE
       // DQ (2/17/2011): Added support for UPC (header are placed into include-staging directory).
          inputCommandLine.push_back("--sys_include");
          inputCommandLine.push_back(findRoseSupportPathFromBuild("include-staging", "share"));
#endif
#endif
        }

  // Generate --upc_threads n
     int intOptionUpcThreads = get_upc_threads();
     if (intOptionUpcThreads > 0)
        {
          stringstream ss;
          ss << intOptionUpcThreads;
          inputCommandLine.push_back("--upc_threads");
          inputCommandLine.push_back(ss.str());
        }


  // *******************************************************************
  // Some EDG options have to turn on mechanism in ROSE
  // *******************************************************************

#if 0
     printf ("Some EDG options have to turn on mechanims in ROSE edgOptionList.size() = %ld \n",edgOptionList.size());
     Rose_STL_Container<string>::iterator j = edgOptionList.begin();
     while (j != edgOptionList.end())
        {
          printf ("edgOptionList: j = %s \n",j->c_str());
          j++;
        }
#endif

  // *******************************************************************
  // Handle specific edg options (-c)
  // *******************************************************************

  //
  // specify compilation only option (new style command line processing)
  //
     bool autoInstantiation = false;
     if ( CommandlineProcessing::isOption(argv,"-","c",false) == true )
        {
       // printf ("In build_EDG_CommandLine(): Option -c found (compile only)! \n");
          set_compileOnly(true);
        }
       else
        {
       // printf ("In build_EDG_CommandLine(): Option -c not found (compile AND link) set autoInstantiation = true ... \n");
          autoInstantiation = true;
        }

     if ( CommandlineProcessing::isOption(argv,"-rose:","wave",ROSE_WAVE_DEFAULT) == true )
        {
       // printf ("Option -c found (compile only)! \n");
          set_wave(true);
        }

     if (isPrelinkPhase() == true)
        {
          printf ("In build_EDG_CommandLine(): isPrelinkPhase() == true set autoInstantiation = true ... \n");
          autoInstantiation = true;
        }

  // DQ (10/15/2005): Trap out case of C programs where it is an EDG error to specify template instantiation details
     if (get_C_only() == true ||get_C99_only() == true )
        {
       // printf ("In build_EDG_CommandLine(): compiling input as C program so turn off all template instantiation details \n");
          autoInstantiation = false;
        }

     Rose_STL_Container<string> additionalOptions_a;
     Rose_STL_Container<string> additionalOptions_b;

     if (autoInstantiation == true)
        {
       // printf ("In build_EDG_CommandLine(): autoInstantiation = true adding --auto_instantiation -tused ... \n");
       // Even though this is not an error to EDG, it does not appear to force instantiation of all templates (because we need "-tused")
          additionalOptions_a.push_back("auto_instantiation");
          CommandlineProcessing::addListToCommandLine(inputCommandLine,"--",additionalOptions_a);

       // DQ (5/12/05): Set the instantiation mode to "used" for specify what sort of templates to instantiate automatically
       // (generate "-tused" instead of "--instantiate used" since EDG does not seems to accept options containing white space).
          additionalOptions_b.push_back("tused");
          CommandlineProcessing::addListToCommandLine(inputCommandLine, "-",additionalOptions_b);
        }
       else
        {
       // There are additional cases where we want to force instantiation of all used templates.

       // DQ (5/20/2005): Force instantiation of all used templated unless it is specified to instantiate no templates (explicitly to ROSE)
          bool instantiateAll = false;
          if (get_project() != NULL)
             {
               instantiateAll = (get_project()->get_template_instantiation_mode() == SgProject::e_default) ||
                                (get_project()->get_template_instantiation_mode() == SgProject::e_used)    ||
                                (get_project()->get_template_instantiation_mode() == SgProject::e_all)     ||
                                (get_project()->get_template_instantiation_mode() == SgProject::e_local);
             }

       // printf ("get_project()->get_template_instantiation_mode() = %d \n",get_project()->get_template_instantiation_mode());
       // printf ("In build_EDG_CommandLine(): instantiateAll = %s if true then adding --auto_instantiation -tlocal ... \n",
       //      instantiateAll ? "true" : "false");

       // DQ (6/1/2005):  This is the case of explicitly specifying the complation of C code and
       // not C++ code (EDG reports an error if auto_instantiation is specified for this case).
          if (get_C_only() == true || get_C99_only() == true)
             {
            // printf ("In build_EDG_CommandLine(): compiling input as C program so turn off all template instantiation details \n");
               instantiateAll = false;
             }

          if (instantiateAll == true)
             {
            // printf ("In build_EDG_CommandLine(): autoInstantiation = true adding --auto_instantiation -tlocal ... \n");
               additionalOptions_a.push_back("auto_instantiation");
               CommandlineProcessing::addListToCommandLine(inputCommandLine,"--",additionalOptions_a);

            // additionalOptions_b.push_back("tused");
               additionalOptions_b.push_back("tlocal");
               CommandlineProcessing::addListToCommandLine(inputCommandLine, "-",additionalOptions_b);
             }
        }

#if 0
  // DQ (5/20/05): Set the instantiation mode to "used" for specify what sort of templates to instantiate automatically
  // (generate "-tused" instead of "--instantiate used" since EDG does not seems to accept options containing white space).
     printf ("In build_EDG_CommandLine(): autoInstantiation = true adding --auto_instantiation -tused ... \n");

  // Even though this is not an error to EDG, it does not appear to force instantiation of all templates (because we need "-tused")
     additionalOptions_a.push_back("auto_instantiation");
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"--",additionalOptions_a);

     additionalOptions_b.push_back("tused");
     CommandlineProcessing::addListToCommandLine(inputCommandLine, "-",additionalOptions_b);
#endif

  // printf ("###### Located source filename = %s \n",get_sourceFileNameWithPath().c_str());
  // ROSE_ASSERT ( get_numberOfSourceFileNames() > 0 );
     ROSE_ASSERT ( get_sourceFileNameWithPath().empty() == false);
     //AS Added support for absolute paths
     Rose_STL_Container<string> fileList;
     std::string sourceFile = get_sourceFileNameWithPath();
     std::string sourceFilePath = StringUtility::getPathFromFileName(sourceFile);
     //Liao, 5/15/2009
     //the file name already has absolute path, the following code may be redundant.
     sourceFile = StringUtility::stripPathFromFileName(sourceFile);
     if(sourceFilePath == "" )
        sourceFilePath = "./";
     sourceFilePath = StringUtility::getAbsolutePathFromRelativePath(sourceFilePath);
#ifndef _MSC_VER
     fileList.push_back(sourceFilePath+"/"+sourceFile);
#else
     fileList.push_back(sourceFilePath+"\\"+sourceFile);
#endif
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"",fileList);

   // Liao, replaceRelativePath
#if 0
  for (size_t i = 0; i< inputCommandLine.size(); i++)
  {
    string cur_string = inputCommandLine[i];
    string::size_type pos = cur_string.find("-I..",0);
    string::size_type pos2 = cur_string.find("-I.",0);
    // replace -I.. -I../path  to -I/absolutepath/.. and -I/absolutepath/../path
    // replace -I. -I./path  to -I/absolutepath/. and -I/absolutepath/./path
    if ((pos ==0) || (pos2 ==0 ))
    {
      string orig_path = cur_string.substr(2); // get ..  ../path  .  ./path
      cur_string = "-I"+sourceFilePath+"/"+orig_path;
    }

   inputCommandLine[i] = cur_string;
  }
#endif
      //Liao, 5/15/2009
      // macro -D__GNUG__ should not be defined  for C only code,
      // some code relies on this macro to tell if bool type is allowed
      // vector<string> & inputCommandLine
       if (get_C_only()  || get_C99_only())
       {
          vector<string>::iterator iter;
          for (iter = inputCommandLine.begin(); iter!=inputCommandLine.end(); iter++)
          {
            string cur_str = *iter;
            string::size_type pos = cur_str.find("-D__GNUG__=",0);
            if (pos != string::npos)
              break;
          }
          if (iter != inputCommandLine.end())
          {
            inputCommandLine.erase(iter);
          }
       }

  // Debugging (verbose) output
     if ( (get_verbose() > 1) )
        {
          std::string argString = CommandlineProcessing::generateStringFromArgList(inputCommandLine,false,false);
          printf ("In build_EDG_CommandLine(): Input Command Line Arguments: \n%s \n",argString.c_str());

       // Alternative way of displaying the commandline parameters
          for (unsigned int i=0; i < inputCommandLine.size(); i++)
               printf ("inputCommandLine[%u] = %s \n",i,inputCommandLine[i].c_str());
        }

  // display("at base of build_EDG_CommandLine()");

#if 0
     printf ("Exiting at base of build_EDG_CommandLine() \n");
     ROSE_ABORT();
#endif
   }

vector<string>
SgFile::buildCompilerCommandLineOptions ( vector<string> & argv, int fileNameIndex, const string& compilerName )
   {
  // This function assembles the commandline that will be passed to the backend (vendor) C++/C, Fortran, or Java compiler
  // (using the new generated source code from the ROSE unparser).

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

  // display("Data in SgFile in buildCompilerCommandLineOptions()");

     if ( SgProject::get_verbose() >= 1 )
        {
          printf ("In buildCompilerCommandLineOptions(): compilerName = %s \n",compilerName.c_str());
        }

  // To use rose in place of a C or C++ compiler specify the compiler name using
  //      rose -compiler <originalCompilerName> ...
  // the default value of "originalCompilerName" is "CC"
     vector<string> compilerNameString;
     compilerNameString.push_back(compilerName);

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

#if 0
  // display("SgFile::buildCompilerCommandLineOptions()");
     printf ("C   compiler              = %s \n",BACKEND_C_COMPILER_NAME_WITH_PATH);
     printf ("C++ compiler              = %s \n",BACKEND_CXX_COMPILER_NAME_WITH_PATH);
     printf ("Fortran compiler          = %s \n",BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH);
     printf ("Java compiler             = %s \n",BACKEND_JAVA_COMPILER_NAME_WITH_PATH);
     printf ("Python interpreter        = %s \n",BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH);
     printf ("get_C_only()              = %s \n",(get_C_only() == true) ? "true" : "false");
     printf ("get_C99_only()            = %s \n",(get_C99_only() == true) ? "true" : "false");
     printf ("get_Cxx_only()            = %s \n",(get_Cxx_only() == true) ? "true" : "false");
     printf ("get_Fortran_only()        = %s \n",(get_Fortran_only() == true) ? "true" : "false");
     printf ("get_F77_only()            = %s \n",(get_F77_only() == true) ? "true" : "false");
     printf ("get_F90_only()            = %s \n",(get_F90_only() == true) ? "true" : "false");
     printf ("get_F95_only()            = %s \n",(get_F95_only() == true) ? "true" : "false");
     printf ("get_F2003_only()          = %s \n",(get_F2003_only() == true) ? "true" : "false");
     printf ("get_CoArrayFortran_only() = %s \n",(get_CoArrayFortran_only() == true) ? "true" : "false");
     printf ("get_Java_only()           = %s \n",(get_Java_only() == true) ? "true" : "false");
     printf ("get_Python_only()         = %s \n",(get_Python_only() == true) ? "true" : "false");
#endif

  // DQ (9/10/2006): We now explicitly store the C and C++ compiler names with
  // paths so that we can assemble the final commandline to compile the generated
  // code within ROSE.
  // We need a better way of identifying the C compiler which might not be known
  // ideally it should be specified at configure time so that it can be known in
  // case the -rose:C_only option is used.
     if (get_C_only() == true || get_C99_only() == true)
        {
       // compilerNameString = "gcc ";
          compilerNameString[0] = BACKEND_C_COMPILER_NAME_WITH_PATH;

       // DQ (6/4/2008): Added support to trigger use of C99 for older versions of GNU that don't use use C99 as the default.
          if (get_C99_only() == true)
             {
               compilerNameString.push_back("-std=gnu99");
             }
        }
       else
        {
          compilerNameString[0] = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
          if (get_Fortran_only() == true)
             {
            // compilerNameString = "f77 ";
               compilerNameString[0] = ROSE_GFORTRAN_PATH;

               if (get_backendCompileFormat() == e_fixed_form_output_format)
                  {
                 // If backend compilation is specificed to be fixed form, then allow any line length (to simplify code generation for now)
                 // compilerNameString += "-ffixed-form ";
                 // compilerNameString += "-ffixed-line-length- "; // -ffixed-line-length-<n>
                    compilerNameString.push_back("-ffixed-line-length-none");
                  }
                 else
                  {
                    if (get_backendCompileFormat() == e_free_form_output_format)
                       {
                      // If backend compilation is specificed to be free form, then allow any line length (to simplify code generation for now)
                      // compilerNameString += "-ffree-form ";
                      // compilerNameString += "-ffree-line-length-<n> "; // -ffree-line-length-<n>
                      // compilerNameString.push_back("-ffree-line-length-none");
#if USE_GFORTRAN_IN_ROSE
                      // DQ (9/16/2009): This option is not available in gfortran version 4.0.x (wonderful).
                         if ( (BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER >= 4) && (BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER >= 1) )
                            {
                              compilerNameString.push_back("-ffree-line-length-none");
                            }
#endif
                       }
                      else
                       {
                      // Do nothing (don't specify any option to control compilation of a specific format, assume defaults)

                      // Make this the default
                         if ( SgProject::get_verbose() >= 1 )
                              printf ("Compiling generated code using gfortran -ffixed-line-length-none to avoid 72 column limit in code generation \n");

                         compilerNameString.push_back("-ffixed-line-length-none");
                       }
                  }
             }
            else if (get_Java_only() == true)
             {
                 // compilerNameString[0] = ROSE_GNU_JAVA_PATH;
                    compilerNameString[0] = BACKEND_JAVA_COMPILER_NAME_WITH_PATH;
             }
            else if (get_Python_only() == true)
             {
                    compilerNameString[0] = BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH;
             }
        }

  // printf ("compilerName       = %s \n",compilerName);
  // printf ("compilerNameString = %s \n",compilerNameString.c_str());

  // tps (28 Aug 2008) : changed this so it does not pick up mpicc for icc
     string name = StringUtility::stripPathFromFileName(compilerNameString[0]);
     //     if (compilerNameString[0].find("icc") != string::npos)
     if (name == "icc")
        {
       // This is the Intel C compiler: icc, we need to add the -restrict option
          compilerNameString.push_back("-restrict");
        }

     //     if (compilerNameString[0].find("icpc") != string::npos)
     if (name == "icpc")
        {
       // This is the Intel C++ compiler: icc, we need to add the -restrict option
          compilerNameString.push_back("-restrict");
        }

  // DQ (9/24/2006): Not clear if we want this, if we just skip stripping it out then it will be passed to the backend directly!
  // But we have to add it in the case of "-rose:strict", so we have to add it uniformally and strip it from the input.
     if (get_strict_language_handling() == true)
        {
       // Check if it is appears as "-ansi" on the original commandline
          if ( CommandlineProcessing::isOption(argv,"-","ansi",false) == true )
             {
               printf ("Option -ansi detected on the original commandline \n");
             }
            else
             {
            // This is might be specific to GNU
               compilerNameString.push_back("-ansi");
             }
        }

#if 0
     printf ("Selected compilerNameString.size() = %zu compilerNameString = %s \n",compilerNameString.size(),StringUtility::listToString(compilerNameString).c_str());
#endif

#ifdef ROSE_USE_NEW_EDG_INTERFACE
  // DQ (1/12/2009): Allow in internal indicator that EDG version 3.10 or 4.0 (or greater)
  // is in use to be properly passed on the compilation of the generated code.
     compilerNameString.push_back("-DROSE_USE_NEW_EDG_INTERFACE");
#endif

  // Since we need to do this often, support is provided in the utility_functions.C
  // and we can simplify this code.
     std::string currentDirectory = getWorkingDirectory();

  // printf ("In buildCompilerCommandLineOptions(): currentDirectory = %s \n",currentDirectory);

     if (get_Java_only() == false && get_Python_only() == false)
        {
       // specify compilation only option (new style command line processing)
          if ( CommandlineProcessing::isOption(argv,"-","c",false) == true )
             {
            // printf ("Option -c found (compile only)! \n");
               set_compileOnly(true);
             }
            else
             {
            // printf ("Option -c not found (compile AND link) ... \n");
            // compilerNameString += " -c ";
             }

       // Liao, 2/13/2009. I think we should pass this macro to the backend compiler also
       // User programs may have rose-specific tweaks to enable ROSE translators to compile them
       // Part of solution to bug 316 :
       // https://outreach.scidac.gov/tracker/index.php?func=detail&aid=316&group_id=24&atid=185
          compilerNameString.push_back("-DUSE_ROSE");

       // Liao, 9/4/2009. If OpenMP lowering is activated. -D_OPENMP should be added
       // since we don't remove condition compilation preprocessing info. during OpenMP lowering
          if (get_openmp_lowering())  
          {
            compilerNameString.push_back("-D_OPENMP");
          }
     }

  // DQ (3/31/2004): New cleaned up source file handling
     Rose_STL_Container<string> argcArgvList = argv;

  // DQ (9/25/2007): Moved to std::vector from std::list uniformly within ROSE.
  // Remove the first argument (argv[0])
  // argcArgvList.pop_front();
     argcArgvList.erase(argcArgvList.begin());

#if 0
  // DQ (1/24/2010): Moved this inside of the true branch below.
     SgProject* project = isSgProject(this->get_parent());
     ROSE_ASSERT (project != NULL);
     Rose_STL_Container<string> sourceFilenames = project->get_sourceFileNameList();

     printf ("sourceFilenames.size() = %zu sourceFilenames = %s \n",sourceFilenames.size(),StringUtility::listToString(sourceFilenames).c_str());
#endif

  // DQ (4/20/2006): Modified to only do this when generating code and compiling it
  // Remove the source names from the argcArgvList (translated versions of these will be inserted later)
  // if (get_skip_unparse() == true && get_skipfinalCompileStep() == false)
     if (get_skip_unparse() == false)
        {
       // DQ (1/24/2010): Now that we have directory support, the parent of a SgFile does not have to be a SgProject.
       // SgProject* project = isSgProject(this->get_parent())
          SgProject* project = TransformationSupport::getProject(this);
          ROSE_ASSERT (project != NULL);
          Rose_STL_Container<string> sourceFilenames = project->get_sourceFileNameList();
#if 0
          printf ("sourceFilenames.size() = %zu sourceFilenames = %s \n",sourceFilenames.size(),StringUtility::listToString(sourceFilenames).c_str());
#endif
          for (Rose_STL_Container<string>::iterator i = sourceFilenames.begin(); i != sourceFilenames.end(); i++)
             {
#if 0
               printf ("Removing sourceFilenames list element i = %s \n",(*i).c_str());
#endif

#if USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST
#error "USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST is not supported yet"

            // DQ (9/1/2006): Check for use of absolute path and convert filename to absolute path if required
               bool usesAbsolutePath = ((*i)[0] == '/');
               if (usesAbsolutePath == false)
                  {
                    string targetSourceFileToRemove = StringUtility::getAbsolutePathFromRelativePath(*i);
                  printf ("Converting source file to absolute path to search for it and remove it! targetSourceFileToRemove = %s \n",targetSourceFileToRemove.c_str());
                    argcArgvList.remove(targetSourceFileToRemove);
                  }
                 else
                  {
                 // printf ("This source file used the absolute path so no conversion to absolute path is required! \n");
                    argcArgvList.remove(*i);
                  }
#else
            // DQ (9/25/2007): Moved to std::vector from std::list uniformally within ROSE.
            // printf ("Skipping test for absolute path removing the source filename as it appears in the source file name list file = % \n",i->c_str());
            // argcArgvList.remove(*i);
            // The if here is to skip binaries that don't appear on the command line for those cases when a single project has both binaries and source code
               if (find(argcArgvList.begin(),argcArgvList.end(),*i) != argcArgvList.end())
                  {
                    argcArgvList.erase(find(argcArgvList.begin(),argcArgvList.end(),*i));
                  }
#endif
             }
        }

#if 0
     printf ("After removing source file name: argcArgvList.size() = %zu argcArgvList = %s \n",argcArgvList.size(),StringUtility::listToString(argcArgvList).c_str());
  // ROSE_ASSERT(false);
#endif

  // AS(080704) Fix so that if user specifies name of -o file rose do not specify another in addition
     bool  objectNameSpecified = false;
     for (Rose_STL_Container<string>::iterator i = argcArgvList.begin(); i != argcArgvList.end(); i++)
        {
       // printf ("In SgFile::buildCompilerCommandLineOptions(): Loop over commandline arguments i = %s \n",i->c_str());
       // DQ (8/17/2006): This fails for directories such as "ROSE/projects/OpenMP_Translator/tests/npb2.3-omp-c"
       // which can be repeated in the specification of include directives on the commandline.
       // We need to check for the leading characters and nothing else.
       // if (i->find("-o") != std::string::npos)
       // if (i->find("-o ") != std::string::npos)
       // printf ("i->substr(0,2) = %s \n",i->substr(0,2).c_str());
          if (i->substr(0,2) == "-o")
             {
            // DQ (6/12/2005): Added error checking!
               if (objectNameSpecified == true)
                  {
                 // Error: "-o" has been specified twice
                    printf ("Error: \"-o \" has been specified twice \n");
                  }
               ROSE_ASSERT(objectNameSpecified == false);
               objectNameSpecified = true;
             }
        }

     Rose_STL_Container<string> tempArgcArgv;
     for (Rose_STL_Container<string>::iterator i = argcArgvList.begin(); i != argcArgvList.end(); i++)
        {
          // Liao, 11/19/2009
          // We now only handles compilation for SgFile::compileOutput(),
          // so we need to remove linking related flags such as '-lxx' from the original command line
          // Otherwise gcc will complain:  -lm: linker input file unused because linking not done
          if(i->substr(0,2) != "-l")
          {
            tempArgcArgv.push_back(*i);
          }
        }
     argcArgvList.swap(tempArgcArgv);
  // DQ (4/14/2005): Fixup quoted strings in args fix "-DTEST_STRING_MACRO="Thu Apr 14 08:18:33 PDT 2005"
  // to be -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\"  This is a problem in the compilation of
  // a Kull file (version.cc), when the backend is specified as /usr/apps/kull/tools/mpig++-3.4.1.  The
  // problem is that /usr/apps/kull/tools/mpig++-3.4.1 is a wrapper for a shell script /usr/local/bin/mpiCC
  // which does not tend to observe quotes well.  The solution is to add additional escaped quotes.
     for (Rose_STL_Container<string>::iterator i = argcArgvList.begin(); i != argcArgvList.end(); i++)
        {
#if 0
          printf ("sizeof(std::string::size_type) = %d \n",sizeof(std::string::size_type));
          printf ("sizeof(std::string::iterator)  = %d \n",sizeof(std::string::iterator));
          printf ("sizeof(unsigned int)           = %d \n",sizeof(unsigned int));
          printf ("sizeof(unsigned long)          = %d \n",sizeof(unsigned long));
#endif

       // DQ (1/26/2006): Fix for 64 bit support.
       // unsigned int startingQuote = i->find("\"");
          std::string::size_type startingQuote = i->find("\"");
          if (startingQuote != std::string::npos)
             {
            // This string at least has a quote
            // unsigned int endingQuote   = i->rfind("\"");
               std::string::size_type endingQuote   = i->rfind("\"");

            // There should be a double quote on both ends of the string
               ROSE_ASSERT (endingQuote != std::string::npos);

               std::string quotedSubstring = i->substr(startingQuote,endingQuote);
            // printf ("quotedSubstring = %s \n",quotedSubstring.c_str());

               std::string fixedQuotedSubstring = std::string("\\\"") + quotedSubstring + std::string("\\\"");
            // printf ("fixedQuotedSubstring = %s \n",fixedQuotedSubstring.c_str());

            // Now replace the quotedSubstring with the fixedQuotedSubstring
               i->replace(startingQuote,endingQuote,fixedQuotedSubstring);

            // printf ("Modified argument = %s \n",(*i).c_str());
             }
        }

  // Add any options specified by the user (and add space at the end)
     compilerNameString.insert(compilerNameString.end(), argcArgvList.begin(), argcArgvList.end());

  // printf ("buildCompilerCommandLineOptions() #1: compilerNameString = \n%s \n",compilerNameString.c_str());

     std::string sourceFileName = get_sourceFileNameWithPath();

     std::string oldFileNamePathOnly = ROSE::getPathFromFileName(sourceFileName.c_str());
     std::string oldFileName         = ROSE::stripPathFromFileName(sourceFileName.c_str());

#if 0
     printf ("oldFileNamePathOnly = %s \n",oldFileNamePathOnly.c_str());
     printf ("oldFileName         = %s \n",oldFileName.c_str());
#endif

  // DQ (4/2/2011): Java does not have -I as an accepted option.
     if (get_Java_only() == false && get_Python_only() == false)
        {
  // DQ (12/8/2004): Add -Ipath option so that source file's directory will be searched for any 
  // possible headers.  This is especially important when we are compiling the generated file
  // located in a different directory!  (When the original source file included header files
  // in the source directory!)  This is only important when get_useBackendOnly() == false
  // since otherwise the source file is the original source file and the compiler will search
  // its directory for header files.  Be sure the put the oldFile's source directory last in the
  // list of -I options so that it will be searched last (preserving the semantics of #include "...").
  // Only add the path if it is a valid name (not an empty name, in which case skip it since the oldFile
  // is in the current directory (likely a generated file itself; e.g. swig or ROSE applied recursively, etc.)).
  // printf ("oldFileNamePathOnly.length() = %d \n",oldFileNamePathOnly.length());
     if (oldFileNamePathOnly.empty() == false)
        {
          vector<string>::iterator iter;
       // find the very first -Ixxx option's position
          for (iter = compilerNameString.begin(); iter != compilerNameString.end(); iter++)
             {
               string cur_string =*iter;
               string::size_type pos = cur_string.find("-I",0);
               if (pos==0)
                    break;
             }
       // Liao, 5/15/2009
       // the input source file's path has to be the first one to be searched for header!
       // This is required since one of the SPEC CPU 2006 benchmarks: gobmk relies on this to be compiled.
       // insert before the position

       // negara1 (07/14/2011): The functionality of header files unparsing takes care of this, so this is needed
       // only when header files unparsing is not enabled.
       if (!this -> get_unparseHeaderFiles()) {
         compilerNameString.insert(iter, std::string("-I") + oldFileNamePathOnly); 
       }
     }
        }

    // Liao 3/30/2011. the search path for the installation path should be the last one, after paths inside
    // source trees, such as -I../../../../sourcetree/src/frontend/SageIII and 
    // -I../../../../sourcetree/src/midend/programTransformation/ompLowering
     if (get_openmp_lowering())  
     {
       vector<string>::iterator iter, iter_last_inc=compilerNameString.begin();
       // find the very last -Ixxx option's position
       // This for loop cannot be merged with the previous one due to iterator invalidation rules.
       for (iter = compilerNameString.begin(); iter != compilerNameString.end(); iter++) 
       {
         string cur_string =*iter;
         string::size_type pos = cur_string.find("-I",0);
         if (pos==0) 
         {
           iter_last_inc = iter;
         }
       }
       if (iter_last_inc != compilerNameString.end())
         iter_last_inc ++; // accommodate the insert-before-an-iterator semantics used in vector::insert() 
 
       // Liao, 9/22/2009, we also specify the search path for libgomp_g.h, which is installed under $ROSE_INS/include
       // and the path to libgomp.a/libgomp.so, which are located in $GCC_GOMP_OPENMP_LIB_PATH

       // Header should always be available 
       // the conditional compilation is necessary to pass make distcheck,
       // where only a minimum configuration options are used and not all macros are defined. 
#ifdef ROSE_INSTALLATION_PATH 
       string include_path(ROSE_INSTALLATION_PATH);
       include_path += "/include"; 
       compilerNameString.insert(iter_last_inc, "-I"+include_path); 
#endif
     }
 
  // DQ (4/20/2006): This allows the ROSE translator to be just a wrapper for the backend (vendor) compiler.
  // compilerNameString += get_unparse_output_filename();
     if (get_skip_unparse() == false)
        {
       // Generate the name of the ROSE generated source file (instead of the original source file)
       // this file will be compiled by the backend (vendor) compiler.
          ROSE_ASSERT(get_unparse_output_filename().empty() == false);
          compilerNameString.push_back(get_unparse_output_filename());
        }
       else
        {
       // In this case the compilerNameString already has the original file name since it was not removed
       // compilerNameString += get_unparse_output_filename();
       // printf ("Case of skip_unparse() == true: original source file name should be present compilerNameString = %s \n",compilerNameString.c_str());
        }

     if ( get_compileOnly() == true )
        {
          std::string objectFileName = generateOutputFileName();
       // printf ("In buildCompilerCommandLineOptions: objectNameSpecified = %s objectFileName = %s \n",objectNameSpecified ? "true" : "false",objectFileName.c_str());

       // DQ (4/2/2011): Java does not have -o as an accepted option, though the "-d <dir>" can be used to specify where class files are put.
       // Currently we explicitly output "-d ." so that generated class files will be put into the current directory (build tree), but this
       // is not standard semantics for Java (though it makes the Java support in ROSE consistent with other languages supported in ROSE).
          if (get_Java_only() == false && get_Python_only() == false)
             {
            // DQ (7/14/2004): Suggested fix from Andreas, make the object file name explicit
               if (objectNameSpecified == false)
                  {
                 // cout<<"making object file explicit for compilation only mode without -o options"<<endl;
                    compilerNameString.push_back("-o");
                    compilerNameString.push_back(currentDirectory + "/" + objectFileName);
                  }
             }
        }
       else
        {
       // Liao 11/19/2009, changed to support linking multiple source files within one command line
       // We change the compilation mode for each individual file to compile-only even
       // when the original command line is to generate the final executable.
       // We generate the final executable at the SgProject level from object files of each source file

          if (get_Java_only() == false && get_Python_only() == false)
             {
            // cout<<"turn on compilation only at the file compilation level"<<endl;
               compilerNameString.push_back("-c");
            // For compile+link mode, -o is used for the final executable, if it exists
            // We make -o objectfile explicit 
               std::string objectFileName = generateOutputFileName();
               compilerNameString.push_back("-o");
               compilerNameString.push_back(currentDirectory + "/" + objectFileName);
             }
        }

#if 0
     printf ("At base of buildCompilerCommandLineOptions: compilerNameString = \n%s\n",CommandlineProcessing::generateStringFromArgList(compilerNameString,false,false).c_str());
#endif
#if 0
     printf ("Exiting at base of buildCompilerCommandLineOptions() ... \n");
     ROSE_ASSERT (false);
#endif

#if 0
     cout<<"Debug: SgFile::buildCompilerCommandLineOptions() compilerNameString is "<<endl;
     for (vector<string>::iterator iter = compilerNameString.begin(); iter != compilerNameString.end(); iter++)
     {
       std::string str = *iter;
       cout<<"\t"<<str<<endl;
      }
#endif

     return compilerNameString;
   } // end of SgFile::buildCompilerCommandLineOptions()

