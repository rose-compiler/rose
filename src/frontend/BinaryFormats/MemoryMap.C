#include "sage3basic.h"
#include "MemoryMap.h"
#include "rose_getline.h"

#include <boost/foreach.hpp>

std::ostream& operator<<(std::ostream &o, const MemoryMap               &x) { x.print(o); return o; }

/******************************************************************************************************************************
 *                                      Exceptions
 ******************************************************************************************************************************/

std::string
MemoryMap::Exception::leader(std::string dflt) const
{
    const char *s = what();
    return s && *s ? dflt : std::string(s);
}

std::string
MemoryMap::Exception::details(bool verbose) const
{
    std::ostringstream ss;
    if (verbose) {
        ss <<"\n";
        if (map)
            map->dump(ss, "  ");
    }
    return ss.str();
}

void
MemoryMap::Exception::print(std::ostream &o, bool verbose) const
{
    o <<leader("problem") <<details(verbose);
}

void
MemoryMap::Inconsistent::print(std::ostream &o, bool verbose) const
{
    o <<leader("inconsistent mapping") <<" for " <<new_range <<" vs. " <<old_range <<details(verbose);
        
}

void
MemoryMap::NotMapped::print(std::ostream &o, bool verbose) const
{
    o <<leader("no mapping") <<" at va " <<StringUtility::addrToString(va) <<details(verbose);
}

void
MemoryMap::NoFreeSpace::print(std::ostream &o, bool verbose) const
{
    o <<leader("no free space") <<" (nbytes=" <<size <<")" <<details(verbose);
}

void
MemoryMap::SyntaxError::print(std::ostream &o, bool verbose) const
{
    o <<leader("syntax error");
    if (!filename.empty()) {
        o <<" at " <<filename <<":" <<linenum;
        if (colnum>0)
            o <<"." <<colnum;
    }
    o <<details(verbose);
}

/******************************************************************************************************************************
 *                                      Buffer methods
 ******************************************************************************************************************************/

std::string
MemoryMap::segmentTitle(const Segment &segment) {
    std::string s;
    
    s += (segment.accessibility() & READABLE)  !=0 ? "r" : "-";
    s += (segment.accessibility() & WRITABLE)  !=0 ? "w" : "-";
    s += (segment.accessibility() & EXECUTABLE)!=0 ? "x" : "-";
    s += (segment.accessibility() & PRIVATE)   !=0 ? "p" : "-";


    std::string bufname = segment.buffer()->name();
    if (bufname.find_first_of(" \t\n()")==std::string::npos)
        bufname = "buffer " + bufname;
    s += " at " + (bufname+std::string(12, ' ')).substr(0, 12);

    s += " + " + StringUtility::addrToString(segment.offset());

    if (!segment.name().empty()) {
        static const size_t limit = 55;
        std::string name = escapeString(segment.name());
        if (name.size()>limit)
            name = name.substr(0, limit-3) + "...";
        s += " " + name;
    }
    return s;
}

/******************************************************************************************************************************
 *                                      MemoryMap methods
 ******************************************************************************************************************************/

size_t
MemoryMap::insertFile(const std::string &fileName, rose_addr_t startVa, bool writable, const std::string &segmentName) {
    Segment segment = Segment::fileInstance(fileName, READABLE | (writable?WRITABLE:0), segmentName);
    AddressInterval fileInterval = AddressInterval::baseSize(startVa, segment.buffer()->size());
    insert(fileInterval, segment);
    return fileInterval.size();
}

SgUnsignedCharList
MemoryMap::readVector(rose_addr_t va, size_t desired, unsigned requiredPerms) const
{
    size_t canRead = at(va).limit(desired).require(requiredPerms).read(NULL).size();
    SgUnsignedCharList retval(canRead);
    size_t nRead = at(va).require(requiredPerms).read(retval).size();
    ASSERT_require(canRead == nRead);
    return retval;
}

std::string
MemoryMap::readString(rose_addr_t va, size_t desired, int(*validChar)(int), int(*invalidChar)(int),
                      unsigned requiredPerms, unsigned prohibitedPerms) const
{
    std::vector<uint8_t> buf(desired, 0);
    size_t nread = at(va).require(requiredPerms).prohibit(prohibitedPerms).read(buf).size();
    for (size_t i=0; i<nread; ++i) {
        if (0==buf[i] || (validChar && !validChar(buf[i])) || (invalidChar && invalidChar(buf[i])))
            nread = i;
    }
    return std::string(buf.begin(), buf.begin()+nread);
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
            }
        }
        if (accessed.greatest() == hull().greatest())
            break;                                      // prevent overflow in next statement
        va += accessed.size();
    }
    if (zeroInterval.size() >= minsize)
        toRemove.insert(zeroInterval);
    BOOST_FOREACH (const AddressInterval &interval, toRemove.intervals())
        erase(interval);
}
                
Sawyer::Optional<rose_addr_t>
MemoryMap::findAny(const Extent &limits, const std::vector<uint8_t> &bytesToFind,
                   unsigned requiredPerms, unsigned prohibitedPerms) const
{
    Sawyer::Nothing NOT_FOUND;
    if (limits.empty() || bytesToFind.empty())
        return NOT_FOUND;

    // Read a bunch of bytes at a time.  If the buffer size is large then we'll have fewer read calls before finding a match,
    // which is good if a match is unlikely.  But if a match is likely, then it's better to use a smaller buffer so we don't
    // ready more than necessary to find a match.  We'll compromise by starting with a small buffer that grows up to some
    // limit.
    size_t nremaining = limits.size();                  // bytes remaining to search (could be zero if limits is universe)
    size_t bufsize = 8;                                 // initial buffer size
    uint8_t buffer[4096];                               // full buffer

    Sawyer::Optional<rose_addr_t> atVa = this->at(limits.first()).require(requiredPerms).prohibit(prohibitedPerms).next();
    while (atVa && *atVa <= limits.last()) {
        if (nremaining > 0)                             // zero implies entire address space
            bufsize = std::min(bufsize, nremaining);
        size_t nread = at(*atVa).limit(bufsize).require(requiredPerms).prohibit(prohibitedPerms).read(buffer).size();
        assert(nread > 0);                              // because of the next() calls
        for (size_t offset=0; offset<nread; ++offset) {
            if (std::find(bytesToFind.begin(), bytesToFind.end(), buffer[offset]) != bytesToFind.end())
                return *atVa + offset;                  // found
        }
        atVa = at(*atVa+nread).require(requiredPerms).prohibit(prohibitedPerms).next();
        bufsize = std::min(2*bufsize, sizeof buffer);   // use a larger buffer next time if possible
        nremaining -= nread;                            // ok if nremaining is already zero
    }

    return NOT_FOUND;
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

    BOOST_FOREACH (const Node &node, nodes()) {
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
