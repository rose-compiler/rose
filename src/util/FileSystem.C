#define BOOST_FILESYSTEM_VERSION 3
#include "FileSystem.h"

#include <boost/foreach.hpp>
#include <set>

namespace rose {
namespace FileSystem {

const char *tempNamePattern = "rose-%%%%%%%-%%%%%%%";

bool
baseNameMatches::operator()(const Path &path) {
    return boost::regex_match(path.filename().string(), re_);
}

Path
createTemporaryDirectory() {
#if BOOST_VERSION >= 104600
    Path dirName = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path(tempNamePattern);
#else
    Path dirName = Path("/tmp") / boost::filesystem::unique_path(tempNamePattern);
#endif
    boost::filesystem::create_directory(dirName);
    return dirName;
}

Path
makeRelative(const Path &path_, const Path &root_) {
    Path path = boost::filesystem::absolute(path_);
    Path root = boost::filesystem::absolute(root_);

    boost::filesystem::path::const_iterator rootIter = root.begin();
    boost::filesystem::path::const_iterator pathIter = path.begin();

    // Skip past common prefix
    while (rootIter!=root.end() && pathIter!=path.end() && *rootIter==*pathIter) {
        ++rootIter;
        ++pathIter;
    }

    // Return value must back out of remaining A components
    Path retval;
    while (rootIter!=root.end()) {
        if (*rootIter++ != ".")
            retval /= "..";
    }

    // Append path components
    while (pathIter!=path.end())
        retval /= *pathIter++;
    return retval;
}

// Copies files to dstDir so that their name relative to dstDir is the same as their name relative to root
void
copyFiles(const std::vector<Path> &fileNames, const Path &root, const Path &dstDir) {
    std::set<Path> dirs;
    BOOST_FOREACH (const Path &fileName, fileNames) {
        Path dirName = dstDir / makeRelative(fileName.parent_path(), root);
        if (dirs.insert(dirName).second)
            boost::filesystem::create_directories(dirName);
        boost::filesystem::copy_file(fileName, dirName / fileName.filename());
    }
}

} // namespace
} // namespace
