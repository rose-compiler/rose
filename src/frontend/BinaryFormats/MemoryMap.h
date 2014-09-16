#ifndef ROSE_MemoryMap_H
#define ROSE_MemoryMap_H

#include "ByteOrder.h"

#include <sawyer/Access.h>
#include <sawyer/AddressMap.h>
#include <sawyer/MappedBuffer.h>
#include <sawyer/Optional.h>

/* Increase ADDR if necessary to make it a multiple of ALMNT */
#define ALIGN_UP(ADDR,ALMNT)       ((((ADDR)+(ALMNT)-1)/(ALMNT))*(ALMNT))

/* Decrease ADDR if necessary to make it a multiple of ALMNT */
#define ALIGN_DN(ADDR,ALMNT)       (((ADDR)/(ALMNT))*(ALMNT))

/** An efficient mapping from an address space to stored data.
 *
 *  This class maps addresses in a 64-bit virtual address space to bytes stored in buffers and is a specialization of @ref
 *  Sawyer::Container::AddressMap.  A MemoryMap stores pairs of address ranges and segments, and each segment points to a
 *  buffer. The address space is segmented into non-overlapping, contiguous regions called "segments" (@ref
 *  Sawyer::Container::AddressSegment) and the addresses in a segment are mapped 1:1 onto data storage containers (@ref
 *  Sawyer::Container::Buffer).
 *
 *  Buffers come in a variety of kinds, all derived from @ref Sawyer::Container::Buffer and they are reference counted via
 *  shared-ownership smart pointers (@ref Sawyer::SharedPtr).  Always refer to a buffer with its @c Ptr type. They should be
 *  created with various @c instance class methods, and they should never be explicitly freed.
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
 *  MemoryMap map;
 *  map.insert(AddressInterval::baseSize(0x08040000, fileBuf->size()),
 *             AddressSegment(fileBuf, 0, MemoryMap::MM_PROT_READ, "the file contents"));
 *  map.insert(AddressInterval::baseSize(0x08040000+1024, dataBuf->size()),
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
 *  size_t nRead = map.at(0x08040100).limit(sizeof data).read(data).size();
 *  assert(nread==sizeof data);
 * @endcode
 *
 *  The Sawyer documentation contains many more examples.
 */
class MemoryMap: public Sawyer::Container::AddressMap<rose_addr_t, uint8_t> {
public:
    typedef rose_addr_t Address;
    typedef uint8_t Value;
    typedef Sawyer::Container::AddressMap<Address, Value> Super;
    typedef Sawyer::Container::Buffer<Address, Value> Buffer;
    typedef Sawyer::Container::AllocatingBuffer<Address, Value> AllocatingBuffer;
    typedef Sawyer::Container::MappedBuffer<Address, Value> MappedBuffer;
    typedef Sawyer::Container::NullBuffer<Address, Value> NullBuffer;
    typedef Sawyer::Container::StaticBuffer<Address, Value> StaticBuffer;
    typedef Sawyer::Container::SegmentPredicate<Address, Value> SegmentPredicate;

private:
    ByteOrder::Endianness endianness_;

public:

    // Whoever pollutes all namespaces with these common word proprocessor symbols is a lunatic!
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
    static const unsigned READABLE = Sawyer::Access::READABLE;
    static const unsigned WRITABLE = Sawyer::Access::WRITABLE;
    static const unsigned EXECUTABLE = Sawyer::Access::EXECUTABLE;
    static const unsigned IMMUTABLE = Sawyer::Access::IMMUTABLE;
    static const unsigned PRIVATE = 0x00000100;

public:
    /** Exception for MemoryMap operations. */
    class Exception: public std::runtime_error {
    public:
        Exception(const std::string &mesg, const MemoryMap *map): std::runtime_error(mesg), map(map) {}
        virtual ~Exception() throw() {}
        virtual std::string leader(std::string dflt="memory map problem") const;   /**< Leading part of the error message. */
        virtual std::string details(bool) const; /**< Details emitted on following lines, indented two spaces. */
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const Exception&);
    public:
        const MemoryMap *map;           /**< Map that caused the exception if available, null otherwise. */
    };

    /** Exception for an inconsistent mapping. This exception occurs when an attemt is made to insert a new segment but the
     *  address range of the new segment is already defined by an existing segment.  The @p new_range and @p new_segment are
     *  information about the segment that was being inserted, and the @p old_range and @p old_segment is information about
     *  an existing segment that conflicts with the new one. */
    struct Inconsistent : public Exception {
        Inconsistent(const std::string &mesg, const MemoryMap *map,
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
        NotMapped(const std::string &mesg, const MemoryMap *map, rose_addr_t va)
            : Exception(mesg, map), va(va) {}
        virtual ~NotMapped() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const NotMapped&);
        rose_addr_t va;
    };

    /** Exception thrown by find_free() when there's not enough free space left. */
    struct NoFreeSpace : public Exception {
        NoFreeSpace(const std::string &mesg, const MemoryMap *map, size_t size)
            : Exception(mesg, map), size(size) {}
        virtual ~NoFreeSpace() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const NoFreeSpace&);
        size_t size;
    };

    /** Exception thrown by load() when there's a syntax error in the index file. */
    struct SyntaxError: public Exception {
        SyntaxError(const std::string &mesg, const MemoryMap *map, const std::string &filename, unsigned linenum, int colnum=-1)
            : Exception(mesg, map), filename(filename), linenum(linenum), colnum(colnum) {}
        virtual ~SyntaxError() throw() {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const SyntaxError&);
        std::string filename;                   /**< Name of index file where error occurred. */
        unsigned linenum;                       /**< Line number (1 origin) where error occurred. */
        int colnum;                             /**< Optional column number (0-origin; negative if unknown). */
    };

public:
    /** Constructs an empty memory map. */
    MemoryMap(): endianness_(ByteOrder::ORDER_UNSPECIFIED) {}

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

    /** Insert file contents into memory map.
     *
     *  Insert the contents of a file into the memory map at the specified address.  This is just a convenience wrapper that
     *  creates a new MappedBuffer and inserts it into the mapping. Returns the size of the file mapping. */
    size_t insertFile(const std::string &fileName, rose_addr_t va, bool writable=false, const std::string &segmentName="");

    /** Erases regions of zero bytes that are executable and readable and at least @p minsize in size. */
    void eraseZeros(size_t minsize);

    /** Read data into buffer. */
    size_t readQuick(void *buf, rose_addr_t startVa, size_t desired) const {
        return at(startVa).limit(desired).require(READABLE).read((uint8_t*)buf).size();
    }
    
    /** Reads a NUL-terminated string from the memory map.  Reads data beginning at @p startVa in the memory map and
     *  continuing until one of the following conditions:
     *    <ul>
     *      <li>The return value contains the @p desired number of characters.</li>
     *      <li>The next character to be read is a NUL character.</li>
     *      <li>A @p validChar function is specified but the next character causes it to return zero.</li>
     *      <li>An @p invalidChar function is specified and the next character causes it to return non-zero.</li>
     *    </ul>
     *
     *  The @p validChar and @p invalidChar take an integer argument and return an integer value so that the C character
     *  classification functions from <ctype.h> can be used directly. */
    std::string readString(rose_addr_t startVa, size_t desired, int(*validChar)(int)=NULL, int(*invalidChar)(int)=NULL,
                           unsigned requiredPerms=READABLE, unsigned prohibitedPerms=0) const;

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
     *  method returns none. */
    Sawyer::Optional<rose_addr_t> findAny(const Extent &limits, const std::vector<uint8_t> &bytesToFind,
                                          unsigned requiredPerms=READABLE, unsigned prohibitedPerms=0) const;

    /** Prints the contents of the map for debugging. The @p prefix string is added to the beginning of every line of output
     *  and typically is used to indent the output.
     *  @{ */
    void dump(FILE*, const char *prefix="") const;
    void dump(std::ostream&, std::string prefix="") const;
    void print(std::ostream &o, std::string prefix="") const { dump(o, prefix); }
    /** @} */

    /** Title of a segment when printing the map. */
    static std::string segmentTitle(const Segment&);

    friend std::ostream& operator<<(std::ostream&, const MemoryMap&);
};

#endif
