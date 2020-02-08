#include <sage3basic.h>
#include <SourceLocation.h>

#include <Combinatorics.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <sstream>

namespace Rose {

SAWYER_THREAD_TRAITS::Mutex SourceLocation::classMutex_;
SourceLocation::FileNames SourceLocation::fileNames_;

size_t
SourceLocation::NameHasher::operator()(const NamePtr &fileName) const {
    ASSERT_not_null(fileName);
    Combinatorics::HasherFnv hasher;
    hasher.insert(fileName->native());
    return hasher.partial();
}

bool
SourceLocation::NameEquivalence::operator()(const NamePtr &a, const NamePtr &b) const {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return *a == *b;
}

SourceLocation::SourceLocation(const boost::filesystem::path &fileName, size_t line, const Sawyer::Optional<size_t> &column)
    : line_(line), column_(column) {
    // Exception-safe
    NamePtr newName = NamePtr(new boost::filesystem::path(fileName));
    SAWYER_THREAD_TRAITS::LockGuard lock(classMutex_);
    fileName_ = *fileNames_.insert(newName).first;      // amortized constant time
}

// The destructor should free a file name if no objects reference it anymore. Doing so is optional; the algorithm will still
// function fine if memory is not reclaimed (i.e., commenting out this implementation will not cause incorrect behavior).
SourceLocation::~SourceLocation() {
    if (fileName_ && fileName_.use_count() == 2) {      // name used only by us and the shared index
        SAWYER_THREAD_TRAITS::LockGuard lock(classMutex_);
        fileNames_.erase(fileName_);                    // amortized constant time
    }
}

// class method
SourceLocation
SourceLocation::parse(const std::string &s) {
    // Parse the line and column
    size_t line = 0;
    Sawyer::Optional<size_t> column;
    std::string filePart;
    boost::regex lineCol("(.*):([0-9]+|0x[0-9a-fA-F]+|0b[01]+):([0-9]+|0x[0-9a-fA-F]+|0b[01]+)$");
    boost::regex lineOnly("(.*):([0-9]+|0x[0-9a-fA-F]+|0b[01]+)$");
    boost::smatch matched;
    if (boost::regex_search(s, matched, lineCol)) {
        filePart = matched[1];
        line = boost::lexical_cast<size_t>(matched[2]);
        column = boost::lexical_cast<size_t>(matched[3]);
    } else if (boost::regex_search(s, matched, lineOnly)) {
        filePart = matched[1];
        line = boost::lexical_cast<size_t>(matched[2]);
    } else {
        filePart = s;
    }
    
    // Parse the file name
    std::string fileName;
    if (filePart.size() >= 2 && filePart[0] == '"' && filePart[filePart.size()-1] == '"') {
        fileName = StringUtility::cUnescape(filePart.substr(1, filePart.size()-2));
    } else {
        fileName = filePart;
    }

    return SourceLocation(fileName, line, column);
}

bool
SourceLocation::isEmpty() const {
    return NULL == fileName_;                           // true if and only if default constructed
}

// Returning a reference is okay here. It has the same semantics as if the filename were stored directly in this object;
// namely, the refered name will exist as long as this object exists (or some other object that references this same name
// exists).
const boost::filesystem::path&
SourceLocation::fileName() const {
    // No lock necessary since *this is immutable and we aren't accessing shared state.
    static const boost::filesystem::path empty;
    return fileName_ ? *fileName_ : empty;
}

bool
SourceLocation::operator==(const SourceLocation &other) const {
    // No lock necessary since *this is immutable and we aren't accessing shared state.  Comparing file name pointers is
    // equivalent to comparing file names since we guarantee in the constructor that two objects with the same file name will
    // end up with the same file name pointer.
    return fileName_ == other.fileName_ && line_ == other.line_ && column_.isEqual(other.column_);
}

int
SourceLocation::compare(const SourceLocation &other) const {
    // No lock necessary since *this is immutable and we aren't accessing shared state. Comparing file name pointers is stable
    // since we guarantee in the constructor that two objects with the same file name will end up with the same file name
    // pointer.
    if (fileName_ != other.fileName_)
        return fileName_ < other.fileName_ ? -1 : 1;
    if (line_ != other.line_)
        return line_ < other.line_ ? -1 : 1;
    if (!column_ && !other.column_)
        return 0;
    if (!column_)
        return -1;
    if (!other.column_)
        return 1;
    if (*column_ == *other.column_)
        return 0;
    return *column_ < *other.column_ ? -1 : 1;
}

std::string
SourceLocation::toString() const {
    // No lock necessary since fileName() is used to access the only shared state.
    if (isEmpty())
        return "";
    std::string s = fileName().native() + ":" + boost::lexical_cast<std::string>(line_);
    if (column_)
        s += ":" + boost::lexical_cast<std::string>(*column_);
    return s;
}

std::string
SourceLocation::printableName() const {
    // no lock necessary
    if (isEmpty())
        return "";
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

void
SourceLocation::print(std::ostream &out) const {
    // no lock necessary since fileName is used to access the only shared state
    out <<fileName() <<":" <<line_;
    if (column_)
        out <<":" <<*column_;
}

void
SourceLocation::registerFileName() {
    if (fileName_) {
        SAWYER_THREAD_TRAITS::LockGuard lock(classMutex_);
        fileName_ = *fileNames_.insert(fileName_).first;
    }
}

std::ostream& operator<<(std::ostream &out, const SourceLocation &x) {
    x.print(out);
    return out;
}

} // namespace
