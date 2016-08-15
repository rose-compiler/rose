#ifndef ROSE_FileUtility_H
#define ROSE_FileUtility_H

#include "commandline_processing.h"

#include <StringUtility.h>

namespace rose {

// [Robb P Matzke 2016-06-16]: These file utilities were all originally part of StringUtility. I'm leaving them here for now
// just so I don't need to make so many changes to projects that use these, but you should expect these to be moved sometime in
// the future.
namespace StringUtility {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This part of the StringUtility API deals with file names and should be moved to some other name space. In particular, it
// provides no definitions for "path", "filename", "extension", etc. and many of these functions won't work properly on a
// non-POSIX system. Therefore, consider using rose::FileSystem, which is mostly a thin wrapper around boost::filesystem. The
// boost::filesystem documentation has good definitions for what the various terms should mean and works on non-POSIX file
// systems.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


enum OSType {
    OS_TYPE_UNKNOWN,
    OS_TYPE_LINUX,
    OS_TYPE_OSX,
    OS_TYPE_WINDOWS,
    OS_TPYE_WINDOWSXP
};

/** String with source location information.
 *
 *  The location information is a file name string (empty means generated code) and a line number. Line numbers are one-origin;
 *  the first line of a file is numbered 1, not 0.
 *
 *  @todo What does it mean to have a non-positive line number? What line number should be used for generated code when the @p
 *  filename member is empty? */
struct StringWithLineNumber {
    std::string str;                                    // DQ (1/23/2010): this name is difficult to trace within the code.
    std::string filename;                               // Empty string means generated code
    unsigned int line;

    StringWithLineNumber(const std::string& str, const std::string& filename, unsigned int line)
        : str(str), filename(filename), line(line) {}

    ROSE_UTIL_API std::string toString() const;
};

/** A vector of strings with line numbers and file names. */
typedef std::vector<StringWithLineNumber> FileWithLineNumbers;

/** Obtain operating system type information. */
ROSE_UTIL_API OSType getOSType();

/** Create a file.
 *
 *  Creates a new file, truncating any existing file with the same name, and writes the string @p outputString into the
 *  file. The name of the file is constructed by concatenating @p directoryName and @p fileNameString without any intervening
 *  component separator (e.g., no "/").
 *
 *  If the file cannot be created then this function silently fails (or aborts if ROSE is compiled in debug mode). */
ROSE_UTIL_API void writeFile(const std::string& outputString, const std::string& fileNameString,
                             const std::string& directoryName);

/** Reads entire text file.
 *
 *  Opens the specified file, reads its contents into a string, closes the file, and returns that string.
 *
 *  If the file cannot be opened then an std::string error message is thrown. The message reads "File not found" regardless of
 *  the actual error condition. */
ROSE_UTIL_API std::string readFile(const std::string& fileName);

/** Reads an entire text file.
 *
 *  Opens the specified file, reads its contents line by line into a vector of strings with location information, closes the
 *  file, and returns the vector.  The file names in the returned value are absolute names. The strings in the return value
 *  have their final line-feeds removed.
 *
 *  If the file cannot be opened then an std::string error message is thrown. The message reads "File not found" regardless of
 *  the actual error condition. */
ROSE_UTIL_API FileWithLineNumbers readFileWithPos(const std::string& fileName);

/** Name of the home directory.
 *
 *  Returns the value of the "HOME" environment variable by copying it into the @p dir argument. Will segfault if this
 *  environment variable is not set. */
ROSE_UTIL_API void homeDir(std::string& dir);

/** Returns the last component of a path in a filesystem.
 *
 *  Removes the "path" part of a "filename" (if there is one) and returns just the file name.
 *
 *  Terms are loosely defined and not likely to work for non-POSIX systems; consider using boost::filesystem instead. */
ROSE_UTIL_API std::string stripPathFromFileName(const std::string &fileNameWithPath);

/** Returns all but the last component of a path in a filesystem.
 *
 *  This function removes the filename from the combined path and filename if it includes a path and returns only the path.
 *  Make it safe to input a filename without a path name (return the filename).
 *
 *  Terms are loosely defined and this function possibly doesn't work for non-POSIX file systems; consider using
 *  boost::filesystem instead. */
ROSE_UTIL_API std::string getPathFromFileName(const std::string &fileNameWithPath);

/** Get the file name without the ".suffix".
 *
 *  Terms are loosely defined and it's not clear what happens for inputs like ".", ".foo", "..", ".foo.bar", "/.",
 *  etc. Consider using boost::filesystem instead. */
ROSE_UTIL_API std::string stripFileSuffixFromFileName(const std::string & fileNameWithSuffix);

/** Get the absolute path from the relative path.
 *
 *  Terms are loosely defined and this function is not likely to work on non-POSIX systems. Consider using boost::filesystem
 *  instead. */
ROSE_UTIL_API std::string getAbsolutePathFromRelativePath(const std::string &relativePath, bool printErrorIfAny = false);

/** Get the file name suffix (extension) without the leading dot.
 *
 *  Filename could be either base name or name with full path. If no dot is found in the input fileName, the function just
 *  returns the original fileName.
 *
 *  Terms are loosely defined and this function is not likely to work correctly in some situations, such as when the "." is not
 *  in the last component of the file name.  Consider using boost::filesystem instead. */
ROSE_UTIL_API std::string fileNameSuffix(const std::string &fileName);


// [Robb Matzke 2016-05-06]: I am deprecating "findfile" because:
//   1. It appears to not be used anywhere in ROSE, projects, tests, or documentation.
//   2. The name is spelled wrong (should be "findFile")
//   3. File operations should not be in StringUtility since they have nothing to do with string manipulation
//   4. rose::FileSystem::findNames does something similar.

/** Find file names non-recursively.
 *
 *  Scans the directory named @p pathString and returns a list of files in that directory which have @p patternString as a
 *  substring of their name. Note that @p patternString is not a glob or regular expression.  The return value strings are
 *  formed by concatenating the @p pathString and the file name with an intervening slash.
 *
 *  This function does not work for non-POSIX systems. Consider using boost::filesystem instead, which has a directory iterator
 *  that works for non-POSIX systems also. */
ROSE_UTIL_API std::list<std::string> findfile(std::string patternString, std::string pathString)
    SAWYER_DEPRECATED("use rose::FileSystem functions instead"); // ROSE_DEPRECATED is not defined here

/* File name location.
 *
 * Files can be classified as being in one of three locations: We don't know if it's user or system It is a user (application)
 * file It is a system library This file does not exist */
enum FileNameLocation {
    FILENAME_LOCATION_UNKNOWN,
    FILENAME_LOCATION_USER,
    FILENAME_LOCATION_LIBRARY,
    FILENAME_LOCATION_NOT_EXIST
};

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

/* This is the return type of classifyFileName, which provides all the details it infers */
class FileNameClassification {
private:
    FileNameLocation location;

    // CH (2/12/2010): Change 'library' type from enum to string to let user set it
    FileNameLibrary library;
    int distance;

public:
    FileNameClassification(FileNameLocation loc, const FileNameLibrary& lib, int dist)
        : location(loc), library(lib), distance(dist) {}

    FileNameClassification()
        : location(FILENAME_LOCATION_UNKNOWN), library("Unknown"), distance(0) {}

    /* Return the FileNameLocation which is described above with the definition of the enum */
    FileNameLocation getLocation() const {
        return location;
    }

    /* Return the FileNameLibrary which is described above with the definition of the enum */
    FileNameLibrary getLibrary() const {
        return library;
    }

    /* Return the "distance" of the filename from the appPath that was supplied during the call.  The distance is defined as
     * the number of cd's that only move up or down one directory that it would take to move from the directory of the filename
     * to the directory that was given by appPath.  This is intended as a heuristic to gage whether or not one believes that
     * the filename is related to the source (appPath) directory.  Examples:
     *
     * Between /a/b/c/file.h and /a/b/d/e/ the distance is 3 because one must cd ..; cd d; cd e; to get to appPath
     *
     * *EXCEPTION*: if the appPath is an ancestor of filename then the distance will be 0.  The idea being that this filename
     * is "in" the appPath somewhere and thus part of the application. */
    int getDistanceFromSourceDirectory() const {
        return distance;
    }

    bool isUserCode() const {
        return location == FILENAME_LOCATION_USER;
    }

    bool isLibraryCode() const {
        return location == FILENAME_LOCATION_LIBRARY;
    }

    /* Return a string name for the library indicated by getLibrary() */
    std::string getLibraryName() const {
        return library;
    }
};

/** Determine whether a file is source code or system library.
 *
 *  Given a fileName and an appPath that is a path to some application's source code directory, return a FileNameClassification
 *  indicating whether the fileName is part of the source code or some system library and automatically determine the operating
 *  system from the host uname */
ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName, const std::string& appPath);

/** Determine whether a file is source code or system library.
 *
 *  Given a fileName and an appPath that is a path to some application's source code directory, return a FileNameClassification
 *  indicating whether the fileName is part of the source code or some system library */
ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName, const std::string& appPath, OSType os);

/** Determine whether a file is source code or system library.
 *
 *  Given a fileName and an appPath that is a path to some application's source code directory, and a collection of library
 *  paths, return a FileNameClassification indicating whether the fileName is part of the source code or some system library
 *  and automatically determine the operating system from the host uname */
ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName, const std::string& appPath,
                                                      const std::map<std::string, std::string>& libPathCollection);

/** Determine whether a file is source code or system library.
 *
 *  Given a fileName and an appPath that is a path to some application's source code directory, and a collection of library
 *  paths, return a FileNameClassification indicating whether the fileName is part of the source code or some system library */
ROSE_UTIL_API FileNameClassification classifyFileName(const std::string& fileName, const std::string& appPath,
                                                      const std::map<std::string, std::string>& libPathCollection,
                                                      OSType os);

/** Remove leading dots.
 *
 *  Removes leading dots plus a space from a header file name that is given in the format that g++ -H returns */
ROSE_UTIL_API const std::string stripDotsFromHeaderFileName(const std::string& name);

/** Edit distance between two directory names.
 *
 *  Essentially the edit distance without substituion in directory name tokens between two directories. Returns the "distance"
 *  between left and right. The distance is defined as the number of cd's that only move up or down one directory that it would
 *  take to move from the directory of the filename to the directory that was given by appPath.  This is intended as a
 *  heuristic to gage whether or not one believes that the left is related to the right directory.  Examples:
 *
 *  Between /a/b/c/file.h and /a/b/d/e/ the distance is 3 because one must cd ..; cd d; cd e */
ROSE_UTIL_API int directoryDistance(const std::string& left, const std::string& right);

/** Reads words from a file.
 *
 *  Opens the specified file for reading and reads all words from the file using <code>std::istream</code>
 *  <code>operator>></code> into <code>std::string</code>.  If the file cannot be opened then an error message is printed to
 *  standard output (not error) and the program exits with status 1. */
ROSE_UTIL_API std::vector<std::string> readWordsInFile(std::string filename);

// popen_wrapper is defined in sage_support.cpp

/** Simple wrapper for Unix popen command.
 *
 *  If there is a failure (cannot create pipes, command not found, command terminated abnormally, input buffer too small, etc),
 *  then an error is printed to standard error (with or without line termination) and false is returned.  When an error occurs
 *  the pipes that were opened to communicate with the subcommand might not be closed. */
bool popen_wrapper(const std::string &command, std::vector<std::string> &result);

/** Prints a StringWithLineNumber. */
inline std::ostream& operator<<(std::ostream& os, const StringWithLineNumber& s) {
    os << s.toString();
    return os;
}

/** Generate C preprocessor #line directives.
 *
 *  Given a vector of strings with source location information, print those strings with intervening C preprocessor #line
 *  directives. For strings that have no source location, use the specified @p filename argument as the name of the file. The
 *  @p physicalLine is the line number to assume for the first line of output (the return value). The #line directives are
 *  not emitted into the return value when they're not needed (e.g., two @p strings are at successive line numbers of the same
 *  source file) or if ROSE has been configured to not emit #line directivies (i.e., the @c
 *  SKIP_HASH_LINE_NUMBER_DECLARATIONS_IN_GENERATED_FILES preprocessor symbol is defined). The @p strings should not have
 *  trailing linefeeds or else the output will contain extra blank lines. */
ROSE_UTIL_API std::string toString(const FileWithLineNumbers& strings, const std::string& filename = "<unknown>", int line = 1);

/** Append strings with source location information to vector of such.
 *
 *  Modifies @p a by appending those strings from @p b. */
inline FileWithLineNumbers& operator+=(FileWithLineNumbers& a, const FileWithLineNumbers& b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

/** Concatenate vectors of strings with source location.
 *
 *  Returns a new vector of strings with location information by concatenating vector @p a with @p b. */
inline FileWithLineNumbers operator+(const FileWithLineNumbers& a, const FileWithLineNumbers& b) {
    FileWithLineNumbers f = a;
    f += b;
    return f;
}

/** Append string to vector of strings with location information.
 *
 *  Appends @p str to the last string in the vector of strings with location information. If the vector is empty then a new
 *  element is created.
 *
 *  The new code is marked as either generated (empty file name) or not generated (non-empty name) based on whether the vector
 *  was initially empty or whether the final element of the vector was generated.  Adding a string to an empty vector makes it
 *  always be a generated string; adding it to a non-empty vector makes it generated or not generated depending on whether the
 *  last element of the vector is generated or not generated.
 *
 *  The string @p str should not include line termination. (see @ref toString).
 *
 *  @{ */
inline FileWithLineNumbers& operator<<(FileWithLineNumbers& f, const std::string& str) {
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
/** @} */

/** Replace all occurrences of a string with another string.
 *
 *  Finds all occurrences of the @p oldToken string in @p inputString and replaces them each with @p newToken, returning the
 *  result.
 *
 * @{ */
ROSE_UTIL_API std::string copyEdit(const std::string& inputString, const std::string & oldToken, const std::string & newToken);
ROSE_UTIL_API FileWithLineNumbers copyEdit(const FileWithLineNumbers& inputString, const std::string& oldToken,
                                           const std::string& newToken);
ROSE_UTIL_API FileWithLineNumbers copyEdit(const FileWithLineNumbers& inputString, const std::string& oldToken,
                                           const FileWithLineNumbers& newToken);
/** @} */
} // namespace
} // namespace

#endif
