#include <rose_config.h>
#include <rosePublicConfig.h>
#include <ROSE_ABORT.h>
#include <Rose/StringUtility.h>

#include <string.h>
#include <iostream>
#include <initializer_list>
#include <vector>
#include <algorithm>

#include "commandline_processing.h"
#include <Rose/Diagnostics.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <rose_paths.h>

// Use Brian Gunney's String List Assignent (SLA) library
#include "sla.h"

#ifdef _MSC_VER
# include <boost/lexical_cast.hpp>
#else
# include <time.h>
#endif


// DQ (12/31/2005): This is allowed in C files where it can not
// effect the users application (just not in header files).
using namespace std;
using namespace Rose;

Rose_STL_Container<std::string> CommandlineProcessing::extraCppSourceFileSuffixes;

Rose_STL_Container<string>
CommandlineProcessing::generateArgListFromString ( string commandline )
   {
     Rose_STL_Container<string> argList;

  // DQ (12/21/2006): Required to be long to avoid "if (subStringEnd == string::npos)" always evaluating to false.
     unsigned long int subStringStart = 0;
     unsigned long int subStringEnd   = commandline.find(" ");

     while (subStringStart < commandline.size())
        {
          string subString = commandline.substr(subStringStart,subStringEnd-subStringStart);
          subStringStart = subStringEnd + 1;
          subStringEnd   = commandline.find(" ",subStringStart);

          if (subStringEnd == string::npos)
             {
               subStringEnd = commandline.size();
             }

          argList.push_back(subString);
        }

     return argList;
   }

//! Convert a vector of string to a single string
std::string CommandlineProcessing::generateStringFromArgList( const Rose_STL_Container<std::string> & argList)
{
  string result;
  Rose_STL_Container<std::string>::const_iterator iter;
  for (iter = argList.begin(); iter != argList.end(); iter ++)
  {
    if (iter !=argList.begin())
      result += " ";
    result += *iter;
  }
  return result;
}

Rose_STL_Container<string>
CommandlineProcessing::generateArgListFromArgcArgv ( int argc, const char* argv[] )
   {
     Rose_STL_Container<string> argList;
     for (int i=0; i < argc; i++)
        {
          if (argv[i] != NULL)
               argList.push_back(argv[i]);
        }

     return argList;
   }

string
CommandlineProcessing::generateStringFromArgList ( Rose_STL_Container<string> argList, bool skipInitialEntry, bool skipSourceFiles )
   {
     string returnString;

     for (Rose_STL_Container<string>::iterator i = argList.begin();
          i != argList.end(); ++i) {
       if (skipInitialEntry && i == argList.begin()) continue;
       if (skipSourceFiles == true) {
               string arg    = *i;
               string suffix = "";
         if (arg.length() > 2) suffix = arg.substr(arg.size() - 2);
         if (suffix == ".C" || arg.find("--edg:definition_list_file") == 0) {
           continue;
         }
       }
       returnString += *i + " ";
     }
     return returnString;
   }

void
CommandlineProcessing::generateArgcArgvFromList ( Rose_STL_Container<string> argList, int & argc, char** & argv )
   {
  // Build the modified argc and argv (returned by reference)
     if (argv != NULL)
        {
           printf ("Error: argv input shoud be NULL! \n");
           ROSE_ABORT();
        }

#ifdef _MSC_VER
#define __builtin_constant_p(exp) (0)
#endif
     ROSE_ASSERT (argv == NULL);
     argc = argList.size();
     argv = (char**) malloc ((argc+1) * sizeof(char**));
     ROSE_ASSERT (argv != NULL);
     argv[argc] = NULL;
     for (int i=0; i < argc; i++)
        {
          string tempString = argList[i];
          int length = tempString.length();
          argv[i] = (char*) malloc ((length+1) * sizeof(char));
          strcpy(argv[i],tempString.c_str());
        }

   }

void
CommandlineProcessing::removeArgs ( vector<string> & argv, string prefix )
   {
     int prefixLength = prefix.length();
     for (unsigned int i=0; i < argv.size(); i++)
        {
          string argString = argv[i];
          if ( argString.substr(0,prefixLength) == prefix )
             {
            // keep track of elements so that they can be deleted later (after exit from loop over the eleents)
               argv.erase(argv.begin() + i);
               --i; // To counteract the i++ in the loop header
             }
        }
   }

void
CommandlineProcessing::removeArgsWithParameters ( vector<string> & argv, string prefix )
   {
     unsigned int prefixLength = prefix.length();
     for (unsigned int i=0; i < argv.size(); i++)
        {
          string argString = argv[i];

          if ( (argString.length() >= prefixLength) && argString.substr(0,prefixLength) == prefix )
             {
               ASSERT_require( i+1 < argv.size() );
               argv.erase(argv.begin() + i, argv.begin() + i + 2);
               --i; // To counteract the i++ in the loop header
             }
        }
   }

//! Remove file names specified in filenameList from argv, except for 'exceptFilename'
void
CommandlineProcessing::removeAllFileNamesExcept ( vector<string> & argv, Rose_STL_Container<std::string> filenameList, std::string exceptFilename )
{
    vector<string>::iterator argv_iter = argv.begin();
    while (argv_iter != argv.end())
    {
      string argString = *(argv_iter);
      bool shouldDelete = false;

      Rose_STL_Container<std::string>::iterator filenameIterator = filenameList.begin();
      while (filenameIterator != filenameList.end())
      {
        if ( argString == *filenameIterator )
        {
          if (*filenameIterator != exceptFilename)
          {
            shouldDelete = true;
            break;
          }
        }
        filenameIterator++;
      } // end while filename iterator

      if (shouldDelete)
      {
        //vector::erase() return a random access iterator pointing to the new location of the element that followed the last element erased by the function call
        //Essentially, it returns an iterator points to next element.
        argv_iter = argv.erase (argv_iter);
      }
      else
        argv_iter ++;
    } // end while argv_iter
}

Rose_STL_Container<string>
CommandlineProcessing::generateOptionList (const Rose_STL_Container<string> & argList, string inputPrefix)
   {
  // This function returns a list of options using the inputPrefix (with the
  // inputPrefix stripped off). It does NOT modify the argList passed as a reference.
     Rose_STL_Container<string> optionList;
     unsigned int prefixLength = inputPrefix.length();
     for (Rose_STL_Container<string>::const_iterator i = argList.begin(); i != argList.end(); i++)
        {
          if ( (*i).substr(0,prefixLength) == inputPrefix )
             {
            // get the rest of the string as the option
               string option = (*i).substr(prefixLength);
               optionList.push_back(option);
             }
        }
     return optionList;
   }

Rose_STL_Container<string>
CommandlineProcessing::generateOptionWithNameParameterList ( Rose_STL_Container<string> & argList, string inputPrefix , string newPrefix )
   {
  // This function returns a list of options using the inputPrefix (with the
  // inputPrefix stripped off and replaced if new Prefix is provided.
  // It also modified the input argList to remove matched options.

     Rose_STL_Container<string> optionList;
     Rose_STL_Container<string> deleteList;
     int prefixLength = inputPrefix.length();
     Rose_STL_Container<string>::iterator it = argList.begin();
     while (it != argList.end())
        {
         if ( it->substr(0,prefixLength) == inputPrefix )
            {
           // get the rest of the string as the option
              optionList.push_back( (newPrefix == "") ? it->substr(prefixLength) : newPrefix + it->substr(prefixLength));
              it = argList.erase(it);

           // That sounds real buggy as to detect if an option has parameters it
           // assumes inputPrefix-ed options are consecutive.
              if ( it->substr(0,prefixLength) != inputPrefix )
                 {
                   optionList.push_back(*it);
                   it = argList.erase(it);
                 }
                else
                 {
                   printf ("Error: missing parameter in option with parameter \n");
                   ROSE_ABORT();
                 }
            } else {
                ++it;
            }
        }

     return optionList;
   }


bool
CommandlineProcessing::isOption ( vector<string> & argv, string optionPrefix, string option, bool removeOption )
   {
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of argc
     int optionCount = sla(argv, optionPrefix, "($)", option, (string*)NULL, removeOption ? 1 : -1);
     return (optionCount > 0);
   }

// Available options are: string,float,double,int,short,long,unsigned int, unsigned short, unsigned long, char

// DQ (2/12/2004): All parameters are handled as strings so no type specialization is required
bool
CommandlineProcessing::isOptionWithParameter ( vector<string> & argv, string optionPrefix, string option, int & optionParameter, bool removeOption )
   {
     int localOptionParameter = 0;
     int optionCount = sla(argv, optionPrefix, "($)^", option, &localOptionParameter, removeOption ? 1 : -1);
     if (optionCount > 0)
          optionParameter = localOptionParameter;
     return (optionCount > 0);
   }
bool
CommandlineProcessing::isOptionWithParameter ( vector<string> & argv, string optionPrefix, string option, float & optionParameter, bool removeOption )
   {
     float localOptionParameter = 0;
     int optionCount = sla(argv, optionPrefix, "($)^", option, &localOptionParameter, removeOption ? 1 : -1);
     if (optionCount > 0)
          optionParameter = localOptionParameter;
     return (optionCount > 0);
   }

void
CommandlineProcessing::addListToCommandLine ( vector<string> & argv , string prefix, Rose_STL_Container<string> argList )
   {
     for (size_t i = 0; i < argList.size(); ++i)
        {
          argv.push_back(prefix + argList[i]);
        }
   }

bool
CommandlineProcessing::isSourceFilename(const std::string& name, bool isAdaProject)
{
    initSourceFileSuffixList();

 // Move to using a consistent mechanism for determining source file names
 // using CommandlineProcessing [Rasmussen 2024.04.19]

 // Replace boost::filesystem with std::filesystem in C++17
    string suffix = boost::filesystem::path(name).extension().string();
    if (suffix.size() && suffix[0] == '.') {
      // Remove '.' from filename extension
      suffix = suffix.substr(1, suffix.size()-1);
    }

    if (CommandlineProcessing::isAdaFileNameSuffix(suffix, isAdaProject)) return true;
    if (CommandlineProcessing::isFortranFileNameSuffix(suffix)) return true;
    if (CommandlineProcessing::isJovialFileNameSuffix(suffix)) return true;
    if (CommandlineProcessing::isJavaFile(name)) return true;

    int length = name.size();
    for (string &suffix : validSourceFileSuffixes)
       {
         int jlength = suffix.size();
         if ( (length > jlength) && (name.compare(length - jlength, jlength, suffix) == 0) )
            {
              return true;
            }
       }

    for (string &suffix : extraCppSourceFileSuffixes)
       {
         int jlength = suffix.size();
         if ( (length > jlength) && (name.compare(length - jlength, jlength, suffix) == 0) )
            {
              return true;
            }
       }

    return false;
}

bool
CommandlineProcessing::isObjectFilename ( string name )
   {
     initObjectFileSuffixList();

     int length = name.size();
     for (string &suffix : validObjectFileSuffixes)
        {
          int jlength = suffix.size();
          if ( (length > jlength) && (name.compare(length - jlength, jlength, suffix) == 0) )
             {
               return true;
             }
        }

     return false;
   }

void
CommandlineProcessing::addCppSourceFileSuffix ( const string &suffix )
   {
     extraCppSourceFileSuffixes.push_back(suffix);
   }


//Rama
//Also refer to the code in functions isCppFileNameSuffix  Dan and I added in StringUtility
//For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.
#ifndef CASE_SENSITIVE_SYSTEM
#define CASE_SENSITIVE_SYSTEM 1
#endif

bool
CommandlineProcessing::isCFileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid C source file name extension (suffix)
  // C source files conventionally use the suffixes ".c", C header files often use ".h".

     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.
     if (suffix == "c") {
         returnValue = true;
     }
     return returnValue;
   }

bool
CommandlineProcessing::isCppFileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid C++ source file name extension (suffix)

  // C++ source files conventionally use one of the suffixes .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx;
  // C++ header files often use .hh or .H; and preprocessed C++ files use the suffix .ii.  GCC
  // recognizes files with these names and compiles them as C++ programs even if you call the compiler
  // the same way as for compiling C programs (usually with the name gcc).

     bool returnValue = false;

//For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if (suffix == "C"
             || suffix == "cc"
             || suffix == "cp"
             || suffix == "c++"
             || suffix == "cpp"
             || suffix == "cxx"
             )
#else//It is a case insensitive system
     if (suffix == "cc"
             || suffix == "cp"
             || suffix == "c++"
             || suffix == "cpp"
             || suffix == "cxx"
             || suffix == "CC"
             || suffix == "CP"
             || suffix == "C++"
             || suffix == "CPP"
             || suffix == "CXX"
             )
#endif
          returnValue = true;

     if(find(extraCppSourceFileSuffixes.begin(), extraCppSourceFileSuffixes.end(),suffix) != extraCppSourceFileSuffixes.end())
     {
       returnValue = true;
     }


     return returnValue;
   }

bool
CommandlineProcessing::isFortranFileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Fortran source file name extension (suffix)

     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if (   suffix == "f"
         || suffix == "f77"
         || suffix == "f90"
         || suffix == "f95"
         || suffix == "f03"
         || suffix == "f08"
         || suffix == "caf"
      // For Fortran, upper case is used to indicate that CPP preprocessing is required.
         || suffix == "F"
         || suffix == "F77"
         || suffix == "F90"
         || suffix == "F95"
         || suffix == "F03"
         || suffix == "F08"
         || suffix == "CAF"
         || suffix == "rmod" //FMZ 5/28/2008: for importing module declaration
             )
#else//It is a case insensitive system
     if (   suffix == "f"
         || suffix == "f77"
         || suffix == "f90"
         || suffix == "f95"
         || suffix == "f03"
         || suffix == "f08"
         || suffix == "F"
         || suffix == "F77"
         || suffix == "F90"
         || suffix == "F95"
         || suffix == "F03"
         || suffix == "F08"
         || suffix == "CAF"
         || suffix == "rmod" //FMZ 5/28/2008: for importing module declaration
             )
#endif
          returnValue = true;

     return returnValue;
   }

bool
CommandlineProcessing::isFortranFileNameSuffixRequiringCPP ( const std::string & suffix )
   {
  // Returns false only if this is a Fortran source file not requireing C preprocessing (file extension (suffix) is *.f??)

     bool returnValue = true;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
  // For Fortran, upper case is used to indicate that CPP preprocessing is required.
     if (   suffix == "f"
         || suffix == "f77"
         || suffix == "f90"
         || suffix == "f95"
         || suffix == "f03"
         || suffix == "f08"
         || suffix == "caf"
         || suffix == "rmod" //FMZ (10/15/2008)
             )
          returnValue = false;
#else
  // It is a case insensitive system (assume that C preprocessing is required since I don't know how to tell the difference.
     returnValue = true;
#endif

     return returnValue;
   }

bool
CommandlineProcessing::isFortran77FileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Fortran 77 source file name extension (suffix)
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

  // DQ (12/10/2007): Fortran 77 codes are defined by a *.f suffix, not a *.F77 suffix.
  // But to make the system more robust, accept either as recognised F77 Fortran source.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "f" || suffix == "F" || suffix == "f77" || suffix == "F77" )
#else//It is a case insensitive system
     if ( suffix == "f" || suffix == "f77")
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isFortran90FileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Fortran 90 source file name extension (suffix)
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "f90" || suffix == "F90" || suffix == "rmod")
#else//It is a case insensitive system
     if ( suffix == "f90" || suffix == "rmod")
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isFortran95FileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Fortran 95 source file name extension (suffix)
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "f95" || suffix == "F95" )
#else//It is a case insensitive system
     if ( suffix == "f95" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isFortran2003FileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Fortran 2003 source file name extension (suffix)
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "f03" || suffix == "F03" )
#else//It is a case insensitive system
     if ( suffix == "f03" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isFortran2008FileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Fortran 2008 source file name extension (suffix)
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "f08" || suffix == "F08" )
#else
  // It is a case insensitive system
     if ( suffix == "f08" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isCoArrayFortranFileNameSuffix ( const std::string & suffix )
   {
  // Returns true only if this is a valid Co-Array Fortran (extension of Fortran 2003) source file name extension (suffix)
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "caf" || suffix == "CAF" )
#else//It is a case insensitive system
     if ( suffix == "caf" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isUPCFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "upc")
#else//It is a case insensitive system
     if ( suffix == "upc" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isPHPFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "php" )
#else//It is a case insensitive system
     if ( suffix == "php" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isPythonFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "py" )
#else//It is a case insensitive system
     if ( suffix == "py" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isCsharpFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "cs" )
#else //It is a case insensitive system
     if ( suffix == "cs" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isAdaFileNameSuffix ( const std::string& suffix, bool isAdaProject )
   {
  // Note that the filename extension is not defined as part of the Ada standard,
  // but GNAT (Gnu Ada) is using "ads" (for the spec) and "adb" (for the body).
  // PP (02/12/21): Other naming schemes exist (GNAT can be customized)
  //                RC-571 support Rational Apex .ada
  // PP (05/31/24): .a is another extension supported on some platforms.
  //                It often combines spec and body and may require preprocessing.
  //                It can only be turned on selectively if is_Ada_language is
  //                defined, in order to not conflict with .a library files
  //                that need to be passed to the linker.
  // PP (05/05/25)  pass in isAdaProject to enable Ada processing for files with extension ".a".
  //                .a files usually need to be chopped before we can process them in ROSE.
  //                Chopping is not yet implemented.
     static std::initializer_list<std::string> adaSuffixes   = { "adb", "ads", "ada" };
     static std::initializer_list<std::string> adaOnlySuffix = { "a" };

  // if !CASE_SENSITIVE_SYSTEM -> update isSuffix to use boost::iequals instead of ==
     auto isSuffix = [&suffix](const std::string& cand) { return suffix == cand; };

     return (  std::any_of(adaSuffixes.begin(), adaSuffixes.end(), isSuffix)
            || (isAdaProject && std::any_of(adaOnlySuffix.begin(), adaOnlySuffix.end(), isSuffix))
            );
   }

bool
CommandlineProcessing::isJovialFileNameSuffix(const std::string& suffix) {
    std::string lc{suffix};
    std::transform(lc.begin(), lc.end(), lc.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // Jovial file extension reflects usage found on web (lower and upper case, not mixed)
    if (lc == "jov" || lc == "cpl" || lc == "j73" || lc == "jovial" || lc == "rcmp") {
        return true;
    }
    return false;
}

bool
CommandlineProcessing::isCudaFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "cu" )
#else//It is a case insensitive system
     if ( suffix == "cu" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isOpenCLFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "ocl" || suffix == "cl" )
#else//It is a case insensitive system
     if ( suffix == "ocl" || suffix == "cl" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

bool
CommandlineProcessing::isJavaFile(const std::string &fileName) {
 // Replace boost::filesystem with std::filesystem in C++17
    if (boost::filesystem::path(fileName).extension() == ".java") {
        return true;
    }
    return false;
}

bool
CommandlineProcessing::isJavaClassFile(const std::string &fileName) {
 // Replace boost::filesystem with std::filesystem in C++17
    if (boost::filesystem::path(fileName).extension() == ".class") {
        return true;
    }
    return false;
}

bool
CommandlineProcessing::isJavaJarFile(const std::string &fileName) {
 // Replace boost::filesystem with std::filesystem in C++17
    if (boost::filesystem::path(fileName).extension() == ".jar") {
        return true;
    }
    return false;
}

bool
CommandlineProcessing::isJavaJvmFile(const std::string &fileName) {
    if (isJavaClassFile(fileName) || isJavaJarFile(fileName)) {
        return true;
    }
    return false;
}

void
CommandlineProcessing::initSourceFileSuffixList ( )
   {
  // For more notes on why some of these are rather bizzarely commented,
  // look at the function isCppFileNameSuffix and isFortranFileNameSuffix

     static bool first_call = true;

     if ( first_call == true )
        {
#if(CASE_SENSITIVE_SYSTEM == 1)
          validSourceFileSuffixes.push_back(".c");
          validSourceFileSuffixes.push_back(".cc");
          validSourceFileSuffixes.push_back(".cp");
          validSourceFileSuffixes.push_back(".c++");
          validSourceFileSuffixes.push_back(".cpp");
          validSourceFileSuffixes.push_back(".cxx");
          validSourceFileSuffixes.push_back(".C");

          validSourceFileSuffixes.push_back(".upc");
          validSourceFileSuffixes.push_back(".php");

          validSourceFileSuffixes.push_back(".py");

          validSourceFileSuffixes.push_back(".cu");
          validSourceFileSuffixes.push_back(".ocl");
          validSourceFileSuffixes.push_back(".cl");
#else
       // it is a case insensitive system
          validSourceFileSuffixes.push_back(".c");
          validSourceFileSuffixes.push_back(".cc");
          validSourceFileSuffixes.push_back(".cp");
          validSourceFileSuffixes.push_back(".c++");
          validSourceFileSuffixes.push_back(".cpp");
          validSourceFileSuffixes.push_back(".cxx");
          validSourceFileSuffixes.push_back(".C");
          validSourceFileSuffixes.push_back(".CC");
          validSourceFileSuffixes.push_back(".CP");
          validSourceFileSuffixes.push_back(".C++");
          validSourceFileSuffixes.push_back(".CPP");
          validSourceFileSuffixes.push_back(".CXX");

          validSourceFileSuffixes.push_back(".upc");
          validSourceFileSuffixes.push_back(".php");

          validSourceFileSuffixes.push_back(".py");

          validSourceFileSuffixes.push_back(".cu");
          validSourceFileSuffixes.push_back(".ocl");
          validSourceFileSuffixes.push_back(".cl");
#endif

          first_call = false;
        }
   }

// Initialization of static data member
//std::list<std::string> *CommandlineProcessing::validSourceFileSuffixes = NULL;


void
CommandlineProcessing::initObjectFileSuffixList ( )
   {
     static bool first_call = true;
     if ( first_call == true )
        {
#if(CASE_SENSITIVE_SYSTEM == 1)
          validObjectFileSuffixes.push_back(".o");
          validObjectFileSuffixes.push_back(".obj");
#else
       // it is a case insensitive system
          validObjectFileSuffixes.push_back(".O");
          validObjectFileSuffixes.push_back(".OBJ");
#endif
          first_call = false;
        }
   }
