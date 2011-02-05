// This avoids requiring the user to use rose_config.h and follows 
// the automake manual request that we use <> instead of ""
#include <rose_config.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"

// DQ (1/21/2010): Use this to turn off the use of #line in ROSETTA generated code.
#define SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES


#if !ROSE_MICROSOFT_OS
// AS added to support the function getAbsolutePathFromRelativePath
#include <sys/param.h>
#else
#include <windows.h>
#include "Shlwapi.h"

#endif
#include <algorithm>
// AS added to support the function findfile
#include <stdlib.h>
#include <stdio.h>              /* standard input/output routines.    */

#if !ROSE_MICROSOFT_OS
#include <dirent.h>             /* readdir(), etc.                    */
#include <sys/stat.h>           /* stat(), etc.                       */
#include <libgen.h>             /* basename(), dirame()               */
#include <unistd.h>             /* getcwd(), etc.                     */
#endif

#include <string.h>             /* strstr(), etc.                     */

#include <iostream>              /* std::cerr */
#include <sstream>              /* std::ostringstream */
#include <fstream>

#include "string_functions.h"

// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE
#include <boost/lexical_cast.hpp>
// #endif

// DQ (9/29/2006): This is required for 64-bit g++ 3.4.4 compiler.
#include <errno.h>

// tps (11/10/2009): This include is needed in windows to find the realpath
#if ROSE_MICROSOFT_OS
#include <unistd.h>
// DQ (11/27/2009): this is required for use of GetFullPathName() (below).
#include <windows.h>
#endif
// DQ (12/31/2005): This is allowed in C files where it can not 
// effect the users applcation (just not in header files).
using namespace std;


/* Added htmlEscape necessary for QROSE work to this utility library - tps (9Oct2008) */
std::string 
StringUtility::htmlEscape(const std::string& s) {
  std::string s2;
  for (size_t i = 0; i < s.size(); ++i) {
    switch (s[i]) {
    case '<': s2 += "&lt;"; break;
    case '>': s2 += "&gt;"; break;
    case '&': s2 += "&amp;"; break;
    default: s2 += s[i]; break;
    }
  }
              return s2;
}


std::list<std::string> 
StringUtility::findfile(std::string patternString, std::string pathString)
   {
     std::list<std::string> patternMatches;

#if ROSE_MICROSOFT_OS
         printf ("Error: MSVS implementation of StringUtility::findfile required (not implemented) \n");
#define __builtin_constant_p(exp) (0)
         // tps: todo Windows: have not hit this assert yet.
         ROSE_ASSERT(false);
#else
     DIR* dir;                        /* pointer to the scanned directory. */
     struct dirent* entry;      /* pointer to one directory entry.   */
  // struct stat dir_stat; /* used by stat().                   */
    
  /* open the directory for reading */
     dir = opendir(pathString.c_str());
     if (!dir) {
          std::cerr << "Cannot read directory:" << pathString << std::endl;
          perror("");
          return patternMatches;
     }

  /* scan the directory, traversing each sub-directory, and */
  /* matching the pattern for each file name.               */
     while ((entry = readdir(dir))) {
       /* check if the pattern matchs. */
          std::string entryName = entry->d_name ? entry->d_name : "";
          if (entry->d_name && entryName.find(patternString) != std::string::npos) {
               patternMatches.push_back(pathString+"/"+entryName);

          }

     }
#endif
     return patternMatches;
   }


//Read all the words in a file into an vector of strings
std::vector<std::string> 
StringUtility::readWordsInFile( std::string filename)
   {
     std::vector<std::string> variantsToUse;
     std::fstream file_op(filename.c_str());
     if (file_op.fail()) {
          std::cout << "error: could not find file \"" << filename 
                   << "\" which is meant to include the styles to enforce with " 
                   << "the name checker." << std::endl;
                                  exit(1);    // abort program
                                      
                                  }

     std::string current_word;

     while(file_op >> current_word){
       //First word denotes what the regular expression should operate
       //upon. Second word denotes the regular expression
       variantsToUse.push_back(current_word);
     }

     return variantsToUse;
   }

//
//Rama: 12/06/06
//We need a function getAbsolutePathFromRelativePath that takes any filename and returns the absolute file name  (with path)
//AS suggested we use realpath that comes with stdlib. However, for a translator like ours,
//we need to have two versions of the file: silent and non-silent ones, depending on whether 
//we need to check and print an error or not.
//That is done by the boolean parameter printErrorIfAny that is set to true by the caller
//The dafault version -- enforced by setting printErrorIfAny to false -- is the silent one.
//
//Also, look at the code added in function 
//void SgFile::setupSourceFilename in file .../src/ROSETTA/Grammar/Support.code
//

std::string
StringUtility::getAbsolutePathFromRelativePath ( const std::string & relativePath, bool printErrorIfAny) //! get the absolute path from the relative path
   {
     string returnString;
     char resolved_path[MAXPATHLEN];
     resolved_path[0] = '\0';

#if ROSE_MICROSOFT_OS
         // tps (08/19/2010): added this function
         PathCanonicalize(resolved_path,relativePath.c_str());
         string resultingPath=string(resolved_path);
#else
  // DQ (9/3/2006): Note that "realpath()" 
  // can return an error if it processes a file or directory that does not exist.  This is 
  // a problem for include paths that are specified on the commandline and which don't exist; 
  // most compilers silently ignore these and we have to at least ignore them.
         //      string resultingPath="";
         // tps (01/08/2010) : This implementation was incorrect as it mixed char* and string. Fixed it.
         char* rp = realpath( relativePath.c_str(), resolved_path);
         string resultingPath = "";
         if (rp!=NULL)
           resultingPath = string(rp);
#endif

         //printf("resultingPath == %s    printErrorIfAny == %d \n",resultingPath.c_str(),printErrorIfAny);
  // If there was an error then resultingPath is NULL, else it points to resolved_path.
     if ( resultingPath.empty() == true ) //== NULL )
        {
       // DQ (9/4/2006): SgProject is not available within this code since it is used to compile 
       // ROSETTA before the IR nodes are defined!  So we should just comment it out.
       // DQ (9/4/2006): Only output such warnings if verbose is set to some value greater than zero.
            if(printErrorIfAny == true)
            {
          //if (SgProject::get_verbose() > 0)
             //{
            // Output the kind of error that occured ...
            //Ask DAN and add checks for 64 bit machines here
            //extern int errno; 
            // Output the kind of error that occured ...  
               printf ("relativePath = %s errno = %d resolved_path is undefined \n",relativePath.c_str(),errno);
               printf ("     error = %s \n",strerror(errno));

            // In case of error return the original relativePath
               printf ("Error: StringUtility::getAbsolutePathFromRelativePath incured an error in use of realpath() and is returning the input relativePath. \n");
             //}
            }
        // printf("returnString0 == %s    relativePath == %s   resolved_path == %s \n",returnString.c_str(),relativePath.c_str(),resolved_path);
                returnString = relativePath;
        }
       else
        {
       // "realpath()" worked so return the corrected absolute path.
        // printf("returnString1 == %s    relativePath == %s   resolved_path == %s \n",returnString.c_str(),relativePath.c_str(),resolved_path);
          returnString = resolved_path;
        }

     //printf("returnString3 == %s    relativePath == %s   resolved_path == %s \n",returnString.c_str(),relativePath.c_str(),resolved_path);

         ROSE_ASSERT(returnString.empty() == false);

     return returnString;
   }

string
StringUtility::listToString ( const list<string> & X, bool separateStrings )
   {
  // Build a string representing the concatination of the list of strings

     string returnString;
  // printf ("In listToString: Print out the list of variable names (X.size() = %zu): \n",X.size());
     list<string>::const_iterator listStringElementIterator;
     for (listStringElementIterator = X.begin(); 
                     listStringElementIterator != X.end();
                     listStringElementIterator++)
        {
       // display each string representing a variable name
#if 0
          printf ("     string element (length=%d) in nameStringList = %s \n",
                          (*listStringElementIterator).length(),
                          (*listStringElementIterator).c_str());
#endif
       // returnString += (*listStringElementIterator) + "\n";
          returnString += *listStringElementIterator + " ";
          if (separateStrings)
               returnString += "\n";
        }

  // printf ("In listToString: returnString = %s \n",returnString.c_str());
     return returnString;
   }

string
StringUtility::listToString ( const vector<string> & X, bool separateStrings )
   {
  // Build a string representing the concatination of the vector of strings
     string returnString;
     vector<string>::const_iterator listStringElementIterator;
     for (listStringElementIterator = X.begin(); 
                     listStringElementIterator != X.end();
                     listStringElementIterator++)
        {
          returnString += *listStringElementIterator + " ";
          if (separateStrings)
               returnString += "\n";
        }

     return returnString;
   }

list<string>
StringUtility::stringToList ( const string & X )
   {
  // Build a list of strings representing the input string spearated at newline characters ("\n"'s)
  // All CR are removed, the list returned has no CR in the list OR within the list elements

  // Create a return value
     list<string> returnStringList;

  // Build a local copy of the input string
     string remainingSubstring = X;

     int currentPos = 0;
  // std::string::size_type nextPos = remainingSubstring.find('\n');
     size_t nextPos = remainingSubstring.find('\n');
  // int subStringLength = 0;

#if 0
     printf ("Initial value: currentPos = %d nextPos = %d remainingSubstring.length() = %d \n",
                     currentPos,nextPos,remainingSubstring.length());
#endif

  // If there is no trailing '\n' then at least include the input X string into the list
     if (nextPos == string::npos)
          returnStringList.push_back(X);

     while (nextPos != string::npos)
        {
          int nextSubStringLength = (nextPos - currentPos) + 0;
          string substring = remainingSubstring.substr(0,nextSubStringLength+0);
#if 0
          printf ("In stringToList: substring = [%s] \n",substring.c_str());
#endif
          returnStringList.push_back(substring);
          remainingSubstring =
                  remainingSubstring.substr(nextPos+1,(remainingSubstring.length()-nextSubStringLength)-0);
#if 0
          printf ("nextSubStringLength = %d substring = %s\n",nextSubStringLength,substring.c_str());
          printf ("remainingSubstring.length() = %d remainingSubstring = \n%s \n",
                          remainingSubstring.length(),remainingSubstring.c_str());
#endif
          currentPos = 0;
          nextPos = remainingSubstring.find('\n');
#if 0
          printf ("Value in loop: currentPos = %d nextPos = %d \n",currentPos,nextPos);
#endif
        }

  // Remove any strings consisting of only CRs and null strings saved within the list
     returnStringList.remove("\n");
     returnStringList.remove("");

#if 0
     printf ("Exiting in StringUtility::stringToList \n");
     ROSE_ABORT();
#endif

  // return by value (copy constructor will be called)
     return returnStringList;
   }

string
StringUtility::listToString ( const list<int> & X, bool separateStrings )
   {
  // Build a string representing the concatenation of the list of strings

     string returnString;
     list<int>::const_iterator listStringElementIterator;
     for (listStringElementIterator  = X.begin(); 
                     listStringElementIterator != X.end();
                     listStringElementIterator++)
        {
       // display each string representing a number
          returnString += numberToString(*listStringElementIterator) + " ";
          if (separateStrings)
               returnString += "\n";
        }

     return returnString;
   }
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifdef USE_ROSE   
#if 0
// Liao, 2/11/2009, Alternative to boost::lexical_cast,
// since ROSE has problem in compiling it. Bug 313
// https://outreach.scidac.gov/tracker/index.php?func=detail&aid=313&group_id=24&atid=185
template <typename T>
static std::string numToString(T x)
   {
     std::ostringstream os;
     os << x;
     return os.str();
   }
#endif

string
StringUtility::numberToString ( long long x )
   {
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE     
     return boost::lexical_cast<std::string>(x);
// #else
//   return numToString<long long>(x);
//#endif
   }

string
StringUtility::numberToString ( unsigned long long x )
   {
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE     
     return boost::lexical_cast<std::string>(x);
// #else     
//   return numToString<unsigned long long >(x);
// #endif     
   }

string
StringUtility::numberToString ( long x )
   {
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE     
     return boost::lexical_cast<std::string>(x);
// #else     
//   return numToString<long>(x);
// #endif     
   }

string
StringUtility::numberToString ( unsigned long x )
   {
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE     
     return boost::lexical_cast<std::string>(x);
// #else     
//   return numToString<unsigned long>(x);
// #endif     
   }

string
StringUtility::numberToString ( int x )
   {
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE     
     return boost::lexical_cast<std::string>(x);
// #else     
//   return numToString<int >(x);
// #endif     
   }

string
StringUtility::numberToString ( unsigned int x )
   {
// DQ (8/31/2009): This now compiles properly (at least for analysis, it might still fail for the code generation).
// #ifndef USE_ROSE     
     return boost::lexical_cast<std::string>(x);
// #else     
//      return numToString<unsigned int >(x);
// #endif     
   }

#if 0
string
// StringUtility::numberToString ( unsigned int x )
StringUtility::numberToString ( size_t x )
   {
  // Build a string representing the dimensionOfArrayStatement
     char numberString[128];
     sprintf (numberString,"%lu",x);
  // printf ("numberString = %s \n",numberString);
     return string(numberString);
   }
#endif

// DQ (8/10/2010): Changed to take parameter as const.
string
StringUtility::numberToString ( const void* x )
   {
  // Build a string representing the dimensionOfArrayStatement
     char numberString[128];
     sprintf (numberString,"%p",x);
  // printf ("numberString = %s \n",numberString);
     return string(numberString);
   }

string
StringUtility::numberToString ( double x )
   {
  // Build a string representing the dimensionOfArrayStatement
     char numberString[128];
     sprintf (numberString,"%2.2f",x);
  // printf ("numberString = %s \n",numberString);
     return string(numberString);
   }

string
StringUtility::addrToString( uint64_t x )
   {
     char numberString[128];
     sprintf(numberString, "0x%08"PRIx64, x);
     return string(numberString);
   }

string
StringUtility::removeRedundentSubstrings ( string X )
   {
  // Convert the string into a list of strings and separate out the redundent entries
     list<string> XStringList = StringUtility::stringToList(X);
     XStringList.sort();
     XStringList.unique();
     return StringUtility::listToString(XStringList);
   }

#if 0
bool
isNumber ( char c )
   {
     return (c >= '0') && (c <= '9');
   }
#endif

//Rama: 12/06/06:
//Donot know where this function is used, but we should be using the stdlib function isdigit
bool
isNumber ( char c )
   {
     return (isdigit(c));
   }

bool
isxNumber ( char c )
   {
     return (isxdigit(c));
   }

bool
isMarker ( char c )
   {
     return (c == '$');
   }

string
StringUtility::removePseudoRedundentSubstrings ( string X )
   {
  // Convert the string into a list of strings and separate out the redundent entries
     list<string> XStringList = StringUtility::stringToList(X);

#if 0
     printf ("XStringList.size() = %zu \n",XStringList.size());
#endif

     XStringList.sort();

#if 0
     printf ("After sort(): XStringList.size() = %zu \n",XStringList.size());
#endif

     XStringList.unique();

#if 0
     printf ("After unique(): XStringList.size() = %zu \n",XStringList.size());
#endif

  // Build a list of the strings that will be modified
     list<string> modifiedStringList;
     list<string> listOfStringsToRemove;

     list<string>::iterator i;

#if 0
     for (i = XStringList.begin(); i != XStringList.end(); i++)
        {
          printf ("AT TOP: (*i = %s) (size: %d) \n",(*i).c_str(),(*i).length());
        }
#endif

  // Two loops over the list of strings represents a quadratic complexity!
     for (i = XStringList.begin(); i != XStringList.end(); i++)
        {
          string i_modifiedString;
       // printf ("At top of loop over XStringList \n");

       // Build list of the differences between strings
          list<string> listOfDifferences;

          for (list<string>::iterator j = XStringList.begin(); j != XStringList.end(); j++)
             {
            // compare *i and *j and check for pseudo-redundence
            // printf ("top of loop through string: compare *i and *j and check for pseudo-redundence \n");

            // build information about *i
               string::const_iterator i_diffpos        = find_if ( (*i).begin(), (*i).end(), isNumber );

            // build information about *j
               string::const_iterator j_diffpos        = find_if ( (*j).begin(), (*j).end(), isNumber );
#if 0
               printf ("Testing (*i = %s) == (*j = %s) ) (sizes are: %d and %d) \n",
                               (*i).c_str(),(*j).c_str(),(*i).length(),(*j).length());
#endif

               unsigned int i_subStringLength  = i_diffpos - (*i).begin();
               unsigned int j_subStringLength  = j_diffpos - (*j).begin();

            // printf ("i_subStringLength = %d j_subStringLength = %d \n",i_subStringLength,j_subStringLength);

            // Must be the same length string AND
            // the same length substring occuring before the first number AND
            // there must have been a number in the string
               if ( ((*i).size() == (*j).size()) &&
                               (i_subStringLength  == j_subStringLength)  &&
                               (i_diffpos != (*i).end()) )
                  {
                 // printf ("substrings could be the same ... \n");

                    i_modifiedString = *i; i_modifiedString[i_subStringLength] = '$';
                    string j_modifiedString = *j; j_modifiedString[j_subStringLength] = '$';
#if 0
                    printf ("Testing (i_modifiedString = %s) == (j_modifiedString = %s) \n",
                                    i_modifiedString.c_str(),j_modifiedString.c_str());
#endif

                 // After modifying the strings (uniformly) see if we have a match
                    if ( i_modifiedString == j_modifiedString )
                       {
                      // (*i) and (*j) match upto the value of the number
                      // record this as a modified string
                      // modifiedStringList.push_back(i_modifiedString);

                      // Remove these strings (after we finish these nested loops)
                         listOfStringsToRemove.push_back(*i);

                      // Build a string from the number that differentiates the two strings
                         string i_numberString(1, *i_diffpos);
                         string j_numberString(1, *j_diffpos);
#if 0
                         printf ("Found a pseudo match between two strings: diff = %s and %s between %s and %s \n",
                                         i_numberString.c_str(),j_numberString.c_str(),(*i).c_str(),(*j).c_str());
#endif
                      // Save the differences between the pseudo matching strings
                         listOfDifferences.push_back(i_numberString);
                         listOfDifferences.push_back(j_numberString);
                       }
                  }
#if 0
               else
                  {
                    printf ("No similar substrings found! \n");
                  }

               printf ("bottom of loop through string: compare *i and *j and check for pseudo-redundence \n");
#endif
             }

       // printf ("listOfDifferences.size() = %zu \n",listOfDifferences.size());

       // If there are any elements then we can proceed
          if (!listOfDifferences.empty())
             {
#if 0
               printf ("Base of test of *i and *j (before sort): listOfDifferences = \n%s \n",listToString(listOfDifferences).c_str());
#endif
               listOfDifferences.sort();
#if 0
               printf ("Base of test of *i and *j (after sort): listOfDifferences = \n%s \n",listToString(listOfDifferences).c_str());
#endif
               listOfDifferences.unique();
#if 0
               printf ("Base of test of *i and *j (after unique): listOfDifferences = \n%s \n",listToString(listOfDifferences).c_str());
#endif

               string maxvalue = listOfDifferences.back();
               ROSE_ASSERT (!maxvalue.empty());

#if 0
               printf ("Max value = %s \n",maxvalue.c_str());
#endif

            // char* diffpos = find_if ( modifiedString.c_str(), modifiedString.c_str()+modifiedString.length(), isMarker );
               string::iterator diffpos = find_if (i_modifiedString.begin(), i_modifiedString.end(), isMarker );

#if 0
               printf ("Before copyEdit: diffpos = %c final string = %s \n",*diffpos,modifiedString.c_str());
#endif

               *diffpos = maxvalue[0];
            // modifiedString = copyEdit(modifiedString,string("$Y"),maxvalue);

#if 0
               printf ("Final string = %s \n",modifiedString.c_str());
#endif

               modifiedStringList.push_back(i_modifiedString);
             }
#if 0
          else
             {
               printf ("No differences to process \n");
             }

          printf ("At base of loop over XStringList \n");
#endif
        }

#if 0
     printf ("Now sort and remove non-unique elements \n");
#endif

  // Remove strings we identified for removal
     listOfStringsToRemove.sort();
     listOfStringsToRemove.unique();

#if 0
     printf ("After loop: listOfStringsToRemove.size() = %zu \n",listOfStringsToRemove.size());
#endif

     for (i = listOfStringsToRemove.begin(); i != listOfStringsToRemove.end(); i++)
        {
          XStringList.remove(*i);
        }
#if 0
     printf ("After loop: XStringList.size() = %zu \n",XStringList.size());
     printf ("After loop: XStringList = %s \n",listToString(XStringList).c_str());
#endif

  // Add the strings the we saved (the resort)
     XStringList.insert(XStringList.end(), modifiedStringList.begin(), modifiedStringList.end());
#if 0
     printf ("Before remove(): XStringList.size() = %zu \n",XStringList.size());
     printf ("Before remove(): XStringList = %s \n",listToString(XStringList).c_str());
#endif

     XStringList.remove(string("\n"));
#if 0
     printf ("Before sort(): XStringList.size() = %zu \n",XStringList.size());
     printf ("Before sort(): XStringList = %s \n",listToString(XStringList).c_str());
#endif

     XStringList.sort();
#if 0
     printf ("After sort(): XStringList.size() = %zu \n",XStringList.size());
     printf ("After sort(): XStringList = %s \n",listToString(XStringList).c_str());
#endif

     XStringList.unique();
#if 0
     printf ("After unique(): XStringList.size() = %zu \n",XStringList.size());
     printf ("After unique(): XStringList = %s \n",listToString(XStringList).c_str());
#endif

#if 0
     for (i = XStringList.begin(); i != XStringList.end(); i++)
        {
          printf ("AT BOTTOM: (*i = %s) (size: %d) \n",(*i).c_str(),(*i).length());
        }

     printf ("Returning from StringUtility::removePseudoRedundentSubstrings() (calling listToString member function) \n");
#endif

     return StringUtility::listToString(XStringList);
   }

#if 0
int
StringUtility::isSameName ( const std::string& s1, const std::string& s2 )
   {
     int returnValue = false;
  // return strcmp(fname, fileName) == 0;
  // The strings are the same only if ZERO is the return value from strcmp()
     if (s1 == s2)
        {
          returnValue = true;
        }
     return returnValue;
   }
#endif

// Macro used only in the copyEdit function
#define DEBUG_COPY_EDIT false

// BP : 10/25/2001, a non recursive version that
// allocs memory only once
string
StringUtility::copyEdit (
                const string& inputString, 
                const string& oldToken, 
                const string& newToken )
                  {
     // std::cerr << "StringUtility::copyEdit '" << inputString << "' '" << oldToken << "' '" << newToken << "'" << std::endl;
     string returnString;
     std::string::size_type oldTokenSize = oldToken.size();

     std::string::size_type position = 0;
     std::string::size_type lastPosition = 0;
     while (true) {
       position = inputString.find(oldToken, position);
       if (position == string::npos) {
         returnString += inputString.substr(lastPosition);
         break;
       } else {
         returnString += inputString.substr(lastPosition, position - lastPosition);
         returnString += newToken;
         position = lastPosition = position + oldTokenSize;
             }
        }

     return returnString;
   }

void
StringUtility::writeFile (
                const string& outputString,
                const string& fileNameString,
                const string& directoryName)
   {
  // char* directoryName = strdup(directoryName);

#if 0
  // char* filenamePrefix = "generatedCode.headers/";
     char* filenamePrefix = new char [strlen(directoryName)+2];
     strcpy(filenamePrefix,directoryName);
     strcat(filenamePrefix,"/");
#else
  // char* filenamePrefix = "";
#endif

     string outputFileName = directoryName + fileNameString;

     ofstream outputFile(outputFileName.c_str());
     ROSE_ASSERT (outputFile.good() == true);

  // Select an output stream for the program tree display (cout or <filename>.C.roseShow)
  // Macro OUTPUT_SHOWFILE_TO_FILE is defined in the transformation_1.h header file
  // ostream & outputStream = (OUTPUT_TO_FILE ? ((ostream&) outputFile) : ((ostream&) cout));
     ROSE_ASSERT (outputFile.good() == true);

     outputFile << outputString;
     ROSE_ASSERT (outputFile.good() == true);

     outputFile.close();
   }


StringUtility::FileWithLineNumbers
StringUtility::copyEdit (
                const StringUtility::FileWithLineNumbers& inputString, 
                const string& oldToken, 
                const string& newToken ) {
  StringUtility::FileWithLineNumbers result = inputString;
  for (unsigned int i = 0; i < result.size(); ++i) {
    result[i].str = copyEdit(result[i].str, oldToken, newToken);
  }
  return result;
}

StringUtility::FileWithLineNumbers
StringUtility::copyEdit (
                const StringUtility::FileWithLineNumbers& inputString, 
                const string& oldToken, 
                const StringUtility::FileWithLineNumbers& newToken ) {
  StringUtility::FileWithLineNumbers result = inputString;
  for (unsigned int i = 0; i < result.size(); ++i) {
    string str = result[i].str;
    std::string::size_type pos = str.find(oldToken);
    if (pos != string::npos) {
      // Split the line into the before-substitution and after-substitution regions
      result[i].str = str.substr(0, pos);
      result.insert(result.begin() + i + 1, StringUtility::StringWithLineNumber(str.substr(pos + oldToken.size()), result[i].filename + " after subst for " + oldToken, result[i].line));
   // result[i].filename += " before subst for " + oldToken;
      // Do the insertion
      result.insert(result.begin() + i + 1, newToken.begin(), newToken.end());
      i += newToken.size(); // Rescan the after-substitution part of the old line, but not any of the new text
    }
  }
  return result;
}

        string 
StringUtility::readFile ( const string& fileName )
   {
  // Reads entire text file and places contents into a single string

  // BP : 10/23/2001, rather than allocate fixed large blocks of memory (350K * sizeof(char) !!)
  // allocate what is required.
  // the code below is a slightly modified version of what I found at:
  // http://www.cplusplus.com/ref/iostream/istream/read.html

     char* buffer = NULL;

     ifstream inputFile;
     inputFile.open( fileName.c_str(), ios::binary );
     if (inputFile.good() != true)
        {
          printf ("ERROR: File not found -- %s \n",fileName.c_str());
          //ROSE_ABORT();
            std::string s( "ERROR: File not found -- " );
            s += fileName;
            throw s;
        }

     ROSE_ASSERT (inputFile.good() == true);

  // get length of file:
     inputFile.seekg (0, ios::end);
     std::streamoff length = inputFile.tellg();
     inputFile.seekg (0, ios::beg);       

  // allocate memory:
     buffer = new char [length+1];
     ROSE_ASSERT(buffer != NULL);

  // read data as a block:
     inputFile.read(buffer,(int)length);
     buffer[length] = '\0';
     inputFile.close();

  // DQ: (10/21/02) Sunjeev reported the following assertion as failing on
  // his machine at UCSD (works for us, but i have made it more general)
  // ROSE_ASSERT(strlen(buffer) <= length);
  // MS: (12/11/02) added the strict test again
     ROSE_ASSERT(strlen(buffer) == (unsigned) length);

     string returnString = buffer;

     return returnString;
   }

StringUtility::FileWithLineNumbers
StringUtility::readFileWithPos ( const string& fileName )
   {
  // Reads entire text file and places contents into a single string

  // BP : 10/23/2001, rather than allocate fixed large blocks of memory (350K * sizeof(char) !!)
  // allocate what is required.
  // the code below is a slightly modified version of what I found at:
  // http://www.cplusplus.com/ref/iostream/istream/read.html

     unsigned int line = 1;
     char* buffer = NULL;

     string fullFileName = StringUtility::getAbsolutePathFromRelativePath(fileName);

  // printf("Opening file : %s\n",fullFileName.c_str());

     ifstream inputFile;


         inputFile.open( fileName.c_str(), ios::binary );


     if (inputFile.good() != true)
        {
             printf ("ERROR: File not found -- %s \n",fileName.c_str());
            // ROSE_ABORT();
          std::string s( "ERROR: File not found -- " );
          s += fileName;
          throw s;
        }

     ROSE_ASSERT (inputFile.good() == true);

  // get length of file:
     inputFile.seekg (0, ios::end);
     std::streamoff length = inputFile.tellg();
     inputFile.seekg (0, ios::beg);       

  // allocate memory:
     buffer = new char [length+1];
     ROSE_ASSERT(buffer != NULL);

  // read data as a block:
     inputFile.read(buffer,(int)length);
     buffer[length] = '\0';
     inputFile.close();

  // DQ: (10/21/02) Sunjeev reported the following assertion as failing on
  // his machine at UCSD (works for us, but i have made it more general)
  // ROSE_ASSERT(strlen(buffer) <= length);
  // MS: (12/11/02) added the strict test again
     ROSE_ASSERT(strlen(buffer) == (unsigned) length);

     string returnString = buffer;
     delete[] buffer;

     StringUtility::FileWithLineNumbers result;
     for (std::string::size_type pos = 0; pos != string::npos; )
        {
          std::string::size_type lastPos = pos;
          pos = returnString.find('\n', lastPos);
          result.push_back(StringUtility::StringWithLineNumber(returnString.substr(lastPos, pos - lastPos), fullFileName, line));
       // cerr << "Added line '" << returnString.substr(lastPos, pos - lastPos) << "' at line " << line << endl;
          ++line;
          ++pos; // Skip newline
          if (pos == returnString.size()) break;
        }

     result.push_back(StringUtility::StringWithLineNumber("", "", 1));
     return result;
   }

std::string
StringUtility::StringWithLineNumber::toString() const
   {
     std::ostringstream os;

  // DQ (1/21/2010): Added support for skipping these when in makes it easer to debug ROSETTA generated files.
#ifdef SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES
  // os << str << std::endl;
     os << "/* #line " << line << " \"" << (filename.empty() ? "" : getAbsolutePathFromRelativePath(filename)) << "\" */\n" << str << std::endl;
#else
     os << "#line " << line << " \"" << (filename.empty() ? "" : getAbsolutePathFromRelativePath(filename)) << "\"\n" << str << std::endl;
#endif

     return os.str();
   }

std::string
StringUtility::toString(const StringUtility::FileWithLineNumbers& strings,
                        const std::string& filename,
                        int physicalLine /* Line number in output file */) {
  std::string result;
  unsigned int lastLineNumber = 1;
  std::string lastFile = "";
  bool inPhysicalFile = true; // Not in a specifically named file
  bool needLineDirective = false;
  for (unsigned int i = 0; i < strings.size(); ++i) {
    // Determine if a #line directive is needed, if the last iteration did not
    // force one to be added
    bool newInPhysicalFile = (strings[i].filename == "");
    if (inPhysicalFile != newInPhysicalFile)
      needLineDirective = true;
    if (!inPhysicalFile &&
        (strings[i].filename != lastFile ||
         strings[i].line != lastLineNumber))
      needLineDirective = true;

    if (strings[i].str == "" && i + 1 == strings.size()) { // Special case
      needLineDirective = false;
    }

    // Print out the #line directive (if needed) and the actual line
    if (needLineDirective) {
#ifdef SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES
           /* Nothing to do here! */
      if (strings[i].filename == "") { // Reset to actual input file
        result += "/* #line " + numberToString(physicalLine + 1 /* Increment because number is the line number of the NEXT line after the #line directive */) + " \"" + filename + "\" */\n";
      } else {
        result += "/* #line " + numberToString(strings[i].line) + " \"" + strings[i].filename + "\" */\n";
      }
#else
      if (strings[i].filename == "") { // Reset to actual input file
        result += "#line " + numberToString(physicalLine + 1 /* Increment because number is the line number of the NEXT line after the #line directive */) + " \"" + filename + "\"\n";
      } else {
        result += "#line " + numberToString(strings[i].line) + " \"" + strings[i].filename + "\"\n";
      }
#endif
      // These are only updated when a #line directive is actually printed,
      // largely because of the blank line exception above (so if a blank line
      // starts a new file, the #line directive needs to be emitted on the
      // first non-blank line)
      lastLineNumber = strings[i].line + 1;
      lastFile = strings[i].filename;
    } else {
      ++lastLineNumber;
    }
    result += strings[i].str + "\n";

    bool printedLineDirective = needLineDirective;

    // Determine if a #line directive is needed for the next iteration
    needLineDirective = false;
    inPhysicalFile = newInPhysicalFile;
    if (strings[i].str.find('\n') != std::string::npos && !inPhysicalFile) {
      needLineDirective = true; // Ensure that the next line has a #line directive
    }

    // Update the physical line counter based on the number of lines output
    if (printedLineDirective) ++physicalLine; // For #line directive
    for (size_t pos = strings[i].str.find('\n');
         pos != std::string::npos; pos = strings[i].str.find('\n', pos + 1)) {
      ++physicalLine; // Increment for \n in string
    }
    ++physicalLine; // Increment for \n added at end of line
  }
  return result;
}


string
StringUtility::indentMultilineString ( const string& inputString, int statementColumnNumber )
   {
  // Indent the transformation to match the statement that it is transforming

     string returnString;

  // Put in linefeeds to avoid endless recursion in the copyEdit (I think)
     ROSE_ASSERT (statementColumnNumber > 0);
     string cr_and_added_space = string(statementColumnNumber, ' ');
     cr_and_added_space[0] = '\t';

  // returnString = copyEdit (inputString,"\n",cr_and_added_space);
     returnString = copyEdit (inputString,"\n",cr_and_added_space);

  // Now exchange the line feeds for carriage returns
  // returnString = copyEdit (returnString,"\t","\n");
     returnString = copyEdit (returnString,"\t","\n");

  // Now indent the first line (since there was no CR) there
     returnString = cr_and_added_space.substr(1) + returnString;

  // printf ("In StringUtility::indentMultilineString(): returnString = %s \n",returnString);
     return returnString;
   }



// JJW reverse-engineered documentation: split a string into substrings based
// on a separator character (previous version used a string as the separator
// but only considered the first character of it); remove any empty substrings
// found
void
StringUtility::splitStringIntoStrings(
                const string& inputString, 
                char separator, 
                vector<string>& stringList )
   {
  // This function was written by Bobby Philip in support of the newer approach toward
  // handling a broader number of back-end C++ compilers.

     stringList.clear();

     std::string::size_type pos = 0, lastPos = 0;
     while (true) {
       pos = inputString.find(separator, pos);
       if (pos == string::npos) {
         stringList.push_back(inputString.substr(lastPos));
         return;
       } else {
         if (pos != lastPos) {
           stringList.push_back(inputString.substr(lastPos, pos - lastPos));
             }
         lastPos = pos = pos + 1;
        }
        }
   }

#if 0
// DQ (2/18/2006): Added simple checksum (good enough for short strings)
        unsigned short int 
StringUtility::chksum(char *buffer, int len)
   {
  // This is a simple checksum function (obtained off the web):
  // This is a very inefficient routine that does the checksumming. The
  // linux checksum is very much more powerful and quicker. However this
  // gives you the general idea. Note that if you are going to checksum
  // a checksummed packet that includes the checksum, you have to compliment
  // the output. Also note that this works ONLY for an even number of bytes.

     ROSE_ASSERT(len % 2 == 0);

     unsigned short int *word;
     unsigned long accum;
     unsigned long chksm;
     int i;

     accum = 0;
     word = (unsigned short *) buffer;
     len >>= 1; /* Words only */
     for (i=0; i< len; i++)
          accum += (unsigned long) *word++;

     chksm = (accum & 0xffff); /* Mask all but low word */
     chksm += (accum >> 16); /* Sum all the carries */

     if (chksm > 0xffff) /* If this also carried */
          chksm++; /* Sum this too */
     return (unsigned short) (chksm & 0xffff);
   }
#endif


// DQ (2/18/2006): Added general name mangling for all declarations (and some other IR nodes).
// JJW (10/15/2007): Does this compute a ones-complement checksum like used for TCP?
        unsigned long
StringUtility::generate_checksum( string s )
   {
  // This function returns a unique checksum from the mangled name used 
  // it provides a simple means to obtain a unique value for any C++ 
  // declaration.  At a later date was shoulduse the MD5 Checksum 
  // implementation (but we can do that later).

  // The declaration is the same under One-time Definition Rule (ODR)
  // if and only if the checksum values for each declaration are the same.

     string uniqueName = s;

  // The checksum function requires a even length string (so we have to fix it up)
     if (uniqueName.size() % 2 != 0)
        {
       // printf ("Adding another character to make string an even valued length \n");
       // Use a character that does not appear in mangled 
       // names so that no other mangled name could include it.
          uniqueName += "#";
        }
     ROSE_ASSERT(uniqueName.size() % 2 == 0);

  // Call a simple checksum function
  // unsigned short int checksum = StringUtility::chksum(buffer,uniqueName.size());

     unsigned long accum = 0;
     unsigned long chksm;

     accum = 0;
     unsigned int len  = uniqueName.size() / 2; /* Words only */
     for (unsigned int i=0; i< len; i++)
          accum += (unsigned long) (((unsigned short*)uniqueName.data())[i]);

     chksm = (accum & 0xffff); /* Mask all but low word */
     chksm += (accum >> 16); /* Sum all the carries */

     if (chksm > 0xffff) /* If this also carried */
          chksm++; /* Sum this too */
     unsigned short checksum = (unsigned short) (chksm & 0xffff);

  // printf ("Checksum = %d \n",checksum);
     return (unsigned long) checksum;
   }

//Rama: 12/06/06
//Replaced the functionality by a call to basename
string
StringUtility::stripPathFromFileName ( const string & fileNameWithPath )
   {
  // This function removes the path from the filename if it includes a path
  // and returns the filename

  // printf ("StringUtility::stripPathFromFileName(fileNameWithPath = %s) \n",fileNameWithPath.c_str());

#if 1
  // DQ (9/6/2008): It seems that the problem might have been the stripFileSuffixFromFileName(), so this is put back!

  // DQ (9/6/2008): This version does not work on paths that have "." in them (basename() function does not work well).
  //    Example of input that fails: ROSE/ROSE_CompileTree/svn-LINUX-64bit-4.2.2/tutorial/inputCode_binaryAST_1
  // returns: svn-LINUX-64bit-4.2

     string returnString;
     char c_version[PATH_MAX]; 
     ROSE_ASSERT (fileNameWithPath.size() + 1 < PATH_MAX);
     strcpy(c_version, fileNameWithPath.c_str());

#if ROSE_MICROSOFT_OS
//       printf ("Error: basename() not available in MSVS (work around not implemented) \n");
//       ROSE_ASSERT(false);
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath(c_version,drive,dir,fname,ext);
         // tps (08/17/2010) - Made this work under Windows. 
         string fnamestr(fname);
         string extstr(ext);
         returnString = fnamestr+extstr;
#else
     returnString = basename(c_version);
#endif

         return returnString;
#endif

#if 0
  // DQ (9/6/2008): Use this version of the code which handles more complex patha names.
  // Make it safe to input a filename without a path name (return the filename)
     string::size_type positionOfLastSlash  = fileNameWithPath.rfind('/');
     string::size_type positionOfFirstSlash = fileNameWithPath.find('/');

     printf ("positionOfLastSlash = %zu \n",positionOfLastSlash);
     printf ("positionOfFirstSlash = %zu \n",positionOfFirstSlash);

     string returnString;
     if (positionOfLastSlash != string::npos)
        {
          returnString = fileNameWithPath.substr(positionOfLastSlash+1);
        }
       else
        {
          returnString = fileNameWithPath;
        }

     printf ("StringUtility::stripPathFromFileName() (after substr) returnString = %s \n",returnString.c_str());

     if (positionOfFirstSlash < positionOfLastSlash)
        {
          printf (" Look for leading \'/\' from the front \n");
          while (returnString[0] == '/')
             {
               returnString.erase(0,1);
             }

          printf ("StringUtility::stripPathFromFileName() (after erase) returnString = %s \n",returnString.c_str());
        }

     printf ("StringUtility::stripPathFromFileName() returnString = %s \n",returnString.c_str());

     return returnString;
#endif

#if 0
  // This is a older version using C style strings
     const size_t len = fileNameWithPath.size();
     const char *startOfString = &(fileNameWithPath[0]);
     const char *search = &(fileNameWithPath[len]);
     while ((search >= startOfString) && ('/' != *search))
        {
          --search;
        }
     ++search;

     char *returnString = new char[1 + len - (search - startOfString)];
     ROSE_ASSERT(returnString != NULL);
     return strcpy(returnString, search);
#endif
   }

string
StringUtility::stripFileSuffixFromFileName ( const string & fileNameWithSuffix )
   {
  // Make it safe to input a filename without a suffix (return the filename)

     string returnString;

#if 0
  // This function is not sophisticated enough to handle binaries with paths such as:
  //    ROSE/ROSE_CompileTree/svn-LINUX-64bit-4.2.2/tutorial/inputCode_binaryAST_1

     string::size_type positionOfDot = fileNameWithSuffix.rfind('.');
     if (positionOfDot != string::npos)
          returnString = fileNameWithSuffix.substr(0,positionOfDot);
     else
          returnString = fileNameWithSuffix;
#else
  // Handle the case of files where the filename does not have a suffix
     size_t lastSlashPos = fileNameWithSuffix.rfind('/');
     size_t lastDotPos   = fileNameWithSuffix.rfind('.');

  // printf ("lastSlashPos = %zu \n",lastSlashPos);
  // printf ("lastDotPos   = %zu \n",lastDotPos);

     if (lastSlashPos != string::npos && lastDotPos < lastSlashPos)
          returnString = fileNameWithSuffix;
       else
          returnString = fileNameWithSuffix.substr(0, lastDotPos);
#endif

  // printf ("fileNameWithSuffix = %s returnString = %s \n",fileNameWithSuffix.c_str(),returnString.c_str());

     return returnString;

// Extra code not used but which was a part of StringUtility::stripFileSuffixFromFileName()
#if 0
     const char *startOfString = &(fileNameWithPath[0]);
     const char *lastDot = strrchr(startOfString, '.');
     const size_t lengthOfFileWithoutSuffix = ((lastDot == NULL) ? fileNameWithPath.size() : (lastDot - fileNameWithSuffix));
     string returnString = fileNameWithSuffix.substr(
                     char *returnString = new char[lengthOfFileWithoutSuffix + 1];
                     ROSE_ASSERT(NULL != returnString);
                     returnString[lengthOfFileWithoutSuffix] = '\0';
                     return (char *)memcpy(returnString, fileNameWithSuffix, 
                             lengthOfFileWithoutSuffix);
#endif
   }

//
//Rama: I am not sure if this mechanism can deal with files ending with .
//Like "test."
//I am not clear about the purpose of the function too. So, not modyfying it.
string
StringUtility::fileNameSuffix ( const string & fileNameWithSuffix )
   {
  // Make it safe to input a filename without a suffix (return the filename)
     string::size_type positionOfDot = fileNameWithSuffix.rfind('.');
     string returnString = fileNameWithSuffix;

  // allow input to not have an extension
     if (positionOfDot != string::npos)
        {
       // Advance past the "."
          positionOfDot++;

          returnString = fileNameWithSuffix.substr(positionOfDot);
        }

     return returnString;
   }


// DQ (3/15/2005): New, simpler and better implementation suggested function from Tom, thanks Tom!
string
StringUtility::getPathFromFileName ( const string & fileNameWithPath )
   {
  // This function removes the filename from the combined path and 
  // filename if it includes a path and returns only the path.
  // Make it safe to input a filename without a path name (return the filename)

     char c_version[PATH_MAX]; 
     ROSE_ASSERT (fileNameWithPath.size() + 1 < PATH_MAX);
     strcpy(c_version, fileNameWithPath.c_str());

#if ROSE_MICROSOFT_OS
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

         _splitpath(c_version,drive,dir,fname,ext);
//       printf ("Error: dirname() not supported in MSVS 9work around not implemented) \n");
//       printf ("dirname = %s \n",dir);
         // tps (08/17/2010) - Made this work under Windows.
         string drivestr(drive);
         string dirstr(dir);
         string returnString = drivestr+dirstr;
//       ROSE_ASSERT(false);
#else
     string returnString = dirname(c_version);
#endif
     //dirname returns a "." if fileNameWithPath does not contain "/"'s
     //I am not sure why this function was written and so, preserve the functionality using empty return string in such cases.

     if(returnString == ".")
          returnString = "";
     return returnString;

#if 0
     string::size_type positionOfSlash = fileNameWithPath.rfind('/');

     string returnString;
     if (positionOfSlash != string::npos)
          returnString = fileNameWithPath.substr(0,positionOfSlash+1);
       else
          returnString = "";

     return returnString;
#endif

#if 0
     const char *lastSlash = strrchr(fileNameWithPath, '/');
     const ptrdiff_t len = (lastSlash == NULL) ? 0 : (1 + lastSlash - fileNameWithPath);
     char *result = new char[len + 1];
     ROSE_ASSERT(NULL != result);
     result[len] = '\0';
     return (char *)memcpy(result, fileNameWithPath, len);
#endif
   }


string
StringUtility::escapeNewLineCharaters ( const string & X )
   {
     string returnString;
     int stringLength = X.length();

     for (int i=0; i < stringLength; i++)
        {
          if ( X[i] == '\n' )
             {
               returnString += "\\l";
             }
          else
             {
               if ( X[i] == '\"' )
                  {
                    returnString += "\\\"";
                  }
               else
                  {
                    returnString += X[i];
                  }
             }
        }

     return returnString;
   }

std::string StringUtility::intToHex(uint64_t i) {
  ostringstream os;
  os << "0x" << std::hex << i;
  return os.str();
}


string
StringUtility::convertToLowerCase( const string & inputString )
   {
  // DQ (11/12/2008): Used to convert module names to lower case.

  // printf ("Before conversion to lower case: inputString = %s \n",inputString.c_str());
     string returnString = inputString;
     for (size_t i=0; i < returnString.length(); i++)
        {
          returnString[i] = tolower(returnString[i]);
        }
  // printf ("After conversion to lower case: returnString = %s \n",returnString.c_str());

     return returnString;
   }




