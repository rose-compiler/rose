#include "sage3basic.h"
#include "MemoryMap.h"
#include "rose_getline.h"

#include <cerrno>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

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
    return mesg.empty() ? dflt : mesg;
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
        assert(nread==n);
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
#ifndef _MSC_VER
    ssize_t n = TEMP_FAILURE_RETRY(::read(fd, data, size));
#else
    ROSE_ASSERT(!"lacking Windows support");
#endif
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
MemoryMap::Segment::check(const Extent &range, rose_addr_t *first_bad_va/*=NULL=*/) const
{
    if (range.empty()) {
        if (first_bad_va)
            *first_bad_va = 0;
        return false;
    }
    if (!get_buffer() || 0==get_buffer()->size() || get_buffer_offset() >= get_buffer()->size()) {
        if (first_bad_va)
            *first_bad_va = range.first();
        return false;
    }
    if (get_buffer_offset() + range.size() > get_buffer()->size()) {
        if (first_bad_va)
            *first_bad_va = range.first() + (get_buffer()->size() - get_buffer_offset());
        return false;
    }

    return true;
}

rose_addr_t
MemoryMap::Segment::get_buffer_offset(const Extent &my_range, rose_addr_t va) const
{
    assert(my_range.contains(Extent(va)));
    return get_buffer_offset() + va - my_range.first();
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
void
MemoryMap::Segment::removing(const Extent &my_range)
{
    assert(!my_range.empty());
}

// RangeMap API
void
MemoryMap::Segment::truncate(const Extent &my_range, rose_addr_t new_end)
{
    assert(new_end>my_range.first() && new_end<=my_range.last());
}

// RangeMap API
bool
MemoryMap::Segment::merge(const Extent &my_range, const Extent &other_range, const Segment &other)
{
    assert(!my_range.empty() && !other_range.empty());

#if 1 // Relaxed version: segments are compatible if they point to the same underlying "char*" buffer
    if (get_buffer()==NULL || other.get_buffer()==NULL)
        return false;
    if (get_buffer()->get_data_ptr()==NULL ||
        get_buffer()->get_data_ptr()!=other.get_buffer()->get_data_ptr())
        return false;
#else // Strict version: compatible only if segments point to the same MemoryMap::Buffer (this is what we eventually want)
    if (get_buffer()!=other.get_buffer())
        return false;
#endif
    if (get_mapperms()!=other.get_mapperms())
        return false;

    if (other_range.abuts_lt(my_range)) {
        if (other.get_buffer_offset(other_range, other_range.last()) + 1 != get_buffer_offset(my_range, my_range.first()))
            return false;
        set_buffer_offset(other.get_buffer_offset());
    } else {
        assert(other_range.abuts_gt(my_range));
        if (get_buffer_offset(my_range, my_range.last()) + 1 != other.get_buffer_offset(other_range, other_range.first()))
            return false;
    }

    merge_names(other);
    return true; // "other" is now a duplicate and will be removed from the map
}

MemoryMap::Segment
MemoryMap::Segment::split(const Extent &range, rose_addr_t new_end)
{
    Segment right = *this;
    right.set_buffer_offset(get_buffer_offset() + new_end-range.first());
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

    switch (copy_level) {
        case COPY_SHALLOW:
            // nothing more to do
            break;
        case COPY_DEEP:
            for (Segments::iterator si=p_segments.begin(); si!=p_segments.end(); ++si) {
                si->second.clear_cow();
                si->second.set_buffer(si->second.get_buffer()->clone());
            }
            break;
        case COPY_ON_WRITE:
            for (Segments::iterator si=p_segments.begin(); si!=p_segments.end(); ++si)
                si->second.set_cow();
            break;
    }
    return *this;
}

void
MemoryMap::insert(const Extent &range, const Segment &segment, bool erase_prior)
{
    Segments::iterator inserted = p_segments.insert(range, segment, erase_prior);
    if (inserted==p_segments.end()) {
        // If the insertion failed, then we need to throw an exception.  The exception should indicate the lowest address at
        // which there was a conflict.  I.e., the first mapped address above range.first().
        Segments::iterator found = p_segments.lower_bound(range.first());
        assert(found!=p_segments.end());
        assert(range.overlaps(found->first));
        throw Inconsistent("insertion failed", this, range, segment, found->first, found->second);
    }
}

bool
MemoryMap::exists(Extent range, unsigned required_perms) const
{
    if (!p_segments.contains(range))
        return false;
    if (0==required_perms)
        return true;

    while (!range.empty()) {
        Segments::const_iterator found = p_segments.find(range.first());
        assert(found!=p_segments.end()); // because p_segments.contains(range)
        const Extent &found_range = found->first;
        const Segment &found_segment = found->second;
        assert(!range.begins_before(found_range)); // ditto
        if ((found_segment.get_mapperms() & required_perms) != required_perms)
            return false;
        if (found_range.last() >= range.last())
            return true;
        range = Extent::inin(found_range.last()+1, range.last());
    }
    return true;
}

void
MemoryMap::erase(const Extent &range)
{
    p_segments.erase(range);
}

void
MemoryMap::erase(const Segment &segment)
{
    for (Segments::iterator si=p_segments.begin(); si!=p_segments.end(); ++si) {
        if (segment==si->second) {
            erase(si->first);
            return;
        }
    }
}

std::pair<Extent, MemoryMap::Segment>
MemoryMap::at(rose_addr_t va) const
{
    Segments::const_iterator found = p_segments.find(va);
    if (found==p_segments.end())
        throw NotMapped("", this, va);
    return *found;
}

rose_addr_t
MemoryMap::find_free(rose_addr_t start_va, size_t size, rose_addr_t alignment) const
{
    ExtentMap free_map = p_segments.invert<ExtentMap>();
    ExtentMap::iterator fmi = free_map.lower_bound(start_va);
    while ((fmi=free_map.first_fit(size, fmi)) != free_map.end()) {
        Extent free_range = fmi->first;
        rose_addr_t free_va = ALIGN_UP(free_range.first(), alignment);
        rose_addr_t free_sz = free_va > free_range.last() ? 0 : free_range.last()+1-free_va;
        if (free_sz > size)
            return free_va;
        ++fmi;
    }
    throw NoFreeSpace("find_free() failed", this, size);
}

rose_addr_t
MemoryMap::find_last_free(rose_addr_t max) const
{
    ExtentMap free_map = p_segments.invert<ExtentMap>();
    ExtentMap::iterator fmi = free_map.find_prior(max);
    if (fmi==free_map.end())
        throw NoFreeSpace("find_last_free() failed", this, 1);
    return fmi->first.first();
}

void
MemoryMap::traverse(Visitor &visitor) const
{
    for (Segments::const_iterator si=p_segments.begin(); si!=p_segments.end(); ++si)
        (void) visitor(this, si->first, si->second);
}

void
MemoryMap::prune(Visitor &predicate)
{
    ExtentMap matches;
    for (Segments::iterator si=p_segments.begin(); si!=p_segments.end(); ++si) {
        if (predicate(this, si->first, si->second))
            matches.insert(si->first);
    }

    for (ExtentMap::iterator mi=matches.begin(); mi!=matches.end(); ++mi)
        p_segments.erase(mi->first);
}

void
MemoryMap::prune(unsigned required, unsigned prohibited)
{
    struct T1: public Visitor {
        unsigned required, prohibited;
        T1(unsigned required, unsigned prohibited): required(required), prohibited(prohibited) {}
        bool operator()(const MemoryMap*, const Extent &range, const Segment &segment) {
            return ((0!=required && 0==(segment.get_mapperms() & required)) ||
                    0!=(segment.get_mapperms() & prohibited));
        }
    } predicate(required, prohibited);
    prune(predicate);
}

size_t
MemoryMap::read1(void *dst_buf/*=NULL*/, rose_addr_t start_va, size_t desired, unsigned req_perms) const
{
    Segments::const_iterator found = p_segments.find(start_va);
    if (found==p_segments.end())
        return 0;

    const Extent &range = found->first;
    assert(range.contains(Extent(start_va)));

    const Segment &segment = found->second;
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

size_t
MemoryMap::write1(const void *src_buf/*=NULL*/, rose_addr_t start_va, size_t desired, unsigned req_perms)
{
    Segments::iterator found = p_segments.find(start_va);
    if (found==p_segments.end())
        return 0;

    const Extent &range = found->first;
    assert(range.contains(Extent(start_va)));

    Segment &segment = found->second;
    if ((segment.get_mapperms() & req_perms) != req_perms || !segment.check(range))
        return 0;

    if (segment.is_cow()) {
        BufferPtr old_buf = segment.get_buffer();
        BufferPtr new_buf = old_buf->clone();
        for (Segments::iterator si=p_segments.begin(); si!=p_segments.end(); ++si) {
            if (si->second.get_buffer()==old_buf) {
                si->second.set_buffer(new_buf);
                si->second.clear_cow();
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

ExtentMap
MemoryMap::va_extents() const
{
    return ExtentMap(p_segments);
}

void
MemoryMap::mprotect(Extent range, unsigned perms, bool relax)
{
    bool done = false;
    while (!range.empty() && !done) {
        Segments::iterator found = p_segments.lower_bound(range.first());

        // Skip over leading part of range that's not mapped
        if (found==p_segments.end() || found->first.right_of(range)) {
            if (!relax)
                throw NotMapped("", this, range.first());
            return;
        }
        if (found->first.begins_after(range)) {
            if (!relax)
                throw NotMapped("", this, range.first());
            range = Extent::inin(found->first.first(), range.last());
        }

        Segment &segment = found->second;
        const Extent segment_range = found->first; // don't use a reference; it might be deleted by MemoryMap::insert() below
        done = segment_range.last() >= range.last();

        if (found->second.get_mapperms()!=perms) {
            if (range.contains(segment_range)) {
                // we can just change the segment in place
                segment.set_mapperms(perms);
            } else {
                // make a hole and insert a new segment
                assert(segment_range.begins_before(range, false/*non-strict*/));
                Extent new_range = Extent::inin(range.first(), std::min(range.last(), segment_range.last()));
                Segment new_segment = segment;
                new_segment.set_mapperms(perms);
                new_segment.set_buffer_offset(segment.get_buffer_offset(segment_range, new_range.first()));
                p_segments.insert(new_range, new_segment, true/*make hole*/); // 'segment' is now invalid
            }
        }

        if (!done)
            range = Extent::inin(segment_range.last()+1, range.last());
    }
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

    for (Segments::const_iterator si=p_segments.begin(); si!=p_segments.end(); ++si) {
        const Extent &range = si->first;
        const Segment &segment = si->second;

        assert(segment.get_buffer());
        std::string basename = segment.get_buffer()->get_name();

        out <<prefix
            <<"va " <<StringUtility::addrToString(range.first())
            <<" + " <<StringUtility::addrToString(range.size())
            <<" = " <<StringUtility::addrToString(range.last()+1) <<" "
            <<segment
            <<"\n";
    }
}

void
MemoryMap::dump(const std::string &basename) const
{
    std::ofstream index((basename+".index").c_str());
    index <<*this;

    for (Segments::const_iterator si=p_segments.begin(); si!=p_segments.end(); ++si) {
        const Extent &range = si->first;
        const Segment &segment = si->second;

        std::string dataname = basename + "-" + StringUtility::addrToString(range.first()).substr(2) + ".data";
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

        Extent range(segment_va, segment_sz);
        Segment segment(buffer, offset, perm, comment);
        insert(range, segment);
    }

    fclose(f);
    if (line) free(line);
    return nread<=0;
}
    
