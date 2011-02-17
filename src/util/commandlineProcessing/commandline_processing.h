#ifndef ROSE_COMMANDLINE_UTILITY_H
#define ROSE_COMMANDLINE_UTILITY_H
#include "setup.h"
//#include "rosedefs.h"
#include <list>
#define Rose_STL_Container std::vector

//Rama (12/22/2006): changing the class to a namespace and removing the "static"ness of the "member" functions

namespace CommandlineProcessing
   {
      //! Separate a string into individual parameters and store them into a string vector
          Rose_STL_Container<std::string> generateArgListFromString ( std::string commandline );

      //! Convert a vector of string to a single string
       // std::string generateStringFromArgList( Rose_STL_Container<std::string> & argList);
          std::string generateStringFromArgList( const Rose_STL_Container<std::string> & argList);

          //! Convert an argc-argv pair into a string vector
          Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, const char* argv[] );
          
          inline Rose_STL_Container<std::string> generateArgListFromArgcArgv ( int argc, char* argv[] ) 
          {return generateArgListFromArgcArgv(argc, (const char**)argv);}
          
          //! Convert a string vector back to an argc-argv pair
          void generateArgcArgvFromList ( Rose_STL_Container<std::string> argList, int & argc, char** & argv );

          Rose_STL_Container<std::string> generateOptionList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix );

       //! Find all options with a form like inputprefix:option from argList, strip off inputprefix: and return a string list for options only.
       //! All matching inputprefix:option in argList are also removed.   
          Rose_STL_Container<std::string> generateOptionWithNameParameterList ( Rose_STL_Container<std::string> & argList, std::string inputPrefix );

          extern Rose_STL_Container<std::string> extraCppSourceFileSuffixes;

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

       //! Search 'argv' for 'optionPrefix:option value',  store the float value into 'optionParameter'. Remove the original option if 'removeOption' is true.
          bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, float & optionParameter, bool removeOption );

       //! Search 'argv' for 'optionPrefix:option value',  store the string type value into 'optionParameter'. Remove the original option if 'removeOption' is true.
          bool isOptionWithParameter ( std::vector<std::string> & argv, std::string optionPrefix, std::string option, std::string & optionParameter, bool removeOption );

      //! Add the strings in argList to the command line represented by argc and argv, prepend 'prefix' to each of the arguments
          void addListToCommandLine ( std::vector<std::string> & argv , std::string prefix, Rose_STL_Container<std::string> argList );
      //! Remove all options matching a specified prefix 'prefix' from the argument list 'argv'  
          void removeArgs ( std::vector<std::string> & argv, std::string prefix );
      //! Remove all options matching a specified prefix 'prefix' from the argument list 'argv', as well as the option values following them
          void removeArgsWithParameters ( std::vector<std::string> & argv, std::string prefix );
       //! Remove file names specified in filenameList from argv, except for 'exceptFilename'
          void removeAllFileNamesExcept ( std::vector<std::string> & argv, Rose_STL_Container<std::string> filenameList, std::string exceptFilename );

      //! Build a string from the argList
          std::string generateStringFromArgList ( Rose_STL_Container<std::string> argList, bool skipInitialEntry, bool skipSourceFiles );

      //! Build the list of isolated file names from the command line
          Rose_STL_Container<std::string> generateSourceFilenames ( Rose_STL_Container<std::string> argList, bool binaryMode );

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support auto-documentation.
      //! Add another valid source file suffix
          void addSourceFileSuffix ( const std::string &suffix );
          void addCppSourceFileSuffix ( const std::string &suffix );

          bool isSourceFilename ( std::string name ); 

          bool isObjectFilename ( std::string name );
          bool isExecutableFilename ( std::string name );

       // DQ (8/20/2008): Added test that will allow bogus exececutable files (marked as .exe but not executable) to pass
          bool isValidFileWithExecutableFileSuffixes ( std::string name );

          bool isCFileNameSuffix ( const std::string & suffix );

       // Liao (6/6/2008): Added support for UPC file suffix name    
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

       // DQ (1/23/2009): Added support for Co-Array Fortran file extension.
          bool isCoArrayFortranFileNameSuffix ( const std::string & suffix );

          bool isPHPFileNameSuffix ( const std::string & suffix );

       // TV (05/17/2010) Support for CUDA
          bool isCudaFileNameSuffix ( const std::string & suffix );
       // TV (05/17/2010) Support for OpenCL
          bool isOpenCLFileNameSuffix ( const std::string & suffix );

       // DQ (10/11/2010): Adding Java support.
          bool isJavaFileNameSuffix ( const std::string & suffix );

          void initSourceFileSuffixList();
          static Rose_STL_Container<std::string> validSourceFileSuffixes;
          
          void initObjectFileSuffixList();
          static Rose_STL_Container<std::string> validObjectFileSuffixes;

          void initExecutableFileSuffixList();
          static Rose_STL_Container<std::string> validExecutableFileSuffixes;

       // bool isOptionTakingFileName( std::string argument );
          bool isOptionTakingSecondParameter( std::string argument );
          bool isOptionTakingThirdParameter ( std::string argument );
   };

// DQ (4/5/2010): This are defined in sageSupport.C
//! Find the path of a ROSE support file.  If ROSE is not installed (see
//! roseInstallPrefix()), the top of the source tree plus sourceTreeLocation is
//! used as the location.  If the variable is not set, the path in
//! installTreeLocation (with the install prefix added) is used instead.
std::string
findRoseSupportPathFromSource(const std::string& sourceTreeLocation,
                              const std::string& installTreeLocation);

// DQ (4/5/2010): This are defined in sageSupport.C
//! Find the path of a ROSE support file.  If ROSE is not installed (see
//! roseInstallPrefix()), the top of the build tree plus buildTreeLocation is
//! used as the location.  If the variable is not set, the path in
//! installTreeLocation (with the install prefix added) is used instead.
std::string
findRoseSupportPathFromBuild(const std::string& buildTreeLocation,
                             const std::string& installTreeLocation);

// DQ (4/5/2010): This are defined in sageSupport.C
//! Find the path of the ROSE install prefix.  There is an assumption that
//! <directory containing librose>/.. is the prefix, and that other things can
//! be found from that.  This may not be true if the various install
//! directories are set by hand (rather than from $prefix).  This function
//! either puts the prefix into RESULT and returns true (for an installed copy
//! of ROSE), or returns false (for a build tree).
bool
roseInstallPrefix(std::string& result);

// endif associated with define ROSE_COMMANDLINE_UTILITY_H
#endif
