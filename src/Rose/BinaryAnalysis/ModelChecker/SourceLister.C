#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/SourceLister.h>

#include <boost/algorithm/string/trim.hpp>

namespace bfs = boost::filesystem;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

SourceLister::SourceLister() {}

SourceLister::SourceLister(const Settings &settings)
    : settings_(settings) {}

SourceLister::Ptr
SourceLister::instance() {
    return Ptr(new SourceLister);
}

SourceLister::Ptr
SourceLister::instance(const Settings &settings) {
    return Ptr(new SourceLister(settings));
}

boost::format
SourceLister::currentLineFormat() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return settings_.currentLineFormat;
}

void
SourceLister::currentLineFormat(const boost::format &f) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    settings_.currentLineFormat = f;
}

boost::format
SourceLister::contextLineFormat() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return settings_.contextLineFormat;
}

void
SourceLister::contextLineFormat(const boost::format &f) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    settings_.contextLineFormat = f;
}

size_t
SourceLister::leadingContext() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return settings_.leadingContext;
}

void
SourceLister::leadingContext(size_t n) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    settings_.leadingContext = n;
}

size_t
SourceLister::trailingContext() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return settings_.trailingContext;
}

void
SourceLister::trailingContext(size_t n) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    settings_.trailingContext = n;
}

std::vector<bfs::path>
SourceLister::directories() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return settings_.directories;
}

void
SourceLister::directories(const std::vector<bfs::path> &dirs) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    settings_.directories = dirs;
}

void
SourceLister::appendDirectory(const boost::filesystem::path &dir) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    settings_.directories.push_back(dir);
}

std::vector<bfs::path>
SourceLister::decompose(const bfs::path &path_) {
    std::vector<bfs::path> retval;
    bfs::path path = path_.lexically_normal();
    for (const bfs::path &component: path) {
        if (component != "/")
            retval.push_back(component);
    }

    // Replace each member of the vector with its concatenation with all subsequent members.
    if (retval.size() > 0) {
        for (size_t i = retval.size() - 1; i > 0; --i)
            retval[i-1] /= retval[i];
    }

    return retval;
}

bfs::path
SourceLister::findFile(const bfs::path &dir, const std::vector<bfs::path> &names) {
    for (const bfs::path &name: names) {
        bfs::path fullName = dir / name;
        if (bfs::is_regular_file(fullName) && (bfs::status(fullName).permissions() & bfs::owner_read) != 0)
            return fullName;
    }
    return {};
}

bool
SourceLister::list(std::ostream &out, const SourceLocation &sloc, const std::string &prefix) {
    if (sloc.isEmpty())
        return false;

    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (settings_.directories.empty())
        settings_.directories.push_back(boost::filesystem::path("."));

    // Create a lister for this file, or use an existing one.
    std::shared_ptr<Sawyer::Container::LineVector> lineVector;
    lineVector = cache_.getOrDefault(sloc.fileName());
    if (!lineVector) {
        std::vector<bfs::path> endings = decompose(sloc.fileName());
        for (const bfs::path &dir: settings_.directories) {
            bfs::path fileName = findFile(dir, endings);
            if (!fileName.empty()) {
                lineVector = std::make_shared<Sawyer::Container::LineVector>(fileName);
                cache_.insert(sloc.fileName(), lineVector);
                break;
            }
        }
    }

    // If we have a line vector, try to print the requested lines.
    if (lineVector && lineVector->lineChars(sloc.line() - (size_t)1/*overflow okay*/)) {
        // These "idx" variables are zero-origin even though sloc line numbers are 1-origin
        const size_t lineIdx = std::max((size_t)1, sloc.line()) - 1;
        const size_t firstIdx = lineIdx >= settings_.leadingContext ? lineIdx - settings_.leadingContext : 0;
        const size_t lastIdx = lineIdx + settings_.trailingContext; // inclusive

        for (size_t i = firstIdx; i <= lastIdx; ++i) {
            std::string line = lineVector->lineString(i);
            if (line.empty())
                break;                                  // we're past the end of the file
            boost::trim_right(line);
            if (lineIdx == i) {
                out <<prefix <<(settings_.currentLineFormat % (i+1) % line) <<"\n";
            } else {
                out <<prefix <<(settings_.contextLineFormat % (i+1) % line) <<"\n";
            }
        }
        return true;
    } else {
        return false;
    }
}

} // namespace
} // namespace
} // namespace

#endif
