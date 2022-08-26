#include <Rose/FileSystem.h>
#include <set>
#include <fstream>

namespace Rose {
namespace FileSystem {

const char *tempNamePattern = "rose-%%%%%%%-%%%%%%%";

bool
baseNameMatches::operator()(const Path &path) {
    return boost::regex_match(path.filename().string(), re_);
}

bool
isExisting(const Path &path) {
    return boost::filesystem::exists(path);
}

bool
isFile(const Path &path) {
    return boost::filesystem::is_regular_file(path);
}

bool
isDirectory(const Path &path) {
    return boost::filesystem::is_directory(path);
}

bool
isSymbolicLink(const Path &path) {
    return boost::filesystem::is_symlink(path);
}

bool
isNotSymbolicLink(const Path &path) {
    return !boost::filesystem::is_symlink(path);
}

Path
createTemporaryDirectory() {
    Path dirName = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path(tempNamePattern);
    boost::filesystem::create_directory(dirName);
    return dirName;
}

Path
makeNormal(const Path &path) {
    std::vector<Path> components;
    for (boost::filesystem::path::const_iterator i=path.begin(); i!=path.end(); ++i) {
        if (0 == i->string().compare("..") && !components.empty()) {
            components.pop_back();
        } else if (0 != i->string().compare(".")) {
            components.push_back(*i);
        }
    }
    Path result;
    for (const Path &component: components)
        result /= component;
    return result;
}

Path
makeAbsolute(const Path &path, const Path &root) {
    return makeNormal(path.is_absolute() ? path : absolute(root / path));
}

Path
makeRelative(const Path &path_, const Path &root_) {
    Path path = makeAbsolute(path_);
    Path root = makeAbsolute(root_);

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

std::vector<Path>
findNames(const Path &root) {
    return findNames(root, isExisting);
}

std::vector<Path>
findNamesRecursively(const Path &root) {
    return findNamesRecursively(root, isExisting, isDirectory);
}

// This doesn't make any sense! First, BOOST_COMPILED_WITH_CXX11 is never defined in any version of boost. Second, even if it
// were defined, it would come from boost header files which are always compiled with the same compile as that which is
// compiling this source file. [Robb Matzke 2016-02-17]
//#if (__cplusplus >= 201103L) 
//#if !defined(BOOST_COMPILED_WITH_CXX11)
//   #warning "Compiling ROSE with C++11 mode: BOOST NOT compiled with C++11 support."
//#else
//   #warning "Compiling ROSE with C++11 mode: BOOST WAS compiled with C++11 support."
//#endif
//#endif

void
copyFile(const Path &src, const Path &dst) {
    // Do not use boost::filesystem::copy_file in boost 1.56 and earlier because it is not possible to cross link c++11 rose
    // with c++89 boost when using this symbol.  Boost issue #6124 fixed in boost 1.57 and later. Our solution is to use C++
    // stream I/O instead, which should still work on non-POSIX systems (Microsoft) although the exception situations might not
    // be exactly precise as POSIX. Use path::string rather than path::native in order to support Filesystem version 2.
    std::ifstream in(src.string().c_str(), std::ios::binary);
    std::ofstream out(dst.string().c_str(), std::ios::binary);
    out <<in.rdbuf();
    if (in.fail()) {
        throw boost::filesystem::filesystem_error("read failed", src,
                                                  boost::system::error_code(errno, boost::system::system_category()));
    }
    if (out.fail()) {
        throw boost::filesystem::filesystem_error("write failed", dst,
                                                  boost::system::error_code(errno, boost::system::system_category()));
    }
}

// Copies files to dstDir so that their name relative to dstDir is the same as their name relative to root
void
copyFiles(const std::vector<Path> &fileNames, const Path &root, const Path &dstDir) {
    std::set<Path> dirs;
    for (const Path &fileName: fileNames) {
        Path dirName = dstDir / makeRelative(fileName.parent_path(), root);
        if (dirs.insert(dirName).second)
            boost::filesystem::create_directories(dirName);
        Path outputName = dirName / fileName.filename();
        copyFile(fileName, outputName);
    }
}

std::vector<Path>
findRoseFilesRecursively(const Path &root) {
    return findNamesRecursively(root, baseNameMatches(boost::regex("rose_.*")), isDirectory);
}

// Don't use this if you can help it!
std::string
toString(const Path &path) {
    return path.generic_string();
}

} // namespace
} // namespace
