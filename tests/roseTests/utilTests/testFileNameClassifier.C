#include "rose.h"
#include "string_functions.h"

#include <vector>

using namespace std;
using namespace StringUtility;

int
main(int argc, char** argv)
{
    FileNameClassification classification;

    string home;
    
    // One would generally populate home here, but we fix the value
    // to make sure these tests are deterministic on across machines
    // homeDir(home);
    home = "/home/stutsman1";

    string slashes = "/////";

    classification = classifyFileName("/usr/include/stdio.h",
                                      home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_C);
    ROSE_ASSERT(classification.getLibraryName() == "c");

    classification = classifyFileName(home + "/include/stdio.h",
                                      home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_USER);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_USER);

    // TODO leading slashes are legal UNIX paths, but its doubtful
    // that the compiler will generate such paths anyways so
    // perhaps this doesn't matter
    classification = classifyFileName(slashes + home + "/include/stdio.h",
                                      home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_UNKNOWN);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_UNKNOWN);
    ROSE_ASSERT(classification.getLibraryName() == "UNKNOWN");

    // TODO we'd like this to be able to classifyFileName this as a library
    // still instead of lumping it in with the application
    // for now this may be okay, though
    classification = classifyFileName(home + "/usr/include/boost/boost-1_35/filesystem.hpp",
                                      home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_USER);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_USER);
    ROSE_ASSERT(classification.getLibraryName() == "USER");

    classification = classifyFileName("/usr/include/boost/boost-1_35/filesystem.hpp",
                                      home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_BOOST);
    ROSE_ASSERT(classification.getLibraryName() == "boost");

    classification = classifyFileName("/usr/include/rose.h", home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_ROSE);
    ROSE_ASSERT(classification.getLibraryName() == "rose");

    classification = classifyFileName("/usr/include/c++/3.4.3/string",
                                      home);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_STDCXX);
    ROSE_ASSERT(classification.getLibraryName() == "stdc++");

    // #### OS X tests ####

    // NOTE: Don't use this overloaded call to classifyFileName in real code
    // it is used here only so that OSX and Windows cases can be tested
    // on GNU/Linux

    home = "/Users/stutsman1";

    OSType os = OS_TYPE_OSX;

    classification = classifyFileName("/System/Library/Frameworks/Tk.framework/Headers/tk.h",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_UNKNOWN);

    classification = classifyFileName("/usr/include/stdio.h",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_C);

    classification = classifyFileName(home + "/include/stdio.h",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_USER);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_USER);

    classification = classifyFileName(slashes + home + "/include/stdio.h",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_UNKNOWN);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_UNKNOWN);

    classification = classifyFileName(home + "/usr/include/boost/boost-1_35/filesystem.hpp",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_USER);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_USER);

    // #### Windows tests ####

    home = "C:\\Documents and Settings\\stutsman1";

    os = OS_TYPE_WINDOWS;

    classification = classifyFileName("C:\\Program Files\\Microsoft Visual Studio"
                                      "\\MSVC\\Include\\stdio.h",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_LIBRARY);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_UNKNOWN);

    classification = classifyFileName("C:\\Documents and Settings\\stutsman1"
                                      "\\Includes\\stdio.h",
                                      home, os);
    ROSE_ASSERT(classification.getLocation() == FILENAME_LOCATION_USER);
    ROSE_ASSERT(classification.getLibrary() == FILENAME_LIBRARY_USER);

    return 0;
}
