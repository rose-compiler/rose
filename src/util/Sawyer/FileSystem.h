// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_FileSystem_H
#define Sawyer_FileSystem_H

#include <Sawyer/Sawyer.h>
#include <boost/filesystem.hpp>
#include <fstream>

namespace Sawyer {

/** File system utilities. */
namespace FileSystem {

/** Creates a temporary file.
 *
 *  Creates a file with the specified name (or a pseudo-random name in the system temp directory), and make sure it gets
 *  deleted from the file system upon object destruction. */
class SAWYER_EXPORT TemporaryFile {
    boost::filesystem::path name_;
    std::ofstream stream_;

private:
    // Don't depend on C++11's explicit delete of member functions
    TemporaryFile(const TemporaryFile&) { ASSERT_not_reachable("no copy constructor"); }
    TemporaryFile& operator=(const TemporaryFile&) { ASSERT_not_reachable("no assignment operator"); }

public:
    /** Create a temporary file in the system temp directory. */
    TemporaryFile() {
        name_ = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        stream_.open(name_.native().c_str());
    }

    /** Create a temporary file with the specified name. */
    explicit TemporaryFile(const boost::filesystem::path &name) {
        name_ = name;
        stream_.open(name.native().c_str());
    }

    /** Unlink the temporary file from the filesystem.
     *
     *  This also closes the stream if it's open. */
    ~TemporaryFile() {
        stream_.close();
        boost::filesystem::remove(name_);
    }

    /** Path of temporary file. */
    const boost::filesystem::path& name() const { return name_; }

    /** Output stream for temporary file. */
    std::ofstream& stream() { return stream_; }
};

/** Create a temporary directory.
 *
 *  Creates a directory with the specified name (or a pseudo-random name in the system temp directory), and makes sure it gets
 *  deleted recursively upon object destruction. */
class SAWYER_EXPORT TemporaryDirectory {
    boost::filesystem::path name_;

private:
    // Don't depend on C++11's explicit delete of member functions
    TemporaryDirectory(const TemporaryDirectory&) { ASSERT_not_reachable("no copy constructor"); }
    TemporaryDirectory& operator=(const TemporaryDirectory&) { ASSERT_not_reachable("no assignment operator"); }

public:
    /** Create a temporary subdirectory in the system's temp directory.
     *
     *  The directory is recursively unlinked from the filesystem when this object is destroyed. */
    TemporaryDirectory()
        : name_(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path()) {
        createOrThrow();
    }

    /** Create a temporary directory with the specified name.
     *
     *  Creates the specified directory. Parent directories must already exist. The directory is recursively unlinked from the
     *  filesystem when this object is destroyed. */
    explicit TemporaryDirectory(const boost::filesystem::path &name)
        : name_(name) {
        createOrThrow();
    }

    /** Recursively unlink the temporary directory.
     *
     *  This destructor recursively unlinks the directory and its contents from the filesystem, but does not remove any parent
     *  directories even if they would become empty. */
    ~TemporaryDirectory() {
        boost::filesystem::remove_all(name_);
    }

    /** Path of temporary directory. */
    const boost::filesystem::path& name() const { return name_; }

private:
    // Create directory or throw exception
    void createOrThrow() {
        boost::system::error_code ec;
        if (!boost::filesystem::create_directory(name_, ec))
            throw boost::filesystem::filesystem_error("cannot create directory", name_, ec);
    }
};

} // namespace
} // namespace

#endif
