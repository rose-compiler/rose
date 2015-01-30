#ifndef ROSE_FileSystem_H
#define ROSE_FileSystem_H

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <vector>

namespace rose {

namespace FileSystem {

/** Pattern to use when creating temporary files. */
extern const char *tempNamePattern;

/** Name of entities in a filesystem. */
typedef boost::filesystem::path Path;

/** Create a temporary directory.
 *
 *  The temporary directory is created as a subdirectory of the directory which is suitable for temporary files under the
 *  conventions of the operating system.  The specifics of how this path is determined are implementation defined (see
 *  <code>boost::filesystem::temp_directory_path</code>).  The created subdirectory has a name of the form
 *  "rose-%%%%%%%%-%%%%%%%%" where each "%" is a random hexadecimal digit.  Returns the path to this directory. */
Path createTemporaryDirectory();

/** Make path relative.
 *
 *  Makes the specified path(s) relative to another path or the current working directory. */
Path makeRelative(const Path &path, const Path &root = boost::filesystem::current_path());

/** Predicate returning true if path exists. */
bool isExisting(const Path &path) { return boost::filesystem::exists(path); }

/** Predicate returning true for existing regular files. */
bool isFile(const Path &path) { return boost::filesystem::is_regular_file(path); }

/** Predicate returning true for existing directories. */
bool isDirectory(const Path &path) { return boost::filesystem::is_directory(path); }

/** Predicate returning true for existing symbolic links. */
bool isSymbolicLink(const Path &path) { return boost::filesystem::is_symlink(path); }

/** Predicate returning inverse of @ref isSymbolicLink. */
bool isNotSymbolicLink(const Path &path) { return !boost::filesystem::is_symlink(path); }

/** Predicate returning true for matching names.
 *
 *  Returns true if and only if the final component of the path matches the specified regular expression.
 *
 *  For example, to find all files whose base name matches the glob "rose_*" use this (note that the corresponding regular
 *  expression is "rose_.*", with a dot):
 *
 * @code
 *  using namespace rose::FileSystem;
 *  Path top = "/foo/bar"; // where the search starts
 *  std::vector<Path> roseFiles = findAllNames(top, baseNameMatches(boost::regex("rose_.*")));
 * @endcode */
class baseNameMatches {
    const boost::regex &re_;
public:
    baseNameMatches(const boost::regex &re): re_(re) {}
    bool operator()(const Path &path) { return boost::regex_match(path.filename().string(), re_); }
};

/** Recursive list of names satisfying predicate.
 *
 *  Returns a list of names by recursively descending into the specified name and returning all names for which the
 *  @p select predicate returns true.  The @p descend predicate should return true for those directories into which the
 *  recursion should descend.  Both predicates are invoked a single argument: the path being tested, which includes @p root
 *  as a prefix.
 *
 *  The predicates are optional (@p select can be omitted only when @p descend is also omitted).  The default is to select
 *  every existing name (files, directories, symbolic links, etc) and descend into every directory (not symbolic links).
 *
 * @{ */
template<class Select, class Descend>
std::vector<Path> findAllNames(const Path &root, Select select, Descend descend) {
    std::vector<Path> matching;
    boost::filesystem::recursive_directory_iterator end;
    for (boost::filesystem::recursive_directory_iterator dentry(root); dentry!=end; ++dentry) {
        if (select(dentry->path()))
            matching.push_back(dentry->path());
        dentry.no_push(!descend(dentry->path()));
    }
    return matching;
}

template<class Select>
std::vector<Path> findAllNames(const Path &root, Select select) {
    return findAllNames(root, select, isDirectory);
}

std::vector<Path> findAllNames(const Path &root) {
    return findAllNames(root, isExisting, isDirectory);
}
/** @} */


/** Return a list of all rose_* files */
std::vector<Path> findRoseFiles(const Path &root) {
    return findAllNames(root, baseNameMatches(boost::regex("rose_.*")), isDirectory);
}

/** Copy files from one directory to another.
 *
 *  Each of the specified files are copied from their location under @p root to a similar location under @p
 *  destinationDirectory. Subdirectories of the destination directory are created as necessary.
 *
 *  Any file whose name is outside the @p root directory will similarly be created outside the @p destinationDirectory.
 *  For instance, copyFiles(["bar/baz"], "foo", "frob") will copy "bar/baz" to "frob/../bar/baz" since "bar" is apparently
 *  a sibling of "foo", and therefore must be a sibling of "frob".
 *
 *  Throws a <code>boost::filesystem::filesystem_error</code> on failure. */
void copyFiles(const std::vector<Path> &files, const Path &root, const Path &destinationDirectory);

/** Recursively copy files.
 *
 *  Get a list of files by recursively matching files under @p root and then copy them to similar locations relative to @p
 *  destination. The @p root and @p destination must not overlap.  The @p select and @p descend arguments are the same as
 *  for the @ref findAllNames method. */
template<class Select, class Descend>
void copyFilesRecursively(const Path &root, const Path &destination, Select select, Descend descend) {
    std::vector<Path> files = findAllNames(root, select, descend);
    files.erase(files.begin(), std::remove_if(files.begin(), files.end(), isFile)); // keep only isFile names
    copyFiles(files, root, destination);
}

} // namespace
} // namespace

#endif
