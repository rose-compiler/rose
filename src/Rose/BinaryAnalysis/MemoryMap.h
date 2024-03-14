#ifndef ROSE_BinaryAnalysis_MemoryMap_H
#define ROSE_BinaryAnalysis_MemoryMap_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>

#include <Combinatorics.h>
#include <Rose/Exception.h>

#include <sageContainer.h>

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
 *  This class, a specialization of @ref Sawyer::Container::AddressMap, maps 64-bit addresses to bytes. The address space is
 *  organized into non-overlapping, contiguous "segments" (@ref Sawyer::Container::AddressSegment) that impart properties such
 *  as names and access permissions. Each segment points into a buffer (subclass of @ref Sawyer::Container::Buffer) where the
 *  data bytes are stored.
 *
 *  The buffers pointed to by the segments are reference counted with smart pointers. Normally, each segment points to the
 *  beginning of its own buffer, but this is not always true. Sometimes segments share buffers, and sometimes segments point
 *  into the middle of buffers. This flexibility allows parts of an address space to be erased or replaced even from the middle
 *  of existing segments. It also allows the same data to be mapped and shared at multiple addresses.
 *
 *  The MemoryMap objects are also reference counted and created by the @ref instance method. Copying a MemoryMap object is
 *  shallow: the new object will have its own mapping and segments, but will share the underying buffers with the source map.
 *
 *  Most of the low level functionality for a MemoryMap comes from the @ref Sawyer::Container::AddressMap base class, such as
 *  the ability to add, erase, or replace parts of the address space and the ability to read data from the address space. The
 *  base class documentation has lots of example code. On the other hand, most of the more complex behaviors are defined in the
 *  MemoryMap class, such as initializing a MemoryMap from a Linux process or reading a NUL-terminated string.
 *
 *  Here's an example of mapping a file into an address space at virtual address 0x08040000 and then temporarily replacing the
 *  second 1kB page of the file with our own data.  We demonstrate using a @ref Sawyer::Container::MappedBuffer because these
 *  are very fast for large files, especially if only small parts of the file are ever accessed.
 *
 * @code
 *  using namespace Sawyer::Container;
 *
 *  // Create and initialize the overlay data
 *  uint8_t myData[8192];
 *  initialize(myData, myDataSize);
 *
 *  // Create the two buffers: one for the file, one for the overlay data
 *  Buffer::Ptr fileBuf = MappedBuffer::instance("the_file", boost::iostreams::mapped_file::readonly);
 *  Buffer::Ptr dataBuf = StaticBuffer::instance(myData, 8192);
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
 *  assert(nread == sizeof data);
 * @endcode
 *
 *  The Sawyer documentation contains many more examples.
 */
class MemoryMap: public Sawyer::Container::AddressMap<rose_addr_t, uint8_t>, public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = MemoryMapPtr;

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

    /** Overwrite (parts of) existing segments? */
    enum class Clobber {NO, YES};

private:
    ByteOrder::Endianness endianness_;
    std::string name_;

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

    /** Property: Byte order.
     *
     *  Every map has a default byte order property which can be used by functions that read and write multi-byte values when
     *  the user does not provide a byte order to those functions.  The MemoryMap constructors initialize this property to @c
     *  ByteOrder::ORDER_UNSPECIFIED.
     *
     * @{ */
    ByteOrder::Endianness byteOrder() const { return endianness_; }
    void byteOrder(ByteOrder::Endianness order) { endianness_ = order; }
     /** @} */

    /** Property: Name.
     *
     *  Optional name of the map, mostly used for debugging.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
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

    /** Adjusts a memory map according to the locator string.
     *
     *  See --help output from tools that support this feature, or look at the implementation of @ref adjustMapDocumentation
     *  for details about the syntax of the @p locatorString. */
    void adjustMap(const std::string &locatorString);

    /** Documentation string for @ref adjustMap. */
    static std::string adjustMapDocumentation();


    /** Information about a process map. */
    struct ProcessMapRecord {
        AddressInterval interval;                       /** Mapped virtual addresses. */
        unsigned accessibility;                         /** The accessibility flags. */
        rose_addr_t fileOffset;                         /** Starting byte offset in the file. */
        std::string deviceName;                         /** The device from which the data is mapped, or "00:00". */
        size_t inode;                                   /** Inode on the device, or zero. */
        std::string comment;                            /** Optional comment. */

        ProcessMapRecord()
            : accessibility(0), fileOffset(0), inode(0) {}
    };

    /** Obtain the memory map information for a process.
     *
     *  Returns an empty vector if there was an error parsing the process information. */
    static std::vector<ProcessMapRecord> readProcessMap(pid_t);

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

    /** Insert part of another process's memory into this memory map.
     *
     * @{ */
    bool insertProcessPid(pid_t, const AddressInterval &where, unsigned accessibility, const std::string &name);
    void insertProcessPid(pid_t, const std::vector<ProcessMapRecord>&);
    bool insertProcessMemory(int memFile, const AddressInterval &where, unsigned accessibility, std::string name);
    /** @} */

    /** Documentation string for @ref insertProcess. */
    static std::string insertProcessDocumentation();

    /** Insert part of another map by reference.
     *
     *  The segments of the @p source map that overlap with the addresses specified by @p where are copied and inserted into @p this
     *  map. The new copied segments point to the same data buffers as @p source, therefore the segments in the new map will share
     *  the same data as the old map and changing the data in either map will change the data in the other map as well.
     *
     *  If the @p Clobber flag is set, then any segment that previously existed in the @p this map will be overwritten by segments
     *  copied from the @p source map. Otherwise, when @p Clobber is clear, only those parts of the @p source map that are not
     *  already mapped in @p this map are copied.
     *
     *  The return value is the set of addresses that were actually copied from the @p source to @p this map.
     *
     * @{ */
    AddressIntervalSet linkTo(const MemoryMap::Ptr &source, const AddressIntervalSet &where, Clobber = Clobber::YES);
    AddressIntervalSet linkTo(const MemoryMap::Ptr &source, const AddressInterval &where, Clobber = Clobber::YES);
    /** @} */

    /** Create a new map by padding and aligning segments.
     *
     *  A new map is created by padding the segments of this map so they're aligned in the returned map. Data is copied (not linked)
     *  from this map to the new map. After alignment and padding, it's possible that more than one segment from the source maps to
     *  a single segment in the destination. When this happens, the destination segment's name comes from the first corresponding
     *  source segment, and the destination access permissions are the union of all the corresponding source segment access
     *  permissions.
     *
     *  A low or high aligment of zero is treated as an alignment of one; that is, the endpoint is not aligned. Higher values will
     *  cause segment beginning address to be aligned downward, and segment one-past-end addresses to be aligned upward. */
    MemoryMap::Ptr align(rose_addr_t lowAlignment, rose_addr_t highAlignment) const;

    /** Copy part of a file into a buffer.
     *
     *  This copies (rather than directly references) part of a file and returns a pointer to a new buffer containing the data.
     *  If an error occurs when reading the file, then a buffer is still returned but its length will only be what was actually
     *  read, and a string is also returned containing the error message. */
    static std::pair<Buffer::Ptr, std::string> copyFromFile(int fd, const AddressInterval&);

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
        ByteOrder::convert((void*)&val, sizeof val, endianness_, ByteOrder::hostOrder());
        return val;
    }

    /** Read a long unsigned value.
     *
     *  Reads a long unsigned value from memory and converts it from the memory byte order to the host byte order.  If the entire
     *  value is not mapped in memory then return nothing (not even any part of the multi-byte value that might have been
     *  present. */
    Sawyer::Optional<uint64_t> readLongUnsinged(rose_addr_t startVa) const {
        uint64_t val = 0;
        if (at(startVa).limit(sizeof val).read((uint8_t*)&val).size() != sizeof val)
            return Sawyer::Nothing();
        ByteOrder::convert((void*)&val, sizeof val, endianness_, ByteOrder::hostOrder());
        return val;
    }
    
     /** Write an unsigned value.
     *
     *  Takes a unsigned value converts it from the memory byte order to the host byte order then writes to memory. 
     *  This does not verify the memory is writable. Returns the number of bytes written. */
    size_t writeUnsigned(uint32_t value, rose_addr_t startVa) {
        return at(startVa).limit(sizeof(uint32_t)).write((const uint8_t*)(&value)).size(); 
    }

     /** Write a long unsigned value.
     *
     *  Takes a long unsigned value converts it from the memory byte order to the host byte order then writes to memory. 
     *  This does not verify the memory is writable. Returns the number of bytes written. */
    size_t writeUnsigned(uint64_t value, rose_addr_t startVa) {
        return at(startVa).limit(sizeof(uint64_t)).write((const uint8_t*)(&value)).size(); 
    } 

    /** Read a byte from memory.
     *
     *  Reads a byte at the specified address and returns it. Returns nothing if the address is not mapped. */
    Sawyer::Optional<uint8_t> readByte(rose_addr_t) const;

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
    void dump() const;                                  // mostly for calling from within GDB or similar
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
