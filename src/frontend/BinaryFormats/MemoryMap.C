#include "sage3basic.h"
#include "MemoryMap.h"
#include "rose_getline.h"

#include <boost/foreach.hpp>
#include <cerrno>
#include <fstream>

#include <fcntl.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/mman.h>
#else
#include <io.h>
  inline void * mmap(void*, size_t length, int, int, int, off_t)
  {
    return new char[length];
  }
  inline void munmap(void* p_data, size_t)
  {
    delete [] (char *)p_data;
  }
#define MAP_FAILED 0
#undef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) expression
#define PROT_WRITE 0x02
#define F_OK 0
#define MAP_SHARED 0
#define MAP_PRIVATE 1
#endif

#ifndef PROT_READ
#define PROT_READ MM_PROT_READ
#endif
#ifndef PROT_WRITE
#define PROT_WRITE MM_PROT_WRITE
#endif


std::ostream& operator<<(std::ostream &o, const MemoryMap               &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const MemoryMap::Exception    &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const MemoryMap::Inconsistent &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const MemoryMap::NotMapped    &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const MemoryMap::NoFreeSpace  &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const MemoryMap::SyntaxError  &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const MemoryMap::Segment      &x) { x.print(o); return o; }

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
MemoryMap::Buffer::new_name()
{
    std::string retval;
    static size_t ncalls = 0;

    retval += 'a' + (ncalls/(26*26))%26;
    retval += 'a' + (ncalls/26)%26;
    retval += 'a' + (ncalls%26);
    ++ncalls;
    return retval;
}

void
MemoryMap::Buffer::save(const std::string &filename) const
{
    std::ofstream file(filename.c_str(), std::ofstream::binary);

    rose_addr_t offset=0;
    char buf[8192];

    while (true) {
        size_t n = read(buf, offset, sizeof buf);
        file.write(buf, n);
        if (file.bad() || n<sizeof buf)
            break;
    }
}

bool
MemoryMap::Buffer::is_zero() const
{
    uint8_t buf[8192];
    size_t at = 0;
    while (at<size()) {
        size_t n = std::min(size()-at, sizeof buf);
        size_t nread = read(buf, at, n);
        if (nread!=n)
            return false;
        for (size_t i=0; i<nread; ++i) {
            if (buf[i]!=0)
                return false;
        }
        at += nread;
    }
    return true;
}

MemoryMap::BufferPtr
MemoryMap::NullBuffer::create(size_t size)
{
    return BufferPtr(new NullBuffer(size));
}

MemoryMap::BufferPtr
MemoryMap::ExternBuffer::create(void *data, size_t size)
{
    return BufferPtr(new ExternBuffer((uint8_t*)data, size));
}

MemoryMap::BufferPtr
MemoryMap::ExternBuffer::create(const void *data, size_t size)
{
    return BufferPtr(new ExternBuffer((const uint8_t*)data, size));
}

MemoryMap::BufferPtr
MemoryMap::ExternBuffer::clone() const
{
    if (!p_data)
        return ByteBuffer::create(NULL, size());
    uint8_t *d = new uint8_t[size()];
    memcpy(d, p_data, size());
    return ByteBuffer::create(d, size());
}

void
MemoryMap::ExternBuffer::resize(size_t n)
{
    assert(!"not implemented");
    abort();
}

size_t
MemoryMap::ExternBuffer::read(void *buf, size_t offset, size_t nbytes) const
{
    if (!p_data || offset>=size())
        return 0;
    size_t n = std::min(nbytes, size()-offset);
    if (buf)
        memcpy(buf, (uint8_t*)p_data+offset, n);
    return n;
}

size_t
MemoryMap::ExternBuffer::write(const void *buf, size_t offset, size_t nbytes)
{
    if (!p_data || is_read_only() || offset>=size())
        return 0;
    size_t n = std::min(nbytes, size()-offset);
    if (buf)
        memcpy((uint8_t*)p_data+offset, buf, n);
    return n;
}

MemoryMap::BufferPtr
MemoryMap::ByteBuffer::create(size_t size)
{
    return BufferPtr(new ByteBuffer(size));
}

MemoryMap::BufferPtr
MemoryMap::ByteBuffer::create(void *data, size_t size)
{
    return BufferPtr(new ByteBuffer((uint8_t*)data, size));
}

MemoryMap::BufferPtr
MemoryMap::ByteBuffer::create_from_file(const std::string &filename, size_t offset)
{
    // Open file and seek to starting offset
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd<0)
        return BufferPtr();
    struct stat sb;
    if (fstat(fd, &sb)<0) {
        close(fd);
        return BufferPtr();
    }
    off_t position = lseek(fd, offset, SEEK_SET);
    if (-1==position || (rose_addr_t)position!=offset) {
        close(fd);
        return BufferPtr();
    }

    // Read data from file until EOF
    rose_addr_t size = offset > (size_t)sb.st_size ? 0 : sb.st_size-offset;
    uint8_t *data = new uint8_t[size];
    ssize_t n = TEMP_FAILURE_RETRY(::read(fd, data, size));
    if (-1==n || n!=sb.st_size) {
        delete[] data;
        close(fd);
        return BufferPtr();
    }
    close(fd);

    // Create the buffer
    return create(data, size);
}

MemoryMap::BufferPtr
MemoryMap::AnonymousBuffer::create(size_t size)
{
    AnonymousBuffer *buf = new AnonymousBuffer(size);
    buf->set_name("anon " + buf->get_name());
    return BufferPtr(buf);
}

MemoryMap::BufferPtr
MemoryMap::AnonymousBuffer::clone() const
{
    if (p_data)
        return ExternBuffer::clone();
    return AnonymousBuffer::create(size());
}

size_t
MemoryMap::AnonymousBuffer::read(void *buf, size_t offset, size_t nbytes) const
{
    if (offset>=size())
        return 0;
    size_t n = std::min(nbytes, size()-offset);
    if (buf) {
        if (p_data) {
            memcpy(buf, (uint8_t*)p_data+offset, n);
        } else {
            memset(buf, 0, n);
        }
    }
    return n;
}

size_t
MemoryMap::AnonymousBuffer::write(const void *buf, size_t offset, size_t nbytes)
{
    if (offset>=size())
        return 0;
    size_t n = std::min(nbytes, size()-offset);
    if (buf) {
        if (!p_data) {
            bool all_zero = true;
            for (size_t i=0; i<n && all_zero; ++i)
                all_zero = ((uint8_t*)buf)[i] == '\0';
            if (all_zero)
                return n;
            p_data = new uint8_t[size()];
            memset(p_data, 0, size());
        }
        memcpy((uint8_t*)p_data+offset, buf, n);
    }
    return n;
}

const void *
MemoryMap::AnonymousBuffer::get_data_ptr() const
{
    if (!p_data) {
        p_data = new uint8_t[size()];
        memset(p_data, 0, size());
    }
    return ExternBuffer::get_data_ptr();
}

bool
MemoryMap::AnonymousBuffer::is_zero() const
{
    return !p_data || ExternBuffer::is_zero();
}

MemoryMap::BufferPtr
MemoryMap::MmapBuffer::create(const std::string &filename, int oflags, int mprot, int mflags)
{
    int fd = open(filename.c_str(), oflags, 0666);
    if (fd<0)
        throw Exception(filename + ": " + strerror(errno), NULL);
    struct stat sb;
    int status = fstat(fd, &sb);
    if (status<0)
        throw Exception(filename + ": " + strerror(errno), NULL);
    BufferPtr buffer;
    try {
        buffer = create(sb.st_size, mprot, mflags, fd, 0);
    } catch (...) {
        close(fd);
        return BufferPtr();
    }
    close(fd);
    return buffer;
}

MemoryMap::BufferPtr
MemoryMap::MmapBuffer::create(size_t length, int prot, int flags, int fd, off_t offset)
{
    void *buf = mmap(NULL, length, prot, flags, fd, 0);
    if (MAP_FAILED==buf)
        throw Exception(strerror(errno), NULL);
    return BufferPtr(new MmapBuffer((uint8_t*)buf, length, 0==(flags & PROT_WRITE)));
}

MemoryMap::MmapBuffer::~MmapBuffer()
{
    if (p_data) {
        munmap(p_data, p_size);
        p_data = NULL;
    }
}

void
MemoryMap::MmapBuffer::resize(size_t)
{
    assert(!"cannot resize");
    abort();
}

/******************************************************************************************************************************
 *                                      Segment methods
 ******************************************************************************************************************************/

bool
MemoryMap::Segment::check(const AddressInterval &range, rose_addr_t *first_bad_va/*=NULL=*/) const
{
    if (range.isEmpty()) {
        if (first_bad_va)
            *first_bad_va = 0;
        return false;
    }
    if (!get_buffer() || 0==get_buffer()->size() || get_buffer_offset() >= get_buffer()->size()) {
        if (first_bad_va)
            *first_bad_va = range.least();
        return false;
    }
    if (get_buffer_offset() + range.size() > get_buffer()->size()) {
        if (first_bad_va)
            *first_bad_va = range.least() + (get_buffer()->size() - get_buffer_offset());
        return false;
    }

    return true;
}

rose_addr_t
MemoryMap::Segment::get_buffer_offset(const AddressInterval &my_range, rose_addr_t va) const
{
    assert(my_range.isContaining(va));
    return get_buffer_offset() + va - my_range.least();
}

void
MemoryMap::Segment::set_buffer_offset(rose_addr_t n)
{
    assert(!buffer || n<buffer->size());
    buffer_offset = n;
}

std::string
MemoryMap::Segment::get_name_pairings(NamePairings *pairings) const
{
    assert(pairings!=NULL);
    std::string retval;
    std::string::size_type i = 0;
    while (i<name.size()) {
        /* Extract the file name up to the left paren */
        while (i<name.size() && isspace(name[i])) i++;
        std::string::size_type fname_start = i;
        while (i<name.size() && !isspace(name[i]) && !strchr("()+", name[i])) i++;
        if (i>=name.size() || '('!=name[i] || fname_start==i) {
            retval += name.substr(fname_start, i-fname_start);
            break;
        }
        std::string fname = name.substr(fname_start, i-fname_start);
        i++; /*skip over the left paren*/
        int parens=0;
        
        /* Extract name(s) separated from one another by '+' */
        while (i<name.size() && ')'!=name[i]) {
            while (i<name.size() && isspace(name[i])) i++;
            std::string::size_type gname_start = i;
            while (i<name.size() && '+'!=name[i] && (parens>0 || ')'!=name[i])) {
                if ('('==name[i]) {
                    parens++;
                } else if (')'==name[i]) {
                    parens--;
                } 
                i++;
            }
            if (i>gname_start)
                (*pairings)[fname].insert(name.substr(gname_start, i-gname_start));
            if (i<name.size() && '+'==name[i]) i++;
        }

        /* Skip over right paren and optional space and '+' */
        if (i<name.size() && ')'==name[i]) i++;
        while (i<name.size() && isspace(name[i])) i++;
        if (i<name.size() && '+'==name[i]) i++;
    }
    if (i<name.size())
        retval += name.substr(i);
    return retval;
}

void
MemoryMap::Segment::set_name(const NamePairings &pairings, const std::string &s1, const std::string &s2)
{
    std::string s;
    for (NamePairings::const_iterator pi=pairings.begin(); pi!=pairings.end(); ++pi) {
        s += (s.empty()?"":"+") + pi->first + "(";
        for (std::set<std::string>::const_iterator si=pi->second.begin(); si!=pi->second.end(); ++si) {
            if ('('!=s[s.size()-1]) s += "+";
            s += *si;
        }
        s += ')';
    }
    if (!s1.empty())
        s += (s.empty()?"":"+") + s1;
    if (!s2.empty())
        s += (s.empty()?"":"+") + s2;
    set_name(s);
}

void
MemoryMap::Segment::merge_names(const Segment &other)
{
    if (get_name().empty()) {
        set_name(other.get_name());
    } else if (other.get_name().empty() || 0==get_name().compare(other.get_name())) {
        /*void*/
    } else {
        NamePairings pairings;
        std::string s1 = get_name_pairings(&pairings);
        std::string s2 = other.get_name_pairings(&pairings);
        set_name(pairings, s1, s2);
    }
}

// RangeMap API
bool
MemoryMap::SegmentMergePolicy::merge(const AddressInterval &leftInterval, Segment &leftSegment,
                                     const AddressInterval &rightInterval, Segment &rightSegment)
{
    assert(!leftInterval.isEmpty() && !rightInterval.isEmpty());

#if 1 // Relaxed version: segments are compatible if they point to the same underlying "char*" buffer
    if (leftSegment.get_buffer()==NULL || rightSegment.get_buffer()==NULL)
        return false;
    if (leftSegment.get_buffer()->get_data_ptr()==NULL ||
        leftSegment.get_buffer()->get_data_ptr()!=rightSegment.get_buffer()->get_data_ptr())
        return false;
#else // Strict version: compatible only if segments point to the same MemoryMap::Buffer (this is what we eventually want)
    if (leftSegment.get_buffer()!=rightSegment.get_buffer())
        return false;
#endif
    if (leftSegment.get_mapperms()!=rightSegment.get_mapperms())
        return false;

    assert(leftInterval.isLeftAdjacent(rightInterval));
    if (leftSegment.get_buffer_offset(leftInterval, leftInterval.greatest()) + 1 !=
        rightSegment.get_buffer_offset(rightInterval, rightInterval.least()))
        return false;

    leftSegment.merge_names(rightSegment);
    return true; // "other" is now a duplicate and will be removed from the map
}

MemoryMap::Segment
MemoryMap::SegmentMergePolicy::split(const AddressInterval &range, Segment &left, rose_addr_t new_end)
{
    Segment right = left;
    right.set_buffer_offset(left.get_buffer_offset() + new_end-range.least());
    return right;
}

// does not compare segment names
bool
MemoryMap::Segment::operator==(const Segment &other) const
{
    return (get_buffer()        == other.get_buffer() &&
            get_buffer_offset() == other.get_buffer_offset() &&
            get_mapperms()      == other.get_mapperms());
}

void
MemoryMap::Segment::print(std::ostream &o) const
{
    std::string bufname = buffer->get_name();
    if (bufname.find_first_of(" \t\n()")==std::string::npos)
        bufname = "buffer " + bufname;

    o <<(0==(get_mapperms() & MM_PROT_READ)    ? "-" : "r")
      <<(0==(get_mapperms() & MM_PROT_WRITE)   ? "-" : "w")
      <<(0==(get_mapperms() & MM_PROT_EXEC)    ? "-" : "x")
      <<(0==(get_mapperms() & MM_PROT_PRIVATE) ? "-" : "p")
      <<" at " <<(bufname+std::string(12, ' ')).substr(0, 12)
      <<" + " <<StringUtility::addrToString(get_buffer_offset());

    if (!get_name().empty()) {
        static const size_t limit = 55;
        std::string name = escapeString(get_name());
        if (name.size()>limit)
            name = name.substr(0, limit-3) + "...";
        o <<" " <<name;
    }
}

/******************************************************************************************************************************
 *                                      MemoryMap methods
 ******************************************************************************************************************************/

void
MemoryMap::clear()
{
    p_segments.clear();
}

MemoryMap&
MemoryMap::init(const MemoryMap &other, CopyLevel copy_level)
{
    p_segments = other.p_segments;
    sex = other.sex;

    switch (copy_level) {
        case COPY_SHALLOW:
            // nothing more to do
            break;
        case COPY_DEEP:
            BOOST_FOREACH (Segment &segment, p_segments.values()) {
                segment.clear_cow();
                segment.set_buffer(segment.get_buffer()->clone());
            }
            break;
        case COPY_ON_WRITE:
            BOOST_FOREACH (Segment &segment, p_segments.values())
                segment.set_cow();
            break;
    }
    return *this;
}

size_t
MemoryMap::size() const
{
    return p_segments.size();
}

AddressInterval
MemoryMap::hull() const
{
    return p_segments.hull();
}

void
MemoryMap::insert(const AddressInterval &range, const Segment &segment, bool erase_prior)
{
    if (erase_prior) {
        p_segments.erase(range);
    } else if (p_segments.isOverlapping(range)) {
        // The insertion would have failed, so throw an exception.  The exception should indicate the lowest address at
        // which there was a conflict.  I.e., the first mapped address above range.least().
        Segments::ConstNodeIterator existing = p_segments.findFirstOverlap(range);
        assert(existing!=p_segments.nodes().end());
        throw Inconsistent("insertion failed", this, range, segment, existing->key(), existing->value());
    }
    p_segments.insert(range, segment);
}

size_t
MemoryMap::insert_file(const std::string &filename, rose_addr_t va, bool writable, bool erase_prior, const std::string &sgmtname)
{
    int o_flags = writable ? O_RDWR : O_RDONLY;
    int m_prot = writable ? (PROT_READ|PROT_WRITE) : PROT_READ;
    int m_flags = writable ? MAP_SHARED : MAP_PRIVATE;
    unsigned s_prot = writable ? MM_PROT_RW : MM_PROT_READ;

    int fd = open(filename.c_str(), o_flags);
    if (-1==fd)
        throw Exception(filename + ": " + strerror(errno), NULL);
    struct stat sb;
    if (-1==fstat(fd, &sb))
        throw Exception(filename + " stat: " + strerror(errno), NULL);
    if (0==sb.st_size)
        return 0;

    AddressInterval extent = AddressInterval::baseSize(va, sb.st_size);
    BufferPtr buf = MmapBuffer::create(sb.st_size, m_prot, m_flags, fd, 0);
    insert(extent, Segment(buf, 0, s_prot, sgmtname.empty()?filename:sgmtname), erase_prior);
    return sb.st_size;
}

bool
MemoryMap::exists(AddressInterval range, unsigned required_perms) const
{
    if (!p_segments.contains(range))
        return false;
    if (0==required_perms)
        return true;

    while (!range.isEmpty()) {
        Segments::ConstNodeIterator found = p_segments.find(range.least());
        assert(found!=p_segments.nodes().end());        // because p_segments.contains(range)
        const AddressInterval &found_range = found->key();
        const Segment &found_segment = found->value();
        assert(range.least() >= found_range.least());   // ditto
        if ((found_segment.get_mapperms() & required_perms) != required_perms)
            return false;
        if (found_range.greatest() >= range.greatest())
            return true;
        range = AddressInterval::hull(found_range.greatest()+1, range.greatest());
    }
    return true;
}

void
MemoryMap::erase(const AddressInterval &range)
{
    p_segments.erase(range);
}

void
MemoryMap::erase(const Segment &segment)
{
    BOOST_FOREACH (Segments::Node &node, p_segments.nodes()) {
        if (segment==node.value()) {
            erase(node.key());
            return;
        }
    }
}

const MemoryMap::Segments::Node&
MemoryMap::at(rose_addr_t va) const
{
    Segments::ConstNodeIterator found = p_segments.find(va);
    if (found==p_segments.nodes().end())
        throw NotMapped("", this, va);
    return *found;
}

Sawyer::Optional<rose_addr_t>
MemoryMap::next(rose_addr_t start_va, unsigned req_perms) const
{
    for (Segments::ConstNodeIterator si=p_segments.lowerBound(start_va); si!=p_segments.nodes().end(); ++si) {
        const AddressInterval &extent = si->key();
        const Segment &segment = si->value();
        if (req_perms == (segment.get_mapperms() & req_perms))
            return std::max(start_va, extent.least());
    }
    return Sawyer::Nothing();
}

rose_addr_t
MemoryMap::find_free(rose_addr_t start_va, size_t size, rose_addr_t alignment) const
{
    AddressIntervalSet addresses(p_segments);
    addresses.invert(AddressInterval::hull(start_va, (rose_addr_t)(-1)));
    AddressIntervalSet::ConstNodeIterator fmi = addresses.lowerBound(start_va);
    while ((fmi=addresses.firstFit(size, fmi)) != addresses.nodes().end()) {
        AddressInterval free_range = *fmi;
        rose_addr_t free_va = ALIGN_UP(free_range.least(), alignment);
        rose_addr_t free_sz = free_va > free_range.greatest() ? 0 : free_range.greatest()+1-free_va;
        if (free_sz > size)
            return free_va;
        ++fmi;
    }
    throw NoFreeSpace("find_free() failed", this, size);
}

rose_addr_t
MemoryMap::find_last_free(rose_addr_t max_va) const
{
    AddressIntervalSet addresses(p_segments);
    addresses.invert(AddressInterval::hull(0, max_va));
    AddressIntervalSet::ConstNodeIterator fmi = addresses.findPrior(max_va);
    if (fmi==addresses.nodes().end())
        throw NoFreeSpace("find_last_free() failed", this, 1);
    return fmi->least();
}

void
MemoryMap::traverse(Visitor &visitor) const
{
    BOOST_FOREACH (const Segments::Node &node, p_segments.nodes())
        (void) visitor(this, node.key(), node.value());
}

void
MemoryMap::prune(Visitor &predicate)
{
    AddressIntervalSet matches;
    BOOST_FOREACH (Segments::Node &node, p_segments.nodes()) {
        if (predicate(this, node.key(), node.value()))
            matches.insert(node.key());
    }

    BOOST_FOREACH (const AddressInterval &range, matches.nodes())
        p_segments.erase(range);
}

void
MemoryMap::prune(unsigned required, unsigned prohibited)
{
    struct T1: public Visitor {
        unsigned required, prohibited;
        T1(unsigned required, unsigned prohibited): required(required), prohibited(prohibited) {}
        bool operator()(const MemoryMap*, const AddressInterval&, const Segment &segment) {
            return ((0!=required && 0==(segment.get_mapperms() & required)) ||
                    0!=(segment.get_mapperms() & prohibited));
        }
    } predicate(required, prohibited);
    prune(predicate);
}

size_t
MemoryMap::read1(void *dst_buf/*=NULL*/, rose_addr_t start_va, size_t desired, unsigned req_perms) const
{
    Segments::ConstNodeIterator found = p_segments.find(start_va);
    if (found==p_segments.nodes().end())
        return 0;

    const AddressInterval &range = found->key();
    assert(range.isContaining(start_va));

    desired = std::min((rose_addr_t)desired, (range.greatest()-start_va)+1);

    const Segment &segment = found->value();
    if ((segment.get_mapperms() & req_perms) != req_perms || !segment.check(range))
        return 0;

    rose_addr_t buffer_offset = segment.get_buffer_offset(range, start_va);
    return segment.get_buffer()->read(dst_buf, buffer_offset, desired);
}

size_t
MemoryMap::read(void *dst_buf/*=NULL*/, rose_addr_t start_va, size_t desired, unsigned req_perms) const
{
    size_t total_copied = 0;
    while (total_copied < desired) {
        uint8_t *ptr = dst_buf ? (uint8_t*)dst_buf + total_copied : NULL;
        size_t n = read1(ptr, start_va+total_copied, desired-total_copied, req_perms);
        if (0==n) break;
        total_copied += n;
    }
    memset((uint8_t*)dst_buf+total_copied, 0, desired-total_copied);
    return total_copied;
}

SgUnsignedCharList
MemoryMap::read(rose_addr_t va, size_t desired, unsigned req_perms) const
{
    SgUnsignedCharList retval;
    while (desired>0) {
        size_t can_read = read1(NULL, va, desired, req_perms);
        if (0==can_read)
            break;
        size_t n = retval.size();
        retval.resize(retval.size()+can_read);
        size_t did_read = read1(&retval[n], va, desired, req_perms);
        assert(did_read==can_read);

        va += did_read;
        desired -= can_read;
    }
    return retval;
}

std::string
MemoryMap::read_string(rose_addr_t va, size_t desired, int(*valid_char)(int), int(*invalid_char)(int), unsigned req_perms) const
{
    std::string retval;
    while (desired>0) {
        unsigned char buf[4096];
        size_t nread = read1(buf, va, desired, req_perms);
        if (0==nread)
            return retval;
        for (size_t i=0; i<nread; ++i) {
            if (buf[i]=='\0' || (NULL!=valid_char && !valid_char(buf[i])) || (NULL!=invalid_char && invalid_char(buf[i])))
                return retval;
            retval += buf[i];
        }
        va += nread;
        desired -= nread;
    }
    return retval;
}

size_t
MemoryMap::write1(const void *src_buf/*=NULL*/, rose_addr_t start_va, size_t desired, unsigned req_perms)
{
    Segments::NodeIterator found = p_segments.find(start_va);
    if (found==p_segments.nodes().end())
        return 0;

    const AddressInterval &range = found->key();
    assert(range.isContaining(start_va));

    Segment &segment = found->value();
    if ((segment.get_mapperms() & req_perms) != req_perms || !segment.check(range))
        return 0;

    if (segment.is_cow()) {
        BufferPtr old_buf = segment.get_buffer();
        BufferPtr new_buf = old_buf->clone();
        BOOST_FOREACH (Segment &s, p_segments.values()) {
            if (s.get_buffer()==old_buf) {
                s.set_buffer(new_buf);
                s.clear_cow();
            }
        }
        assert(segment.get_buffer()==new_buf);
        assert(!segment.is_cow());
    }

    rose_addr_t buffer_offset = segment.get_buffer_offset(range, start_va);
    return segment.get_buffer()->write(src_buf, buffer_offset, desired);
}

size_t
MemoryMap::write(const void *src_buf/*=NULL*/, rose_addr_t start_va, size_t desired, unsigned req_perms)
{
    size_t total_copied = 0;
    while (total_copied < desired) {
        uint8_t *ptr = src_buf ? (uint8_t*)src_buf + total_copied : NULL;
        size_t n = write1(ptr, start_va+total_copied, desired-total_copied, req_perms);
        if (0==n) break;
        total_copied += n;
    }
    return total_copied;
}

AddressIntervalSet
MemoryMap::va_extents() const
{
    return AddressIntervalSet(p_segments);
}

void
MemoryMap::mprotect(AddressInterval range, unsigned perms, bool relax)
{
    bool done = false;
    while (!range.isEmpty() && !done) {
        Segments::NodeIterator found = p_segments.lowerBound(range.least());

        // Skip over leading part of range that's not mapped
        if (found==p_segments.nodes().end() || found->key().isRightOf(range)) {
            if (!relax)
                throw NotMapped("", this, range.least());
            return;
        }
        if (found->key().least() > range.least()) {
            if (!relax)
                throw NotMapped("", this, range.least());
            range = AddressInterval::hull(found->key().least(), range.greatest());
        }

        Segment &segment = found->value();
        const AddressInterval segment_range = found->key(); // copy since it might be deleted by MemoryMap::insert() below
        done = segment_range.greatest() >= range.greatest();

        if (found->value().get_mapperms()!=perms) {
            if (range.isContaining(segment_range)) {
                // we can just change the segment in place
                segment.set_mapperms(perms);
            } else {
                // make a hole and insert a new segment
                assert(segment_range.least() <= range.least());
                AddressInterval new_range = AddressInterval::hull(range.least(),
                                                                  std::min(range.greatest(), segment_range.greatest()));
                Segment new_segment = segment;
                new_segment.set_mapperms(perms);
                new_segment.set_buffer_offset(segment.get_buffer_offset(segment_range, new_range.least()));
                p_segments.insert(new_range, new_segment, true/*make hole*/); // 'segment' is now invalid
            }
        }

        if (!done)
            range = AddressInterval::hull(segment_range.greatest()+1, range.greatest());
    }
}

void
MemoryMap::erase_zeros(size_t minsize)
{
    AddressIntervalSet to_remove;
    BOOST_FOREACH (Segments::Node &node, p_segments.nodes()) {
        AddressInterval extent = node.key();
        const Segment &segment = node.value();
        if (0==(segment.get_mapperms() & MM_PROT_EXEC) || extent.size() < minsize)
            continue; // not executable or too small to remove
        BufferPtr buffer = segment.get_buffer();
        if (buffer->is_zero()) {
            to_remove.insert(extent);
        } else {
            for (size_t i=0; i<extent.size(); /*void*/) {
                uint8_t page[8192];
                size_t nread = read(page, extent.least()+i, sizeof page);
                if (0==nread)
                    break;
                for (size_t j=0; j<nread; /*void*/) {
                    if (0==page[j]) {
                        size_t k = 1;
                        while (j+k<nread && 0==page[j+k]) ++k;
                        if (k>=minsize)
                            to_remove.insert(AddressInterval::baseSize(extent.least()+i+j, k));
                        j += k;
                    } else {
                        ++j;
                    }
                }
                i += nread;
            }
        }
    }
    BOOST_FOREACH (const AddressInterval &range, to_remove.nodes())
        erase(range);
}

size_t
MemoryMap::match_bytes(rose_addr_t start_va, const std::vector<uint8_t> &bytesToMatch, unsigned req_perms) const
{
    SgUnsignedCharList buffer = read(start_va, bytesToMatch.size(), req_perms);
    for (size_t i=0; i<buffer.size(); ++i) {
        if (buffer[i] != bytesToMatch[i])
            return i;
    }
    return buffer.size();
}

Sawyer::Optional<rose_addr_t>
MemoryMap::find_sequence(const Extent &limits, const std::vector<uint8_t> &bytesToMatch, unsigned req_perms) const
{
    Sawyer::Nothing NOT_FOUND;
    const size_t nBytesToMatch = bytesToMatch.size();

    if (limits.first() + nBytesToMatch < limits.first() || limits.first() + nBytesToMatch >= limits.last())
        return NOT_FOUND;
    if (0==nBytesToMatch)
        return limits.first();

    // circular buffer to hold the data being read from the memory map. Using a circular buffer allows us to not have to
    // copy-shift the buffer as we read more bytes, but it does mean that we need modulo indices.  However, by using a buffer
    // that's twice as long as the thing we're looking for, we also don't need to modulo.
    std::vector<uint8_t> buffer = read(limits.first(), nBytesToMatch);
    if (buffer.size() != nBytesToMatch)
        return NOT_FOUND;
    buffer.insert(buffer.end(), buffer.begin(), buffer.end());  // double the buffer by repeating it
    size_t bufferInsertIdx = 0;                                 // modulo nBytesToMatch

    // look for a match or advance
    rose_addr_t va = limits.first();
    while (true) {
        if (0==memcmp(&bytesToMatch[0], &buffer[bufferInsertIdx], nBytesToMatch)) {
            return va;
        } else if (va + nBytesToMatch >= limits.last()) {
            return NOT_FOUND;                                   // reached end of limit without matching
        }

        // read another byte (and place it in two places in the buffer)
        size_t nread = read(&(buffer[bufferInsertIdx]), va, 1, req_perms);
        if (nread!=1)
            return NOT_FOUND;                                   // reached an unmapped or unreadable address
        buffer[bufferInsertIdx + nBytesToMatch] = buffer[bufferInsertIdx];
        bufferInsertIdx = (bufferInsertIdx + 1) % nBytesToMatch;
        ++va;
    }
}

Sawyer::Optional<rose_addr_t>
MemoryMap::find_any(const Extent &limits, const std::vector<uint8_t> &bytesToFind, unsigned req_perms) const
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

    Sawyer::Optional<rose_addr_t> at = next(limits.first(), req_perms);
    while (at && *at <= limits.last()) {
        if (nremaining > 0)                             // zero implies entire address space
            bufsize = std::min(bufsize, nremaining);
        size_t nread = read(buffer, *at, bufsize, req_perms);
        assert(nread > 0);                              // because of the next() calls
        for (size_t offset=0; offset<nread; ++offset) {
            if (std::find(bytesToFind.begin(), bytesToFind.end(), buffer[offset]) != bytesToFind.end())
                return *at + offset;                    // found
        }
        at = next(*at + nread, req_perms);
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
    if (0==p_segments.size()) {
        out <<prefix <<"empty\n";
        return;
    }

    BOOST_FOREACH (const Segments::Node &node, p_segments.nodes()) {
        const AddressInterval &range = node.key();
        const Segment &segment = node.value();

        assert(segment.get_buffer());
        std::string basename = segment.get_buffer()->get_name();

        out <<prefix
            <<"va " <<StringUtility::addrToString(range.least())
            <<" + " <<StringUtility::addrToString(range.size())
            <<" = " <<StringUtility::addrToString(range.greatest()+1) <<" "
            <<segment
            <<"\n";
    }
}

void
MemoryMap::dump(const std::string &basename) const
{
    std::ofstream index((basename+".index").c_str());
    index <<*this;

    BOOST_FOREACH (const Segments::Node &node, p_segments.nodes()) {
        const AddressInterval &range = node.key();
        const Segment &segment = node.value();

        std::string dataname = basename + "-" + StringUtility::addrToString(range.least()).substr(2) + ".data";
        segment.get_buffer()->save(dataname);
    }
}

bool
MemoryMap::load(const std::string &basename)
{
    clear();
    std::string indexname = basename + ".index";
    FILE *f = fopen(indexname.c_str(), "r");
    if (!f) return false;

    char *line = NULL;
    size_t line_nalloc = 0;
    ssize_t nread;
    unsigned nlines=0;
    std::map<std::string, BufferPtr> buffers;

    while (0<(nread=rose_getline(&line, &line_nalloc, f))) {
        char *rest, *s=line;
        nlines++;

        /* Check for empty lines and comments */
        while (isspace(*s)) s++;
        if (!*s || '#'==*s) continue;

        /* Starting virtual address with optional "va " prefix */
        if (!strncmp(s, "va ", 3)) s += 3;
        errno = 0;
#ifndef _MSC_VER
        rose_addr_t segment_va = strtoull(s, &rest, 0);
#else
        rose_addr_t segment_va = 0;
        ROSE_ASSERT(!"lacking Windows support");
#endif
        if (rest==s || errno)
            throw SyntaxError("starting virtual address expected", this, indexname, nlines, s-line);
        s = rest;

        /* Size, prefixed with optional "+" or "," */
        while (isspace(*s)) s++;
        if ('+'==*s || ','==*s) s++;
        while (isspace(*s)) s++;
        errno = 0;
#ifndef _MSC_VER
        rose_addr_t segment_sz = strtoull(s, &rest, 0);
#else
        rose_addr_t segment_sz = 0;
        ROSE_ASSERT(!"lacking Windows support");
#endif
        if (rest==s || errno)
            throw SyntaxError("virtual size expected", this, indexname, nlines, s-line);
        s = rest;

        /* Optional ending virtual address prefixed with "=" */
        while (isspace(*s)) s++;
        if ('='==*s) {
            s++;
            errno = 0;
#ifndef _MSC_VER
            (void)strtoull(s, &rest, 0);
#else
            ROSE_ASSERT(!"lacking Windows support");
#endif
            if (rest==s || errno)
                throw SyntaxError("ending virtual address expected after '='", this, indexname, nlines, s-line);
            s = rest;
        }

        /* Permissions with optional ',' prefix. Permissions are the letters 'r', 'w', and/or 'x'. Hyphens can appear in the
         * r/w/x string at any position and are ignored. */
        while (isspace(*s)) s++;
        if (','==*s) s++;
        while (isspace(*s)) s++;
        unsigned perm = 0;
        while (strchr("rwxp-", *s)) {
            switch (*s++) {
                case 'r': perm |= MM_PROT_READ; break;
                case 'w': perm |= MM_PROT_WRITE; break;
                case 'x': perm |= MM_PROT_EXEC; break;
                case 'p': perm |= MM_PROT_PRIVATE; break;
                case '-': break;
                default: break; /*to suppress a compiler warning*/
            }
        }

        /* File or buffer name terminated by ',' or '0x' and then strip trailing space. */
        while (isspace(*s)) s++;
        if (','==*s) s++;
        while (isspace(*s)) s++;
        if (!strncmp(s, "at", 2) && isspace(s[2])) s+= 3;
        while (isspace(*s)) s++;
        int buffer_name_col = s-line;
        std::string buffer_name;
        while (*s && ','!=*s && 0!=strncmp(s, "0x", 2))
            buffer_name += *s++;
        size_t bnsz = buffer_name.size();
        while (bnsz>0 && isspace(buffer_name[bnsz-1])) --bnsz;
        buffer_name = buffer_name.substr(0, bnsz);
        if (buffer_name.empty())
            throw SyntaxError("file/buffer name expected", this, indexname, nlines, buffer_name_col);

        /* Offset from base address; optional prefix of "," or "+". */
        while (isspace(*s)) s++;
        if (','==*s || '+'==*s) s++;
        while (isspace(*s)) s++;
        errno = 0;
#ifndef _MSC_VER
        rose_addr_t offset = strtoull(s, &rest, 0);
#else
        rose_addr_t offset = 0;
        ROSE_ASSERT(!"lacking Windows support");
#endif
        if (rest==s || errno)
            throw SyntaxError("file/buffer offset expected", this, indexname, nlines, s-line);
        s = rest;

        /* Comment (optional) */
        while (isspace(*s)) s++;
        if (','==*s) s++;
        while (isspace(*s)) s++;
        char *end = s + strlen(s);
        while (end>s && isspace(end[-1])) --end;
        std::string comment(s, end-s);

        /* Create the buffer or use one we already created. */
        BufferPtr buffer;
        std::map<std::string, BufferPtr>::iterator bi = buffers.find(buffer_name);
        if (bi!=buffers.end() && 0==(perm & MM_PROT_PRIVATE)) {
            buffer = bi->second;
        } else if (0==access(buffer_name.c_str(), F_OK)) {
            buffer = ByteBuffer::create_from_file(buffer_name);
            buffers.insert(std::make_pair(buffer_name, buffer));
        } else {
            ROSE_ASSERT(!"not implemented yet");
        }

        AddressInterval range = AddressInterval::baseSize(segment_va, segment_sz);
        Segment segment(buffer, offset, perm, comment);
        insert(range, segment);
    }

    fclose(f);
    if (line) free(line);
    return nread<=0;
}
