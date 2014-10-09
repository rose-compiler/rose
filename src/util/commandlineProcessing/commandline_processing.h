#ifndef ROSE_COMMANDLINE_UTILITY_H
#define ROSE_COMMANDLINE_UTILITY_H
#include "setup.h"
//#include "rosedefs.h"
#include "rosedll.h"
#include <list>
#define Rose_STL_Container std::vector
#include <sawyer/CommandLine.h>

//Rama (12/22/2006): changing the class to a namespace and removing the "static"ness of the "member" functions

namespace CommandlineProcessing
   {
          /** Generic command-line components.
           *
           *  Returns a description of the switches that should be available for all ROSE tools. To make a command-line
           *  parser that recognizes these switches, add the switches to the parser using its @c with method.  For example:
           *
           * @code
           *  static Sawyer::CommandLine::ParserResult
           *  parseCommandLine(int argc, char *argv[]) {
           *      Sawyer::CommandLine::Parser parser;
           *      return parser
           *          .with(CommandlineProcessing::genericSwitches()) // these generic switches
           *          .with(mySwitches)                               // my own switches, etc.
           *          .parse(argc, argv)                              // parse without side effects
           *          .apply();                                       // apply parser results
           *  }
           * @endcode
           *
           *  See any recent tool for more examples. */
          Sawyer::CommandLine::SwitchGroup genericSwitches();


      //! Separate a string into individual parameters and store them into a string vector
          ROSE_UTIL_API Rose_STL_Container<std::string> generateArgListFromString ( std::string commandline );

      //! Convert a vector of string to a single string
       // std::string generateStringFromArgList( Rose_STL_Container<std::string> & argList);
          ROSE_UTIL_API std::string generateStringFromArgList( const Rose_STL_Container<std::string> & argList);

          //! Convert an argc-argv pair into a string vector
          ROSE_UTIL_API Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, const char* argv[] );
          
          inline Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, char* argv[] ) 
          {return generateArgListFromArgcArgv(argc, (const char**)argv);}
          
          //! Convert a string vector back to an argc-argv pair
          ROSE_UTIL_API void generateArgcArgvFromList ( Rose_STL_Container<std::string> argList, int & argc, char** & argv );

          //! Looks for inputPrefix prefixed options. Push the stripped-off option in the result list.
          //! Warning: As opposed to what the former documentation was saying this function doesn't modify argList.
          ROSE_UTIL_API Rose_STL_Container<std::string> generateOptionList (const Rose_STL_Container<std::string> & argList, std::string inputPrefix );

          //! Looks for inputPrefix-prefixed options. Push the stripped-off option in the result list.
          //! If isOptionTakingSecondParameter of the inputPrefix-prefixed returns true, add the parameter(s) to the result list.
          ROSE_UTIL_API Rose_STL_Container<std::string> generateOptionListWithDeclaredParameters (const Rose_STL_Container<std::string> & argList, std::string inputPrefix );

       //! Find all options matching 'inputPrefix:optionName' || 'inputPrefix:optionName optionValue' from argList,
       //! strip off 'inputPrefix:' or replace it by 'newPrefix' if provided. Returns a string list of matched options.
       //! All matching options and values are removed from argList.
          ROSE_UTIL_API Rose_STL_Container<std::string> generateOptionWithNameParameterList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix, std::string newPrefix = "");

          extern Rose_STL_Container<std::string> extraCppSourceFileSuffixes;

       //! Search 'argv' for an option like optionPrefixOption, remove the option if 'removeOption' is true. e.g. isOption(argv,"-rose:","(C99|C99_only)",false)
       /*! 
        The argument 'option' adds () to the actual option, and allows the |(OR) operations.For example: 
              CommandlineProcessing::isOption(argv,"-rose:","(skip_syntax_check)",true)
              CommandlineProcessing::isOption(argv,"-rose:","(C99|C99_only)",false) 
       */       
          ROSE_UTIL_API bool isOption ( std::vector<std::string> & argv, std::string optionPrefix, std::string Option, bool removeOption );

       //! Search 'argv' for 'optionPrefixOption value',  store the integer value into 'optionParameter'. Remove the original option if 'removeOption' is true.
       //! Available value types are: str, float, double, int, short, long, unsigned int, unsigned short, unsigned long, char, etc.
          ROSE_UTIL_API bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string Option, int & optionParameter, bool removeOption );

       //! Search 'argv' for 'optionPrefixOption value',  store the float value into 'optionParameter'. Remove the original option if 'removeOption' is true.
          ROSE_UTIL_API bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string Option, float & optionParameter, bool removeOption );

       //! Search 'argv' for 'optionPrefixOption value',  store the string type value into 'optionParameter'. Remove the original option if 'removeOption' is true.
          ROSE_DLL_API bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string Option, std::string & optionParameter, bool removeOption );

      //! Add the strings in argList to the command line represented by argc and argv, prepend 'prefix' to each of the arguments
          ROSE_UTIL_API void addListToCommandLine ( std::vector<std::string> & argv , std::string prefix, Rose_STL_Container<std::string> argList );
      //! Remove all options matching a specified prefix 'prefix' from the argument list 'argv'  
          ROSE_UTIL_API void removeArgs ( std::vector<std::string> & argv, std::string prefix );
      //! Remove all options matching a specified prefix 'prefix' from the argument list 'argv', as well as the option values following them
          ROSE_UTIL_API void removeArgsWithParameters ( std::vector<std::string> & argv, std::string prefix );
       //! Remove file names specified in filenameList from argv, except for 'exceptFilename'
          ROSE_UTIL_API void removeAllFileNamesExcept ( std::vector<std::string> & argv, Rose_STL_Container<std::string> filenameList, std::string exceptFilename );

      //! Build a string from the argList
          ROSE_UTIL_API std::string generateStringFromArgList ( Rose_STL_Container<std::string> argList, bool skipInitialEntry, bool skipSourceFiles );

      //! Build the list of isolated file names from the command line
          ROSE_DLL_API Rose_STL_Container<std::string> generateSourceFilenames ( Rose_STL_Container<std::string> argList, bool binaryMode );

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support auto-documentation.
      //! Add another valid source file suffix
          ROSE_UTIL_API void addSourceFileSuffix ( const std::string &suffix );
          ROSE_UTIL_API void addCppSourceFileSuffix ( const std::string &suffix );

          ROSE_UTIL_API bool isSourceFilename ( std::string name ); 

          ROSE_UTIL_API bool isObjectFilename ( std::string name );
          ROSE_DLL_API bool isExecutableFilename ( std::string name );

       // DQ (8/20/2008): Added test that will allow bogus exececutable files (marked as .exe but not executable) to pass
          ROSE_DLL_API bool isValidFileWithExecutableFileSuffixes ( std::string name );

          ROSE_UTIL_API bool isCFileNameSuffix ( const std::string & suffix );

       // Liao (6/6/2008): Added support for UPC file suffix name    
          ROSE_UTIL_API bool isUPCFileNameSuffix ( const std::string & suffix );

          ROSE_UTIL_API bool isCppFileNameSuffix ( const std::string & suffix );

       // DQ (8/7/2007): Added support for Fortran file suffix names.
          ROSE_UTIL_API bool isFortranFileNameSuffix ( const std::string & suffix );

       // DQ (5/18/2008): Support to marking when C preprocessing is required for 
       // Fortran files, default is true for C and C++.
          ROSE_UTIL_API bool isFortranFileNameSuffixRequiringCPP ( const std::string & suffix );

       // DQ (11/17/2007): Added fortran mode specific suffix checking
          ROSE_UTIL_API bool isFortran77FileNameSuffix   ( const std::string & suffix );
          ROSE_UTIL_API bool isFortran90FileNameSuffix   ( const std::string & suffix );
          ROSE_UTIL_API bool isFortran95FileNameSuffix   ( const std::string & suffix );
          ROSE_UTIL_API bool isFortran2003FileNameSuffix ( const std::string & suffix );
          ROSE_UTIL_API bool isFortran2008FileNameSuffix ( const std::string & suffix );

       // DQ (1/23/2009): Added support for Co-Array Fortran file extension.
          ROSE_UTIL_API bool isCoArrayFortranFileNameSuffix ( const std::string & suffix );

          ROSE_UTIL_API bool isPHPFileNameSuffix ( const std::string & suffix );

          ROSE_UTIL_API bool isPythonFileNameSuffix ( const std::string & suffix );

       // TV (05/17/2010) Support for CUDA
          ROSE_UTIL_API bool isCudaFileNameSuffix ( const std::string & suffix );
       // TV (05/17/2010) Support for OpenCL
          ROSE_UTIL_API bool isOpenCLFileNameSuffix ( const std::string & suffix );

       // DQ (10/11/2010): Adding Java support.
          ROSE_UTIL_API bool isJavaFileNameSuffix ( const std::string & suffix );
          ROSE_UTIL_API bool isX10FileNameSuffix (const std::string& suffix);

          ROSE_UTIL_API void initSourceFileSuffixList();
          static Rose_STL_Container<std::string> validSourceFileSuffixes;
          
          ROSE_UTIL_API void initObjectFileSuffixList();
          static Rose_STL_Container<std::string> validObjectFileSuffixes;

          ROSE_DLL_API void initExecutableFileSuffixList();
          static Rose_STL_Container<std::string> validExecutableFileSuffixes;

       // bool isOptionTakingFileName( std::string argument );
          ROSE_DLL_API bool isOptionTakingSecondParameter( std::string argument );
          ROSE_DLL_API bool isOptionTakingThirdParameter ( std::string argument );
   };

// DQ (4/5/2010): This are defined in sage_support.cpp
//! Find the path of a ROSE support file.  If ROSE is not installed (see
//! roseInstallPrefix()), the top of the source tree plus sourceTreeLocation is
//! used as the location.  If the variable is not set, the path in
//! installTreeLocation (with the install prefix added) is used instead.
ROSE_DLL_API std::string
findRoseSupportPathFromSource(const std::string& sourceTreeLocation,
                              const std::string& installTreeLocation);

// DQ (4/5/2010): This are defined in sage_support.cpp
//! Find the path of a ROSE support file.  If ROSE is not installed (see
//! roseInstallPrefix()), the top of the build tree plus buildTreeLocation is
//! used as the location.  If the variable is not set, the path in
//! installTreeLocation (with the install prefix added) is used instead.
ROSE_DLL_API std::string
findRoseSupportPathFromBuild(const std::string& buildTreeLocation,
                             const std::string& installTreeLocation);

// DQ (4/5/2010): This are defined in sage_support.cpp
//! Find the path of the ROSE install prefix.  There is an assumption that
//! <directory containing librose>/.. is the prefix, and that other things can
//! be found from that.  This may not be true if the various install
//! directories are set by hand (rather than from $prefix).  This function
//! either puts the prefix into RESULT and returns true (for an installed copy
//! of ROSE), or returns false (for a build tree).
ROSE_DLL_API bool
roseInstallPrefix(std::string& result);

// endif associated with define ROSE_COMMANDLINE_UTILITY_H
#endif
