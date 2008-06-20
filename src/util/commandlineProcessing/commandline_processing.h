#ifndef ROSE_COMMANDLINE_UTILITY_H
#define ROSE_COMMANDLINE_UTILITY_H

#include "setup.h"

//Rama (12/22/2006): changing the class to a namespace and removing the "static"ness of the "member" functions

namespace CommandlineProcessing
   {
          /// Separate a string into individual parameters and store them into a string vector
          Rose_STL_Container<std::string> generateArgListFromString ( std::string commandline );
          /// Convert an arc-argv pair into a string vector
          Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, const char* argv[] );
          inline Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, char* argv[] ) {return generateArgListFromArgcArgv(argc, (const char**)argv);}
          /// Convert a string vector back to an arc-argv pair
          void generateArgcArgvFromList ( Rose_STL_Container<std::string> argList, int & argc, char** & argv );

          Rose_STL_Container<std::string> generateOptionList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix );

       //! Find all options with a form like inputprefix:option from argList, strip off inputprefix: and return a string list for options only.
       //! All matching inputprefix:option in argList are also removed.   
          Rose_STL_Container<std::string> generateOptionWithNameParameterList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix );

       //! Search 'argv' for an option like optionPrefix:option, remove the option if 'removeOption' is true.
       /*! 
        The argument 'option' adds () to the actual option, and allows the |(OR) operations.For example: 
              CommandlineProcessing::isOption(argv,"-rose:","(skip_syntax_check)",true)
              CommandlineProcessing::isOption(argv,"-rose:","(C99|C99_only)",false) 
       */       
          bool isOption ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, bool removeOption );

       //! Search 'argv' for 'optionPrefix:option value',  store the integer value into 'optionParameter'. Remove the original option if 'removeOption' is true.
       //! Available value types are: str, float, double, int, short, long, unsigned int, unsigned short, unsigned long, char, etc.
          bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, int & optionParameter, bool removeOption );

       //! Search 'argv' for 'optionPrefix:option value',  store the string type value into 'optionParameter'. Remove the original option if 'removeOption' is true.
          bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, std::string & optionParameter, bool removeOption );

      //! Add the strings in argList to the command line represented by argc and argv, prepend 'prefix' to each of the arguments
          void addListToCommandLine ( std::vector<std::string> & argv , std::string prefix, Rose_STL_Container<std::string> argList );

          void removeArgs ( std::vector<std::string> & argv, std::string prefix );
          void removeArgsWithParameters ( std::vector<std::string> & argv, std::string prefix );

          void removeAllFileNamesExcept ( std::vector<std::string> & argv, Rose_STL_Container<std::string> filenameList, std::string exceptFilename );

      //! Build a string from the argList
          std::string generateStringFromArgList ( Rose_STL_Container<std::string> argList, bool skipInitialEntry, bool skipSourceFiles );

      //! Build the list of isolated file names from the command line
          Rose_STL_Container<std::string> generateSourceFilenames ( Rose_STL_Container<std::string> argList );

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support auto-documentation.
      //! Add another valid source file suffix
          void addSourceFileSuffix ( const std::string &suffix );
          bool isSourceFilename ( std::string name ); 

          bool isObjectFilename ( std::string name );
          bool isExecutableFilename ( std::string name );

          bool isCFileNameSuffix ( const std::string & suffix );
      //Liao (6/6/2008): Added support for UPC file suffix name    
          bool isUPCFileNameSuffix ( const std::string & suffix );
          bool isCppFileNameSuffix ( const std::string & suffix );

       // DQ (8/7/2007): Added support for Fortran file suffix names.
          bool isFortranFileNameSuffix ( const std::string & suffix );

       // DQ (5/18/2008): Support to marking when C preprocessing is required for 
       // Fortran files, default is true for C and C++.
          bool isFortranFileNameSuffixRequiringCPP ( const std::string & suffix );

       // DQ (11/17/2007): Added fortran mode specific suffix checking
          bool isFortran77FileNameSuffix   ( const std::string & suffix );
          bool isFortran90FileNameSuffix   ( const std::string & suffix );
          bool isFortran95FileNameSuffix   ( const std::string & suffix );
          bool isFortran2003FileNameSuffix ( const std::string & suffix );
          bool isFortran2008FileNameSuffix ( const std::string & suffix );

          void initSourceFileSuffixList();
          static Rose_STL_Container<std::string> validSourceFileSuffixes;

          void initObjectFileSuffixList();
          static Rose_STL_Container<std::string> validObjectFileSuffixes;

          void initExecutableFileSuffixList();
          static Rose_STL_Container<std::string> validExecutableFileSuffixes;

          bool isOptionTakingFileName( std::string argument );
   };

//! Find the path of a ROSE support file.  If the environment variable
//! ROSE_IN_BUILD_TREE is set, the top of the source tree plus
//! sourceTreeLocation is used as the location.  If the variable is not set,
//! the path in installTreeLocation (with no prefix added) is used instead.
std::string
findRoseSupportPathFromSource(const std::string& sourceTreeLocation,
                              const std::string& installTreeLocation);

//! Find the path of a ROSE support file.  If the environment variable
//! ROSE_IN_BUILD_TREE is set, that path plus buildTreeLocation is used as
//! the location.  If the variable is not set, the path in
//! installTreeLocation (with no prefix added) is used instead.
std::string
findRoseSupportPathFromBuild(const std::string& buildTreeLocation,
                             const std::string& installTreeLocation);

// endif associated with define ROSE_COMMANDLINE_UTILITY_H
#endif
