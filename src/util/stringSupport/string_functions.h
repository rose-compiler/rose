#ifndef ROSE_STRING_UTILITY_H
#define ROSE_STRING_UTILITY_H

// Move this to rose.h or a build a rose_utility.h file later
#include "commandline_processing.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <stdint.h>
#if ROSE_MICROSOFT_OS
// This is the boost solution for lack of support for stdint.h (e.g. types such as "uint64_t")
#include <msvc_stdint.h>
#else
#endif

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

     struct StringWithLineNumber
        {
          std::string str;      // DQ (1/23/2010): this name is difficult to trace within the code.
          std::string filename; // Empty string means generated code
          unsigned int line;

          StringWithLineNumber(const std::string& str, const std::string& filename, unsigned int line): str(str), filename(filename), line(line) {}

          std::string toString() const;
        };

#ifndef USE_ROSE
     typedef std::vector<StringWithLineNumber> FileWithLineNumbers;
#else
  // workaround of bug 315, separating definitions for a namespace
  // Liao, 2/16/2009
   }

namespace StringUtility
   {
     typedef std::vector<StringUtility::StringWithLineNumber> FileWithLineNumbers;
#endif

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

      //! Generate a file
          void  writeFile ( const std::string& outputString, const std::string& fileNameString, const std::string& directoryName );

      //! Read an existing file
           std::string readFile ( const std::string& fileName );

      //! Read a file, keeping source line number information
           FileWithLineNumbers readFileWithPos(const std::string& fileName);

       /*! @{ */
       /*! @name New C++ string functions
          \brief C++ string style support.

           Supporting functions for string operations common within ROSE.
           \internal Uses C++ string interface.
       */
       //! Substring replacement function
           std::string copyEdit ( const std::string& inputString, const std::string & oldToken, const std::string & newToken );
       //! Convert a number to a string
           std::string numberToString ( long long x );
           std::string numberToString ( unsigned long long x );
           std::string numberToString ( long x );
           std::string numberToString ( unsigned long x );
           std::string numberToString ( int x );
           std::string numberToString ( unsigned int x );
       //! Convert an integer to a Hex string
           std::string intToHex(uint64_t i);
       //! Convert a unsigned integer (size_t) to a string
       //  string numberToString ( unsigned int x );
       //  std::string numberToString ( size_t x );


       // DQ (8/10/2010): Changed to take parameter as const.
       //! Convert a pointer value to a string
           std::string numberToString ( const void* x );

       //! Convert a number to a string
           std::string numberToString ( double x );
       //! Convert an address to a string, preferring a hexadecimal representation with at least 8 digits.
           std::string addrToString( uint64_t x );

       //! Formatting support for generated code strings
	   std::string indentMultilineString ( const std::string& inputString, int statementColumnNumber );

      //! Generate a string from a list of ints
           std::string listToString ( const std::list<int> & X, bool separateStrings = false );
      //! Generate a string from a list of strings
           std::string listToString ( const std::list<std::string> & X, bool separateStrings = false );
      //! Generate a list of strings from a string (list elements are delimited by "\n")
           std::list<std::string> stringToList ( const std::string & X );

      //! Generate a string from a vector of strings
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
       //! Replaced by equality operator in C++ string class
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

       //! Convert string to lower case (e.g. for module file name generation).
           std::string convertToLowerCase( const std::string & inputString );

   // std::string mangledName ( std::string s );

  //! Popen wrapper to support demangling mangled functions
      bool popen_wrapper ( const std::string & command, std::vector<std::string> & result );

  //! Support for demangling mangled functions
      std::string demangledName ( std::string s );

//--------------------------------------------------------------
//@{
/*! @name File name strings
  \brief Handling for /path/name.suffix. Namespace CommandlineProcessing also has some file name handling functions. 

*/
       // DQ (3/5/2006): Copies from ROSE class (deprecated in there previous location)
          //! Get name.suffix from /path/name.suffix
           std::string stripPathFromFileName           ( const std::string & fileNameWithPath ); 
           //! Get the path from the full file name: /path/name.suffix 
           std::string getPathFromFileName             ( const std::string & fileNameWithPath );   
           //! Get the file name without the ".suffix"
           std::string stripFileSuffixFromFileName     ( const std::string & fileNameWithSuffix ); 
           //! Get the absolute path from the relative path
           std::string getAbsolutePathFromRelativePath ( const std::string & relativePath, bool printErrorIfAny /* = false */ );       
           //! Get the file name suffix (extension), fileName could be either base name or name with full path
           std::string fileNameSuffix                  ( const std::string & fileName );          

  // True only if this is a valid C++ source file name extension (suffix). Duplicate of CommandlineProcessing::isCppFileNameSuffix(). 
//           bool isCppFileNameSuffix                    ( const std::string & fileName ); 

//@}
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

           // RSS 7/1/2008 New functionality to support filename processing
           enum OSType { OS_TYPE_UNKNOWN,
                         OS_TYPE_LINUX,
                         OS_TYPE_OSX,
                         OS_TYPE_WINDOWS,
						 OS_TPYE_WINDOWSXP};

           // Return OSType based on uname kernel name results
           OSType getOSType();

           // Populate homeDir from $HOME environment var
           void homeDir(std::string& homeDir);

           /* Files can be classified as being in one of three
            * locations: We don't know if it's user or system It is a
            * user (application) file It is a system library This file
            * does not exist */
           enum FileNameLocation { FILENAME_LOCATION_UNKNOWN, 
                                   FILENAME_LOCATION_USER,    
                                   FILENAME_LOCATION_LIBRARY,
                                   FILENAME_LOCATION_NOT_EXIST };
           
           /* Files can be classified as being part of one of these
            * libraries: Unknown, it isn't a library - it's part of
            * the user application, or any of the libraries that the
            * enum values imply, this list will likely be added to
            * over time */
	   /* 
           enum FileNameLibrary { FILENAME_LIBRARY_UNKNOWN,
                                  FILENAME_LIBRARY_USER,
                                  FILENAME_LIBRARY_C,
                                  FILENAME_LIBRARY_STDCXX,
				  FILENAME_LIBRARY_STL,
                                  FILENAME_LIBRARY_LINUX,
                                  FILENAME_LIBRARY_GCC,
                                  FILENAME_LIBRARY_BOOST,
                                  FILENAME_LIBRARY_ROSE };
				  */

	   static const std::string FILENAME_LIBRARY_UNKNOWN = "Unknown";
	   static const std::string FILENAME_LIBRARY_USER = "User";
	   static const std::string FILENAME_LIBRARY_C = "C";
	   static const std::string FILENAME_LIBRARY_STDCXX = "C++";
	   static const std::string FILENAME_LIBRARY_STL = "STL";
	   static const std::string FILENAME_LIBRARY_LINUX = "Linux";
	   static const std::string FILENAME_LIBRARY_GCC = "GCC";
	   static const std::string FILENAME_LIBRARY_BOOST = "Boost";
	   static const std::string FILENAME_LIBRARY_ROSE = "Rose";

	   // CH (2/16/2010): Use this typedef to avoid following changes
	   typedef std::string FileNameLibrary;

           /* This is the return type of classifyFileName, which
            * provides all the details it infers */
           class FileNameClassification
           {
           private:
               FileNameLocation location;

	       // CH (2/12/2010): Change 'library' type from enum to string to let user set it
               FileNameLibrary library;
	       
               int distance;

           public:
               FileNameClassification(FileNameLocation loc,
                                      const FileNameLibrary& lib,
                                      int dist) : location(loc),
                                                  library(lib),
                                                  distance(dist)
                   {}
               FileNameClassification() : location(FILENAME_LOCATION_UNKNOWN),
                                          library("Unknown"),
                                          distance(0)
                   {}

               /* Return the FileNameLocation which is described above
                * with the definition of the enum */
               FileNameLocation getLocation() const
                   { return location; }

               /* Return the FileNameLibrary which is described above
                * with the definition of the enum */
               FileNameLibrary getLibrary() const
                   { return library; }

               /* Return the "distance" of the filename from the
                * appPath that was supplied during the call.  The
                * distance is defined as the number of cd's that only
                * move up or down one directory that it would take to
                * move from the directory of the filename to the
                * directory that was given by appPath.  This is
                * intended as a heuristic to gage whether or not one
                * believes that the filename is related to the source
                * (appPath) directory.  Examples:
                *
                * Between /a/b/c/file.h and /a/b/d/e/ the distance is 3
                * because one must cd ..; cd d; cd e; to get to appPath
                *
                * *EXCEPTION*: if the appPath is an ancestor of filename
                * then the distance will be 0.  The idea being that this
                * filename is "in" the appPath somewhere and thus part
                * of the application.
                */
               int getDistanceFromSourceDirectory() const
                   { return distance; }

               bool isUserCode() const
                   { return location == FILENAME_LOCATION_USER; }
               bool isLibraryCode() const
                   { return location == FILENAME_LOCATION_LIBRARY; }

               /* Return a string name for the library indicated by
                * getLibrary() */
               std::string getLibraryName() const
		   { return library; }
           };

           /* Given a fileName and an appPath that is a path to some
       	    * application's source code directory, return a
       	    * FileNameClassification indicating whether the fileName
       	    * is part of the source code or some system library and
       	    * automatically determine the operating system from the
       	    * host uname */
           FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath);

           /* Given a fileName and an appPath that is a path to some
       	    * application's source code directory, return a
       	    * FileNameClassification indicating whether the fileName
       	    * is part of the source code or some system library */
           FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath,
						   OSType os);

           /* Given a fileName and an appPath that is a path to some
       	    * application's source code directory, and a collection 
	    * of library paths, return a FileNameClassification
	    * indicating whether the fileName is part of the source 
	    * code or some system library and automatically determine 
	    * the operating system from the host uname */
           FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath,
						   const std::map<std::string, std::string>& libPathCollection);

           /* Given a fileName and an appPath that is a path to some
       	    * application's source code directory, and a collection 
	    * of library paths, return a FileNameClassification
	    * indicating whether the fileName is part of the source 
	    * code or some system library */ 
           FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath,
						   const std::map<std::string, std::string>& libPathCollection,
                                                   OSType os);

           /* Remove leading dots plus a space from a header file name
            * that is fiven in the format that g++ -H returns */
           const std::string
           stripDotsFromHeaderFileName(const std::string& name);

           /* Essentially the edit distance without substituion in
            * directory name tokens between two directories. Returns
            * the "distance" between left and right. The distance is
            * defined as the number of cd's that only move up or down
            * one directory that it would take to move from the
            * directory of the filename to the directory that was
            * given by appPath.  This is intended as a heuristic to
            * gage whether or not one believes that the left is
            * related to the right directory.  Examples:
            *
            * Between /a/b/c/file.h and /a/b/d/e/ the distance is 3
            * because one must cd ..; cd d; cd e */
            int directoryDistance(const std::string& left,
                                  const std::string& right);


	    /* Added htmlEscape necessary for QROSE work to this utility library - tps (9Oct2008) */
	    std::string htmlEscape(const std::string& s);

    // DQ (2/3/2009): Moved this function from attach_all_info.C
       std::vector<std::string> readWordsInFile( std::string filename);

   };


// endif for ROSE_STRING_UTILITY_H
#endif



