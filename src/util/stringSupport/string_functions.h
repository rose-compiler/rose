#ifndef ROSE_STRING_UTILITY_H
#define ROSE_STRING_UTILITY_H

// Move this to rose.h or a build a rose_utility.h file later
#include "commandline_processing.h"
#include <vector>
#include <string>
#include <sstream>

// extern const char** roseGlobalVariantNameList;
//Rama: 12/14/06: Changed the class to namespace and removed 'static'ness of the erstwhile "member functions"
//There is still a lot of clean up do be done: Like 
//      reorganize the functions
//      Make a single utililities namespace for ROSE with different functionalites
//      including the functions in util directory.

namespace StringUtility
   {
  // Container class for numerous string utility functions that are useful in different parts of the ROSE project.
  // Rama (12/22/2006): Clearly, some of the following are not string utilities, but file utilities.
  // This class is the wrapper around the realpath of UNIX

     std::string getAbsolutePathFromRelativePath ( const std::string & relativePath, bool printErrorIfAny = false); // Real declaration is below

     struct StringWithLineNumber {
          std::string str;
          std::string filename; // Empty string means generated code
          unsigned int line;

          StringWithLineNumber(const std::string& str, const std::string& filename, unsigned int line): str(str), filename(filename), line(line) {}

          std::string toString() const;
        };

	 typedef std::vector<StringWithLineNumber> FileWithLineNumbers;

	 inline std::ostream& operator<<(std::ostream& os, const StringWithLineNumber& s) {
	   os << s.toString();
	   return os;
	 }

	 std::string toString(const FileWithLineNumbers& strings, const std::string& filename = "<unknown>", int line = 1);

	 inline FileWithLineNumbers& operator+=(FileWithLineNumbers& a, const FileWithLineNumbers& b) {
	   a.insert(a.end(), b.begin(), b.end());
	   return a;
	 }

	 inline FileWithLineNumbers operator+(const FileWithLineNumbers& a, const FileWithLineNumbers& b) {
	   FileWithLineNumbers f = a;
	   f += b;
	   return f;
	 }

#if 0
	 inline std::ostream& operator<<(std::ostream& os, const FileWithLineNumbers& f) {
	   os << StringUtility::toString(f);
	   return os;
	 }
#endif

	 inline FileWithLineNumbers& operator<<(FileWithLineNumbers& f, const std::string& str) {
	   // Add loose text to the output file
	   if (!f.empty() && f.back().filename == "") {
	     f.back().str += str;
	   } else {
	     f.push_back(StringWithLineNumber(str, "", 1));
	   }
	   return f;
	 }

	 inline FileWithLineNumbers& operator<<(FileWithLineNumbers& f, const char* str) {
	   f << std::string(str);
	   return f;
	 }

#if 0
         // enum VariantT;
      //! get the name of a SAGE III class from the variant (calls mechanism in ROSE/AstProcessing)
          std::string getVariantName ( int v );
#endif

      //! generate a file
          void  writeFile ( const std::string& outputString, const std::string& fileNameString, const std::string& directoryName );

      //! read an existing file
           std::string readFile ( const std::string& fileName );

      //! read a file, keeping source line number information
           FileWithLineNumbers readFileWithPos(const std::string& fileName);

       /*! @{ */
       /*! @name New C++ string functions
          \brief C++ string style support.

           Supporting functions for string operations common within ROSE.
           \internal Uses C++ string interface.
       */
       //! substring replacement function
           std::string copyEdit ( const std::string& inputString, const std::string & oldToken, const std::string & newToken );
       //! convert a number to a string
           std::string numberToString ( long x );
           std::string numberToString ( unsigned long x );
           std::string numberToString ( int x );
           std::string numberToString ( unsigned int x );
       //! convert an integer to a Hex string
           std::string intToHex(uint32_t i);
       //! convert a unsigned integer (size_t) to a string
       //  string numberToString ( unsigned int x );
       //  std::string numberToString ( size_t x );
       //! convert a pointer value to a string
           std::string numberToString ( void* x );
       //! convert a number to a string
           std::string numberToString ( double x );

       //! formatting support for generated code strings
	   std::string indentMultilineString ( const std::string& inputString, int statementColumnNumber );

      //! generate a string from a list of ints
           std::string listToString ( const std::list<int> & X, bool separateStrings = false );
      //! generate a string from a list of strings
           std::string listToString ( const std::list<std::string> & X, bool separateStrings = false );
      //! generate a list of strings from a string (list elements are delimited by "\n")
           std::list<std::string> stringToList ( const std::string & X );

      //! generate a string from a vector of strings
           std::string listToString ( const std::vector<std::string> & X, bool separateStrings = false );

       //! Remove redundent lines (substrings must be separated by "\n"
           std::string removeRedundentSubstrings ( std::string X );
      //! Remove redundent lines containing special substrings of form string#
           std::string removePseudoRedundentSubstrings ( std::string X );
       /*! @} */

       /*! @{ */
       /*! @name Old C style string support functions
          \brief C style string support.
          \deprecated These functions will be removed after we have converted all 
                      source code to use the C++ string class.

          This function was used before moving to C++ strings.  It remains in use in some
          places where the code still uses the older C style strings.
       */
       //! replaced by equality operator in C++ string class
           // int isSameName ( const std::string& s1, const std::string& s2 );

           // char* stringDuplicate ( const char* tempString );
	   std::string copyEdit ( const std::string& inputString, const std::string& oldToken, const std::string& newToken );
	   FileWithLineNumbers copyEdit ( const FileWithLineNumbers& inputString, const std::string& oldToken, const std::string& newToken );
	   FileWithLineNumbers copyEdit ( const FileWithLineNumbers& inputString, const std::string& oldToken, const FileWithLineNumbers& newToken );
       //  bool isContainedIn ( const char* longString, const char* shortString );
           inline bool isContainedIn ( const std::string & longString, const std::string & shortString ) {
	     return longString.find(shortString) != std::string::npos;
	   }


       //! A smarter string concatination function (corrects for space in the target automatically)
           // char* stringConcatinate ( const char* targetString , const char* endingString );

      //! Support for commandline construction used for building argv and argc (I think) (written by Bobby Philip)
           void splitStringIntoStrings( const std::string& inputString, char separator, std::vector<std::string>& stringList );
       /*! @} */

      //! Simple function to generate checksum for string (can be used with get_mangled_name() to provide simpler names)
       //  unsigned short int chksum(char *buffer, int len);
           unsigned long generate_checksum( std::string s );

       // DQ (3/5/2006): Copies from ROSE class (depricated in there previous location)
           std::string stripPathFromFileName           ( const std::string & fileNameWithPath );   //! get the filename from the full filename
           std::string getPathFromFileName             ( const std::string & fileNameWithPath );   //! get the path from the full filename
           std::string stripFileSuffixFromFileName     ( const std::string & fileNameWithSuffix ); //! get the name without the ".C"
           std::string getAbsolutePathFromRelativePath ( const std::string & relativePath, bool printErrorIfAny /* = false */ );       //! get the absolute path from the relative path
           std::string fileNameSuffix                  ( const std::string & fileName );           //! get the file name extension (suffix)

           bool isCppFileNameSuffix                    ( const std::string & fileName );           //! true only if this is a valid C++ source file name extension (suffix)

       /*
        * The function 
        * findfile
        * traverse the current directory, searching
        * for files with a given string in their name.
        * input:    string to match and directory to match it in.
        * output:   any file found, returned as a list of strings with a full path.
        */
           std::list<std::string> findfile(std::string patternString, std::string pathString);

      //! Translates newline characters to linefeeds and escapes quotes (used to generate text for DOT nodes containing source code).
           std::string escapeNewLineCharaters ( const std::string & X );
   };


// endif for ROSE_STRING_UTILITY_H
#endif



