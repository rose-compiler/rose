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

// DQ (2/22/2014): Used below to control use of __int128 type.
// However, this file can't be included here and must be included 
// in the C source file calling this header file.
// #include "rose_config.h"

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

     ROSE_UTIL_API std::string getAbsolutePathFromRelativePath ( const std::string & relativePath, bool printErrorIfAny = false); // Real declaration is below

     struct StringWithLineNumber
        {
          std::string str;      // DQ (1/23/2010): this name is difficult to trace within the code.
          std::string filename; // Empty string means generated code
          unsigned int line;

          StringWithLineNumber(const std::string& str, const std::string& filename, unsigned int line): str(str), filename(filename), line(line) {}

          ROSE_UTIL_API std::string toString() const;
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

         ROSE_UTIL_API std::string toString(const FileWithLineNumbers& strings, const std::string& filename = "<unknown>", int line = 1);

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
          ROSE_UTIL_API void  writeFile ( const std::string& outputString, const std::string& fileNameString, const std::string& directoryName );

      //! Read an existing file
           ROSE_UTIL_API std::string readFile ( const std::string& fileName );

      //! Read a file, keeping source line number information
           ROSE_UTIL_API FileWithLineNumbers readFileWithPos(const std::string& fileName);

       /*! @{ */
       /*! @name New C++ string functions
          \brief C++ string style support.

           Supporting functions for string operations common within ROSE.
           \internal Uses C++ string interface.
       */
       //! Substring replacement function
           ROSE_UTIL_API std::string copyEdit ( const std::string& inputString, const std::string & oldToken, const std::string & newToken );
       //! Convert a number to a string
           ROSE_UTIL_API std::string numberToString ( long long x );
           ROSE_UTIL_API std::string numberToString ( unsigned long long x );
           ROSE_UTIL_API std::string numberToString ( long x );
           ROSE_UTIL_API std::string numberToString ( unsigned long x );
           ROSE_UTIL_API std::string numberToString ( int x );
           ROSE_UTIL_API std::string numberToString ( unsigned int x );
       //! Convert an integer to a Hex string
           ROSE_UTIL_API std::string intToHex(uint64_t i);
       //! Convert a unsigned integer (size_t) to a string
       //  string numberToString ( unsigned int x );
       //  ROSE_UTIL_API std::string numberToString ( size_t x );

#ifndef _MSC_VER
// #if !defined(__STRICT_ANSI__) && defined(_GLIBCXX_USE_INT128)
// #if ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6))
   #if (defined(BACKEND_CXX_IS_GNU_COMPILER) && (((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6)) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4)))
        // DQ (2/22/2014): Required code for GNU versions greater than 4.6.
           ROSE_UTIL_API std::string numberToString ( __int128 x );
           ROSE_UTIL_API std::string numberToString ( unsigned __int128 x );
   #endif
#endif

       // DQ (8/10/2010): Changed to take parameter as const.
       //! Convert a pointer value to a string
           ROSE_UTIL_API std::string numberToString ( const void* x );

       //! Convert a number to a string
           ROSE_UTIL_API std::string numberToString ( double x );

       //! Convert an address to a string, preferring a hexadecimal representation with at least 8 digits.  The second argument
       //  indicates the number of significant bits; if it's zero then the function chooses 32 or 64 depending on @p x.
           ROSE_UTIL_API std::string addrToString(uint64_t x, size_t nbits=0);

       //! Formatting support for generated code strings
           ROSE_UTIL_API std::string indentMultilineString ( const std::string& inputString, int statementColumnNumber );

      //! Generate a string from a list of ints
           ROSE_UTIL_API std::string listToString ( const std::list<int> & X, bool separateStrings = false );
      //! Generate a string from a list of strings
           ROSE_UTIL_API std::string listToString ( const std::list<std::string> & X, bool separateStrings = false );
      //! Generate a list of strings from a string (list elements are delimited by "\n"). \sa prefixLines().
           ROSE_UTIL_API std::list<std::string> stringToList ( const std::string & X );

      //! Generate a string from a vector of strings
           ROSE_UTIL_API std::string listToString ( const std::vector<std::string> & X, bool separateStrings = false );

           ROSE_UTIL_API std::list<std::string> tokenize ( std::string X, char delim );

       //! Remove redundant lines (substrings must be separated by "\n". FIXME (spelling)
           ROSE_UTIL_API std::string removeRedundentSubstrings ( std::string X ); // sic
      //! Remove redundant lines containing special substrings of form string#. FIXME (spelling)
           ROSE_UTIL_API std::string removePseudoRedundentSubstrings ( std::string X ); // sic
       //! Append an abbreviation or full name to a string.
           ROSE_UTIL_API void add_to_reason_string(std::string &result, bool isset, bool do_pad,
                                     const std::string &abbr, const std::string &full);
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
          ROSE_UTIL_API std::string copyEdit ( const std::string& inputString, const std::string& oldToken, const std::string& newToken );
          ROSE_UTIL_API FileWithLineNumbers copyEdit ( const FileWithLineNumbers& inputString, const std::string& oldToken, const std::string& newToken );
          ROSE_UTIL_API FileWithLineNumbers copyEdit ( const FileWithLineNumbers& inputString, const std::string& oldToken, const FileWithLineNumbers& newToken );
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
           ROSE_UTIL_API std::string convertToLowerCase( const std::string & inputString );

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
          ROSE_UTIL_API std::string stripPathFromFileName           ( const std::string & fileNameWithPath ); 
           //! Get the path from the full file name: /path/name.suffix 
          ROSE_UTIL_API std::string getPathFromFileName             ( const std::string & fileNameWithPath );   
           //! Get the file name without the ".suffix"
          ROSE_UTIL_API std::string stripFileSuffixFromFileName     ( const std::string & fileNameWithSuffix ); 
           //! Get the absolute path from the relative path
           ROSE_UTIL_API std::string getAbsolutePathFromRelativePath ( const std::string & relativePath, bool printErrorIfAny /* = false */ );       
           //! Get the file name suffix (extension) without the leading dot, fileName could be either base name or name with full path. 
           // If no dot is found in the input fileName, the function just returns the original fileName
           ROSE_UTIL_API std::string fileNameSuffix                  ( const std::string & fileName );          

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
           ROSE_UTIL_API std::string escapeNewLineCharaters ( const std::string & X );

           // RSS 7/1/2008 New functionality to support filename processing
           enum OSType 
                   { 
                                     OS_TYPE_UNKNOWN,
                         OS_TYPE_LINUX,
                         OS_TYPE_OSX,
                         OS_TYPE_WINDOWS,
                                                 OS_TPYE_WINDOWSXP};

           // Return OSType based on uname kernel name results
           ROSE_UTIL_API OSType getOSType();

           // Populate homeDir from $HOME environment var
           void homeDir(std::string& homeDir);

           /* Files can be classified as being in one of three
            * locations: We don't know if it's user or system It is a
            * user (application) file It is a system library This file
            * does not exist */
           enum FileNameLocation 
                               { 
                                                           FILENAME_LOCATION_UNKNOWN, 
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
           ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath);

           /* Given a fileName and an appPath that is a path to some
            * application's source code directory, return a
            * FileNameClassification indicating whether the fileName
            * is part of the source code or some system library */
           ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath,
                                                   OSType os);

           /* Given a fileName and an appPath that is a path to some
            * application's source code directory, and a collection 
            * of library paths, return a FileNameClassification
            * indicating whether the fileName is part of the source 
            * code or some system library and automatically determine 
            * the operating system from the host uname */
           ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath,
                                                   const std::map<std::string, std::string>& libPathCollection);

           /* Given a fileName and an appPath that is a path to some
            * application's source code directory, and a collection 
            * of library paths, return a FileNameClassification
            * indicating whether the fileName is part of the source 
            * code or some system library */ 
           ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName,
                                                   const std::string& appPath,
                                                   const std::map<std::string, std::string>& libPathCollection,
                                                   OSType os);

           /* Remove leading dots plus a space from a header file name
            * that is fiven in the format that g++ -H returns */
           ROSE_UTIL_API const std::string
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
            ROSE_UTIL_API int directoryDistance(const std::string& left,
                                  const std::string& right);


            /* Added htmlEscape necessary for QROSE work to this utility library - tps (9Oct2008) */
            ROSE_UTIL_API std::string htmlEscape(const std::string& s);

   /** Escape as for C/C++ string literals. */
   ROSE_UTIL_API std::string cEscape(const std::string&);

    // DQ (2/3/2009): Moved this function from attach_all_info.C
       ROSE_UTIL_API std::vector<std::string> readWordsInFile( std::string filename);

   /** Convert an ASCII hexadecimal character to an integer. */
   ROSE_UTIL_API unsigned hexadecimalToInt(char);

   /** Convert a number to a hexadecimal and decimal string.
    *
    *  The returned string starts with the hexadecimal representation of the number and an optional decimal representation
    *  in angle brackets.  The decimal portion will contain a signed and/or unsigned value depending on whether the value
    *  is interpretted as signed and whether the sign bit is set.  The signedToHex versions print the decimal value for only
    *  the signed interpretation; the unsignedToHex versions print only the decimal unsigned interpretation, and the toHex
    *  versions print both (but not redunantly).
    *
    *  @{ */
   ROSE_UTIL_API std::string toHex2(uint64_t value, size_t nbits,
                                    bool show_unsigned_decimal=true, bool show_signed_decimal=true,
                                    uint64_t decimal_threshold=256);
   ROSE_UTIL_API std::string signedToHex2(uint64_t value, size_t nbits);
   ROSE_UTIL_API std::string unsignedToHex2(uint64_t value, size_t nbits);

   template<typename T> std::string toHex(T value) { return toHex2((uint64_t)value, 8*sizeof(T)); }
   template<typename T> std::string signedToHex(T value) { return signedToHex2((uint64_t)value, 8*sizeof(T)); }
   template<typename T> std::string unsignedToHex(T value) { return unsignedToHex2((uint64_t)value, 8*sizeof(T)); }
   /** @} */

   /** Append an assembly comment to a string.  Assembly comments are surrounded by "<" and ">" characters.  If the string
    *  already ends with an assembly comment, then the specified comment is inserted before the final ">" and separated from
    *  the previous comment with a comma.  Assembly comments are usually used for things like printing a decimal representation
    *  of a hexadecimal value, etc.
    *
    *  Example: after executing these statements:
    *  
    *  @code
    *   std::string s = "0xff";
    *   s = appendAsmComment(s, "255");
    *   s = appendAsmComment(s, "-1");
    *  @endcode
    *
    *  The variable "s" will contain "0xff<255,-1>" */
   ROSE_UTIL_API std::string appendAsmComment(const std::string &s, const std::string &comment);

       /** Insert a prefix string before every line.  This function breaks the @p lines string into individual lines,
        *  inserts the @p prefix string at the beginning of each line, then concatenates the lines together into a return
        *  value.  If @p prefixAtFront is true (the default) then the prefix is added to the first line of @p lines, otherwise
        *  the first line is unchanged.  An empty @p lines string is considered to be a single line.  If @p prefixAtBack is
        *  false (the default) then the prefix is not appended to the @p lines string if @p lines ends with a linefeed. */
       ROSE_UTIL_API std::string prefixLines(const std::string &lines, const std::string &prefix,
                               bool prefixAtFront=true, bool prefixAtBack=false);

       /** Returns true if the string ends with line termination. Only common ASCII-based line terminations are recognized:
        *  CR+LF, LF+CR, CR (only), or LF (only). */
       ROSE_UTIL_API bool isLineTerminated(const std::string &s);

       /** Normalizes line termination.  Changes ASCII-based line termination conventions used by various operating systems
        *  into the LF (line-feed) termination used by Multics, Unix and Unix-like systems (GNU/Linux, Mac OS X, FreeBSD, AIX,
        *  Xenix, etc.), BeOS, Amiga, RISC OS and others.  Any occurrance of CR+LF, LF+CR, or CR by itself (in that order of
        *  left-to-right matching) is replaced by a single LF character. */
       ROSE_UTIL_API std::string fixLineTermination(const std::string &input);

       /** Converts a multi-line string to a single line.  This function converts a multi-line string to a single line by
        *  replacing line-feeds and carriage-returns (and their surrounding white space) with a user-supplied replacement
        *  string (that defaults to a single space). Line termination (and it's surrounding white space) that appears at the
        *  front or back of the input string is removed without replacing it.
        *
        *  See roseTests/utilTests/stringTests.C for lots of examples.
        *
        *  A new string is returned. */
       ROSE_UTIL_API std::string makeOneLine(const std::string &s, std::string replacement=" ");

       /** Convert binary data to base-64. The base64 number system uses the characters A-Z, a-z, 0-9, +, and / (in that
        * order). The returned string does not include linefeeds.  If @p do_pad is true then '=' characters may appear at the
        * end to make the total length a multiple of four.
        * @{ */
       std::string encode_base64(const std::vector<uint8_t> &data, bool do_pad=true);
       std::string encode_base64(const uint8_t *data, size_t nbytes, bool do_padd=true);
       /** @} */

       /** Convert base-64 to binary. */
       std::vector<uint8_t> decode_base64(const std::string &encoded);

      /** Join individual strings to form a single string.  Unlike listToString(), this function allows the caller to indicate
       *  how the strings should be separated from one another: the @p separator (default SPC) is inserted between each pair
       *  of strings, but not at the beginning or end, even if strings are empty.
       * @{ */
      template<class Container>
      std::string join(const std::string &separator, const Container &strings) {
          std::string retval;
          for (typename Container::const_iterator i=strings.begin(); i!=strings.end(); ++i)
              retval += (i==strings.begin() ? std::string() : separator) + *i;
          return retval;
      }
      template<class Iterator>
      std::string join_range(const std::string &separator, Iterator begin, Iterator end) {
          std::string retval;
          for (Iterator i=begin; i!=end; ++i)
              retval += (i==begin ? std::string() : separator) + *i;
          return retval;
      }
      std::string join(const std::string &separator, char *strings[], size_t nstrings);
      std::string join(const std::string &separator, const char *strings[], size_t nstrings);
      /** @} */

     /** Splits strings into parts.  Unlink stringToList(), this function allows the caller to indicate where to split the
      *  string.  The parts are the portions of the string on either side of the separator: if the separator appears at the
      *  beginning of the string, then the first part is empty; likewise if the separator appears at the end of the string then
      *  the last part is empty. At most @p maxparts are returned, the last of which may contain occurrences of the separator.
      *  If @p trim_white_space is true then white space is removed from the beginning and end of each part. Empty parts are
      *  never removed from the returned vector since the C++ library already has functions for that. The first few arguments
      *  are in the same order as for Perl's "split" operator. */
     ROSE_UTIL_API std::vector<std::string> split(const std::string &separator, const std::string &str, size_t maxparts=(size_t)(-1),
                                    bool trim_white_space=false);
     ROSE_UTIL_API std::vector<std::string> split(char separator, const std::string &str, size_t maxparts=(size_t)(-1),
                                    bool trim_white_space=false);

     /** Trims white space from the beginning and end of a string. Caller may specify the characters to strip and whether the
      * stripping occurs at the begining, the end, or both. */
     ROSE_UTIL_API std::string trim(const std::string &str, const std::string &strip=" \t\r\n", bool at_beginning=true, bool at_end=true);

     /** Expand horizontal tab characters. */
     std::string untab(const std::string &str, size_t tabstops=8, size_t firstcol=0);

     /** Converts a bunch of numbers to strings.  This is convenient when one has a container of numbers and wants to
      *  call join() to turn it into a single string.  For instance, here's how to convert a set of integers to a
      *  comma-separated list:
      * @code
      *  using namespace StringUtility;
      *  std::set<int> numbers = ...;
      *  std::string s = join(", ", toStrings(numbers));
      * @endcode
      *
      *  Here's how to convert a vector of addresses to space-separated hexadecimal values:
      * @code
      *  using namespace StringUtility;
      *  std::vector<rose_addr_t> addresses = ...;
      *  std::string s = join(" ", toStrings(addresses, addrToString));
      * @endcode
      *
      *  Here's how one could surround each address with angle brackets:
      * @code
      *  using namespace StringUtility;
      *  struct AngleSurround {
      *      std::string operator()(rose_addr_t addr) {
      *         return "<" + addrToString(addr) + ">";
      *      }
      *  };
      *  std::string s = join(" ", toStrings(addresses, AngleSurround()));
      * @endcode
      * @{ */
     template<class Container, class Stringifier>
     std::vector<std::string> toStrings(const Container &numbers, const Stringifier &stringifier=numberToString) {
         return toStrings_range(numbers.begin(), numbers.end(), stringifier);
     }
     template<class Iterator, class Stringifier>
     std::vector<std::string> toStrings_range(Iterator begin, Iterator end, const Stringifier &stringifier=numberToString) {
         std::vector<std::string> retval;
         for (/*void*/; begin!=end; ++begin)
             retval.push_back(stringifier(*begin));
         return retval;
     }
     /** @} */

    /** Helpful way to print singular or plural words.
     *
     * @code
     *  size_t n = ...;
     *  std::cout <<"received " <<plural(n, "values") <<"\n";
     * @encode
     *
     *  Output for various values of <em>n</em> will be:
     *
     * @code
     *  received 0 values
     *  received 1 value
     *  received 2 values
     * @endcode
     *
     * This function uses a handful of grade-school rules for converting the supplied plural word to a singular word when
     * necessary.  If these are not enough, then the singular form can be supplied as the third argument.
     *
     * @code
     *  std::cout <<"graph contains " <<plural(nverts, "vertices", "vertex") <<"\n";
     * @endcode
     */
    template<typename T>
    std::string plural(T n, const std::string &plural_word, const std::string &singular_word="") {
        assert(!plural_word.empty());
        std::string retval = numberToString(n) + " ";
        if (1==n) {
            if (!singular_word.empty()) {
                retval += singular_word;
            } else if (plural_word.size()>3 && 0==plural_word.substr(plural_word.size()-3).compare("ies")) {
                // string ends with "ies", as in "parties", so emit "party" instead
                retval += plural_word.substr(0, plural_word.size()-3) + "y";
            } else if (plural_word.size()>1 && plural_word[plural_word.size()-1]=='s') {
                // just drop the final 's'
                retval += plural_word.substr(0, plural_word.size()-1);
            } else {
                // I give up.  Use the plural and risk being grammatically incorrect.
                retval += plural_word;
            }
        } else {
            retval += plural_word;
        }
        return retval;
    }
   
} // namespace

#endif
