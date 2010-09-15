// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
//#include "astMergeAPI.h"
#include "rose_paths.h"
//#include "setup.h"
#include "astPostProcessing.h"
#include <sys/stat.h>

#include "omp_lowering.h"
#include "attachPreprocessingInfo.h"
#include "astMergeAPI.h"
//#include "sageSupport.h"

#include "BinaryLoader.h"
#include "Partitioner.h"
#include "sageBuilder.h"

#ifdef _MSC_VER
//#pragma message ("WARNING: wait.h header file not available in MSVC.")
#else
#include <sys/wait.h>
#include "PHPFrontend.h"
#endif

#ifdef _MSC_VER
//#pragma message ("WARNING: libgen.h header file not available in MSVC.")
#else
#include <libgen.h>
#endif

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
//FMZ (5/19/2008): 
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#include "FortranModuleInfo.h"
#include "FortranParserState.h"
#include "unparseFortran_modfile.h"
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif

#ifdef HAVE_DLADDR
#include <dlfcn.h>
#endif

#if USING_OLD_EXECUTABLE_FORMAT_SUPPORT
// DQ (8/12/2008): This constructor is implemented in sageSupport.C and 
// will be removed later once the new IR nodes are integrated into use.
#include "ExecELF.h"
#endif

//Needed for boost::filesystem::exists(...)
#include "boost/filesystem.hpp"
#include <stdio.h>

//Liao, 10/27/2008: parsing OpenMP pragma here
//Handle OpenMP pragmas. This should be called after preprocessing information is attached since macro calls may exist within pragmas, Liao, 3/31/2009
#include "OmpAttribute.h"
extern int omp_parse();
extern OmpSupport::OmpAttribute* getParsedDirective();
extern void omp_parser_init(SgNode* aNode, const char* str);
void processOpenMP(SgSourceFile* sageFilePtr);
//Fortran OpenMP parser interface
void parse_fortran_openmp(SgSourceFile *sageFilePtr);
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

// DQ (9/17/2009): This appears to only be required for the GNU 4.1.x compiler (not for any earlier or later versions).
extern const std::string ROSE_GFORTRAN_PATH;

#ifdef _MSC_VER
// DQ (11/29/2009): MSVC does not support sprintf, but "_snprintf" is equivalent 
// (note: printf_S is the safer version but with a different function argument list).
// We can use a macro to handle this portability issue for now.
#define snprintf _snprintf
#endif

std::string
SgValueExp::get_constant_folded_value_as_string()
   {
  // DQ (8/18/2009): Added support for generating a string from a SgValueExp.
  // Note that the point is not not call unparse since that would providethe 
  // expression tree and we want the constant folded value.

     string s;
     const int max_buffer_size = 500;
     char buffer[max_buffer_size];
     switch (variantT())
        {
          case V_SgIntVal: 
             {
               SgIntVal* integerValueExpression = isSgIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %d \n",numericValue);
               snprintf (buffer,max_buffer_size,"%d",numericValue);
               s = buffer;
               break;
             }

          case V_SgUnsignedLongLongIntVal:
             {
               SgUnsignedLongLongIntVal* integerValueExpression = isSgUnsignedLongLongIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned long long int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %llu \n",numericValue);
               snprintf (buffer,max_buffer_size,"%llu",numericValue);
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgUnsignedLongVal:
             {
               SgUnsignedLongVal* integerValueExpression = isSgUnsignedLongVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned long int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %llu \n",numericValue);
               snprintf (buffer,max_buffer_size,"%lu",numericValue);
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgUnsignedIntVal:
             {
               SgUnsignedIntVal* integerValueExpression = isSgUnsignedIntVal(this);
               ROSE_ASSERT(integerValueExpression != NULL);
               unsigned int numericValue = integerValueExpression->get_value();
            // printf ("numericValue of constant folded expression = %llu \n",numericValue);
               snprintf (buffer,max_buffer_size,"%u",numericValue);
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgBoolValExp:
             {
               SgBoolValExp* booleanValueExpression = isSgBoolValExp(this);
               ROSE_ASSERT(booleanValueExpression != NULL);
               bool booleanValue = booleanValueExpression->get_value();
               snprintf (buffer,max_buffer_size,"%s",booleanValue == true ? "true" : "false");
               s = buffer;
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgStringVal:
             {
               SgStringVal* stringValueExpression = isSgStringVal(this);
               ROSE_ASSERT(stringValueExpression != NULL);
               s = stringValueExpression->get_value();
               break;
             }

       // DQ (8/19/2009): Added case
          case V_SgCharVal:
             {
               SgCharVal* charValueExpression = isSgCharVal(this);
               ROSE_ASSERT(charValueExpression != NULL);
               s = charValueExpression->get_value();
               break;
             }

          default:
             {
               printf ("Error case of value = %s not handled \n",this->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

void
whatTypeOfFileIsThis( const string & name )
   {
  // DQ (2/3/2009): It is helpful to report what type of file this is where possible.
  // Call the Unix "file" command, it would be great if this was an available 
  // system call (but Robb thinks it might not always be available).

     vector<string> commandLineVector;
     commandLineVector.push_back("file -b " + name);

  // printf ("Unknown file: %s ",name.c_str());
     printf ("Error: unknown file type: ");
     flush(cout);
     
  // I could not make this work!
  // systemFromVector (commandLineVector);

  // Use "-b" for brief mode!
     string commandLine = "file " + name;
     system(commandLine.c_str());
   }



void
outputTypeOfFileAndExit( const string & name )
   {
  // DQ (8/20/2008): The code (from Robb) identifies what kind of file this is or 
  // more specifically what kind of file most tools would think this 
  // file is (using the system file(1) command as a standard way to identify
  // file types using their first few bytes.

  // printf ("In outputTypeOfFileAndExit(%s): Evaluate the file type \n",name.c_str());

#if 0
  // DQ (2/3/2009): This works now, I think that Andreas fixed it.

  // Use file(1) to try to figure out the file type to report in the exception
     int child_stdout[2];
     pipe(child_stdout);
     pid_t pid = fork();

     printf ("pid = %d \n",pid);

     if (pid == -1)
        { // Error
          perror("fork: error in outputTypeOfFileAndExit ");
          exit (1);
        }
     if (0 == pid)
        {
          close(0);
          dup2(child_stdout[1], 1);
          close(child_stdout[0]);
          close(child_stdout[1]);
          execlp("/usr/bin/file", "/usr/bin/file", "-b", name.c_str(), NULL);
          exit(1);
        }
       else
        {
          int status;
          if (waitpid(pid, &status, 0) == -1)
             {
               perror("waitpid");
               abort();
             }

          char buf[4096];
          memset(buf, 0, sizeof buf);
          read(child_stdout[0], buf, sizeof buf);
          std::string buffer(buf);
          buffer =  name+ " unrecognized file format: " + buffer;

       // DQ (2/3/2009): It is helpful to report what type of file this is where possible.
          whatTypeOfFileIsThis(name);

          throw SgAsmGenericFile::FormatError(buffer.c_str());
        }
#else
     whatTypeOfFileIsThis(name);

  // printf ("\n\nExiting: Unknown file Error \n\n");
  // ROSE_ASSERT(false);
     abort();
#endif
   }



// DQ (1/5/2008): These are functions separated out of the generated
// code in ROSETTA.  These functions don't need to be generated since
// there implementation is not as dependent upon the IR as other functions
// (e.g. IR node member functions).
//
// Switches taking a second parameter need to be added to CommandlineProcessing::isOptionTakingSecondParameter().

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


string
findRoseSupportPathFromSource(const string& sourceTreeLocation,
                              const string& installTreeLocation) {
  string installTreePath;
  bool inInstallTree = roseInstallPrefix(installTreePath);
  if (inInstallTree) {
    return installTreePath + "/" + installTreeLocation;
  } else {
    return string(ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR) + "/" + sourceTreeLocation;
  }
}

string
findRoseSupportPathFromBuild(const string& buildTreeLocation,
                             const string& installTreeLocation) {
  string installTreePath;
  bool inInstallTree = roseInstallPrefix(installTreePath);
  if (inInstallTree) {
    return installTreePath + "/" + installTreeLocation;
  } else {
    return string(ROSE_AUTOMAKE_TOP_BUILDDIR) + "/" + buildTreeLocation;
  }
}
//! Check if we can get an installation prefix of rose based on the current running translator.
// There are two ways
//   1. if dladdr is supported: we resolve a rose function (roseInstallPrefix()) to obtain the 
//      file (librose.so) defining this function 
//      Then we check the parent directory of librose.so
//          if .libs or src --> in a build tree
//          otherwise: librose.so is in an installation tree
//   2. if dladdr is not supported or anything goes wrong, we check an environment variable
//     ROSE_IN_BUILD_TREE to tell if the translator is started from a build tree or an installation tree
//     Otherwise we pass the --prefix= ROSE_AUTOMAKE_PREFIX as the installation prefix
bool roseInstallPrefix(std::string& result) {
#ifdef HAVE_DLADDR
  {
    Dl_info info;
    int retval = dladdr((void*)(&roseInstallPrefix), &info);
    if (retval == 0) goto default_check;
    char* libroseName = strdup(info.dli_fname);
    if (libroseName == NULL) goto default_check;
    char* libdir = dirname(libroseName);
    if (libdir == NULL) {free(libroseName); goto default_check;}
    char* libdirCopy = strdup(libdir);
    if (libdirCopy == NULL) {free(libroseName); free(libdirCopy); goto default_check;}
    char* libdirBasenameCS = basename(libdirCopy);
    if (libdirBasenameCS == NULL) {free(libroseName); free(libdirCopy); goto default_check;}
    string libdirBasename = libdirBasenameCS;
    free(libdirCopy);
    char* prefixCS = dirname(libdir);
    if (prefixCS == NULL) {free(libroseName); goto default_check;}
    string prefix = prefixCS;
    free(libroseName);
// Liao, 12/2/2009
// Check the librose's parent directory name to tell if it is within a build or installation tree
// This if statement has the assumption that libtool is used to build librose so librose.so is put under .libs 
// which is not true for cmake building system
// For cmake, librose is created directly under build/src
//    if (libdirBasename == ".libs") {
    if (libdirBasename == ".libs" || libdirBasename == "src") {
      return false;
    } else {
      // the translator must locate in the installation_tree/lib 
      // TODO what about lib64??
       if (libdirBasename != "lib")
          {
            printf ("Error: unexpected libdirBasename = %s (result = %s, prefix = %s) \n",libdirBasename.c_str(),result.c_str(),prefix.c_str());
          }

   // DQ (12/5/2009): Is this really what we need to assert?
   // ROSE_ASSERT (libdirBasename == "lib");

      result = prefix;
      return true;
    }
  }
#endif
default_check:
#ifdef HAVE_DLADDR
  // Emit a warning that the hard-wired prefix is being used
  cerr << "Warning: roseInstallPrefix() is using the hard-wired prefix and ROSE_IN_BUILD_TREE even though it should be relocatable" << endl;
#endif
  // dladdr is not supported, we check an environment variables to tell if the 
  // translator is running from a build tree or an installation tree
  if (getenv("ROSE_IN_BUILD_TREE") != NULL) {
    return false;
  } else {
// Liao, 12/1/2009 
// this variable is set via a very bad way, there is actually a right way to use --prefix VALUE within automake/autoconfig
// config/build_rose_paths.Makefile
// Makefile:       @@echo "const std::string ROSE_AUTOMAKE_PREFIX        = \"/home/liao6/opt/roseLatest\";" >> src/util/rose_paths.C
// TODO fix this to support both automake and cmake 's installation configuration options
    result = ROSE_AUTOMAKE_PREFIX;
    return true;
  }
}

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

     if ( CommandlineProcessing::isOption(local_commandLineArgumentList,"-rose:","wave",false) == true )
        {
       // printf ("Option -c found (compile only)! \n");
          set_wave(true);
        }

     
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

  // DQ (1/13/2009): Added support for GNU -include <header_file> option (include this file before all others).
     string tempDirectory;
     iter = local_commandLineArgumentList.begin();
     while ( iter != local_commandLineArgumentList.end() )
        {
          if ( *iter == "-isystem")
             {
               iter++;
               tempDirectory = *iter;

            // printf ("Adding tempHeaderFile = %s to p_preincludeDirectoryList \n",tempDirectory.c_str());
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
                    ROSE_ASSERT(false);
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
	      //AS Changed source code to support absolute paths
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
        }

  //
  // skip_transformation option: if transformations of the AST check this variable then the 
  // resulting translators can skip the transformatios via this command-line specified option.
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_transformation)",true) == true )
        {
          printf ("option -rose:skip_transformation found \n");
          set_skip_transformation(true);
        }

  //
  // skip_unparse option: if analysis only (without transformatio is required, then this can significantly 
  // improve the performance since it will also skip the backend compilation, as I recall)
  //
     if ( CommandlineProcessing::isOption(argv,"-rose:","(skip_unparse)",true) == true )
        {
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
         try {
             unsigned heuristics = get_disassemblerSearchHeuristics();
             heuristics = Disassembler::parse_switches(stringParameter, heuristics);
             set_disassemblerSearchHeuristics(heuristics);
         } catch(const Disassembler::Exception &e) {
             fprintf(stderr, "%s in \"-rose:disassembler_search\" switch\n", e.mesg.c_str());
             ROSE_ASSERT(!"error parsing -rose:disassembler_search");
         }
     }

  // RPM (1/4/2010): Partitioner function search methods
     if (CommandlineProcessing::isOptionWithParameter(argv, "-rose:", "partitioner_search", stringParameter, true)) {
         try {
             unsigned heuristics = get_partitionerSearchHeuristics();
             heuristics = Partitioner::parse_switches(stringParameter, heuristics);
             set_partitionerSearchHeuristics(heuristics);
         } catch(const std::string &e) {
             fprintf(stderr, "%s in \"-rose:partitioner_search\" switch\n", e.c_str());
             ROSE_ASSERT(!"error parsing -rose:partitioner_search");
         }
     }

  // RPM (6/9/2010): Partitioner configuration
     if (CommandlineProcessing::isOptionWithParameter(argv, "-rose:", "partitioner_config", stringParameter, true)) {
         set_partitionerConfigurationFileName(stringParameter);
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
     optionCount = sla(argv, "-rose:", "($)", "(exit_after_parser)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_syntax_check)",1);
     optionCount = sla(argv, "-rose:", "($)", "(relax_syntax_check)",1);

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

     optionCount = sla(argv, "-rose:", "($)", "(skip_rose)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_transformation)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_unparse)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_includes)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_line_directives)",1);

     optionCount = sla(argv, "-rose:", "($)", "(unparse_instruction_addresses)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_raw_memory_contents)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_binary_file_format)",1);

     optionCount = sla(argv, "-rose:", "($)", "(collectAllCommentsAndDirectives)",1);
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

  // DQ (2/20/2010): Remove this option when building the command line for the vendore compiler.
     int optionCount = 0;
     optionCount = sla(argv, "--edg:", "($)", "(no_warnings)",1);

#if 0
     Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("In SgFile::stripEdgCommandLineOptions: argv = \n%s \n",StringUtility::listToString(l).c_str());

     printf ("Exiting in stripEdgCommandLineOptions() \n");
     ROSE_ASSERT (1 == 2);
#endif
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


/* This function suffers from the same problems as CommandlineProcessing::isExecutableFilename(), namely that the list of
 * magic numbers used here needs to be kept in sync with changes to the binary parsers. */
bool
isBinaryExecutableFile ( string sourceFilename )
   {
     bool returnValue = false;

     if ( SgProject::get_verbose() > 1 )
          printf ("Inside of isBinaryExecutableFile(%s) \n",sourceFilename.c_str());

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "rb");
     if (!f)
        {
          printf ("Could not open file");
          ROSE_ASSERT(false);
        }

     int character0 = fgetc(f);
     int character1 = fgetc(f);

  // The first character of an ELF binary is '\127' and for a PE binary it is 'M'
  // Note also that some MS-DOS headers can start with "ZM" instead of "MZ" due to
  // early confusion about little endian handling for MS-DOS where it was ported 
  // to not x86 plaforms.  I am not clear how wide spread loaders of this type are.

     if (character0 == 127 || character0 == 77)
        {
          if (character1 == 'E' || character1 == 'Z')
             {
               returnValue = true;
             }
        }

      fclose(f);

      return returnValue;
    }

bool
isLibraryArchiveFile ( string sourceFilename )
   {
  // The if this is a "*.a" file, not that "*.so" files
  // will appear as an executable (same for Windows "*.dll"
  // files.

     bool returnValue = false;

     if ( SgProject::get_verbose() > 1 )
          printf ("Inside of isLibraryArchiveFile(%s) \n",sourceFilename.c_str());

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "rb");
     if (!f)
        {
          printf ("Could not open file in isLibraryArchiveFile()");
          ROSE_ASSERT(false);
        }

     string magicHeader;
     for (int i = 0; i < 7; i++)
        {
          magicHeader = magicHeader + (char)getc(f);
        }

  // printf ("magicHeader = %s \n",magicHeader.c_str());
     returnValue = (magicHeader == "!<arch>");

  // printf ("isLibraryArchiveFile() returning %s \n",returnValue ? "true" : "false");

     fclose(f);

     return returnValue;
   }


void
SgFile::initializeSourcePosition( const std::string & sourceFilename )
   {
     ROSE_ASSERT(this != NULL);

  // printf ("Inside of SgFile::initializeSourcePosition() \n");

     Sg_File_Info* fileInfo = new Sg_File_Info(sourceFilename,1,1);
     ROSE_ASSERT(fileInfo != NULL);

  // set_file_info(fileInfo);
     set_startOfConstruct(fileInfo);
     fileInfo->set_parent(this);
     ROSE_ASSERT(get_startOfConstruct() != NULL);
     ROSE_ASSERT(get_file_info() != NULL);
   }

void
SgSourceFile::initializeGlobalScope()
   {
     ROSE_ASSERT(this != NULL);

  // printf ("Inside of SgSourceFile::initializeGlobalScope() \n");

  // Note that SgFile::initializeSourcePosition() should have already been called.
     ROSE_ASSERT(get_startOfConstruct() != NULL);

     string sourceFilename = get_startOfConstruct()->get_filename();

  // DQ (8/31/2006): Generate a NULL_FILE (instead of SgFile::SgFile) so that we can 
  // enforce that the filename is always an absolute path (starting with "/").
  // Sg_File_Info* globalScopeFileInfo = new Sg_File_Info("SgGlobal::SgGlobal",0,0);
     Sg_File_Info* globalScopeFileInfo = new Sg_File_Info(sourceFilename,0,0);
     ROSE_ASSERT (globalScopeFileInfo != NULL);

  // printf ("&&&&&&&&&& In SgSourceFile::initializeGlobalScope(): Building SgGlobal (with empty filename) &&&&&&&&&& \n");

     set_globalScope( new SgGlobal( globalScopeFileInfo ) );
     ROSE_ASSERT (get_globalScope() != NULL);

  // DQ (2/15/2006): Set the parent of the SgGlobal IR node
     get_globalScope()->set_parent(this);

  // DQ (8/21/2008): Set the end of the global scope (even if it is updated later)
  // printf ("In SgFile::initialization(): p_root->get_endOfConstruct() = %p \n",p_root->get_endOfConstruct());
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct() == NULL);
     get_globalScope()->set_endOfConstruct(new Sg_File_Info(sourceFilename,0,0));
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct() != NULL);

  // DQ (1/21/2008): Set the filename in the SgGlobal IR node so that the traversal to add CPP directives and comments will succeed.
     ROSE_ASSERT (get_globalScope() != NULL);
     ROSE_ASSERT(get_globalScope()->get_startOfConstruct() != NULL);

  // DQ (8/21/2008): Modified to make endOfConstruct consistant (avoids warning in AST consistancy check).
  // ROSE_ASSERT(p_root->get_endOfConstruct()   == NULL);
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct()   != NULL);

  // p_root->get_file_info()->set_filenameString(p_sourceFileNameWithPath);
  // ROSE_ASSERT(p_root->get_file_info()->get_filenameString().empty() == false);

#if 0
     Sg_File_Info::display_static_data("Resetting the SgGlobal startOfConstruct and endOfConstruct");
     printf ("Resetting the SgGlobal startOfConstruct and endOfConstruct filename (p_sourceFileNameWithPath = %s) \n",p_sourceFileNameWithPath.c_str());
#endif

  // DQ (12/22/2008): Added to support CPP preprocessing of Fortran files.
     string filename = p_sourceFileNameWithPath;
     if (get_requires_C_preprocessor() == true)
        {
       // This must be a Fortran source file (requiring the use of CPP to process its directives.
          filename = generate_C_preprocessor_intermediate_filename(filename);
        }

  // printf ("get_requires_C_preprocessor() = %s filename = %s \n",get_requires_C_preprocessor() ? "true" : "false",filename.c_str());

  // get_globalScope()->get_startOfConstruct()->set_filenameString(p_sourceFileNameWithPath);
     get_globalScope()->get_startOfConstruct()->set_filenameString(filename);
     ROSE_ASSERT(get_globalScope()->get_startOfConstruct()->get_filenameString().empty() == false);

  // DQ (8/21/2008): Uncommented to make the endOfConstruct consistant (avoids warning in AST consistancy check).
  // get_globalScope()->get_endOfConstruct()->set_filenameString(p_sourceFileNameWithPath);
     get_globalScope()->get_endOfConstruct()->set_filenameString(filename);
     ROSE_ASSERT(get_globalScope()->get_endOfConstruct()->get_filenameString().empty() == false);     

#if 0
     printf ("DONE: Resetting the SgGlobal startOfConstruct and endOfConstruct filename (filename = %s) \n",filename.c_str());
     Sg_File_Info::display_static_data("DONE: Resetting the SgGlobal startOfConstruct and endOfConstruct");
#endif

  // DQ (12/23/2008): These should be in the Sg_File_Info map already.
     ROSE_ASSERT(Sg_File_Info::getIDFromFilename(get_file_info()->get_filename()) >= 0);
     if (get_requires_C_preprocessor() == true)
        {
          ROSE_ASSERT(Sg_File_Info::getIDFromFilename(generate_C_preprocessor_intermediate_filename(get_file_info()->get_filename())) >= 0);
        }
   }


SgFile* 
#if 0 //FMZ (07/07/2010): "nextErrorCode" should be call by reference argument
determineFileType ( vector<string> argv, int nextErrorCode, SgProject* project )
#else 
determineFileType ( vector<string> argv, int& nextErrorCode, SgProject* project )
#endif
   {
     SgFile* file = NULL;

  // DQ (2/4/2009): The specification of "-rose:binary" causes filenames to be interpreted 
  // differently if they are object files or libary archive files.
  // DQ (4/21/2006): New version of source file name handling (set the source file name early)
  // printf ("In determineFileType(): Calling CommandlineProcessing::generateSourceFilenames(argv) \n");
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv);
     ROSE_ASSERT(project != NULL);
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv,project->get_binary_only());

#if 0
  // this->display("In SgFile::setupSourceFilename()");
     printf ("In determineFileType(): listToString(argv) = %s \n",StringUtility::listToString(argv).c_str());
     printf ("In determineFileType(): listToString(fileList) = %s \n",StringUtility::listToString(fileList).c_str());
#endif

  // DQ (2/6/2009): This fails for the build function SageBuilder::buildFile(), so OK to comment it out.
  // DQ (12/23/2008): I think that we may be able to assert this is true, if so then we can simplify the code below.
     ROSE_ASSERT(fileList.empty() == false);

     if (fileList.empty() == false)
        {
       // Note that we always process one file at a time using EDG or the Fortran frontend.
          ROSE_ASSERT(fileList.size() == 1);

       // DQ (8/31/2006): Convert the source file to have a path if it does not already
       // p_sourceFileNameWithPath    = *(fileList.begin());
          string sourceFilename = *(fileList.begin());

       // printf ("Before conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());

       // sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename);
          sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename, true);

       // printf ("After conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());

       // This should be an absolute path
          string targetSubstring = "/";
       // if (sourceFilename.substr(0,targetSubstring.size()) != targetSubstring)
       //      printf ("@@@@@@@@@@@@@@@@@@@@ In SgFile::setupSourceFilename(int,char**): sourceFilename = %s @@@@@@@@@@@@@@@@@@@@\n",sourceFilename.c_str());
       // ROSE_ASSERT(sourceFilename.substr(0,targetSubstring.size()) == targetSubstring);

       // Rama: 12/06/06: Fixup for problem with file names.  
	    // Made changes to this file and string utilities function getAbsolutePathFromRelativePath by cloning it with name getAbsolutePathFromRelativePathWithErrors
	    // Also refer to script that tests -- reasonably exhaustively -- to various combinarions of input files.

          if (sourceFilename.substr(0,targetSubstring.size()) != targetSubstring)
               printf ("sourceFilename encountered an error in filename\n");

       // DQ (11/29/2006): Even if this is C mode, we have to define the __cplusplus macro 
       // if we detect we are processing a source file using a C++ filename extension.
          string filenameExtension = StringUtility::fileNameSuffix(sourceFilename);

       // printf ("filenameExtension = %s \n",filenameExtension.c_str());
       // ROSE_ASSERT(false);

       // DQ (5/18/2008): Set this to true (redundant, since the default already specified as true)
       // file->set_requires_C_preprocessor(true);

       // DQ (11/17/2007): Mark this as a file using a Fortran file extension (else this turns off options down stream).
          if (CommandlineProcessing::isFortranFileNameSuffix(filenameExtension) == true)
             {
               SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
               file = sourceFile;

            // printf ("----------- Great location to set the sourceFilename = %s \n",sourceFilename.c_str());

            // DQ (12/23/2008): Moved initialization of source position (call to initializeSourcePosition()) 
            // to earliest position in setup of SgFile.

            // printf ("Calling file->set_sourceFileUsesFortranFileExtension(true) \n");
               file->set_sourceFileUsesFortranFileExtension(true);

            // Use the filename suffix as a default means to set this value
               file->set_outputLanguage(SgFile::e_Fortran_output_language);

               file->set_Fortran_only(true);

            // DQ (5/18/2008): Set this to true (redundant, since the default already specified as true).
            // DQ (12/23/2008): Actually this is not redundant since the SgFile::initialization sets it to "false".
            // Note: This is a little bit inconsistnat with the default set in ROSETTA.
               file->set_requires_C_preprocessor(CommandlineProcessing::isFortranFileNameSuffixRequiringCPP(filenameExtension));

            // printf ("Called set_requires_C_preprocessor(%s) \n",file->get_requires_C_preprocessor() ? "true" : "false");

            // DQ (12/23/2008): This needs to be called after the set_requires_C_preprocessor() function is called.
            // If CPP processing is required then the global scope should have a source position using the intermediate
            // file name (generated by generate_C_preprocessor_intermediate_filename()).
               sourceFile->initializeGlobalScope();

            // Now set the specific types of Fortran file extensions
               if (CommandlineProcessing::isFortran77FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran77FileExtension(true) \n");
                    file->set_sourceFileUsesFortran77FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_fixed_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_fixed_form_output_format);

                    file->set_F77_only(true);
                  }

               if (CommandlineProcessing::isFortran90FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran90FileExtension(true) \n");
                    file->set_sourceFileUsesFortran90FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                    file->set_F90_only(true);
                  }

               if (CommandlineProcessing::isFortran95FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran95FileExtension(true) \n");
                    file->set_sourceFileUsesFortran95FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                    file->set_F95_only(true);
                  }

               if (CommandlineProcessing::isFortran2003FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran2003FileExtension(true) \n");
                    file->set_sourceFileUsesFortran2003FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                    file->set_F2003_only(true);
                  }

               if (CommandlineProcessing::isCoArrayFortranFileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling file->set_sourceFileUsesFortran2003FileExtension(true) \n");
                    file->set_sourceFileUsesCoArrayFortranFileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);

                 // DQ (1/23/2009): I think that since CAF is an extension of F2003, we want to mark this as F2003 as well.
                    file->set_F2003_only(true);
                    file->set_CoArrayFortran_only(true);
                  }

               if (CommandlineProcessing::isFortran2008FileNameSuffix(filenameExtension) == true)
                  {
                    printf ("Sorry, Fortran 2008 specific support is not yet implemented in ROSE ... \n");
                    ROSE_ASSERT(false);

                 // This is not yet supported.
                 // file->set_sourceFileUsesFortran2008FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    file->set_outputFormat(SgFile::e_free_form_output_format);
                    file->set_backendCompileFormat(SgFile::e_free_form_output_format);
                  }
             }
            else
             {
               if (CommandlineProcessing::isPHPFileNameSuffix(filenameExtension) == true)
                  {
                 // file = new SgSourceFile ( argv,  project );
                    SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                    file = sourceFile;

                    file->set_sourceFileUsesPHPFileExtension(true);

                    file->set_outputLanguage(SgFile::e_PHP_output_language);

                    file->set_PHP_only(true);

                 // DQ (12/23/2008): We don't handle CPP directives and comments for PHP yet.
                 // file->get_skip_commentsAndDirectives(true);

                 // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                 // Note that file->get_requires_C_preprocessor() should be false.
                    ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                    sourceFile->initializeGlobalScope();
                  }
                 else
                  {
                 // printf ("Calling file->set_sourceFileUsesFortranFileExtension(false) \n");
               
                 // if (StringUtility::isCppFileNameSuffix(filenameExtension) == true)
                    if (CommandlineProcessing::isCppFileNameSuffix(filenameExtension) == true)
                       {
                      // file = new SgSourceFile ( argv,  project );
                         SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                         file = sourceFile;

                      // This is a C++ file (so define __cplusplus, just like GNU gcc would)
                      // file->set_requires_cplusplus_macro(true);
                         file->set_sourceFileUsesCppFileExtension(true);

                      // Use the filename suffix as a default means to set this value
                         file->set_outputLanguage(SgFile::e_Cxx_output_language);

                         file->set_Cxx_only(true);

                      // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                      // Note that file->get_requires_C_preprocessor() should be false.
                         ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                         sourceFile->initializeGlobalScope();
                       }
                      else
                       {
                      // Liao, 6/6/2008, Assume AST with UPC will be unparsed using the C unparser
                         if ( ( CommandlineProcessing::isCFileNameSuffix(filenameExtension)   == true ) ||
                              ( CommandlineProcessing::isUPCFileNameSuffix(filenameExtension) == true ) )
                            {
                           // file = new SgSourceFile ( argv,  project );
                              SgSourceFile* sourceFile = new SgSourceFile ( argv,  project );
                              file = sourceFile;

                           // This a not a C++ file (assume it is a C file and don't define the __cplusplus macro, just like GNU gcc would)
                              file->set_sourceFileUsesCppFileExtension(false);

                           // Use the filename suffix as a default means to set this value
                              file->set_outputLanguage(SgFile::e_C_output_language);

                              file->set_C_only(true);

                           // Liao 6/6/2008  Set the newly introduced p_UPC_only flag.
                              if (CommandlineProcessing::isUPCFileNameSuffix(filenameExtension) == true)
                                   file->set_UPC_only(true);

                           // DQ (12/23/2008): This is the eariliest point where the global scope can be set.
                           // Note that file->get_requires_C_preprocessor() should be false.
                              ROSE_ASSERT(file->get_requires_C_preprocessor() == false);
                              sourceFile->initializeGlobalScope();
                            }
                           else
                              if ( CommandlineProcessing::isCudaFileNameSuffix(filenameExtension)   == true )
                                   file->set_Cuda_only(true);
                              else if ( CommandlineProcessing::isOpenCLFileNameSuffix(filenameExtension)   == true )
                                   file->set_OpenCL_only(true);
                              else
                            {
                           // This is not a source file recognized by ROSE, so it is either a binary executable or library archive or something that we can't process.

                           // printf ("This still might be a binary file (can not be an object file, since these are not accepted into the fileList by CommandlineProcessing::generateSourceFilenames()) \n");

                           // Detect if this is a binary (executable) file!
                              bool isBinaryExecutable = isBinaryExecutableFile(sourceFilename);
                              bool isLibraryArchive   = isLibraryArchiveFile(sourceFilename);

                           // If -rose:binary was specified and the relatively simple-minded checks of isBinaryExecutableFile()
                           // and isLibararyArchiveFile() both failed to detect anything, then assume this is an executable.
                              if (!isBinaryExecutable && !isLibraryArchive)
                                  isBinaryExecutable = true;
                              
                           // printf ("isBinaryExecutable = %s isLibraryArchive = %s \n",isBinaryExecutable ? "true" : "false",isLibraryArchive ? "true" : "false");
                              if (isBinaryExecutable == true || isLibraryArchive == true)
                                 {
                                // Build a SgBinaryComposite to represent either the binary executable or the library archive.
                                   SgBinaryComposite* binary = new SgBinaryComposite ( argv,  project );
                                   file = binary;

                                // This should have already been setup!
                                // file->initializeSourcePosition();

                                   file->set_sourceFileUsesBinaryFileExtension(true);

                                // If this is an object file being processed for binary analysis then mark it as an object 
                                // file so that we can trigger analysis to mar the sections that will be disassembled.
                                   string binaryFileName = file->get_sourceFileNameWithPath();
                                   if (CommandlineProcessing::isObjectFilename(binaryFileName) == true)
                                      {
                                        file->set_isObjectFile(true);
                                      }

                                // DQ (2/5/2009): Put this at both the SgProject and SgFile levels.
                                // DQ (2/4/2009):  This is now a data member on the SgProject instead of on the SgFile.
                                   file->set_binary_only(true);

                                // DQ (5/18/2008): Set this to false (since binaries are never preprocessed using the C preprocessor).
                                   file->set_requires_C_preprocessor(false);

                                   ROSE_ASSERT(file->get_file_info() != NULL);

                                   if (isLibraryArchive == true)
                                      {
                                     // This is the case of processing a library archive (*.a) file. We want to process these files so that 
                                     // we can test the library identification mechanism to build databases of the binary functions in 
                                     // libraries (so that we detect these in staticaly linked binaries).
                                        ROSE_ASSERT(isBinaryExecutable == false);

                                     // Note that since a archive can contain many *.o files each of these will be a SgAsmGenericFile object and 
                                     // the SgBinaryComposite will contain a list of SgAsmGenericFile objects to hold them all.
                                        string archiveName = file->get_sourceFileNameWithPath();

                                        printf ("archiveName = %s \n",archiveName.c_str());

                                     // Mark this as a library archive.
                                        file->set_isLibraryArchive(true);

                                     // Later we can make the tmp file specific to a given archive if we want to do that.
                                     // string commandLine = "mkdir -p tmp_objects; cd tmp_objects; ar -vox " + archiveName;

                                     // Put the names of the extracted objects into a file and the extracted object file into the directory: tmp_objects
                                        string objectNameFile = "object_names.txt";
                                        string commandLine = "mkdir -p tmp_objects; cd tmp_objects; ar -vox " + archiveName + " > ../object_names.txt";
                                        printf ("Running System Command: %s \n",commandLine.c_str());

                                     // Run the system command...
                                        system(commandLine.c_str());

                                        vector<string> wordList = StringUtility::readWordsInFile(objectNameFile);
                                        vector<string> objectFileList;

                                        for (vector<string>::iterator i = wordList.begin(); i != wordList.end(); i++)
                                           {
                                          // Get each word in the file of names (*.o)
                                             string word = *i;
                                          // printf ("word = %s \n",word.c_str());
                                             size_t wordSize = word.length();
                                             string targetSuffix = ".o";
                                             size_t targetSuffixSize = targetSuffix.length();
                                             if (wordSize > targetSuffixSize && word.substr(wordSize-targetSuffixSize) == targetSuffix)
                                                  objectFileList.push_back(word);
                                           }

                                        for (vector<string>::iterator i = objectFileList.begin(); i != objectFileList.end(); i++)
                                           {
                                          // Get each object file name (*.o)
                                             string objectFileName = *i;
                                             printf ("objectFileName = %s \n",objectFileName.c_str());
                                             binary->get_libraryArchiveObjectFileNameList().push_back(objectFileName);
                                             printf ("binary->get_libraryArchiveObjectFileNameList().size() = %zu \n",binary->get_libraryArchiveObjectFileNameList().size());
                                           }
#if 0
                                        printf ("Exiting in processing a library archive file. \n");
                                     // ROSE_ASSERT(false);
#endif
                                      }
#if 0
                                   printf ("Processed as a binary file! \n");
#endif
                                 }
                                else
                                 {
                                   file = new SgUnknownFile ( argv,  project );

                                // This should have already been setup!
                                // file->initializeSourcePosition();

                                   ROSE_ASSERT(file->get_parent() != NULL);
                                   ROSE_ASSERT(file->get_parent() == project);

                                // If all else fails, then output the type of file and exit.
                                   file->set_sourceFileTypeIsUnknown(true);
                                   file->set_requires_C_preprocessor(false);

                                   ROSE_ASSERT(file->get_file_info() != NULL);
                                // file->set_parent(project);

                                // DQ (2/3/2009): Uncommented this to report the file type when we don't process it...
                                // outputTypeOfFileAndExit(sourceFilename);
                                   printf ("Warning: This is an unknown file type, not being processed by ROSE \n");
                                   outputTypeOfFileAndExit(sourceFilename);
                                 }
                            }
                       }
                  }

               file->set_sourceFileUsesFortranFileExtension(false);
             }
        }
       else
        {
       // DQ (2/6/2009): This case is used by the build function SageBuilder::buildFile().

#if 1
       // DQ (12/22/2008): Make any error message from this branch more clear for debugging!
       // AS Is this option possible?
          printf ("Is this branch reachable? \n");
          ROSE_ASSERT(false);
       // abort();

       // ROSE_ASSERT (p_numberOfSourceFileNames == 0);
          ROSE_ASSERT (file->get_sourceFileNameWithPath().empty() == true);

       // If no source code file name was found then likely this is:
       //   1) a link command, or
       //   2) called as part of the SageBuilder::buildFile()
       // using the C++ compiler.  In this case skip the EDG processing.
          file->set_disable_edg_backend(true);
#endif
       // printf ("No source file found on command line, assuming to be linker command line \n");
        }

  // DQ (2/6/2009): Can use this assertion with the build function SageBuilder::buildFile().
  // DQ (2/3/2009): I think this is a new assertion!
  // ROSE_ASSERT(file != NULL);
  // file->display("SgFile* determineFileType(): before calling file->callFrontEnd()");

  // The frontend is called explicitly outside the constructor since that allows for a cleaner
  // control flow. The callFrontEnd() relies on all the "set_" flags to be already called therefore
  // it was placed here.
  // if ( isSgUnknownFile(file) == NULL && file != NULL  )
     if ( file != NULL && isSgUnknownFile(file) == NULL )
        {
       // printf ("Calling file->callFrontEnd() \n");
          nextErrorCode = file->callFrontEnd();
       // printf ("DONE: Calling file->callFrontEnd() \n");
          ROSE_ASSERT ( nextErrorCode <= 3);
        }

  // Keep the filename stored in the Sg_File_Info consistant.  Later we will want to remove this redundency
  // The reason we have the Sg_File_Info object is so that we can easily support filename matching based on
  // the integer values instead of string comparisions.  Required for the handling co CPP directives and comments.

#if 0
     if (file != NULL)
        {
          printf ("Calling file->display() \n");
          file->display("SgFile* determineFileType()");
        }
#endif

  // printf ("Leaving determineFileType() \n");

     return file;
   }

 
static void makeSysIncludeList(const Rose_STL_Container<string>& dirs, Rose_STL_Container<string>& result)
   {
     string includeBase = findRoseSupportPathFromBuild("include-staging", "include");
     for (Rose_STL_Container<string>::const_iterator i = dirs.begin(); i != dirs.end(); ++i)
        {
          ROSE_ASSERT (!i->empty());
          string fullPath = (*i)[0] == '/' ? *i : (includeBase + "/" + *i);
          result.push_back("--sys_include");
          result.push_back(fullPath);
        }
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

  // Old information recorded here (but not useful any more):
  // Complete Command line required to run ROSE Preprocessor (old version for SUN CC 4.2
  // compiler worked out by Kei Davis at LANL):
  //    ../src/rose -D__CPLUSPLUS -D__STDC__=0 -Dsun -Dsparc -Dunix -D__KCC -D__sun__ 
  //         -D__sparc__ -D__unix__ -D_parc -D__unix -D__SVR4 -D_NO_LONGLONG 
  //         -I/opt/SUNWspro/SC4.2/include/CC/ -I$APlusPlus/include test1.C

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

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

  // JJW (12/11/2008): Change all of this to use vectors of strings, and add
  // --edg_base_dir as a new ROSE-set flag
     vector<string> commandLine;

#ifdef ROSE_USE_NEW_EDG_INTERFACE

  // Note that the new EDG/Sage interface does not require a generated set of header files specific to ROSE.
     commandLine.push_back("--edg_base_dir");

  // DQ (12/29/2008): Added support for EDG version 4.0 (constains design changes that break a number of things in the pre-version 4.0 work)
#ifdef ROSE_USE_EDG_VERSION_4
  // DQ (2/1/2010): I think this needs to reference the source tree (to pickup src/frontend/CxxFrontend/EDG/EDG_4.0/lib/predefined_macros.txt).
  // DQ (12/21/2009): The locaion of the EDG directory has been changed now that it is a submodule in our git repository.
  // commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG_4.0/lib", "share"));
  // commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG/EDG_4.0/lib", "share"));
     commandLine.push_back(findRoseSupportPathFromSource("src/frontend/CxxFrontend/EDG/EDG_4.0/lib", "share"));
#else
  // DQ (2/1/2010): I think this needs to reference the source tree (to pickup src/frontend/CxxFrontend/EDG/EDG_4.0/lib/predefined_macros.txt).
  // DQ (12/21/2009): The locaion of the EDG directory has been changed now that it is a submodule in our git repository.
  // commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG_3.10/lib", "share"));
     commandLine.push_back(findRoseSupportPathFromBuild("src/frontend/CxxFrontend/EDG/EDG_3.10/lib", "share"));
  // commandLine.push_back(findRoseSupportPathFromSource("src/frontend/CxxFrontend/EDG/EDG_3.10/lib", "share"));
#endif
#endif

  // display("Called from SgFile::build_EDG_CommandLine");

  // AS (03/08/2006) Added support for g++ preincludes
  // Rose_STL_Container<std::string> listOfPreincludes;

#if 0
  // This functionality has been moved to before source name extraction since the 
  // -include file will be extracted as a file and treated as a source file name 
  // and the -include will not have an option.

  // DQ (12/1/2006): Code added by Andreas (07/03/06) and moved to a new position 
  // so that we could modify the string input from CXX_SPEC_DEF (configDefs).
     string preinclude_string_target = "-include";
     for (unsigned int i=1; i < argv.size(); i++)
        {
       // AS (070306) Handle g++ --include directives
          std::string stack_arg(argv[i]);
       // std::cout << "stack arg is: " << stack_arg << std::endl;
          if( stack_arg.find(preinclude_string_target) <= 2)
             {
               i++;
               ROSE_ASSERT(i<argv.size());
               string currentArgument(argv[i]);

            // Note that many new style C++ header files don't have a ".h" suffix
               string headerSuffix = ".h";

               int jlength = headerSuffix.size();
               int length = currentArgument.size();
               ROSE_ASSERT( length > jlength);
               ROSE_ASSERT( ( currentArgument.compare(length - jlength, jlength, headerSuffix) == 0 ) || CommandlineProcessing::isSourceFilename(currentArgument));

               string sourceFilePath = StringUtility::getPathFromFileName(currentArgument);
               currentArgument = StringUtility::stripPathFromFileName(currentArgument);
               if (sourceFilePath == "" )
                    sourceFilePath = "./";
               sourceFilePath = StringUtility::getAbsolutePathFromRelativePath(sourceFilePath);
               currentArgument = sourceFilePath+"/"+currentArgument;

            // std::cout << "Found preinclude : " << currentArgument << std::endl;

               commandLine.push_back("--preinclude");
               commandLine.push_back(currentArgument);
             }
        }
#else
     SgProject* project = isSgProject(this->get_parent());
     ROSE_ASSERT (project != NULL);

  // DQ (1/13/2009): The preincludeFileList was built if the -include <file> option was used
     for (SgStringList::iterator i = project->get_preincludeFileList().begin(); i != project->get_preincludeFileList().end(); i++)
        {
       // Build the preinclude file list
          ROSE_ASSERT(project->get_preincludeFileList().empty() == false);

       // printf ("Building commandline: --preinclude %s \n",(*i).c_str());
          commandLine.push_back("--preinclude");
          commandLine.push_back(*i);
        }
#endif

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
#if 1
               includeDirectorySpecifier = StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
#endif               
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

//AS (2/22/08): GCC looks for system headers in '-I' first. We need to support this. 
//PC (10/20/2009): This code was moved from SgProject as it is file-specific (required by AST merge)
     for (vector<string>::iterator i = includePaths.begin(); i != includePaths.end(); ++i)
        {
          commandLine.push_back("--sys_include");
          commandLine.push_back(*i);
        }

     for (SgStringList::iterator i = project->get_preincludeDirectoryList().begin(); i != project->get_preincludeDirectoryList().end(); i++)
        {
       // Build the preinclude directory list
       // printf ("Building commandline: --sys_include %s \n",(*i).c_str());
          commandLine.push_back("--sys_include");
          commandLine.push_back(*i);
        }
#endif

     commandLine.insert(commandLine.end(), configDefs.begin(), configDefs.end());

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
     
     if (enable_cuda || enable_opencl) {
	makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
     	if (enable_cuda && !enable_opencl) {
     		commandLine.push_back("-DROSE_LANGUAGE_MODE=2");
     	}
     	else if (enable_opencl && !enable_cuda) {
     		commandLine.push_back("-DROSE_LANGUAGE_MODE=3");
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
                              makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
                            }
                           else
                            {
                              makeSysIncludeList(Cxx_ConfigIncludeDirs, commandLine);
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
                         makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
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
                         makeSysIncludeList(C_ConfigIncludeDirs, commandLine);
                       }
                      else
                       {
                         makeSysIncludeList(Cxx_ConfigIncludeDirs, commandLine);
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
     inputCommandLine.insert(inputCommandLine.begin(), "dummy_argv0_for_edg");

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
          set_skipfinalCompileStep(true);
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


  // Liao, 6/20/2008, handle UPC specific EDG options.
  // Generate --upc 
    if (get_UPC_only()) 
    {
       inputCommandLine.push_back("--upc");
       inputCommandLine.push_back("--restrict");
    }  


  // Generate --upc_threads n 
     int intOptionUpcThreads = get_upc_threads();  
     if (intOptionUpcThreads>0) 
     { 
       stringstream ss;
       ss<<intOptionUpcThreads;
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

#if 0
     printf ("Exiting at base of build_EDG_CommandLine() \n");
     ROSE_ABORT();
#endif
      // display("at base of build_EDG_CommandLine()");

   }


#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
//FMZ(5/19/2008):
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
extern void jserver_init();
extern void jserver_finish();
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif

//! internal function to invoke the EDG frontend and generate the AST
int
SgProject::parse(const vector<string>& argv)
   {
  // Not sure that if we are just linking that we should call a function called "parse()"!!!

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST (SgProject::parse(argc,argv)):");

#if 0
     printf ("Inside of SgProject::parse(const vector<string>& argv) \n");
#endif

  // builds file list (or none if this is a link line)
	  processCommandLine(argv);

     int errorCode = 0;

  // DQ (7/7/2005): Added support for AST Merge Mechanism
     if (p_astMerge == true)
        {
       // If astMerge is specified, then the command file is accessed to execute all 
       // the commands from each of the associated working directories.  Each new AST 
       // in merged with the previous AST.

          if (p_astMergeCommandFile != "")
             {
            // If using astMerge mechanism we have to save the command line and 
            // working directories to a separate file.  This permits a makefile to
            // call a ROSE translator repeatedly and the command line for each 
            // file be saved.
               errorCode = AstMergeSupport(this);
             }
            else
             {
            // DQ (5/26/2007): This case could make sense, if there were more than
            // one file on the command line (or if we wanted to force a single file
            // to share as much as possible in a merge with itself, there is a 
            // typical 20% reduction in memory useage for this case since the 
            // types are then better shared than is possible during initial construction 
            // of the AST).
#if 0
            // error case
               printf ("astMerge requires specification of a command file \n");
               ROSE_ASSERT(false);
               errorCode = -1;
#endif
               errorCode = AstMergeSupport(this);
             }
        }
       else
        {
       // DQ (7/7/2005): Specification of the AST merge command filename triggers accumulation 
       // of working directories and commandlines into the specified file (no other processing 
       // is done, the AST (beyond the SgProject) is not built). 
          if (p_astMergeCommandFile != "")
             {
            // If using astMerge mechanism we have to save the command line and 
            // working directories to a separate file.

            // DQ (5/26/2007): This might be a problem where object files are required to be built
            // and so we might have to call the backend compiler as a way of forcing the correct
            // object files to be built so that, for example, libraries can be constructed when
            // operating across multiple directories.

               errorCode = buildAstMergeCommandFile(this);
             }
            else
             {
            // Normal case without AST Merge: Compiling ...
            // printf ("In SgProject::parse(const vector<string>& argv): get_sourceFileNameList().size() = %zu \n",get_sourceFileNameList().size());
               if (get_sourceFileNameList().size() > 0)
                  {
                 // This is a compile line
                 // printf ("Calling parse() from SgProject::parse(const vector<string>& argv) \n");


                  /*
                   * FMZ (5/19/2008)
                   *   "jserver_init()"   does nothing. The Java VM will be loaded at the first time
                   *                      it needed (i.e for parsing the 1st fortran file).
                   *   "jserver_finish()" will dostroy the Java VM if it is running.
                   */

                    if (SgProject::get_verbose() > 1)
                       {
                         printf ("Calling Open Fortran Parser: jserver_init() \n");
                       }
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
                    jserver_init();
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif
                    errorCode = parse();

                    // FMZ deleteComm jserver_finish();
                  }

            // DQ (5/26/2007): This is meaningless, so remove it!
            // errorCode = errorCode;
             }
        }

#if 1
  // DQ (8/22/2009): We test the parent of SgFunctionTypeTable in the AST post processing,
  // so we need to make sure that it is set.
     SgFunctionTypeTable* functionTypeTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(functionTypeTable != NULL);
     if (functionTypeTable->get_parent() == NULL)
        {
#if 0
          printf ("This (globalFunctionTypeTable) should have been set to point to the SgProject not the SgFile \n");
          ROSE_ASSERT(false);
#endif
       // ROSE_ASSERT(numberOfFiles() > 0);
       // printf ("Inside of SgProject::parse(const vector<string>& argv): set the parent of SgFunctionTypeTable \n");
          if (numberOfFiles() > 0)
               functionTypeTable->set_parent(&(get_file(0)));
            else
               functionTypeTable->set_parent(this);
        }
     ROSE_ASSERT(functionTypeTable->get_parent() != NULL);

     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_parent() != NULL);
#endif

#if 1
  // DQ (7/25/2010): We test the parent of SgTypeTable in the AST post processing,
  // so we need to make sure that it is set.
     SgTypeTable* typeTable = SgNode::get_globalTypeTable();
     ROSE_ASSERT(typeTable != NULL);
     if (typeTable->get_parent() == NULL)
        {
#if 0
          printf ("This (globalTypeTable) should have been set to point to the SgProject not the SgFile \n");
          ROSE_ASSERT(false);
#endif
       // ROSE_ASSERT(numberOfFiles() > 0);
       // printf ("Inside of SgProject::parse(const vector<string>& argv): set the parent of SgTypeTable \n");
          if (numberOfFiles() > 0)
               typeTable->set_parent(&(get_file(0)));
            else
               typeTable->set_parent(this);
        }
     ROSE_ASSERT(typeTable->get_parent() != NULL);

  // DQ (7/30/2010): This test fails in tests/CompilerOptionsTests/testCpreprocessorOption
  // DQ (7/25/2010): Added new test.
  // printf ("typeTable->get_parent()->class_name() = %s \n",typeTable->get_parent()->class_name().c_str());
  // ROSE_ASSERT(isSgProject(typeTable->get_parent()) != NULL);

     ROSE_ASSERT(SgNode::get_globalTypeTable() != NULL);
     ROSE_ASSERT(SgNode::get_globalTypeTable()->get_parent() != NULL);
#endif

     return errorCode;
   }


SgSourceFile::SgSourceFile ( vector<string> & argv , SgProject* project )
// : SgFile (argv,errorCode,fileNameIndex,project)
   {
  // printf ("In the SgSourceFile constructor \n");

     set_globalScope(NULL);

  // This constructor actually makes the call to EDG/OFP to build the AST (via callFrontEnd()).
  // printf ("In SgSourceFile::SgSourceFile(): Calling doSetupForConstructor() \n");
     doSetupForConstructor(argv,  project);
    }

#if 0
SgSourceFile::SgSourceFile ( vector<string> & argv , int & errorCode, int fileNameIndex, SgProject* project )
// : SgFile (argv,errorCode,fileNameIndex,project)
   {
  // printf ("In the SgSourceFile constructor \n");

     set_globalScope(NULL);

  // This constructor actually makes the call to EDG to build the AST (via callFrontEnd()).
     doSetupForConstructor(argv, errorCode, fileNameIndex, project);

    }
#endif

int
SgSourceFile::callFrontEnd()
   {
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT     
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
     FortranParserState* currStks = new FortranParserState(); 
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif

     int frontendErrorLevel = SgFile::callFrontEnd();
  // DQ (1/21/2008): This must be set for all languages
     ROSE_ASSERT(get_globalScope() != NULL);
     ROSE_ASSERT(get_globalScope()->get_file_info() != NULL);
     ROSE_ASSERT(get_globalScope()->get_file_info()->get_filenameString().empty() == false);
  // printf ("p_root->get_file_info()->get_filenameString() = %s \n",p_root->get_file_info()->get_filenameString().c_str());

  // DQ (8/21/2008): Added assertion.
     ROSE_ASSERT (get_globalScope()->get_startOfConstruct() != NULL);
     ROSE_ASSERT (get_globalScope()->get_endOfConstruct()   != NULL);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT 
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
     delete  currStks ;
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif
     return frontendErrorLevel;
   }

int
SgBinaryComposite::callFrontEnd()
   {
     int frontendErrorLevel = SgFile::callFrontEnd();
  // DQ (1/21/2008): This must be set for all languages
     return frontendErrorLevel;
   }

int
SgUnknownFile::callFrontEnd()
   {
  // DQ (2/3/2009): This function is defined, but should never be called.
     printf ("Error: calling SgUnknownFile::callFrontEnd() \n");
     ROSE_ASSERT(false);

     return 0;
   }

SgBinaryComposite::SgBinaryComposite ( vector<string> & argv ,  SgProject* project )
    : p_genericFileList(NULL), p_interpretations(NULL)
{
    p_interpretations = new SgAsmInterpretationList();
    p_genericFileList = new SgAsmGenericFileList();
    
  // DQ (2/3/2009): This data member has disappeared (in favor of a list).
  // p_binaryFile = NULL;

  // printf ("In the SgBinaryComposite constructor \n");

  // This constructor actually makes the call to EDG to build the AST (via callFrontEnd()).
  // printf ("In SgBinaryComposite::SgBinaryComposite(): Calling doSetupForConstructor() \n");
     doSetupForConstructor(argv,  project);

  // printf ("Leaving SgBinaryComposite constructor \n");
}


int 
SgProject::parse()
   {
     int errorCode = 0;

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST (SgProject::parse()):");

  // ROSE_ASSERT (p_fileList != NULL);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // FMZ (5/29/2008)
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT

     FortranModuleInfo::setCurrentProject(this);
     FortranModuleInfo::set_inputDirs(this );

#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif

  // Simplify multi-file handling so that a single file is just the trivial 
  // case and not a special separate case.
#if 0
     printf ("Loop through the source files on the command line! p_sourceFileNameList = %zu \n",p_sourceFileNameList.size());
#endif

     Rose_STL_Container<string>::iterator nameIterator = p_sourceFileNameList.begin();
     unsigned int i = 0;
     while (nameIterator != p_sourceFileNameList.end())
        {
#if 0
          printf ("Build a SgFile object for file #%d \n",i);
#endif
          int nextErrorCode = 0;

       // DQ (4/20/2006): Exclude other files from list in argc and argv
          vector<string> argv = get_originalCommandLineArgumentList();
          string currentFileName = *nameIterator;
#if 0
          printf ("In SgProject::parse(): before removeAllFileNamesExcept() file = %s argv = %s \n",
               currentFileName.c_str(),CommandlineProcessing::generateStringFromArgList(argv,false,false).c_str());
#endif
          CommandlineProcessing::removeAllFileNamesExcept(argv,p_sourceFileNameList,currentFileName);
#if 0
          printf ("In SgProject::parse(): after removeAllFileNamesExcept() from command line for file = %s argv = %s \n",
               currentFileName.c_str(),CommandlineProcessing::generateStringFromArgList(argv,false,false).c_str());
          printf ("currentFileName = %s \n",currentFileName.c_str());
#endif
       // DQ (11/13/2008): Removed overly complex logic here!
#if 0
          printf ("+++++++++++++++ Calling determineFileType() currentFileName = %s \n",currentFileName.c_str());
#endif
          SgFile* newFile = determineFileType(argv, nextErrorCode, this);
          ROSE_ASSERT (newFile != NULL);
#if 0
          printf ("+++++++++++++++ DONE: Calling determineFileType() currentFileName = %s \n",currentFileName.c_str());
          printf ("In SgProject::parse(): newFile = %p = %s \n",newFile,newFile->class_name().c_str());
#endif
          ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);
          ROSE_ASSERT (newFile->get_parent() != NULL);

       // DQ (9/2/2008): This should have already been set!
       // Set the parent explicitly (so that we can easily find the SgProject from the SgFile).
       // newFile->set_parent(this);

       // This just adds the new file to the list of files stored internally
          set_file ( *newFile );

       // newFile->display("Called from SgProject::parse()");

#if 0
          printf ("In Project::parse(): get_file(%d).get_skipfinalCompileStep() = %s \n",i,(get_file(i).get_skipfinalCompileStep()) ? "true" : "false");
#endif

       // errorCode = (errorCode >= nextErrorCode) ? errorCode : nextErrorCode; // use STL max
          errorCode = max(errorCode,nextErrorCode); // use STL max

          nameIterator++;
          i++;
        }

  // printf ("Inside of SgProject::parse() before AstPostProcessing() \n");

  // GB (8/19/2009): Moved the AstPostProcessing call from
  // SgFile::callFrontEnd to this point. Thus, it is only called once for
  // the whole project rather than once per file. Repeated calls to
  // AstPostProcessing are slow due to repeated memory pool traversals. The
  // AstPostProcessing is only to be called if there are input files to run
  // it on, and they are meant to be used in some way other than just
  // calling the backend on them. (If only the backend is used, this was
  // never called by SgFile::callFrontEnd either.)
  // if ( !get_fileList().empty() && !get_useBackendOnly() )
     if ( (get_fileList().empty() == false) && (get_useBackendOnly() == false) )
        {
          AstPostProcessing(this);
        }
#if 0
       else
        {
       // Alternatively if this is a part of binary analysis then process via AstPostProcessing().
          if (this->get_binary_only() == true)
             {
               AstPostProcessing(this);
             }
        }
#endif

  // GB (9/4/2009): Moved the secondary pass over source files (which
  // attaches the preprocessing information) to this point. This way, the
  // secondary pass over each file runs after all fixes have been done. This
  // is relevant where the AstPostProcessing mechanism must first mark nodes
  // to be output before preprocessing information is attached.
     SgFilePtrList &files = get_fileList();
     SgFilePtrList::iterator fIterator;
     for (fIterator = files.begin(); fIterator != files.end(); ++fIterator)
        {
          SgFile *file = *fIterator;
          ROSE_ASSERT(file != NULL);
          file->secondaryPassOverSourceFile();
        }

     if ( get_verbose() > 0 )
        {
       // Report the error code if it is non-zero (but only in verbose mode)
          if (errorCode > 0)
             {
               printf ("Frontend Warnings only: errorCode = %d \n",errorCode);
               if (errorCode > 3)
                  {
                    printf ("Frontend Errors found: errorCode = %d \n",errorCode);
                  }
             }
        }

  // warnings from EDG processing are OK but not errors
     ROSE_ASSERT (errorCode <= 3);

  // if (get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= 1 )
          cout << "C++ source(s) parsed. AST generated." << endl;

     if ( get_verbose() > 3 )
        {
          printf ("In SgProject::parse() (verbose mode ON): \n");
          display ("In SgProject::parse()");
        }

  // DQ (5/22/2007): Moved to astPostProcessing
  // DQ (5/8/2007): Now build the hidden lists for types and declarations (Robert Preissl's work)
  // buildHiddenTypeAndDeclarationLists(this);

     return errorCode;
   }


void
SgSourceFile::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
  // Call the base class implementation!
     SgFile::doSetupForConstructor(argv, project);
   }

void
SgBinaryComposite::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
     SgFile::doSetupForConstructor(argv, project);
   }

void
SgUnknownFile::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
     SgFile::doSetupForConstructor(argv, project);
   }

void
SgFile::doSetupForConstructor(const vector<string>& argv, SgProject* project)
   {
  // JJW 10-26-2007 ensure that this object is not on the stack
     preventConstructionOnStack(this);

  // printf ("!!!!!!!!!!!!!!!!!! Inside of SgFile::doSetupForConstructor() !!!!!!!!!!!!!!! \n");

  // Set the project early in the construction phase so that we can access data in 
  // the parent if needed (useful for template handling but also makes sure the parent is
  // set (and avoids fixup (currently done, but too late in the construction process for 
  // the template support).
     if (project != NULL)
          set_parent(project);

     ROSE_ASSERT(project != NULL);
     ROSE_ASSERT(get_parent() != NULL);

  // initalize all local variables to default values
     initialization();

     ROSE_ASSERT(get_parent() != NULL);

  // DQ (2/4/2009): The specification of "-rose:binary" causes filenames to be interpreted 
  // differently if they are object files or libary archive files.
  // DQ (4/21/2006): Setup the source filename as early as possible
  // setupSourceFilename(argv);
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv);
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv,get_binary_only());
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv,project->get_binary_only());

  // DQ (12/23/2008): Use of this assertion will simplify the code below!
     ROSE_ASSERT (fileList.empty() == false);
     string sourceFilename = *(fileList.begin());

  // printf ("Before conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());
  // sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename);
     sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename, true);

     set_sourceFileNameWithPath(sourceFilename);

  // printf ("In SgFile::setupSourceFilename(const vector<string>& argv): p_sourceFileNameWithPath = %s \n",get_sourceFileNameWithPath().c_str());
//tps: 08/18/2010, This should call StringUtility for WINDOWS- there are two implementations of this?
//     set_sourceFileNameWithoutPath( ROSE::stripPathFromFileName(get_sourceFileNameWithPath().c_str()) );
     set_sourceFileNameWithoutPath( StringUtility::stripPathFromFileName(get_sourceFileNameWithPath().c_str()) );

#if 1
     initializeSourcePosition(sourceFilename);
     ROSE_ASSERT(get_file_info() != NULL);

  // printf ("In SgFile::doSetupForConstructor(): source position set for sourceFilename = %s \n",sourceFilename.c_str());
#else
     ROSE_ASSERT(get_file_info() != NULL);
     get_file_info()->set_filenameString( get_sourceFileNameWithPath() );
#endif

  // DQ (5/9/2007): Moved this call from above to where the file name is available so that we could include 
  // the filename in the label.  This helps to identify the performance data with individual files where
  // multiple source files are specificed on the command line.
  // printf ("p_sourceFileNameWithPath = %s \n",p_sourceFileNameWithPath);
     string timerLabel = "AST SgFile Constructor for " + p_sourceFileNameWithPath + ":";
     TimingPerformance timer (timerLabel);

  // Build a DEEP COPY of the input parameters!
     vector<string> local_commandLineArgumentList = argv;

  // Save the commandline as a list of strings (we made a deep copy because the "callFrontEnd()" function might change it!
     set_originalCommandLineArgumentList( local_commandLineArgumentList );

  // DQ (5/22/2005): Store the file name index in the SgFile object so that it can figure out 
  // which file name applies to it.  This helps support functions such as "get_filename()" 
  // used elsewhere in Sage III.  Not clear if we really need this!
  // error checking
     ROSE_ASSERT (argv.size() > 1);

#if 0
  // DQ (1/18/2006): Set the filename in the SgFile::p_file_info
     ROSE_ASSERT(get_file_info() != NULL);
     get_file_info()->set_filenameString(p_sourceFileNameWithPath);
#endif

  // DQ (12/23/2008): Added assertion.
     ROSE_ASSERT(get_file_info() != NULL);

  // DQ (5/3/2007): Added assertion.
     ROSE_ASSERT (get_startOfConstruct() != NULL);

  // printf ("Leaving  SgFile::doSetupForConstructor() \n");
   }




#if 1
#define CASE_SENSITIVE_SYSTEM 1

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

       // DQ (8/20/2008): Add support for Qing's options!
          argument == "-annot" ||
          argument == "-bs" ||
          isOptionTakingThirdParameter(argument) ||

       // DQ (9/30/2008): Added support for java class specification required for Fortran use of OFP.
          argument == "--class" ||
          //AS(02/20/08):  When used with -M or -MM, -MF specifies a file to write 
          //the dependencies to. Need to tell ROSE to ignore that output paramater
          argument == "-MF" ||
          argument == "-outputdir" ||  //FMZ (12/22/1009) added for caf compiler
          argument == "-rose:disassembler_search" ||
          argument == "-rose:partitioner_search" ||
          argument == "-rose:partitioner_config" ||
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
#endif



string
SgFile::generate_C_preprocessor_intermediate_filename( string sourceFilename )
   {
  // Note: for "foo.F90" the fileNameSuffix() returns "F90"
     string filenameExtension              = StringUtility::fileNameSuffix(sourceFilename);
     string sourceFileNameWithoutExtension = StringUtility::stripFileSuffixFromFileName(sourceFilename);

  // string sourceFileNameInputToCpp = get_sourceFileNameWithPath();

  // printf ("Before lowering case: filenameExtension = %s \n",filenameExtension.c_str());

  // We need to turn on the 5th bit to make the capital a lower case character (assume ASCII)
     filenameExtension[0] = filenameExtension[0] | (1 << 5);

  // printf ("After lowering case: filenameExtension = %s \n",filenameExtension.c_str());

  // Rename the CPP generated intermediate file (strip path to put it in the current directory)
  // string sourceFileNameOutputFromCpp = sourceFileNameWithoutExtension + "_preprocessed." + filenameExtension;
     string sourceFileNameWithoutPathAndWithoutExtension = StringUtility::stripPathFromFileName(sourceFileNameWithoutExtension);
     string sourceFileNameOutputFromCpp = sourceFileNameWithoutPathAndWithoutExtension + "_postprocessed." + filenameExtension;

     return sourceFileNameOutputFromCpp;
   }


#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
// This is the "C" function implemented in:
//    ROSE/src/frontend/OpenFortranParser_SAGE_Connection/openFortranParser_main.c
// This function calls the Java JVM to load the Java implemented parser (written 
// using ANTLR, a parser generator).
int openFortranParser_main(int argc, char **argv );
#endif 

int
SgFile::callFrontEnd()
   {
  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

     int fileNameIndex = 0;

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Front End Processing (SgFile):");

  // This function processes the command line and calls the EDG frontend.
     int frontendErrorLevel = 0;

  // Build an argc,argv based C style commandline (we might not really need this)
     vector<string> argv = get_originalCommandLineArgumentList();

#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 9)
        {
       // Print out the input arguments, so we can set them up internally instead of
       // on the command line (which is driving me nuts)
          for (unsigned int i=0; i < argv.size(); i++)
               printf ("argv[%d] = %s \n",i,argv[i]);
        }
#endif

  // printf ("Inside of SgFile::callFrontEnd(): fileNameIndex = %d \n",fileNameIndex);

  // Save this so that it can be used in the template instantiation phase later.
  // This file is later written into the *.ti file so that the compilation can 
  // be repeated as required to instantiate all function templates.
     std::string translatorCommandLineString = CommandlineProcessing::generateStringFromArgList(argv,false,true);
  // printf ("translatorCommandLineString = %s \n",translatorCommandLineString.c_str());
     set_savedEdgCommandLine(translatorCommandLineString);

  // display("At TOP of SgFile::callFrontEnd()");

  // local copies of argc and argv variables
  // The purpose of building local copies is to avoid
  // the modification of the command line by SLA
     vector<string> localCopy_argv = argv;
  // printf ("DONE with copy of command line! \n");

  // Process command line options specific to ROSE
  // This leaves all filenames and non-rose specific option in the argv list
     processRoseCommandLineOptions (localCopy_argv);

  // DQ (6/21/2005): Process template specific options so that we can generated 
  // code for the backend compiler (this processing is backend specific).
     processBackendSpecificCommandLineOptions (localCopy_argv);

  // display("AFTER processRoseCommandLineOptions in SgFile::callFrontEnd()");

  // Use ROSE buildCommandLine() function
  // int numberOfCommandLineArguments = 24;
  // char** inputCommandLine = new char* [numberOfCommandLineArguments];
  // ROSE_ASSERT (inputCommandLine != NULL);
     vector<string> inputCommandLine;

  // Build the commandline for EDG
     build_EDG_CommandLine (inputCommandLine,localCopy_argv,fileNameIndex );

  // DQ (10/15/2005): This is now a single C++ string (and not a list)
  // Make sure the list of file names is allocated, even if there are no file names in the list.
  // DQ (1/23/2004): I wish that C++ string objects had been used uniformally through out this code!!!
  // ROSE_ASSERT (get_sourceFileNamesWithoutPath() != NULL);
  // ROSE_ASSERT (get_sourceFileNameWithoutPath().empty() == false);

     // display("AFTER build_EDG_CommandLine in SgFile::callFrontEnd()");

  // Exit if we are to ONLY call the vendor's backend compiler
     if (p_useBackendOnly == true)
          return 0;

     ROSE_ASSERT (p_useBackendOnly == false);

  // DQ (4/21/2006): If we have called the frontend for this SgFile then mark this file to be unparsed.
  // This will cause code to be generated and the compileOutput() function will then set the name of
  // the file that the backend (vendor) compiler will compile to be the the intermediate file. Else it
  // will be set to be the origianl source file.  In the new design, the frontendShell() can be called
  // to generate just the SgProject and SgFile nodes and we can loop over the SgFile objects and call 
  // the frontend separately for each SgFile.  so we have to set the output file name to be compiled
  // late in the processing (at backend compile time since we don't know when or if the frontend will
  // be called for each SgFile).
     set_skip_unparse(false);

#if 0
  // DQ (2/13/2004): This is no longer used!!!

  // This sets up a "call back" function. 
  // This function sets a function pointer internal to SAGE (EDG sage_gen_be.C) which 
  // is called by the sage processing after the EDG AST is translated into the SAGE AST.
  // The alternative would be to have the generation of the EDG PDF file be generated as 
  // an option to EDG, but this would requirre the addition of the PDF headers to EDG which 
  // I would like to avoid (I want to avoid modifying EDG if possible).
  // set_sage_transform_function(roseDisplayMechanism);
  // set_sage_edg_AST_display_function(roseDisplayMechanism);

  // DQ (4/23/2006): Declaration of friend function required here by g++ 4.1.0!
     void alternativeSageEdgInterfaceConstruction( SgFile *file );

     set_sage_transform_function(alternativeSageEdgInterfaceConstruction);
#endif

  // DQ (1/22/2004): As I recall this has a name that really 
  // should be "disable_edg" instead of "disable_edg_backend".
     if ( get_disable_edg_backend() == false )
        {
       // ROSE::new_frontend = true;

       // We can either use the newest EDG frontend separately (useful for debugging) 
       // or the EDG frontend that is included in SAGE III (currently EDG 3.3). 
       // New EDG frontend:
       //      This permits testing with the most up-to-date version of the EDG frontend and 
       //      can be useful in identifing errors or bugs in the SAGE processing (or ROSE itself).
       // EDG frontend used by SAGE III:
       //      The use of this frontend permits the continued processing via ROSE and the 
       //      unparsing of the AST to rebuilt the C++ source code (with transformations if any 
       //      were done).

       // DQ (10/15/2005): This is now a C++ string (and not char* C style string)
       // Make sure that we have generated a proper file name (or move filename
       // processing to processRoseCommandLineOptions()).
       // printf ("Print out the file name to make sure it is processed \n");
       // printf ("     filename = %s \n",get_unparse_output_filename());
       // ROSE_ASSERT (get_unparse_output_filename() != NULL);
       // ROSE_ASSERT (get_unparse_output_filename().empty() == false);

          if ( get_new_frontend() == true )
             {
            // Use the current version of the EDG frontend from EDG (or any other version)
               abort();
               printf ("ROSE::new_frontend == true (call edgFrontEnd using unix system() function!) \n");

               std::string frontEndCommandLineString;
               if ( get_KCC_frontend() == true )
                  {
                    frontEndCommandLineString = "KCC ";  // -cpfe_only is no longer supported (I think)
                  }
                 else
                  {
                    frontEndCommandLineString = "edgFrontEnd ";
                  }
               frontEndCommandLineString += CommandlineProcessing::generateStringFromArgList(inputCommandLine,true,false);

               if ( get_verbose() > 1 )
                    printf ("frontEndCommandLineString = %s \n",frontEndCommandLineString.c_str());

               ROSE_ASSERT (!"Should not get here");
               system(frontEndCommandLineString.c_str());

            // exit(0);
             }
            else
             {
            // Call the "INTERNAL" EDG Front End used by ROSE (with modified command
            // line input so that ROSE's command line is simplified)!
               if ( get_verbose() > 1 )
                    printf ("Calling edg_main \n");
#if 0
               frontEndCommandLineString = std::string(argv[0]) + std::string(" ") + CommandlineProcessing::generateStringFromArgList(inputCommandLine,false,false);

               if ( get_verbose() > 1 )
                    printf ("frontEndCommandLineString = %s \n",frontEndCommandLineString.c_str());
#endif
            // We need to detect errors in this stage so that we can prevent further processing
            // int edg_errorLevel = edg_main (numberOfCommandLineArguments, inputCommandLine,sageFile);
            // int edg_errorLevel = edg_main (numberOfCommandLineArguments, inputCommandLine,*this);
            // int frontendErrorLevel = 0;

#if 0
               this->get_project()->display("SgProject::callFrontEnd()");
               display("SgFile::callFrontEnd()");
               printf ("get_C_only()              = %s \n",(get_C_only()       == true) ? "true" : "false");
               printf ("get_C99_only()            = %s \n",(get_C99_only()     == true) ? "true" : "false");
               printf ("get_Cxx_only()            = %s \n",(get_Cxx_only()     == true) ? "true" : "false");
               printf ("get_Fortran_only()        = %s \n",(get_Fortran_only() == true) ? "true" : "false");
               printf ("get_F77_only()            = %s \n",(get_F77_only()     == true) ? "true" : "false");
               printf ("get_F90_only()            = %s \n",(get_F90_only()     == true) ? "true" : "false");
               printf ("get_F95_only()            = %s \n",(get_F95_only()     == true) ? "true" : "false");
               printf ("get_F2003_only()          = %s \n",(get_F2003_only()   == true) ? "true" : "false");
               printf ("get_CoArrayFortran_only() = %s \n",(get_CoArrayFortran_only()   == true) ? "true" : "false");
               printf ("get_PHP_only()            = %s \n",(get_PHP_only()     == true) ? "true" : "false");
               printf ("get_binary_only()         = %s \n",(get_binary_only()  == true) ? "true" : "false");

            // DQ (18/2008): We now explicit mark files that require C preprocessing...
               printf ("get_requires_C_preprocessor() = %s \n",(get_requires_C_preprocessor() == true) ? "true" : "false");
#endif
#if 0
               printf ("Exiting while testing binary \n");
               ROSE_ASSERT(false);
#endif

            // DQ (9/2/2008): Factored out the details of building the AST for Source code (SgSourceFile IR node) and Binaries (SgBinaryComposite IR node)
            // Note that making buildAST() a virtual function does not appear to solve the problems since it is called form the base class.  This is 
            // awkward code which is temporary.

            // printf ("Before calling buildAST(): this->class_name() = %s \n",this->class_name().c_str());

               switch (this->variantT())
                  {
                    case V_SgFile:
                    case V_SgSourceFile:
                       {
                         SgSourceFile* sourceFile = const_cast<SgSourceFile*>(isSgSourceFile(this));
                         frontendErrorLevel = sourceFile->buildAST(argv,inputCommandLine);
                         break;
                       }

                    case V_SgBinaryComposite:
                       {
                         SgBinaryComposite* binary = const_cast<SgBinaryComposite*>(isSgBinaryComposite(this));
                         frontendErrorLevel = binary->buildAST(argv,inputCommandLine);
                         break;
                       }
                    case V_SgUnknownFile:
                       {
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in unparser: class name = %s \n",this->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }

            // printf ("After calling buildAST(): this->class_name() = %s \n",this->class_name().c_str());
#if 0
               SgSourceFile* sourceFile = const_cast<SgSourceFile*>(isSgSourceFile(this));
               SgBinaryComposite* binary = const_cast<SgBinaryComposite*>(isSgBinaryComposite(this));
               if (binary != NULL)
                  {
                    ROSE_ASSERT(sourceFile == NULL);
                    frontendErrorLevel = binary->buildAST(argv,inputCommandLine);
                  }
                 else
                  {
                    if (sourceFile != NULL)
                       {
                         ROSE_ASSERT(binary == NULL);
                         frontendErrorLevel = sourceFile->buildAST(argv,inputCommandLine);
                       }
                      else
                       {
                         printf ("Error: neither sourceFile nor binary are valid pointers this = %s \n",class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
#endif
            // if there are warnings report that there are in the verbose mode and continue 
               if (frontendErrorLevel > 0)
                  {
                    if ( get_verbose() >= 1 )
                         cout << "Warnings in Rose parser/IR translation processing! (continuing ...) " << endl;
                  }
             }
        }

  // DQ (4/20/2006): This code was moved from the SgFile constructor so that is would
  // permit the separate construction of the SgProject and call to the front-end cleaner.

  // DQ (5/22/2005): This is a older function with a newer more up-to-date comment on why we have it.
  // This function is a repository for minor AST fixups done as a post-processing step in the 
  // construction of the Sage III AST from the EDG frontend.  In some cases it fixes specific 
  // problems in either EDG or the translation of EDG to Sage III (more the later than the former).
  // In other cases if does post-processing (e.g. setting parent pointers in the AST) can could
  // only done from a more complete global view of the staticly defined AST.  In many cases these
  // AST fixups are not so temporary so the name of the function might change at some point.
  // Notice that all AST fixup is done before attachment of the comments to the AST.
  // temporaryAstFixes(this);

#if 0
  // FMZ (this is just debugging support)
     list<SgScopeStatement*> *stmp = &astScopeStack;
     printf("FMZ :: before AstPostProcessing astScopeStack = %p \n",stmp);
#endif
 
  // GB (8/19/2009): Commented this out and moved it to SgProject::parse().
  // Repeated calls to AstPostProcessing (one per file) can be slow on
  // projects consisting of multiple files due to repeated memory pool
  // traversals.
  // AstPostProcessing(this);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // FMZ: 05/30/2008.  Do not generate .rmod file for the PU imported by "use" stmt
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
     if (get_Fortran_only() == true && FortranModuleInfo::isRmodFile() == false)
        {
          if (get_verbose() > 1)
               printf ("Generating a Fortran 90 module file (*.rmod) \n");

          generateModFile(this);

          if (get_verbose() > 1)
               printf ("DONE: Generating a Fortran 90 module file (*.rmod) \n");
        }
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif 
#if 0
     printf ("Leaving SgFile::callFrontEnd(): fileNameIndex = %d \n",fileNameIndex);
     display("At bottom of SgFile::callFrontEnd()");
#endif

  // return the error code associated with the call to the C++ Front-end
     return frontendErrorLevel;
   }



void
SgFile::secondaryPassOverSourceFile()
   {
  // **************************************************************************
  //                      Secondary Pass Over Source File
  // **************************************************************************
  // This pass collects extra information about the soruce file thay may not have 
  // been available from previous tools that operated on the file. For example:
  //    1) EDG ignores comments and so we collect the whole token stream in this phase.
  //    2) OFP ignores comments similarly to EDG and so we collect the whole token stream. 
  //    3) Binary disassemblily ignores the binary format so we collect this information
  //       about the structure of the ELF binary separately.
  // For source code (C,C++,Fortran) we collect the whole token stream, for example:
  //    1) Comments
  //    2) Preprocessors directives
  //    3) White space
  //    4) All tokens (each is classified as to what specific type of token it is)
  //
  // There is no secondary processing for binaries.

  // GB (9/4/2009): Factored out the secondary pass. It is now done after
  // the whole project has been constructed and fixed up.

     if (get_binary_only() == true)
        {
       // What used to be done here is now done above so that we can know the machine specific details
       // of the executable as early as possible before disassembly.
        }
       else
        {
       // This is set in the unparser now so that we can handle the source file plus all header files
          ROSE_ASSERT (p_preprocessorDirectivesAndCommentsList == NULL);

       // Build the empty list container so that we can just add lists for new files as they are encountered
          p_preprocessorDirectivesAndCommentsList = new ROSEAttributesListContainer();
          ROSE_ASSERT (p_preprocessorDirectivesAndCommentsList != NULL);

#if 0
       // This is empty so there is nothing to display!
          p_preprocessorDirectivesAndCommentsList->display("Seconadary Source File Processing at bottom of SgFile::callFrontEnd()");
#endif

       // DQ (4/19/2006): since they can take a while and includes substantial 
       // file I/O we make this optional (selected from the command line).
       // bool collectAllCommentsAndDirectives = get_collectAllCommentsAndDirectives();

       // DQ (12/17/2008): The merging of CPP directives and comments from either the 
       // source file or including all the include files is not implemented as a single 
       // traversal and has been rewritten.
          if (get_skip_commentsAndDirectives() == false)
             {
               if (get_verbose() > 1)
                  {
                    printf ("In SgFile::secondaryPassOverSourceFile(): calling attachAllPreprocessingInfo() \n");
                  }

            // printf ("Secondary pass over source file = %s to comment comments and CPP directives \n",this->get_file_info()->get_filenameString().c_str());
            // SgSourceFile* sourceFile = const_cast<SgSourceFile*>(this);
               SgSourceFile* sourceFile = isSgSourceFile(this);
               ROSE_ASSERT(sourceFile != NULL);

            // Save the state of the requirement fo CPP processing (fortran only)
               bool requiresCPP = false;
               if (get_Fortran_only() == true)
                  {
                    requiresCPP = get_requires_C_preprocessor();
                    if (requiresCPP == true)
                         set_requires_C_preprocessor(false);
                  }
#if 1
            // Debugging code (eliminate use of CPP directives from source file so that we
            // can debug the insertion of linemarkers from first phase of CPP processing.
            // printf ("In SgFile::secondaryPassOverSourceFile(): requiresCPP = %s \n",requiresCPP ? "true" : "false");
               if (requiresCPP == false)
                  {
                    attachPreprocessingInfo(sourceFile);
                 // printf ("Exiting as a test (should not be called for Fortran CPP source files) \n");
                 // ROSE_ASSERT(false);
                  }
#else
            // Normal path calling attachPreprocessingInfo()
               attachPreprocessingInfo(sourceFile);
#endif

            // Liao, 3/31/2009 Handle OpenMP here to see macro calls within directives
               processOpenMP(sourceFile);

            // Reset the saved state (might not really be required at this point).
               if (requiresCPP == true)
                    set_requires_C_preprocessor(false);

#if 0
               printf ("In SgFile::callFrontEnd(): exiting after attachPreprocessingInfo() \n");
               ROSE_ASSERT(false);
#endif
               if (get_verbose() > 1)
                  {
                    printf ("In SgFile::callFrontEnd(): Done with attachAllPreprocessingInfo() \n");
                  }
             } //end if get_skip_commentsAndDirectives() is false
        }

#if 0
     printf ("Leaving SgFile::callFrontEnd(): fileNameIndex = %d \n",fileNameIndex);
     display("At bottom of SgFile::callFrontEnd()");
#endif

#if 1
  // DQ (8/22/2009): We test the parent of SgFunctionTypeTable in the AST post processing,
  // so we need to make sure that it is set.
     SgFunctionTypeTable* functionTypeTable = SgNode::get_globalFunctionTypeTable();
  // ROSE_ASSERT(functionTypeTable != NULL);
     if (functionTypeTable != NULL && functionTypeTable->get_parent() == NULL)
        {
       // printf ("In SgFile::callFrontEnd(): set the parent of SgFunctionTypeTable \n");
          functionTypeTable->set_parent(this);
        }
  // ROSE_ASSERT(functionTypeTable->get_parent() != NULL);
#endif

  // ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
  // ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_parent() != NULL);
   }


#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
// DQ (9/30/2008): Refactored the setup of the class path for Java and OFP.
string
global_build_classpath()
   {
  // This function builds the class path for use with Java and the cal to the OFP.
     string classpath = "-Djava.class.path=";
  // DQ (3/11/2010): Updating to new Fortran OFP version 0.7.2 with Craig.
  // classpath += findRoseSupportPathFromBuild("/src/3rdPartyLibraries/fortran-parser/OpenFortranParser.jar", "lib/OpenFortranParser.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-2.7.7.jar", "lib/antlr-2.7.7.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-3.0.1.jar", "lib/antlr-3.0.1.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-runtime-3.0.1.jar", "lib/antlr-runtime-3.0.1.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/stringtemplate-3.1b1.jar", "lib/stringtemplate-3.1b1.jar") + ":";
     classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-3.2.jar", "lib/antlr-3.2.jar") + ":";

     string ofp_jar_file_name = string("OpenFortranParser-") + StringUtility::numberToString(ROSE_OFP_MAJOR_VERSION_NUMBER) + "." + StringUtility::numberToString(ROSE_OFP_MINOR_VERSION_NUMBER) + "." + StringUtility::numberToString(ROSE_OFP_PATCH_VERSION_NUMBER) + string(".jar");
     string ofp_class_path = "src/3rdPartyLibraries/fortran-parser/" + ofp_jar_file_name;
     classpath += findRoseSupportPathFromBuild(ofp_class_path, string("lib/") + ofp_jar_file_name) + ":";

     classpath += ".";

     if (SgProject::get_verbose() > 1)
        {
          printf ("In global_build_classpath(): classpath = %s \n",classpath.c_str());
        }

     return classpath;
   }

string
SgSourceFile::build_classpath()
   {
     return global_build_classpath();
   }

int
SgSourceFile::build_Fortran_AST( vector<string> argv, vector<string> inputCommandLine )
   {
  // This is how we pass the pointer to the SgFile created in ROSE before the Open 
  // Fortran Parser is called to the Open Fortran Parser.  In the case of C/C++ using
  // EDG the SgFile is passed through the edg_main() function, but not so with the 
  // Open Fortran Parser's openFortranParser_main() function API.  So we use this
  // global variable to pass the SgFile (so that the parser c_action functions can
  // build the Fotran AST using the existing SgFile.
     extern SgSourceFile* OpenFortranParser_globalFilePointer;

  // printf ("######################### Inside of SgSourceFile::build_Fortran_AST() ############################ \n");

     bool requires_C_preprocessor = get_requires_C_preprocessor();
     if (requires_C_preprocessor == true)
        {
       // If we detect that the input file requires processing via CPP (e.g. filename of form *.F??) then 
       // we generate the command to run CPP on the input file and collect the results in a file with 
       // the suffix "_postprocessed.f??".  Note: instead of using CPP we use the target backend fortran 
       // compiler with the "-E" option.

          vector<string> fortran_C_preprocessor_commandLine;

       // Note: The `-traditional' and `-undef' flags are supplied to cpp by default [when used with cpp is used by gfortran], 
       // to help avoid unpleasant surprises.  So to simplify use of cpp and make it more consistant with gfortran we use 
       // gfortran to call cpp.
          fortran_C_preprocessor_commandLine.push_back(BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH);

       // DQ (5/19/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               fortran_C_preprocessor_commandLine.push_back(includeList[i]);
             }

          fortran_C_preprocessor_commandLine.push_back("-E");

#if 0
// DQ (9/16/2009): I don't think we need to pass this to gfortran if we are just using gfortran to call CPP

// We need this #if since if gfortran is unavailable the macros for the major and minor version numbers will be empty strings (blank).
#if USE_GFORTRAN_IN_ROSE
       // DQ (9/16/2009): This option is not available in gfortran version 4.0.x (wonderful).
       // DQ (5/20/2008): Need to select between fixed and free format
       // fortran_C_preprocessor_commandLine.push_back("-ffree-line-length-none");
          if ( (BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER >= 4) && (BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER >= 1) )
             {
               fortran_C_preprocessor_commandLine.push_back("-ffree-line-length-none");
             }
#endif
#endif
          string sourceFilename              = get_sourceFileNameWithPath();
          fortran_C_preprocessor_commandLine.push_back(sourceFilename);

          fortran_C_preprocessor_commandLine.push_back("-o");
          string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
          fortran_C_preprocessor_commandLine.push_back(sourceFileNameOutputFromCpp);

          if ( SgProject::get_verbose() > 0 )
               printf ("cpp command line = %s \n",CommandlineProcessing::generateStringFromArgList(fortran_C_preprocessor_commandLine,false,false).c_str());

          int errorCode = 0;
#if USE_GFORTRAN_IN_ROSE
       // Some security checking here could be helpful!!!
          errorCode = systemFromVector (fortran_C_preprocessor_commandLine);
#endif
       // DQ (10/1/2008): Added error checking on return value from CPP.
          if (errorCode != 0)
             {
               printf ("Error in running cpp on Fortran code: errorCode = %d \n",errorCode);
               ROSE_ASSERT(false);
             }

#if 0
          printf ("Exiting as a test ... (after calling C preprocessor)\n");
          ROSE_ASSERT(false);
#endif
        }


  // DQ (9/30/2007): Introduce syntax checking on input code (initially we can just call the backend compiler 
  // and let it report on the syntax errors).  Later we can make this a command line switch to disable (default 
  // should be true).
  // bool syntaxCheckInputCode = true;
     bool syntaxCheckInputCode = (get_skip_syntax_check() == false);

  // printf ("In build_Fortran_AST(): syntaxCheckInputCode = %s \n",syntaxCheckInputCode ? "true" : "false");

     if (syntaxCheckInputCode == true)
        {
       // Note that syntax checking of Fortran 2003 code using gfortran versions greater than 4.1 can 
       // be a problem because there are a lot of bugs in the Fortran 2003 support in later versions 
       // of gfortran (not present in initial Fortran 2003 support for syntax checking only). This problem 
       // has been verified in version 4.2 and 4.3 of gfortran.

       // DQ (9/30/2007): Introduce tracking of performance of ROSE.
          TimingPerformance timer ("Fortran syntax checking of input:");

       // DQ (9/30/2007): For Fortran, we want to run gfortran up front so that we can verify that
       // the input file is syntax error free.  First lets see what data is avilable to use to check
       // that we have a fortran file.
       // display("Before calling OpenFortranParser, what are the values in the SgFile");

       // DQ (9/30/2007): Call the backend Fortran compiler (typically gfortran) to check the syntax 
       // of the input program.  When using GNU gfortran, use the "-S" option which means:
       // "Compile only; do not assemble or link".

       // DQ (11/17/2007): Note that syntax and semantics checking is turned on using -fno-backend not -S 
       // as I previously thought.  Also I have turned on all possible warnings and specified the Fortran 2003
       // features.  I have also specified use of cray-pointers.
       // string syntaxCheckingCommandline = "gfortran -S " + get_sourceFileNameWithPath();
       // string warnings = "-Wall -Wconversion -Waliasing -Wampersand -Wimplicit-interface -Wline-truncation -Wnonstd-intrinsics -Wsurprising -Wunderflow -Wunused-labels";
       // DQ (12/8/2007): Added commandline control over warnings output in using gfortran sytax checking prior to use of OFP.

       // printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Setting up Fortran Syntax check @@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");

          vector<string> fortranCommandLine;
          fortranCommandLine.push_back(ROSE_GFORTRAN_PATH);
          fortranCommandLine.push_back("-fsyntax-only");

       // DQ (5/19/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               fortranCommandLine.push_back(includeList[i]);
             }

          if (get_output_warnings() == true)
             {
            // These are gfortran specific options
            // As of 2004, -Wall implied: -Wunused-labels, -Waliasing, -Wsurprising and -Wline-truncation
            // Additional major options include:
            //      -fsyntax-only -pedantic -pedantic-errors -w -Wall -Waliasing -Wconversion -Wimplicit-interface
            //      -Wsurprising -Wunderflow -Wunused-labels -Wline-truncation -Werror -W
            // warnings = "-Wall -Wconversion -Waliasing -Wampersand -Wimplicit-interface -Wline-truncation -Wnonstd-intrinsics -Wsurprising -Wunderflow";

            // If warnings are requested (on the comandline to ROSE translator) then we want to output all possible warnings by defaul (at leas for how)

            // Check if we are using GNU compiler backend (if so then we are using gfortran, though we have no test in place currently for what 
            // version of gfortran (as we do for C and C++))
               string backendCompilerSystem = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
               if (backendCompilerSystem == "g++" || backendCompilerSystem == "mpicc" || backendCompilerSystem == "mpicxx")
                  {
                 // Since this is specific to gfortran version 4.1.2, we will exclude it (it is also redundant since it is included in -Wall)
                 // warnings += " -Wunused-labels";
                 // warnings = "-Wall -Wconversion -Wampersand -Wimplicit-interface -Wnonstd-intrinsics -Wunderflow";
                    fortranCommandLine.push_back("-Wall");

                 // Add in the gfortran extra warnings
                    fortranCommandLine.push_back("-W");

                 // More warnings not yet turned on.
                    fortranCommandLine.push_back("-Wconversion");
                    fortranCommandLine.push_back("-Wampersand");
                    fortranCommandLine.push_back("-Wimplicit-interface");
                    fortranCommandLine.push_back("-Wnonstd-intrinsics");
                    fortranCommandLine.push_back("-Wunderflow");
                  }
                 else
                  {
                    printf ("Currently only the GNU compiler backend is supported (gfortran) backendCompilerSystem = %s \n",backendCompilerSystem.c_str());
                    ROSE_ASSERT(false);
                  }
             }

       // Refactor the code below so that we can conditionally set the -ffree-line-length-none 
       // or -ffixed-line-length-none options (not available in all versions of gfortran).
          string use_line_length_none_string;

          bool relaxSyntaxCheckInputCode = (get_relax_syntax_check() == true);

       // DQ (11/17/2007): Set the fortran mode used with gfortran.
          if (get_F90_only() == true || get_F95_only() == true)
             {
            // For now let's consider f90 to be syntax checked under f95 rules (since gfortran does not support a f90 specific mode)
               if (relaxSyntaxCheckInputCode == false)
                    fortranCommandLine.push_back("-std=f95");

            // DQ (5/20/2008)
            // fortranCommandLine.push_back("-ffree-line-length-none");
               use_line_length_none_string = "-ffree-line-length-none";
             }
            else
             {
               if (get_F2003_only() == true)
                  {
                 // fortranCommandLine.push_back("-std=f2003");
                    if (relaxSyntaxCheckInputCode == false)
                         fortranCommandLine.push_back("-std=f2003");

                 // DQ (5/20/2008)
                 // fortranCommandLine.push_back("-ffree-line-length-none");
                    use_line_length_none_string = "-ffree-line-length-none";
                  }
                 else
                  {
                 // This should be the default mode (fortranMode string is empty). So is it f77?

                 // DQ (5/20/2008)
                 // fortranCommandLine.push_back ("-ffixed-line-length-none");
                    use_line_length_none_string = "-ffixed-line-length-none";
                  }
             }

// We need this #if since if gfortran is unavailable the macros for the major and minor version numbers will be empty strings (blank).
#if USE_GFORTRAN_IN_ROSE
       // DQ (9/16/2009): This option is not available in gfortran version 4.0.x (wonderful).
       // DQ (5/20/2008): Need to select between fixed and free format
       // fortran_C_preprocessor_commandLine.push_back("-ffree-line-length-none");
          if ( (BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER >= 4) && (BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER >= 1) )
             {
               fortranCommandLine.push_back(use_line_length_none_string);
             }
#endif

       // DQ (12/8/2007): Added support for cray pointers from commandline.
          if (get_cray_pointer_support() == true)
             {
               fortranCommandLine.push_back("-fcray-pointer");
             }

       // Note that "-c" is required to enforce that we only compile and not link the result (even though -fno-backend is specified)
       // A web page specific to -fno-backend suggests using -fsyntax-only instead (so the "-c" options is not required).
#if 1
       // if ( SgProject::get_verbose() > 0 )
          if ( get_verbose() > 0 )
             {
               printf ("Checking syntax of input program using gfortran: syntaxCheckingCommandline = %s \n",CommandlineProcessing::generateStringFromArgList(fortranCommandLine,false,false).c_str());
             }
#endif
       // Call the OS with the commandline defined by: syntaxCheckingCommandline
#if 0
          fortranCommandLine.push_back(get_sourceFileNameWithPath());
#else
       // DQ (5/19/2008): Support for C preprocessing
          if (requires_C_preprocessor == true)
             {
            // If C preprocessing was required then we have to provide the generated filename of the preprocessed file!

            // Note that since we are using gfortran to do the syntax checking, we could just
            // hand the original file to gfortran instead of the one that we generate using CPP.
               string sourceFilename    = get_sourceFileNameWithPath();
               string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
               fortranCommandLine.push_back(sourceFileNameOutputFromCpp);
             }
            else
             {
            // This will cause the original file to be used for syntax checking (instead of 
            // the CPP generated one, if one was generated).
               fortranCommandLine.push_back(get_sourceFileNameWithPath());
             }
#endif
       // At this point we have the full command line with the source file name
          if ( get_verbose() > 0 )
             {
               printf ("Checking syntax of input program using gfortran: syntaxCheckingCommandline = %s \n",CommandlineProcessing::generateStringFromArgList(fortranCommandLine,false,false).c_str());
             }

          int returnValueForSyntaxCheckUsingBackendCompiler = 0;
#if USE_GFORTRAN_IN_ROSE
          returnValueForSyntaxCheckUsingBackendCompiler = systemFromVector (fortranCommandLine);
#else
          printf ("backend fortran compiler (gfortran) unavailable ... (not an error) \n");
#endif

       // Check that there are no errors, I think that warnings are ignored!
          if (returnValueForSyntaxCheckUsingBackendCompiler != 0)
             {
               printf ("Syntax errors detected in input fortran program ... \n");

            // We should define some convention for error codes returned by ROSE
               exit(1);
             }
          ROSE_ASSERT(returnValueForSyntaxCheckUsingBackendCompiler == 0);

       // printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@ DONE: Setting up Fortran Syntax check @@@@@@@@@@@@@@@@@@@@@@@@@ \n");

#if 0
          printf ("Exiting as a test ... (after syntax check) \n");
          ROSE_ASSERT(false);
#endif
        }

  // Build the classpath list for Java support.
     const string classpath = build_classpath();

  // This is part of debugging output to call OFP and output the list of parser actions that WOULD be called.
  // printf ("get_output_parser_actions() = %s \n",get_output_parser_actions() ? "true" : "false");
     if (get_output_parser_actions() == true)
        {
       // DQ (1/19/2008): New version of OFP requires different calling syntax.
       // string OFPCommandLineString = std::string("java parser.java.FortranMain") + " --dump " + get_sourceFileNameWithPath();
          vector<string> OFPCommandLine;
          OFPCommandLine.push_back(JAVA_JVM_PATH);
          OFPCommandLine.push_back(classpath);
          OFPCommandLine.push_back("fortran.ofp.FrontEnd");
          OFPCommandLine.push_back("--dump");

       // DQ (5/18/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               OFPCommandLine.push_back(includeList[i]);
             }

       // DQ (5/19/2008): Support for C preprocessing
          if (requires_C_preprocessor == true)
             {
            // If C preprocessing was required then we have to provide the generated filename of the preprocessed file!
            // Note that OFP has no support for CPP directives and will ignore them all.
               string sourceFilename              = get_sourceFileNameWithPath();
               string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
               OFPCommandLine.push_back(sourceFileNameOutputFromCpp);
             }
            else
             {
            // Build the command line using the original file (to be used by OFP).
               OFPCommandLine.push_back(get_sourceFileNameWithPath());
             }

#if 1
          printf ("output_parser_actions: OFPCommandLine = %s \n",CommandlineProcessing::generateStringFromArgList(OFPCommandLine,false,false).c_str());
#endif

#if 1
       // Some security checking here could be helpful!!!
       // Run OFP with the --dump option so that we can get the parset actions (used only for internal debugging support).
          int errorCode = systemFromVector(OFPCommandLine);

          if (errorCode != 0)
             {
               printf ("Running OFP ONLY causes an error (errorCode = %d) \n",errorCode);
#if 1
            // DQ (10/4/2008): Need to work with Liao to see why this passes for me but fails for him (and others).
            // for now we can comment out the error checking on the running of OFP as part of getting the 
            // output_parser_actions option (used for debugging).
               ROSE_ASSERT(false);
#else
               printf ("Skipping enforcement of exit after running OFP ONLY as (part of output_parser_actions option) \n");
#endif
             }
#else

#error "REMOVE THIS CODE"

       // This fails, I think because we can't call the openFortranParser_main twice. 
       // DQ (11/30/2008):  Does the work by Rice fix this now?
          int openFortranParser_dump_argc    = 0;
          char** openFortranParser_dump_argv = NULL;
          CommandlineProcessing::generateArgcArgvFromList(OFPCommandLine,openFortranParser_dump_argc,openFortranParser_dump_argv);
          frontendErrorLevel = openFortranParser_main (openFortranParser_dump_argc, openFortranParser_dump_argv);

#endif
       // If this was selected as an option then we can stop here (rather than call OFP again).
       // printf ("--- get_exit_after_parser() = %s \n",get_exit_after_parser() ? "true" : "false");
          if (get_exit_after_parser() == true)
             {
               printf ("Exiting after parsing... \n");
               exit(0);
             }

       // End of option handling to generate list of OPF parser actions.
        }

  // Option to just run the parser (not constructing the AST) and quit.
  // printf ("get_exit_after_parser() = %s \n",get_exit_after_parser() ? "true" : "false");
     if (get_exit_after_parser() == true)
        {
       // DQ (1/19/2008): New version of OFP requires different calling syntax.
       // string OFPCommandLineString = std::string("java parser.java.FortranMain") + " " + get_sourceFileNameWithPath();
          vector<string> OFPCommandLine;
          OFPCommandLine.push_back(JAVA_JVM_PATH);
          OFPCommandLine.push_back(classpath);
          OFPCommandLine.push_back("fortran.ofp.FrontEnd");

          bool foundSourceDirectoryExplicitlyListedInIncludePaths = false;

       // DQ (5/18/2008): Added support for include paths as required for relatively new Fortran specific include mechanism in OFP.
          const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();
          for (size_t i = 0; i < includeList.size(); i++)
             {
               OFPCommandLine.push_back(includeList[i]);

            // printf ("includeList[%d] = %s \n",i,includeList[i].c_str());

            // I think we have to permit an optional space between the "-I" and the path
               if ("-I" + getSourceDirectory() == includeList[i] || "-I " + getSourceDirectory() == includeList[i])
                  {
                 // The source file path is already included!
                    foundSourceDirectoryExplicitlyListedInIncludePaths = true;
                  }
             }

       // printf ("foundSourceDirectoryExplicitlyListedInIncludePaths = %s \n",foundSourceDirectoryExplicitlyListedInIncludePaths ? "true" : "false");
          if (foundSourceDirectoryExplicitlyListedInIncludePaths == false)
             {
            // Add the source directory to the include list so that we reproduce the semantics of gfortran
               OFPCommandLine.push_back("-I" + getSourceDirectory() );
             }

       // DQ (8/24/2010): Detect the use of CPP on the fortran file and use the correct generated file from CPP, if required.
       // OFPCommandLine.push_back(get_sourceFileNameWithPath());
          if (requires_C_preprocessor == true)
             {
               string sourceFilename = get_sourceFileNameWithoutPath();
               string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
               OFPCommandLine.push_back(sourceFileNameOutputFromCpp);
             }
            else
             {
               OFPCommandLine.push_back(get_sourceFileNameWithPath());
             }
          
#if 0
          printf ("exit_after_parser: OFPCommandLine = %s \n",StringUtility::listToString(OFPCommandLine).c_str());
#endif
#if 1
       // Some security checking here could be helpful!!!
          int errorCode = systemFromVector (OFPCommandLine);

       // DQ (9/30/2008): Added error checking of return value
          if (errorCode != 0)
             {
               printf ("Using option -rose:exit_after_parser (errorCode = %d) \n",errorCode);
               ROSE_ASSERT(false);
             }
#else

// #error "REMOVE THIS CODE"

       // This fails, I think because we can't call the openFortranParser_main twice.
          int openFortranParser_only_argc    = 0;
          char** openFortranParser_only_argv = NULL;
          CommandlineProcessing::generateArgcArgvFromList(OFPCommandLine,openFortranParser_only_argc,openFortranParser_only_argv);
       // frontendErrorLevel = openFortranParser_main (openFortranParser_only_argc, openFortranParser_only_argv);
          int errorCode = openFortranParser_main (openFortranParser_only_argc, openFortranParser_only_argv);

#endif
          printf ("Skipping all processing after parsing fortran (OFP) ... (get_exit_after_parser() == true) errorCode = %d \n",errorCode);
       // exit(0);

          ROSE_ASSERT(errorCode == 0);
          return errorCode;
       }
                    
  // DQ (1/19/2008): New version of OFP requires different calling syntax; new lib name is: libfortran_ofp_parser_java_FortranParserActionJNI.so old name: libparser_java_FortranParserActionJNI.so
  // frontEndCommandLineString = std::string(argv[0]) + " --class parser.java.FortranParserActionJNI " + get_sourceFileNameWithPath();
     vector<string> frontEndCommandLine;

     frontEndCommandLine.push_back(argv[0]);
  // frontEndCommandLine.push_back(classpath);
     frontEndCommandLine.push_back("--class");
     frontEndCommandLine.push_back("fortran.ofp.parser.c.jni.FortranParserActionJNI");

#if 0
  // Debugging output
     get_project()->display("Calling SgProject display");
     display("Calling SgFile display");
#endif

     const SgStringList & includeList = get_project()->get_includeDirectorySpecifierList();

     bool foundSourceDirectoryExplicitlyListedInIncludePaths = false;

  // printf ("getSourceDirectory() = %s \n",getSourceDirectory().c_str());
     for (size_t i = 0; i < includeList.size(); i++)
        {
          frontEndCommandLine.push_back(includeList[i]);

       // printf ("includeList[%d] = %s \n",i,includeList[i].c_str());

       // I think we have to permit an optional space between the "-I" and the path
          if (	"-I" + getSourceDirectory() == includeList[i] || "-I " + getSourceDirectory() == includeList[i])
             {
            // The source file path is already included!
               foundSourceDirectoryExplicitlyListedInIncludePaths = true;
             }
        }

  // printf ("foundSourceDirectoryExplicitlyListedInIncludePaths = %s \n",foundSourceDirectoryExplicitlyListedInIncludePaths ? "true" : "false");
     if (foundSourceDirectoryExplicitlyListedInIncludePaths == false)
        {
       // Add the source directory to the include list so that we reproduce the semantics of gfortran
          frontEndCommandLine.push_back("-I" + getSourceDirectory() );
        }

  // DQ (5/19/2008): Support for C preprocessing
     if (requires_C_preprocessor == true)
        {
       // If C preprocessing was required then we have to provide the generated filename of the preprocessed file!

       // Note that the filename referenced in the Sg_File_Info objects will use the original file name and not 
       // the generated file name of the CPP generated file.  This is because it gets the filename from the 
       // SgSourceFile IR node and not from the filename provided on the internal command line generated for call OFP.

          string sourceFilename              = get_sourceFileNameWithPath();
          string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);

          frontEndCommandLine.push_back(sourceFileNameOutputFromCpp);
        }
       else
        {
       // If not being preprocessed, the fortran filename is just the original input source file name.
          frontEndCommandLine.push_back(get_sourceFileNameWithPath());
        }

#if 1
     if ( get_verbose() > 0 )
          printf ("numberOfCommandLineArguments = %zu frontEndCommandLine = %s \n",inputCommandLine.size(),CommandlineProcessing::generateStringFromArgList(frontEndCommandLine,false,false).c_str());
#endif

     int openFortranParser_argc    = 0;
     char** openFortranParser_argv = NULL;
     CommandlineProcessing::generateArgcArgvFromList(frontEndCommandLine,openFortranParser_argc,openFortranParser_argv);

  // printf ("openFortranParser_argc = %d openFortranParser_argv = %s \n",openFortranParser_argc,CommandlineProcessing::generateStringFromArgList(openFortranParser_argv,false,false).c_str());

  // DQ (8/19/2007): Setup the global pointer used to pass the SgFile to which the Open Fortran Parser 
  // should attach the AST.  This is a bit ugly, but the parser interface only takes a commandline so it 
  // would be more ackward to pass a pointer to a C++ object through the commandline or the Java interface.
     OpenFortranParser_globalFilePointer = const_cast<SgSourceFile*>(this);
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);

     if ( get_verbose() > 1 )
          printf ("Calling openFortranParser_main(): OpenFortranParser_globalFilePointer = %p \n",OpenFortranParser_globalFilePointer);

#if USE_ROSE_SSL_SUPPORT
  // The use of the JVM required to support Java is a problem when linking to the SSL library (either -lssl or -lcrypto)
  // this may be fixed in Java version 6, but this is a hope, it has not been tested.  Java version 6 does
  // appear to fix the problem with zlib (we think) and this appears to be a similar problem).
     int frontendErrorLevel = 1;

     printf ("********************************************************************************************** \n");
     printf ("Fortran support using the JVM is incompatable with the use of the SSL library (fails in jvm).  \n");
     printf ("To enable the use of Fortran support in ROSE don't use --enable-ssl on configure command line. \n");
     printf ("********************************************************************************************** \n");
#else

  // frontendErrorLevel = openFortranParser_main (numberOfCommandLineArguments, inputCommandLine);
     int frontendErrorLevel = openFortranParser_main (openFortranParser_argc, openFortranParser_argv);
#endif

     if ( get_verbose() > 1 )
          printf ("DONE: Calling the openFortranParser_main() function (which loads the JVM) \n");

  // Reset this global pointer after we are done (just to be safe and avoid it being used later and causing strange bugs).
     OpenFortranParser_globalFilePointer = NULL;

  // Now read the CPP directives such as "# <number> <filename> <optional numeric code>", in the generated file from CPP.
     if (requires_C_preprocessor == true)
        {
       // If this was part of the processing of a CPP generated file then read the preprocessed file 
       // to get the CPP directives (different from those of the original *.F?? file) which will 
       // indicate line numbers and files where text was inserted as part of CPP processing.  We 
       // mostly want to read CPP declarations of the form "# <number> <filename> <optional numeric code>".
       // these are the only directives that will be in the CPP generated file (as I recall).

          string sourceFilename              = get_sourceFileNameWithPath();
          string sourceFileNameOutputFromCpp = generate_C_preprocessor_intermediate_filename(sourceFilename);
#if 0
          printf ("Need to preprocess the CPP generated fortran file so that we can attribute source code statements to files: sourceFileNameOutputFromCpp = %s \n",sourceFileNameOutputFromCpp.c_str());
#endif
#if 1
       // Note that the collection of CPP linemarker directives from the CPP generated file 
       // (and their insertion into the AST), should be done before the collection and 
       // insertion of the Comments and more general CPP directives from the original source 
       // file and their insertion. This will allow the correct representation of source 
       // position information to be used in the final insertion of comments and CPP directives
       // from the original file.

#if 0
       // DQ (12/19/2008): This is now done by the AttachPreprocessingInfoTreeTrav

       // List of all comments and CPP directives (collected from the generated CPP file so that we can collect the 
       // CPP directives that indicate source file boundaries of included regions of source code.
       // E.g. # 42 "foobar.f" 2
          ROSEAttributesList* currentListOfAttributes = new ROSEAttributesList();
          ROSE_ASSERT(currentListOfAttributes != NULL);

       // DQ (11/28/2008): This will collect the CPP directives from the generated CPP file so that 
       // we can associate parts of the AST included from different files with the correct file.  
       // Without this processing all the parts of the AST will be associated with the same generated file.
          currentListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(sourceFileNameOutputFromCpp,ROSEAttributesList::e_C_language);
#endif

#if 0
          printf ("Secondary pass over Fortran source file = %s to comment comments and CPP directives (might still be referencing the original source file) \n",sourceFileNameOutputFromCpp.c_str());
          printf ("Calling attachPreprocessingInfo() \n");
#endif

          attachPreprocessingInfo(this);

       // printf ("DONE: calling attachPreprocessingInfo() \n");

       // DQ (12/19/2008): Now we have to do an analysis of the AST to interpret the linemarkers.
          processCppLinemarkers();

#endif
#if 0
          printf ("Exiting as a test ... (collect the CPP directives from the CPP generated file after building the AST)\n");
          ROSE_ASSERT(false);
#endif
        }

  // printf ("######################### Leaving SgSourceFile::build_Fortran_AST() ############################ \n");

     return frontendErrorLevel;
   }
#else // for !USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT

// DQ (11/26/2008): I am unclear why this is required (I think it was added by Rice).
string
SgSourceFile::build_classpath()
   {
     fprintf(stderr, "Fortran parser not supported \n");
     ROSE_ASSERT(false);
  // abort();

  // DQ (11/30/2009): MSVC requires a return stmt from a non-void function (an error, not a warning).
	 return "error in SgSourceFile::build_classpath()";
   }

int
SgSourceFile::build_Fortran_AST( vector<string> argv, vector<string> inputCommandLine )
   {
     fprintf(stderr, "Fortran parser not supported \n");
     ROSE_ASSERT(false);
  // abort();

  // DQ (11/30/2009): MSVC requires a return stmt from a non-void function (an error, not a warning).
	 return -1;
   }
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT

#endif // ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT   


namespace SgSourceFile_processCppLinemarkers
   {
  // This class (AST traversal) supports the traversal of the AST required 
  // to translate the source position using the CPP linemarkers.

     class LinemarkerTraversal : public AstSimpleProcessing
        {
          public:
           // list<PreprocessingInfo*> preprocessingInfoStack;
              list< pair<int,int> > sourcePositionStack;

              LinemarkerTraversal( const string & sourceFilename );

              void visit ( SgNode* astNode );
        };
   }


SgSourceFile_processCppLinemarkers::LinemarkerTraversal::LinemarkerTraversal( const string & sourceFilename )
   {
  // Build an initial element on the stack so that the original source file name will be used to 
  // set the global scope (which will be traversed before we visit any statements that might have 
  // CPP directives attached (or which are CPP direcitve IR nodes).

  // Get the fileId of the assocated filename
     int fileId = Sg_File_Info::getIDFromFilename(sourceFilename);

  // Assume this is line 1 (I forget why zero is not a great idea here, 
  // I expect that it causes a consstancy test to fail somewhere).
     int line = 1;

     if ( SgProject::get_verbose() > 1 )
          printf ("In LinemarkerTraversal::LinemarkerTraversal(): Push initial stack entry for line = %d fileId = %d sourceFilename = %s \n",line,fileId,sourceFilename.c_str());

  // Push an entry onto the stack before doing the traversal over the whole AST.
     sourcePositionStack.push_front( pair<int,int>(line,fileId) );
   }

void
SgSourceFile_processCppLinemarkers::LinemarkerTraversal::visit ( SgNode* astNode )
   {
     SgStatement* statement = isSgStatement(astNode);

  // printf ("LinemarkerTraversal::visit(): statement = %p = %s \n",statement,(statement != NULL) ? statement->class_name().c_str() : "NULL");

     if (statement != NULL)
        {
          if ( SgProject::get_verbose() > 1 )
               printf ("LinemarkerTraversal::visit(): statement = %p = %s \n",statement,statement->class_name().c_str());

          AttachedPreprocessingInfoType *commentOrDirectiveList = statement->getAttachedPreprocessingInfo();

          if ( SgProject::get_verbose() > 1 )
               printf ("LinemarkerTraversal::visit(): commentOrDirectiveList = %p (size = %zu) \n",commentOrDirectiveList,(commentOrDirectiveList != NULL) ? commentOrDirectiveList->size() : 0);

          if (commentOrDirectiveList != NULL)
             {
               AttachedPreprocessingInfoType::iterator i = commentOrDirectiveList->begin();
               while(i != commentOrDirectiveList->end())
                  {
                    if ( (*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker )
                       {
                      // This is a CPP linemarker
                         int line = (*i)->get_lineNumberForCompilerGeneratedLinemarker();

                      // DQ (12/23/2008): Note this is a quoted name, we need the unquoted version!
                         std::string quotedFilename = (*i)->get_filenameForCompilerGeneratedLinemarker();
                         ROSE_ASSERT(quotedFilename[0] == '\"');
                         ROSE_ASSERT(quotedFilename[quotedFilename.length()-1] == '\"');
                         std::string filename = quotedFilename.substr(1,quotedFilename.length()-2);

                         std::string options  = (*i)->get_optionalflagsForCompilerGeneratedLinemarker();

                      // Add the new filename to the static map stored in the Sg_File_Info (no action if filename is already in the map).
                         Sg_File_Info::addFilenameToMap(filename);

                         int fileId = Sg_File_Info::getIDFromFilename(filename);

                         if ( SgProject::get_verbose() > 1 )
                              printf ("line = %d fileId = %d quotedFilename = %s filename = %s options = %s \n",line,fileId,quotedFilename.c_str(),filename.c_str(),options.c_str());

                      // Just record the first linemarker so that we can test getting the filename correct.
                         if (line == 1 && sourcePositionStack.empty() == true)
                            {
                              sourcePositionStack.push_front( pair<int,int>(line,fileId) );
                            }
                       }

                    i++;
                  }
             }

       // ROSE_ASSERT(sourcePositionStack.empty() == false);
          if (sourcePositionStack.empty() == false)
             {
               int line   = sourcePositionStack.front().first;
               int fileId = sourcePositionStack.front().second;

               if ( SgProject::get_verbose() > 1 )
                    printf ("Setting the source position of statement = %p = %s to line = %d fileId = %d \n",statement,statement->class_name().c_str(),line,fileId);

               statement->get_file_info()->set_file_id(fileId);
            // statement->get_file_info()->set_line(line);

               if ( SgProject::get_verbose() > 1 )
                    Sg_File_Info::display_static_data("Setting the source position of statement");

               string filename = Sg_File_Info::getFilenameFromID(fileId);

               if ( SgProject::get_verbose() > 1 )
                  {
                    printf ("filename = %s \n",filename.c_str());
                    printf ("filename = %s \n",statement->get_file_info()->get_filenameString().c_str());
                  }

               ROSE_ASSERT(statement->get_file_info()->get_filename() != NULL);
               ROSE_ASSERT(statement->get_file_info()->get_filenameString().empty() == false);
             }
        }
   }


void
SgSourceFile::processCppLinemarkers()
   {
     SgSourceFile* sourceFile = const_cast<SgSourceFile*>(this);

     SgSourceFile_processCppLinemarkers::LinemarkerTraversal linemarkerTraversal(sourceFile->get_sourceFileNameWithPath());

     linemarkerTraversal.traverse(sourceFile,preorder);

#if 0
     printf ("Exiting as a test ... (processing linemarkers)\n");
     ROSE_ASSERT(false);
#endif
   }

int
SgSourceFile::build_C_and_Cxx_AST( vector<string> argv, vector<string> inputCommandLine )
   {
     std::string frontEndCommandLineString;
     frontEndCommandLineString = std::string(argv[0]) + std::string(" ") + CommandlineProcessing::generateStringFromArgList(inputCommandLine,false,false);

     if ( get_verbose() > 1 )
          printf ("Before calling edg_main: frontEndCommandLineString = %s \n",frontEndCommandLineString.c_str());

     int edg_argc = 0;
     char **edg_argv = NULL;
     CommandlineProcessing::generateArgcArgvFromList(inputCommandLine, edg_argc, edg_argv);

#ifdef ROSE_BUILD_CXX_LANGUAGE_SUPPORT
  // This is the function call to the EDG front-end (modified in ROSE to pass a SgFile)
     int edg_main(int, char *[], SgSourceFile & sageFile );
     int frontendErrorLevel = edg_main (edg_argc, edg_argv, *this);
#else
     int frontendErrorLevel = 0;
#endif

     return frontendErrorLevel;
   }
 
int
SgSourceFile::build_PHP_AST()
   {
     string phpFileName = this->get_sourceFileNameWithPath();
#ifdef _MSC_VER
#pragma message ("WARNING: PHP not supported within initial MSVC port of ROSE.")
	 printf ("WARNING: PHP not supported within initial MSVC port of ROSE.");
	 ROSE_ASSERT(false);

	 int frontendErrorLevel = -1;
#else
#ifdef ROSE_BUILD_PHP_LANGUAGE_SUPPORT
     int frontendErrorLevel = php_main(phpFileName, this);
#else
     int frontendErrorLevel = 0;
#endif
#endif
     return frontendErrorLevel;
   }


/* Parses a single binary file and adds a SgAsmGenericFile node under this SgBinaryComposite node. */
void
SgBinaryComposite::buildAsmAST(string executableFileName)
   {
     if ( get_verbose() > 0 || SgProject::get_verbose() > 0)
          printf ("Disassemble executableFileName = %s \n",executableFileName.c_str());

  // Parse the binary container, but do not disassemble instructions yet.
     SgAsmGenericFile *file = SgAsmExecutableFileFormat::parseBinaryFormat(executableFileName.c_str());
     ROSE_ASSERT(file != NULL);

  // Attach the file to this node
     get_genericFileList()->get_files().push_back(file);
     file->set_parent(this);

  // Add a disassembly interpretation for each header. Actual disassembly will occur later.
  // NOTE: This probably isn't the right place to add interpretation nodes, but I'm leaving it here for the time being. We
  //       probably don't want an interpretation for each header if we're doing dynamic linking. [RPM 2009-09-17]
     const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
     for (size_t i = 0; i < headers.size(); ++i)
        {
          SgAsmInterpretation* interp = new SgAsmInterpretation();
          get_interpretations()->get_interpretations().push_back(interp);
          interp->set_parent(this);
          interp->get_headers()->get_headers().push_back(headers[i]);
        }

#if USE_ROSE_DWARF_SUPPORT
  // DQ (3/14/2009): Dwarf support now works within ROSE when used with Intel Pin
  // (was a huge problem until everything (e.g. libdwarf) was dynamically linked).
  // DQ (11/7/2008): New Dwarf support in ROSE (Dwarf IR nodes are generated in the AST).
     readDwarf(file);
#endif

  // Make sure this node is correctly parented
     SgProject* project = isSgProject(this->get_parent());
     ROSE_ASSERT(project != NULL);

#if 0
     printf ("At base of SgBinaryComposite::buildAsmAST(): exiting... \n");
     ROSE_ASSERT(false);
#endif
   }

/* Builds the entire AST under the SgBinaryComposite node:
 *    - figures out what binary files are needed
 *    - parses binary container of each file (SgAsmGenericFile nodes)
 *    - optionally disassembles instructions (SgAsmInterpretation nodes) */
int
SgBinaryComposite::buildAST(vector<string> /*argv*/, vector<string> /*inputCommandLine*/)
{

    /* Parse the specified binary file to create the AST. Do not disassemble instructions yet. If the file is dynamically
     * linked then optionally load (i.e., parse the container, map sections into process address space, and perform relocation
     * fixups) all dependencies also.  See the BinaryLoader class for details. */
    if (get_isLibraryArchive()) {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == false);
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == (get_isLibraryArchive() == false));

        for (size_t i = 0; i < get_libraryArchiveObjectFileNameList().size(); i++) {
            printf("Build binary AST for get_libraryArchiveObjectFileNameList()[%zu] = %s \n",
                    i, get_libraryArchiveObjectFileNameList()[i].c_str());
            string filename = "tmp_objects/" + get_libraryArchiveObjectFileNameList()[i];
            printf("Build SgAsmGenericFile from: %s \n", filename.c_str());
            buildAsmAST(filename);
        }
    } else {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty());
	BinaryLoader::load(this, get_read_executable_file_format_only());
    }

    /* Disassemble each interpretation */
    if (!get_read_executable_file_format_only()) {
        const SgAsmInterpretationPtrList &interps = get_interpretations()->get_interpretations();
        for (size_t i=0; i<interps.size(); i++) {
            Disassembler::disassembleInterpretation(interps[i]);
        }
    }

    // DQ (1/22/2008): The generated unparsed assemble code can not currently be compiled because the 
    // addresses are unparsed (see Jeremiah for details).
    // Skip running gnu assemble on the output since we include text that would make this a problem.
    if (get_verbose() > 1)
        printf("set_skipfinalCompileStep(true) because we are on a binary '%s'\n", this->get_sourceFileNameWithoutPath().c_str());
    this->set_skipfinalCompileStep(true);

    // This is now done below in the Secondary file processing phase.
    // Generate the ELF executable format structure into the AST
    // generateBinaryExecutableFileInformation(executableFileName,asmFile);

    int frontendErrorLevel = 0;
    return frontendErrorLevel;
}


#if 0
/* Builds the entire AST under the SgBinaryFile node:
 *    - figures out what binary files are needed
 *    - parses binary container of each file (SgAsmGenericFile nodes)
 *    - optionally disassembles instructions (SgAsmInterpretation nodes) */
int
SgBinaryFile::buildAST(vector<string> /*argv*/, vector<string> /*inputCommandLine*/)
{
    if (get_isLibraryArchive()) {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == false);
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == (get_isLibraryArchive() == false));

        for (size_t i = 0; i < get_libraryArchiveObjectFileNameList().size(); i++) {
            printf("Build binary AST for get_libraryArchiveObjectFileNameList()[%zu] = %s \n",
                    i, get_libraryArchiveObjectFileNameList()[i].c_str());
            string filename = "tmp_objects/" + get_libraryArchiveObjectFileNameList()[i];
            printf("Build SgAsmGenericFile from: %s \n", filename.c_str());
            buildAsmAST(filename);
        }
    } else {
        ROSE_ASSERT(get_libraryArchiveObjectFileNameList().empty() == true);
        buildAsmAST(this->get_sourceFileNameWithPath());
    }

    /* Disassemble each interpretation */
    if (get_read_executable_file_format_only()) {
        printf ("\nWARNING: Skipping instruction disassembly \n\n");
    } else {
        const SgAsmInterpretationPtrList &interps = get_interpretations()->get_interpretations();
        for (size_t i=0; i<interps.size(); i++) {
            Disassembler::disassembleInterpretation(interps[i]);
        }
    }

    // DQ (1/22/2008): The generated unparsed assemble code can not currently be compiled because the 
    // addresses are unparsed (see Jeremiah for details).
    // Skip running gnu assemble on the output since we include text that would make this a problem.
    if (get_verbose() > 1)
        printf("set_skipfinalCompileStep(true) because we are on a binary '%s'\n", this->get_sourceFileNameWithoutPath().c_str());
    this->set_skipfinalCompileStep(true);

    // This is now done below in the Secondary file processing phase.
    // Generate the ELF executable format structure into the AST
    // generateBinaryExecutableFileInformation(executableFileName,asmFile);

    int frontendErrorLevel = 0;
    return frontendErrorLevel;
}
#endif

int
SgSourceFile::buildAST( vector<string> argv, vector<string> inputCommandLine )
   {
  // printf ("######################## Calling SgSourceFile::buildAST() ##########################\n");

  // ROSE_ASSERT(false);

     int frontendErrorLevel = 0;
     if (get_Fortran_only() == true)
        {
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT           

#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
          frontendErrorLevel = build_Fortran_AST(argv,inputCommandLine);
#else
          fprintf(stderr, "USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT is not defined. Trying to parse a Fortran file when Fortran is not supported (ROSE must be configured using with Java (default)) and gfortran \n");
          ROSE_ASSERT(false);
#endif

#else
          fprintf(stderr, "ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT is not defined. Trying to parse a Fortran file when Fortran is not supported (ROSE must be configured using with Java (default)) \n");
          ROSE_ASSERT(false);
#endif

        }
       else
        {
          if ( get_PHP_only() == true )
             {
               frontendErrorLevel = build_PHP_AST();
             }
            else
             {
               frontendErrorLevel = build_C_and_Cxx_AST(argv,inputCommandLine);
             }
        }

  // Uniform processing of the error code!

     if ( get_verbose() > 1 )
          printf ("DONE: frontend called (frontendErrorLevel = %d) \n",frontendErrorLevel);

  // DQ (12/29/2008): The newer version of EDG (version 3.10 and 4.0) use different return codes for indicating an error.
     bool frontend_failed = false;
#ifdef ROSE_USE_NEW_EDG_INTERFACE
  // Any non-zero value indicates an error.
     frontend_failed = (frontendErrorLevel != 0);
#else
  // non-zero error code can mean warnings were produced, values greater than 3 indicate errors.
     frontend_failed = (frontendErrorLevel > 3);
#endif

  // If we had any errors reported by the frontend then quite now
     if (frontend_failed)
        {
       // cout << "Errors in Processing: (frontendErrorLevel > 3)" << endl;
          if ( get_verbose() > 1 )
               printf ("frontendErrorLevel = %d \n",frontendErrorLevel);

       // DQ (9/22/2006): We need to invert the test result (return code) for 
       // negative tests (where failure is expected and success is an error).
          if (get_negative_test() == true)
             {
               if ( get_verbose() > 1 )
                  {
                    printf ("(evaluation of frontend results) This is a negative tests, so an error in compilation is a PASS but a successful \n");
                    printf ("compilation is not a FAIL since the faulure might happen in the compilation of the generated code by the vendor compiler. \n");
                  }
               exit(0);
             }
            else
             {
            // Exit because there are errors in the input program
               //cout << "Errors in Processing: (frontend_failed)" << endl;
               ROSE_ABORT("Errors in Processing: (frontend_failed)");
             }
        }

     return frontendErrorLevel;
   }



vector<string>
SgFile::buildCompilerCommandLineOptions ( vector<string> & argv, int fileNameIndex, const string& compilerName )
   {
  // This function assembles the commandline that will be passed to the backend (vendor) C++/C compiler 
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
     printf ("get_C_only()              = %s \n",(get_C_only() == true) ? "true" : "false");
     printf ("get_C99_only()            = %s \n",(get_C99_only() == true) ? "true" : "false");
     printf ("get_Cxx_only()            = %s \n",(get_Cxx_only() == true) ? "true" : "false");
     printf ("get_Fortran_only()        = %s \n",(get_Fortran_only() == true) ? "true" : "false");
     printf ("get_F77_only()            = %s \n",(get_F77_only() == true) ? "true" : "false");
     printf ("get_F90_only()            = %s \n",(get_F90_only() == true) ? "true" : "false");
     printf ("get_F95_only()            = %s \n",(get_F95_only() == true) ? "true" : "false");
     printf ("get_F2003_only()          = %s \n",(get_F2003_only() == true) ? "true" : "false");
     printf ("get_CoArrayFortran_only() = %s \n",(get_CoArrayFortran_only() == true) ? "true" : "false");
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
       // Liao, 9/22/2009, we also specify the search path for libgomp_g.h, which is installed under $ROSE_INS/include
       // and the path to libgomp.a/libgomp.so, which are located in $GCC_GOMP_OPENMP_LIB_PATH

       // Header should always be available 
       // the conditional compilation is necessary to pass make distcheck,
       // where only a minimum configuration options are used and not all macros are defined. 
#ifdef ROSE_INSTALLATION_PATH 
       string include_path(ROSE_INSTALLATION_PATH);
       include_path += "/include"; 
       compilerNameString.push_back("-I"+include_path); 
#endif
#if 0  // moved to the very end       
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY       
       // lib path is available if --with-gomp_omp_runtime_library=XXX is used
       if (USE_ROSE_GOMP_OPENMP_LIBRARY)
       {
         // only linking phase needs this
         if (!get_compileOnly()) 
         {
           string gomp_lib_path(GCC_GOMP_OPENMP_LIB_PATH);
           ROSE_ASSERT (gomp_lib_path.size() != 0);
 //          compilerNameString.push_back("-L"+gomp_lib_path); 
           compilerNameString.push_back(gomp_lib_path+"/libgomp.a"); // static linking for simplicity
           compilerNameString.push_back("-lpthread"); 
         }
       }
#endif       
#endif       
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
#if 1       
       // Liao, 5/15/2009
       // the input source file's path has to be the first one to be searched for header!
       // This is required since one of the SPEC CPU 2006 benchmarks: gobmk relies on this to be compiled.
          vector<string>::iterator iter;
       // find the very first -Ixxx option's position
          for (iter = compilerNameString.begin(); iter != compilerNameString.end(); iter++) 
             {
               string cur_string =*iter;
               string::size_type pos = cur_string.find("-I",0);
               if (pos==0)
                    break;
             }

       // insert before the position
          compilerNameString.insert(iter, std::string("-I") + oldFileNamePathOnly); 
#else        
          compilerNameString.push_back(std::string("-I") + oldFileNamePathOnly);
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

       // DQ (7/14/2004): Suggested fix from Andreas, make the object file name explicit
          if (objectNameSpecified == false)
             {
             //  cout<<"making object file explicit for compilation only mode without -o options"<<endl;
               compilerNameString.push_back("-o");
               compilerNameString.push_back(currentDirectory + "/" + objectFileName);
             }
        }
       else
        {
       // Liao 11/19/2009, changed to support linking multiple source files within one command line
       // We change the compilation mode for each individual file to compile-only even
       // when the original command line is to generate the final executable.
       // We generate the final executable at the SgProject level from object files of each source file

       // cout<<"turn on compilation only at the file compilation level"<<endl;
          compilerNameString.push_back("-c");
       // For compile+link mode, -o is used for the final executable, if it exists
       // We make -o objectfile explicit 
          std::string objectFileName = generateOutputFileName();
          compilerNameString.push_back("-o");
          compilerNameString.push_back(currentDirectory + "/" + objectFileName);
        }

#if 0
     printf ("At base of buildCompilerCommandLineOptions: compilerNameString = \n%s\n",CommandlineProcessing::generateStringFromArgList(compilerNameString,false,false).c_str());
#endif
#if 0
     printf ("Exiting at base of buildCompilerCommandLineOptions() ... \n");
     ROSE_ASSERT (false);
#endif

#if 0
  // moved to the linking phase function of SgProject
  // Liao, 9/23/2009, optional linker flags to support OpenMP lowering targeting GOMP
     if (get_openmp_lowering())
     {
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY       
       // lib path is available if --with-gomp_omp_runtime_library=XXX is used
       if (USE_ROSE_GOMP_OPENMP_LIBRARY)
       {
         // only linking phase needs this
         if (!get_compileOnly()) 
         {
           string gomp_lib_path(GCC_GOMP_OPENMP_LIB_PATH);
           ROSE_ASSERT (gomp_lib_path.size() != 0);
           //          compilerNameString.push_back("-L"+gomp_lib_path); 
           compilerNameString.push_back(gomp_lib_path+"/libgomp.a"); // static linking for simplicity
           compilerNameString.push_back("-lpthread"); 
         }
       }
#endif          
     }

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


int
SgFile::compileOutput ( vector<string>& argv, int fileNameIndex, const string& compilerNameOrig )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (compile output):");

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

#if 0
     printf ("\n\n***************************************************** \n");
     printf ("Calling SgFile::compileOutput() \n");
     printf ("***************************************************** \n\n\n");
#endif

  // This function does the final compilation of the unparsed file
  // Remaining arguments from the original compile time are used as well
  // as additional arguments added by the buildCompilerCommandLineOptions() function

  // DQ NOTE: This function has to be modified to compile more than 
  //       just one file (as part of the multi-file support)
  // ROSE_ASSERT (sageProject.numberOfFiles() == 1);

  // ******************************************************************************
  // At this point in the control flow (for ROSE) we have returned from the processing 
  // via the EDG frontend (or skipped it if that option was specified).
  // The following has been done or explicitly skipped if such options were specified 
  // on the commandline:
  //    1) The application program has been parsed
  //    2) All AST's have been build (one for each grammar)
  //    3) The transformations have been edited into the C++ AST
  //    4) The C++ AST has been unparsed to form the final output file (all code has 
  //       been generated into a different filename)
  // ******************************************************************************

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code).
     int returnValueForRose = 0;

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

  // BP : 11/13/2001, checking to see that the compiler name is set
     string compilerName = compilerNameOrig + " ";

  // DQ (4/21/2006): Setup the output file name.
  // Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argc,argv);
  // ROSE_ASSERT (fileList.size() == 1);
  // p_sourceFileNameWithPath    = *(fileList.begin());
  // p_sourceFileNameWithoutPath = ROSE::stripPathFromFileName(p_sourceFileNameWithPath.c_str());

#if 1
  // ROSE_ASSERT (get_unparse_output_filename().empty() == true);

  // DQ (4/21/2006): If we have not set the unparse_output_filename then we could not have called 
  // unparse and we want to compile the original source file as a backup mechanism to generate an 
  // object file.
     if (get_unparse_output_filename().empty() == true)
        {
          ROSE_ASSERT(get_skip_unparse() == true);
          string outputFilename = get_sourceFileNameWithPath();
#if 0
          if (get_skip_unparse() == true)
             {
            // We we are skipping the unparsing then we didn't generate an intermediate 
            // file and so we want to compile the original source file.
               outputFilename = get_sourceFileNameWithPath();
             }
            else
             {
            // If we did unparse an intermediate file then we want to compile that 
            // file instead of the original source file.
               outputFilename = "rose_" + get_sourceFileNameWithoutPath();
             }
#endif
          set_unparse_output_filename(outputFilename);

       // DQ (6/25/2006): I think it is OK to not have an output file name specified.
       // display ("In SgFile::compileOutput(): get_unparse_output_filename().empty() == true");
        }
#endif

     ROSE_ASSERT (get_unparse_output_filename().empty() == false);

  // Now call the compiler that rose is replacing
  // if (get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= 1 )
        {
          printf ("Now call the backend (vendor's) compiler compilerNameOrig = %s for file = %s \n",compilerNameOrig.c_str(),get_unparse_output_filename().c_str());
        }

  // Build the commandline to hand off to the C++/C compiler
     vector<string> compilerNameString = buildCompilerCommandLineOptions (argv,fileNameIndex, compilerName );
  // ROSE_ASSERT (compilerNameString != NULL);

  // printf ("SgFile::compileOutput(): compilerNameString = \n%s\n",CommandlineProcessing::generateStringFromArgList(compilerNameString,false,false).c_str());

     int returnValueForCompiler = 0;

  // error checking
  // display("Called from SgFile::compileOutput()");

  // Allow conditional skipping of the final compile step for testing ROSE.
  // printf ("SgFile::compileOutput(): get_skipfinalCompileStep() = %s \n",get_skipfinalCompileStep() ? "true" : "false");
     if (get_skipfinalCompileStep() == false)
        {
       // Debugging code
          if ( get_verbose() > 1 )
             {
               printf ("calling systemFromVector() \n");
               printf ("Number of command line arguments: %zu\n", compilerNameString.size());
               for (size_t i = 0; i < compilerNameString.size(); ++i)
                  {
                    printf ("Backend compiler arg[%zu]: = %s\n", i, compilerNameString[i].c_str());
                  }
               printf("End of command line for backend compiler\n");

            // I need the exact command line used to compile the generate code with the backendcompiler (so that I can reuse it to test the generated code).
               printf ("SgFile::compileOutput(): compilerNameString = \n%s\n",CommandlineProcessing::generateStringFromArgList(compilerNameString,false,false).c_str());
             }

       // Call the backend compiler. For Fortran inputs, if ROSE is configured with Java this can cause the backend fortran compiler to be called.
          returnValueForCompiler = systemFromVector (compilerNameString);
        }
       else
        {
          if ( get_verbose() > 1 )
               printf ("COMPILER NOT CALLED: compilerNameString = %s \n", "<unknown>" /* compilerNameString.c_str() */);

       // DQ (8/21/2008): If this is a binary then we don't need the message output.
       // Liao 8/29/2008: disable it for non-verbose model
          if ( (get_binary_only() == false) && (get_verbose() > 0) )
             {
               printf ("Skipped call to backend vendor compiler! \n");
             }
        }

  // DQ (7/20/2006): Catch errors returned from unix "system" function 
  // (commonly "out of memory" errors, suggested by Peter and Jeremiah).
     if (returnValueForCompiler < 0)
        {
          perror("Serious Error returned from internal systemFromVector command");
        }

  // Assemble an exit status that combines the values for ROSE and the C++/C compiler
  // return an exit status which is the boolean OR of the bits from the EDG/SAGE/ROSE and the compile step
     int finalCompiledExitStatus = returnValueForRose | returnValueForCompiler;

  // It is a strange property of the UNIX $status that it does not map uniformally from 
  // the return value of the "exit" command (or "return" statement).  So if the exit 
  // status from the compilation stage is nonzero then we just make the exit status 1
  // (this does seem to be a portable solution).
  // FYI: only the first 8 bits of the exit value are significant (Solaris uses 'exit_value mod 256').
     if (finalCompiledExitStatus != 0)
        {
       // If this it is non-zero then make it 1 to be more clear to external tools (e.g. make)
          finalCompiledExitStatus = 1;
        }

  // DQ (9/19/2006): We need to invert the test result (return code) for 
  // negative tests (where failure is expected and success is an error).
     if (get_negative_test() == true)
        {
          if ( get_verbose() > 1 )
               printf ("This is a negative tests, so an error in compilation is a PASS and successful compilation is a FAIL (vendor compiler return value = %d) \n",returnValueForCompiler);

          finalCompiledExitStatus = (finalCompiledExitStatus == 0) ? /* error */ 1 : /* success */ 0;
        }

  // printf ("Program Terminated Normally (exit status = %d)! \n\n\n\n",finalCompiledExitStatus);

     return finalCompiledExitStatus;
   }


//! project level compilation and linking
// three cases: 1. preprocessing only
//              2. compilation: 
//              3. linking:
int
SgProject::compileOutput( const std::string& compilerName )
   {
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Backend Compilation (SgProject):");

     int errorCode = 0;
     int linkingReturnVal = 0;
     int i = 0;

     if (numberOfFiles() == 0)
        {
       // printf ("Note in SgProject::compileOutput(%s): numberOfFiles() == 0 \n",compilerName.c_str());
          printf ("ROSE using %s as backend compiler: no input files \n",compilerName.c_str());

       // DQ (8/24/2008): We can't recreate same behavior on exit as GNU on exit with no
       // files since it causes the test immediately after building librose.so to fail.
       // exit(1);
        }

  // printf ("In SgProject::compileOutput(): get_C_PreprocessorOnly() = %s \n",get_C_PreprocessorOnly() ? "true" : "false");

  // case 1: preprocessing only
     if (get_C_PreprocessorOnly() == true)
        {
       // DQ (10/16/2005): Handle special case (issue a single compile command for all files)
          vector<string> argv = get_originalCommandLineArgumentList();

       // strip out any rose options before passing the command line.
          SgFile::stripRoseCommandLineOptions( argv );

       // strip out edg specific options that would cause an error in the backend linker (compiler).
          SgFile::stripEdgCommandLineOptions( argv );

       // Skip the name of the ROSE translator (so that we can insert the backend compiler name, below)
       // bool skipInitialEntry = true;

       // Include all the specified source files
       // bool skipSourceFiles  = false;

          vector<string> originalCommandLine = argv;
          ROSE_ASSERT (!originalCommandLine.empty());

       // DQ (8/13/2006): Use the path to the compiler specified as that backend compiler (should not be specifi to GNU!)
       // DQ (8/6/2006): Test for g++ and use gcc with "-E" option 
       // (makes a different for header file processing in ARES configuration)
       // string compilerNameString = compilerName;
          string& compilerNameString = originalCommandLine[0];
          if (get_C_only() == true)
             {
               compilerNameString = BACKEND_C_COMPILER_NAME_WITH_PATH;
             }
            else
             {
               compilerNameString = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
               if (get_Fortran_only() == true)
                  {
                    compilerNameString = "f77";
                  }
             }

       // DQ (8/13/2006): Add a space to avoid building "g++-E" as output.
       // compilerNameString += " ";

       // Prepend the compiler name to the original command line
       // originalCommandLine = std::string(compilerName) + std::string(" ") + originalCommandLine;
       // originalCommandLine = compilerNameString + originalCommandLine;

       // Prepend the compiler name to the original command line
       // originalCommandLine = std::string(compilerName) + std::string(" ") + originalCommandLine;

       // printf ("originalCommandLine = %s \n",originalCommandLine.c_str());

#if 0
          printf ("Support for \"-E\" not implemented yet. \n");
          ROSE_ASSERT(false);
#endif

          errorCode = systemFromVector(originalCommandLine);

#if 0
       // DQ (8/22/2009): We test the parent of SgFunctionTypeTable in the AST post processing,
       // so we need to make sure that it is set.
          SgFunctionTypeTable* functionTypeTable = SgNode::get_globalFunctionTypeTable();
          ROSE_ASSERT(functionTypeTable != NULL);
          if (functionTypeTable->get_parent() == NULL)
             {
               ROSE_ASSERT(numberOfFiles() > 0);
               printf ("set the parent of SgFunctionTypeTable \n");
               functionTypeTable->set_parent(&get_file(0));
             }
          ROSE_ASSERT(functionTypeTable->get_parent() != NULL);
#endif

          ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
          ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_parent() != NULL);

       // printf ("Exiting after call to compiler using -E option! \n");
       // ROSE_ASSERT(false);
        }
       else // non-preprocessing-only case
        {
       // printf ("In Project::compileOutput(): Compiling numberOfFiles() = %d \n",numberOfFiles());

// case 2: compilation  for each file 
       // Typical case
          for (i=0; i < numberOfFiles(); i++)
             {
               SgFile & file = get_file(i);
#if 0
               printf ("In Project::compileOutput(%s): (in loop) get_file(%d).get_skipfinalCompileStep() = %s \n",compilerName,i,(get_file(i).get_skipfinalCompileStep()) ? "true" : "false");
#endif
            // printf ("In Project::compileOutput(): (TOP of loop) file = %d \n",i);

            // DQ (8/13/2006): Only use the first file (I don't think this
            // makes sense with multiple files specified on the commandline)!
            // int localErrorCode = file.compileOutput(i, compilerName);
               int localErrorCode = file.compileOutput(0, compilerName);

               if (localErrorCode > errorCode)
                    errorCode = localErrorCode;

            // printf ("In Project::compileOutput(): (BASE of loop) file = %d errorCode = %d localErrorCode = %d \n",i,errorCode,localErrorCode);
             }

// case 3: linking at the project level
           // Liao, 11/19/2009, 
           // I really want to just move the SgFile::compileOutput() to SgProject::compileOutput() 
           // and have both compilation and linking finished at the same time, just as the original command line does.
           // Then we don't have to compose compilation command line for each of the input source file
           //  or to compose the final linking command line. 
           //
           // But there may be some advantages of doing the compilation and linking separately at two levels.
           // I just discussed it with Dan.
           // The two level scheme is needed to support mixed language input, like a C file and a Fortran file
           // In this case, we cannot have a single one level command line to compile and link those two files
           // We have to compile each of them first and finally link the object files.
#ifndef _MSC_VER
		  // tps 08/18/2010 : Do not link right now in Windows - it breaks - want test to pass here for now.
		  // todo windows: put this back in.
		  linkingReturnVal = link (compilerName);
#else
#pragma message ("sageSupport.C : linkingReturnVal = link (compilerName); not implemented yet.")
#endif
        } // end if preprocessing-only is false

     //return errorCode;
     return errorCode + linkingReturnVal;
   }


bool
SgFile::isPrelinkPhase() const
   {
  // This function checks if the "-prelink" option was passed to the ROSE preprocessor
  // It could alternatively just check the commandline and set a flag in the SgFile object.
  // But then there would be a redundent flag in each SgFile object (perhaps the design needs to 
  // be better, using a common base class for commandline options (in both the SgProject and 
  // the SgFile (would not be a new IR node)).

     bool returnValue = false;

  // DQ (5/9/2004): If the parent is not set then this was compiled as a SgFile directly
  // (likely by the rewrite mechanism). IF so it can't be a prelink phase, which is 
  // called only on SgProjects). Not happy with this mechanism!
     if (get_parent() != NULL)
        {
          ROSE_ASSERT ( get_parent() != NULL );

       // DQ (1/24/2010): Now that we have directory support, the parent of a SgFile does not have to be a SgProject.
       // SgProject* project = isSgProject(get_parent());
          SgProject* project = TransformationSupport::getProject(this);

          ROSE_ASSERT ( project != NULL );
          returnValue = project->get_prelink();
        }

     return returnValue;

  // Note that project can be false if this is called on an intermediate file 
  // generated by the rewrite system.
  // return (project == NULL) ? false : project->get_prelink();
   }

//! Preprocessing command line and pass it to generate the final linking command line
int
SgProject::link ( std::string linkerName )
   {
  // DQ (1/25/2010): We have to now test for both numberOfFiles() and numberOfDirectories(),
  // or perhaps define a more simple function to use more directly.
  // Liao, 11/20/2009
  // translator test1.o will have ZERO SgFile attached with SgProject
  // Special handling for this case
  // if (numberOfFiles() == 0)
     if (numberOfFiles() == 0 && numberOfDirectories() == 0)
        {
          if (get_verbose() > 0)
               cout << "SgProject::link maybe encountering an object file ..." << endl;

       // DQ (1/24/2010): support for directories not in place yet.
          if (numberOfDirectories() > 0)
             {
               printf ("Directory support for linking not implemented... (unclear what this means...)\n");
               return 0;
             }
        }
       else
        {
       // normal cases that rose translators will actually do something about the input files
       // and we have SgFile for each of the files.
       // if ((numberOfFiles()== 0) || get_compileOnly() || get_file(0).get_skipfinalCompileStep() 
          if ( get_compileOnly() || get_file(0).get_skipfinalCompileStep() ||get_file(0).get_skip_unparse())
             {
               if (get_verbose() > 0)
                    cout << "Skipping SgProject::link ..." << endl;
               return 0;
             }
        }

  // Compile the output file from the unparsing
     vector<string> argcArgvList = get_originalCommandLineArgumentList();

  // error checking
     if (numberOfFiles()!= 0) 
          ROSE_ASSERT (argcArgvList.size() > 1);
     ROSE_ASSERT(linkerName != "");

  // strip out any rose options before passing the command line.
     SgFile::stripRoseCommandLineOptions( argcArgvList );

  // strip out edg specific options that would cause an error in the backend linker (compiler).
     SgFile::stripEdgCommandLineOptions( argcArgvList );

   // remove the original compiler/linker name
     argcArgvList.erase(argcArgvList.begin()); 

     // remove all original file names
     Rose_STL_Container<string> sourceFilenames = get_sourceFileNameList();
     for (Rose_STL_Container<string>::iterator i = sourceFilenames.begin(); i != sourceFilenames.end(); i++)
     {
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
         argcArgvList.remove(*i);
       }
#else
       if (find(argcArgvList.begin(),argcArgvList.end(),*i) != argcArgvList.end())
       {
         argcArgvList.erase(find(argcArgvList.begin(),argcArgvList.end(),*i));
       }
#endif
     }
     // fix double quoted strings
     // DQ (4/14/2005): Fixup quoted strings in args fix "-DTEST_STRING_MACRO="Thu Apr 14 08:18:33 PDT 2005" 
     // to be -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\"  This is a problem in the compilation of
     // a Kull file (version.cc), when the backend is specified as /usr/apps/kull/tools/mpig++-3.4.1.  The
     // problem is that /usr/apps/kull/tools/mpig++-3.4.1 is a wrapper for a shell script /usr/local/bin/mpiCC
     // which does not tend to observe quotes well.  The solution is to add additional escaped quotes.
     for (Rose_STL_Container<string>::iterator i = argcArgvList.begin(); i != argcArgvList.end(); i++)
     {
       std::string::size_type startingQuote = i->find("\"");
       if (startingQuote != std::string::npos)
       {
         std::string::size_type endingQuote   = i->rfind("\"");

         // There should be a double quote on both ends of the string
         ROSE_ASSERT (endingQuote != std::string::npos);

         std::string quotedSubstring = i->substr(startingQuote,endingQuote);

         std::string fixedQuotedSubstring = std::string("\\\"") + quotedSubstring + std::string("\\\"");

         // Now replace the quotedSubstring with the fixedQuotedSubstring
         i->replace(startingQuote,endingQuote,fixedQuotedSubstring);

         // printf ("Modified argument = %s \n",(*i).c_str());
       }
     }  

  // Call the compile
     int errorCode = link ( argcArgvList, linkerName );

  // return the error code from the compilation
     return errorCode;
   }

int
SgProject::link ( const std::vector<std::string>& argv, std::string linkerName )
   {
       // argv.size could be 0 after strip off compiler name, original source file, etc
      // ROSE_ASSERT(argv.size() > 0);

  // This link function will be moved into the SgProject IR node when complete
     const std::string whiteSpace = " ";
  // printf ("This link function is not longer called (I think!) \n");
  // ROSE_ASSERT(false);

  // DQ (10/15/2005): Trap out case of C programs where we want to make sure that we don't use the C++ compiler to do our linking!
     if (get_C_only() == true || get_C99_only() == true)
        {
          linkerName = BACKEND_C_COMPILER_NAME_WITH_PATH;
        }
       else
        {
       // The default is C++
          linkerName = BACKEND_CXX_COMPILER_NAME_WITH_PATH;

          if (get_Fortran_only() == true)
             {
            // linkerName = "f77 ";
               linkerName = BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH;
             }
        }

  // This is a better implementation since it will include any additional command line options that target the linker
     Rose_STL_Container<string> linkingCommand ; 

     linkingCommand.push_back (linkerName);
     // find all object files generated at file level compilation
     // The assumption is that -o objectFileName is made explicit and
     // is generated by SgFile::generateOutputFileName()
     for (int i=0; i < numberOfFiles(); i++)
        {
          linkingCommand.push_back(get_file(i).generateOutputFileName());
        }

  // Add any options specified in the original command line (after preprocessing)
     linkingCommand.insert(linkingCommand.end(), argv.begin(), argv.end());

  // Check if -o option exists, otherwise append -o a.out to the command line

  // Additional libraries to be linked with
  // Liao, 9/23/2009, optional linker flags to support OpenMP lowering targeting GOMP
     if ((numberOfFiles() !=0) && (get_file(0).get_openmp_lowering()))
     {

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY       
       // add libxomp.a , Liao 6/12/2010
       string xomp_lib_path(ROSE_INSTALLATION_PATH);
       ROSE_ASSERT (xomp_lib_path.size() != 0);
       linkingCommand.push_back(xomp_lib_path+"/lib/libxomp.a"); // static linking for simplicity

       // lib path is available if --with-gomp_omp_runtime_library=XXX is used
         string gomp_lib_path(GCC_GOMP_OPENMP_LIB_PATH);
         ROSE_ASSERT (gomp_lib_path.size() != 0);
         linkingCommand.push_back(gomp_lib_path+"/libgomp.a"); 
         linkingCommand.push_back("-lpthread");
#else
  // GOMP has higher priority when both GOMP and OMNI are specified (wrongfully)
  #ifdef OMNI_OPENMP_LIB_PATH
           // a little redundant code to defer supporting 'ROSE_INSTALLATION_PATH' in cmake
           string xomp_lib_path(ROSE_INSTALLATION_PATH);
           ROSE_ASSERT (xomp_lib_path.size() != 0);
           linkingCommand.push_back(xomp_lib_path+"/lib/libxomp.a"); 

           string omni_lib_path(OMNI_OPENMP_LIB_PATH);
           ROSE_ASSERT (omni_lib_path.size() != 0);
           linkingCommand.push_back(omni_lib_path+"/libgompc.a"); 
           linkingCommand.push_back("-lpthread");
  #else
     printf("Warning: OpenMP lowering is requested but no target runtime library is specified!\n");
  #endif
#endif
     }

     if ( get_verbose() > 0 )
        {
          printf ("In SgProject::link command line = %s \n",CommandlineProcessing::generateStringFromArgList(linkingCommand,false,false).c_str());
        }

     int status = systemFromVector(linkingCommand);

     if ( get_verbose() > 1 )
          printf ("linker error status = %d \n",status);

     return status;
   }


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
"This ROSE translator provides a means for operating on C, C++, and Fortran\n"
"source code, as well as on x86, ARM, and PowerPC object code.\n"
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
"     -rose:upc_threads n     Enable UPC static threads compilation with n threads\n"
"                             n>=1: static threads; dynamic(default) otherwise\n"
"     -rose:Fortran, -rose:F, -rose:f\n"
"                             compile Fortran code, determining version of\n"
"                             Fortran from file suffix)\n"
"     -rose:CoArrayFortran, -rose:CAF, -rose:caf\n"
"                             compile Co-Array Fortran code (extension of Fortran 2003)\n"
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
"     -rose:skip_rose         process command line and call backend directly,\n"
"                             skipping all ROSE-specific processing\n"
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
"                             most of the SgAsmFunctionDeclaration::FunctionReason\n"
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



// DQ (12/22/2005): Jochen's support for a constant (non-NULL) valued pointer
// to use to distinguish valid from invalid IR nodes within the memory pools.
namespace AST_FileIO
   {
     SgNode* IS_VALID_POINTER()
        {
       // static SgNode* value = (SgNode*)(new char[1]);

       // DQ (1/17/2006): Set to the pointer value 0xffffffff (as used by std::string::npos)
          static SgNode* value = (SgNode*)(std::string::npos);
       // printf ("In AST_FileIO::IS_VALID_POINTER(): value = %p \n",value);

          return value;
        }

  // similar vlue for reprentation of subsets of the AST 
     SgNode* TO_BE_COPIED_POINTER()
        {
       // static SgNode* value = (SgNode*)(new char[1]);
          static SgNode* value = (SgNode*)((std::string::npos) - 1);
       // printf ("In AST_FileIO::TO_BE_COPIED_POINTER(): value = %p \n",value);
          return value;
        }
   }


//! Prints pragma associated with a grammatical element.
/*!       (fill in more detail here!)
 */
void
print_pragma(SgAttributePtrList& pattr, std::ostream& os)
   {
     SgAttributePtrList::const_iterator p = pattr.begin();
     if (p == pattr.end())
          return;
       else
          p++;

     while (p != pattr.end())
        {
          if ( (*p)->isPragma() )
             {
               SgPragma *pr = (SgPragma*)(*p);
               if (!pr->gotPrinted())
                  {
                    os << std::endl << "#pragma " <<pr->get_name() << std::endl;
                    pr->setPrinted(1);
                  }
             }
          p++;
        }
   }




// Temporary function to be later put into Sg_FileInfo
StringUtility::FileNameLocation
get_location ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return StringUtility::FILENAME_LOCATION_UNKNOWN;
     return classification.getLocation();
   }

StringUtility::FileNameLibrary
get_library ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return StringUtility::FILENAME_LIBRARY_UNKNOWN;
     return classification.getLibrary();
   }

std::string
get_libraryName ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return "";
     return classification.getLibraryName();
   }

StringUtility::OSType
get_OS_type ()
   {
  // return StringUtility::OS_TYPE_UNKNOWN;
     return StringUtility::getOSType();
   }

int
get_distanceFromSourceDirectory ( Sg_File_Info* X )
   {
     SgFile* file = TransformationSupport::getFile(X->get_parent());
     ROSE_ASSERT(file != NULL);
     string sourceFilename = file->getFileName();
     string sourceDirectory = StringUtility::getPathFromFileName(sourceFilename);

     StringUtility::FileNameClassification classification = StringUtility::classifyFileName(X->get_filenameString(),sourceDirectory,StringUtility::getOSType());

  // return 0;
     return classification.getDistanceFromSourceDirectory();
   }





int
SgNode::numberOfNodesInSubtree()
   {
     int value = 0;

     class CountTraversal : public SgSimpleProcessing
        {
          public:
              int count;
              CountTraversal() : count(0) {}
              void visit ( SgNode* n ) { count++; }
        };

     CountTraversal counter;
     SgNode* thisNode = const_cast<SgNode*>(this);
     counter.traverse(thisNode,preorder);
     value = counter.count;

     return value;
   }

namespace SgNode_depthOfSubtree
   {
  // This class (AST traversal) could not be defined in the function SgNode::depthOfSubtree()
  // So I have constructed a namespace for this class to be implemented outside of the function.

     class DepthInheritedAttribute
        {
          public:
               int treeDepth;
               DepthInheritedAttribute( int depth ) : treeDepth(depth) {}         
        };

     class MaxDepthTraversal : public AstTopDownProcessing<DepthInheritedAttribute>
        {
          public:
              int maxDepth;
              MaxDepthTraversal() : maxDepth(0) {}

              DepthInheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, DepthInheritedAttribute inheritedAttribute )
                 {
                   if (inheritedAttribute.treeDepth > maxDepth)
                        maxDepth = inheritedAttribute.treeDepth;
#if 0
                   printf ("maxDepth = %d for IR nodes = %p = %s \n",maxDepth,astNode,astNode->class_name().c_str());
#endif
                   return DepthInheritedAttribute(inheritedAttribute.treeDepth + 1);
                 }
        };
   }

int
SgNode::depthOfSubtree()
   {
     int value = 0;

     SgNode_depthOfSubtree::MaxDepthTraversal depthCounter;
     SgNode_depthOfSubtree::DepthInheritedAttribute inheritedAttribute(0);
     SgNode* thisNode = const_cast<SgNode*>(this);

     depthCounter.traverse(thisNode,inheritedAttribute);

     value = depthCounter.maxDepth;

     return value;
   }

#if 0
// We only need one definition for this function at the SgNode IR node.
size_t
SgFile::numberOfNodesInSubtree()
   {
     printf ("Base class of virtual function (SgFile::numberOfNodesInSubtree() should not be called! \n");
     ROSE_ASSERT(false);

     return 0;
   }

size_t
SgSourceFile::numberOfNodesInSubtree()
   {
     return get_globalScope()->numberOfNodesInSubtree() + 1;
   }

size_t
SgBinaryComposite::numberOfNodesInSubtree()
   {
     return get_binaryFile()->numberOfNodesInSubtree() + 1;
   }
#endif


// DQ (10/3/2008): Added support for getting interfaces in a module
std::vector<SgInterfaceStatement*>
SgModuleStatement::get_interfaces() const
   {
     std::vector<SgInterfaceStatement*> returnList;

     SgModuleStatement* definingModuleStatement = isSgModuleStatement(get_definingDeclaration());
     ROSE_ASSERT(definingModuleStatement != NULL);

     SgClassDefinition* moduleDefinition = definingModuleStatement->get_definition();
     ROSE_ASSERT(moduleDefinition != NULL);

     SgDeclarationStatementPtrList & declarationList = moduleDefinition->getDeclarationList();

     SgDeclarationStatementPtrList::iterator i = declarationList.begin();
     while (i != declarationList.end())
        {
          SgInterfaceStatement* interfaceStatement = isSgInterfaceStatement(*i);
          if (interfaceStatement != NULL)
             {
               returnList.push_back(interfaceStatement);
             }

          i++;
        }

     return  returnList;
   }

// DQ (11/23/2008): This is a static function
SgC_PreprocessorDirectiveStatement*
SgC_PreprocessorDirectiveStatement::createDirective ( PreprocessingInfo* currentPreprocessingInfo )
   {
  // This is the new factory interface to build CPP directives as IR nodes.
     PreprocessingInfo::DirectiveType directive = currentPreprocessingInfo->getTypeOfDirective();

  // SgC_PreprocessorDirectiveStatement* cppDirective = new SgEmptyDirectiveStatement();
     SgC_PreprocessorDirectiveStatement* cppDirective = NULL;

     switch(directive)
        {
          case PreprocessingInfo::CpreprocessorUnknownDeclaration:
             {
            // I think this is an error...
            // locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
               printf ("Error: directive == PreprocessingInfo::CpreprocessorUnknownDeclaration \n");
               ROSE_ASSERT(false);
               break;
             }

          case PreprocessingInfo::C_StyleComment:
          case PreprocessingInfo::CplusplusStyleComment:
          case PreprocessingInfo::FortranStyleComment:
          case PreprocessingInfo::CpreprocessorBlankLine:
          case PreprocessingInfo::ClinkageSpecificationStart:
          case PreprocessingInfo::ClinkageSpecificationEnd:
             {
               printf ("Error: these cases chould not generate a new IR node (directiveTypeName = %s) \n",PreprocessingInfo::directiveTypeName(directive).c_str());
               ROSE_ASSERT(false);
               break;
             }

          case PreprocessingInfo::CpreprocessorIncludeDeclaration:          { cppDirective = new SgIncludeDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:      { cppDirective = new SgIncludeNextDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorDefineDeclaration:           { cppDirective = new SgDefineDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorUndefDeclaration:            { cppDirective = new SgUndefDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIfdefDeclaration:            { cppDirective = new SgIfdefDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIfndefDeclaration:           { cppDirective = new SgIfndefDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIfDeclaration:               { cppDirective = new SgIfDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorDeadIfDeclaration:           { cppDirective = new SgDeadIfDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorElseDeclaration:             { cppDirective = new SgElseDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorElifDeclaration:             { cppDirective = new SgElseifDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorEndifDeclaration:            { cppDirective = new SgEndifDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorLineDeclaration:             { cppDirective = new SgLineDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorErrorDeclaration:            { cppDirective = new SgErrorDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorWarningDeclaration:          { cppDirective = new SgWarningDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorEmptyDeclaration:            { cppDirective = new SgEmptyDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorIdentDeclaration:            { cppDirective = new SgIdentDirectiveStatement(); break; }
          case PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker: { cppDirective = new SgLinemarkerDirectiveStatement(); break; }

          default:
             {
               printf ("Error: directive not handled directiveTypeName = %s \n",PreprocessingInfo::directiveTypeName(directive).c_str());
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(cppDirective != NULL);

     cppDirective->set_directiveString(currentPreprocessingInfo->getString());

  // Set the defining declaration to be a self reference...
     cppDirective->set_definingDeclaration(cppDirective);

  // Build source position information...
     cppDirective->set_startOfConstruct(new Sg_File_Info(*(currentPreprocessingInfo->get_file_info())));
     cppDirective->set_endOfConstruct(new Sg_File_Info(*(currentPreprocessingInfo->get_file_info())));

     return cppDirective;
   }

bool
StringUtility::popen_wrapper ( const string & command, vector<string> & result )
   {
  // DQ (2/5/2009): Simple wrapper for Unix popen command.

     const int  SIZE = 10000;
     bool       returnValue = true;
     FILE*      fp = NULL;
     char       buffer[SIZE];

     result = vector<string>();




     // CH (4/6/2010): The Windows version of popen is _popen
#ifdef _MSC_VER
     if ((fp = _popen(command.c_str (), "r")) == NULL)
#else
     if ((fp = popen(command.c_str (), "r")) == NULL)
#endif
        {
          cerr << "Files or processes cannot be created" << endl;
          returnValue = false;
          return returnValue;
        }

     string  current_string;
     while (fgets(buffer, sizeof (buffer), fp))
        {
          current_string = buffer;
          if (current_string [current_string.size () - 1] != '\n')
             {
               cerr << "SIZEBUF too small (" << SIZE << ")" << endl;
               returnValue = false;
               return returnValue;
             }
          ROSE_ASSERT(current_string [current_string.size () - 1] == '\n');
          result.push_back (current_string.substr (0, current_string.size () - 1));
        }

#ifdef _MSC_VER
     if (_pclose(fp) == -1)
#else
     if (pclose(fp) == -1)
#endif
        {
          cerr << ("Cannot execute pclose");
          returnValue = false;
        }
//#endif

     return returnValue;
   }

string
StringUtility::demangledName ( string s )
   {
  // Support for demangling of C++ names. We take care of an empty 
  // string, but an input string with a single space might be an issue.

     vector<string> result;
     if (s.empty() == false)
        {
          if (!popen_wrapper ("c++filt " + s, result))
             {
               cout << "Cannot execute popen_wrapper" << endl;
               return "unknown demangling " + s;
             }
#if 0
       // Debugging...
          for (size_t i = 0; i < result.size (); i++)
             {
               cout << "[" << i << "]\t : " << result [i] << endl;
             }
#endif
        }
       else
        {
          result.push_back("unknown");
        }

     return result[0];
   }
 

#if 1
// DQ (2/16/2009): Moved from expression.code to sageSupport.C while this is tested and debugged!

// DQ (2/8/2009): I always wanted to have this function!
SgFunctionDeclaration*
SgFunctionCallExp::getAssociatedFunctionDeclaration() const
   {
  // This is helpful in chasing down the associated declaration to this function reference.
     SgFunctionDeclaration* returnFunctionDeclaration = NULL;

     SgFunctionSymbol* associatedFunctionSymbol = getAssociatedFunctionSymbol();
     // It can be NULL for a function pointer
     //ROSE_ASSERT(associatedFunctionSymbol != NULL);
     if (associatedFunctionSymbol != NULL)
       returnFunctionDeclaration = associatedFunctionSymbol->get_declaration();

    // ROSE_ASSERT(returnFunctionDeclaration != NULL);

     return returnFunctionDeclaration;
}


SgFunctionSymbol*
SgFunctionCallExp::getAssociatedFunctionSymbol() const
{
	// This is helpful in chasing down the associated declaration to this function reference.
	// But this refactored function does the first step of getting the symbol, so that it
	// can also be used separately in the outlining support.
	SgFunctionSymbol* returnSymbol = NULL;

	// Note that as I recall there are a number of different types of IR nodes that
	// the functionCallExp->get_function() can return (this is the complete list,
	// as tested in astConsistancyTests.C):
	//   - SgDotExp
	//   - SgDotStarOp
	//   - SgArrowExp
	//   - SgArrowStarOp
	//   - SgPointerDerefExp
	//   - SgFunctionRefExp
	//   - SgMemberFunctionRefExp

	//Some virtual functions are resolved statically (e.g. for objects allocated on the stack)
	bool isAlwaysResolvedStatically = false;

	SgExpression* functionExp = this->get_function();
	switch (functionExp->variantT())
	{
		case V_SgFunctionRefExp:
		{
			SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExp);
			ROSE_ASSERT(functionRefExp != NULL);
			returnSymbol = functionRefExp->get_symbol();

			// DQ (2/8/2009): Can we assert this! What about pointers to functions?
			ROSE_ASSERT(returnSymbol != NULL);
			break;
		}

		case V_SgMemberFunctionRefExp:
		{
			SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionExp);
			ROSE_ASSERT(memberFunctionRefExp != NULL);
			returnSymbol = memberFunctionRefExp->get_symbol();

			// DQ (2/8/2009): Can we assert this! What about pointers to functions?
			ROSE_ASSERT(returnSymbol != NULL);
			break;
		}

		case V_SgArrowExp:
		{
			// The lhs is the this pointer (SgThisExp) and the rhs is the member function.
			SgArrowExp* arrayExp = isSgArrowExp(functionExp);
			ROSE_ASSERT(arrayExp != NULL);

			SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(arrayExp->get_rhs_operand());

			// DQ (2/21/2010): Relaxed this constraint because it failes in fixupPrettyFunction test.
			// ROSE_ASSERT(memberFunctionRefExp != NULL);
			if (memberFunctionRefExp != NULL)
			{
				returnSymbol = memberFunctionRefExp->get_symbol();

				// DQ (2/8/2009): Can we assert this! What about pointers to functions?
				ROSE_ASSERT(returnSymbol != NULL);
			}
			break;
		}

		case V_SgDotExp:
		{
			SgDotExp * dotExp = isSgDotExp(functionExp);
			ROSE_ASSERT(dotExp != NULL);
			SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());
			ROSE_ASSERT(memberFunctionRefExp != NULL);
			returnSymbol = memberFunctionRefExp->get_symbol();

			// DQ (2/8/2009): Can we assert this! What about pointers to functions?
			ROSE_ASSERT(returnSymbol != NULL);

			//Virtual functions called through the dot operator are resolved statically if they are not
			//called on reference types.
			isAlwaysResolvedStatically = !isSgReferenceType(dotExp->get_lhs_operand());

			break;
		}

		// Liao, 5/19/2009
		// A pointer to function can be associated to any functions with a matching function type
		// There is no single function declaration which is associated with it.
		// In this case return NULL should be allowed and the caller has to handle it accordingly
		case V_SgPointerDerefExp:
		{
			break;
		}

		//DotStar (Section 5.5 of C++ standard) is used to call a member function pointer and implicitly specify
		//the associated 'this' parameter. In this case, we can't statically determine which function is getting called
		//and should return null.
		case V_SgDotStarOp:
		{
			break;
		}

		//ArrowStar (Section 5.5 of C++ standard) is used to call a member function pointer and implicitly specify
		//the associated 'this' parameter. In this case, we can't statically determine which function is getting called
		//and should return null.
		case V_SgArrowStarOp:
		{
			break;
		}

		default:
		{
			printf("Error: There should be no other cases functionExp = %p = %s \n", functionExp, functionExp->class_name().c_str());
			ROSE_ASSERT(false);
		}
	}

	//If the function is virtual, the function call might actually be to a different symbol.
	//We should return NULL in this case to preserve correctness
	if (returnSymbol != NULL && !isAlwaysResolvedStatically)
	{
		SgFunctionModifier& functionModifier = returnSymbol->get_declaration()->get_functionModifier();
		if (functionModifier.isVirtual() || functionModifier.isPureVirtual())
		{
			returnSymbol = NULL;
		}
	}

	return returnSymbol;
}
#endif
// TODO move the following OpenMP processing code into a separated source file
// maybe in src/midend/openmpSupport ?
// -----------------------------------------------------------------------------

// an internal data structure to avoid redundant AST traversal to find OpenMP pragmas
static std::list<SgPragmaDeclaration* > omp_pragma_list; 

// find all SgPragmaDeclaration nodes within a file and parse OpenMP pragmas into OmpAttribute info.
void attachOmpAttributeInfo(SgSourceFile *sageFilePtr)
{
  ROSE_ASSERT(sageFilePtr != NULL);
  if (sageFilePtr->get_openmp() == false)
    return;
  // For Fortran, search comments for OpenMP directives
  if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
      sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
  {
    parse_fortran_openmp(sageFilePtr);
  } //end if (fortran)
  else
  {
    // For C/C++, search pragma declarations for OpenMP directives 
    std::vector <SgNode*> all_pragmas = NodeQuery::querySubTree (sageFilePtr, V_SgPragmaDeclaration);
    std::vector<SgNode*>::iterator iter;
    for(iter=all_pragmas.begin();iter!=all_pragmas.end();iter++)
    {
      SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(*iter);
      ROSE_ASSERT(pragmaDeclaration != NULL);
      SageInterface::replaceMacroCallsWithExpandedStrings(pragmaDeclaration);
      string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
      istringstream istr(pragmaString);
      std::string key;
      istr >> key;
      if (key == "omp")
      {
	// Liao, 3/12/2009
	// Outliner may move pragma statements to a new file
	// after the pragma has been attached OmpAttribute.
	// We have to skip generating the attribute again in the new file
	OmpAttributeList* previous = getOmpAttributeList(pragmaDeclaration);
	// store them into a buffer, reused by build_OpenMP_AST()
        omp_pragma_list.push_back(pragmaDeclaration);

	if (previous == NULL )
	{
	  // Call parser
	  omp_parser_init(pragmaDeclaration,pragmaString.c_str());
	  omp_parse();
	  OmpAttribute* attribute = getParsedDirective();
	  //cout<<"sage_gen_be.C:23758 debug:\n"<<pragmaString<<endl;
	  //attribute->print();//debug only for now
	  addOmpAttribute(attribute,pragmaDeclaration);
          //cout<<"debug: attachOmpAttributeInfo() for a pragma:"<<pragmaString<<"at address:"<<pragmaDeclaration<<endl;
          //cout<<"file info for it is:"<<pragmaDeclaration->get_file_info()->get_filename()<<endl;
          
#if 1 // Liao, 2/12/2010, this could be a bad idea. It causes trouble in comparing 
        //user-defined and compiler-generated OmpAttribute.
	  // We attach the attribute redundantly on affected loops also
	  // for easier loop handling later on in autoTuning's outlining step (reproducing lost pragmas)
	  if (attribute->getOmpDirectiveType() ==e_for ||attribute->getOmpDirectiveType() ==e_parallel_for)
	  {
	    SgForStatement* forstmt = isSgForStatement(getNextStatement(pragmaDeclaration));
	    ROSE_ASSERT(forstmt != NULL);
	    //forstmt->addNewAttribute("OmpAttribute",attribute);
	    addOmpAttribute(attribute,forstmt);
	  }
#endif
        }
      }
    }// end for
  }
}
// Clause node builders
//----------------------------------------------------------

//! Build SgOmpDefaultClause from OmpAttribute, if any
SgOmpDefaultClause * buildOmpDefaultClause(OmpAttribute* att)
{
   ROSE_ASSERT(att != NULL);
   if (!att->hasClause(e_default))
     return NULL;
   //grab default option  
   omp_construct_enum dv = att->getDefaultValue();
   SgOmpClause::omp_default_option_enum sg_dv;
   switch (dv)
   {
     case e_default_none:
      sg_dv = SgOmpClause::e_omp_default_none;
      break;
     case e_default_shared:
      sg_dv = SgOmpClause::e_omp_default_shared;
      break;
     case e_default_private:
      sg_dv = SgOmpClause::e_omp_default_private;
      break;
     case e_default_firstprivate:
      sg_dv = SgOmpClause::e_omp_default_firstprivate;
      break;
    default:
      {
        cerr<<"error: buildOmpDefaultClase() Unacceptable default option from OmpAttribute:"
          <<OmpSupport::toString(dv)<<endl;
       ROSE_ASSERT(false) ;  
      }
   }//end switch
   SgOmpDefaultClause* result = new SgOmpDefaultClause(sg_dv);
   setOneSourcePositionForTransformation(result);
   ROSE_ASSERT(result);
   return result;
}

//Build if clause
SgOmpExpressionClause* buildOmpExpressionClause(OmpAttribute* att, omp_construct_enum clause_type)
{
  ROSE_ASSERT(att != NULL);
  if (!att->hasClause(clause_type)) 
    return NULL;
  SgOmpExpressionClause * result = NULL ;   
  switch (clause_type)
  {
    case e_collapse:
      {
        ROSE_ASSERT(att->getExpression(e_collapse).second!=NULL);

        result = new SgOmpCollapseClause(att->getExpression(e_collapse).second);
        break;
      }
    case e_if:
      {
        ROSE_ASSERT(att->getExpression(e_if).second!=NULL);
        result = new SgOmpIfClause(att->getExpression(e_if).second);
        break;
      }
    case e_num_threads:
      {
        ROSE_ASSERT(att->getExpression(e_num_threads).second!=NULL);
        result = new SgOmpNumThreadsClause(att->getExpression(e_num_threads).second);
        break;
      }
    default:
      {
        printf("error in buildOmpExpressionClause(): unacceptable clause type:%s\n",
            OmpSupport::toString(clause_type).c_str());
        ROSE_ASSERT(false);
      }
  }

  ROSE_ASSERT(result != NULL);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgOmpNowaitClause * buildOmpNowaitClause(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  if (!att->hasClause(e_nowait))
    return NULL;
  SgOmpNowaitClause* result = new SgOmpNowaitClause();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}
SgOmpOrderedClause * buildOmpOrderedClause(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  if (!att->hasClause(e_ordered_clause))
    return NULL;
  SgOmpOrderedClause* result = new SgOmpOrderedClause();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgOmpUntiedClause * buildOmpUntiedClause(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  if (!att->hasClause(e_untied))
    return NULL;
  SgOmpUntiedClause* result = new SgOmpUntiedClause();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//Build SgOmpScheduleClause from OmpAttribute, if any
SgOmpScheduleClause* buildOmpScheduleClause(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  if (!att->hasClause(e_schedule))
    return NULL;
  // convert OmpAttribute schedule kind to SgOmpClause schedule kind
  omp_construct_enum oa_kind = att->getScheduleKind();
  SgOmpClause::omp_schedule_kind_enum sg_kind;
  switch (oa_kind)
  {
    case   e_schedule_static:
      sg_kind = SgOmpClause::e_omp_schedule_static;
      break;
    case   e_schedule_dynamic:
      sg_kind = SgOmpClause::e_omp_schedule_dynamic;
      break;
    case   e_schedule_guided:
      sg_kind = SgOmpClause::e_omp_schedule_guided;
      break;
    case   e_schedule_auto:
      sg_kind = SgOmpClause::e_omp_schedule_auto;
      break;
    case   e_schedule_runtime:
      sg_kind = SgOmpClause::e_omp_schedule_runtime;
      break;
    default:
      {
        cerr<<"error: buildOmpScheduleClause() Unacceptable schedule kind from OmpAttribute:"
          <<OmpSupport::toString(oa_kind)<<endl;
        ROSE_ASSERT(false) ;  
      }
  }
  SgExpression* chunksize_exp = att->getExpression(e_schedule).second;
 // ROSE_ASSERT(chunksize_exp != NULL); // chunk size is optional
  // finally build the node
  SgOmpScheduleClause* result = new SgOmpScheduleClause(sg_kind, chunksize_exp);
//  setOneSourcePositionForTransformation(result);
  ROSE_ASSERT(result != NULL);
  return  result;
}

//! A helper function to convert OmpAttribute reduction operator to SgClause reduction operator
//TODO move to sageInterface?
static   SgOmpClause::omp_reduction_operator_enum toSgOmpClauseReductionOperator(omp_construct_enum at_op)
{
  SgOmpClause::omp_reduction_operator_enum result = SgOmpClause::e_omp_reduction_unkown;
  switch (at_op)
  {
    case e_reduction_plus: //+
      {
        result = SgOmpClause::e_omp_reduction_plus;
        break;
      }
    case e_reduction_mul:  //*
       {
        result = SgOmpClause::e_omp_reduction_mul;
        break;
      }
   case e_reduction_minus: // -
       {
        result = SgOmpClause::e_omp_reduction_minus;
        break;
      }
     // C/C++ only
    case e_reduction_bitand: // &
       {
        result = SgOmpClause::e_omp_reduction_bitand;
        break;
      }
   case e_reduction_bitor:  // |
       {
        result = SgOmpClause::e_omp_reduction_bitor;
        break;
      }
   case e_reduction_bitxor:  // ^
       {
        result = SgOmpClause::e_omp_reduction_bitxor;
        break;
      }
   case e_reduction_logand:  // &&
       {
        result = SgOmpClause::e_omp_reduction_logand;
        break;
      }
   case e_reduction_logor:   // ||
      {
        result = SgOmpClause::e_omp_reduction_logor;
        break;
      }

      // fortran operator
    case e_reduction_and: // .and.
       {
        result = SgOmpClause::e_omp_reduction_and;
        break;
      }
   case e_reduction_or: // .or.
       {
        result = SgOmpClause::e_omp_reduction_or;
        break;
      }
   case e_reduction_eqv:   // fortran .eqv.
       {
        result = SgOmpClause::e_omp_reduction_eqv;
        break;
      }
   case e_reduction_neqv:   // fortran .neqv.
      // reduction intrinsic procedure name for Fortran
       {
        result = SgOmpClause::e_omp_reduction_neqv;
        break;
      }
   case e_reduction_max:
       {
        result = SgOmpClause::e_omp_reduction_max;
        break;
      }
   case e_reduction_min:
       {
        result = SgOmpClause::e_omp_reduction_min;
        break;
      }
   case e_reduction_iand:
       {
        result = SgOmpClause::e_omp_reduction_iand;
        break;
      }
   case e_reduction_ior:
       {
        result = SgOmpClause::e_omp_reduction_ior;
        break;
      }
   case e_reduction_ieor:
      {
        result = SgOmpClause::e_omp_reduction_ieor;
        break;
      }
    default:
      {
        printf("error: unacceptable omp construct enum for reduction operator conversion:%s\n", OmpSupport::toString(at_op).c_str());
        ROSE_ASSERT(false);
        break;
      }
  }
  ROSE_ASSERT(result != SgOmpClause::e_omp_reduction_unkown);
  return result;
}
//A helper function to set SgInitializedNamePtrList  from OmpAttribute's construct-varlist map
static void setClauseVariableList(SgOmpVariablesClause* target, OmpAttribute* att, omp_construct_enum key)
{
  ROSE_ASSERT(target&&att);
  // build variable list
  std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(key);
#if 0  
  // Liao 6/10/2010 we relax this assertion to workaround 
  //  shared(num_threads),  a clause keyword is used as a variable 
  //  we skip variable list of shared() for now so shared clause will have empty variable list
#endif  
   ROSE_ASSERT(varlist.size()!=0);
  std::vector<std::pair<std::string,SgNode* > >::iterator iter;
  for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
  {
    SgInitializedName* iname = isSgInitializedName((*iter).second);
    ROSE_ASSERT(iname !=NULL);
    target->get_variables().push_back(iname);
  }
}

//! Try to build a reduction clause with a given operation type from OmpAttribute
SgOmpReductionClause* buildOmpReductionClause(OmpAttribute* att, omp_construct_enum reduction_op)
{
  ROSE_ASSERT(att !=NULL);
  if (!att->hasReductionOperator(reduction_op))
    return NULL;
  SgOmpClause::omp_reduction_operator_enum  sg_op = toSgOmpClauseReductionOperator(reduction_op); 
  SgOmpReductionClause* result = new SgOmpReductionClause(sg_op);
  setOneSourcePositionForTransformation(result);
  ROSE_ASSERT(result != NULL);

  // build variable list
  setClauseVariableList(result, att, reduction_op); 
  return result;
}

//Build one of the clauses with a variable list
SgOmpVariablesClause * buildOmpVariableClause(OmpAttribute* att, omp_construct_enum clause_type)
{
  ROSE_ASSERT(att != NULL);
  if (!att->hasClause(clause_type))
    return NULL;
  SgOmpVariablesClause* result = NULL;  
  switch (clause_type) 
  {
    case e_copyin:
      {
        result = new SgOmpCopyinClause();
        break;
      }
    case e_copyprivate:
      {
        result = new SgOmpCopyprivateClause();
        break;
      }
    case e_firstprivate:
      {
        result = new SgOmpFirstprivateClause();
        break;
      }
    case e_lastprivate:
      {
        result = new SgOmpLastprivateClause();
        break;
      }
    case e_private:
      {
        result = new SgOmpPrivateClause();
        break;
      }
    case e_shared:
      {
        result = new SgOmpSharedClause();
        break;
      }
    case e_reduction:
      {
        printf("error: buildOmpVariableClause() does not handle reduction\n");
        ROSE_ASSERT(false);
      }
    default:
      {
        cerr<<"error: buildOmpVariableClause() Unacceptable clause type:"
          <<OmpSupport::toString(clause_type)<<endl;
        ROSE_ASSERT(false) ;  
      }
  } //end switch

  //build varlist
  ROSE_ASSERT(result != NULL);
  setClauseVariableList(result, att, clause_type);
  return result;
}

// Build a single SgOmpClause from OmpAttribute for type c_clause_type, excluding reduction clauses
SgOmpClause* buildOmpNonReductionClause(OmpAttribute* att, omp_construct_enum c_clause_type)
{
  SgOmpClause* result = NULL;
  ROSE_ASSERT(att != NULL);
  ROSE_ASSERT(isClause(c_clause_type));
  if (!att->hasClause(c_clause_type))
    return NULL;
  switch (c_clause_type) 
  {
    case e_default:
      {
        result = buildOmpDefaultClause(att); 
        break;
      }
    case e_nowait:
      {
        result = buildOmpNowaitClause(att); 
        break;
      }
    case e_ordered_clause:
      {
        result = buildOmpOrderedClause(att); 
        break;
      }
   case e_schedule:
      {
        result = buildOmpScheduleClause(att);
        break;
      }
   case e_untied:
      {
        result = buildOmpUntiedClause(att); 
        break;
      }
    case e_if:
    case e_collapse:
    case e_num_threads:
      {
        result = buildOmpExpressionClause(att, c_clause_type);
        break;
      }
    case e_copyin:  
    case e_copyprivate:  
    case e_firstprivate:  
    case e_lastprivate:
    case e_private:
    case e_shared:
      {
        result = buildOmpVariableClause(att, c_clause_type);
        break;
      }
    case e_reduction:
      {
        printf("error: buildOmpNonReductionClause() does not handle reduction. Please use buildOmpReductionClause().\n");
        ROSE_ASSERT(false);
        break;
      }
    default:
      {
        printf("Warning: buildOmpNoReductionClause(): unhandled clause type: %s\n", OmpSupport::toString(c_clause_type).c_str());
        ROSE_ASSERT(false);
        break;
      }

  }
  ROSE_ASSERT(result != NULL);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Get the affected structured block from an OmpAttribute
SgStatement* getOpenMPBlockFromOmpAttribte (OmpAttribute* att)
{
  SgStatement* result = NULL;
  ROSE_ASSERT(att != NULL);
  omp_construct_enum c_clause_type = att->getOmpDirectiveType();

  // Some directives have no followed statements/blocks 
  if (!isDirectiveWithBody(c_clause_type))
    return NULL;

  SgNode* snode = att-> getNode ();
  ROSE_ASSERT(snode != NULL); //? not sure for Fortran
  SgFile * file = getEnclosingFileNode (snode);
  if (file->get_Fortran_only()||file->get_F77_only()||file->get_F90_only()||
      file->get_F95_only() || file->get_F2003_only())
  { //Fortran check setNode()
    printf("buildOmpParallelStatement() Fortran is not handled yet\n");
  }
  else // C/C++ must be pragma declaration statement
  {
    SgPragmaDeclaration* pragmadecl = att->getPragmaDeclaration();
    result = getNextStatement(pragmadecl);
  }

//  ROSE_ASSERT(result!=NULL);
// Not all pragma decl has a structured body!!
  return result;
}

//add clauses to target based on OmpAttribute
static void appendOmpClauses(SgOmpClauseBodyStatement* target, OmpAttribute* att)
{
  ROSE_ASSERT(target && att);
  // for Omp statements with clauses
  // must copy those clauses here, since they will be deallocated later on
  vector<omp_construct_enum> clause_vector = att->getClauses();
  std::vector<omp_construct_enum>::iterator citer;
  for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++)
  {
    omp_construct_enum c_clause = *citer;
    if (!isClause(c_clause))
    {
      //      printf ("Found a construct which is not a clause:%s\n within attr:%p\n", OmpSupport::toString(c_clause).c_str(), att);
      ROSE_ASSERT(isClause(c_clause));
      continue;
    }
    else
    {
      // printf ("Found a clause construct:%s\n", OmpSupport::toString(c_clause).c_str());
    }
    // special handling for reduction
    if (c_clause == e_reduction) 
    {
      std::vector<omp_construct_enum> rops  = att->getReductionOperators();
      ROSE_ASSERT(rops.size()!=0);
      std::vector<omp_construct_enum>::iterator iter;
      for (iter=rops.begin(); iter!=rops.end();iter++)
      {
        omp_construct_enum rop = *iter;
        SgOmpClause* sgclause = buildOmpReductionClause(att, rop);
        target->get_clauses().push_back(sgclause);
      }
    }
    else 
    {
      SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
      target->get_clauses().push_back(sgclause);
      sgclause->set_parent(target); // is This right?
    }
  }
}

// Directive statement builders
//----------------------------------------------------------
//! Build a SgOmpBodyStatement
// handle body and optional clauses for it
SgOmpBodyStatement * buildOmpBodyStatement(OmpAttribute* att)
{
  SgStatement* body = getOpenMPBlockFromOmpAttribte(att);
  //Must remove the body from its previous parent
  removeStatement(body);

  if (body==NULL)
  {
    cerr<<"error: buildOmpBodyStatement() found empty body for "<<att->toOpenMPString()<<endl;
    ROSE_ASSERT(body != NULL);
  }
  SgOmpBodyStatement* result = NULL;
  switch (att->getOmpDirectiveType())
  {
    case e_atomic:
      result = new SgOmpAtomicStatement(NULL, body); 
      break;
    case e_critical:
      result = new SgOmpCriticalStatement(NULL, body, SgName(att->getCriticalName())); 
      break;
    case e_master:
      result = new SgOmpMasterStatement(NULL, body); 
      break;
    case e_ordered_directive:
      result = new SgOmpOrderedStatement(NULL, body); 
      break;
    case e_section:
      result = new SgOmpSectionStatement(NULL, body); 
      break;
    case e_parallel:
      result = new SgOmpParallelStatement(NULL, body); 
      break;
    case e_for:  
      result = new SgOmpForStatement(NULL, body); 
      break;
    case e_single:
      result = new SgOmpSingleStatement(NULL, body); 
      break;
   case e_sections:
      result = new SgOmpSectionsStatement(NULL, body); 
      break;
   case e_task:
      result = new SgOmpTaskStatement(NULL, body); 
     // cout<<"Debug:sageSupport.C Found an OmpAttribute from a task pragma"<<endl;
      break;
    default:
      {
        cerr<<"error: unacceptable omp construct for buildOmpBodyStatement():"<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
        ROSE_ASSERT(false);
      }
  }
  ROSE_ASSERT(result != NULL);
  setOneSourcePositionForTransformation(result);
  //set the current parent
  body->set_parent(result);
  // add clauses for those SgOmpClauseBodyStatement
  if (isSgOmpClauseBodyStatement(result))
    appendOmpClauses(isSgOmpClauseBodyStatement(result), att);
  return result;
}

SgOmpFlushStatement* buildOmpFlushStatement(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  SgOmpFlushStatement* result = new SgOmpFlushStatement();
  ROSE_ASSERT(result !=NULL);
  setOneSourcePositionForTransformation(result);
  // build variable list
  std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(e_flush);
 // ROSE_ASSERT(varlist.size()!=0); // can have empty variable list
  std::vector<std::pair<std::string,SgNode* > >::iterator iter;
  for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
  {
    SgInitializedName* iname = isSgInitializedName((*iter).second);
    ROSE_ASSERT(iname !=NULL);
    result->get_variables().push_back(iname);
  }
  return result;
}

SgOmpThreadprivateStatement* buildOmpThreadprivateStatement(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  SgOmpThreadprivateStatement* result = new SgOmpThreadprivateStatement();
  ROSE_ASSERT(result !=NULL);
  setOneSourcePositionForTransformation(result);
  // build variable list
  std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(e_threadprivate);
  ROSE_ASSERT(varlist.size()!=0);
  std::vector<std::pair<std::string,SgNode* > >::iterator iter;
  for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
  {
    SgInitializedName* iname = isSgInitializedName((*iter).second);
    ROSE_ASSERT(iname !=NULL);
    result->get_variables().push_back(iname);
  }
  result->set_definingDeclaration(result);
  return result;
}
//! Build nodes for combined OpenMP directives:
//    parallel for
//    parallel sections
//    parallel workshare //TODO fortran later on
// We don't provide dedicated Sage node for combined directives, 
// so we separate them in the AST as 1st and 2nd directive statement
// the first is always parallel and we return it from the function
SgOmpParallelStatement* buildOmpParallelStatementFromCombinedDirectives(OmpAttribute* att)
{
  ROSE_ASSERT(att != NULL);
  SgStatement* body = getOpenMPBlockFromOmpAttribte(att);
  //Must remove the body from its previous parent
   removeStatement(body);
  ROSE_ASSERT(body != NULL);

  // build the 2nd directive node first
  SgStatement * second_stmt = NULL; 
  switch (att->getOmpDirectiveType())  
  {
    case e_parallel_for:
      {
        second_stmt = new SgOmpForStatement(NULL, body);
        setOneSourcePositionForTransformation(second_stmt);
        break;
      }
    case e_parallel_sections:
      {
        second_stmt = new SgOmpSectionsStatement(NULL, body);
        setOneSourcePositionForTransformation(second_stmt); 
        break;
      }
      //   case e_parallel_workshare: //TODO fortran
     default:
      {
        cerr<<"error: unacceptable directive type in buildOmpParallelStatementFromCombinedDirectives(): "<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
        ROSE_ASSERT(false);
      }
  } //end switch

  ROSE_ASSERT(second_stmt);
  body->set_parent(second_stmt);

  // build the 1st directive node then
  SgOmpParallelStatement* first_stmt = new SgOmpParallelStatement(NULL, second_stmt); 
  setOneSourcePositionForTransformation(first_stmt);
  second_stmt->set_parent(first_stmt);

  // allocate clauses to them, let the 2nd one have higher priority 
  // if a clause can be allocated to either of them
  vector<omp_construct_enum> clause_vector = att->getClauses();
  std::vector<omp_construct_enum>::iterator citer;
  for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++)
  {
    omp_construct_enum c_clause = *citer;
    if (!isClause(c_clause))
    {
      printf ("Found a construct which is not a clause:%s\n within attr:%p\n", OmpSupport::toString(c_clause).c_str(), att);
      ROSE_ASSERT(isClause(c_clause));
      continue;
    }
    else
    {
      // printf ("Found a clause construct:%s\n", OmpSupport::toString(c_clause).c_str());
    }

    switch (c_clause)
    {
      // clauses allocated to omp parallel
      case e_if:
      case e_num_threads:
      case e_default:
      case e_shared:
      case e_copyin:
        {
          SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
          ROSE_ASSERT(sgclause != NULL);
          first_stmt->get_clauses().push_back(sgclause);
          break;
        }
      // unique clauses allocated to omp for
     case e_schedule:
      case e_collapse:
      case e_ordered_clause:
        {
          if (!isSgOmpForStatement(second_stmt))
          {
            printf("error: unacceptable clauses in parallel for\n");
            att->print();
            ROSE_ASSERT(false);
          }
        }
     case e_private:
      case e_firstprivate:
      case e_lastprivate:
        // case e_nowait: // nowait should not appear with combined directives
        {
          SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
          ROSE_ASSERT(sgclause != NULL);
          // TODO parallel workshare 
          isSgOmpClauseBodyStatement(second_stmt)->get_clauses().push_back(sgclause);
          break;
        }
      case e_reduction: //special handling for reduction
        {
          std::vector<omp_construct_enum> rops  = att->getReductionOperators();
          ROSE_ASSERT(rops.size()!=0);
          std::vector<omp_construct_enum>::iterator iter;
          for (iter=rops.begin(); iter!=rops.end();iter++)
          {
            omp_construct_enum rop = *iter;
            SgOmpClause* sgclause = buildOmpReductionClause(att, rop);
            ROSE_ASSERT(sgclause != NULL);
            isSgOmpClauseBodyStatement(second_stmt)->get_clauses().push_back(sgclause);
          }
         break;
        }
      default:
      {
        cerr<<"error: unacceptable clause for combined parallel for directive:"<<OmpSupport::toString(c_clause)<<endl;
        ROSE_ASSERT(false);
      }
    }
  } // end clause allocations 

 /*
  handle dangling #endif  attached to the loop
  1. original 
 #ifdef _OPENMP
  #pragma omp parallel for  private(i,k)
 #endif 
   for () ...

  2. after splitting

 #ifdef _OPENMP
  #pragma omp parallel 
  #pragma omp for  private(i,k)
 #endif 
   for () ...
  
  3. We need to move #endif to omp parallel statement 's after position
   transOmpParallel () will take care of it later on

    #ifdef _OPENMP
      #pragma omp parallel 
      #pragma omp for  private(i) reduction(+ : j)
      for (i = 1; i < 1000; i++)
        if ((key_array[i - 1]) > (key_array[i]))
          j++;
    #endif
  This is no perfect solution until we handle preprocessing information as structured statements in AST
 */
   movePreprocessingInfo(body, first_stmt, PreprocessingInfo::before, PreprocessingInfo::after, true);
  return first_stmt;
}

//! for C/C++ replace OpenMP pragma declaration with an SgOmpxxStatement
void replaceOmpPragmaWithOmpStatement(SgPragmaDeclaration* pdecl, SgStatement* ompstmt)
{
  ROSE_ASSERT(pdecl != NULL);
  ROSE_ASSERT(ompstmt!= NULL);

  SgScopeStatement* scope = pdecl ->get_scope();
  ROSE_ASSERT(scope !=NULL);
#if 0  
  SgOmpBodyStatement * omp_cb_stmt = isSgOmpBodyStatement(ompstmt);
 // do it within buildOmpBodyStatement()
 // avoid two parents point to the same structured block
  // optionally remove the immediate structured block
  if (omp_cb_stmt!= NULL)
  {
    SgStatement* next_stmt = getNextStatement(pdecl);
    // not true after splitting combined directives, the body becomes the 2nd directive
   // ROSE_ASSERT(next_stmt == omp_cb_stmt->get_body()); // ompstmt's body is set already
    removeStatement(next_stmt);
  }
#endif  
  // replace the pragma
  moveUpPreprocessingInfo(ompstmt, pdecl); // keep #ifdef etc attached to the pragma
  replaceStatement(pdecl, ompstmt);
}

//! Convert omp_pragma_list to SgOmpxxx nodes
void convert_OpenMP_pragma_to_AST (SgSourceFile *sageFilePtr)
{
  list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
  ROSE_ASSERT (sageFilePtr != NULL);
  for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter ++)
  {
    // Liao, 11/18/2009
    // It is possible that several source files showing up in a single compilation line
    // We have to check if the pragma declaration's file information matches the current file being processed
    // Otherwise we will process the same pragma declaration multiple times!!
    SgPragmaDeclaration* decl = *iter; 
    // Liao, 2/8/2010
    // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
    // e.g if ()
    //      #pragma
    //        do_sth()
    //  will be changed to
    //     if ()
    //     {
    //       #pragma
    //        do_sth()
    //     }
    // So we process a pragma if it is either within the same file or marked as transformation
    if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
        && !(decl->get_file_info()->isTransformation()))
      continue;
    //cout<<"debug: convert_OpenMP_pragma_to_AST() handling pragma at "<<decl<<endl;  
    OmpAttributeList* oattlist= getOmpAttributeList(decl);
    ROSE_ASSERT (oattlist != NULL) ;
    vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
    ROSE_ASSERT (ompattlist.size() != 0) ;
    vector <OmpAttribute* >::iterator i = ompattlist.begin();
    for (; i!=ompattlist.end(); i++)
    {
      OmpAttribute* oa = *i;
      omp_construct_enum omp_type = oa->getOmpDirectiveType();
      ROSE_ASSERT(isDirective(omp_type));
      SgStatement* omp_stmt = NULL;
      switch (omp_type)
      {
        // simplest OMP directives
        case e_barrier:
          {
            omp_stmt = new SgOmpBarrierStatement();
            break;
          }
        case e_taskwait:
          {
            omp_stmt = new SgOmpTaskwaitStatement();
            break;
          }
          // with variable list
        case e_threadprivate:
          {
            omp_stmt = buildOmpThreadprivateStatement(oa);
            break;
          }
        case e_flush:
        {
          omp_stmt = buildOmpFlushStatement(oa);
          break;
        }
        // with a structured block/statement followed
        case e_atomic:
        case e_master:
        case e_section:
        case e_critical:
        case e_ordered_directive:
          // case e_workshare;// TODO fortran
        case e_parallel:
        case e_for:
        case e_single:
        case e_task:
        case e_sections: 
          {
            omp_stmt = buildOmpBodyStatement(oa);
            break;
          }
        case e_parallel_for:
        case e_parallel_sections:
          //case e_parallel_workshare://TODO fortran
          {
            omp_stmt = buildOmpParallelStatementFromCombinedDirectives(oa);
            break;
          }
        default:
          {
            cerr<<"Error: convert_OpenMP_pragma_to_AST(): unhandled OpenMP directive type:"<<OmpSupport::toString(omp_type)<<endl;
            assert (false);
            break;
          }
      }
      setOneSourcePositionForTransformation(omp_stmt);
      replaceOmpPragmaWithOmpStatement(decl, omp_stmt);

    } // end for (OmpAttribute)
  }// end for (omp_pragma_list)
}

void build_OpenMP_AST(SgSourceFile *sageFilePtr)
   {
  // build AST for OpenMP directives and clauses 
  // by converting OmpAttributeList to SgOmpxxx Nodes 
     if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
         sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
        {
          printf("AST construction for Fortran OpenMP is not yet implemented. \n");
          assert(false);
       // end if (fortran)
        }
       else
        {
       // for  C/C++ pragma's OmpAttributeList --> SgOmpxxx nodes
          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling convert_OpenMP_pragma_to_AST() \n");
             }
     
          convert_OpenMP_pragma_to_AST( sageFilePtr);
        }
   }

// Liao, 5/31/2009 an entry point for OpenMP related processing
// including parsing, AST construction, and later on tranlation
void processOpenMP(SgSourceFile *sageFilePtr)
   {
  // DQ (4/4/2010): This function processes both C/C++ and Fortran code.
  // As a result of the Fortran processing some OMP pragmas will cause
  // transformation (e.g. declaration of private variables will add variables
  // to the local scope).  So this function has side-effects for all languages.

     if (SgProject::get_verbose() > 1)
        {
          printf ("Processing OpenMP directives \n");
        }

     ROSE_ASSERT(sageFilePtr != NULL);
     if (sageFilePtr->get_openmp() == false)
        {
          if (SgProject::get_verbose() > 1)
             {
               printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp() = %s \n",sageFilePtr->get_openmp() ? "true" : "false");
             }
          return;
        }
     
  // parse OpenMP directives and attach OmpAttributeList to relevant SgNode
     attachOmpAttributeInfo(sageFilePtr);

  // stop here if only OpenMP parsing is requested
     if (sageFilePtr->get_openmp_parse_only())
        {
          if (SgProject::get_verbose() > 1)
             {
               printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp_parse_only() = %s \n",sageFilePtr->get_openmp_parse_only() ? "true" : "false");
             }
          return;
        }

  // Build OpenMP AST nodes based on parsing results
     build_OpenMP_AST(sageFilePtr);

  // stop here if only OpenMP AST construction is requested
     if (sageFilePtr->get_openmp_ast_only())
        {
          if (SgProject::get_verbose() > 1)
             {
               printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp_ast_only() = %s \n",sageFilePtr->get_openmp_ast_only() ? "true" : "false");
             }
          return;
        }

     lower_omp(sageFilePtr);
   }


