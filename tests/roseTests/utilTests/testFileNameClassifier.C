#include "rose.h"
#include "string_functions.h"

#include <vector>

using namespace std;
using namespace StringUtility;

int
main(int argc, char** argv)
{
    FileNameInfo r;

    vector<string> a;
    string home;
    
    // One would generally populate home here, but we fix the value
    // to make sure these tests are deterministic on across machines
    // homeDir(home);
    home = "/home/stutsman1";
    a.push_back(home);

    string slashes = "/////";

    classifyFileName("/usr/include/stdio.h", r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_C);
    ROSE_ASSERT(r.getLibraryName() == "c");

    classifyFileName(home + "/include/stdio.h", r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_USER);
    ROSE_ASSERT(r.library == FILENAME_LIB_USER);

    // TODO leading slashes are legal UNIX paths, but its doubtful
    // that the compiler will generate such paths anyways so
    // perhaps this doesn't matter
    classifyFileName(slashes + home + "/include/stdio.h", r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_UNKNOWN);
    ROSE_ASSERT(r.library == FILENAME_LIB_UNKNOWN);
    ROSE_ASSERT(r.getLibraryName() == "UNKNOWN");

    // TODO we'd like this to be able to classifyFileName this as a library
    // still instead of lumping it in with the application
    // for now this may be okay, though
    classifyFileName(home + "/usr/include/boot/boost-1_35/filesystem.hpp",
                     r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_USER);
    ROSE_ASSERT(r.library == FILENAME_LIB_USER);
    ROSE_ASSERT(r.getLibraryName() == "USER");

    classifyFileName("/usr/include/boot/boost-1_35/filesystem.hpp", r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_BOOST);
    ROSE_ASSERT(r.getLibraryName() == "boost");

    classifyFileName("/usr/include/rose.h", r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_ROSE);
    ROSE_ASSERT(r.getLibraryName() == "rose");

    classifyFileName("/usr/include/c++/3.4.3/string", r, a);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_STDCXX);
    ROSE_ASSERT(r.getLibraryName() == "stdc++");

    // #### OS X tests ####

    // NOTE: Don't use this overloaded call to classifyFileName in real code
    // it is used here only so that OSX and Windows cases can be tested
    // on GNU/Linux

    home = "/Users/stutsman1";
    a.clear();
    a.push_back(home);

    OSType os = OS_TYPE_OSX;

    classifyFileName("/System/include/stdio.h", r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_C);

    classifyFileName(home + "/include/stdio.h", r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_USER);
    ROSE_ASSERT(r.library == FILENAME_LIB_USER);

    classifyFileName(slashes + home + "/include/stdio.h", r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_UNKNOWN);
    ROSE_ASSERT(r.library == FILENAME_LIB_UNKNOWN);

    classifyFileName(home + "/usr/include/boot/boost-1_35/filesystem.hpp",
                     r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_USER);
    ROSE_ASSERT(r.library == FILENAME_LIB_USER);

    classifyFileName("/System/include/boot/boost-1_35/filesystem.hpp",
                     r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_BOOST);

    classifyFileName("/System/include/rose.h", r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_ROSE);

    // #### Windows tests ####

    home = "C:\\Documents and Settings\\stutsman1";
    a.clear();
    a.push_back(home);

    os = OS_TYPE_WINDOWS;

    classifyFileName("C:\\Program Files\\Microsoft Visual Studio"
                     "\\MSVC\\Include\\stdio.h", r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_LIB);
    ROSE_ASSERT(r.library == FILENAME_LIB_UNKNOWN);

    classifyFileName("C:\\Documents and Settings\\stutsman1"
                     "\\Includes\\stdio.h", r, a, os);
    ROSE_ASSERT(r.location == FILENAME_LOC_USER);
    ROSE_ASSERT(r.library == FILENAME_LIB_USER);

    return 0;
}
