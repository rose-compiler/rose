#ifndef ROSE_COMMANDLINE_UTILITY_H
#define ROSE_COMMANDLINE_UTILITY_H

#include "setup.h"

//Rama (12/22/2006): changing the class to a namespace and removing the "static"ness of the "member" functions

namespace CommandlineProcessing
   {
          Rose_STL_Container<std::string> generateArgListFromString ( std::string commandline );

          Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, const char* argv[] );
          inline Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, char* argv[] ) {return generateArgListFromArgcArgv(argc, (const char**)argv);}

          void generateArgcArgvFromList ( Rose_STL_Container<std::string> argList, int & argc, char** & argv );

          Rose_STL_Container<std::string> generateOptionList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix );
          Rose_STL_Container<std::string> generateOptionWithNameParameterList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix );

          bool isOption ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, bool removeOption );

       // Available options are: str, float, double, int, short, long, unsigned int, unsigned short, unsighed long, char
          bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, int & optionParameter, bool removeOption );

          bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, std::string & optionParameter, bool removeOption );

      //! Add the strings in argList to the command line represented by argc and argv
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
          bool isCppFileNameSuffix ( const std::string & suffix );

       // DQ (8/7/2007): Added support for Fortran file suffix names.
          bool isFortranFileNameSuffix ( const std::string & suffix );

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

// endif associated with define ROSE_COMMANDLINE_UTILITY_H
#endif
