#ifndef ROSE_BinaryAnalysis_MemoryMap_H
#define ROSE_BinaryAnalysis_MemoryMap_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <ByteOrder.h>
#include <Combinatorics.h>
#include <RoseException.h>

#include <Sawyer/Access.h>
#include <Sawyer/AddressMap.h>
#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/MappedBuffer.h>
#include <Sawyer/NullBuffer.h>
#include <Sawyer/Optional.h>
#include <Sawyer/StaticBuffer.h>

#include <boost/config.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/type_traits/is_integral.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Align address downward to boundary.
 *
 *  Returns the smallest multiple of @p alignment which is greater than or equal to @p address. The alignment is cast to the same
 *  type as the address before any calculations are performed. Both arguments must be integral types. An alignment less than
 *  one has undefined behavior. */
template<typename T, typename U>
typename boost::enable_if_c<boost::is_integral<T>::value && boost::is_integral<U>::value, T>::type
alignUp(T address, U alignment) {
    ASSERT_require(alignment > 0);
    T almt = static_cast<T>(alignment);
    return ((address + almt - 1) / almt) * almt;
}

/** Align address upward to boundary.
 *
 *  Returns the largest multiple of @p alignment which is less than or equal to @p address. The alignment is cast to the
 *  same type as the address before any calculations are performed. Both arguments must be integral types. An alignment less
 *  than one has undefined behavior. Returns zero if no such value can be returned due to overflow. */
template<typename T, typename U>
typename boost::enable_if_c<boost::is_integral<T>::value && boost::is_integral<U>::value, T>::type
alignDown(T address, U alignment) {
    ASSERT_require(alignment > 0);
    T almt = static_cast<T>(alignment);
    return (address / almt) * almt;
}

/** An efficient mapping from an address space to stored data.
 *
 *  This class maps addresses in a 64-bit virtual address space to bytes stored in buffers and is a specialization of @ref
 *  Sawyer::Container::AddressMap.  A MemoryMap stores pairs of address ranges and segments, and each segment points to a
 *  buffer. The address space is segmented into non-overlapping, contiguous regions called "segments" (@ref
 *  Sawyer::Container::AddressSegment) and the addresses in a segment are mapped 1:1 onto data storage containers (@ref
 *  Sawyer::Container::Buffer).
 *
 *  Buffers come in a variety of kinds, all derived from @ref Sawyer::Container::Buffer and they are reference counted via
 *  shared-ownership smart pointers (@ref heap_object_shared_ownership).  Always refer to a buffer with its @c Ptr type. They
 *  should be created with various @c instance class methods, and they should never be explicitly freed.
 *
 *  MemoryMap objects are reference counted and always created on the heap. They are referred to by the @ref Ptr type which can
 *  be usually treated as an ordinary C++ pointer.  Objects can be created with the static method @ref instance, or by doing a
 *  shallow copy of an existing object using the @ref shallowCopy method. Plain assignment (operator=) is similar to @ref
 *  shallowCopy in that after the assignment the two objects will have independent copies of the segment information but will
 *  share the underlying data buffers.  MemoryMap objects should not be explicitly deleted since the shared pointers will
 *  delete the object when it's no longer referenced.
 *
 *  Here's an example of mapping a file into an address space at virtual address 0x08040000 and then temporarily replacing the
 *  second 1kB page of the file with our own data.  We demonstrate using a @ref Sawyer::Container::MappedBuffer because these
 *  are very fast for large files, especially if only small parts of the file are accessed due to their use of OS-level memory
 *  mapping.
 *
 * @code
 *  using namespace Sawyer::Container;
 *
 *  // Create and initialize the overlay data
 *  myData_size = 8192;
 *  uint8_t *myData = new uint8_t[myDataSize];
 *  initialize(myData, myDataSize);
 *
 *  // Create the two buffers: one for the file, one for the overlay data
 *  Buffer::Ptr fileBuf = MappedBuffer::instance("the_file", boost::iostreams::mapped_file::readonly);
 *  Buffer::Ptr dataBuf = StaticBuffer::instance(myData, myDataSize);
 *
 *  // Create the memory map.
 *  MemoryMap::Ptr map = MemoryMap::instance();
 *  map->insert(AddressInterval::baseSize(0x08040000, fileBuf->size()),
 *              AddressSegment(fileBuf, 0, MemoryMap::MM_PROT_READ, "the file contents"));
 *  map->insert(AddressInterval::baseSize(0x08040000+1024, dataBuf->size()),
 *             AddressSegment(dataBuf, 0, MemoryMap::MM_PROT_RW, "data overlay"));
 * @endcode
 *
 *  A MemoryMap provides methods to easily read from and write to the underlying data storage, addressing it in terms of the
 *  virtual address space.  These functions return the addresses that were accessed, and are prefixed by calls that describe
 *  what data is to be accessed.
 *
 * @code
 *  // read part of the data, right across the file/overlay boundary
 *  uint8_t data[4096];
 *  size_t nRead = map->at(0x08040100).limit(sizeof data).read(data).size();
 *  assert(nread==sizeof data);
 * @endcode
 *
 *  The Sawyer documentation contains many more examples.
 */
class MemoryMap: public Sawyer::Container::AddressMap<rose_addr_t, uint8_t>, public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<MemoryMap> Ptr;
    typedef rose_addr_t Address;
    typedef uint8_t Value;
    typedef Sawyer::Container::AddressMap<Address, Value> Super;
    typedef Sawyer::Container::Buffer<Address, Value> Buffer;
    typedef Sawyer::Container::AllocatingBuffer<Address, Value> AllocatingBuffer;
    typedef Sawyer::Container::MappedBuffer<Address, Value> MappedBuffer;
    typedef Sawyer::Container::NullBuffer<Address, Value> NullBuffer;
    typedef Sawyer::Container::StaticBuffer<Address, Value> StaticBuffer;
    typedef Sawyer::Container::SegmentPredicate<Address, Value> SegmentPredicate;
    typedef Sawyer::Container::AddressMapConstraints<Sawyer::Container::AddressMap<rose_addr_t, uint8_t> > Constraints;
    typedef Sawyer::Container::AddressMapConstraints<const Sawyer::Container::AddressMap<rose_addr_t, uint8_t> > ConstConstraints;

    /** Attach with ptrace first when reading a process? */
    struct Attach {                                     // For consistency with other <Feature>::Boolean types
        enum Boolean {
            NO,                                         /**< Assume ptrace is attached and process is stopped. */
            YES                                         /**< Attach with ptrace, get memory, then detach. */
        };
    };

private:
    ByteOrder::Endianness endianness_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s.template register_type<AllocatingBuffer>();
        s.template register_type<MappedBuffer>();
        s.template register_type<NullBuffer>();
        s.template register_type<StaticBuffer>();
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(endianness_);
    }
#endif

public:

    // Whoever pollutes all namespaces with these common word preprocessor symbols is a lunatic!
#   if defined(READABLE) || defined(WRITABLE) || defined(EXECUTABLE) || defined(IMMUTABLE) || defined(PRIVATE)
#    ifdef _MSC_VER
#     pragma message("Undefining common words from the global namespace: READABLE, WRITABLE, EXECUTABLE, IMMUTABLE, PRIVATE")
#    else
#     warning "Undefining common words from the global namespace: READABLE, WRITABLE, EXECUTABLE, IMMUTABLE, PRIVATE"
#    endif
#    undef READABLE
#    undef WRITABLE
#    undef EXECUTABLE
#    undef IMMUTABLE
#    undef PRIVATE
#   endif

    // Accessibility flags
    static const unsigned NO_ACCESS = 0;
    static const unsigned READABLE      = Sawyer::Access::READABLE;
    static const unsigned WRITABLE      = Sawyer::Access::WRITABLE;
    static const unsigned EXECUTABLE    = Sawyer::Access::EXECUTABLE;
    static const unsigned IMMUTABLE     = Sawyer::Access::IMMUTABLE;
    static const unsigned PRIVATE       = Sawyer::Access::PRIVATE;
    static const unsigned INITIALIZED   = 0x00000200;   // Partitioner2: initialized memory even if writable

    // Aggregate accessibility flags
    static const unsigned READ_WRITE = READABLE | WRITABLE;
    static const unsigned READ_EXECUTE = READABLE | EXECUTABLE;
    static const unsigned READ_WRITE_EXECUTE = READABLE | WRITABLE | EXECUTABLE;

    // These bits are reserved for use in ROSE
    static const unsigned RESERVED_ACCESS_BITS = 0x0000ffff;


public:
    /** Exception for MemoryMap operations. */
    class Exception: public Rose::Exception {
    public:
        Exception(const std::string &mesg, const MemoryMap::Ptr map): Rose::Exception(mesg), map(map) {}
        virtual ~Exception() throw() {}
        virtual std::string leader(std::string dflt="memory map problem") const;   /**< Leading part of the error message. */
        virtual std::string details(bool) const; /**< Details emitted on following lines, indented two spaces. */
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const Exception&);
    public:
        MemoryMap::Ptr map;                             /**< Map that caused the exception if available, null otherwise. */
    };

    /** Exception for an inconsistent mapping. This exception occurs when an attemt is made to insert a new segment but the
     *  address range of the new segment is already defined by an existing segment.  The @p new_range and @p new_segment are
     *  information about the segment that was being inserted, and the @p old_range and @p old_segment is information about
     *  an existing segment that conflicts with the new one. */
    struct Inconsistent : public Exception {
        Inconsistent(const std::string &mesg, const MemoryMap::Ptr &map,
                     const AddressInterval &new_range, const Segment &new_segment,
                     const AddressInterval &old_range, const Segment &old_segment)
            : Exception(mesg, map),
              new_range(new_range), old_range(old_range),
              new_segment(new_segment), old_segment(old_segment) {}
        virtual ~Inconsistent() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const Inconsistent&);
        AddressInterval new_range, old_range;
        Segment new_segment, old_segment;
    };

    /** Exception for when we try to access a virtual address that isn't mapped. */
    struct NotMapped : public Exception {
        NotMapped(const std::string &mesg, const MemoryMap::Ptr &map, rose_addr_t va)
            : Exception(mesg, map), va(va) {}
        virtual ~NotMapped() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const NotMapped&);
        rose_addr_t va;
    };

    /** Exception thrown by find_free() when there's not enough free space left. */
    struct NoFreeSpace : public Exception {
        NoFreeSpace(const std::string &mesg, const MemoryMap::Ptr &map, size_t size)
            : Exception(mesg, map), size(size) {}
        virtual ~NoFreeSpace() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const NoFreeSpace&);
        size_t size;
    };

    /** Exception thrown by load() when there's a syntax error in the index file. */
    struct SyntaxError: public Exception {
        SyntaxError(const std::string &mesg, const MemoryMap::Ptr &map, const std::string &filename,
                    unsigned linenum, int colnum=-1)
            : Exception(mesg, map), filename(filename), linenum(linenum), colnum(colnum) {}
        virtual ~SyntaxError() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const SyntaxError&);
        std::string filename;                   /**< Name of index file where error occurred. */
        unsigned linenum;                       /**< Line number (1 origin) where error occurred. */
        int colnum;                             /**< Optional column number (0-origin; negative if unknown). */
    };

protected:
    /** Constructs an empty memory map. */
    MemoryMap(): endianness_(ByteOrder::ORDER_UNSPECIFIED) {}

public:
    /** Construct an empty memory map. */
    static Ptr instance() {
        return Ptr(new MemoryMap);
    }

    /** Create a new copy of the memory map.
     *
     *  The copy maintains its own independent list of segments, but points to the same data buffers as the source map. */
    Ptr shallowCopy() {
        return Ptr(new MemoryMap(*this));
    }

    /** Property: byte order.
     *
     *  Every map has a default byte order property which can be used by functions that read and write multi-byte values when
     *  the user does not provide a byte order to those functions.  The MemoryMap constructors initialize this property to @ref
     *  ByteOrder::ORDER_UNSPECIFIED.
     *
     * @{ */
    ByteOrder::Endianness byteOrder() const { return endianness_; }
    void byteOrder(ByteOrder::Endianness order) { endianness_ = order; }
     /** @} */

    // Note that the order of the enum members is for backward compatibility with an older version of insertFile whose third
    // argument was "bool writable = false" (MAP_RDONLY, but now intended to be MAP_PRIVATE) and when it was true was the same
    // as MAP_READWRITE.
    // 
    /** Mapping mode for insertFile. */
    enum InsertFileMapMode {
        MAP_PRIVATE = 0,                                /**< File is mapped privately. Writing to the memory map is allowed,
                                                         *   but the changes will not show up in the file. */
        MAP_READWRITE = 1,                              /**< File is mapped with read and write permission. Changes to the
                                                         *   memory map will also cause the file to change. */
        MAP_RDONLY = 2                                  /**< File is mapped with read-only permission. Any attempt to modify
                                                         *   the file will likely result in a segmentation fault. */
    };
    
    /** Insert file contents into memory map.
     *
     *  Insert the contents of a file into the memory map at the specified address.  This is just a convenience wrapper that
     *  creates a new MappedBuffer and inserts it into the mapping. Returns the size of the file mapping. */
    size_t insertFile(const std::string &fileName, rose_addr_t va, InsertFileMapMode mode = MAP_PRIVATE,
                      std::string segmentName = "");

    /** Insert file contents into memory map.
     *
     *  Uses a locator string to load a file into a memory map.
     *
     *  Returns the address interval that was inserted into this memory map, or throws an <code>std::runtime_error</code> for
     *  syntax errors and problems reading the file.
     *
     * @section syntax Locator Syntax
     *
     *  The locator string is a file name preceded by various other parameters to control where the file is loaded in memory.
     *  It takes the form:
     *
     * @verbatim
     *  :[ADDR][+VMSIZE][=PERM]:[OFFSET[+FSIZE]]:FILENAME
     * @endverbatim
     *
     *  The fields between the first and second colon are parameters for virtual memory; the fields between the second and
     *  third colon are parameters for the file.  Their meanings are:
     *
     * @li @c ADDR: The virtual address where the first byte of the file is mapped.  This can be specified in decimal,
     *     octal, or hexadecimal using the usual C syntax.  If no address is specified then the file is mapped at the lowest
     *     unmapped region which is large enough to hold the file.
     *
     * @li @c VMSIZE: Size of virtual memory to map.  If VMSIZE is not specified then it is either the FSIZE (if specified) or
     *     the file size.  On POSIX systems the file size is that which is reported by @p stat, and on other systems it is
     *     the number of bytes that can be read from the file. The size can be specified as decimal, octal, or hexadecimal with
     *     the usual C syntax.  If VMSIZE is greater than the specified or calculated FSIZE then the data from the file is
     *     padded with zero bytes.
     *
     * @li @c PERM: Accessibility for the mapped segment.  If present, it should be any of the letters "r", "w", and/or "x" in
     *     that order to indicate readable, writable, and/or executable.  If not present, the accessibility of the segment is
     *     the same as the user's accessibility of the file (on POSIX systems; "rwx" on Windows systems).
     *
     * @li @c OFFSET: Byte offset within file for first byte to map. If no offset is specified then zero is assumed. The size
     *     can be decimal, octal, or hexadecimal in the usual C sytax.
     *
     * @li @c FSIZE: Number of file bytes to map.  If not specified the entire readable content of the file is mapped beginning
     *     at the specified OFFSET but not exceeding a specified VMSIZE.  If this number of bytes cannot be read from the file
     *     then an error is thrown.
     *
     * @li @c FILENAME: Name of file to read. The file must be readable by the user and its contents are copied into the memory
     *     map.  Once inside the memory map, the segment can be given any accessibility according to PERM.  The name of the
     *     segment will be the non-directory part of the FILENAME (e.g., on POSIX systems, the part after the final slash).
     *
     * @section exampes Examples
     *
     *  To load a couple raw files at the lowest available addresses:
     *
     * @code
     *  :::myfile1.bin :::myfile2.bin
     * @endcode
     *
     *  To load a 4k page of zeros with read and execute permission at address 0x7ffff000 (won't work on Microsoft systems):
     *
     * @code
     *  :0x7ffff000+0x1000=rx::/dev/zero
     * @code
     *
     *  On Microsoft Windows one could create a large file containing zeros:
     *
     * @code
     *  :0x7ffff000+0x1000=rx::myzeros.dat
     * @endcode
     *
     *  To load the .text and .data segments from a PE file when we know where they occur in the PE file but the PE file is
     *  damaged to the point where it cannot be loaded by ROSE's @c frontend. Both sections are zero-padded since the
     *  memory segment size is larger than the file size.  But if one wants to more closely emulate the Windows loader, leave
     *  off the file sizes:
     *
     * @code
     *  :0x01000400+0xa00=rx:0x1000+0x9a8:proxycfg.exe
     *  :0x01000e00+0x200=rw:0x3000+8:proxycfg.exe
     * @endcode
     *
     * @sa insertFileDocumentation */
    AddressInterval insertFile(const std::string &locatorString);

    /** Documentation string for @ref insertFile. */
    static std::string insertFileDocumentation();

    /** Insert data into a memory map.
     *
     *  This is intended for insert small pieces of data parsed from the locator string.  The locator string has the form:
     *
     * @verbatim
     *  :[ADDR][+VMSIZE][=PERM]::DATA
     * @endverbatim
     *
     *  The fields between the first and second colon are parameters for virtual memory; the fields between the second and
     *  third colon are parameters for the data (none currently defined). Their meanings are:
     *
     *  @li @c ADDR: The virtual address where the first byte of data is mapped. This can be specified in decimal, octal, or
     *      hexadecimal using the usual C syntax. If no address is specified then the data is mapped at the lowest unmapped
     *      region which is large enough to hold it.
     *
     *  @li @c VMSIZE: Size in bytes of the virtual memory to map.  If VMSIZE is not specified then it is the same as the
     *      number of bytes of DATA. If VMSIZE is smaller than DATA then the DATA will be truncated; if VMSIZE is larger than
     *      DATA then DATA is zero-padded. If the resulting memory are size is zero then no change is made to the memory map.
     *
     *  @li @c PERM: Accessibility for the mapped segment. If present, it should be any of the letters "r", "w", and/or "x" in
     *      that order to indicate readable, writable, and/or executable. If not present, then the new memory is readable,
     *      writable, and executable.
     *
     *  @li @c DATA: The byte values in ascending address order. The values should be separated from one another by white space
     *      and all values must be in the range 0 through 255, inclusive.  Values can be specified in hexadecimal (leading
     *      "0x"), binary (leading "0b"), octal (leading "0"), or decimal. */
    AddressInterval insertData(const std::string &locatorString);

    /** Documentation string for @ref insertData. */
    static std::string insertDataDocumentation();
    

#ifdef BOOST_WINDOWS
    void insertProcess(int pid, Attach::Boolean attach);
#else
    /** Insert the memory of some other process into this memory map. */
    void insertProcess(pid_t pid, Attach::Boolean attach);
#endif

    /** Insert the memory of some other process into this memory map.
     *
     *  The locator string follows the syntax described in @ref insertProcessDocumentation. */
    void insertProcess(const std::string &locatorString);

    /** Documentation string for @ref insertProcess. */
    static std::string insertProcessDocumentation();

    /** Erases regions of zero bytes that are executable and readable and at least @p minsize in size. */
    void eraseZeros(size_t minsize);

    /** Shrink buffers and remove sharing.
     *
     *  Creates a new buffer per segment and copies the data for that segment into the new buffer.  The new buffers are
     *  allocated to be just large enough to hold the data for the segment's interval.  Segments that shared the same
     *  underlying data no longer share data.
     *
     *  Returns true if new buffers could be allocated for all segments, and false otherwise.  A false return value could occur
     *  if a buffer does not support the @ref Sawyer::Container::Buffer::data. As of this writing (Nov 2016) the only buffer
     *  type that doesn't support @c data is @ref Sawyer::Container::NullBuffer "NullBuffer", which doesn't appear in
     *  memory maps created by ROSE's binary specimen mappers. */
    bool shrinkUnshare();

    /** Read data into buffer. */
    size_t readQuick(void *buf, rose_addr_t startVa, size_t desired) const {
        return at(startVa).limit(desired).require(READABLE).read((uint8_t*)buf).size();
    }

    /** Reads a NUL-terminated string from the memory map.  Reads data beginning at @p startVa in the memory map and
     *  continuing until one of the following conditions is met:
     *
     *  @li The desired number of characters has been read (returns empty string)
     *  @li The next character is the termination character (defaults to NUL)
     *  @li An @p invalidChar function is specified and the next character causes it to return true (returns empty string)
     *  @li A validChar function is specified and the next character causes it to return false (returns empty string)
     *
     *  The empty string is returned unless the terminator character is encountered.
     *
     *  The @p validChar and @p invalidChar take an integer argument and return an integer value so that the C character
     *  classification functions from <ctype.h> can be used directly. */
    std::string readString(rose_addr_t startVa, size_t desired, int(*validChar)(int)=NULL, int(*invalidChar)(int)=NULL,
                           unsigned requiredPerms=READABLE, unsigned prohibitedPerms=0, char terminator='\0') const;

    /** Read an unsigned value.
     *
     *  Reads an unsigned value from memory and converts it from the memory byte order to the host byte order.  If the entire
     *  value is not mapped in memory then return nothing (not even any part of the multi-byte value that might have been
     *  present. */
    template<typename U>
    Sawyer::Optional<U> readUnsigned(rose_addr_t startVa) const {
        U val = 0;
        if (at(startVa).limit(sizeof val).read((uint8_t*)&val).size() != sizeof val)
            return Sawyer::Nothing();
        ByteOrder::convert((void*)&val, sizeof val, endianness_, ByteOrder::host_order());
        return val;
    }

    /** Read quickly into a vector. */
    SgUnsignedCharList readVector(rose_addr_t startVa, size_t desired, unsigned requiredPerms=READABLE) const;

    /** Write data from buffer. */
    size_t writeQuick(const void *buf, rose_addr_t startVa, size_t desired) {
        return at(startVa).limit(desired).require(WRITABLE).write((const uint8_t*)buf).size();
    }

    /** Search for any byte.
     *
     *  Searches for all of the specified bytes simultaneously and returns the lowest address (subject to @p limits) where one
     *  of the specified values appears.  If none of the specified bytes appear within the given address extent, then this
     *  method returns none.
     *
     *  @{ */
    Sawyer::Optional<rose_addr_t> findAny(const Extent &limits, const std::vector<uint8_t> &bytesToFind,
                                          unsigned requiredPerms=READABLE, unsigned prohibitedPerms=0) const;
    Sawyer::Optional<rose_addr_t> findAny(const AddressInterval &limits, const std::vector<uint8_t> &bytesToFind,
                                          unsigned requiredPerms=READABLE, unsigned prohibitedPerms=0) const;
    /** @} */

    /** Search for a byte sequence.
     *
     *  Searches for the bytes specified by @p sequence occuring within the specified @p interval.  If the @p interval is empty
     *  or the sequence cannot be found then nothing is returned. Otherwise, the virtual address for the start of the sequence
     *  is returned. An empty sequence matches at the beginning of the @p interval. */
    Sawyer::Optional<rose_addr_t> findSequence(const AddressInterval &interval, const std::vector<uint8_t> &sequence) const;

    /** Prints the contents of the map for debugging. The @p prefix string is added to the beginning of every line of output
     *  and typically is used to indent the output.
     *  @{ */
    void dump(FILE*, const char *prefix="") const;
    void dump(std::ostream&, std::string prefix="") const;
    void print(std::ostream &o, std::string prefix="") const { dump(o, prefix); }
    /** @} */

    /** Compute a hash of the entire memory contents.
     *
     *  This hashes the memory contents. Segment information (names, addresses, permissions, etc) are not included in the hash;
     *  only the bytes stored in the map.  The user should supply a hasher whose @c append method will be called to add memory
     *  map contents to the hash.  For instance, here's one way to hash the contents of a file without having to read the
     *  entire file into memory first:
     *
     * @code
     *  MemoryMap::Ptr file = MemoryMap::instance();
     *  file->insertFile("/name/of/the/file", 0);
     *  HasherSha1 hasher;
     *  file->hash(hasher);
     *  std::cout <<"file SHA1 hash is " <<hash <<"\n";
     * @endcode */
    Combinatorics::Hasher& hash(Combinatorics::Hasher&) const;

    /** Title of a segment when printing the map. */
    static std::string segmentTitle(const Segment&);

    friend std::ostream& operator<<(std::ostream&, const MemoryMap&);
};

} // namespace
} // namespace

// Register the types needed for serialization since some of them are derived from polymorphic class templates.
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::MemoryMap::AllocatingBuffer);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::MemoryMap::MappedBuffer);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::MemoryMap::NullBuffer);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::MemoryMap::StaticBuffer);
#endif

#endif
#endif
