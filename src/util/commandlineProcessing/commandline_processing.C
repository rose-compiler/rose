// #ifdef HAVE_CONFIG_H
// This avoids requiring the user to use rose_config.h and follows 
// the automake manual request that we use <> instead of ""
#include <rose_config.h>
// #endif

#include "string_functions.h"
#include <string.h>
#include <iostream>
#include "commandline_processing.h"
#include <vector>
#include <algorithm>

// Use Brian Gunney's String List Assignent (SLA) library
#include "sla.h"

// DQ (12/31/2005): This is allowed in C files where it can not 
// effect the users application (just not in header files).
using namespace std;

Rose_STL_Container<std::string> CommandlineProcessing::extraCppSourceFileSuffixes;

#if 0
// DQ (10/6/2008): (comment added) This function is replaced by:
// "bool CommandlineProcessing::isOptionTakingSecondParameter( string argument )"
// defined in sageSupport.C.

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
#endif

// DQ (7/8/2005): 
Rose_STL_Container<string>
CommandlineProcessing::generateArgListFromString ( string commandline )
   {
     Rose_STL_Container<string> argList;

  // DQ (12/21/2006): Required to be long to avoid "if (subStringEnd == string::npos)" always evaluating to false.
     unsigned long int subStringStart = 0;
     unsigned long int subStringEnd   = commandline.find(" ");

  // printf ("commandline.size() = %ld \n",commandline.size());
     while (subStringStart < commandline.size())
        {
          string subString = commandline.substr(subStringStart,subStringEnd-subStringStart);
       // printf ("subString (%ld,%ld) = %s \n",subStringStart,subStringEnd,subString.c_str());

       // DQ (8/1/2005): Fix suggested by Milind (supporting astMerge in compilation of multiple files)
       // subStringStart = subStringEnd;
       // subStringEnd   = commandline.find(" ",subStringStart+1);
          subStringStart = subStringEnd + 1;
          subStringEnd   = commandline.find(" ",subStringStart);

       // printf ("New values subStringStart = %ld subStringEnd = %ld \n",subStringStart,subStringEnd);
          if (subStringEnd == string::npos)
             {
               subStringEnd = commandline.size();
            // printf ("Reset subStringEnd = %ld \n",subStringEnd);
             }

          argList.push_back(subString);
        }

     return argList;
   }

//! Convert a vector of string to a single string
std::string CommandlineProcessing::generateStringFromArgList( Rose_STL_Container<std::string> & argList)
{
  string result; 
  Rose_STL_Container<std::string>::iterator iter;
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
  // printf ("In generateArgListFromArgcArgv(): argc = %d \n",argc);
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
                 // DQ (5/13/2004): It was not a great idea to put this filter into this function 
                 // remove it and handle the filtering of definition_list_file better ...  later!
	   continue;
             }
             }
       // returnString += *i;
       returnString += *i + " ";
        }

  // printf ("In generateStringFromArgList(): returnString = %s \n",returnString.c_str());

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
     argv = (char**) malloc (argc * sizeof(char**));
     ROSE_ASSERT (argv != NULL);
     for (int i=0; i < argc; i++)
        {
       // DQ (9/25/2007): Moved from std::list to std::vector.
       // string tempString = argList.front();
       // argList.pop_front();
          string tempString = argList[i];
       // argList.erase(argList.begin());

          int length = tempString.length();
          argv[i] = (char*) malloc ((length+1) * sizeof(char));
          strcpy(argv[i],tempString.c_str());
       // printf ("argv[%d] = %s \n",i,argv[i]);
        }

#if 0
     printf ("Modified argv (argc = %d): \n",argc);
     for (int i=0; i < argc; i++)
        {
          printf ("     argv[%d] = %s \n",i,argv[i]);
        }
#endif
   }

void
CommandlineProcessing::removeArgs ( vector<string> & argv, string prefix )
   {
     int prefixLength = prefix.length();

  // printf ("In CommandlineProcessing::removeArgs prefix = %s prefixLength = %d \n",prefix.c_str(),prefixLength);

     for (unsigned int i=0; i < argv.size(); i++)
        {
          string argString = argv[i];
       // printf ("argString = %s \n",argString.c_str());

          if ( argString.substr(0,prefixLength) == prefix )
             {
            // printf ("Found an option to remove (removeArgs): %s \n",argString.c_str());

            // keep track of elements so that they can be deleted later (after exit from loop over the eleents)
	       argv.erase(argv.begin() + i);
	       --i; // To counteract the i++ in the loop header
             }
        }

#if 0
     printf ("Display arg list! \n");
     Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("In removeArgs (at base): argv = \n%s \n",StringUtility::listToString(l).c_str());
#endif
   }


void
CommandlineProcessing::removeArgsWithParameters ( vector<string> & argv, string prefix )
   {
     unsigned int prefixLength = prefix.length();

  // printf ("In CommandlineProcessing::removeArgs prefix = %s prefixLength = %d \n",prefix.c_str(),prefixLength);

     for (unsigned int i=0; i < argv.size(); i++)
        {
          string argString = argv[i];

       // printf ("i = %d argString = %s \n",i,argString.c_str());

          if ( (argString.length() >= prefixLength) && argString.substr(0,prefixLength) == prefix )
             {
            // printf ("Found an option to remove (removeArgsWithParameters): %s \n",argString.c_str());
               ROSE_ASSERT ( i+1 < argv.size());

            // printf ("Argv[%d] = %s %s \n",i,argv[i],argv[i+1]);

	       argv.erase(argv.begin() + i, argv.begin() + i + 2);
	       --i; // To counteract the i++ in the loop header
             }
        }

#if 0
     printf ("Display arg list! \n");
     Rose_STL_Container<string> l = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
     printf ("In removeArgs (at base): argv = \n%s \n",StringUtility::listToString(l).c_str());
#endif
   }
//! Remove file names specified in filenameList from argv, except for 'exceptFilename'
void
CommandlineProcessing::removeAllFileNamesExcept ( vector<string> & argv, Rose_STL_Container<std::string> filenameList, std::string exceptFilename )
   {
#if 0
     printf ("In CommandlineProcessing::removeAllFileNamesExcept exceptFilename = %s \n",exceptFilename.c_str());
     printf ("In removeAllFileNamesExcept (at top): argv         = \n%s \n",StringUtility::listToString(argv).c_str());
     printf ("In removeAllFileNamesExcept (at top): filenameList = \n%s \n",StringUtility::listToString(filenameList).c_str());
#endif

     for (unsigned int i=0; i < argv.size(); i++)
        {
          string argString = argv[i];
#if 0
          printf ("i = %u argString = %s \n",i,argString.c_str());
#endif
          Rose_STL_Container<std::string>::iterator filenameIterator = filenameList.begin();
          while (filenameIterator != filenameList.end())
             {
#if 0
               printf ("filenameIterator = %s \n",filenameIterator->c_str());
#endif
            // DQ (1/17/2009): This is a match with filenameIterator = a.out and argString = a.out.new!
            // I think we only want to do anything about exact matches.
            // if ( argString.substr(0,filenameIterator->size()) == *filenameIterator )
               if ( argString == *filenameIterator )
                  {
#if 0
                    printf ("Found a file name (removeAllFileNamesExcept): %s \n",argString.c_str());
#endif
                    if (*filenameIterator != exceptFilename)
                       {
#if 0
                         printf ("*filenameIterator != exceptFilename so erase end of argv for i = %u \n",i);
#endif
                      // This is not an iterator invalidation error, but it is strange code!
                         argv.erase(argv.begin() + i);
                         --i; // To counteract the i++ in the loop header
#if 0
                         printf ("After erase: i = %u argv = \n%s \n",i,StringUtility::listToString(argv).c_str());
#endif
                       }
                  }

               filenameIterator++;
             }
        }

#if 0
     printf ("Leaving removeAllFileNamesExcept (at bottom): argv         = \n%s \n",StringUtility::listToString(argv).c_str());
#endif
   }

Rose_STL_Container<string>
CommandlineProcessing::generateOptionList ( Rose_STL_Container<string> & argList, string inputPrefix )
   {
  // This function returns a list of options using the inputPrefix (with the 
  // inputPrefix stripped off). It also modified the input argList to remove
  // those options from the argList returned by reference (modified).

  // printf ("Input argList = \n%s \n",StringUtility::listToString(argList).c_str());

     Rose_STL_Container<string> optionList;
  // Rose_STL_Container<string> deleteList;
     unsigned int prefixLength = inputPrefix.length();
     for (Rose_STL_Container<string>::iterator i = argList.begin(); i != argList.end(); i++)
        {
          if ( (*i).substr(0,prefixLength) == inputPrefix )
             {
            // get the rest of the string as the option
               optionList.push_back((*i).substr(prefixLength));

            // keep track of elements so that they can be deleted later (after exit from loop over the eleents)
            // deleteList.push_back(*i);
             }
        }
#if 0
  // remove the elements identified in the previous loop
     for (Rose_STL_Container<string>::iterator i = deleteList.begin(); i != deleteList.end(); i++)
        {
       // DQ (9/25/2007): Moved to use of std::vector instead of std::list
       // argList.remove(*i);
          argList.erase(i);
        }
#else
  // DQ (9/25/2007): Moved to use of std::vector instead of std::list
  // argList.erase(deleteList.begin(),deleteList.end());
  // argList.clear();
#endif

  // printf ("return value: optionList = \n%s \n",StringUtility::listToString(optionList).c_str());

     return optionList;
   }

Rose_STL_Container<string>
CommandlineProcessing::generateOptionWithNameParameterList ( Rose_STL_Container<string> & argList, string inputPrefix )
   {
  // This function returns a list of options using the inputPrefix (with the 
  // inputPrefix stripped off). It also modified the input argList to remove
  // those options from the argList returned by reference (modified).

  // printf ("Input argList = \n%s \n",StringUtility::listToString(argList).c_str());

     Rose_STL_Container<string> optionList;
     Rose_STL_Container<string> deleteList;
     int prefixLength = inputPrefix.length();
  // for (list<string>::iterator i = argList.begin(); i != argList.end(); i++)
     Rose_STL_Container<string>::iterator i = argList.begin(); 
     while (i != argList.end())
        {
          if ( (*i).substr(0,prefixLength) == inputPrefix )
             {
            // printf ("Found an option: %s \n",(*i).c_str());
            // get the rest of the string as the option
               optionList.push_back((*i).substr(prefixLength));

            // keep track of elements so that they can be deleted later (after exit from loop over the eleents)
               deleteList.push_back(*i);

               i++;
            // This is the name following the option (with out the prefix)
               if ( (*i).substr(0,prefixLength) != inputPrefix )
                  {
                 // printf ("Found associated option: %s \n",(*i).c_str());
                    string fullOption = optionList.back() + string(" ") + *i;
                    optionList.pop_back();
                    optionList.push_back(fullOption);
                    deleteList.push_back(*i);
                  }
                 else
                  {
                    printf ("Error: missing parameter in option with parameter \n");
                    ROSE_ABORT();
                  }
               
             }
          i++;
        }

#if 1
  // remove the elements identified in the previous loop
     for (Rose_STL_Container<string>::iterator i = deleteList.begin(); i != deleteList.end(); i++)
        {
       // DQ (9/25/2007): Moved to use of std::vector instead of std::list
       // argList.remove(*i);
          argList.erase(find(argList.begin(),argList.end(),*i));
        }
#else
  // DQ (9/25/2007): Moved to use of std::vector instead of std::list
  // argList.erase(deleteList.begin(),deleteList.end());
  // argList.clear();
#endif

  // printf ("return value: optionList = \n%s \n",StringUtility::listToString(optionList).c_str());

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
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of argc
  // int optionCount = sla(&argc, argv, optionPrefix.c_str(), "($)", option.c_str(), removeOption);
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


#if 0
bool
CommandlineProcessing::isOptionWithParameter ( vector<string> & argv, string optionPrefix, string option, string & optionParameter, bool removeOption )
   {
  // I could not make this work cleanly with valgrind withouth allocatting memory twice
     string localString;
     int optionCount = sla(argv, optionPrefix, "($)^", option, &localString, removeOption ? 1 : -1);
     if (optionCount > 0)
          optionParameter = localString;

     return (optionCount > 0);
   }
#endif

void
CommandlineProcessing::addListToCommandLine ( vector<string> & argv , string prefix, Rose_STL_Container<string> argList )
        {
     for (unsigned int i = 0; i < argList.size(); ++i) {
       argv.push_back(prefix + argList[i]);
   }
   }

#if 0
Rose_STL_Container<string>
CommandlineProcessing::generateSourceFilenames ( Rose_STL_Container<string> argList, bool binaryMode )
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
          if ( ((*i)[0] != '-') && ((*i)[0] != '+') )
             {
            // printf ("In CommandlineProcessing::generateSourceFilenames(): Look for file names:  argv[%d] = %s length = %zu \n",counter,(*i).c_str(),(*i).size());

            // bool foundSourceFile = false;

               if ( isSourceFilename(*i) == false && isExecutableFilename(*i) == true )
                  {
                 // printf ("This is an executable file: *i = %s \n",(*i).c_str());
                 // executableFileList.push_back(*i);
                    sourceFileList.push_back(*i);
                  }
            // PC (4/27/2006): Support for custom source file suffixes
            // if ( isSourceFilename(*i) )
               if ( isObjectFilename(*i) == false && isSourceFilename(*i) == true )
                  {
                 // printf ("This is a source file: *i = %s \n",(*i).c_str());
                 // foundSourceFile = true;
                    sourceFileList.push_back(*i);
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

          counter++;
          i++;
        }

     return sourceFileList;
   }
#endif

// PC (4/27/2006): Support for custom source file suffixes
bool
CommandlineProcessing::isSourceFilename ( string name )
   {
     initSourceFileSuffixList();

     int length = name.size();
     for ( Rose_STL_Container<string>::iterator j = validSourceFileSuffixes.begin(); j != validSourceFileSuffixes.end(); j++ )
        {
          int jlength = (*j).size();
          if ( (length > jlength) && (name.compare(length - jlength, jlength, *j) == 0) )
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
     for ( Rose_STL_Container<string>::iterator j = validObjectFileSuffixes.begin(); j != validObjectFileSuffixes.end(); j++ )
        {
          int jlength = (*j).size();
          if ( (length > jlength) && (name.compare(length - jlength, jlength, *j) == 0) )
             {
               return true;
             }
        }

     return false;
   }

void
CommandlineProcessing::addSourceFileSuffix ( const string &suffix )
   {
  // DQ (8/7/2007): This function appears to be used only in the projects/DocumentationGenerator project.
     initSourceFileSuffixList();
     validSourceFileSuffixes.push_back(suffix);
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

     if (suffix == "c")
          returnValue = true;

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
//It seems that the upper-case versions of the above should also be accepted.
//However, it does not look like GNU-g++ accepts them.
//So, I am commenting them out
             /*
             || suffix == "CC"  
             || suffix == "CPP" 
             || suffix == "C++" 
             || suffix == "CP"  
             || suffix == "CXX"
             */
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
  // DQ (11/17/2007): Added fortran mode specific suffix checking

  // Returns true only if this is a valid Fortran 77 source file name extension (suffix)

     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

  // DQ (12/10/2007): Fortran 77 codes are defined by a *.f suffix, not a *.F77 suffix.
  // But to make the system more robust, accept either as recognised F77 Fortran source.

#if(CASE_SENSITIVE_SYSTEM == 1)
  // if ( suffix == "f77" || suffix == "F77" )
  // if ( suffix == "f" || suffix == "F" )
     if ( suffix == "f" || suffix == "F" || suffix == "f77" || suffix == "F77" )
#else//It is a case insensitive system
  // if ( suffix == "f77" )
  // if ( suffix == "f" )
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
  // DQ (11/17/2007): Added fortran mode specific suffix checking

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
  // DQ (11/17/2007): Added fortran mode specific suffix checking

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
  // DQ (11/17/2007): Added fortran mode specific suffix checking

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
  // DQ (11/17/2007): Added fortran mode specific suffix checking

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
  // DQ (1/23/2009): Added Co-Array fortran mode specific suffix checking

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

//Liao, 6/6/2008 Support for UPC
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
   
// TV (05/17/2010) Support for CUDA
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

// TV (05/17/2010) Support for OpenCL
bool
CommandlineProcessing::isOpenCLFileNameSuffix ( const std::string & suffix )
   {
     bool returnValue = false;

  // For now define CASE_SENSITIVE_SYSTEM to be true, as we are currently a UNIXish project.

#if(CASE_SENSITIVE_SYSTEM == 1)
     if ( suffix == "ocl" )
#else//It is a case insensitive system
     if ( suffix == "ocl" )
#endif
        {
          returnValue = true;
        }

     return returnValue;
   }

void
CommandlineProcessing::initSourceFileSuffixList ( )
   {
  // For more notes on why some of these are rather bizzarely commented,
  // look at the function isCppFileNameSuffix and isFortranFileNameSuffix

     static bool first_call = true;

     if ( first_call == true )
        {
       // DQ (1/5/2008): For a binary (executable) file, no suffix is a valid suffix, so allow this case
       // validSourceFileSuffixes.push_back("");

#if(CASE_SENSITIVE_SYSTEM == 1)
          validSourceFileSuffixes.push_back(".c");
          validSourceFileSuffixes.push_back(".cc");
          validSourceFileSuffixes.push_back(".cp");
          validSourceFileSuffixes.push_back(".c++");
          validSourceFileSuffixes.push_back(".cpp");
          validSourceFileSuffixes.push_back(".cxx");
          validSourceFileSuffixes.push_back(".C");
          validSourceFileSuffixes.push_back(".f");
          validSourceFileSuffixes.push_back(".f77");
          validSourceFileSuffixes.push_back(".f90");
          validSourceFileSuffixes.push_back(".f95");
          validSourceFileSuffixes.push_back(".f03");
          validSourceFileSuffixes.push_back(".f08");
          validSourceFileSuffixes.push_back(".caf");
     /*
          validSourceFileSuffixes.push_back(".CC");
          validSourceFileSuffixes.push_back(".CP");
          validSourceFileSuffixes.push_back(".C++");
          validSourceFileSuffixes.push_back(".CXX");
          validSourceFileSuffixes.push_back(".CPP");
     */
          validSourceFileSuffixes.push_back(".F");
          validSourceFileSuffixes.push_back(".F77");
          validSourceFileSuffixes.push_back(".F90");
          validSourceFileSuffixes.push_back(".F95");
          validSourceFileSuffixes.push_back(".F03");
          validSourceFileSuffixes.push_back(".F08");
          validSourceFileSuffixes.push_back(".CAF");
          validSourceFileSuffixes.push_back(".rmod"); //FMZ 5/28/2008

     // Liao (6/6/2008)  Support for UPC   
          validSourceFileSuffixes.push_back(".upc");
          validSourceFileSuffixes.push_back(".php");
     // TV (05/17/2010) Support for CUDA
          validSourceFileSuffixes.push_back(".cu");
     // TV (05/17/2010) Support for OpenCL
          validSourceFileSuffixes.push_back(".ocl");
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
          validSourceFileSuffixes.push_back(".f");
          validSourceFileSuffixes.push_back(".f77");
          validSourceFileSuffixes.push_back(".f90");
          validSourceFileSuffixes.push_back(".f95");
          validSourceFileSuffixes.push_back(".f03");
          validSourceFileSuffixes.push_back(".f08");
          validSourceFileSuffixes.push_back(".caf");
          validSourceFileSuffixes.push_back(".F");
          validSourceFileSuffixes.push_back(".F77");
          validSourceFileSuffixes.push_back(".F90");
          validSourceFileSuffixes.push_back(".F95");
          validSourceFileSuffixes.push_back(".F03");
          validSourceFileSuffixes.push_back(".F08");
          validSourceFileSuffixes.push_back(".CAF");
          validSourceFileSuffixes.push_back(".rmod"); //FMZ 5/28/2008
          validSourceFileSuffixes.push_back(".upc");
          validSourceFileSuffixes.push_back(".php");
     // TV (05/17/2010) Support for CUDA
          validSourceFileSuffixes.push_back(".cu");
     // TV (05/17/2010) Support for OpenCL
          validSourceFileSuffixes.push_back(".ocl");
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

#if 0
void
CommandlineProcessing::initExecutableFileSuffixList ( )
   {
     static bool first_call = true;

     if ( first_call == true )
        {
       // DQ (1/5/2008): For a binary (executable) file, no suffix is a valid suffix, so allow this case
          validExecutableFileSuffixes.push_back("");

#if(CASE_SENSITIVE_SYSTEM == 1)
          validExecutableFileSuffixes.push_back(".exe");
#else
       // it is a case insensitive system
          validExecutableFileSuffixes.push_back(".EXE");
#endif
          first_call = false;
        }
   }
#endif









