// DQ (6/25/2009): Added comments to this file.

// This file contains the code to classify files as being either user files or system files.
// The classification levesl are more envolved but this support allows users to extress
// rules that apply to there own code differently than to the STL for example.  The
// approach is to associate a classification with files based on their location and 
// sometimes also the filename. Thus analysis can be done for user code that checks
// a rule but skips non-user code where that rule might not be valid.

// DQ (6/25/2009): Greg White reported that the file locations for STL (and other system files)
// are not correctly interpreted when soft-links are used.  Need to use "lstat()" and "readlink()"
// to chase down and resolved links so that we can more robustly classify files.  
// Note that system files used in ROSE are in fact links to the system files used in the
// back-end vendor compiler, so this is important for classification of system files
// used in processing software using ROSE.

// DQ (2/11/2010): Added so that we can detect what compiler is being used to compile this file.
#include "rose_config.h"

// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"

#include <algorithm>
#include <map>
#include "string_functions.h"
// DQ (3/22/2009): Windows does not have this header.
#if ROSE_MICROSOFT_OS
#else
#include <sys/utsname.h>
#endif

#include <boost/foreach.hpp>

// CH (1/29/2010): Needed for boost::filesystem::exists(...)
#include <boost/filesystem.hpp>

#include <boost/version.hpp>
#if BOOST_VERSION >= 103600
#  define BOOST_HAS_BRANCH_PATH has_parent_path
#else
#  define BOOST_HAS_BRANCH_PATH has_branch_path
#endif

using namespace std;

// DQ (2/8/2010): I don't like namespaces used to define functions (too unclear).
// namespace StringUtility
// {

    namespace
    {
	bool startsWith(const string& str, const string& substr)
	{
	    if (str.length() < substr.length())
		return false;
	    int result = str.compare(0, substr.length(), substr);
	    return (result == 0);
	}

	bool endsWith(const string& str, const string& substr)
	{
	    if (str.length() < substr.length())
		return false;
	    int result = str.compare(str.length() - substr.length(),
		    substr.length(), substr);
	    return (result == 0);
	}

	// TODO can we static initialize stl structures?
	// if so it'd be better to create these as sets
	// test for existance, which would be *much* faster (and safer)
	const char* LINUX_INCLUDES[] = {
	    "acpi",
	    "asm",
	    "asm-generic",
	    "asm-i386",
	    "asm-x86_64",
	    "config",
	    "linux",
	    "math-emu",
	    "media",
	    "net",
	    "pcmcia",
	    "rxrpc",
	    "scsi",
	    "sound",
	    "video",
	    NULL };

	const char* GLIBC_INCLUDES[] = {
	    "_G_config.h", 
	    "a.out.h", 
	    "aio.h", 
	    "aliases.h", 
	    "alloca.h", 
	    "ar.h", 
	    "argp.h", 
	    "argz.h", 
	    "arpa", 
	    "assert.h", 
	    "bits", 
	    "byteswap.h", 
	    "complex.h", 
	    "cpio.h", 
	    "crypt.h", 
	    "ctype.h", 
	    "dirent.h", 
	    "dlfcn.h", 
	    "elf.h", 
	    "endian.h", 
	    "envz.h", 
	    "err.h", 
	    "errno.h", 
	    "error.h", 
	    "execinfo.h", 
	    "fcntl.h", 
	    "features.h", 
	    "fenv.h", 
	    "fmtmsg.h", 
	    "fnmatch.h", 
	    "fpu_control.h", 
	    "fstab.h", 
	    "fts.h", 
	    "ftw.h", 
	    "gconv.h", 
	    "getopt.h", 
	    "glob.h", 
	    "gnu", 
	    "grp.h", 
	    "iconv.h", 
	    "ieee754.h", 
	    "ifaddrs.h", 
	    "inttypes.h", 
	    "langinfo.h", 
	    "lastlog.h", 
	    "libgen.h", 
	    "libintl.h", 
	    "libio.h", 
	    "limits.h", 
	    "link.h", 
	    "locale.h", 
	    "malloc.h", 
	    "math.h", 
	    "mcheck.h", 
	    "memory.h", 
	    "mntent.h", 
	    "monetary.h", 
	    "mqueue.h", 
	    "net", 
	    "netash", 
	    "netatalk", 
	    "netax25", 
	    "netdb.h", 
	    "neteconet", 
	    "netinet", 
	    "netipx", 
	    "netpacket", 
	    "netrom", 
	    "netrose", 
	    "nfs", 
	    "nl_types.h", 
	    "nptl", 
	    "nss.h", 
	    "obstack.h", 
	    "paths.h", 
	    "poll.h", 
	    "printf.h", 
	    "protocols", 
	    "pthread.h", 
	    "pty.h", 
	    "pwd.h", 
	    "re_comp.h", 
	    "regex.h", 
	    "regexp.h", 
	    "resolv.h", 
	    "rpc", 
	    "rpcsvc", 
	    "sched.h", 
	    "scsi", 
	    "search.h", 
	    "semaphore.h", 
	    "setjmp.h", 
	    "sgtty.h", 
	    "shadow.h", 
	    "signal.h", 
	    "spawn.h", 
	    "stab.h", 
	    "stdint.h", 
	    "stdio.h", 
	    "stdio_ext.h", 
	    "stdlib.h", 
	    "string.h", 
	    "strings.h", 
	    "stropts.h", 
	    "sys", 
	    "syscall.h", 
	    "sysexits.h", 
	    "syslog.h", 
	    "tar.h", 
	    "termio.h", 
	    "termios.h", 
	    "tgmath.h", 
	    "thread_db.h", 
	    "time.h", 
	    "ttyent.h", 
	    "ucontext.h", 
	    "ulimit.h", 
	    "unistd.h", 
	    "ustat.h", 
	    "utime.h", 
	    "utmp.h", 
	    "utmpx.h", 
	    "values.h", 
	    "wait.h", 
	    "wchar.h", 
	    "wctype.h", 
	    "wordexp.h", 
	    "xlocale.h",
	    NULL };

	const char * STL_INCLUDES[] = {
	    "algorithm",
	    "bitset",
	    "complex",
	    "deque",
	    "functional",
	    "list",
	    "map",
	    "numeric",
	    "queue",
	    "set",
	    "stack",
	    "utility",
	    "valarray",
	    "vector",
	    NULL };

	// CH (2/2/2010): Standard C++ header files (include new C++ 0x header files)
	const char * GLIBCXX_INCLUDES[] = { 
	    "algorithm",
	    "bitset",
	    "cassert",
	    "cctype",
	    "cerrno",
	    "climits",
	    "clocale",
	    "cmath",
	    "complex",
	    "csetjmp",
	    "csignal",
	    "cstdarg",
	    "cstddef",
	    "cstdio",
	    "cstdlib",
	    "cstring",
	    "ctime",
	    "deque",
	    "exception",
	    "fstream",
	    "functional",
	    "iomanip",
	    "ios",
	    "iosfwd",
	    "iostream",
	    "istream",
	    "iterator",
	    "limits",
	    "list",
	    "locale",
	    "map",
	    "memory",
	    "new",
	    "numeric",
	    "ostream",
	    "queue",
	    "set",
	    "sstream",
	    "stack",
	    "stdexcept",
	    "streambuf",
	    "string",
	    "typeinfo",
	    "utility",
	    "valarray",
	    "vector",
	    NULL };

#if 0
// DQ (10/14/2010): This is defined but not used.
	const char * GLIBCXX0X_INCLUDES[] = {
	    "array",
	    "random",
	    "regex",
	    "tuple",
	    "type_traits",
	    "unordered_map",
	    "unordered_set",
	    NULL };
#endif


	bool
	    charListMatches(const char** clist,
		    const string& prefix,
		    const string& fileName)
	    {
		const char **substr;
		substr = clist;
		while(*substr != NULL)
		{
		    string inclstr = prefix;
		    inclstr += *substr;
		    if (fileName.find(inclstr) != string::npos)
			return true;
		    substr++;
		}
		return false;
	    }

	StringUtility::FileNameLibrary
	    classifyLibrary(const string& fileName)
	    {
#ifndef CXX_IS_ROSE_CODE_GENERATION
		using namespace boost::filesystem;

		if (charListMatches(LINUX_INCLUDES, "include/", fileName))
		{
		    return StringUtility::FILENAME_LIBRARY_LINUX;
		}
		if (charListMatches(GLIBC_INCLUDES, "include/", fileName))
		{
		    return StringUtility::FILENAME_LIBRARY_C;
		}
		if (fileName.find("lib/gcc") != string::npos)
		{
		    return StringUtility::FILENAME_LIBRARY_GCC;
		}
		if (fileName.find("boost") != string::npos)
		{
		    return StringUtility::FILENAME_LIBRARY_BOOST;
		}
		if (fileName.find("rose.h") != string::npos ||
			fileName.find("include-staging/g++_HEADERS") != string::npos ||
			fileName.find("include-staging/gcc_HEADERS") != string::npos)
		{
		    return StringUtility::FILENAME_LIBRARY_ROSE;
		}

		// the path of C++ header files does not have to contain "c++"
		//if (fileName.find("c++") != string::npos)
		{
		    const char ** substr = STL_INCLUDES;
		    while (*substr != NULL)
		    {
			if (endsWith(fileName, *substr))
			    return StringUtility::FILENAME_LIBRARY_STL;
			++substr;
		    }
		}

		path p = fileName;
		while (p.BOOST_HAS_BRANCH_PATH())
		{
		    p = p.branch_path();
		    if(exists(p / path("rose.h")))
			return StringUtility::FILENAME_LIBRARY_ROSE;

		    const char ** substr = GLIBCXX_INCLUDES;
		    bool isCxxHeader = true;
		    while (*substr != NULL)
		    {
			if (!exists(p / path(*substr)))
			{
			    isCxxHeader = false;
			    break;
			}
			++substr;
		    }
		    if(isCxxHeader)
			return StringUtility::FILENAME_LIBRARY_STDCXX;
		}

		/* 
		if (fileName.find("c++") != string::npos)
		{
		    const char ** substr = GLIBCXX_INCLUDES;
		    while (*substr != NULL)
		    {
			if (endsWith(fileName, *substr))
			    return FILENAME_LIBRARY_STDCXX;
			++substr;
		    }
		}
		*/
#endif
		return StringUtility::FILENAME_LIBRARY_UNKNOWN;
	    }
    } // end unnamed namespace for file location definitions

    int
	StringUtility::directoryDistance(const string& left, const string& right)
	{
	    vector<string> lvec;
	    splitStringIntoStrings(left, '/', lvec);
	    vector<string> rvec;
	    splitStringIntoStrings(right, '/', rvec);

	    assert(!lvec.empty());
	    assert(!rvec.empty());

	    lvec.erase(lvec.end());
	    rvec.erase(rvec.end());

	    vector<string>::iterator l = lvec.begin();
	    vector<string>::iterator r = rvec.begin();
	    // empty body, this is just to advance the iters as long
	    // as elts match
	    for (; l != lvec.end() && r != rvec.end() && *l == *r; l++, r++)
		;

	    return distance(l, lvec.end()) + distance(r, rvec.end());
	}

StringUtility::OSType
StringUtility::getOSType()
    {
#if ROSE_MICROSOFT_OS
    OSVERSIONINFO osvi;
    BOOL bIsWindowsXPorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsXPorLater = 
       ( (osvi.dwMajorVersion > 5) ||
       ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) ));
	    
	string sysname;
	if (bIsWindowsXPorLater)
		sysname="WindowsXP";
	else
		sysname="Windows";

#else
	    struct utsname val;

	    int ret = uname(&val);
	    if (ret == -1)
		return OS_TYPE_UNKNOWN;

	    string sysname = val.sysname;
#endif
	    if (sysname == "Linux")
		return OS_TYPE_LINUX;
	    else if (sysname == "Darwin")
		return OS_TYPE_OSX;
	    else if (sysname == "WindowsXP")
		return OS_TPYE_WINDOWSXP;
	    else if (sysname == "Windows")
		return OS_TYPE_WINDOWS;
	    else
		return OS_TYPE_UNKNOWN;
	}

    void
StringUtility::homeDir(string& dir)
    {
        const char* home = getenv("HOME");
#ifdef _MSC_VER
#define __builtin_constant_p(exp) (0)
#endif
	    ROSE_ASSERT(home);
	    dir = home;
	}

    // Update FileNameInfo class with details about where the
    // file comes from and what library it might be a part of
    StringUtility::FileNameClassification
	StringUtility::classifyFileName(const string& fileName,
		const string& appPath)
	{
	    return classifyFileName(fileName, appPath, std::map<string, string>(), getOSType());
	}

    StringUtility::FileNameClassification
	StringUtility::classifyFileName(const string& fileName,
		const string& appPath,
		OSType os)
	{
	    return classifyFileName(fileName, appPath, std::map<string, string>(), os);
	}

    StringUtility::FileNameClassification
	StringUtility::classifyFileName(const string& filename,
		const string& appPath,
		const std::map<string, string>& libPathCollection)
	{
	    return classifyFileName(filename, appPath, libPathCollection, getOSType());
	}

    // Internal function to above public interface, this version
    // is exposed just for testing purposes 
    StringUtility::FileNameClassification
	StringUtility::classifyFileName(const string& fileName,
		const string& appPathConst,
		const std::map<string, string>& libPathCollection,
		OSType os)
	{
	    // First, check if this file exists. Filename may be changed 
	    // into an illegal one by #line directive
	    if(!boost::filesystem::exists(fileName))
		return FileNameClassification(FILENAME_LOCATION_NOT_EXIST,
			"Unknown",
			0);

	    string appPath = appPathConst;

	    for(std::map<string, string>::const_iterator it = libPathCollection.begin();
		it != libPathCollection.end(); ++it)	
	    {
		if (startsWith(fileName, it->first))
		{
		    return FileNameClassification(FILENAME_LOCATION_LIBRARY,
			    it->second,
			    directoryDistance(fileName, appPath));
		}
	    }

	    // Consider all non-absolute paths to be application code
	    if (os == OS_TYPE_WINDOWS)
	    {
		// Ensure all appPaths are given with a trailing slash since
		// they represent directories
		if (appPath.empty() || *(appPath.end() - 1) != '\\')
		    appPath += '\\';

		// Tricky for Windows, make sure it doesn't just start
		// with a drive letter
		if (fileName.length() < 2 || fileName[1] != ':')
		{
		    return FileNameClassification(FILENAME_LOCATION_USER,
			    "User", //FILENAME_LIBRARY_USER,
			    0);
		}
	    }
	    else
	    {
		// Ensure all appPaths are given with a trailing slash since
		// they represent directories
		if (appPath.empty() || *(appPath.end() - 1) != '/')
		    appPath += '/';

		if (!startsWith(fileName, "/"))
		{
		    return FileNameClassification(FILENAME_LOCATION_USER,
			    "User", //FILENAME_LIBRARY_USER,
			    0);
		}
	    }

	    // If this is anywhere in the home dir or whitelis
	    // then return that it's part of the application/user code
	    if (startsWith(fileName, appPath))
	    {
		return FileNameClassification(FILENAME_LOCATION_USER,
			"User", //FILENAME_LIBRARY_USER,
			0);
	    }

	    FileNameLibrary filenameLib = classifyLibrary(fileName);
	    if (filenameLib != StringUtility::FILENAME_LIBRARY_UNKNOWN)
	    {
		return FileNameClassification(FILENAME_LOCATION_LIBRARY,
			filenameLib,
			directoryDistance(fileName, appPath));
	    }

	    if (os == OS_TYPE_LINUX)
	    {
		if (startsWith(fileName, "/usr") ||
			startsWith(fileName, "/opt"))
		{
		    return FileNameClassification(FILENAME_LOCATION_LIBRARY,
			    classifyLibrary(fileName),
			    directoryDistance(fileName,
				appPath));
		}
	    }
	    else if (os == OS_TYPE_OSX)
	    {
		if (startsWith(fileName, "/usr") ||
			startsWith(fileName, "/opt") ||
			startsWith(fileName, "/System"))
		{
		    return FileNameClassification(FILENAME_LOCATION_LIBRARY,
			    classifyLibrary(fileName),
			    directoryDistance(fileName,
				appPath));
		}
	    }
	    else if (os == OS_TYPE_WINDOWS)
	    {
		// TODO Need to convert Windows fileName and appPath to be
		// UNIX style to run directoryDistance on it
		if (startsWith(fileName, "C:\\Program Files"))
		{
		    return FileNameClassification(FILENAME_LOCATION_LIBRARY,
			    classifyLibrary(fileName),
			    directoryDistance(fileName,
				appPath));
		}
	    }
	    return FileNameClassification(FILENAME_LOCATION_UNKNOWN,
		    "Unknown", //FILENAME_LIBRARY_UNKNOWN,
		    directoryDistance(fileName, appPath));
	}

/* 
    // Possible return values for FileNameClassification::getLibraryName
    // Corresponds 1-to-1 to FileNameLibrary enum in the header file
    static const size_t FILENAME_NAMES_SIZE = 8;
    static const char* FILENAME_NAMES[FILENAME_NAMES_SIZE] = { "UNKNOWN",
                                                               "USER",
                                                               "c",
                                                               "stdc++",
                                                               "linux",
                                                               "gcc",
                                                               "boost",
                                                               "rose" };    
const string
StringUtility::FileNameClassification::getLibraryName() const
    {
        ROSE_ASSERT(this);
        ROSE_ASSERT(library >= 0 && library < (int)FILENAME_NAMES_SIZE);
        return FILENAME_NAMES[library];
    }
*/

const string
StringUtility::stripDotsFromHeaderFileName(const string& name)
   {
     if (name.empty() || (name[0] != '.' && name[0] != ' '))
          return name;
     return name.substr(name.find(" ") + 1);
   }

 // end namespace StringUtility
// }


