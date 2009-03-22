// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"

#include "string_functions.h"

// DQ (3/22/2009): Windows does not have this header.
#if ROSE_MICROSOFT_OS
// Unclear what to use here for Windows.
#else
#include <sys/utsname.h>
#endif

using namespace std;

namespace StringUtility
{

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
        const char* LINUX_INCLUDES[] = { "acpi",
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

        const char* GLIBC_INCLUDES[] = { "_G_config.h", 
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

        FileNameLibrary
        classifyLibrary(const string& fileName)
        {
            if (charListMatches(LINUX_INCLUDES, "include/", fileName))
            {
                return FILENAME_LIBRARY_LINUX;
            }
            if (charListMatches(GLIBC_INCLUDES, "include/", fileName))
            {
                return FILENAME_LIBRARY_C;
            }
            if (fileName.find("lib/gcc") != string::npos)
            {
                return FILENAME_LIBRARY_GCC;
            }
            if (fileName.find("boost") != string::npos)
            {
                return FILENAME_LIBRARY_BOOST;
            }
            if (fileName.find("rose.h") != string::npos)
            {
                return FILENAME_LIBRARY_ROSE;
            }
            if (fileName.find("c++") != string::npos &&
                (endsWith(fileName, "list") ||
                 endsWith(fileName, "map") ||
                 endsWith(fileName, "set") ||
                 endsWith(fileName, "vector") ||
                 endsWith(fileName, "string")))
            {
                return FILENAME_LIBRARY_STDCXX;
            }
            return FILENAME_LIBRARY_UNKNOWN;
        }
    } // end unnamed namespace for file location definitions

    int
    directoryDistance(const string& left, const string& right)
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

    OSType
    getOSType()
    {
#if ROSE_MICROSOFT_OS
        string sysname;
     // We should have a proper implementation instead of defaulting to Windows.
		printf ("Error: uname() not supported in MSVS (not implemented but will default to WINDOWS) \n");
		ROSE_ASSERT(false);
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
        else
            return OS_TYPE_WINDOWS;
    }

    void
    homeDir(string& dir)
    {
        const char* home = getenv("HOME");
        ROSE_ASSERT(home);
        dir = home;
    }

	// Update FileNameInfo class with details about where the
    // file comes from and what library it might be a part of
    FileNameClassification
    classifyFileName(const string& fileName,
                     const string& appPath)
    {
        return classifyFileName(fileName, appPath, getOSType());
    }

	// Internal function to above public interface, this version
	// is exposed just for testing purposes
    FileNameClassification
    classifyFileName(const string& fileName,
                     const string& appPathConst, OSType os)
    {
        string appPath = appPathConst;

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
                                              FILENAME_LIBRARY_USER,
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
                                              FILENAME_LIBRARY_USER,
                                              0);
            }
        }

        // If this is anywhere in the home dir or whitelist
        // then return that it's part of the application/user code
        if (startsWith(fileName, appPath))
        {
            return FileNameClassification(FILENAME_LOCATION_USER,
                                          FILENAME_LIBRARY_USER,
                                          0);
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
                                      FILENAME_LIBRARY_UNKNOWN,
                                      directoryDistance(fileName, appPath));
    }

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
    FileNameClassification::getLibraryName() const
    {
        ROSE_ASSERT(this);
        ROSE_ASSERT(library >= 0 && library < (int)FILENAME_NAMES_SIZE);
        return FILENAME_NAMES[library];
    }

    const string
    stripDotsFromHeaderFileName(const string& name)
    {
        if (name.empty() || (name[0] != '.' && name[0] != ' '))
            return name;
        return name.substr(name.find(" ") + 1);
    }

} // end namespace StringUtility
