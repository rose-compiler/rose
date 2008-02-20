#include "rose.h"
#include <sys/stat.h>
#include <sys/wait.h>

#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
#include "objdumpToRoseBinaryAst.h"
#include "RoseBin_unparse.h"
#endif

using namespace std;

// This needs to be moved somewhere else
int systemFromVector(const vector<string>& argv) {
  ROSE_ASSERT (!argv.empty());
  pid_t pid = fork();
  if (pid == -1) {perror("fork: "); abort();}
  if (pid == 0) { // Child
    vector<const char*> argvC(argv.size() + 1);
    for (size_t i = 0; i < argv.size(); ++i) {
      argvC[i] = strdup(argv[i].c_str());
    }
    argvC.back() = NULL;
    int err = execvp(argv[0].c_str(), (char* const*)&argvC[0]);
    exit(1); // Should not get here normally
  } else { // Parent
    int status;
    pid_t err = waitpid(pid, &status, 0);
    if (err == -1) {perror("waitpid: "); abort();}
    return status;
  }
}

// These need to be moved somewhere else
// EOF is not handled correctly here -- EOF is normally set when the child
// process exits
FILE* popenReadFromVector(const vector<string>& argv) {
  ROSE_ASSERT (!argv.empty());
  int pipeDescriptors[2];
  int pipeErr = pipe(pipeDescriptors);
  if (pipeErr == -1) {perror("pipe: "); abort();}
  pid_t pid = fork();
  if (pid == -1) {perror("fork: "); abort();}
  if (pid == 0) { // Child
    vector<const char*> argvC(argv.size() + 1);
    for (size_t i = 0; i < argv.size(); ++i) {
      argvC[i] = strdup(argv[i].c_str());
    }
    argvC.back() = NULL;
    int dup2Err = dup2(pipeDescriptors[1], 1); // stdout
    if (dup2Err == -1) {perror("dup2: "); abort();}
    int err = execvp(argv[0].c_str(), (char* const*)&argvC[0]);
    exit(1); // Should not get here normally
  } else { // Parent
    return fdopen(pipeDescriptors[0], "r");
  }
}

int pcloseFromVector(FILE* f) { // Assumes there is only one child process
  int status;
  pid_t err = wait(&status);
  fclose(f);
  return status;
}

// DQ (1/5/2008): These are functions separated out of the generated
// code in ROSETTA.  These functions don't need to be generated since
// there implementation is not as dependent upon the IR as other functions
// (e.g. IR node member functions).

#if 1
bool
CommandlineProcessing::isOptionWithParameter ( vector<string> & argv, string optionPrefix, string option, string & optionParameter, bool removeOption )
   {
  // I could not make this work cleanly with valgrind withouth allocatting memory twice
     string localString;

  // printf ("Calling sla for string! removeOption = %s \n",removeOption ? "true" : "false");
     int optionCount = sla(argv, optionPrefix, "($)^", option, &localString, removeOption ? 1 : -1);
  // printf ("DONE: Calling sla for string! optionCount = %d localString = %s \n",optionCount,localString.c_str());

  // optionCount = sla(argv, optionPrefix, "($)^", option, &localString, removeOption ? 1 : -1);
  // printf ("DONE: Calling sla for string! optionCount = %d localString = %s \n",optionCount,localString.c_str());

     if (optionCount > 0)
          optionParameter = localString;

     return (optionCount > 0);
   }
#endif

static bool sameFile(const string& a, const string& b) {
  // Uses stat() to test device and inode numbers
  // Returning false on error is so that a non-existant file doesn't match
  // anything
  // printf("sameFile('%s', '%s')\n", a.c_str(), b.c_str());
  struct stat aStat, bStat;
  int statResultA = stat(a.c_str(), &aStat);
  if (statResultA != 0) return false;
  int statResultB = stat(b.c_str(), &bStat);
  if (statResultB != 0) return false;
  return aStat.st_dev == bStat.st_dev && aStat.st_ino == bStat.st_ino;
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
  //         both comilation and linking).  (e.g. -l<libname>, -L <directory>, <libname>.a,
  //         <filename>.C, <filename>.c, -I<directory name>, <filename>.h
  // NOTE: there is no side-effect to argc and argv.  Thus the original ROSE translator can 
  // see all options.  Any ROSE or EDG specific options can be striped by calling the
  // appropriate functions to strip them out.

  // This function now makes an internal copy of the command line parameters to
  // allow the originals to remain unmodified (SLA modifies the command line).

  // printf ("Inside of SgProject::processCommandLine() \n");

  // local copies of argc and argv variables
  // The purpose of building local copies is to avoid
  // the modification of the command line by SLA (to save the original command line)
     vector<string> local_commandLineArgumentList = input_argv;

  // Save a deep copy fo the original command line input the the translator
  // pass in out copies of the argc and argv to make clear that we don't modify argc and argv
     set_originalCommandLineArgumentList( local_commandLineArgumentList );

  // printf ("DONE with copy of command line in SgProject constructor! \n");

  // printf ("SgProject::processCommandLine(): local_commandLineArgumentList.size() = %zu \n",local_commandLineArgumentList.size());
  // printf ("SgProject::processCommandLine(): local_commandLineArgumentList = %s \n",StringUtility::listToString(local_commandLineArgumentList).c_str());

  // Build the empty STL lists
     p_fileList = new SgFilePtrList();
     ROSE_ASSERT (p_fileList != NULL);

  // JJW (1/30/2008): added detection of path of current executable
     {
       ROSE_ASSERT (!local_commandLineArgumentList.empty());
       string argv0 = local_commandLineArgumentList.front();
    // printf("Have argv[0] = '%s'\n", argv0.c_str());
       string fullExecutableFile;
       if (argv0.find('/') == string::npos) { // Need to search $PATH
         const char* pathRaw = getenv("PATH");
         string path = pathRaw ? pathRaw : "/usr/bin:/bin";
      // printf("Searching path '%s'\n", path.c_str());
         for (string::size_type oldi = (string::size_type)(-1),
                                i = path.find(':');
              i != string::npos; oldi = i, i = path.find(':', i + 1)) {
           string pathDir = path.substr(oldi + 1, i - 1 - oldi);
        // printf("Get path entry '%s'\n", pathDir.c_str());
           bool fileExists = false;
           string fullNameToTest = pathDir + "/" + argv0;
           FILE* f = fopen(fullNameToTest.c_str(), "r");
           if (f) {fileExists = true; fclose(f);}
           if (fileExists) {
             fullExecutableFile = fullNameToTest;
             break;
           }
         }
       } else {
         fullExecutableFile = argv0;
      // printf("argv[0] has /, so this is a full path\n");
       }
    // printf("Found executable file name '%s'\n", fullExecutableFile.c_str());
       ROSE_ASSERT(!fullExecutableFile.empty());
       string executablePath = ROSE::getPathFromFileName(fullExecutableFile);
       ROSE_ASSERT(!executablePath.empty());
    // printf("Found executable path '%s'\n", executablePath.c_str());
       bool inInstallDir = sameFile(executablePath, ROSE_AUTOMAKE_BINDIR);
    // printf(inInstallDir ? "Running from installed ROSE\n" : "Running from build tree\n");
       this->set_runningFromInstalledRose(inInstallDir);
     }

  // return value for calls to SLA
     int optionCount = 0;

  //
  // help option (allows alternative -h or -help instead of just -rose:help)
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "-", "($)", "(h|help)",1);
     optionCount = sla_none(local_commandLineArgumentList, "-", "($)", "(h|help)",1);
     if( optionCount > 0 )
        {
       // printf ("option -help found \n");
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
       // ROSE::usage(0);
          SgFile::usage(0);
          exit(0);
        }

  // printf ("After SgProject processing -help option argc = %d \n",argc);

  //
  // help option (allows alternative --h or --help)
  //
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "--", "($)", "(h|help)",1);
     optionCount = sla_none(local_commandLineArgumentList, "--", "($)", "(h|help)",1);
     if( optionCount > 0 )
        {
       // printf ("option --help found \n");
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
       // ROSE::usage(0);
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
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          exit(0);
        }

  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
  // optionCount = sla(&argc, argv, "--", "($)", "(V|version)",1);
     optionCount = sla_none(local_commandLineArgumentList, "-rose:", "($)", "(V|version)",1);
     if ( optionCount > 0 )
        {
       // printf ("SgProject::processCommandLine(): option --version found \n");
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          exit(0);
        }

#if 0
  // DQ (8/6/2006): Not sure that I want this here!
  //
  // version option (using -rose:version)
  //
     if ( CommandlineProcessing::isOption(argc,argv,"-rose:","(V|version)",true) == true )
        {
       // function in SAGE III to access version number of EDG
          extern std::string edgVersionString();
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          printf ("     Using C++ and C frontend from EDG (version %s) internally \n",edgVersionString().c_str());
        }
#endif
  // DQ (10/15/2005): Added because verbose was not set when the number of files (source files) was zero (case for linking only)
  //
  // specify verbose setting for projects (should be set even for linking where there are no source files
  //
     ROSE_ASSERT (get_verbose() == 0);
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

  // DQ (10/16/2005):
  // Build versions of argc and argv that are separate from the input_argc and input_argv 
  // (so that we can be clear that there are no side-effects to the original argc and argv 
  // that come from the user's ROSE translator.  Mostly we want to use the short names 
  // (e.g. "argc" and "argv").
     vector<string> argv = get_originalCommandLineArgumentList();
     ROSE_ASSERT(argv.size() > 0);

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
          p_sourceFileNameList = CommandlineProcessing::generateSourceFilenames(argv);

       // printf ("In SgProject::processCommandLine(): p_sourceFileNameList.size() = %zu \n",p_sourceFileNameList.size());

#if 0
       // look only for .o  (object code files)
          if ( (length > 2) && 
               ( (argv[i][0] != '-') || (argv[i][0] != '+') ) && 
               ( (argv[i][length-2] == '.') && (argv[i][length-1] == 'o') ) )
             {
               std::string objectFile = argv[i];
               p_objectFileNameList.push_back(objectFile);
             }
#else
       // DQ (1/16/2008): This is a better (simpler) implementation
          if (CommandlineProcessing::isObjectFilename(argv[i]) == true)
               p_objectFileNameList.push_back(argv[i]);
#endif

       // look only for .a files (library files)
          if ( (length > 2) &&
               ( (argv[i][0] != '-') && (argv[i][0] != '+') ) &&
               ( (argv[i][length-2] == '.') && (argv[i][length-1] == 'a') ) )
             {
               std::string libraryFile = argv[i];
               p_libraryFileList.push_back(libraryFile);
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
	      //AS Changed source code to support absolute paths
	      std::string includeDirectorySpecifier =  argv[i].substr(2);
	      includeDirectorySpecifier = StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
              p_includeDirectorySpecifierList.push_back("-I"+includeDirectorySpecifier);
             }
        }

#if 0
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
          printf ("-rose:astMerge option found \n");
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
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
       // ROSE::usage(0);
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
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
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
               printf ("skip syntax checking mode ON \n");
          set_skip_syntax_check(true);
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

  // DQ (12/27/2007): Allow defaults to be set based on filename extension.
     if ( CommandlineProcessing::isOption(argv,"-rose:","(binary|binary_only)",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran only mode ON \n");
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

  // DQ (8/19/2007): I have added the option here so that we can start to support OpenMP for Fortran.
  // Allows handling of OpenMP "!$omp" directives in free form and "c$omp", *$omp and "!$omp" directives in fixed form, enables "!$" conditional 
  // compilation sentinels in free form and "c$", "*$" and "!$" sentinels in fixed form and when linking arranges for the OpenMP runtime library 
  // to be linked in. (Not implemented yet).
     set_fortran_openmp(false);
     ROSE_ASSERT (get_fortran_openmp() == false);
     if ( CommandlineProcessing::isOption(argv,"-","fopenmp",true) == true )
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("Fortran OpenMP option specified \n");
          set_fortran_openmp(true);
          if (get_sourceFileUsesFortranFileExtension() == false)
             {
               printf ("Warning, Non Fortran source file name specificed with explicit OpenMP option! \n");
               set_fortran_openmp(false);
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
SgFile::stripRoseCommandLineOptions ( vector<string>& argv )
   {
  // Strip out the rose specific commandline options
  // the assume all other arguments are to be passed onto the C or C++ compiler

     int optionCount = 0;
  // int i = 0;

#if ROSE_INTERNAL_DEBUG
  // printf ("ROSE_DEBUG = %d \n",ROSE_DEBUG);
  // printf ("get_verbose() = %s value = %d \n",(get_verbose() > 1) ? "true" : "false",get_verbose());

     if ( (ROSE_DEBUG >= 1) || (get_verbose() > 2 )
        {
          printf ("In stripRoseCommandLineOptions: List ALL arguments: argc = %zu \n",argv.size());
          for (size_t i=0; i < argv.size(); i++)
               printf ("     argv[%d] = %s \n",i,argv[i]);
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
     optionCount = sla(argv, "-rose:", "($)", "(C|C_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(C99|C99_only)",1);
     optionCount = sla(argv, "-rose:", "($)", "(Cxx|Cxx_only)",1);

     optionCount = sla(argv, "-rose:", "($)", "(output_warnings)",1);
     optionCount = sla(argv, "-rose:", "($)", "(cray_pointer_support)",1);

     optionCount = sla(argv, "-rose:", "($)", "(output_parser_actions)",1);
     optionCount = sla(argv, "-rose:", "($)", "(exit_after_parser)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_syntax_check)",1);

  // DQ (8/11/2007): Support for Fortran and its different flavors
     optionCount = sla(argv, "-rose:", "($)", "(f|F|Fortran)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f77|F77|Fortran77)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f90|F90|Fortran90)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f95|F95|Fortran95)",1);
     optionCount = sla(argv, "-rose:", "($)", "(f2003|F2003|Fortran2003)",1);

  // DQ (8/27/2007):Support for Fortran language output format

     optionCount = sla(argv, "-rose:", "($)", "(fixedOutput|fixedFormatOutput)",1);
     optionCount = sla(argv, "-rose:", "($)", "(freeOutput|freeFormatOutput)",1);

     optionCount = sla(argv, "-rose:", "($)", "(compileFixed|backendCompileFixedFormat)",1);
     optionCount = sla(argv, "-rose:", "($)", "(compileFree|backendCompileFreeFormat)",1);

     optionCount = sla(argv, "-rose:", "($)", "(C_output_language|Cxx_output_language|Fortran_output_language|Promela_output_language)",1);

  // DQ (5/19/2005): The output file name is constructed from the input source name (as I recall)
  // optionCount = sla(argv, "-rose:", "($)^", "(o|output)", &p_unparse_output_filename ,1);

     optionCount = sla(argv, "-rose:", "($)", "(skip_rose)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_transformation)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_unparse)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_includes)",1);
     optionCount = sla(argv, "-rose:", "($)", "(unparse_line_directives)",1);
     optionCount = sla(argv, "-rose:", "($)", "(collectAllCommentsAndDirectives)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skip_commentsAndDirectives)",1);
     optionCount = sla(argv, "-rose:", "($)", "(skipfinalCompileStep)",1);
     optionCount = sla(argv, "-rose:", "($)", "(prelink)",1);
     optionCount = sla(argv, "-"     , "($)", "(ansi)",1);
     optionCount = sla(argv, "-rose:", "($)", "(markGeneratedFiles)",1);
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
  // would influence how ROSE instatiates or outputs templates in the code generation phase.

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

#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
       // Detect if this is a binary (executable) file!

#include "elf.h"

// DQ (2/11/2008): Added support for reading binaries.
// Note that there is a redundant use of the SelectObject name so I have placed the Wine header files into a namespace.
//      ROSE/src/util/support/FunctionObject.h:5: error: previous declaration of `template<class T> class SelectObject'
// We are using the Wine project and their header files to handle the details of the structure of Windows binaries.
#if 0
namespace Rose_Wine
   {
// I have modified the win.h file to change the data member "class" to "window_class" (see note in win.h).
#include "win.h"
   }
#endif

bool
isBinaryExecutableFile ( string sourceFilename )
   {
     bool returnValue = false;

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "r");
     if (!f)
        {
          printf ("Could not open file");
          ROSE_ASSERT(false);
        }

     int character0 = fgetc(f);

  // The first character of an ELF binary is '\127' and for a PE binary it is 'M'
  // if (character0 == 127)
     if (character0 == 127 || character0 == 77)
        {
          returnValue = true;
        }

      fclose(f);

      return returnValue;
    }



string
processorArchitectureName (SgAsmFile::pe_processor_architecture_enum processor_architecture_kind)
   {
  // This function supports the Portable Execution binary format (PE) used in Windows binaries.
  // PE format is a variation of the COFF format used by IBM and others.

     string s;

     switch (processor_architecture_kind)
        {
          case SgAsmFile::e_processor_architecture_none:    s = "No machine"; break;
          case SgAsmFile::e_processor_architecture_INTEL:   s = "INTEL";      break;
          case SgAsmFile::e_processor_architecture_MIPS:    s = "MIPS";       break;
          case SgAsmFile::e_processor_architecture_ALPHA:   s = "ALPHA";      break;
          case SgAsmFile::e_processor_architecture_PPC:     s = "PPC";        break;
          case SgAsmFile::e_processor_architecture_SHX:     s = "SHX";        break;
          case SgAsmFile::e_processor_architecture_ARM:     s = "ARM";        break;
          case SgAsmFile::e_processor_architecture_IA64:    s = "IA64";       break;
          case SgAsmFile::e_processor_architecture_ALPHA64: s = "ALPHA64";    break;
          case SgAsmFile::e_processor_architecture_MSIL:    s = "MSIL";       break;
          case SgAsmFile::e_processor_architecture_AMD64:   s = "AMD64";      break;
          case SgAsmFile::e_processor_architecture_UNKNOWN: s = "UNKNOWN";    break;

          default:
             {
               s = "error";

               printf ("Error: default reach for processor_architecture_type = %d \n",processor_architecture_kind);
             }
        }

     return s;
   }

string
processorTypeName (SgAsmFile::pe_processor_type_enum processor_type_kind)
   {
  // This function supports the Portable Execution binary format (PE) used in Windows binaries.
  // PE format is a variation of the COFF format used by IBM and others.

     string s;

     switch (processor_type_kind)
        {
          case SgAsmFile::e_processor_type_none:          s = "No machine";    break;
          case SgAsmFile::e_processor_type_INTEL_386:     s = "INTEL_386";     break;
          case SgAsmFile::e_processor_type_INTEL_486:     s = "INTEL_486";     break;
          case SgAsmFile::e_processor_type_INTEL_PENTIUM: s = "INTEL_PENTIUM"; break;
          case SgAsmFile::e_processor_type_INTEL_860:     s = "INTEL_860";     break;
          case SgAsmFile::e_processor_type_INTEL_IA64:    s = "INTEL_IA64";    break;
          case SgAsmFile::e_processor_type_AMD_X8664:     s = "AMD_X8664";     break;
          case SgAsmFile::e_processor_type_MIPS_R2000:    s = "MIPS_R2000";    break;
          case SgAsmFile::e_processor_type_MIPS_R3000:    s = "MIPS_R3000";    break;
          case SgAsmFile::e_processor_type_MIPS_R4000:    s = "MIPS_R4000";    break;
          case SgAsmFile::e_processor_type_ALPHA_21064:   s = "ALPHA_21064";   break;
          case SgAsmFile::e_processor_type_PPC_601:       s = "PPC_601";       break;
          case SgAsmFile::e_processor_type_PPC_603:       s = "PPC_603";       break;
          case SgAsmFile::e_processor_type_PPC_604:       s = "PPC_604";       break;
          case SgAsmFile::e_processor_type_PPC_620:       s = "PPC_620";       break;
          case SgAsmFile::e_processor_type_HITACHI_SH3:   s = "HITACHI_SH3";   break;
          case SgAsmFile::e_processor_type_HITACHI_SH3E:  s = "HITACHI_SH3E";  break;
          case SgAsmFile::e_processor_type_HITACHI_SH4:   s = "HITACHI_SH4";   break;
          case SgAsmFile::e_processor_type_MOTOROLA_821:  s = "MOTOROLA_821";  break;
          case SgAsmFile::e_processor_type_SHx_SH3:       s = "SHx_SH3";       break;
          case SgAsmFile::e_processor_type_SHx_SH4:       s = "SHx_SH4";       break;
          case SgAsmFile::e_processor_type_STRONGARM:     s = "STRONGARM";     break;
          case SgAsmFile::e_processor_type_ARM720:        s = "ARM720";        break;
          case SgAsmFile::e_processor_type_ARM820:        s = "ARM820";        break;
          case SgAsmFile::e_processor_type_ARM920:        s = "ARM920";        break;
          case SgAsmFile::e_processor_type_ARM_7TDMI:     s = "ARM_7TDMI";     break;

          default:
             {
               s = "error";

               printf ("Error: default reach for processor_type_type = %d \n",processor_type_kind);
             }
        }

     return s;
   }



string
machineArchitectureName (SgAsmFile::elf_machine_architecture_enum machine_architecture_kind)
   {
     string s;

     switch (machine_architecture_kind)
        {
          case SgAsmFile::e_machine_architecture_none:                     s = "No machine";               break;
          case SgAsmFile::e_machine_architecture_ATT_WE_32100:             s = "AT&T WE 32100";            break;
          case SgAsmFile::e_machine_architecture_Sun_Sparc:                s = "SUN SPARC";                break;
          case SgAsmFile::e_machine_architecture_Intel_80386:              s = "Intel 80386";              break;
          case SgAsmFile::e_machine_architecture_Motorola_m68k_family:     s = "Motorola m68k family";     break;
          case SgAsmFile::e_machine_architecture_Motorola_m88k_family:     s = "Motorola m88k family";     break;
          case SgAsmFile::e_machine_architecture_Intel_80860:              s = "Intel 80860";              break;
          case SgAsmFile::e_machine_architecture_MIPS_R3000_big_endian:    s = "MIPS R3000 big-endian";    break;
          case SgAsmFile::e_machine_architecture_IBM_System_370:           s = "IBM System/370";           break;
          case SgAsmFile::e_machine_architecture_MIPS_R3000_little_endian: s = "MIPS R3000 little-endian"; break;
          case SgAsmFile::e_machine_architecture_HPPA:               s = "HPPA";                 break;
          case SgAsmFile::e_machine_architecture_Fujitsu_VPP500:     s = "Fujitsu VPP500";       break;
          case SgAsmFile::e_machine_architecture_Sun_v8plus:         s = "Sun's v8plus";         break;
          case SgAsmFile::e_machine_architecture_Intel_80960:        s = "Intel 80960";          break;
          case SgAsmFile::e_machine_architecture_PowerPC:            s = "PowerPC";              break;
          case SgAsmFile::e_machine_architecture_PowerPC_64bit:      s = "PowerPC 64-bit";       break;
          case SgAsmFile::e_machine_architecture_IBM_S390:           s = "IBM S390";             break;
          case SgAsmFile::e_machine_architecture_NEC_V800_series:    s = "NEC V800 series";      break;
          case SgAsmFile::e_machine_architecture_Fujitsu_FR20:       s = "Fujitsu FR20";         break;
          case SgAsmFile::e_machine_architecture_TRW_RH_32:          s = "TRW RH-32";            break;
          case SgAsmFile::e_machine_architecture_Motorola_RCE:       s = "Motorola RCE";         break;
          case SgAsmFile::e_machine_architecture_ARM:                s = "ARM";                  break;
          case SgAsmFile::e_machine_architecture_Digital_Alpha_fake: s = "Digital Alpha (fake)"; break;
          case SgAsmFile::e_machine_architecture_Hitachi_SH:         s = "Hitachi SH";           break;
          case SgAsmFile::e_machine_architecture_SPARC_v9_64bit:     s = "SPARC v9 64-bit";      break;
          case SgAsmFile::e_machine_architecture_Siemens_Tricore:    s = "Siemens Tricore";      break;
          case SgAsmFile::e_machine_architecture_Argonaut_RISC_Core: s = "Argonaut RISC Core";   break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8_300:     s = "Hitachi H8/300";       break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8_300H:    s = "Hitachi H8/300H";      break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8S:        s = "Hitachi H8S";          break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8_500:     s = "Hitachi H8/500";       break;
          case SgAsmFile::e_machine_architecture_Intel_Merced:       s = "Intel Merced";         break;
          case SgAsmFile::e_machine_architecture_Stanford_MIPS_X:    s = "Stanford MIPS-X";      break;
          case SgAsmFile::e_machine_architecture_Motorola_Coldfire:  s = "Motorola Coldfire";    break;
          case SgAsmFile::e_machine_architecture_Motorola_M68HC12:   s = "Motorola M68HC12";     break;
          case SgAsmFile::e_machine_architecture_Fujitsu_MMA_Multimedia_Accelerator:   s = "Fujitsu MMA Multimedia Accelerator"; break;
          case SgAsmFile::e_machine_architecture_Siemens_PCP:                          s = "Siemens PCP";                        break;
          case SgAsmFile::e_machine_architecture_Sony_nCPU_embeeded_RISC:              s = "Sony nCPU embeeded RISC";            break;
          case SgAsmFile::e_machine_architecture_Denso_NDR1_microprocessor:            s = "Denso NDR1 microprocessor";          break;
          case SgAsmFile::e_machine_architecture_Motorola_Start_Core_processor:        s = "Motorola Start*Core processor";      break;
          case SgAsmFile::e_machine_architecture_Toyota_ME16_processor:                s = "Toyota ME16 processor";              break;
          case SgAsmFile::e_machine_architecture_STMicroelectronic_ST100_processor:    s = "STMicroelectronic ST100 processor";  break;
          case SgAsmFile::e_machine_architecture_Advanced_Logic_Corp_Tinyj_emb_family: s = "Advanced Logic Corp. Tinyj emb.fam"; break;
          case SgAsmFile::e_machine_architecture_AMD_x86_64_architecture:              s = "AMD x86-64 architecture";            break;
          case SgAsmFile::e_machine_architecture_Sony_DSP_Processor:                   s = "Sony DSP Processor";                 break;
          case SgAsmFile::e_machine_architecture_Siemens_FX66_microcontroller:         s = "Siemens FX66 microcontroller";       break;
          case SgAsmFile::e_machine_architecture_STMicroelectronics_ST9_plus_8_16_micocontroler: s = "STMicroelectronics ST9+ 8/16 mc";   break;
          case SgAsmFile::e_machine_architecture_STMicroelectronics_ST7_8bit_micocontroler:      s = "STmicroelectronics ST7 8 bit mc";   break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC16_microcontroller:              s = "Motorola MC68HC16 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC11_microcontroller:              s = "Motorola MC68HC11 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC08_microcontroller:              s = "Motorola MC68HC08 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC05_microcontroller:              s = "Motorola MC68HC05 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Silicon_Graphics_SVx:                           s = "Silicon Graphics SVx";              break;
          case SgAsmFile::e_machine_architecture_STMicroelectronics_ST19_8bit_microcontroller:   s = "STMicroelectronics ST19 8 bit mc";  break;
          case SgAsmFile::e_machine_architecture_Digital_VAX:                                    s = "Digital VAX";                       break;
          case SgAsmFile::e_machine_architecture_Axis_Communications_32bit_embedded_processor:   s = "Axis Communications 32-bit embedded processor";   break;
          case SgAsmFile::e_machine_architecture_Infineon_Technologies_32bit_embedded_processor: s = "Infineon Technologies 32-bit embedded processor"; break;
          case SgAsmFile::e_machine_architecture_Element_14_64bit_DSP_Processor:                 s = "Element 14 64-bit DSP Processor";                 break;
          case SgAsmFile::e_machine_architecture_LSI_Logic_16bit_DSP_Processor:                  s = "LSI Logic 16-bit DSP Processor";                  break;
          case SgAsmFile::e_machine_architecture_Donald_Knuths_educational_64bit_processor:      s = "Donald Knuth's educational 64-bit processor";     break;
          case SgAsmFile::e_machine_architecture_Harvard_University_machine_independent_object_files: s = "Harvard University machine-independent object files"; break;
          case SgAsmFile::e_machine_architecture_SiTera_Prism:                      s = "SiTera Prism";                       break;
          case SgAsmFile::e_machine_architecture_Atmel_AVR_8bit_microcontroller:    s = "Atmel AVR 8-bit microcontroller";    break;
          case SgAsmFile::e_machine_architecture_Fujitsu_FR30:                      s = "Fujitsu FR30";                       break;
          case SgAsmFile::e_machine_architecture_Mitsubishi_D10V:                   s = "Mitsubishi D10V";                    break;
          case SgAsmFile::e_machine_architecture_Mitsubishi_D30V:                   s = "Mitsubishi D30V";                    break;
          case SgAsmFile::e_machine_architecture_NEC_v850:                          s = "NEC v850";                           break;
          case SgAsmFile::e_machine_architecture_Mitsubishi_M32R:                   s = "Mitsubishi M32R";                    break;
          case SgAsmFile::e_machine_architecture_Matsushita_MN10300:                s = "Matsushita MN10300";                 break;
          case SgAsmFile::e_machine_architecture_Matsushita_MN10200:                s = "Matsushita MN10200";                 break;
          case SgAsmFile::e_machine_architecture_picoJava:                          s = "picoJava";                           break;
          case SgAsmFile::e_machine_architecture_OpenRISC_32bit_embedded_processor: s = "OpenRISC 32-bit embedded processor"; break;
          case SgAsmFile::e_machine_architecture_ARC_Cores_Tangent_A5:              s = "ARC Cores Tangent-A5";               break;
          case SgAsmFile::e_machine_architecture_Tensilica_Xtensa_Architecture:     s = "Tensilica Xtensa Architecture";      break;
          case SgAsmFile::e_machine_architecture_Digital_Alpha:                     s = "Digital Alpha";                      break;

          default:
             {
               s = "error";

               printf ("Error: default reach for machine_architecture_type = %d \n",machine_architecture_kind);
             }
        }

     return s;
   }

#if 0
void
generateBinaryExecutableFileInformation_Windows ( string sourceFilename, SgAsmFile* asmFile )
   {
     ROSE_ASSERT(asmFile != NULL);

     ROSE_ASSERT(isBinaryExecutableFile(sourceFilename) == true);

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "r");
     if (!f)
        {
          printf ("Could not open binary file = %s \n",sourceFilename.c_str());
          ROSE_ASSERT(false);
        }

     int firstCharacter = fgetc(f);

     printf ("In generateBinaryExecutableFileInformation_Windows(): firstCharacter = %d \n",firstCharacter);
     ROSE_ASSERT(firstCharacter == 77);

  // This is likely a binary executable
  // fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

#if 0
// This is the MSDOS header
typedef struct _IMAGE_DOS_HEADER {
    WORD  e_magic;      /* 00: MZ Header signature */
    WORD  e_cblp;       /* 02: Bytes on last page of file */
    WORD  e_cp;         /* 04: Pages in file */
    WORD  e_crlc;       /* 06: Relocations */
    WORD  e_cparhdr;    /* 08: Size of header in paragraphs */
    WORD  e_minalloc;   /* 0a: Minimum extra paragraphs needed */
    WORD  e_maxalloc;   /* 0c: Maximum extra paragraphs needed */
    WORD  e_ss;         /* 0e: Initial (relative) SS value */
    WORD  e_sp;         /* 10: Initial SP value */
    WORD  e_csum;       /* 12: Checksum */
    WORD  e_ip;         /* 14: Initial IP value */
    WORD  e_cs;         /* 16: Initial (relative) CS value */
    WORD  e_lfarlc;     /* 18: File address of relocation table */
    WORD  e_ovno;       /* 1a: Overlay number */
    WORD  e_res[4];     /* 1c: Reserved words */
    WORD  e_oemid;      /* 24: OEM identifier (for e_oeminfo) */
    WORD  e_oeminfo;    /* 26: OEM information; e_oemid specific */
    WORD  e_res2[10];   /* 28: Reserved words */
    DWORD e_lfanew;     /* 3c: Offset to extended header */
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;
#endif

  // The first x3c bytes are the MS-DOS header, the last word is the offset to the PE header (extended header)
     Rose_Wine::IMAGE_DOS_HEADER msdos_header;
     char* msdos_header_array = (char*) &msdos_header;

     msdos_header_array[0]  = firstCharacter;

     int msdos_header_value = 0;
     int msdos_header_size = sizeof(Rose_Wine::IMAGE_DOS_HEADER);
     ROSE_ASSERT(msdos_header_size == 64);
     int i = 1;
     do {
          msdos_header_value = fgetc(f);
          msdos_header_array[i]  = msdos_header_value;
          i++;
        }
  // while (i <= msdos_header_size && msdos_header_value != EOF);
     while (i < msdos_header_size && msdos_header_value != EOF);

     printf ("msdos_header.e_magic    = %d \n",(int)msdos_header.e_magic);
     printf ("msdos_header.e_cblp     = %d \n",(int)msdos_header.e_cblp);
     printf ("msdos_header.e_cp       = %d \n",(int)msdos_header.e_cp);
     printf ("msdos_header.e_crlc     = %d \n",(int)msdos_header.e_crlc);
     printf ("msdos_header.e_cparhdr  = %d \n",(int)msdos_header.e_cparhdr);
     printf ("msdos_header.e_minalloc = %d \n",(int)msdos_header.e_minalloc);
     printf ("msdos_header.e_maxalloc = %d \n",(int)msdos_header.e_maxalloc);
     printf ("msdos_header.e_ss       = %d \n",(int)msdos_header.e_ss);
     printf ("msdos_header.e_sp       = %d \n",(int)msdos_header.e_sp);
     printf ("msdos_header.e_csum     = %d \n",(int)msdos_header.e_csum);
     printf ("msdos_header.e_ip       = %d \n",(int)msdos_header.e_ip);
     printf ("msdos_header.e_cs       = %d \n",(int)msdos_header.e_cs);
     printf ("msdos_header.e_lfarlc   = %d \n",(int)msdos_header.e_lfarlc);
     printf ("msdos_header.e_ovno     = %d \n",(int)msdos_header.e_ovno);
     printf ("msdos_header.e_res[0]   = %d \n",(int)msdos_header.e_res[0]);
     printf ("msdos_header.e_res[1]   = %d \n",(int)msdos_header.e_res[1]);
     printf ("msdos_header.e_res[2]   = %d \n",(int)msdos_header.e_res[2]);
     printf ("msdos_header.e_res[3]   = %d \n",(int)msdos_header.e_res[3]);
     printf ("msdos_header.e_oemid    = %d \n",(int)msdos_header.e_oemid);
     printf ("msdos_header.e_oeminfo  = %d \n",(int)msdos_header.e_oeminfo);
     printf ("msdos_header.e_res2[0]  = %d \n",(int)msdos_header.e_res2[0]);
     printf ("msdos_header.e_res2[1]  = %d \n",(int)msdos_header.e_res2[1]);
     printf ("msdos_header.e_res2[2]  = %d \n",(int)msdos_header.e_res2[2]);
     printf ("msdos_header.e_res2[3]  = %d \n",(int)msdos_header.e_res2[3]);
     printf ("msdos_header.e_res2[4]  = %d \n",(int)msdos_header.e_res2[4]);
     printf ("msdos_header.e_res2[5]  = %d \n",(int)msdos_header.e_res2[5]);
     printf ("msdos_header.e_res2[6]  = %d \n",(int)msdos_header.e_res2[6]);
     printf ("msdos_header.e_res2[7]  = %d \n",(int)msdos_header.e_res2[7]);
     printf ("msdos_header.e_res2[8]  = %d \n",(int)msdos_header.e_res2[8]);
     printf ("msdos_header.e_res2[9]  = %d \n",(int)msdos_header.e_res2[9]);
     printf ("msdos_header.e_lfanew   = %d \n",(int)msdos_header.e_lfanew);

  // printf ("Exiting at base of generateBinaryExecutableFileInformation_Windows() after reading the msdos header \n");
  // ROSE_ASSERT(false);

     printf ("Read the file up to the PE header msdos_header.e_lfanew = %d \n",msdos_header.e_lfanew);

     for (int i = msdos_header_size; i < msdos_header.e_lfanew; i++)
        {
          int value = fgetc(f);
          ROSE_ASSERT(value != EOF);
        }

     printf ("Now read the PE header \n");

  // Now read the PE header!

     char characterArray [4+1];
     for (int i=0; i < 4; i++)
          characterArray[i] = 'X';
     characterArray[4] = '\0';

  // string magic_number_string(char[EI_NIDENT]);
     string magic_number_string = characterArray;
  // printf ("magic_number_string.size() = %zu \n",magic_number_string.size());
     ROSE_ASSERT(magic_number_string.size() == 4);

     int pe_string[4] = { -1, -1, -1, -1 };
     int magic_number_array[4];

     magic_number_array [0] = firstCharacter;
     magic_number_string[0] = (char)firstCharacter;

     int magic_number_value = 0;
     i = 0;
     do {
          magic_number_value     = fgetc(f);

          printf ("magic_number_value = %d \n",magic_number_value);
          char charValue = (char)magic_number_value;
          if (charValue >= 'a' && charValue <= 'Z')
               printf ("charValue = %c \n",charValue);

          pe_string[i]           = magic_number_value;
          magic_number_array[i]  = magic_number_value;
          magic_number_string[i] = (char)magic_number_value;
          i++;
        }
  // while (i <= 3 && magic_number_value != EOF);
     while (i < 4 && magic_number_value != EOF);

     ROSE_ASSERT(magic_number_value != EOF);

  // The PE format has the string "PE\0\0" in the first 4 bytes.
     if ( pe_string[0] == 'P' && pe_string[1] == 'E' && pe_string[2] == '\0' && pe_string[3] == '\0')
        {
          printf ("This is some sort of PE (likely Windows) binary executable \n");
        }
       else
        {
          printf ("Error: this is a binary, but not in COFF format \n");
          ROSE_ASSERT(false);
        }

#if 0
typedef struct _IMAGE_FILE_HEADER {
  WORD  Machine;
  WORD  NumberOfSections;
  DWORD TimeDateStamp;
  DWORD PointerToSymbolTable;
  DWORD NumberOfSymbols;
  WORD  SizeOfOptionalHeader;
  WORD  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;
#endif

  // This needs to be referenced later (to get the number of sections)
     Rose_Wine::IMAGE_FILE_HEADER coff_file_header;
     char* coff_file_header_array = (char*) &coff_file_header;

     int coff_file_header_value = 0;
     int coff_file_header_size = sizeof(Rose_Wine::IMAGE_FILE_HEADER);
  // ROSE_ASSERT(coff_file_header_size == 64);
     i = 0;
     do {
          coff_file_header_value = fgetc(f);
          coff_file_header_array[i]  = coff_file_header_value;
          i++;
        }
  // while (i <= msdos_header_size && msdos_header_value != EOF);
     while (i < coff_file_header_size && coff_file_header_value != EOF);

     printf ("coff_file_header.Machine                = %d \n",(int)coff_file_header.Machine);
     printf ("coff_file_header.NumberOfSections       = %d \n",(int)coff_file_header.NumberOfSections);
     printf ("coff_file_header.TimeDateStamp          = %d \n",(int)coff_file_header.TimeDateStamp);

  // Note that ctime() adds a "\n" at the end of the string that it returns.
     printf ("coff_file_header.TimeDateStamp (string) = %s",ctime((const time_t*)&coff_file_header.TimeDateStamp));

     printf ("coff_file_header.PointerToSymbolTable   = %d \n",(int)coff_file_header.PointerToSymbolTable);
     printf ("coff_file_header.NumberOfSymbols        = %d \n",(int)coff_file_header.NumberOfSymbols);
     printf ("coff_file_header.SizeOfOptionalHeader   = %d \n",(int)coff_file_header.SizeOfOptionalHeader);
     printf ("coff_file_header.Characteristics        = %p \n",(void*)coff_file_header.Characteristics);


  // Save the PE header in the SgAsmFile asmFile

#if 0
typedef struct _IMAGE_OPTIONAL_HEADER {

  /* Standard fields */

  WORD  Magic; /* 0x10b or 0x107 */	/* 0x00 */
  BYTE  MajorLinkerVersion;
  BYTE  MinorLinkerVersion;
  DWORD SizeOfCode;
  DWORD SizeOfInitializedData;
  DWORD SizeOfUninitializedData;
  DWORD AddressOfEntryPoint;		/* 0x10 */
  DWORD BaseOfCode;
  DWORD BaseOfData;

  /* NT additional fields */

  DWORD ImageBase;
  DWORD SectionAlignment;		/* 0x20 */
  DWORD FileAlignment;
  WORD  MajorOperatingSystemVersion;
  WORD  MinorOperatingSystemVersion;
  WORD  MajorImageVersion;
  WORD  MinorImageVersion;
  WORD  MajorSubsystemVersion;		/* 0x30 */
  WORD  MinorSubsystemVersion;
  DWORD Win32VersionValue;
  DWORD SizeOfImage;
  DWORD SizeOfHeaders;
  DWORD CheckSum;			/* 0x40 */
  WORD  Subsystem;
  WORD  DllCharacteristics;
  DWORD SizeOfStackReserve;
  DWORD SizeOfStackCommit;
  DWORD SizeOfHeapReserve;		/* 0x50 */
  DWORD SizeOfHeapCommit;
  DWORD LoaderFlags;
  DWORD NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; /* 0x60 */
  /* 0xE0 */
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;
#endif

     Rose_Wine::IMAGE_OPTIONAL_HEADER32 coff_optional_header;
     char* coff_optional_header_array = (char*) &coff_optional_header;

     int coff_optional_header_value = 0;
     int coff_optional_header_size = sizeof(Rose_Wine::IMAGE_OPTIONAL_HEADER32);

     printf ("coff_optional_header_size = %d sizeof(Rose_Wine::IMAGE_OPTIONAL_HEADER32) = %d coff_file_header.SizeOfOptionalHeader = %d \n",
          coff_optional_header_size,(int)sizeof(Rose_Wine::IMAGE_OPTIONAL_HEADER32),(int)coff_file_header.SizeOfOptionalHeader);

     ROSE_ASSERT(coff_optional_header_size == coff_file_header.SizeOfOptionalHeader);

     i = 0;
     do {
          coff_optional_header_value = fgetc(f);
          coff_optional_header_array[i]  = coff_optional_header_value;
          i++;
        }
     while (i < coff_optional_header_size && coff_optional_header_value != EOF);

     printf ("coff_optional_header.Magic                       = %p \n",(int*)coff_optional_header.Magic);
     printf ("coff_optional_header.MajorLinkerVersion          = %d \n",(int)coff_optional_header.MajorLinkerVersion);
     printf ("coff_optional_header.MinorLinkerVersion          = %d \n",(int)coff_optional_header.MinorLinkerVersion);
     printf ("coff_optional_header.SizeOfCode                  = %d \n",(int)coff_optional_header.SizeOfCode);
     printf ("coff_optional_header.SizeOfInitializedData       = %d \n",(int)coff_optional_header.SizeOfInitializedData);
     printf ("coff_optional_header.SizeOfUninitializedData     = %d \n",(int)coff_optional_header.SizeOfUninitializedData);
     printf ("coff_optional_header.AddressOfEntryPoint         = %p \n",(int*)coff_optional_header.AddressOfEntryPoint);
     printf ("coff_optional_header.BaseOfCode                  = %p \n",(int*)coff_optional_header.BaseOfCode);
     printf ("coff_optional_header.BaseOfData                  = %d \n",(int)coff_optional_header.BaseOfData);
     printf ("coff_optional_header.ImageBase                   = %p \n",(int*)coff_optional_header.ImageBase);
     printf ("coff_optional_header.SectionAlignment            = %d \n",(int)coff_optional_header.SectionAlignment);
     printf ("coff_optional_header.FileAlignment               = %d \n",(int)coff_optional_header.FileAlignment);
     printf ("coff_optional_header.MajorOperatingSystemVersion = %d \n",(int)coff_optional_header.MajorOperatingSystemVersion);
     printf ("coff_optional_header.MinorOperatingSystemVersion = %d \n",(int)coff_optional_header.MinorOperatingSystemVersion);
     printf ("coff_optional_header.MajorImageVersion           = %d \n",(int)coff_optional_header.MajorImageVersion);
     printf ("coff_optional_header.MinorSubsystemVersion       = %d \n",(int)coff_optional_header.MinorSubsystemVersion);
     printf ("coff_optional_header.Win32VersionValue           = %d \n",(int)coff_optional_header.Win32VersionValue);
     printf ("coff_optional_header.SizeOfImage                 = %d \n",(int)coff_optional_header.SizeOfImage);
     printf ("coff_optional_header.SizeOfHeaders               = %d \n",(int)coff_optional_header.SizeOfHeaders);
     printf ("coff_optional_header.CheckSum                    = %d \n",(int)coff_optional_header.CheckSum);
     printf ("coff_optional_header.Subsystem                   = %d \n",(int)coff_optional_header.Subsystem);
     printf ("coff_optional_header.DllCharacteristics          = %p \n",(int*)coff_optional_header.DllCharacteristics);
     printf ("coff_optional_header.SizeOfStackReserve          = %d \n",(int)coff_optional_header.SizeOfStackReserve);
     printf ("coff_optional_header.SizeOfStackCommit           = %d \n",(int)coff_optional_header.SizeOfStackCommit);
     printf ("coff_optional_header.SizeOfHeapReserve           = %d \n",(int)coff_optional_header.SizeOfHeapReserve);
     printf ("coff_optional_header.SizeOfHeapCommit            = %d \n",(int)coff_optional_header.SizeOfHeapCommit);
     printf ("coff_optional_header.LoaderFlags                 = %d \n",(int)coff_optional_header.LoaderFlags);
     printf ("coff_optional_header.NumberOfRvaAndSizes         = %d \n",(int)coff_optional_header.NumberOfRvaAndSizes);
  // printf ("coff_optional_header.              = %d \n",(int)coff_optional_header.);

  // Save the PE optional header in the SgAsmFile asmFile

     const int MaxNumberOfDirectoryHeaders = 100;
  // Rose_Wine::IMAGE_DATA_DIRECTORY coff_data_directory_headers[16];
     Rose_Wine::IMAGE_DATA_DIRECTORY coff_data_directory_headers[MaxNumberOfDirectoryHeaders];
     char* coff_data_directory_array = (char*) coff_data_directory_headers;

  // ROSE_ASSERT(coff_optional_header.NumberOfRvaAndSizes == 16);
     ROSE_ASSERT(coff_optional_header.NumberOfRvaAndSizes < MaxNumberOfDirectoryHeaders);

     int coff_data_directory_header_value = 0;
     int coff_data_directory_headers_size = coff_optional_header.NumberOfRvaAndSizes * sizeof(Rose_Wine::IMAGE_DATA_DIRECTORY);

     i = 0;
     do {
          coff_data_directory_header_value = fgetc(f);
          coff_data_directory_array[i]     = coff_data_directory_header_value;
          i++;
        }
     while (i < coff_data_directory_headers_size && coff_data_directory_header_value != EOF);

     for (int i = 0; i < coff_optional_header.NumberOfRvaAndSizes; i++)
        {
          printf ("coff_data_directory_headers[%d].VirtualAddress = %p \n",i,(void*)coff_data_directory_headers[i].VirtualAddress);
          printf ("coff_data_directory_headers[%d].Size           = %u \n",i,(unsigned int)coff_data_directory_headers[i].Size);
        }

#if 0
typedef struct _IMAGE_SECTION_HEADER {
  BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    DWORD PhysicalAddress;
    DWORD VirtualSize;
  } Misc;
  DWORD VirtualAddress;
  DWORD SizeOfRawData;
  DWORD PointerToRawData;
  DWORD PointerToRelocations;
  DWORD PointerToLinenumbers;
  WORD  NumberOfRelocations;
  WORD  NumberOfLinenumbers;
  DWORD Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;
#endif

#if 1
  // Reread the file from the start
     rewind(f);

     int coff_sectionHeaderFilePosition_value = 0;
     int signatureSize = 4;

  // The COFF header starts at a position specified by msdos_header.e_lfanew
  // int sectionHeaderFilePosition  = msdos_header_size + signatureSize + coff_file_header_size + coff_optional_header_size + coff_data_directory_headers_size;
     int sectionHeaderFilePosition  = msdos_header.e_lfanew + signatureSize + coff_file_header_size + coff_optional_header_size + coff_data_directory_headers_size;

     printf ("sectionHeaderFilePosition = %d \n",sectionHeaderFilePosition);

     i = 0;
     do {
          coff_sectionHeaderFilePosition_value = fgetc(f);
          i++;
        }
     while (i < sectionHeaderFilePosition && coff_sectionHeaderFilePosition_value != EOF);
#endif

  // Note that the Windows loader limits the number of sections to 96 (see PE spec: page 7 section 3.3).
     const int MaxNumberOfSectionHeaders = 96;
     Rose_Wine::IMAGE_SECTION_HEADER coff_section_headers[MaxNumberOfSectionHeaders];
     char* coff_section_array = (char*) coff_section_headers;

     ROSE_ASSERT(coff_file_header.NumberOfSections < MaxNumberOfSectionHeaders);

     unsigned int coff_section_header_value = 0;
     int coff_section_headers_size = coff_file_header.NumberOfSections * sizeof(Rose_Wine::IMAGE_SECTION_HEADER);

     i = 0;
     do {
          coff_section_header_value = fgetc(f);
          coff_section_array[i]     = coff_section_header_value;
          i++;
        }
     while (i < coff_section_headers_size && coff_section_header_value != EOF);

  // Assertion based on PE spec page 12 (SizeOfHeaders description)
     ROSE_ASSERT(sectionHeaderFilePosition + coff_section_headers_size < coff_optional_header.SizeOfHeaders);

     for (int i = 0; i < coff_file_header.NumberOfSections; i++)
        {
          unsigned char nameString[9];
          printf ("coff_section_headers[%d].Name (byte values)   = ",i);
          for (int j = 0; j < 8; j++)
             {
               nameString[i] = coff_section_headers[i].Name[j];
               printf ("%d ",nameString[i]);
             }
          nameString[8] = '\0';
          printf ("\n");

          printf ("coff_section_headers[%d].Name                 = \"%s\" \n",i,nameString);
          printf ("coff_section_headers[%d].Misc.PhysicalAddress = %d \n",i,(int)coff_section_headers[i].Misc.PhysicalAddress);
          printf ("coff_section_headers[%d].Misc.VirtualSize     = %d \n",i,(int)coff_section_headers[i].Misc.VirtualSize);
          printf ("coff_section_headers[%d].VirtualAddress       = %d \n",i,(int)coff_section_headers[i].VirtualAddress);
          printf ("coff_section_headers[%d].SizeOfRawData        = %d \n",i,(int)coff_section_headers[i].SizeOfRawData);
          printf ("coff_section_headers[%d].PointerToRawData     = %p \n",i,(void*)coff_section_headers[i].PointerToRawData);
          printf ("coff_section_headers[%d].PointerToRelocations = %p \n",i,(void*)coff_section_headers[i].PointerToRelocations);
          printf ("coff_section_headers[%d].PointerToLinenumbers = %p \n",i,(void*)coff_section_headers[i].PointerToLinenumbers);
          printf ("coff_section_headers[%d].NumberOfRelocations  = %d \n",i,(int)coff_section_headers[i].NumberOfRelocations);
          printf ("coff_section_headers[%d].NumberOfLinenumbers  = %d \n",i,(int)coff_section_headers[i].NumberOfLinenumbers);
          printf ("coff_section_headers[%d].Characteristics      = %p \n",i,(void*)coff_section_headers[i].Characteristics);
        }

  // Reread the file from the start
     rewind(f);









     printf ("Exiting at base of generateBinaryExecutableFileInformation_Windows() \n");
     ROSE_ASSERT(false);
   }
#endif
 
void
generateBinaryExecutableFileInformation_ELF ( string sourceFilename, SgAsmFile* asmFile )
   {
     ROSE_ASSERT(asmFile != NULL);

     ROSE_ASSERT(isBinaryExecutableFile(sourceFilename) == true);

  // Open file for reading
     FILE* f = fopen(sourceFilename.c_str(), "r");
     if (!f)
        {
          printf ("Could not open binary file = %s \n",sourceFilename.c_str());
          ROSE_ASSERT(false);
        }

     int firstCharacter = fgetc(f);
     ROSE_ASSERT(firstCharacter == 127);

  // This is likely a binary executable
  // fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

     char characterArray [EI_NIDENT+1];
     for (int i=0; i < EI_NIDENT; i++)
          characterArray[i] = 'X';
     characterArray[EI_NIDENT] = '\0';

  // string magic_number_string(char[EI_NIDENT]);
     string magic_number_string = characterArray;
  // printf ("magic_number_string.size() = %zu \n",magic_number_string.size());
     ROSE_ASSERT(magic_number_string.size() == EI_NIDENT);

     int elf_string[3] = { -1, -1, -1 };
     int magic_number_array[EI_NIDENT];

     magic_number_array[0] = firstCharacter;
     magic_number_string[0] = (char)firstCharacter;

     int magic_number_value = 0;
     int i = 1;
     do {
          magic_number_value = fgetc(f);
          elf_string[i-1] = magic_number_value;
          magic_number_array[i] = magic_number_value;
          magic_number_string[i] = (char)magic_number_value;
          i++;
        }
     while (i <= 3 && magic_number_value != EOF);

     ROSE_ASSERT(magic_number_value != EOF);

     if ( elf_string[0] == 'E' && elf_string[1] == 'L' && elf_string[2] == 'F' )
        {
       // printf ("This is some sort of ELF binary executable \n");

       // Make sure that this is what I expect!
          ROSE_ASSERT(EI_NIDENT == 16);

       // Gather the remaining magic numbers (now that we are sure this is an ELF file)
          do {
               magic_number_value = fgetc(f);
               magic_number_array [i] = magic_number_value;
               magic_number_string[i] = (char)magic_number_value;
               i++;
             }
          while (i < EI_NIDENT && magic_number_value != EOF);
          ROSE_ASSERT(magic_number_value != EOF);

          asmFile->set_magic_number_string(magic_number_string);
        }
       else
        {
          printf ("Error: this is a binary, but not ELF \n");
          ROSE_ASSERT(false);
        }

#if 0
  // At this point we have the ELF header magic number array (print it out)
     for (int i=0; i < EI_NIDENT; i++)
        {
          printf ("ELF header magic_number_array[%d] = %d \n",i,magic_number_array[i]);
        }
#endif

  // Reread the file from the start
     rewind(f);

     Elf32_Ehdr elf_32_bit_header;
     Elf64_Ehdr elf_64_bit_header;

     int elfHeaderSize_32bit = sizeof(Elf32_Ehdr);
     int elfHeaderSize_64bit = sizeof(Elf64_Ehdr);

  // printf ("elfHeaderSize_32bit = %d elfHeaderSize_64bit = %d \n",elfHeaderSize_32bit,elfHeaderSize_64bit);
     ROSE_ASSERT(elfHeaderSize_32bit <= elfHeaderSize_64bit);

     int elementsRead = 0;

  // Read the header as a 32bit elf file header first, if there is an error then try as 64 bit elf header.
     elementsRead = fread(&elf_32_bit_header,1,elfHeaderSize_32bit,f);
     if (elementsRead != elfHeaderSize_32bit)
        {
          printf ("Error in reading to the end of the elf_32_bit_header (read only %d bytes, elf header is %d bytes) \n",elementsRead,elfHeaderSize_32bit);
          ROSE_ASSERT(false);
        }

     bool isA32bitElfHeader = false;
     bool isA64bitElfHeader = false;

     ROSE_ASSERT(EI_CLASS == 4);
     if (magic_number_array[EI_CLASS] == ELFCLASS32)
        {
       // This appears to be a 32 bit (class 1) elf file.
          if ( SgProject::get_verbose() >= 1 )
               printf ("This appears to be a 32 bit (class 1) ELF file \n");

          isA32bitElfHeader = true;

          asmFile->set_binary_class_type(SgAsmFile::e_class_32);
        }
       else
        {
       // Reread the file from the start
          rewind(f);

       // Read the header as a 64bit elf file header (second), if there is an error then there is some other problem!
          elementsRead = fread(&elf_64_bit_header,1,elfHeaderSize_64bit,f);
          if (elementsRead != elfHeaderSize_64bit)
             {
               printf ("Error in reading to the end of the elf_32_bit_header (read only %d bytes, elf header is %d bytes) \n",elementsRead,elfHeaderSize_32bit);
               ROSE_ASSERT(false);
             }

       // if (elf_64_bit_header.e_type == ELFCLASS64)
          if (magic_number_array[EI_CLASS] == ELFCLASS64)
             {
            // This appears to be a 64 bit (class 1) elf file.
               if ( SgProject::get_verbose() >= 1 )
                    printf ("This appears to be a 64 bit (class 2) ELF file \n");
               isA64bitElfHeader = true;

               asmFile->set_binary_class_type(SgAsmFile::e_class_64);
             }
            else
             {
               if (magic_number_array[EI_CLASS] == ELFCLASSNONE)
                  {
                    printf ("magic_number_array[4] marked as ELFCLASSNONE \n");
                    asmFile->set_binary_class_type(SgAsmFile::e_class_none);
                  }
                 else
                  {
                    asmFile->set_binary_class_type(SgAsmFile::e_class_unknown);
                  }
             }
        }

  // Only one of these should be true.
     ROSE_ASSERT(isA32bitElfHeader == true  || isA64bitElfHeader == true);
     ROSE_ASSERT(isA32bitElfHeader == false || isA64bitElfHeader == false);

     ROSE_ASSERT(EI_DATA == 5);
     switch (magic_number_array[EI_DATA])
        {
          case ELFDATANONE:
            // This appears to be an error 
            // printf ("Error: Data encoding is ELFDATANONE \n");
               asmFile->set_data_encoding(SgAsmFile::e_data_encoding_none);
               break;

          case ELFDATA2LSB:
            // little endian 
            // printf ("Data encoding is LITTLE endian \n");
               asmFile->set_data_encoding(SgAsmFile::e_data_encoding_least_significant_byte);
               break;

          case ELFDATA2MSB:
            // big endian 
            // printf ("Data encoding is BIG endian \n");
               asmFile->set_data_encoding(SgAsmFile::e_data_encoding_most_significant_byte);
               break;

          default:
             {
               printf ("Error: default reached in data encoding specification magic_number_array[EI_DATA] = %d \n",magic_number_array[EI_DATA]);
            // ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(EI_VERSION == 6);
     switch (magic_number_array[EI_VERSION])
        {
          case EV_NONE:
            // This appears to be an error 
            // printf ("Error in ELF version number: magic_number_array[EI_VERSION] = EV_NONE \n");
               asmFile->set_version(SgAsmFile::e_version_none);
               break;

          case EV_CURRENT:
            // Only acceptable value for ELF
            // printf ("Only acceptable value for ELF version number: magic_number_array[EI_VERSION] = EV_CURRENT \n");
               asmFile->set_version(SgAsmFile::e_version_current);
               break;

          default:
             {
               printf ("Error: default reached in ELF version specification magic_number_array[EI_VERSION] = %d \n",magic_number_array[EI_VERSION]);
             }
        }

  // Now set the object file type
     SgAsmFile::elf_object_file_type_enum object_file_kind = SgAsmFile::e_file_type_error;
     int object_file_type = 0;
     if (isA32bitElfHeader == true)
        {
          ROSE_ASSERT(isA64bitElfHeader == false);
          object_file_type = elf_32_bit_header.e_type;
        }
       else
        {
          ROSE_ASSERT(isA64bitElfHeader == true);
          object_file_type = elf_64_bit_header.e_type;
        }

     switch (object_file_type)
        {
          case ET_NONE: 
               object_file_kind = SgAsmFile::e_file_type_none;
               break;

          case ET_REL: 
               object_file_kind = SgAsmFile::e_file_type_relocatable;
               break;

          case ET_EXEC: 
               object_file_kind = SgAsmFile::e_file_type_executable;
               break;

          case ET_DYN: 
               object_file_kind = SgAsmFile::e_file_type_shared;
               break;

          case ET_CORE: 
               object_file_kind = SgAsmFile::e_file_type_core;
               break;

          default:
             {
               printf ("Error: default reach for object_file_type = %d \n",object_file_type);
             }
        }

     asmFile->set_object_file_type(object_file_kind);

  // Now set the machine architecture type
     SgAsmFile::elf_machine_architecture_enum machine_architecture_kind = SgAsmFile::e_machine_architecture_error;
     int machine_architecture_type = 0;
     if (isA32bitElfHeader == true)
        {
          ROSE_ASSERT(isA64bitElfHeader == false);
          machine_architecture_type = elf_32_bit_header.e_machine;
        }
       else
        {
          ROSE_ASSERT(isA64bitElfHeader == true);
          machine_architecture_type = elf_64_bit_header.e_machine;
        }

     switch (machine_architecture_type)
        {
          case EM_NONE:        machine_architecture_kind = SgAsmFile::e_machine_architecture_none;                     break;
          case EM_M32:         machine_architecture_kind = SgAsmFile::e_machine_architecture_ATT_WE_32100;             break;
          case EM_SPARC:       machine_architecture_kind = SgAsmFile::e_machine_architecture_Sun_Sparc;                break;
          case EM_386:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Intel_80386;              break;
          case EM_68K:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_m68k_family;     break;
          case EM_88K:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_m88k_family;     break;
          case EM_860:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Intel_80860;              break;
          case EM_MIPS:        machine_architecture_kind = SgAsmFile::e_machine_architecture_MIPS_R3000_big_endian;    break;
          case EM_S370:        machine_architecture_kind = SgAsmFile::e_machine_architecture_IBM_System_370;           break;
          case EM_MIPS_RS3_LE: machine_architecture_kind = SgAsmFile::e_machine_architecture_MIPS_R3000_little_endian; break;
          case EM_PARISC:      machine_architecture_kind = SgAsmFile::e_machine_architecture_HPPA;               break;
          case EM_VPP500:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Fujitsu_VPP500;     break;
          case EM_SPARC32PLUS: machine_architecture_kind = SgAsmFile::e_machine_architecture_Sun_v8plus;         break;
          case EM_960:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Intel_80960;        break;
          case EM_PPC:         machine_architecture_kind = SgAsmFile::e_machine_architecture_PowerPC;            break;
          case EM_PPC64:       machine_architecture_kind = SgAsmFile::e_machine_architecture_PowerPC_64bit;      break;
          case EM_S390:        machine_architecture_kind = SgAsmFile::e_machine_architecture_IBM_S390;           break;
          case EM_V800:        machine_architecture_kind = SgAsmFile::e_machine_architecture_NEC_V800_series;    break;
          case EM_FR20:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Fujitsu_FR20;       break;
          case EM_RH32:        machine_architecture_kind = SgAsmFile::e_machine_architecture_TRW_RH_32;          break;
          case EM_RCE:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_RCE;       break;
          case EM_ARM:         machine_architecture_kind = SgAsmFile::e_machine_architecture_ARM;                break;
          case EM_FAKE_ALPHA:  machine_architecture_kind = SgAsmFile::e_machine_architecture_Digital_Alpha_fake; break;
          case EM_SH:          machine_architecture_kind = SgAsmFile::e_machine_architecture_Hitachi_SH;         break;
          case EM_SPARCV9:     machine_architecture_kind = SgAsmFile::e_machine_architecture_SPARC_v9_64bit;     break;
          case EM_TRICORE:     machine_architecture_kind = SgAsmFile::e_machine_architecture_Siemens_Tricore;    break;
          case EM_ARC:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Argonaut_RISC_Core; break;
          case EM_H8_300:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Hitachi_H8_300;     break;
          case EM_H8_300H:     machine_architecture_kind = SgAsmFile::e_machine_architecture_Hitachi_H8_300H;    break;
          case EM_H8S:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Hitachi_H8S;        break;
          case EM_H8_500:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Hitachi_H8_500;     break;
          case EM_IA_64:       machine_architecture_kind = SgAsmFile::e_machine_architecture_Intel_Merced;       break;
          case EM_MIPS_X:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Stanford_MIPS_X;    break;
          case EM_COLDFIRE:    machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_Coldfire;  break;
          case EM_68HC12:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_M68HC12;   break;
          case EM_MMA:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Fujitsu_MMA_Multimedia_Accelerator;   break;
          case EM_PCP:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Siemens_PCP;                          break;
          case EM_NCPU:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Sony_nCPU_embeeded_RISC;              break;
          case EM_NDR1:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Denso_NDR1_microprocessor;            break;
          case EM_STARCORE:    machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_Start_Core_processor;        break;
          case EM_ME16:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Toyota_ME16_processor;                break;
          case EM_ST100:       machine_architecture_kind = SgAsmFile::e_machine_architecture_STMicroelectronic_ST100_processor;    break;
          case EM_TINYJ:       machine_architecture_kind = SgAsmFile::e_machine_architecture_Advanced_Logic_Corp_Tinyj_emb_family; break;
          case EM_X86_64:      machine_architecture_kind = SgAsmFile::e_machine_architecture_AMD_x86_64_architecture;              break;
          case EM_PDSP:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Sony_DSP_Processor;                   break;
          case EM_FX66:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Siemens_FX66_microcontroller;         break;
          case EM_ST9PLUS:     machine_architecture_kind = SgAsmFile::e_machine_architecture_STMicroelectronics_ST9_plus_8_16_micocontroler; break;
          case EM_ST7:         machine_architecture_kind = SgAsmFile::e_machine_architecture_STMicroelectronics_ST7_8bit_micocontroler;      break;
          case EM_68HC16:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_MC68HC16_microcontroller;              break;
          case EM_68HC11:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_MC68HC11_microcontroller;              break;
          case EM_68HC08:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_MC68HC08_microcontroller;              break;
          case EM_68HC05:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Motorola_MC68HC05_microcontroller;              break;
          case EM_SVX:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Silicon_Graphics_SVx;                           break;
          case EM_ST19:        machine_architecture_kind = SgAsmFile::e_machine_architecture_STMicroelectronics_ST19_8bit_microcontroller;   break;
          case EM_VAX:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Digital_VAX;                                    break;
          case EM_CRIS:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Axis_Communications_32bit_embedded_processor;   break;
          case EM_JAVELIN:     machine_architecture_kind = SgAsmFile::e_machine_architecture_Infineon_Technologies_32bit_embedded_processor; break;
          case EM_FIREPATH:    machine_architecture_kind = SgAsmFile::e_machine_architecture_Element_14_64bit_DSP_Processor;                 break;
          case EM_ZSP:         machine_architecture_kind = SgAsmFile::e_machine_architecture_LSI_Logic_16bit_DSP_Processor;                  break;
          case EM_MMIX:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Donald_Knuths_educational_64bit_processor;      break;
          case EM_HUANY:       machine_architecture_kind = SgAsmFile::e_machine_architecture_Harvard_University_machine_independent_object_files; break;
          case EM_PRISM:       machine_architecture_kind = SgAsmFile::e_machine_architecture_SiTera_Prism;                      break;
          case EM_AVR:         machine_architecture_kind = SgAsmFile::e_machine_architecture_Atmel_AVR_8bit_microcontroller;    break;
          case EM_FR30:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Fujitsu_FR30;                      break;
          case EM_D10V:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Mitsubishi_D10V;                   break;
          case EM_D30V:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Mitsubishi_D30V;                   break;
          case EM_V850:        machine_architecture_kind = SgAsmFile::e_machine_architecture_NEC_v850;                          break;
          case EM_M32R:        machine_architecture_kind = SgAsmFile::e_machine_architecture_Mitsubishi_M32R;                   break;
          case EM_MN10300:     machine_architecture_kind = SgAsmFile::e_machine_architecture_Matsushita_MN10300;                break;
          case EM_MN10200:     machine_architecture_kind = SgAsmFile::e_machine_architecture_Matsushita_MN10200;                break;
          case EM_PJ:          machine_architecture_kind = SgAsmFile::e_machine_architecture_picoJava;                          break;
          case EM_OPENRISC:    machine_architecture_kind = SgAsmFile::e_machine_architecture_OpenRISC_32bit_embedded_processor; break;
          case EM_ARC_A5:      machine_architecture_kind = SgAsmFile::e_machine_architecture_ARC_Cores_Tangent_A5;              break;
          case EM_XTENSA:      machine_architecture_kind = SgAsmFile::e_machine_architecture_Tensilica_Xtensa_Architecture;     break;
          case EM_ALPHA:       machine_architecture_kind = SgAsmFile::e_machine_architecture_Digital_Alpha;                     break;

          default:
             {
               printf ("Error: default reach for machine_architecture_type = %d \n",machine_architecture_type);
             }
        }

     asmFile->set_machine_architecture(machine_architecture_kind);

     if ( SgProject::get_verbose() >= 1 )
          printf ("Machine arcitecture = %s \n",machineArchitectureName(machine_architecture_kind).c_str());

     if (isA32bitElfHeader == true)
        {
          ROSE_ASSERT(isA64bitElfHeader == false);

          asmFile->set_associated_entry_point(elf_32_bit_header.e_entry);
          asmFile->set_program_header_offset(elf_32_bit_header.e_phoff);
          asmFile->set_section_header_offset(elf_32_bit_header.e_shoff);
          asmFile->set_processor_specific_flags(elf_32_bit_header.e_flags);
          asmFile->set_elf_header_size(elf_32_bit_header.e_ehsize);
          asmFile->set_program_header_entry_size(elf_32_bit_header.e_phentsize);
          asmFile->set_number_of_program_headers(elf_32_bit_header.e_phnum);
          asmFile->set_section_header_entry_size(elf_32_bit_header.e_shentsize);
          asmFile->set_number_of_section_headers(elf_32_bit_header.e_shnum);
          asmFile->set_section_header_string_table_index(elf_32_bit_header.e_shstrndx);
        }
       else
        {
          ROSE_ASSERT(isA64bitElfHeader == true);

          asmFile->set_associated_entry_point(elf_64_bit_header.e_entry);
          asmFile->set_program_header_offset(elf_64_bit_header.e_phoff);
          asmFile->set_section_header_offset(elf_64_bit_header.e_shoff);
          asmFile->set_processor_specific_flags(elf_64_bit_header.e_flags);
          asmFile->set_elf_header_size(elf_64_bit_header.e_ehsize);
          asmFile->set_program_header_entry_size(elf_64_bit_header.e_phentsize);
          asmFile->set_number_of_program_headers(elf_64_bit_header.e_phnum);
          asmFile->set_section_header_entry_size(elf_64_bit_header.e_shentsize);
          asmFile->set_number_of_section_headers(elf_64_bit_header.e_shnum);
          asmFile->set_section_header_string_table_index(elf_64_bit_header.e_shstrndx);
        }

     if ( SgProject::get_verbose() >= 3 )
        {
          printf ("asmFile->get_associated_entry_point()            = %lu \n",asmFile->get_associated_entry_point());
          printf ("asmFile->get_program_header_offset()             = %lu \n",asmFile->get_program_header_offset());
          printf ("asmFile->get_section_header_offset()             = %lu \n",asmFile->get_section_header_offset());
          printf ("asmFile->get_processor_specific_flags()          = %lu \n",asmFile->get_processor_specific_flags());
          printf ("asmFile->get_elf_header_size()                   = %lu \n",asmFile->get_elf_header_size());
          printf ("asmFile->get_program_header_entry_size()         = %lu \n",asmFile->get_program_header_entry_size());
          printf ("asmFile->get_number_of_program_headers()         = %lu \n",asmFile->get_number_of_program_headers());
          printf ("asmFile->get_section_header_entry_size()         = %lu \n",asmFile->get_section_header_entry_size());
          printf ("asmFile->get_number_of_section_headers()         = %lu \n",asmFile->get_number_of_section_headers());
          printf ("asmFile->get_section_header_string_table_index() = %lu \n",asmFile->get_section_header_string_table_index());
        }

  // Now get the program headers

  // Can we move the file pointer more directly than this???
     unsigned long programHeaderOffset = asmFile->get_program_header_offset();
     fseek(f, programHeaderOffset, SEEK_SET);

     Elf32_Phdr elf_32_bit_program_header;
     Elf64_Phdr elf_64_bit_program_header;

     int elfProgramHeaderSize_32bit = sizeof(Elf32_Phdr);
     int elfProgramHeaderSize_64bit = sizeof(Elf64_Phdr);

     SgAsmProgramHeaderList* programHeaderList = new SgAsmProgramHeaderList();

     asmFile->set_programHeaderList(programHeaderList);
     programHeaderList->set_parent(asmFile);

     if ( SgProject::get_verbose() >= 3 )
          printf ("elfProgramHeaderSize_32bit = %d elfProgramHeaderSize_64bit = %d \n",elfProgramHeaderSize_32bit,elfProgramHeaderSize_64bit);
     ROSE_ASSERT(elfProgramHeaderSize_32bit <= elfProgramHeaderSize_64bit);

     int number_of_program_headers = asmFile->get_number_of_program_headers();
     for (int i = 0; i < number_of_program_headers; i++)
        {
          SgAsmProgramHeader* programHeader = new SgAsmProgramHeader();
          ROSE_ASSERT(programHeader != NULL);

          programHeaderList->get_program_headers().push_back(programHeader);
          programHeader->set_parent(programHeaderList);

       // If we used a 32 bit Elf header then the program header (and section header) must match, I think.
          if (isA32bitElfHeader == true)
             {
               ROSE_ASSERT(isA64bitElfHeader == false);

               elementsRead = fread(&elf_32_bit_program_header,1,elfProgramHeaderSize_32bit,f);
               if (elementsRead != elfProgramHeaderSize_32bit)
                  {
                    printf ("Error in reading to the end of the elf_32_bit_program_header (read only %d bytes, elf header is %d bytes) \n",elementsRead,elfProgramHeaderSize_32bit);
                    ROSE_ASSERT(false);
                  }

               programHeader->set_type(elf_32_bit_program_header.p_type);
               programHeader->set_starting_file_offset(elf_32_bit_program_header.p_offset);
               programHeader->set_starting_virtual_memory_address(elf_32_bit_program_header.p_vaddr);
               programHeader->set_starting_physical_memory_address(elf_32_bit_program_header.p_paddr);
               programHeader->set_file_image_size(elf_32_bit_program_header.p_filesz);
               programHeader->set_memory_image_size(elf_32_bit_program_header.p_memsz);
               programHeader->set_segment_flags(elf_32_bit_program_header.p_flags);
               programHeader->set_alignment(elf_32_bit_program_header.p_align);
             }
            else
             {
               ROSE_ASSERT(isA64bitElfHeader == true);

               elementsRead = fread(&elf_64_bit_program_header,1,elfProgramHeaderSize_64bit,f);
               if (elementsRead != elfProgramHeaderSize_64bit)
                  {
                    printf ("Error in reading to the end of the elf_64_bit_program_header (read only %d bytes, elf header is %d bytes) \n",elementsRead,elfProgramHeaderSize_32bit);
                    ROSE_ASSERT(false);
                  }

               programHeader->set_type(elf_64_bit_program_header.p_type);
               programHeader->set_starting_file_offset(elf_64_bit_program_header.p_offset);
               programHeader->set_starting_virtual_memory_address(elf_64_bit_program_header.p_vaddr);
               programHeader->set_starting_physical_memory_address(elf_64_bit_program_header.p_paddr);
               programHeader->set_file_image_size(elf_64_bit_program_header.p_filesz);
               programHeader->set_memory_image_size(elf_64_bit_program_header.p_memsz);
               programHeader->set_segment_flags(elf_64_bit_program_header.p_flags);
               programHeader->set_alignment(elf_64_bit_program_header.p_align);
             }

          if ( SgProject::get_verbose() >= 3 )
             {
               printf ("programHeader->get_type() = %lu \n",programHeader->get_type());
               printf ("programHeader->get_starting_file_offset() = %lu \n",programHeader->get_starting_file_offset());
               printf ("programHeader->get_starting_virtual_memory_address() = %lu \n",programHeader->get_starting_virtual_memory_address());
               printf ("programHeader->get_starting_physical_memory_address() = %lu \n",programHeader->get_starting_physical_memory_address());
               printf ("programHeader->get_file_image_size() = %lu \n",programHeader->get_file_image_size());
               printf ("programHeader->get_memory_image_size() = %lu \n",programHeader->get_memory_image_size());
               printf ("programHeader->get_segment_flags() = %lu \n",programHeader->get_segment_flags());
               printf ("programHeader->get_alignment() = %lu \n",programHeader->get_alignment());
             }
        }

  // Now get the segment headers

  // Can we move the file pointer more directly than this???
     unsigned long segmentHeaderOffset = asmFile->get_section_header_offset();
     fseek(f, segmentHeaderOffset, SEEK_SET);

     Elf32_Shdr elf_32_bit_section_header;
     Elf64_Shdr elf_64_bit_section_header;

     int elfSectionHeaderSize_32bit = sizeof(Elf32_Shdr);
     int elfSectionHeaderSize_64bit = sizeof(Elf64_Shdr);

     if ( SgProject::get_verbose() >= 1 )
          printf ("elfSectionHeaderSize_32bit = %d elfSectionHeaderSize_64bit = %d \n",elfSectionHeaderSize_32bit,elfSectionHeaderSize_64bit);
     ROSE_ASSERT(elfSectionHeaderSize_32bit <= elfSectionHeaderSize_64bit);

     SgAsmSectionHeaderList* sectionHeaderList = new SgAsmSectionHeaderList();

     asmFile->set_sectionHeaderList(sectionHeaderList);
     sectionHeaderList->set_parent(asmFile);

     int number_of_section_headers = asmFile->get_number_of_section_headers();
     for (int i = 0; i < number_of_section_headers; i++)
        {
          SgAsmSectionHeader* sectionHeader = new SgAsmSectionHeader();
          ROSE_ASSERT(sectionHeader != NULL);

          sectionHeaderList->get_section_headers().push_back(sectionHeader);
          sectionHeader->set_parent(sectionHeaderList);

       // If we used a 32 bit Elf header then the program header (and section header) must match, I think.
          if (isA32bitElfHeader == true)
             {
               ROSE_ASSERT(isA64bitElfHeader == false);

               elementsRead = fread(&elf_32_bit_section_header,1,elfSectionHeaderSize_32bit,f);
               if (elementsRead != elfSectionHeaderSize_32bit)
                  {
                    printf ("Error in reading to the end of the elf_32_bit_section_header (read only %d bytes, elf header is %d bytes) \n",elementsRead,elfSectionHeaderSize_32bit);
                    ROSE_ASSERT(false);
                  }

               string name = "not looked-up yet (elf_32_bit_section_header)";
               sectionHeader->set_name(name);

               sectionHeader->set_name_string_index(elf_32_bit_section_header.sh_name);
               sectionHeader->set_type(elf_32_bit_section_header.sh_type);
               sectionHeader->set_flags(elf_32_bit_section_header.sh_flags);
               sectionHeader->set_starting_memory_address(elf_32_bit_section_header.sh_addr);
               sectionHeader->set_starting_file_offset(elf_32_bit_section_header.sh_offset);
               sectionHeader->set_size(elf_32_bit_section_header.sh_size);
               sectionHeader->set_table_index_link(elf_32_bit_section_header.sh_link);
               sectionHeader->set_info(elf_32_bit_section_header.sh_info);
               sectionHeader->set_address_alignment(elf_32_bit_section_header.sh_addralign);
               sectionHeader->set_table_entry_size(elf_32_bit_section_header.sh_entsize);
             }
            else
             {
               ROSE_ASSERT(isA64bitElfHeader == true);

               elementsRead = fread(&elf_64_bit_section_header,1,elfSectionHeaderSize_64bit,f);
               if (elementsRead != elfSectionHeaderSize_64bit)
                  {
                    printf ("Error in reading to the end of the elf_64_bit_section_header (read only %d bytes, elf header is %d bytes) \n",elementsRead,elfSectionHeaderSize_32bit);
                    ROSE_ASSERT(false);
                  }

               string name = "not looked-up yet (elf_64_bit_section_header)";
               sectionHeader->set_name(name);

               sectionHeader->set_type(elf_64_bit_section_header.sh_type);
               sectionHeader->set_flags(elf_64_bit_section_header.sh_flags);
               sectionHeader->set_starting_memory_address(elf_64_bit_section_header.sh_addr);
               sectionHeader->set_starting_file_offset(elf_64_bit_section_header.sh_offset);
               sectionHeader->set_size(elf_64_bit_section_header.sh_size);
               sectionHeader->set_table_index_link(elf_64_bit_section_header.sh_link);
               sectionHeader->set_info(elf_64_bit_section_header.sh_info);
               sectionHeader->set_address_alignment(elf_64_bit_section_header.sh_addralign);
               sectionHeader->set_table_entry_size(elf_64_bit_section_header.sh_entsize);
             }

          if ( SgProject::get_verbose() >= 3 )
             {
               printf ("sectionHeader->get_name()                    = %s \n",sectionHeader->get_name().c_str());
               printf ("sectionHeader->get_name_string_index()       = %lu \n",sectionHeader->get_name_string_index());
               printf ("sectionHeader->get_type()                    = %lu \n",sectionHeader->get_type());
               printf ("sectionHeader->get_flags()                   = %lu \n",sectionHeader->get_flags());
               printf ("sectionHeader->get_starting_memory_address() = %lu \n",sectionHeader->get_starting_memory_address());
               printf ("sectionHeader->get_starting_file_offset()    = %lu \n",sectionHeader->get_starting_file_offset());
               printf ("sectionHeader->get_size()                    = %lu \n",sectionHeader->get_size());
               printf ("sectionHeader->get_table_index_link()        = %lu \n",sectionHeader->get_table_index_link());
               printf ("sectionHeader->get_info()                    = %lu \n",sectionHeader->get_info());
               printf ("sectionHeader->get_address_alignment()       = %lu \n",sectionHeader->get_address_alignment());
               printf ("sectionHeader->get_table_entry_size()        = %lu \n",sectionHeader->get_table_entry_size());
             }
        }

     fclose(f);

  // printf ("Exiting as a test in generateBinaryExecutableFileInformation() \n");
  // ROSE_ASSERT(false);
   }

 
void
generateBinaryExecutableFileInformation ( string sourceFilename, SgAsmFile* asmFile )
   {
  // Need a mechanism to select what kind of binary we will process.
     generateBinaryExecutableFileInformation_ELF     ( sourceFilename, asmFile );

  // generateBinaryExecutableFileInformation_Windows ( sourceFilename, asmFile );
   }

#endif

void
SgFile::setupSourceFilename ( const vector<string>& argv )
   {
  // DQ (4/21/2006): New version of source file name handling (set the source file name early)
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(argv);

  // this->display("In SgFile::setupSourceFilename()");
     // printf ("listToString(argv) = %s \n",StringUtility::listToString(argv).c_str());
     // printf ("listToString(fileList) = %s \n",StringUtility::listToString(fileList).c_str());

     if (fileList.empty() == false)
        {
          ROSE_ASSERT(fileList.size() == 1);

       // DQ (8/31/2006): Convert the source file to have a path if it does not already
       // p_sourceFileNameWithPath    = *(fileList.begin());
          string sourceFilename = *(fileList.begin());

       // printf ("Before conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());

       // sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename);
          sourceFilename = StringUtility::getAbsolutePathFromRelativePath(sourceFilename, true);

       // printf ("After conversion to absolute path: sourceFilename = %s \n",sourceFilename.c_str());

          p_sourceFileNameWithPath = sourceFilename;

       // printf ("In SgFile::setupSourceFilename(const vector<string>& argv): p_sourceFileNameWithPath = %s \n",p_sourceFileNameWithPath.c_str());

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
       // is we detect we are processing a source file using the a C++ filename extension.
          string filenameExtension = StringUtility::fileNameSuffix(sourceFilename);

       // DQ (11/17/2007): Mark this as a file using a Fortran file extension (else this turns off options down stream).
          if (CommandlineProcessing::isFortranFileNameSuffix(filenameExtension) == true)
             {
            // printf ("Calling set_sourceFileUsesFortranFileExtension(true) \n");
               set_sourceFileUsesFortranFileExtension(true);

            // Use the filename suffix as a default means to set this value
               set_outputLanguage(SgFile::e_Fortran_output_language);

               set_Fortran_only(true);

            // Now set the specific types of Fortran file extensions
               if (CommandlineProcessing::isFortran77FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling set_sourceFileUsesFortran77FileExtension(true) \n");
                    set_sourceFileUsesFortran77FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    set_outputFormat(SgFile::e_fixed_form_output_format);
                    set_backendCompileFormat(SgFile::e_fixed_form_output_format);

                    set_F77_only(true);
                  }

               if (CommandlineProcessing::isFortran90FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling set_sourceFileUsesFortran90FileExtension(true) \n");
                    set_sourceFileUsesFortran90FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    set_outputFormat(SgFile::e_free_form_output_format);
                    set_backendCompileFormat(SgFile::e_free_form_output_format);

                    set_F90_only(true);
                  }

               if (CommandlineProcessing::isFortran95FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling set_sourceFileUsesFortran95FileExtension(true) \n");
                    set_sourceFileUsesFortran95FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    set_outputFormat(SgFile::e_free_form_output_format);
                    set_backendCompileFormat(SgFile::e_free_form_output_format);

                    set_F95_only(true);
                  }

               if (CommandlineProcessing::isFortran2003FileNameSuffix(filenameExtension) == true)
                  {
                 // printf ("Calling set_sourceFileUsesFortran2003FileExtension(true) \n");
                    set_sourceFileUsesFortran2003FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    set_outputFormat(SgFile::e_free_form_output_format);
                    set_backendCompileFormat(SgFile::e_free_form_output_format);

                    set_F2003_only(true);
                  }

               if (CommandlineProcessing::isFortran2008FileNameSuffix(filenameExtension) == true)
                  {
                    printf ("Sorry, Fortran 2008 specific support is not yet implemented in ROSE ... \n");
                    ROSE_ASSERT(false);

                 // This is not yet supported.
                 // set_sourceFileUsesFortran2008FileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    set_outputFormat(SgFile::e_free_form_output_format);
                    set_backendCompileFormat(SgFile::e_free_form_output_format);
                  }
             }
            else
             {
            // printf ("Calling set_sourceFileUsesFortranFileExtension(false) \n");
               set_sourceFileUsesFortranFileExtension(false);

            // if (StringUtility::isCppFileNameSuffix(filenameExtension) == true)
               if (CommandlineProcessing::isCppFileNameSuffix(filenameExtension) == true)
                  {
                 // This is a C++ file (so define __cplusplus, just like GNU gcc would)
                 // set_requires_cplusplus_macro(true);
                    set_sourceFileUsesCppFileExtension(true);

                 // Use the filename suffix as a default means to set this value
                    set_outputLanguage(SgFile::e_Cxx_output_language);

                    set_Cxx_only(true);
                  }
                 else
                  {
                    if (CommandlineProcessing::isCFileNameSuffix(filenameExtension) == true)
                       {
                      // This a not a C++ file (assume it is a C file and don't define the __cplusplus macro, just like GNU gcc would)
                         set_sourceFileUsesCppFileExtension(false);

                      // Use the filename suffix as a default means to set this value
                         set_outputLanguage(SgFile::e_C_output_language);

                         set_C_only(true);
                       }
                      else
                       {
                      // printf ("This still might be a binary file (can not be an object file, since these are not accepted into the fileList by CommandlineProcessing::generateSourceFilenames()) \n");

#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
                      // Detect if this is a binary (executable) file!
                         bool isBinaryExecutable = isBinaryExecutableFile(sourceFilename);
#else
                         bool isBinaryExecutable = false;
#endif

                         if (isBinaryExecutable == true)
                            {
                              set_sourceFileUsesBinaryFileExtension(true);
                              set_binary_only(true);
                            }
                       }
                  }

             }

          p_sourceFileNameWithoutPath = ROSE::stripPathFromFileName(p_sourceFileNameWithPath.c_str());
        }
       else
        {
       // ROSE_ASSERT (p_numberOfSourceFileNames == 0);
          ROSE_ASSERT (p_sourceFileNameWithPath.empty() == true);
       // If no source code file name was found then likely this is a link command
       // using the C++ compiler.  In this case skip the EDG processing.
          set_disable_edg_backend(true);
       // printf ("No source file found on command line, assuming to be linker command line \n");
        }

  // Keep the filename stored in the Sg_File_Info consistant.  Later we will want to remove this redundency
  // The reason we have the Sg_File_Info object is so that we can easily support filename matching based on
  // the integer values instead of string comparisions.
     get_file_info()->set_filenameString(p_sourceFileNameWithPath);

  // display("SgFile::setupSourceFilename()");
   }

static string makeSysIncludeList(const Rose_STL_Container<string>& dirs,
                                 bool useInstalledIncludedir) {
  string includeBase = useInstalledIncludedir ?
                       ROSE_AUTOMAKE_INCLUDEDIR :
                       (ROSE_AUTOMAKE_TOP_BUILDDIR "/include-staging");
  string result;
  for (Rose_STL_Container<string>::const_iterator i = dirs.begin();
       i != dirs.end(); ++i) {
    ROSE_ASSERT (!i->empty());
    string fullPath = (*i)[0] == '/' ? *i : (includeBase + "/" + *i);
    result += "--sys_include " + fullPath + " ";
  }
  return result;
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

  // BP: (11/26/2001) trying out a new method of figuring out internal compiler definitions
#if defined(CXX_SPEC_DEF)

#if 1
     string configDefsString                = CXX_SPEC_DEF;
     const char* Cxx_ConfigIncludeDirsRaw[] = CXX_INCLUDE_STRING;
     const char* C_ConfigIncludeDirsRaw[]   = C_INCLUDE_STRING;
     Rose_STL_Container<string> Cxx_ConfigIncludeDirs(Cxx_ConfigIncludeDirsRaw, Cxx_ConfigIncludeDirsRaw + sizeof(Cxx_ConfigIncludeDirsRaw) / sizeof(const char*));
     Rose_STL_Container<string> C_ConfigIncludeDirs(C_ConfigIncludeDirsRaw, C_ConfigIncludeDirsRaw + sizeof(C_ConfigIncludeDirsRaw) / sizeof(const char*));
     SgProject* myProject = isSgProject(this->get_parent());
     ROSE_ASSERT (myProject);

  // Removed reference to __restrict__ so it could be placed into the preinclude vendor specific header file for ROSE.
  // DQ (9/10/2004): Attept to add support for restrict (but I think this just sets it to true, using "-Dxxx=" works)
  // const string roseSpecificDefs    = "-DUSE_RESTRICT_POINTERS_IN_ROSE_TRANSFORMATIONS -DUSE_ROSE -D__restrict__=";
     const string roseSpecificDefs    = "-DUSE_RESTRICT_POINTERS_IN_ROSE_TRANSFORMATIONS -DUSE_ROSE";

     ROSE_ASSERT(configDefsString.empty() == false);
     ROSE_ASSERT(Cxx_ConfigIncludeDirs.empty() == false);
     ROSE_ASSERT(C_ConfigIncludeDirs.empty() == false);

  // printf ("configDefsString = %s \n",configDefsString);
#if 0
     printf ("Cxx_ConfigIncludeString = %s \n",Cxx_ConfigIncludeString.c_str());
     printf ("C_ConfigIncludeString   = %s \n",C_ConfigIncludeString.c_str());
#endif

  // AS (03/08/2006) Added support for g++ preincludes
  // Rose_STL_Container<std::string> listOfPreincludes;

  // DQ (12/1/2006): Code added by Andreas (07/03/06) and moved to a new position 
  // so that we could modify the string input from CXX_SPEC_DEF (configDefsString).
     string preinclude_string_target = "-include";
     string preinclude_string;
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

               preinclude_string += " --preinclude " + currentArgument + " ";
             }
        }

     configDefsString += preinclude_string;

     string initString;

  // This is OK since these use the existing memory and not the C language 
  // malloc/free (which would interfere with C++'s new/delete)
     initString = configDefsString;
     initString += " ";
  // initString += Cxx_ConfigIncludeString;

  // DQ (12/2/2006): Both GNU and EDG determin the language mode from the source file name extension. 
  // In ROSE we also require that C files be explicitly specified to use the C language mode. Thus 
  // C++ source files will be treated as C++ even if the C language rules are specified, however they 
  // are restricted to the C subset of C++.
  // bool treatAsCSourceFile = ((get_C_only() == true || get_C99_only() == true) && (get_sourceFileUsesCppFileExtension() == false));
  // if (treatAsCSourceFile == true)

  // Find the C++ sys include path for the rose_edg_required_macros_and_functions.h
     string roseHeaderDirCPP = " --sys_include ";

     for (Rose_STL_Container<string>::iterator i = Cxx_ConfigIncludeDirs.begin(); i != Cxx_ConfigIncludeDirs.end(); i++)
        {
          string file = (*i) + "/rose_edg_required_macros_and_functions.h";
          FILE* testIfFileExist = fopen(file.c_str(),"r");
          if (testIfFileExist)
          {
            roseHeaderDirCPP+=(*i);
            fclose(testIfFileExist);
            break;
          }
        }

  // Find the C sys include path for the rose_edg_required_macros_and_functions.h
     string roseHeaderDirC = " --sys_include ";

     for (Rose_STL_Container<string>::iterator i = C_ConfigIncludeDirs.begin(); i != C_ConfigIncludeDirs.end(); i++)
        {
          string file = (*i) + "/rose_edg_required_macros_and_functions.h";
          FILE* testIfFileExist = fopen(file.c_str(),"r");
          // std::cout << file << std::endl;
          if (testIfFileExist)
          {
            roseHeaderDirC+=(*i);
            fclose(testIfFileExist);
            break;
          }
        }


     if (get_C_only() == true || get_C99_only() == true)
        {
       // AS(02/21/07) Add support for the gcc 'nostdinc' and 'nostdinc++' options
          string roseHeaderDir = " --sys_include ";

       // DQ (11/29/2006): if required turn on the use of the __cplusplus macro
       // if (get_requires_cplusplus_macro() == true)
          if (get_sourceFileUsesCppFileExtension() == true)
             {
            // The value here should be 1 to match that of GNU gcc (the C++ standard requires this to be "199711L")
            // initString += " -D__cplusplus=0 ";
               initString += " -D__cplusplus=1 ";
               if ( CommandlineProcessing::isOption(argv,"-","nostdinc",false) == true )
                  {
                    initString+=roseHeaderDirC; 
                 // no standard includes when -nostdinc is specified
                  }
                 else
                  {
                    if ( CommandlineProcessing::isOption(argv,"-","nostdinc++",false) == true )
                       {
                         initString += roseHeaderDirCPP;
                         initString += makeSysIncludeList(C_ConfigIncludeDirs, myProject->get_runningFromInstalledRose());
                       }
                      else
                       {
                         initString += makeSysIncludeList(Cxx_ConfigIncludeDirs, myProject->get_runningFromInstalledRose());
                       }
                  }

            // DQ (11/29/2006): Specify C++ mode for handling in rose_edg_required_macros_and_functions.h
               initString += " -DROSE_CPP_MODE=1 ";
             }
            else
             {
               if ( CommandlineProcessing::isOption(argv,"-","nostdinc",false) == true )
                  {
                    initString += roseHeaderDirC; 
                 // no standard includes when -nostdinc is specified
                  }
                 else
                  {
                    initString += makeSysIncludeList(C_ConfigIncludeDirs, myProject->get_runningFromInstalledRose());
                  }
       
            // DQ (11/29/2006): Specify C mode for handling in rose_edg_required_macros_and_functions.h
               initString += " -DROSE_CPP_MODE=0 ";
             }
        }
       else
        {
          if ( CommandlineProcessing::isOption(argv,"-","nostdinc",false) == true )
             {
               initString += roseHeaderDirCPP;
            // no standard includes when -nostdinc is specified
             }
            else
             {
               if ( CommandlineProcessing::isOption(argv,"-","nostdinc++",false) == true )
                  {
                    initString += roseHeaderDirCPP;
                    initString += makeSysIncludeList(C_ConfigIncludeDirs, myProject->get_runningFromInstalledRose());
                  }
                 else
                  {
                    initString += makeSysIncludeList(Cxx_ConfigIncludeDirs, myProject->get_runningFromInstalledRose());
                  }
             }

       // DQ (11/29/2006): Specify C++ mode for handling in rose_edg_required_macros_and_functions.h
          initString += " -DROSE_CPP_MODE=1 ";
        }

  // AS(02/24/06) Add support for the gcc "-isystem" option (this added a specificed directory 
  // to the start of the system include path).  This maps to the "--sys_include" in EDG.
     string isystem_string_target = "-isystem";
     string isystem_string;
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
              isystem_string += " --sys_include " + currentArgument + " ";
          }
     }

     initString += isystem_string;


     initString += " ";
     initString += roseSpecificDefs;

  // DQ (9/17/2006): We should be able to build a version of this code which hands a std::string to StringUtility::splitStringIntoStrings()
  // Separate the string into substrings consistant with the structure of argv command line input
     inputCommandLine.clear();
     StringUtility::splitStringIntoStrings(initString, ' ', inputCommandLine);
#endif

#else
  // Output an error and exit
     cout << "ERROR: The preprocessor definition CXX_SPEC_DEF should have been defined by the configure process!!" << endl;
     ROSE_ABORT();
#endif

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
               includeDirectorySpecifier = "-I"+StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
               includePaths.push_back(includeDirectorySpecifier);
             }
        }

#if 0
     for (int i=0; i < includePathCounter; i++)
        {
          printf ("     includePaths[%d] = %s \n",i,includePaths[i]);
        }
#endif

  // Add the -I definitions to the command line
     inputCommandLine.insert(inputCommandLine.end(), includePaths.begin(), includePaths.end());

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
  // Some EDG options have to turn on mechanims in ROSE
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
#if 0
     if ( CommandlineProcessing::isOption(argc,argv,"-","c",false) == true )
        {
          printf ("In build_EDG_CommandLine(): Option -c found (compile only)! \n");
          set_compileOnly(true);
        }
       else
        {
          printf ("In build_EDG_CommandLine(): Option -c not found (compile AND link) adding --auto_instantiation ... \n");
          Rose_STL_Container<string> additionalOptions_a;
          Rose_STL_Container<string> additionalOptions_b;
       // Even though this is not an error to EDG, it does not appear to force instantiation of all templates
          additionalOptions_a.push_back("auto_instantiation");
          CommandlineProcessing::addListToCommandLine(numberOfCommandLineArguments,inputCommandLine,"--",additionalOptions_a);

       // DQ (5/12/05): Set the instantiation mode to "used" for specify what sort of templates to instantiate automatically
       // (generate "-tused" instead of "--instantiate used" since EDG does not seems to accept options containing white space).
          additionalOptions_b.push_back("tused");
          CommandlineProcessing::addListToCommandLine(numberOfCommandLineArguments,inputCommandLine, "-",additionalOptions_b);
        }
#else
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
#if 1
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
#else
  // DQ (5/20/05): Set the instantiation mode to "used" for specify what sort of templates to instantiate automatically
  // (generate "-tused" instead of "--instantiate used" since EDG does not seems to accept options containing white space).
     printf ("In build_EDG_CommandLine(): autoInstantiation = true adding --auto_instantiation -tused ... \n");
     
  // Even though this is not an error to EDG, it does not appear to force instantiation of all templates (because we need "-tused")
     additionalOptions_a.push_back("auto_instantiation");
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"--",additionalOptions_a);

     additionalOptions_b.push_back("tused");
     CommandlineProcessing::addListToCommandLine(inputCommandLine, "-",additionalOptions_b);
#endif

#endif

  // printf ("###### Located source filename = %s \n",get_sourceFileNameWithPath().c_str());
  // ROSE_ASSERT ( get_numberOfSourceFileNames() > 0 );
     ROSE_ASSERT ( get_sourceFileNameWithPath().empty() == false);
     //AS Added support for absolute paths
     Rose_STL_Container<string> fileList;
     std::string sourceFile = get_sourceFileNameWithPath();
     std::string sourceFilePath = StringUtility::getPathFromFileName(sourceFile);
     sourceFile = StringUtility::stripPathFromFileName(sourceFile);
     if(sourceFilePath == "" )
        sourceFilePath = "./";
     sourceFilePath = StringUtility::getAbsolutePathFromRelativePath(sourceFilePath);
     fileList.push_back(sourceFilePath+"/"+sourceFile);
     
     CommandlineProcessing::addListToCommandLine(inputCommandLine,"",fileList);

  // AS Add Support for g++ style preincludes
  // CommandlineProcessing::addListToCommandLine(inputCommandLine,"--preinclude ",listOfPreincludes);

  // ROSE_ASSERT (saveSkipfinalCompileStep == p_skipfinalCompileStep);

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

//! internal function to invoke the EDG frontend and generate the AST
int
SgProject::parse(const vector<string>& argv)
   {
  // Not sure that if we are just linking that we should call a function called "parse()"!!!

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST (SgProject::parse(argc,argv)):");

     // printf ("Inside of SgProject::parse(const vector<string>& argv) \n");

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
                    errorCode = parse();
                  }

            // DQ (5/26/2007): This is meaningless, so remove it!
            // errorCode = errorCode;
             }
        }

     return errorCode;
   }

int 
SgProject::parse()
   {
     int errorCode = 0;

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST (SgProject::parse()):");

     ROSE_ASSERT (p_fileList != NULL);

  // Simplify multi-file handling so that a single file is just the trivial 
  // case and not a special separate case.

     // printf ("Loop through the source files on the command line! p_sourceFileNameList = %zu \n",p_sourceFileNameList.size());

     Rose_STL_Container<string>::iterator nameIterator = p_sourceFileNameList.begin();
     unsigned int i = 0;
     while (nameIterator != p_sourceFileNameList.end())
        {
       // printf ("Build a SgFile object for file #%d \n",i);
          int nextErrorCode = 0;

       // DQ (4/20/2006): Exclude other files from list in argc and argv
          vector<string> argv = get_originalCommandLineArgumentList();
          string currentFileName = *nameIterator;
          CommandlineProcessing::removeAllFileNamesExcept(argv,p_sourceFileNameList,currentFileName);

       // printf ("Command line being passed to SgFile constructor: \n%s\n",CommandlineProcessing::generateStringFromArgcArgv(argc,(const char**)argv,false,false).c_str());

       // DQ (2/24/2004): Added support for SgProject to be passed so that the parent 
       // pointer could be set earily in the construction process.
       // SgFile* newFile = new SgFile ( argc, argv, nextErrorCode, i, this );
          SgFile* newFile = new SgFile ( argv, nextErrorCode, 0, this );
          ROSE_ASSERT (newFile != NULL);

          ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);

       // Set the parent explicitly (so that we can easily find the SgProject from the SgFile).
          newFile->set_parent(this);

       // This just adds the new file to the list of files stored internally
          set_file ( *newFile );

#if 0
          printf ("In Project::parse(): get_file(%d).get_skipfinalCompileStep() = %s \n",
               i,(get_file(i).get_skipfinalCompileStep()) ? "true" : "false");
#endif

       // errorCode = (errorCode >= nextErrorCode) ? errorCode : nextErrorCode; // use STL max
          errorCode = max(errorCode,nextErrorCode); // use STL max

          nameIterator++;
          i++;
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
SgFile::doSetupForConstructor(const vector<string>& argv, int& errorCode, int fileNameIndex, SgProject* project)
   {
  // JJW 10-26-2007 ensure that this object is not on the stack
     preventConstructionOnStack(this);

     // printf ("Inside of SgFile::doSetupForConstructor() \n");

  // DQ (4/21/2006): I think we can now assert this! This is an unused function parameter!
     ROSE_ASSERT(fileNameIndex == 0);

  // DQ (5/9/2007): Moved this call down to after where the file name is available so that we could include the filename in the label.
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
  // TimingPerformance timer ("AST SgFile Constructor:");

  // Set the project early in the construction phase so that we can access data in 
  // the parent if needed (useful for template handling but also makes sure the parent is
  // set (and avoids fixup (currently done, but too late in the construction process for 
  // the template support).
     if (project != NULL)
          set_parent(project);

     ROSE_ASSERT (project);

  // DQ (5/9/2007): The initialization() should do this, so this should not be required.
     p_root = NULL;
     p_binaryFile = NULL;

  // initalize all local variables to default values
     initialization();

  // DQ (4/21/2006): Setup the source filename as early as possible
     setupSourceFilename(argv);

  // DQ (1/21/2008): Set the filename in the SgGlobal IR node so that the traversal to add CPP directives and comments will succeed.
     ROSE_ASSERT (p_root != NULL);
     ROSE_ASSERT(p_root->get_startOfConstruct() != NULL);
     ROSE_ASSERT(p_root->get_endOfConstruct() == NULL);
  // p_root->get_file_info()->set_filenameString(p_sourceFileNameWithPath);
  // ROSE_ASSERT(p_root->get_file_info()->get_filenameString().empty() == false);
     p_root->get_startOfConstruct()->set_filenameString(p_sourceFileNameWithPath);
  // p_root->get_endOfConstruct()->set_filenameString(p_sourceFileNameWithPath);
     ROSE_ASSERT(p_root->get_startOfConstruct()->get_filenameString().empty() == false);
  // ROSE_ASSERT(p_root->get_endOfConstruct()->get_filenameString().empty() == false);

  // printf ("Found a Sg_File_Info using filename == NULL_FILE: fileInfo = %p SgGlobal (p_root = %p) p_root->get_file_info()->get_filenameString() = %s \n",p_root->get_file_info(),p_root,p_root->get_file_info()->get_filenameString().c_str());

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
  // p_fileNameIndex = fileNameIndex;

#if 0
     printf ("In SgFile::SgFile() after initialization: get_skipfinalCompileStep() = %s \n",
          get_skipfinalCompileStep() ? "true" : "false");
#endif

  // printf ("In SgFile constructor: fileNameIndex = %d \n",fileNameIndex);

  // Store the command line for later use with compiling the unparsed code
  // set_NumberOfCommandLineArguments ( argc );
  // set_CommandLineArgumentList ( argv );

     ROSE_ASSERT (p_root != NULL);

  // error checking
     ROSE_ASSERT (argv.size() > 1);

  // printf ("DONE with copy of command line in SgFile constructor! \n");

#if 0
     printf ("In SgFile::SgFile() before callFrontEnd(): get_skipfinalCompileStep() = %s \n",
          get_skipfinalCompileStep() ? "true" : "false");
#endif

#if 1
  // DQ (10/16/2005): Modified to make clear that argc and argv are not modified
  // Call the EDG fron-end to generate the abstract syntax tree
  // int EDG_FrontEndErrorCode = callFrontEnd ( argc, argv, *this, fileNameIndex );
  // int EDG_FrontEndErrorCode = callFrontEnd (  local_commandLineArgumentList, *this, fileNameIndex );
  // int EDG_FrontEndErrorCode = callFrontEnd (  local_commandLineArgumentList );
     int EDG_FrontEndErrorCode = callFrontEnd();

  // Warning from EDG processing are OK but not errors
     ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);

     errorCode = EDG_FrontEndErrorCode;

  // cout << "EDG/SAGE/ROSE Processing DONE! " << endl;
#endif

#if 0
     printf ("In SgFile::SgFile() after callFrontEnd(): get_skipfinalCompileStep() = %s \n",
          get_skipfinalCompileStep() ? "true" : "false");
#endif

  // DQ (5/24/2005): Fixup the file info information at the SgFile and the global scope (SgGlobal)
  // Opps, SgFile object does not have a Sg_File_Info pointer data member.
  // Sg_File_Info* localfileInfo = new Sg_File_Info(p_sourceFileNamesWithPath[fileNameIndex],0,0);
  // ROSE_ASSERT (localfileInfo != NULL);
  // set_file_info(localfileInfo);

  // DQ (10/15/2005): This has not been converted to a C++ string!
  // DQ (5/24/2005): Now fixup the file info's file name in the global scope!
  // printf ("In SgFile::SgFile p_sourceFileNamesWithPath[%d] = %s \n",fileNameIndex,p_sourceFileNamesWithPath[fileNameIndex]);
  // p_root->get_file_info()->set_filename(p_sourceFileNamesWithPath[fileNameIndex]);

  // DQ (1/22/2008): This should have been set above!
  // p_root->get_file_info()->set_filenameString(p_sourceFileNameWithPath);

  // DQ (1/21/2008): This must be set for all languages
     ROSE_ASSERT(p_root != NULL);
     ROSE_ASSERT(p_root->get_file_info() != NULL);
     ROSE_ASSERT(p_root->get_file_info()->get_filenameString().empty() == false);
  // printf ("p_root->get_file_info()->get_filenameString() = %s \n",p_root->get_file_info()->get_filenameString().c_str());

  // DQ (1/18/2006): Set the filename in the SgFile::p_file_info
     ROSE_ASSERT(get_file_info() != NULL);
     get_file_info()->set_filenameString(p_sourceFileNameWithPath);

#if 0
     printf ("In SgFile::SgFile(): get_skipfinalCompileStep() = %s \n",
          get_skipfinalCompileStep() ? "true" : "false");
#endif

  // DQ (5/3/2007): Added assertion.
     ROSE_ASSERT (get_startOfConstruct() != NULL);
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
bool
CommandlineProcessing::isExecutableFilename ( string name )
   {
     initExecutableFileSuffixList();

     // printf ("CommandlineProcessing::isExecutableFilename(): name = %s validExecutableFileSuffixes.size() = %zu \n",name.c_str(),validExecutableFileSuffixes.size());
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
               FILE* f = fopen(name.c_str(), "r");
               if (!f)
                  {
                    printf ("CommandlineProcessing::isExecutableFilename(): Could not open file, so it is assumed to NOT be an executable");
                 // ROSE_ASSERT(false);
                  }
                 else
                  {
                 // Check for if this is a binary executable file!
                    int character0 = fgetc(f);
                    int character1 = fgetc(f);

                 // The first character of an ELF binary is '\177' and for a PE binary it is 'M'
                 // if (character0 == 127)
                    if ((character0 == 0x7F && character1 == 0x45) ||
                        (character0 == 0x4D && character1 == 0x56))
                       {
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

// DQ (1/16/2008): This function was moved from the commandling_processing.C file to support the debugging specific to binary analysis
bool
CommandlineProcessing::isOptionTakingFileName( string argument )
   {
     bool result = false;
  // printf ("In CommandlineProcessing::isOptionTakingFileName(): argument = %s \n",argument.c_str());

  // List any rose options that take source filenames here, so that they can avoid 
  // being confused with the source file name that is to be read by EDG and translated.

  // DQ (1/6/2008): Added another test for a rose option that takes a filename
     if ( argument == "-o" ||                               // Used to specify output file to compiler
          argument == "-opt" ||                             // Used in loopProcessor
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
          argument == "-rose:astMergeCommandFile" )
        {
          result = true;
        }

  // printf ("In CommandlineProcessing::isOptionTakingFileName(): argument = %s result = %s \n",argument.c_str(),result ? "true" : "false");

     return result;
   }

// DQ (1/16/2008): This function was moved from the commandling_processing.C file to support the debugging specific to binary analysis
Rose_STL_Container<string>
CommandlineProcessing::generateSourceFilenames ( Rose_STL_Container<string> argList )
   {
     Rose_STL_Container<string> sourceFileList;

     Rose_STL_Container<string>::iterator i = argList.begin();

  // skip the 0th entry since this is just the name of the program (e.g. rose)
     ROSE_ASSERT(argList.size() > 0);
     i++;

     int counter = 0;
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

            // bool foundSourceFile = false;

#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
            // printf ("isExecutableFilename(%s) = %s \n",(*i).c_str(),isExecutableFilename(*i) ? "true" : "false");
               if ( isSourceFilename(*i) == false && isObjectFilename(*i) == false && isExecutableFilename(*i) == true )
                  {
                    // printf ("This is an executable file: *i = %s \n",(*i).c_str());
                 // executableFileList.push_back(*i);
                    sourceFileList.push_back(*i);
                    goto incrementPosition;
                  }
#endif
            // PC (4/27/2006): Support for custom source file suffixes
            // if ( isSourceFilename(*i) )
               if ( isObjectFilename(*i) == false && isSourceFilename(*i) == true )
                  {
                 // printf ("This is a source file: *i = %s \n",(*i).c_str());
                 // foundSourceFile = true;
                    sourceFileList.push_back(*i);
                    goto incrementPosition;
                  }
#if 0
               if ( isObjectFilename(*i) )
                  {
                    objectFileList.push_back(*i);
                  }
#endif

             }

       // DQ (12/8/2007): Looking for rose options that take filenames that would accidentally be considered as source files.
          if (isOptionTakingFileName(*i) == true)
             {
            // Jump over the next argument when such options are identified.
               counter++;
               i++;
             }

incrementPosition:

          counter++;
          i++;
        }

     return sourceFileList;
   }
#endif

// This is the "C" function implemented in:
//    ROSE/src/frontend/OpenFortranParser_SAGE_Connection/openFortranParser_main.c
// This function calls the Java JVM to load the Java implemented parser (written 
// using ANTLR, a parser generator).
extern "C" int openFortranParser_main(int argc, char **argv );

int
SgFile::callFrontEnd ()
   {
  // This is the function call to the EDG front-end (modified in ROSE to pass a SgFile)
     int edg_main(int, char *[], SgFile & sageFile );

  // This is how we pass the pointer to the SgFile created in ROSE before the Open 
  // Fortran Parser is called to the Open Fortran Parser.  In the case of C/C++ using
  // EDG the SgFile is passed through the edg_main() function, but not so with the 
  // Open Fortran Parser's openFortranParser_main() function API.  So we use this
  // global variable to pass the SgFile (so that the parser c_action functions can
  // build the Fotran AST using the existing SgFile.
     extern SgFile* OpenFortranParser_globalFilePointer;

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

          std::string frontEndCommandLineString;
          if ( get_new_frontend() == true )
             {
            // Use the current version of the EDG frontend from EDG (or any other version)
               abort();
               printf ("ROSE::new_frontend == true (call edgFrontEnd using unix system() function!) \n");

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
               display("SgFile::callFrontEnd()");
               printf ("get_C_only()       = %s \n",(get_C_only() == true) ? "true" : "false");
               printf ("get_C99_only()     = %s \n",(get_C99_only() == true) ? "true" : "false");
               printf ("get_Cxx_only()     = %s \n",(get_Cxx_only() == true) ? "true" : "false");
               printf ("get_Fortran_only() = %s \n",(get_Fortran_only() == true) ? "true" : "false");
               printf ("get_F77_only()     = %s \n",(get_F77_only() == true) ? "true" : "false");
               printf ("get_F90_only()     = %s \n",(get_F90_only() == true) ? "true" : "false");
               printf ("get_F95_only()     = %s \n",(get_F95_only() == true) ? "true" : "false");
               printf ("get_F2003_only()   = %s \n",(get_F2003_only() == true) ? "true" : "false");
               printf ("get_binary_only()  = %s \n",(get_binary_only() == true) ? "true" : "false");
#endif
#if 0
               printf ("Exiting while testing binary \n");
               ROSE_ASSERT(false);
#endif

               if (get_Fortran_only() == true)
                  {
                 // frontendErrorLevel = openFortranParser_main (numberOfCommandLineArguments, inputCommandLine,*this);
                 // printf ("Calling the openFortranParser_main() function (which loads the JVM) \n");

                 // DQ (9/30/2007): Introduce syntax checking on input code (initially we can just call the backend compiler 
                 // and let it report on the syntax errors).  Later we can make this a command line switch to disable (default 
                 // should be true).
                 // bool syntaxCheckInputCode = true;
                    bool syntaxCheckInputCode = (get_skip_syntax_check() == false);

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

                         vector<string> fortranCommandLine;
                         fortranCommandLine.push_back("gfortran");
                         fortranCommandLine.push_back("-fsyntax-only");
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
                              if (backendCompilerSystem == "g++")
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

                      // DQ (11/17/2007): Set the fortran mode used with gfortran.
                         if (get_F90_only() == true || get_F95_only() == true)
                            {
                           // For now let's consider f90 to be syntax checked under f95 rules (since gfortran does not support a f90 specific mode)
                              fortranCommandLine.push_back("-std=f95");
                            }
                           else
                            {
                              if (get_F2003_only() == true)
                                 {
                                   fortranCommandLine.push_back("-std=f2003");
                                 }
                                else
                                 {
                                // This should be the default mode (fortranMode string is empty). So is it f77?
                                 }
                            }

                      // DQ (12/8/2007): Added support for cray pointers from commandline.
                         if (get_cray_pointer_support() == true)
                            {
                              fortranCommandLine.push_back("-fcray-pointer");
                            }

                      // Note that "-c" is required to enforce that we only compile and not link the result (even though -fno-backend is specified)
                      // A web page specific to -fno-backend suggests using -fsyntax-only instead (so the "-c" options is not required).

                      // if ( SgProject::get_verbose() > 0 )
                         if ( get_verbose() > 0 )
                            {
                              printf ("Checking syntax of input program using gfortran: syntaxCheckingCommandline = %%s \n" /* ,syntaxCheckingCommandline.c_str() FIXME */);
                            }

                      // Call the OS with the commandline defined by: syntaxCheckingCommandline
                         fortranCommandLine.push_back(get_sourceFileNameWithPath());
                         int returnValueForSyntaxCheckUsingBackendCompiler = systemFromVector (fortranCommandLine);

                      // Check that there are no errors, I think that warnings are ignored!
                         if (returnValueForSyntaxCheckUsingBackendCompiler != 0)
                            {
                              printf ("Syntax errors detected in input fortran program ... \n");
                              exit(1);
                            }
                         ROSE_ASSERT(returnValueForSyntaxCheckUsingBackendCompiler == 0);
                       }

                 // printf ("get_output_parser_actions() = %s \n",get_output_parser_actions() ? "true" : "false");
                    if (get_output_parser_actions() == true)
                       {
                      // DQ (1/19/2008): New version of OFP requires different calling syntax.
                      // string OFPCommandLineString = std::string("java parser.java.FortranMain") + " --dump " + get_sourceFileNameWithPath();
                         vector<string> OFPCommandLine;
                         OFPCommandLine.push_back("java");
                         OFPCommandLine.push_back("fortran.ofp.FrontEnd");
                         OFPCommandLine.push_back("--dump");
                         OFPCommandLine.push_back(get_sourceFileNameWithPath());
                         // printf ("output_parser_actions: OFPCommandLineString = %s \n",OFPCommandLineString.c_str());
#if 1
                      // Some security checking here could be helpful!!!
                         systemFromVector (OFPCommandLine);
#else
                      // This fails, I think because we can't call the openFortranParser_main twice.
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
                       }

                 // Option to just run the parser (not constructing the AST) and quit.
                 // printf ("get_exit_after_parser() = %s \n",get_exit_after_parser() ? "true" : "false");
                    if (get_exit_after_parser() == true)
                       {
                      // DQ (1/19/2008): New version of OFP requires different calling syntax.
                      // string OFPCommandLineString = std::string("java parser.java.FortranMain") + " " + get_sourceFileNameWithPath();
                         vector<string> OFPCommandLine;
                         OFPCommandLine.push_back("java");
                         OFPCommandLine.push_back("fortran.ofp.FrontEnd");
                         OFPCommandLine.push_back(get_sourceFileNameWithPath());

                      // printf ("exit_after_parser: OFPCommandLineString = %s \n",OFPCommandLineString.c_str());
#if 1
                      // Some security checking here could be helpful!!!
                         systemFromVector (OFPCommandLine);
#else
                      // This fails, I think because we can't call the openFortranParser_main twice.
                         int openFortranParser_only_argc    = 0;
                         char** openFortranParser_only_argv = NULL;
                         CommandlineProcessing::generateArgcArgvFromList(OFPCommandLine,openFortranParser_only_argc,openFortranParser_only_argv);
                         frontendErrorLevel = openFortranParser_main (openFortranParser_only_argc, openFortranParser_only_argv);
#endif
                         printf ("Exiting after parsing... (get_exit_after_parser() == true) \n");
                         exit(0);
                       }
                    
                 // DQ (1/19/2008): New version of OFP requires different calling syntax; new lib name is: libfortran_ofp_parser_java_FortranParserActionJNI.so old name: libparser_java_FortranParserActionJNI.so
                 // frontEndCommandLineString = std::string(argv[0]) + " --class parser.java.FortranParserActionJNI " + get_sourceFileNameWithPath();
                    vector<string> frontEndCommandLine;
                    frontEndCommandLine.push_back(argv[0]);
                    frontEndCommandLine.push_back("--class");
                    frontEndCommandLine.push_back("fortran.ofp.parser.java.FortranParserActionJNI");
                    frontEndCommandLine.push_back(get_sourceFileNameWithPath());

#if 0
                    if ( get_verbose() > 0 )
                         printf ("numberOfCommandLineArguments = %zu frontEndCommandLineString = %s \n",inputCommandLine.size(),frontEndCommandLineString.c_str());
#endif

                    int openFortranParser_argc    = 0;
                    char** openFortranParser_argv = NULL;
                    CommandlineProcessing::generateArgcArgvFromList(frontEndCommandLine,openFortranParser_argc,openFortranParser_argv);

                    // printf ("openFortranParser_argc = %d openFortranParser_argv = %s \n",openFortranParser_argc,CommandlineProcessing::generateStringFromArgList(openFortranParser_argv,false,false).c_str());

                 // DQ (8/19/2007): Setup the global pointer used to pass the SgFile to which the Open Fortran Parser 
                 // should attach the AST.  This is a bit ugly, but the parser interface only takes a commandline so it 
                 // would be more ackward to pass a pointer to a C++ object through the commandline or the Java interface.
                    OpenFortranParser_globalFilePointer = const_cast<SgFile*>(this);
                    ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);

                    if ( get_verbose() > 1 )
                         printf ("Calling openFortranParser_main(): OpenFortranParser_globalFilePointer = %p \n",OpenFortranParser_globalFilePointer);

                 // frontendErrorLevel = openFortranParser_main (numberOfCommandLineArguments, inputCommandLine);
                    frontendErrorLevel = openFortranParser_main (openFortranParser_argc, openFortranParser_argv);

                    if ( get_verbose() > 1 )
                         printf ("DONE: Calling the openFortranParser_main() function (which loads the JVM) \n");

                 // Reset this global pointer after we are done (just to be safe and avoid it being used later and causing strange bugs).
                    OpenFortranParser_globalFilePointer = NULL;
                  }
                 else
                  {
                    if (get_binary_only() == true)
                       {
#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
                         string executableFileName = this->get_sourceFileNameWithPath();

                         if ( get_verbose() > 0 )
                              printf ("Disassemble executableFileName = %s \n",executableFileName.c_str());

                      // We likely need some error reporting from objdumpToRoseBinaryAst()
                         frontendErrorLevel = 0;

                      // Disassemble the binary file (using recursive disassembler based on objdump.
                      // SgAsmFile* asmFile = objdumpToRoseBinaryAst(executableFileName);
                         SgAsmFile* asmFile = new SgAsmFile();
                         ROSE_ASSERT(asmFile != NULL);

                         printf ("Calling generateBinaryExecutableFileInformation() \n");

                      // Get the structure of the binary file (only implemented for ELF formatted files currently).
                      // Later we will implement a PE reader to get the structure of MS Windows executables.
                         generateBinaryExecutableFileInformation(executableFileName,asmFile);

#if 0
                      // DQ (1/22/2008): This is a temporary way to setup the Binary analysis support, in the 
                      // future the binary analysis will query the information in the SgAsmFile.
                         if (asmFile->get_machine_architecture() == SgAsmFile::e_machine_architecture_Intel_80386)
                            {
                              RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
                            }
                           else
                            {
                              printf ("Error: only Intel_80386 machine architecture currently supported! \n");
                            }

                         if (asmFile->get_binary_class_type() == SgAsmFile::e_class_32)
                            {
                              RoseBin_Arch::arch = RoseBin_Arch::bit32;
                            }
                           else
                            {
                              printf ("Error: only 32-bit binaries are currently supported for disassembly using ROSE! \n");
                            }

                      // We need to figure out which type of binary this is, x86 or ARM, etc.
                      // To do this we need to consult the binary file format data (computed 
                      // by generateBinaryExecutableFileInformation()).
                      // RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
                         ROSE_ASSERT(asmFile->get_machine_architecture() == SgAsmFile::e_machine_architecture_Intel_80386);
                         ROSE_ASSERT(asmFile->get_binary_class_type()    == SgAsmFile::e_class_32);
#endif

                      // This is an object required for the unparsing of instructions, we 
                      // built it now so that it can always be accessed later.
                         RoseBin_unparse* unparser = new RoseBin_unparse();
                         ROSE_ASSERT(unparser != NULL);
                      // RoseBin_support::setUnparseVisitor(unparser->getVisitor());

                      // Fill in the instructions into the SgAsmFile IR node
                         SgProject* project = isSgProject(this->get_parent());
                         ROSE_ASSERT(project != NULL);
                         objdumpToRoseBinaryAst(executableFileName,asmFile,project);

                      // Attach the SgAsmFile to the SgFile
                         this->set_binaryFile(asmFile);
                         asmFile->set_parent(this);

                      // DQ (1/22/2008): The generated unparsed assemble code can not currently be compiled because the 
                      // addresses are unparsed (see Jeremiah for details).
                      // Skip running gnu assemble on the output since we include text that would make this a problem.
                         project->skipfinalCompileStep(true);

                      // This is now done below in the Secondary file processing phase.
                      // Generate the ELF executable format structure into the AST
                      // generateBinaryExecutableFileInformation(executableFileName,asmFile);
#endif
                       }
                      else
                       {
                         frontEndCommandLineString = std::string(argv[0]) + std::string(" ") + CommandlineProcessing::generateStringFromArgList(inputCommandLine,false,false);

                         if ( get_verbose() > 1 )
                              printf ("Before calling edg_main: frontEndCommandLineString = %s \n",frontEndCommandLineString.c_str());

                         int edg_argc = 0;
                         char **edg_argv = NULL;
                         CommandlineProcessing::generateArgcArgvFromList(inputCommandLine, edg_argc, edg_argv);

                         frontendErrorLevel = edg_main (edg_argc, edg_argv, *this);
                       }
                  }

               if ( get_verbose() > 1 )
                    printf ("DONE: frontend called (frontendErrorLevel = %d) \n",frontendErrorLevel);

            // If we had any errors reported by the frontend then quite now
               if (frontendErrorLevel > 3)
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
                         cout << "Errors in Processing: (frontendErrorLevel > 3)" << endl;
                         ROSE_ABORT();
                       }
                  }

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
     AstPostProcessing(this);


  // **************************************************************************
  //                      Secondary Pass Over Source File
  // **************************************************************************
  // This pass collects extra information about the soruce file thay may not have 
  // been available from previous tools that operated on the file. For example:
  //    1) EDG ignores comments and so we collect the whole token stream in this phase.
  //    2) OFP ignores comments similarly to EDG and so we collect the whole token stream. 
  //    3) Binary disassemblily ignores the binary format so we collect this information
  //       about the structure of the ELF binary seperately.
  // For source code (C,C++,Fortran) we collect the whole token stream, for example:
  //    1) Comments
  //    2) Preprocessors directives
  //    3) White space
  //    4) All tokens (each is classified as to what specific type of token it is)
  //
  // There is no secondary processing for binaries.

     if (get_binary_only() == true)
        {
       // This is now done above so that we can know the machine specific details
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

       // DQ (4/19/2006): attachAllPreprocessingInfo() is now merged into the attachPreprocessingInfo() function.
       // DQ (4/6/2006): This is also the correct function to call to use Wave.
       // Markus Kowarschik: attach preprocessor directives to AST nodes
       // This uses the AST traversal and so needs to be done after the 
       // call to temporaryAstFixes(), above.
          if (get_skip_commentsAndDirectives() == false)
             {
               if (get_verbose() > 1)
                  {
                    printf ("In SgFile::callFrontEnd(): calling attachAllPreprocessingInfo() \n");
                  }

               attachPreprocessingInfo(this);

               if (get_verbose() > 1)
                  {
                    printf ("In SgFile::callFrontEnd(): Done with attachAllPreprocessingInfo() \n");
                  }
             }
        }

  // DQ (4/11/2006): This is Lingxiao's work (new patch, since the first and second attempts didn't work) 
  // to support attaching comments from all header files to the AST.  It seems that both 
  // attachPreprocessingInfo() and attachAllPreprocessingInfo() can be run as a mechanism to test 
  // Lingxiao's work on at least the source file.  Note that performance associated with collecting
  // all comments and CPP directives from header files might be a problem.
  // DQ (4/1/2006): This will have to be fixed a little later (next release)
  // DQ (3/29/2006): This is Lingxiao's work to support attaching comments from all header files to the 
  // AST (the previous mechanism only extracted comments from the source file and atted them to the AST, 
  // the part of the AST representing the input source file).
  // attachAllPreprocessingInfo(this,collectAllCommentsAndDirectives);

  // display("At bottom of SgFile::callFrontEnd()");

  // return the error code associated with the call to the C++ Front-end
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

  // To use rose in place of a C or C++ compiler specify the compiler name using
  //      rose -compiler <originalCompilerName> ...
  // the default value of "originalCompilerName" is "CC"
     vector<string> compilerNameString;
     compilerNameString.push_back(compilerName);

  // DQ (1/17/2006): test this
  // ROSE_ASSERT(get_fileInfo() != NULL);

#if 0
     display("SgFile::buildCompilerCommandLineOptions()");
     printf ("C   compiler       = %s \n",BACKEND_C_COMPILER_NAME_WITH_PATH);
     printf ("C++ compiler       = %s \n",BACKEND_CXX_COMPILER_NAME_WITH_PATH);
     printf ("get_C_only()       = %s \n",(get_C_only() == true) ? "true" : "false");
     printf ("get_C99_only()     = %s \n",(get_C99_only() == true) ? "true" : "false");
     printf ("get_Cxx_only()     = %s \n",(get_Cxx_only() == true) ? "true" : "false");
     printf ("get_Fortran_only() = %s \n",(get_Fortran_only() == true) ? "true" : "false");
     printf ("get_F77_only()     = %s \n",(get_F77_only() == true) ? "true" : "false");
     printf ("get_F90_only()     = %s \n",(get_F90_only() == true) ? "true" : "false");
     printf ("get_F95_only()     = %s \n",(get_F95_only() == true) ? "true" : "false");
     printf ("get_F2003_only()   = %s \n",(get_F2003_only() == true) ? "true" : "false");
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
        }
       else
        {
          compilerNameString[0] = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
          if (get_Fortran_only() == true)
             {
            // compilerNameString = "f77 ";
               compilerNameString[0] = "gfortran";

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
                         compilerNameString.push_back("-ffree-line-length-none");
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
     if (compilerNameString[0].find("icc") != string::npos)
        {
       // This is the Intel C compiler: icc, we need to add the -restrict option
          compilerNameString.push_back("-restrict");
        }

     if (compilerNameString[0].find("icpc") != string::npos)
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

  // printf ("Selected compiler = %s \n",compilerNameString.c_str());

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

  // DQ (3/31/2004): New cleaned up source file handling
     Rose_STL_Container<string> argcArgvList = argv;

  // DQ (9/25/2007): Moved to std::vector from std::list uniformally within ROSE.
  // Remove the first argument (argv[0])
  // argcArgvList.pop_front();
     argcArgvList.erase(argcArgvList.begin());

     SgProject* project = isSgProject(this->get_parent());
     ROSE_ASSERT (project != NULL);
     Rose_STL_Container<string> sourceFilenames = project->get_sourceFileNameList();

  // printf ("sourceFilenames.size() = %zu sourceFilenames = %s \n",sourceFilenames.size(),StringUtility::listToString(sourceFilenames).c_str());

  // DQ (4/20/2006): Modified to only do this when generating code and compiling it
  // Remove the source names from the argcArgvList (translated versions of these will be inserted later)
  // if (get_skip_unparse() == true && get_skipfinalCompileStep() == false)
     if (get_skip_unparse() == false)
        {
          for (Rose_STL_Container<string>::iterator i = sourceFilenames.begin(); i != sourceFilenames.end(); i++)
             {
            // printf ("Removing sourceFilenames list element i = %s \n",(*i).c_str());
#if USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST
#error "USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST is not supported yet"

            // DQ (9/1/2006): Check for use of absolute path and convert filename to absolute path if required
               bool usesAbsolutePath = ((*i)[0] == '/');
               if (usesAbsolutePath == false)
                  {
                    string targetSourceFileToRemove = StringUtility::getAbsolutePathFromRelativePath(*i);
                 // printf ("Converting source file to absolute path to search for it and remove it! targetSourceFileToRemove = %s \n",targetSourceFileToRemove.c_str());
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
               argcArgvList.erase(find(argcArgvList.begin(),argcArgvList.end(),*i));
#endif
             }
        }

  // printf ("After removing source file name: argcArgvList.size() = %zu argcArgvList = %s \n",argcArgvList.size(),StringUtility::listToString(argcArgvList).c_str());
  // ROSE_ASSERT(false);

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
          compilerNameString.push_back(std::string("-I") + oldFileNamePathOnly);

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

       // DQ (7/14/2004): Suggested fix from Andreas
          if (objectNameSpecified == false)
             {
               compilerNameString.push_back("-o");
               compilerNameString.push_back(currentDirectory + "/" + objectFileName);
             }
        }

  // printf ("At base of buildCompilerCommandLineOptions: compilerNameString = \n%s\n",compilerNameString.c_str());

#if 0
     printf ("Exiting at base of buildCompilerCommandLineOptions() ... \n");
     ROSE_ASSERT (false);
#endif

     return compilerNameString;
   }

int
SgFile::compileOutput ( vector<string>& argv, int fileNameIndex, const string& compilerNameOrig )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (compile output):");

  // DQ (4/21/2006): I think we can now assert this!
     ROSE_ASSERT(fileNameIndex == 0);

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
          printf ("Now call the backend (vendor's) compiler for file = %s \n",get_unparse_output_filename().c_str());

  // Build the commandline to hand off to the C++/C compiler
     vector<string> compilerNameString = buildCompilerCommandLineOptions (argv,fileNameIndex, compilerName );
  // ROSE_ASSERT (compilerNameString != NULL);

     int returnValueForCompiler = 0;

  // error checking
  // display("Called from SgFile::compileOutput()");

  // allow conditional skipping of the final compile step for testing ROSE
     if (get_skipfinalCompileStep() == false)
        {
#if 0
          if ( get_verbose() > 1 )
               printf ("calling systemFromVector(%s) \n",compilerNameString.c_str());
#endif

          returnValueForCompiler = systemFromVector (compilerNameString);
        }
       else
        {
          if ( get_verbose() > 1 )
               printf ("COMPILER NOT CALLED: compilerNameString = %s \n", "<unknown>" /* compilerNameString.c_str() */);

          printf ("Skipped call to backend vendor compiler! \n");
        }

  // DQ (7/20/2006): Catch errors returned from unix "system" function (commonly "out of memory" errors, suggested by Peter and Jeremiah).
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
       // printf ("     (before) finalCompiledExitStatus = %d \n",finalCompiledExitStatus);
          finalCompiledExitStatus = (finalCompiledExitStatus == 0) ? /* error */ 1 : /* success */ 0;
       // printf ("     (after) finalCompiledExitStatus = %d \n",finalCompiledExitStatus);
        }

  // printf ("Program Terminated Normally (exit status = %d)! \n\n\n\n",finalCompiledExitStatus);

     return finalCompiledExitStatus;
   }



int
SgProject::compileOutput( const std::string& compilerName )
   {
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Backend Compilation (SgProject):");

     int errorCode = 0;
     int i = 0;

     if (numberOfFiles() == 0)
        {
          printf ("Note in SgProject::compileOutput(%s): numberOfFiles() == 0 \n",compilerName.c_str());
        }

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

       // printf ("Exiting after call to compiler using -E option! \n");
       // ROSE_ASSERT(false);
        }
       else
        {
       // Typical case
          for (i=0; i < numberOfFiles(); i++)
             {
               SgFile & file = get_file(i);
#if 0
               printf ("In Project::compileOutput(%s): get_file(%d).get_skipfinalCompileStep() = %s \n",
                    compilerName,i,(get_file(i).get_skipfinalCompileStep()) ? "true" : "false");
#endif

            // DQ (8/13/2006): Only use the first file (I don't think this
            // makes sense with multiple files specified on the commandline)!
            // int localErrorCode = file.compileOutput(i, compilerName);
               int localErrorCode = file.compileOutput(0, compilerName);
               if (localErrorCode > errorCode)
                    errorCode = localErrorCode;
             }
        }

     return errorCode;
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
          SgProject* project = isSgProject(get_parent());

          ROSE_ASSERT ( project != NULL );
          returnValue = project->get_prelink();
        }

     return returnValue;

  // Note that project can be false if this is called on an intermediate file 
  // generated by the rewrite system.
  // return (project == NULL) ? false : project->get_prelink();
   }

int
SgProject::link ( std::string linkerName )
   {
  // Compile the output file from the unparing
     vector<string> argv = get_originalCommandLineArgumentList();

  // error checking
     ROSE_ASSERT (argv.size() > 1);
     ROSE_ASSERT(linkerName != "");

  // strip out any rose options before passing the command line.
     SgFile::stripRoseCommandLineOptions( argv );

  // strip out edg specific options that would cause an error in the backend linker (compiler).
     SgFile::stripEdgCommandLineOptions( argv );

  // Call the compile
     int errorCode = link ( argv, linkerName );

  // return the error code from the compilation
     return errorCode;
   }

int
SgProject::link ( const std::vector<std::string>& argv, std::string linkerName )
   {
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
               linkerName = "f77 ";
             }
        }
     
  // This is a better implementation since it will include any additional command line options that target the linker
     Rose_STL_Container<string> l = argv;

  // remove the name of the translator (argv[0]) from the command line input
     ROSE_ASSERT(l.size() > 0);

  // DQ (9/25/2007): Moved to std::vector from std::list uniformally within ROSE.
  // l.pop_front();
  // l.erase(l.begin());
     l[0] = linkerName;

  // std::string linkerCommandLine = linkerName + whiteSpace + StringUtility::listToString(l);
#if 0
     if ( get_verbose() > 1 )
          printf ("linkerCommandLine = %s \n",linkerCommandLine.c_str());
#endif

     int status = systemFromVector(l);

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
"source code, as well as on x86 and ARM object code.\n"
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
"     -rose:Fortran, -rose:F, -rose:f\n"
"                             compile Fortran code, determining version of\n"
"                             Fortran from file suffix)\n"
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
"     -rose:astMerge          merge ASTs from different files\n"
"     -rose:astMergeCommandFile FILE\n"
"                             filename where compiler command lines are stored\n"
"                               for later processing (using AST merge mechanism)\n"
"     -rose:compilationPerformanceFile FILE\n"
"                             filename where compiler performance for internal\n"
"                               phases (in CSV form) is placed for later\n"
"                               processing (using script/graphPerformance)\n"
"     -rose:exit_after_parser just call the parser (fortran only)\n"
"     -rose:skip_syntax_check skip Fortran syntax checking (required for F2003 code\n"
"                               when using gfortran versions greater than 4.1)\n"
"     -rose:skip_rose         process command line and call backend directly,\n"
"                               skipping all ROSE-specific processing\n"
"     -rose:skip_transformation\n"
"                             read input file and skip all transformations\n"
"     -rose:skip_unparse      read and process input file but skip generation of\n"
"                               final C++ output file\n"
"     -rose:collectAllCommentsAndDirectives\n"
"                             store all comments and CPP directives in header\n"
"                               files into the AST\n"
"     -rose:excludeCommentsAndDirectives PATH\n"
"                             provide path to exclude when using the\n"
"                               collectAllCommentsAndDirectives option\n"
"     -rose:excludeCommentsAndDirectivesFrom FILENAME\n"
"                             provide filename to file with paths to exclude\n"
"                               when using the collectAllCommentsAndDirectives\n"
"                               option\n"
"     -rose:includeCommentsAndDirectives PATH\n"
"                             provide path to include when using the\n"
"                               collectAllCommentsAndDirectives option\n"
"     -rose:includeCommentsAndDirectivesFrom FILENAME\n"
"                             provide filename to file with paths to include\n"
"                             when using the collectAllCommentsAndDirectives\n"
"                             option\n"
"     -rose:skip_commentsAndDirectives\n"
"                             ignore all comments and CPP directives (can\n"
"                               generate (unparse) invalid code if not used with\n"
"                               -rose:unparse_includes)\n"
"     -rose:skipfinalCompileStep\n"
"                             read input file and skip all transformations\n"
"     -rose:prelink           activate prelink mechanism to force instantiation\n"
"                               of templates and assignment to files\n"
"     -rose:instantiation XXX control template instantiation\n"
"                             XXX is one of (none, used, all, local)\n"
"\n"
"GNU g++ options recognized:\n"
"     -ansi                   equivalent to -rose:strict\n"
"     -fno-implicit-templates disable output of template instantiations in\n"
"                               generated source\n"
"     -fno-implicit-inline-templates\n"
"                             disable output of inlined template instantiations\n"
"                               in generated source\n"
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
"Control code generation:\n"
"     -rose:unparse_line_directives\n"
"                               unparse statements using #line directives with\n"
"                               reference to the original file and line number\n"
"                               to support view of original source in debuggers\n"
"                               and external tools\n"
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









