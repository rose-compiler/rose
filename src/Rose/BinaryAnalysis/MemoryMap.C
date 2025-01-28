#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/MemoryMap.h>

#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/SRecord.h>
#include <Rose/Diagnostics.h>
#include <Rose/FileSystem.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>
#include <rose_getline.h>
#include <rose_strtoull.h>

#include <Sawyer/Parse.h>
#include <Sawyer/Result.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/scope_exit.hpp>
#include <regex>

#include <boost/config.hpp>

#ifndef BOOST_WINDOWS
# include <fcntl.h>                                     // for open()
# include <sys/ptrace.h>                                // for ptrace()
# include <sys/stat.h>                                  // for stat()
# include <sys/wait.h>                                  // for waitpid()
# include <unistd.h>                                    // for access()
#endif

#if defined(__APPLE__) && defined(__MACH__)
#  define PTRACE_ATTACH PT_ATTACHEXC
#  define PTRACE_DETACH PT_DETACH
#endif

// This is the other half of the BOOST_CLASS_EXPORT_KEY from the header file.
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::MemoryMap::AllocatingBuffer);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::MemoryMap::MappedBuffer);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::MemoryMap::NullBuffer);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::MemoryMap::StaticBuffer);
#endif

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {


std::ostream& operator<<(std::ostream &o, const MemoryMap &x) { x.print(o); return o; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
MemoryMap::Exception::leader(std::string dflt) const {
    const char *s = what();
    return s && *s ? std::string(s) : dflt;
}

std::string
MemoryMap::Exception::details(bool verbose) const {
    std::ostringstream ss;
    if (verbose) {
        ss <<"\n";
        if (map)
            map->dump(ss, "  ");
    }
    return ss.str();
}

void
MemoryMap::Exception::print(std::ostream &o, bool verbose) const {
    o <<leader("problem") <<details(verbose);
}

void
MemoryMap::Inconsistent::print(std::ostream &o, bool verbose) const {
    o <<leader("inconsistent mapping") <<" for " <<new_range <<" vs. " <<old_range <<details(verbose);
}

void
MemoryMap::NotMapped::print(std::ostream &o, bool verbose) const {
    o <<leader("no mapping") <<" at va " <<StringUtility::addrToString(va) <<details(verbose);
}

void
MemoryMap::NoFreeSpace::print(std::ostream &o, bool verbose) const {
    o <<leader("no free space") <<" (nbytes=" <<size <<")" <<details(verbose);
}

void
MemoryMap::SyntaxError::print(std::ostream &o, bool verbose) const {
    o <<leader("syntax error");
    if (!filename.empty()) {
        o <<" at " <<filename <<":" <<linenum;
        if (colnum>0)
            o <<"." <<colnum;
    }
    o <<details(verbose);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Buffer methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
MemoryMap::segmentTitle(const Segment &segment) {
    std::string s;

    s += (segment.accessibility() & READABLE)  !=0 ? "r" : "-";
    s += (segment.accessibility() & WRITABLE)  !=0 ? "w" : "-";
    s += (segment.accessibility() & EXECUTABLE)!=0 ? "x" : "-";
    s += (segment.accessibility() & PRIVATE)   !=0 ? "p" : "-";
    unsigned otherAccess = segment.accessibility() & ~(READABLE|WRITABLE|EXECUTABLE|PRIVATE);

    std::string bufname = segment.buffer()->name();
    if (bufname.find_first_of(" \t\n()")==std::string::npos)
        bufname = "buffer " + bufname;
    s += " at " + (bufname+std::string(12, ' ')).substr(0, 12);

    s += " + " + StringUtility::addrToString(segment.offset());

    if (otherAccess != 0)
        s += " access=" + StringUtility::addrToString(otherAccess, 8*sizeof otherAccess);

    if (!segment.name().empty()) {
        static const size_t limit = 100;
        std::string name = StringUtility::cEscape(segment.name());
        if (name.size()>limit)
            name = name.substr(0, limit-3) + "...";
        s += " " + name;
    }
    return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       MemoryMap methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string&
MemoryMap::name() const {
    return name_;
}

void
MemoryMap::name(const std::string &s) {
    name_ = s;
}

size_t
MemoryMap::insertFile(const std::string &fileName, rose_addr_t startVa, InsertFileMapMode mode, std::string segmentName) {
    if (segmentName.empty())
        segmentName = FileSystem::toString(boost::filesystem::path(fileName).filename());
    unsigned accessBits = 0;
    switch (mode) {
        case MAP_PRIVATE:
            accessBits = MemoryMap::READ_WRITE | MemoryMap::PRIVATE;
            break;
        case MAP_RDONLY:
            accessBits = MemoryMap::READABLE;
            break;
        case MAP_READWRITE:
            accessBits = MemoryMap::READ_WRITE;
            break;
    }

    Segment segment = Segment::fileInstance(fileName, accessBits, segmentName);
    AddressInterval fileInterval = AddressInterval::baseSize(startVa, segment.buffer()->size());
    insert(fileInterval, segment);
    return fileInterval.size();
}

static std::runtime_error
insertFileError(const std::string &locatorString, const std::string &mesg) {
    throw std::runtime_error("MemoryMap::insertFile: " + mesg + " in \"" + StringUtility::cEscape(locatorString) + "\"");
}

template<typename UnsignedInteger>
static Sawyer::Optional<UnsignedInteger>
parseInteger(const char *&s) {
    char *rest = const_cast<char*>(s);
    errno = 0;
    UnsignedInteger n = rose_strtoull(s, &rest, 0);
    if (errno!=0 || rest==s)
        return Sawyer::Nothing();
    s = rest;
    return n;
}

std::string
MemoryMap::insertFileDocumentation() {
    return ("Beginning with the first colon, a memory map resource string has the form "
            "\":@v{memory_properties}:@v{file_properties}:@v{file_name}\" where @v{memory_properties} and "
            "@v{file_properties} are optional but the three colons are always required.  The @v{memory_properties} "
            "have the form \"[@v{address}][+@v{vsize}][=@v{access}]\" where each of the items is optional (indicated by "
            "the square brackets which should not be present in the actual resource string). The @v{address} is the "
            "starting address where the file will be mapped and defaults to the address of the lowest unmapped interval "
            "that is large enough to hold the new map segment; @v{vsize} is the size in bytes of the interval to be "
            "mapped, defaulting to the size of the file data; and @v{access} is the accessibility represented by "
            "zero or more of the characters \"r\" (readable), \"w\" (writable), and \"x\" (executable) in that order and "
            "defaulting to the accessibility of the file.  The @v{file_properties} have the form "
            "\"[@v{offset}][+@v{fsize}]\" where @v{offset} is an offset from the beginning of the file defaulting to zero; "
            "@v{size} is the number of bytes to read from the file, defaulting to the amount of data that is available. "
            "If @v{vsize} is specified then exactly that many bytes are mapped by zero-padding the file data if necessary; "
            "otherwise, when @v{fsize} is specified then exactly @v{fsize} bytes are mapped by zero padding the file data "
            "that could be read; otherwise the file size (adjusted by @v{offset}) determines the mapped size. The numeric "
            "properties can be specified in decimal, hexadecimal, or binary using the usual C syntax (leading \"0x\" for "
            "hexadecimal, leading \"0b\" for binary, otherwise decimal).");
}

// Insert file from a locator string of the form:
//   :[VA][+VSIZE][=PERMS]:[OFFSET][+FSIZE]:FILENAME
AddressInterval
MemoryMap::insertFile(const std::string &locatorString) {

    // These resources need to be cleaned up for all returns and exceptions
    struct Resources {
        uint8_t *data;                                  // line read from a file, allocated with operator new[]
        Resources(): data(NULL) {}
        ~Resources() { delete[] data; }
    } r;

    //--------------------------------------
    // Parse the parts of the locator string
    //--------------------------------------

    // Leading colon
    const char *s = locatorString.c_str();
    if (':'!=*s++)
        throw insertFileError(locatorString, "not a locator string");

    // Virtual address
    Sawyer::Optional<rose_addr_t> optionalVa;
    if (isdigit(*s)) {
        optionalVa = parseInteger<rose_addr_t>(s /*in,out*/);
        if (!optionalVa)
            throw insertFileError(locatorString, "virtual address expected");
    }

    // Virtual size
    Sawyer::Optional<size_t> optionalVSize;
    if ('+'==*s) {
        ++s;
        optionalVSize = parseInteger<size_t>(s /*in,out*/);
        if (!optionalVSize)
            throw insertFileError(locatorString, "virtual size expected");
    }

    // Virtual accessibility
    Sawyer::Optional<unsigned> optionalAccess;
    if ('='==*s) {
        ++s;
        unsigned a = 0;
        if ('r'==*s) {
            ++s;
            a |= READABLE;
        }
        if ('w'==*s) {
            ++s;
            a |= WRITABLE;
        }
        if ('x'==*s) {
            ++s;
            a |= EXECUTABLE;
        }
        optionalAccess = a;
    }

    // Second colon
    if (':'!=*s) {
        if (*s && optionalAccess)
            throw insertFileError(locatorString, "invalid access spec");
        throw insertFileError(locatorString, "syntax error before second colon");
    }
    ++s;

    // File offset
    Sawyer::Optional<size_t> optionalOffset;
    if (isdigit(*s)) {
        optionalOffset = parseInteger<size_t>(s /*in,out*/);
        if (!optionalOffset)
            throw insertFileError(locatorString, "file offset expected");
    }

    // File size
    Sawyer::Optional<size_t> optionalFSize;
    if ('+'==*s) {
        ++s;
        optionalFSize = parseInteger<size_t>(s /*in,out*/);
        if (!optionalFSize)
            throw insertFileError(locatorString, "file size expected");
    }

    // Third colon
    if (':'!=*s)
        throw insertFileError(locatorString, "syntax error before third colon");
    ++s;

    // File name
    if (!*s)
        throw insertFileError(locatorString, "file name expected after third colon");
    std::string fileName = s;
    if (fileName.size()!=strlen(fileName.c_str()))
        throw insertFileError(locatorString, "invalid file name");
    std::string segmentName = FileSystem::toString(boost::filesystem::path(fileName).filename());

    //--------------------------------
    // Open the file and read the data
    //--------------------------------

    // Open the file and seek to the start of data
    std::ifstream file(fileName.c_str());
    if (!file.good())
        throw std::runtime_error("MemoryMap::insertFile: cannot open file \""+StringUtility::cEscape(fileName)+"\"");
    if (optionalOffset)
        file.seekg(*optionalOffset);
    if (!file.good())
        throw std::runtime_error("MemoryMap::insertFile: cannot seek in file \""+StringUtility::cEscape(fileName)+"\"");

    // If no file size was specified then try to get one, or delay getting one until later.  On POSIX systems we can use stat
    // to get the file size, which is useful because infinite devices (like /dev/zero) will return zero.  Otherwise we'll get
    // the file size by trying to read from the file.
#if !defined(BOOST_WINDOWS)                             // not targeting Windows; i.e., not Microsoft C++ and not MinGW
    if (!optionalFSize) {
        struct stat sb;
        if (0==stat(fileName.c_str(), &sb))
            optionalFSize = sb.st_size;
    }
#endif

    // Limit the file size according to the virtual size.  We never need to read more than what would be mapped.
    if (optionalVSize) {
        if (optionalFSize) {
            optionalFSize = std::min(*optionalFSize, *optionalVSize);
        } else {
            optionalFSize = optionalVSize;
        }
    }

    // Read the file data.  If we know the file size then we can allocate a buffer and read it all in one shot, otherwise we'll
    // have to read a little at a time (only happens on Windows due to stat call above).
    size_t nRead = 0;                                   // bytes of data actually allocated, read, and initialized in "data"
    if (optionalFSize) {
        // This is reasonably fast and not too bad on memory
        if (0 != *optionalFSize) {
            r.data = new uint8_t[*optionalFSize];
            file.read((char*)r.data, *optionalFSize);
            nRead = file.gcount();
            if (nRead != *optionalFSize)
                throw std::runtime_error("MemoryMap::insertFile: short read from \""+StringUtility::cEscape(fileName)+"\"");
        }
    } else {
        while (file.good()) {
            uint8_t page[4096];
            file.read((char*)page, sizeof page);
            size_t n = file.gcount();
            uint8_t *tmp = new uint8_t[nRead + n];
            memcpy(tmp, r.data, nRead);
            memcpy(tmp+nRead, page, n);
            delete[] r.data;
            r.data = tmp;
            nRead += n;
        }
        optionalFSize = nRead;
    }

    // Choose virtual size
    if (!optionalVSize) {
        ASSERT_require(optionalFSize);
        optionalVSize = optionalFSize;
    }

    // Choose accessibility
    if (!optionalAccess) {
#ifdef BOOST_WINDOWS
        optionalAccess = READABLE | WRITABLE;
#else
        unsigned a = 0;
        if (0==::access(fileName.c_str(), R_OK))
            a |= READABLE;
        if (0==::access(fileName.c_str(), W_OK))
            a |= WRITABLE;
        if (0==::access(fileName.c_str(), X_OK))
            a |= EXECUTABLE;
        optionalAccess = a;
#endif
    }

    // Find a place to map the file.
    if (!optionalVa) {
        ASSERT_require(optionalVSize);
        optionalVa = findFreeSpace(*optionalVSize);
    }

    // Adjust the memory map
    ASSERT_require(optionalVa);
    ASSERT_require(optionalVSize);
    ASSERT_require(optionalAccess);
    ASSERT_require(nRead <= *optionalVSize);
    if (0 == *optionalVSize)
        return AddressInterval();                       // empty
    AddressInterval interval = AddressInterval::baseSize(*optionalVa, *optionalVSize);
    insert(interval, Segment::anonymousInstance(interval.size(), *optionalAccess, segmentName));
    size_t nCopied = at(interval.least()).limit(nRead).write(r.data).size();
    ASSERT_always_require(nRead==nCopied);              // better work since we just created the segment!
    return interval;
}

std::string
MemoryMap::adjustMapDocumentation() {
    return ("Beginning with the first colon, a meta resource string has the form "
            "\":@v{region}:@v{command}\" The @v{region} describes the region of the virtual address space on "
            "which the @v{command} applies, and is of the form \"@v{address}[+@v{size}]\". If the @v{size} is "
            "absent then the region extends to the maximum address. The following @v{command} forms are recognized:"

            "@named{Print meta information}{The \"print\" command prints meta information about the mapping to standard output. "
            "This command ignores the @v{region}, always printing the entire map.}"

            "@named{Change permissions}{The \"perm @v{op} @v{value}\" command changes the permissions of mapped memory "
            "that falls within the @v{region} (white space is optional). The operation, @v{op} is one of \"=\" to set "
            "the permissions as specified, \"-\" to remove the specified permissions, or \"+\" to add the specified "
            "permissions. The permissions are any combination of the letters \"r\" (read), \"w\" (write), and \"x\" (execute) "
            "in any order.}"

            "@named{Remove mapping}{The \"unmap\" command unmaps the specified region of memory.}"

            "@named{Hexdump output}{The \"hexdump[=@v{filename}]\" prints the region data to either standard output "
            "or the specified file (creating or truncating it first) using a format similar to the \"hexdump -Cv\" command.}"

            "@named{S-Record output}{The \"srec[=@v{filename}]\" prints the region data to either standard output "
            "or the specified file (creating or truncating it first) using Motorola S-Record format.}"

            "@named{Intel HEX output}{The \"hex[=@v{filename}]\" prints the region data to either standard output "
            "or the specified file (creating or truncating it first) using Intel HEX format.}");
}

static std::runtime_error
adjustMapError(const std::string &locatorString, const std::string &mesg) {
    throw std::runtime_error("MemoryMap::adjust: " + mesg + " in \"" + StringUtility::cEscape(locatorString) + "\"");
}

void
MemoryMap::adjustMap(const std::string &locatorString) {
    const char *s = locatorString.c_str();
    if (':' != *s++)
        throw adjustMapError(locatorString, "not a locator string");

    // Addresses to be adjusted
    AddressIntervalSet where;
    while (isspace(*s)) ++s;
    if ('/' == *s) {
        // Addresses are all the regions whose name matches the specified regular expression. The regular
        // expression ends at the next occurrance of "/:"
        const char *terminator = strstr(s+1, "/:");
        if (!terminator)
            throw adjustMapError(locatorString, "regular expression must end with \"/:\"");
        std::string reStr(s+1, terminator - (s+1));
        std::regex re(reStr);
        for (const Node &node: nodes()) {
            if (std::regex_search(node.value().name(), re))
                where.insert(node.key());
        }
        s = terminator + 1;

    } else {
        // Region is specified by a starting address and a size in bytes.
        Sawyer::Optional<rose_addr_t> va = parseInteger<rose_addr_t>(s /*in,out*/);
        if (!va)
            throw adjustMapError(locatorString, "virtual address expected");

        // Size of region
        AddressInterval region;
        while (isspace(*s)) ++s;
        if ('+' == *s) {
            ++s;
            Sawyer::Optional<rose_addr_t> size = parseInteger<rose_addr_t>(s /*in,out*/);
            if (!size)
                throw adjustMapError(locatorString, "region size expected after '+'");
            if (!*size)
                throw adjustMapError(locatorString, "region cannot be empty");
            region = AddressInterval::baseSize(*va, *size);
        } else {
            region = AddressInterval::hull(*va, AddressInterval::whole().greatest());
        }
        where.insert(region);
    }

    // Commands
    while (isspace(*s)) ++s;
    if (':' != *s++)
        throw adjustMapError(locatorString, "expected colon after region specification");
    while (isspace(*s)) ++s;
    if (!strncmp(s, "perm", 4)) {
        // match /perm[-+=][rwx]+/
        s += 4;
        while (isspace(*s)) ++s;
        if ('+' != *s && '-' != *s && '=' != *s)
            throw adjustMapError(locatorString, "invalid perm operator");
        char op = *s++;
        unsigned perm = 0;
        while ('r' == *s || 'w' == *s || 'x' == *s) {
            switch (*s++) {
                case 'r':
                    perm |= READABLE;
                    break;
                case 'w':
                    perm |= WRITABLE;
                    break;
                case 'x':
                    perm |= EXECUTABLE;
                    break;
                default:
                    ASSERT_not_reachable("invalid permission operator");
            }
        }
        for (const AddressInterval &region: where.intervals()) {
            switch (op) {
                case '-':
                    within(region).changeAccess(0, perm);
                    break;
                case '+':
                    within(region).changeAccess(perm, 0);
                    break;
                case '=':
                    within(region).changeAccess(perm, ~perm);
                    break;
            }
        }

    } else if (!strncmp(s, "hexdump", 7)) {
        s += 7;

        // Open the output stream, or use standard output
        std::ofstream fout;
        if ('=' == *s) {
            s++;
            fout.open(s);
            if (!fout)
                throw adjustMapError(locatorString, "failed to write to \"" + StringUtility::cEscape(s) + "\"");
            s = "";
        }
        std::ostream &out = fout.is_open() ? fout : std::cout;

        HexdumpFormat fmt;
        for (AddressInterval region: where.intervals()) {
            while (AddressInterval selected = atOrAfter(region.least()).singleSegment().available()) {
                selected = selected & region;
                rose_addr_t va = selected.least();
                const ConstNodeIterator inode = at(va).nodes().begin();
                const MemoryMap::Segment &segment = inode->value();
                rose_addr_t bufferOffset = segment.offset() + selected.least() - inode->key().least();
                const uint8_t *data = segment.buffer()->data() + bufferOffset;
                out <<"# segment " <<segmentTitle(segment) <<"\n";

                // Hexdumps are typically aligned so the first byte on each line is aligned on a 16-byte address, so print out some
                // stuff to get the rest aligned if necessary.
                rose_addr_t nRemain = selected.size();
                rose_addr_t nLeader = std::min(16 - va % 16, nRemain);
                if (nLeader != 16) {
                    hexdump(out, va, data, nLeader, fmt);
                    va += nLeader;
                    data += nLeader;
                    nRemain -= nLeader;
                    out <<"\n";
                }
                if (nRemain > 0) {
                    hexdump(out, va, data, nRemain, fmt);
                    out <<"\n";
                }
                if (selected.greatest() == region.greatest())
                    break;
                region = AddressInterval::hull(selected.greatest()+1, region.greatest());
            }
        }

    } else if (!strncmp(s, "srec", 4)) {
        s += 4;

        // Open the output stream, or use standard output
        std::ofstream fout;
        if ('=' == *s) {
            s++;
            fout.open(s);
            if (!fout)
                throw adjustMapError(locatorString, "failed to write to \"" + StringUtility::cEscape(s) + "\"");
            s = "";
        }
        std::ostream &out = fout.is_open() ? fout : std::cout;

        for (const AddressInterval &region: where.intervals()) {
            MemoryMap::Ptr tmpMap = shallowCopy();
            tmpMap->at(region).keep();
            if (!tmpMap->isEmpty()) {
                for (const SRecord &srec: SRecord::create(tmpMap, SRecord::SREC_MOTOROLA))
                    out <<srec.toString() <<"\n";

            }
        }

    } else if (!strncmp(s, "hex", 3)) {
        s += 3;

        // Open the output stream, or use standard output
        std::ofstream fout;
        if ('=' == *s) {
            s++;
            fout.open(s);
            if (!fout)
                throw adjustMapError(locatorString, "failed to write to \"" + StringUtility::cEscape(s) + "\"");
            s = "";
        }
        std::ostream &out = fout.is_open() ? fout : std::cout;

        for (const AddressInterval &region: where.intervals()) {
            MemoryMap::Ptr tmpMap = shallowCopy();
            tmpMap->at(region).keep();
            if (!tmpMap->isEmpty()) {
                for (const SRecord &srec: SRecord::create(tmpMap, SRecord::SREC_INTEL))
                    out <<srec.toString() <<"\n";
            }
        }

    } else if (!strncmp(s, "print", 5)) {
        s += 5;
        dump();

    } else if (!strncmp(s, "unmap", 5)) {
        s += 5;
        for (const AddressInterval &region: where.intervals())
            within(region).prune();

    } else {
        throw adjustMapError(locatorString, "unrecognized command");
    }

    while (isspace(*s)) ++s;
    if (*s)
        throw adjustMapError(locatorString, "unexpected extra text after command");
}

std::string
MemoryMap::insertDataDocumentation() {
    return ("Beginning with the first colon, a data resource string has the form "
            "\":@v{memory_properties}:@v{data_properties}:@v{data}\" where @v{memory_properties} and "
            "@v{data_properties} are optional but the three colons are always required. The @v{memory_properties} "
            "have the form \"[@v{address}][+@v{vsize}][=@v{access}]\" where each of the items is optional (indicated by "
            "the square brackets which should not be present in the actual resource string). The @v{address} is the "
            "starting address where the data will be mapped and defaults to the address of the lowest unmapped interval "
            "that is large enough to hold the new map segment; @v{vsize} is the size in bytes of the interval to be "
            "mapped, default to the size of the data; and @v{access} is the accessibility represented by "
            "zero or more of the characters \"r\" (readable), \"w\" (writable), and \"x\" (executable) in that order and "
            "defaulting to read, write, and execute.  No @v{data_properties} are defined at this time, so the "
            "@v{data_properties} string is always empty. The @v{data} is a space-separated list of byte values in "
            "decimal, hexadecimal (0x), or binary (0b) using the usual C syntax.  If the @v{vsize} is larger "
            "than the amount of @v{data} then the data will be zero-padded.");
}

static std::runtime_error
insertDataError(const std::string &locatorString, const std::string &mesg) {
    throw std::runtime_error("MemoryMap::insertData: " + mesg + " in \"" + StringUtility::cEscape(locatorString) + "\"");
}

AddressInterval
MemoryMap::insertData(const std::string &locatorString) {
    const char *s = locatorString.c_str();
    if (':' != *s++)
        throw insertDataError(locatorString, "not a locator string");

    // Virtual address
    Sawyer::Optional<rose_addr_t> optionalVa;
    if (isdigit(*s)) {
        optionalVa = parseInteger<rose_addr_t>(s /*in,out*/);
        if (!optionalVa)
            throw insertDataError(locatorString, "virtual address expected");
    }

    // Virtual size
    Sawyer::Optional<size_t> optionalVSize;
    if ('+' == *s) {
        ++s;
        optionalVSize = parseInteger<size_t>(s /*in,out*/);
        if (!optionalVSize)
            throw insertDataError(locatorString, "virtual size expected");
    }

    // Virtual accessibility
    unsigned accessFlags = READ_WRITE_EXECUTE;
    if ('='==*s) {
        ++s;
        accessFlags = 0;
        if ('r'==*s) {
            ++s;
            accessFlags |= READABLE;
        }
        if ('w'==*s) {
            ++s;
            accessFlags |= WRITABLE;
        }
        if ('x'==*s) {
            ++s;
            accessFlags |= EXECUTABLE;
        }
    }

    // Second colon
    if (':'!=*s) {
        if (*s && accessFlags)
            throw insertDataError(locatorString, "invalid access specification");
        throw insertDataError(locatorString, "syntax error before second colon");
    }
    ++s;

    // Third colon
    if (':'!=*s)
        throw insertDataError(locatorString, "syntax error before third colon");
    ++s;

    // The data
    std::vector<uint8_t> data;
    while (1) {
        while (isspace(*s)) ++s;
        if (!*s)
            break;
        rose_addr_t u = 0;
        if (!parseInteger<rose_addr_t>(s /*in,out*/).assignTo(u))
            throw insertDataError(locatorString, "expected numeric value for byte " + StringUtility::numberToString(data.size()));
        if (u > 0xff)
            throw insertDataError(locatorString, "value " + StringUtility::numberToString(u) +
                                  " is out of bounds for byte " + StringUtility::numberToString(data.size()));
        data.push_back(u);
    }
    if (!optionalVSize) {
        optionalVSize = data.size();
    } else if (*optionalVSize > data.size()) {
        mlog[WARN] <<"data (" <<StringUtility::plural(data.size(), "bytes") <<") will be truncated to"
                   <<" specified segment size (" <<StringUtility::plural(*optionalVSize, "bytes") <<")\n";
    } else if (0 == *optionalVSize) {
        mlog[WARN] <<"data is empty; nothing to map for \"" <<StringUtility::cEscape(locatorString) <<"\"\n";
        return AddressInterval();
    }

    // Find a place to map the file
    ASSERT_require(optionalVSize);
    if (!optionalVa) {
        optionalVa = findFreeSpace(*optionalVSize);
        if (!optionalVa) {
            mlog[ERROR] <<"no virtual address specified and not enough space available"
                        <<" for \"" <<StringUtility::cEscape(locatorString) <<"\"\n";
        }
    }

    // Adjust the memory map
    ASSERT_require(optionalVa);
    ASSERT_require(optionalVSize);
    ASSERT_require(*optionalVSize > 0);
    AddressInterval interval = AddressInterval::baseSize(*optionalVa, *optionalVSize);
    insert(interval, Segment::anonymousInstance(data.size(), accessFlags, "data"));
    size_t nCopied = at(interval.least()).write(data).size();
    ASSERT_always_require(data.size() == nCopied);
    return interval;
}

std::string
MemoryMap::insertProcessDocumentation() {
    return ("Beginning with the first colon, a process resource string has the form "
            "\":@v{options}:@v{pid}\" where @v{options} controls how the process memory is read and @v{pid} is the process ID. "
            "The @v{options} are a comma-separated list of words where the following are recognized:"

            "@bullet{\"noattach\" means do not attempt to attach or detach from the process. This is useful when the process "
            "is already running under some debugger (it has the \"T\" state in @man{ps}{1} output).}"

            "The process will be momentarily stopped (unless the \"noattach\" option was specified, in which case it is assumed "
            "to already be stopped) while its readable memory is copied into ROSE and mapped at the same addresses and with "
            "the same permissions as in the process. Then the process is resumed (unless \"noattach\").  If a read fails when "
            "copying a memory segment from the process into ROSE then "
            "the memory map will contain only that data which was successfully read and all subsequent addresses for that "
            "segment are not mapped in ROSE.  The segments will have names like \"proc:@v{pid}@v{error}(@v{name})\" where "
            "@v{error} is an optional error message in square brackets and @v{name} is the name of the memory segment "
            "according to the kernel (not all segments have names in the kernel).  For example, a segment named "
            "\"proc:24112[input/output error](/lib/ld-2.11.3.so)\" means it came from the \"/lib/ld-2.11.3.so\" library "
            "that was loaded for process 24112 but ROSE was unable to read the entire segment due to an error.  If an error "
            "occurs when reading the very first byte of a segment then no entry will appear in the final memory map since "
            "maps never have zero-length segments.");
}

#ifndef BOOST_WINDOWS
static std::runtime_error
insertProcessError(const std::string &prefix, pid_t pid = -1, const std::string &suffix = "") {
    std::string s = prefix;
    if (pid >= 0)
        s += " process " + StringUtility::numberToString(pid);
    if (!suffix.empty())
        s += ": " + suffix;
    throw std::runtime_error("MemoryMap::insertProcess: " + s);
}
#endif

void
MemoryMap::insertProcess(const std::string &locatorString) {
    // Parse the locator string.
    Attach::Boolean doAttach = Attach::YES;
    const char *s = locatorString.c_str();
    if (':'!=*s++)
        throw insertProcessError("initial colon expected in \"" + StringUtility::cEscape(locatorString) + "\"");
    while (':'!=*s) {
        if (boost::starts_with(s, "noattach")) {
            doAttach = Attach::NO;
            s += strlen("noattach");
        } else {
            throw insertProcessError("unknown option in \"" + StringUtility::cEscape(locatorString) +
                                     " beginning at ...\"" + StringUtility::cEscape(std::string(s)) + "\"");
        }
        if (','==*s)
            ++s;
    }
    if (':'!=*s++)
        throw insertProcessError("second colon expected in \"" + StringUtility::cEscape(locatorString) + "\"");

    pid_t pid = 0;
    if (!parseInteger<pid_t>(s /*in,out*/).assignTo(pid))
        throw insertProcessError("process ID expected");
    insertProcess(pid, doAttach);
}

// class method
std::vector<MemoryMap::ProcessMapRecord>
MemoryMap::readProcessMap(pid_t pid) {
    std::vector<MemoryMap::ProcessMapRecord> records;

    //               1           2               3                    4              5         6         7
    //               first       last+1          accessibility        offset         device    inode     comment
    boost::regex re("([0-9a-f]+)-([0-9a-f]+)\\s+([-r][-w][-x][-p])\\s+([0-9a-f]+)\\s+(\\S+)\\s+(\\d+)\\s+(.*)");

    boost::filesystem::path mapsName = "/proc/" + boost::lexical_cast<std::string>(pid) + "/maps";
    std::ifstream maps(mapsName.c_str());
    while (maps) {
        std::string line = rose_getline(maps);
        boost::smatch matched;
        if (!boost::regex_match(line, matched, re))
            break;

        // virtual addresses
        ProcessMapRecord record;
        rose_addr_t beginVa = rose_strtoull(matched.str(1).c_str(), NULL, 16);
        rose_addr_t endVa = rose_strtoull(matched.str(2).c_str(), NULL, 16);
        if (endVa <= beginVa)
            break;
        record.interval = AddressInterval::hull(beginVa, endVa - 1);

        // accessibility
        if (matched.str(3)[0] == 'r')
            record.accessibility |= READABLE;
        if (matched.str(3)[1] == 'w')
            record.accessibility |= WRITABLE;
        if (matched.str(3)[2] == 'x')
            record.accessibility |= EXECUTABLE;
        if (matched.str(3)[3] == 'p')
            record.accessibility |= PRIVATE;

        // the rest of the fields
        record.fileOffset = rose_strtoull(matched.str(4).c_str(), NULL, 16);
        record.deviceName = matched.str(5);
        record.inode = boost::lexical_cast<size_t>(matched.str(6));
        record.comment = boost::trim_copy(matched.str(7));
        records.push_back(record);
    }
    return records;
}

std::pair<MemoryMap::Buffer::Ptr, std::string>
MemoryMap::copyFromFile(int fd, const AddressInterval &where) {
    if (where.isEmpty()) {
        return {AllocatingBuffer::instance(0), ""};

    } else {
        auto buffer = AllocatingBuffer::instance(where.size());
        uint8_t *p = (uint8_t*)buffer->data();
        size_t nRemaining = where.size();
        if (-1 == lseek(fd, where.least(), SEEK_SET)) {
            buffer->resize(0);
            return {buffer, std::string("seek failed: ") + strerror(errno)};
        }

        while (nRemaining > 0) {
            ssize_t nRead = ::read(fd, p, nRemaining);
            if (-1 == nRead) {
                if (EINTR != errno) {
                    buffer->resize(where.size() - nRemaining);
                    return {buffer, std::string("read failed: ") + strerror(errno)};
                }
            } else if (0 == nRead) {
                buffer->resize(where.size() - nRemaining);
                return {buffer, "short read after " + StringUtility::plural(buffer->size(), "bytes")};
            } else {
                ASSERT_require((size_t)nRead <= nRemaining);
                nRemaining -= (size_t)nRead;
                p += (size_t)nRead;
            }
        }
        return {buffer, ""};
    }
}

bool
MemoryMap::insertProcessMemory(int memFile, const AddressInterval &where, unsigned accessibility, std::string name) {
    // Mapping from /proc/*/mem to here doesn't work, so we read the data instead
    std::pair<Buffer::Ptr, std::string> pair = copyFromFile(memFile, where);
    Buffer::Ptr buffer = pair.first;
    if (!pair.second.empty()) {
        mlog[WARN] <<"segment \"" <<name <<"\" at " <<StringUtility::addrToString(where)
                       <<": " <<pair.second <<"\n";
        name += "[" + boost::to_lower_copy(pair.second) + "]";
    }
    ASSERT_not_null(buffer);

    // If a read failed, map only what we could read
    if (buffer->size() > 0) {
        AddressInterval toMap = AddressInterval::baseSize(where.least(), buffer->size());
        insert(toMap, Segment(buffer, 0, accessibility, name));
        return true;                                    // inserted
    } else {
        return false;                                   // not inserted
    }
}

bool
MemoryMap::insertProcessPid(pid_t pid, const AddressInterval &where, unsigned accessibility, const std::string &name) {
    std::string memName = "/proc/" + boost::lexical_cast<std::string>(pid) + "/mem";
    int memFile = ::open(memName.c_str(), O_RDONLY);
    if (-1 == memFile)
        throw insertProcessError("cannot open " + memName + " for" + strerror(errno));
    BOOST_SCOPE_EXIT(memFile) { ::close(memFile); } BOOST_SCOPE_EXIT_END;
    return insertProcessMemory(memFile, where, accessibility, name);
}

void
MemoryMap::insertProcessPid(pid_t pid, const std::vector<ProcessMapRecord> &records) {
    std::string memName = "/proc/" + boost::lexical_cast<std::string>(pid) + "/mem";
    int memFile = ::open(memName.c_str(), O_RDONLY);
    if (-1 == memFile)
        throw insertProcessError("cannot open " + memName + " for" + strerror(errno));
    BOOST_SCOPE_EXIT(memFile) { ::close(memFile); } BOOST_SCOPE_EXIT_END;
    std::string namePrefix = "proc:" + boost::lexical_cast<std::string>(pid);

    for (const ProcessMapRecord &record: records) {
        std::string segmentName = namePrefix;
        if (!record.comment.empty())
            segmentName += "(" + record.comment + ")";
        insertProcessMemory(memFile, record.interval, record.accessibility, segmentName);
    }
}

void
MemoryMap::insertProcess(pid_t pid, Attach::Boolean doAttach) {
#ifdef __linux__
    pid_t resumeProcess = -1;
    BOOST_SCOPE_EXIT(&resumeProcess) {
        if (-1 != resumeProcess)
            ptrace(PTRACE_DETACH, resumeProcess, 0, 0);
    } BOOST_SCOPE_EXIT_END;

    // We need to attach to the process with ptrace before we're allowed to read from its /proc/xxx/mem file. We should also
    // stop it while we read its state.
    if (doAttach) {
        if (-1 == ptrace(PTRACE_ATTACH, pid, 0, 0))
            throw insertProcessError("cannot attach to", pid, strerror(errno));
        int wstat = 0;
        if (-1 == waitpid(pid, &wstat, 0))
            throw insertProcessError("cannot wait for", pid, strerror(errno));
        if (WIFEXITED(wstat))
            throw insertProcessError("cannot read from", pid, "early exit");
        if (WIFSIGNALED(wstat))
            throw insertProcessError("cannot read from", pid, "died with " +
                                     boost::to_lower_copy(std::string(strsignal(WTERMSIG(wstat)))));
        resumeProcess = pid;
        ASSERT_require2(WIFSTOPPED(wstat) && WSTOPSIG(wstat)==SIGSTOP, "subordinate process did not stop");
    }

    // Read memory
    std::vector<ProcessMapRecord> mapRecords = readProcessMap(pid);
    insertProcessPid(pid, mapRecords);

#else
    mlog[WARN] <<"unused parameter 'pid'" << pid << "\n";
    mlog[WARN] <<"unused parameter 'doAttach'" << doAttach << "\n";
    throw std::runtime_error("MemoryMap::insertProcess is not available on this system");
#endif
}

AddressIntervalSet
MemoryMap::linkTo(const MemoryMap::Ptr &other, const AddressIntervalSet &where, Clobber clobber) {
    ASSERT_not_null(other);
    AddressIntervalSet retval;

    // Decide what to copy. If not clobbering, then don't copy parts that already exist in the destination map.
    const AddressIntervalSet modifiable = [this, clobber](AddressIntervalSet parts /*intentionally copied*/) {
        if (Clobber::NO == clobber && !parts.isEmpty()) {
            for (auto node: findAll(AddressInterval::hull(parts.least(), parts.greatest())))
                parts -= node.key();
        }
        return parts;
    }(where);

    // Copy parts that we decide can be modified in the destination.
    for (const AddressInterval &part: modifiable.intervals()) {
        for (auto node: other->findAll(part)) {
            const AddressInterval srcAddrs = node.key();
            const Segment &srcSegment = node.value();
            const AddressInterval dstAddrs = part & srcAddrs;
            ASSERT_forbid(dstAddrs.isEmpty());

            // Which part of the src's buffer do we want to link to?
            const rose_addr_t bufferOffset = srcAddrs.least() <= part.least() ?
                                             part.least() - srcAddrs.least() + srcSegment.offset() :
                                             srcSegment.offset();
            ASSERT_require(bufferOffset < srcSegment.buffer()->size());
            ASSERT_require(bufferOffset + dstAddrs.size() <= srcSegment.buffer()->size());

            // Create a new segment for the destination map, which points into the same buffer as the source map.
            Segment dstSegment(srcSegment.buffer(), bufferOffset, srcSegment.accessibility(), srcSegment.name());
            insert(dstAddrs, dstSegment);
            retval |= dstAddrs;
        }
    }

    return retval;
}

AddressIntervalSet
MemoryMap::linkTo(const MemoryMap::Ptr &other, const AddressInterval &where, Clobber clobber) {
    AddressIntervalSet set;
    set |= where;
    return linkTo(other, set, clobber);
}

Sawyer::Optional<uint8_t>
MemoryMap::readByte(rose_addr_t va) const {
    uint8_t byte = 0;
    if (at(va).limit(1).read(&byte)) {
        return byte;
    } else {
        return {};
    }
}

SgUnsignedCharList
MemoryMap::readVector(rose_addr_t va, size_t desired, unsigned requiredPerms) const
{
    size_t canRead = at(va).limit(desired).require(requiredPerms).read(NULL).size();
    SgUnsignedCharList retval(canRead);
    size_t nRead = at(va).require(requiredPerms).read(retval).size();
    ASSERT_always_require(canRead == nRead);
    return retval;
}

std::string
MemoryMap::readString(rose_addr_t va, size_t desired, int(*validChar)(int), int(*invalidChar)(int),
                      unsigned requiredPerms, unsigned prohibitedPerms, char terminator) const
{
    std::vector<uint8_t> buf(desired, 0);
    size_t nread = at(va).require(requiredPerms).prohibit(prohibitedPerms).read(buf).size();
    for (size_t i=0; i<nread; ++i) {
        if (buf[i] == terminator)
            return std::string(buf.begin(), buf.begin()+i);

        if ((invalidChar && invalidChar(buf[i])) || (validChar && !validChar(buf[i])))
            return "";
    }
    return "";
}

void
MemoryMap::eraseZeros(size_t minsize)
{
    if (isEmpty())
        return;
    unsigned permissions = READABLE | EXECUTABLE;       // access permissions that must be present
    AddressIntervalSet toRemove;                        // to save up intervals until we're done iterating
    AddressInterval zeroInterval;
    uint8_t buf[8192];
    rose_addr_t va = hull().least();
    while (AddressInterval accessed = atOrAfter(va).require(permissions).limit(sizeof buf).read(buf)) {
        for (size_t offset=0; offset<accessed.size(); ++offset) {
            if (0 == buf[offset]) {
                if (zeroInterval.isEmpty()) {
                    zeroInterval = AddressInterval(accessed.least()+offset);
                } else if (zeroInterval.greatest()+1 < offset) {
                    if (zeroInterval.size() >= minsize)
                        toRemove.insert(zeroInterval);
                    zeroInterval = AddressInterval(accessed.least()+offset);
                } else {
                    zeroInterval = AddressInterval::hull(zeroInterval.least(), zeroInterval.greatest()+1);
                }
            } else if (!zeroInterval.isEmpty()) {
                if (zeroInterval.size() >= minsize)
                    toRemove.insert(zeroInterval);
                zeroInterval = AddressInterval();
            }
        }
        if (accessed.greatest() == hull().greatest())
            break;                                      // prevent overflow in next statement
        va += accessed.size();
    }
    if (zeroInterval.size() >= minsize)
        toRemove.insert(zeroInterval);
    for (const AddressInterval &interval: toRemove.intervals())
        erase(interval);
}

Sawyer::Optional<rose_addr_t>
MemoryMap::findAny(const Extent &limits, const std::vector<uint8_t> &bytesToFind,
                   unsigned requiredPerms, unsigned prohibitedPerms) const {
    if (limits.empty() || bytesToFind.empty())
        return Sawyer::Nothing();
    AddressInterval interval = AddressInterval::hull(limits.first(), limits.last());
    return findAny(interval, bytesToFind, requiredPerms, prohibitedPerms);
}

Sawyer::Optional<rose_addr_t>
MemoryMap::findAny(const AddressInterval &limits, const std::vector<uint8_t> &bytesToFind,
                   unsigned requiredPerms, unsigned prohibitedPerms) const {
    if (limits.isEmpty())
        return Sawyer::Nothing();

    // Start small, then increase to bufMaxSize.
    std::vector<uint8_t> buf(10 * bytesToFind.size());
    size_t bufMaxSize = std::max(buf.size(), (size_t)65536);

    // Search...
    rose_addr_t va = limits.least();
    AddressInterval prevBuffer;                         // location of buffer previous time through loop
    while (atOrAfter(va).require(requiredPerms).prohibit(prohibitedPerms).next().assignTo(va)) {
        // Since the thing for which we're searching could overlap between two buffers (the previous loop iteration and this
        // iteration), we might need to preserve some of the previous buffer contents.
        size_t bufOffset = 0;                        // where in the buffer to put the memory about to be read
        if (!prevBuffer.isEmpty() && prevBuffer.greatest() + 1 == va) {
            bufOffset = bytesToFind.size() - 1;      // number of bytes that need to be preserved from previous iteration
            memmove(buf.data(), buf.data() + (prevBuffer.size() - bufOffset), bufOffset); // move to front of buffer
        }

        // Read memory into the buffer and describe what the buffer contains.
        size_t maxRead = buf.size() - bufOffset;
        size_t nRead = at(va).require(requiredPerms).prohibit(prohibitedPerms).limit(maxRead).read(buf.data() + bufOffset).size();
        ASSERT_require(nRead > 0);
        AddressInterval curBuffer = AddressInterval::baseSize(va - bufOffset, bufOffset + nRead);

        // Search
        for (size_t i = 0; i + bytesToFind.size() < curBuffer.size(); ++i) {
            bool found = true;
            for (size_t j = 0; j < bytesToFind.size(); ++j) {
                if (buf[i+j] != bytesToFind[j]) {
                    found = false;
                    break;
                }
            }
            if (found)
                return curBuffer.least() + i;
        }

        // Avoid overflow
        if (curBuffer.greatest() == hull().greatest())
            break;
        va = curBuffer.greatest() + 1;

        // Next time through the loop, maybe read even more.
        buf.resize(std::min(bufMaxSize, 2*buf.size()));
        prevBuffer = curBuffer;
    }
    return Sawyer::Nothing();
}

Sawyer::Optional<rose_addr_t>
MemoryMap::findSequence(const AddressInterval &interval, const std::vector<uint8_t> &sequence) const {
    if (interval.isEmpty())
        return Sawyer::Nothing();
    if (sequence.empty())
        return interval.least();
    std::vector<uint8_t> buffer(4096);                  // size is arbitrary
    ASSERT_require2(sequence.size() <= buffer.size(), "long sequences not implemented yet");
    rose_addr_t searchVa = interval.least();
    while (AddressInterval window = atOrAfter(searchVa).read(buffer)) {
        for (size_t offset=0; offset+sequence.size()<=window.size(); ++offset) {
            if (std::equal(sequence.begin(), sequence.end(), &buffer[offset]))
                return window.least() + offset;
        }
        if (window.size()==buffer.size()) {
            searchVa = window.greatest() - buffer.size() + 2; // search for sequence that overlaps window boundary
        } else if (window.greatest() == hull().greatest()) {
            break;                                      // avoid possible overflow
        } else {
            searchVa = window.greatest() + 1;
        }
    }
    return Sawyer::Nothing();
}

bool
MemoryMap::shrinkUnshare() {
    bool success = true;
    for (MemoryMap::Node &node: nodes()) {
        const AddressInterval &interval = node.key();
        MemoryMap::Segment &segment = node.value();
        if (const uint8_t *data = segment.buffer()->data()) {
            // Create a new buffer for this segment, copying the old data
            Buffer::Ptr buf = AllocatingBuffer::instance(interval.size());
            if (buf->write(data + segment.offset(), 0, interval.size()) != interval.size()) {
                success = false;
            } else {
                segment.offset(0);
                segment.buffer(buf);
            }
        } else {
            success = false;
        }
    }
    return success;
}

bool
MemoryMap::combineAdjacentSegments() {
    bool changed = false;

    // Figure out what segments can be combined to form the parts of the new map
    std::vector<std::pair<AddressInterval, Segment*>> parts;
    for (NodeIterator node = nodes().begin(); node != nodes().end(); ++node) {
        if (!parts.empty() &&
            node->key().least() == parts.back().first.greatest() + 1 &&
            node->value().accessibility() == parts.back().second->accessibility() &&
            node->value().name() == parts.back().second->name()) {
            // Combine this node with the previous
            parts.back().first = parts.back().first.hull(node->key());
            changed = true;
        } else {
            parts.push_back(std::make_pair(node->key(), &node->value()));
        }
    }
    if (!changed)
        return false;

    // Create a new map by combining things
    std::array<uint8_t, 8192> buffer;
    MemoryMap newmap;
    for (const auto &part: parts) {
        newmap.insert(part.first,
                      MemoryMap::Segment::anonymousInstance(part.first.size(), part.second->accessibility(), part.second->name()));

        AddressInterval remaining = part.first;
        while (remaining) {
            const size_t n = std::min(remaining.size(), buffer.size());
            const AddressInterval read = this->at(remaining.least()).limit(n).read(buffer.data());
            ASSERT_always_require(read.size() == n);
            const AddressInterval write = newmap.at(remaining.least()).limit(n).write(buffer.data());
            ASSERT_always_require(write.size() == n);
            if (read.greatest() == remaining.greatest()) {
                remaining = AddressInterval();
            } else {
                remaining = AddressInterval::hull(read.greatest() + 1, remaining.greatest());
            }
        }
    }

    std::swap(*this, newmap);
    return true;
}

static AddressInterval
alignInterval(const AddressInterval &src, rose_addr_t loAlignment, rose_addr_t hiAlignment) {
    if (src.greatest() >= AddressInterval::whole().greatest()) {
        return AddressInterval::hull(Rose::BinaryAnalysis::alignDown(src.least(), loAlignment), src.greatest());
    } else {
        return AddressInterval::hull(Rose::BinaryAnalysis::alignDown(src.least(), loAlignment),
                                     Rose::BinaryAnalysis::alignUp(src.greatest()+1, hiAlignment) - 1);
    }
}

MemoryMap::Ptr
MemoryMap::align(rose_addr_t loAlignment, rose_addr_t hiAlignment) const {
    loAlignment = std::max(loAlignment, (rose_addr_t)1);
    hiAlignment = std::max(hiAlignment, (rose_addr_t)1);
    auto retval = MemoryMap::instance();
    retval->byteOrder(byteOrder());

    auto srcNode = nodes().begin();
    while (srcNode != nodes().end()) {

        // Take the current source segment. Also take all subsequent source segments that when aligned overlap with what we have so
        // far.
        AddressInterval dstAligned = alignInterval(srcNode->key(), loAlignment, hiAlignment);
        const std::string name = srcNode->value().name();
        unsigned perms = srcNode->value().accessibility();
        for (++srcNode; srcNode != nodes().end(); ++srcNode) {
            const AddressInterval srcAligned = alignInterval(srcNode->key(), loAlignment, hiAlignment);
            if (dstAligned.intersection(srcAligned)) {
                perms |= srcNode->value().accessibility();
                dstAligned = AddressInterval::hull(dstAligned.least(), srcAligned.greatest());
            } else {
                break;
            }
        }

        retval->insert(dstAligned, Segment::anonymousInstance(dstAligned.size(), perms, name));
    }

    // Now that the destination map has been created, copy data from source to destination.
    for (const auto &interval: intervals()) {
        std::vector<uint8_t> buf(interval.size(), 0);
        const AddressInterval readAt = at(interval).read(buf);
        ASSERT_always_require(readAt == interval);
        const AddressInterval writeAt = retval->at(interval).write(buf);
        ASSERT_always_require(writeAt == interval);
    }

    return retval;
}

Combinatorics::Hasher&
MemoryMap::hash(Combinatorics::Hasher &hasher) const {
    uint8_t buffer[4096];                               // arbitrary size
    rose_addr_t va = 0;
    while (AddressInterval where = this->atOrAfter(va).limit(sizeof buffer).read(buffer)) {
        hasher.append(buffer, where.size());
        if (where.greatest() == hull().greatest())
            break;                                      // prevent overflow in next statement
        va = where.greatest() + 1;
    }
    return hasher;
}

void
MemoryMap::dump(FILE *f, const char *prefix) const
{
    std::ostringstream ss;
    dump(ss, prefix);
    fputs(ss.str().c_str(), f);
}

void
MemoryMap::dump(std::ostream &out, std::string prefix) const
{
    if (isEmpty()) {
        out <<prefix <<"empty\n";
        return;
    }

    for (const Node &node: nodes()) {
        const AddressInterval &range = node.key();
        const Segment &segment = node.value();
        out <<prefix
            <<"va " <<StringUtility::addrToString(range.least())
            <<" + " <<StringUtility::addrToString(range.size())
            <<" = " <<StringUtility::addrToString(range.greatest()+1) <<" "
            <<segmentTitle(segment)
            <<"\n";
    }
}

void
MemoryMap::dump() const {
    dump(std::cout, "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Locator string parsing functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Result<MemoryMap::AddrSizePerm, std::pair<std::string /*message*/, size_t /*index*/>>
MemoryMap::parseAddrSizePerm(const std::string &input) {
    std::regex addrRe("^(0x[0-9a-fA-F]+|0b[01]+|[0-9]+)");
    std::regex sizeRe("^\\+(0x[0-9a-fA-F]+|0b[01]+|[0-9]+)");
    std::regex permRe("^=([rwx]*)");

    AddrSizePerm retval;
    std::string remaining = input;
    std::smatch found;
    size_t at = 0;

    // Address
    if (std::regex_search(remaining, found, addrRe)) {
        const std::string &number = found.str(1);
        if (const auto parsed = Sawyer::parse<Address>(number)) {
            retval.addr = *parsed;
        } else {
            return Sawyer::makeError(std::make_pair("address " + parsed.unwrapError(), 0));
        }
        at = found.str(0).size();
        remaining = input.substr(at);
    }

    // Size
    retval.sizeAt = at;
    if (std::regex_search(remaining, found, sizeRe)) {
        const std::string &number = found.str(1);
        if (const auto parsed = Sawyer::parse<uint64_t>(number)) {
            retval.size = *parsed;
        } else {
            return Sawyer::makeError(std::make_pair("size " + parsed.unwrapError(), at + 1));
        }
        at += found.str(0).size();
        remaining = input.substr(at);
    }

    // Permissions
    retval.permAt = at;
    if (std::regex_search(remaining, found, permRe)) {
        retval.perm = 0;
        for (const char perm: found.str(1)) {
            switch (perm) {
                case 'r':
                    retval.perm = *retval.perm | READABLE;
                    break;
                case 'w':
                    retval.perm = *retval.perm | WRITABLE;
                    break;
                case 'x':
                    retval.perm = *retval.perm | EXECUTABLE;
                    break;
                default:
                    ASSERT_not_reachable("invalid permission character");
            }
        }
        at += found.str(0).size();
        if (!input.substr(at).empty())
            return Sawyer::makeError(std::make_pair("illegal permission character", at));
    }

    if (!input.substr(at).empty())
        return Sawyer::makeError(std::make_pair("syntax error", at));

    return Sawyer::makeOk(retval);
}

std::tuple<std::vector<std::tuple<std::string /*name*/, std::string /*value*/, size_t /*start_idx*/>>,
           std::string /*rest*/,
           size_t /*rest_idx*/>
MemoryMap::parseNameValuePairs(const std::string &input) {
    std::tuple<std::vector<std::tuple<std::string, std::string, size_t>>, std::string, size_t> retval;

    std::regex nameValueRe("^(,?)"                                                // comma separator (only after first item)
                           "([a-zA-Z][a-zA-Z0-9]*(?:[-_][a-zA-Z][a-zA-Z0-9]*)*)=" // name
                           "(\"(?:\\[\\\"'?abfnrtv]|\\[0-7]{1,3}|[^\"])*\""       // value is double-quoted
                           "|'(?:\\[\\\"'?abfnrtv]|\\[0-7]{1,3}|[^'])*'"          // value is single-quoted
                           "|[^:,\\\"']*)");                                      // value is unquoted

    size_t at = 0;
    std::string remaining = input;
    std::smatch found;
    while (std::regex_search(remaining, found, nameValueRe)) {
        const bool isFirstPair = std::get<0>(retval).empty();
        const bool hasLeadingComma = !found.str(1).empty();

        // Leading comma must be present iff this is not the first pair.
        if ((isFirstPair && hasLeadingComma) || (!isFirstPair && !hasLeadingComma))
            break;

        const std::string &name = found.str(2);
        const std::string value = [](const std::string &s) {
            if (boost::starts_with(s, "\"") || boost::starts_with(s, "'")) {
                ASSERT_require(s.size() >= 2);          // at least the beginning and ending quotes
                return s.substr(1, s.size()-2);
            } else {
                return s;
            }
        }(found.str(3));
        std::get<0>(retval).push_back(std::make_tuple(name, value, at + 1));
        at += found.str(0).size();
        remaining = input.substr(at);
    }

    std::get<1>(retval) = input.substr(at);
    std::get<2>(retval) = at;
    return retval;
}

} // namespace
} // namespace

#endif
