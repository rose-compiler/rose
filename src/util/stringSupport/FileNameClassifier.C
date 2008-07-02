#include "string_functions.h"

#include <sys/utsname.h>
#include <boost/foreach.hpp>

using namespace std;

namespace StringUtility {

    static bool startsWith(const string& str, const string& substr)
    {
        if (str.length() < substr.length())
            return false;
        int result = str.compare(0, substr.length(), substr);
        return (result == 0);
    }

    static bool endsWith(const string& str, const string& substr)
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
    static const char* LINUX_INCLUDES[] = { "acpi",
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

    static const char* GLIBC_INCLUDES[] = { "_G_config.h", 
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

    static bool
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

    static FileNameLib
    classifyLibrary(const string& fileName)
    {
        if (charListMatches(LINUX_INCLUDES, "include/", fileName))
        {
            return FILENAME_LIB_LINUX;
        }
        if (charListMatches(GLIBC_INCLUDES, "include/", fileName))
        {
            return FILENAME_LIB_C;
        }
        if (fileName.find("lib/gcc") != string::npos)
        {
            return FILENAME_LIB_GCC;
        }
        if (fileName.find("boost") != string::npos)
        {
            return FILENAME_LIB_BOOST;
        }
        if (fileName.find("rose.h") != string::npos)
        {
            return FILENAME_LIB_ROSE;
        }
        if (fileName.find("c++") != string::npos &&
            (endsWith(fileName, "list") ||
             endsWith(fileName, "map") ||
             endsWith(fileName, "set") ||
             endsWith(fileName, "vector") ||
             endsWith(fileName, "string")))
        {
            return FILENAME_LIB_STDCXX;
        }
        return FILENAME_LIB_UNKNOWN;
    }

    OSType
    getOSType()
    {
        struct utsname val;

        int ret = uname(&val);
        if (ret == -1)
            return OS_TYPE_UNKNOWN;

        string sysname = val.sysname;

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
    void
    classifyFileName(const string& fileName, FileNameInfo& result,
                     const vector<string>& appPaths)
    {
        classifyFileName(fileName, result, appPaths, getOSType());
    }

	// Internal function to above public interface, this version
	// is exposed just for testing purposes
    void
    classifyFileName(const string& fileName, FileNameInfo& result,
                     const vector<string>& appPaths, OSType os)
    {
        // Consider all non-absolute paths to be application code
        if (os == OS_TYPE_WINDOWS)
        {
            // Tricky for Windows, make sure it doesn't just start
            // with a drive letter
            if (fileName.length() < 2 || fileName[1] != ':')
            {
                result.location = FILENAME_LOC_USER;
                result.library = FILENAME_LIB_USER;
                return;
            }
        }
        else
        {
            if(!startsWith(fileName, "/"))
            {
                result.location = FILENAME_LOC_USER;
                result.library = FILENAME_LIB_USER;
                return;
            }
        }

        // If this is anywhere in the home dir or whitelist
        // then return that it's part of the application/user code
        BOOST_FOREACH(const string &dir, appPaths)
        {
            if (startsWith(fileName, dir))
            {
                result.location = FILENAME_LOC_USER;
                result.library = FILENAME_LIB_USER;
                return;
            }
        }

        if (os == OS_TYPE_LINUX)
        {
            if (startsWith(fileName, "/usr") ||
                startsWith(fileName, "/opt"))
            {
                result.location = FILENAME_LOC_LIB;
                result.library = classifyLibrary(fileName);
                return;
            }
        }
        else if (os == OS_TYPE_OSX)
        {
            if (startsWith(fileName, "/System"))
            {
                result.location = FILENAME_LOC_LIB;
                result.library = classifyLibrary(fileName);
                return;
            }
        }
        else if (os == OS_TYPE_WINDOWS)
        {
            if (startsWith(fileName, "C:\\Program Files"))
            {
                result.location = FILENAME_LOC_LIB;
                result.library = classifyLibrary(fileName);
                return;
            }
        }
        result.location = FILENAME_LOC_UNKNOWN;
        result.library = FILENAME_LIB_UNKNOWN;
        return;
    }

    static const char* FILENAME_LIBRARY_NAMES[] = { "UNKNOWN",
                                                    "USER",
                                                    "c",
                                                    "stdc++",
                                                    "linux",
                                                    "gcc",
                                                    "boost",
                                                    "rose" };
    static const size_t NAMES_LEN = sizeof(FILENAME_LIBRARY_NAMES) / sizeof(FILENAME_LIBRARY_NAMES[0]);

    const string
    FileNameInfo::getLibraryName() const
    {
        ROSE_ASSERT(this);
        ROSE_ASSERT(library >= 0 && library < (int)NAMES_LEN);
        return FILENAME_LIBRARY_NAMES[library];
    }

}; // end namespace StringUtility
