static const char *purpose = "generates source code for defining grammars";

static const char *description =
"This program demonstrates the Meta-Program Level where the details "
"of a preprocessor are specified.  In this case this program "
"represents the MetaProgram to build a preprocessor for the A++/P++ "
"array class.\n\n"

"Currently the grammars are defined, the execution of this program "
"(a C++ program) generates the source code for defining the grammars "
"to be used in building a preprocessor.  So this example does not "
"yet build all the code required to build a preprocessor (the rest "
"is specified in the ROSE/src directory structure).";

// include definitions of grammars, terminals, and non-terminals
// (objects within ROSETTA)
#include "grammar.h"

#include <iostream>
#include <rose_config.h>
#include <Sawyer/CommandLine.h>
#include <string>
#include <vector>

bool verbose = false;

// Parse command-line, returning the positional (non-switch) arguments.
static std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    Parser p;
    p.purpose(purpose);
    p.doc("Description", description);
    p.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{outputDirectory}]");
    p.chapter(1, "ROSE Command-line Tools");
#if defined(ROSE_PACKAGE_VERSION)
    std::string v = ROSE_PACKAGE_VERSION;
#elif defined(PACKAGE_VERSION)
    std::string v = PACKAGE_VERSION;
#else
    std::string v = std::string(ROSE_SCM_VERSION_ID).substr(0, 8);
#endif
    p.version(v, ROSE_CONFIGURE_DATE);
    p.groupNameSeparator(":");

    p.with(Switch("help", 'h')
           .doc("Show this documentation.")
           .action(showHelpAndExit(0)));

    p.with(Switch("version", 'V')
           .doc("Show version number.")
           .action(showVersionAndExit(v, 0)));

    p.with(Switch("verbose", 'v')
           .intrinsicValue(true, verbose)
           .doc("Print some information to standard output as this generator runs."));

    return p.parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char * argv[])
   {
     using namespace std;

     std::vector<std::string> args = parseCommandLine(argc, argv);

  // Main Function for ROSE Preprocessor
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     if (verbose) {
         printf ("***************************************************************************************** \n");
         printf ("Build the C++ grammar (essentially an automated generation of a modified version of SAGE) \n");
         printf ("***************************************************************************************** \n");
     }
     
  // First build the C++ grammar
     std::string target_directory = ".";
     if (args.size() > 1) {
         cerr <<argv[0] <<": incorrect usage; see --help\n";
         exit(1);
     }
     if (args.size() == 1)
         target_directory = std::string(args[0]);

  // For base level grammar use prefix "Sg" to be compatable with SAGE
     Grammar sageGrammar ( /* name of grammar */ "Cxx_Grammar", 
                           /* Prefix to names */ "Sg", 
                           /* Parent Grammar  */ "ROSE_BaseGrammar",
                           /* No parent Grammar */ NULL,
                           target_directory
                           );

  // Build the header files and source files representing the
  // grammar's implementation
     sageGrammar.buildCode();

  // Support for output of constructors as part of generated documentation
     string documentedConstructorPrototypes = sageGrammar.staticContructorPrototypeString;

     if (verbose) {
         printf ("documentedConstructorPrototypes = %s \n",documentedConstructorPrototypes.c_str());
         printf ("Rosetta finished.\n");
     }

     return 0;
}
