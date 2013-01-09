#ifndef ROSE_MemoryMap_H
#define ROSE_MemoryMap_H

#include <boost/shared_ptr.hpp>

/* Increase ADDR if necessary to make it a multiple of ALMNT */
#define ALIGN_UP(ADDR,ALMNT)       ((((ADDR)+(ALMNT)-1)/(ALMNT))*(ALMNT))

/* Decrease ADDR if necessary to make it a multiple of ALMNT */
#define ALIGN_DN(ADDR,ALMNT)       (((ADDR)/(ALMNT))*(ALMNT))


/** An efficient mapping from an address space to stored data.
 *
 *  This class maps addresses in a 64-bit virtual address space to data stored in buffers.  The address space is segmented into
 *  non-overlapping, contiguous regions called "segments" (see MemoryMap::Segment) and the addresses in a segment are mapped
 *  1:1 onto data storage containers (see MemoryMap::Buffer).  A MemoryMap stores pairs of address ranges and segments, and
 *  each Segment points to a Buffer.
 *
 *  Buffers come in a variety of kinds, all derived from MemoryMap::Buffer, and they are reference counted via Boost smart
 *  pointers.  Always refer to a buffer with the MemoryMap::BufferPtr type. They should be created with various create() class
 *  methods, and they should never be explicitly freed.
 *
 *  Here's an example of mapping a file into an address space at virtual address 0x08040000 and then temporarily replacing the
 *  second 8k page of the file with our own data.  We demonstrate using an MmapBuffer because these are very fast for large
 *  files, especially if only small parts of the file are accessed.  We could have also used
 *  MemoryMap::ByteBuffer::create_from_file(), but this copies the entire file contents into a heap-allocated buffer, which is
 *  slower.
 *
 * @code
 *  // Create and initialize the overlay data
 *  my_data_size = 8192;
 *  uint8_t *my_data = new uint8_t[my_data_size];
 *  initialize(my_data, my_data_size);
 *
 *  // Create the two buffers: one for the file, one for the overlay data
 *  MemoryMap::BufferPtr file_buf = MemoryMap::MmapBuffer("the_file", O_RDONLY, PROT_READ, MAP_PRIVATE);
 *  MemoryMap::BufferPtr data_buf = MemoryMap::ByteBuffer(my_data, my_data_size);
 *  my_data = NULL; // it is now owned by data_buf and will be deleted automatically
 *
 *  // Create the memory map.
 *  MemoryMap map;
 *  map.insert(Extent(0x08040000, file_buf->size()),
 *             MemoryMap::Segment(file_buf, 0, MemoryMap::MM_PROT_READ, "the file contents"));
 *  map.insert(Extent(0x08042000, data_buf->size()),
 *             MemoryMap::Segment(data_buf, 0, MemoryMap::MM_PROT_RW, "data overlay"));
 * @endcode
 *
 *  A MemoryMap provides methods to easily read from and write to the underlying data storage, addressing it in terms of the
 *  virtual address space.  These functions return the number of bytes copied.
 *
 * @code
 *  // read part of the data, right across the file/overlay boundary
 *  uint8_t data[4096];
 *  size_t nread = map.read(data, 0x0804ff00, sizeof data);
 *  assert(nread==sizeof data);
 * @endcode
 *
 *  A MemoryMap is built on top of a RangeMap<Extent,Segment> and the map is available with via the segments() method.
 *  Therefore, all the usual RangeMap operations are available.  For instance, here's one way to determine if there's a large
 *  free area at 0xc0000000:
 *
 * @code
 *  // The set of addresses that are unmapped
 *  ExtentMap free_areas = map.segments().invert<ExtentMap>();
 *  bool b = free_areas.contains(Extent(0xc0000000,0x20000000));
 * @endcode
 */
class MemoryMap {
public:

    /** Mapping permissions. */
    enum Protection {
        /* Protection bits */
        MM_PROT_BITS    = 0x00000007,    /**< Bits used to indication memory region protections. */ /*NO_STRINGIFY*/
        MM_PROT_READ    = 0x00000001,    /**< Pages can be read. */
        MM_PROT_WRITE   = 0x00000002,    /**< Pages can be written. */
        MM_PROT_EXEC    = 0x00000004,    /**< Pages can be executed. */

        /* Protection convenience stuff */
        MM_PROT_NONE    = 0x00000000,    /**< Pages cannot be accessed. */
        MM_PROT_ANY     = 0x00000007,    /**< Any access. */
        MM_PROT_RW      = (MM_PROT_READ|MM_PROT_WRITE), /**< Read or write. */                  /*NO_STRINGIFY*/
        MM_PROT_RX      = (MM_PROT_READ|MM_PROT_EXEC),  /**< Read or execute. */                /*NO_STRINGIFY*/
        MM_PROT_RWX     = (MM_PROT_ANY),                                                        /*NO_STRINGIFY*/

        /* Other flags. These generally aren't interpreted by MemoryMap, but can be used to pass info.  When merging memory
         * segments, MemoryMap::Segment::merge() will not treat two regions as being consistent if they have different
         * bits set. */
        MM_PROT_FLAGS   = 0xfffffff0,   /**< Mask of protection bits that are available for use by other layers. */
        MM_PROT_PRIVATE = 0x00000010,   /**< Pages are not shared between mapped regions. */
    };

    /** Map copying. */
    enum CopyLevel {
        COPY_SHALLOW,                   /**< Copy segments, but not the buffers to which they point. */
        COPY_DEEP,                      /**< Copy segments and their buffers.  This copies buffer data. */
        COPY_ON_WRITE                   /**< Copy segments and mark them so they copy buffers on write. */
    };

    /**************************************************************************************************************************
     *                                  Buffers
     **************************************************************************************************************************/
public:
    class Buffer;
    typedef boost::shared_ptr<Buffer> BufferPtr;

    /** Base class for data associated with a memory segment.  Memory map buffers are reference counted and managed by
     *  boost::shared_ptr.  Use the class' create methods to allocate new Buffer objects.  A Buffer object usually (but not
     *  necessarily) points to an array of bytes, and it is up to the Buffer subclass as to how to manage that array.  The
     *  Buffer desctructor is only called when its reference count reaches zero. */
    class Buffer {
    public:
        virtual ~Buffer() {}

        /** Create a new buffer from an existing buffer.  The new buffer will point to a different copy of the data so that
         *  writing data into one buffer will not cause it to appear when reading from the other buffer.  The type of the new
         *  buffer might not be the same as this buffer because not all buffers are able to copy their data.  For instance,
         *  cloning an ExternBuffer or a MmapBuffer will create a ByteBuffer. */
        virtual BufferPtr clone() const = 0;

        /** Property indicating whether buffer is read-only.  The "modifiability" of a buffer is orthogonal to whether segments
         *  pointing to this buffer have the MemoryMap::MM_PROT_WRITE bit set.  For instance, the storage for the buffer can be
         *  a const buffer (is_read_only() returning true) even though a MemoryMap::Segment could be marked as writable.
         * @{ */
        virtual bool is_read_only() const { return read_only; }
        virtual void set_read_only(bool b=true) { read_only = b; }
        void clear_read_only() { set_read_only(false); } // final
        /** @} */

        /** Debug name for buffer.  Rather than print buffer pointers to distinguish one buffer from another, we use a three
         *  character name composed of the 26 lower-case letters.  The user can set this name to something else if they like.
         * @{ */
        virtual std::string get_name() const { return name; }
        virtual void set_name(const std::string &s) { name = s; }
        /** @} */

        /** Size of buffer in bytes.
         * @{ */
        virtual size_t size() const { return p_size; }
        virtual void resize(size_t n) { p_size = n; }
        /** @} */

        /** Reads data from a buffer.  Reads up to @p nbytes of data from this buffer and copies it to the caller-supplied
         *  address, the @p buf argument.  Reading starts at the specified byte offset from the beginning of this buffer.
         *  Returns the number of bytes copied. The output buffer is not zero-padded for short reads.
         *
         *  If @p buf is the null pointer, then no data is copied and the return value is the number of bytes that would have
         *  been copied if @p buf had not been null. */
        virtual size_t read(void *buf, size_t offset, size_t nbytes) const = 0;

        /** Writes data into a buffer. Writes up to @p nbytes of data from @p buf into this buffer starting at the specified
         *  byte offset within this buffer.  Returns the number of bytes written.  The return value will be less than @p nbytes
         *  if an error occurs. */
        virtual size_t write(const void *buf, size_t offset, size_t nbytes) = 0;

        /** Saves data to a file.  Writes the entire buffer contents to the specified file, creating or truncating the file if
         *  necessary. */
        virtual void save(const std::string &filename) const;

        /** Return pointer to low-level data.  This probably shouldn't be used to access the data because it makes assumptions
         *  about the implementation (use read() and write() methods instead).  But it is sometimes useful when trying to
         *  determine if a buffer was initialized with data known to the caller. */
        virtual const void* get_data_ptr() const = 0;

        /** Returns true if the buffer's data is all zero.  Some subclasses will be able to do something more efficient than
         *  reading all the data. */
        virtual bool is_zero() const;

    protected:
        Buffer(size_t size): read_only(false), p_size(size) { name=new_name(); }
        std::string new_name() /*final*/;

        bool read_only;                         /**< If true, buffer data cannot be modified. */
        std::string name;                       /**< Name for debugging. */
        size_t p_size;                          /**< Size of buffer in bytes. */
    };

    /** Buffer that has no data.  This can be useful to reserve areas of the virtual memory address space without actually
     *  storing any data at them.  All reads and writes using such a buffer will fail (return zero). */
    class NullBuffer: public Buffer {
    public:
        /** Construct a new buffer.  The buffer contains no data and all reads and writes will fail by returning zero. */
        static BufferPtr create(size_t size);

        /** Always returns the null pointer. */
        virtual const void *get_data_ptr() const /*overrides*/ { return NULL; }

        virtual BufferPtr clone() const { return create(size()); }
        virtual size_t read(void*, size_t offset, size_t nbytes) const /*overrides*/ { return 0; }
        virtual size_t write(const void*, size_t offset, size_t nbytes) /*overrides*/ { return 0; }

    protected:
        NullBuffer(size_t size): Buffer(size) {}
    };

    /** Buffer of data owned by someone else.  The data is not deleted when the buffer is deleted.  This class is mostly for
     *  backward compatibility with older versions of ROSE where memory maps never owned the data to which they pointed. */
    class ExternBuffer: public Buffer {
    public:
        /** Construct from caller-supplied data.  The caller supplies a pointer to data and the size of that data.  The new
         *  buffer object does not take ownership of the data or copy it, thus the caller-supplied data must continue to exist
         *  for as long as the Buffer exists.  This is mostly for backward compatibility with older versions of ROSE.
         * @{ */
        static BufferPtr create(void *data, size_t size);
        static BufferPtr create(const void *data, size_t size);
        /** @} */

        virtual BufferPtr clone() const;
        virtual void resize(size_t n) /*overrides*/;
        virtual const void *get_data_ptr() const /*overrides*/ { return p_data; }
        virtual size_t read(void*, size_t offset, size_t nbytes) const /*overrides*/;
        virtual size_t write(const void*, size_t offset, size_t nbytes) /*overrides*/;

    protected:
        ExternBuffer(const uint8_t *data, size_t size)
            : Buffer(size), p_data(const_cast<uint8_t*>(data)) { set_read_only(); }
        ExternBuffer(uint8_t *data, size_t size)
            : Buffer(size), p_data(data) {}
        mutable uint8_t *p_data;
    };

    /** Buffer of bytes.  The bytes are deleted when the buffer is deleted. */
    class ByteBuffer: public ExternBuffer {
    public:
        virtual ~ByteBuffer() { delete[] p_data; }

        /** Construct from caller-supplied data.  The caller supplies a pointer to data allocated on the heap (with new) and
         *  the size of that data.  The new buffer object takes ownership of the data, which is deleted when the buffer object
         *  is destroyed. */
        static BufferPtr create(void *data, size_t size);

        /** Construct from a file. The new buffer is created by reading all data from the specified file beginning at byte
         *  offset @p start_offset. */
        static BufferPtr create_from_file(const std::string &filename, size_t start_offset=0);

    protected:
        ByteBuffer(uint8_t *data, size_t size): ExternBuffer(data, size) {}
    };

    /** Buffer whose underlying storage is from mmap. */
    class MmapBuffer: public ExternBuffer {
    public:
        virtual ~MmapBuffer();

        /** Construct a new buffer from part of a file.  The file is mapped into memory via mmap and unmapped when the last
         *  reference to the buffer is deleted.  The arguments are the same as for mmap.  A MemoryMap::Exception is thrown if
         *  the file cannot be mapped.  The file is not closed after mapping. */
        static BufferPtr create(size_t length, int prot, int flags, int fd, off_t offset);

        /** Construct a new buffer from part of a file.  The file is opened with the flags @p oflags and then mapped into
         *  memory by calling mmap with the @p mprot and @p mflags arguments.  The entire file is mapped.  A
         *  MemoryMap::Exception is thrown if the file cannot be opened or the memory cannot be mapped.  The file is closed
         *  immediately after mapping. */
        static BufferPtr create(const std::string &filename, int oflags, int mprot, int mflags);

        /** Mmap buffers cannot be resized. This method will abort if called. */
        virtual void resize(size_t n) /*overrides*/;

    protected:
        MmapBuffer(uint8_t *data, size_t size, bool read_only): ExternBuffer(data, size) { set_read_only(read_only); }
    };

    /** Buffer of bytes.  The bytes are initialized to all zero and storage is allocated only when a non-zero value is written
     *  to the buffer. */
    class AnonymousBuffer: public ExternBuffer {
    public:
        virtual ~AnonymousBuffer() { delete[] p_data; }

        /** Construct an anonymous buffer. Storage for the buffer is allocated when a non-zero value is written to the buffer.
         *  Reads from unallocated buffers return all zero bytes. */
        static BufferPtr create(size_t size);

        virtual BufferPtr clone() const /*overrides*/;
        virtual const void *get_data_ptr() const /*overrides*/;
        virtual size_t read(void*, size_t offset, size_t nbytes) const /*overrides*/;
        virtual size_t write(const void*, size_t offset, size_t nbytes) /*overrides*/;
        virtual bool is_zero() const /*overrides*/;

    protected:
        AnonymousBuffer(size_t size): ExternBuffer((uint8_t*)NULL, size) {}
    };

    /**************************************************************************************************************************
     *                                  Segments
     **************************************************************************************************************************/
public:

    /** A contiguous, homogeneous region of an address space.  A Segment is a contiguous region of the address space that does
     *  not overlap with any other segment of the address space, and which corresponds with (part of) a single MemoryMap::Buffer
     *  object.  The addresses described by a Segment all have the same set of properties, such as the permission properties.
     *
     *  A segment doesn't know to which part of the virtual memory address space it's mapped--that's the responsibility of the
     *  MemoryMap class and the RangeMap on which it's implemented.
     *
     *  Segments reference count the MemoryMap::Buffer objects to which they point. */
    class Segment {
    public:
        Segment(): buffer_offset(0), mapperms(0), copy_on_write(false) {}

        /** Constructor.  Constructs a segment that points to a particular offset in a buffer.  The segment also has certain
         *  access permissions. */
        Segment(const BufferPtr &buffer, rose_addr_t offset, unsigned perms, std::string name="")
            : buffer(buffer), buffer_offset(offset), mapperms(perms), name(name), copy_on_write(false) {}

        /** Underlying buffer. Every segment must point to some underlying buffer that contains the data for the segment.  The
         *  addresses in the segment correspond 1:1 with the bytes in the buffer, although the segment's addresses can be
         *  shifted a constant offset within the buffer.  All addresses defined by the segment must correspond to valid data
         *  elements within the buffer. Buffers are reference counted using boost::shared_ptr, so the caller should never free
         *  the buffer.
         * @{ */
        BufferPtr get_buffer() const { return buffer; }
        void set_buffer(const BufferPtr &b) { buffer = b; }
        /** @} */

        /** Check segment-buffer compatibility.  The segment is verified to point to a Buffer object and that the addresses
         *  specified in the given @p range map to valid data in the buffer.  Returns true if everything checks out, false
         *  otherwise.
         *
         *  This method is necessary because a Segment doesn't actually know its range of addresses.  The association between
         *  address ranges and segments is managed by the MemoryMap object and the RangeMap on which it's based.
         *
         *  If the @p first_bad_va pointer is non-null, then it will be initialized with the lowest address in @p range which
         *  is invalid.  The initialization only occurs when check() returns false. */
        bool check(const Extent &range, rose_addr_t *first_bad_va=NULL) const;

        /** Mapping permissions.  The mapping permissions are a bit vector of MemoryMap::Protection bits.  These bits describe
         *  what operations can be performed on a segment's address space.  The set of operations can be further restricted by
         *  the underlying Buffer object.
         * @{ */
        unsigned get_mapperms() const { return mapperms; }
        void set_mapperms(unsigned p) { mapperms = p; }
        /** @} */

        /** Offset with respect to underlying Buffer.  The addresses of a segment correspond 1:1 with the bytes of the
         *  underlying Buffer object, but they can be shifted by a constant amount.  The shift amount is measured as a byte
         *  offset from the beginning of the buffer.
         *
         *  When setting an offset, the new offset must be compatible with the underlying buffer.  In other words, the
         *  addresses represented by the Segment must all continue to map to valid data locations in the buffer.  The actual
         *  consistency check is delayed until an operation like read() or write() because we don't have all the necessary
         *  information at the time of the set_buffer_offset() call.
         *
         *  The two-argument version of get_buffer_offset() computes the buffer offset for a virtual address that must be
         *  within the range of virtual addresses represented by this segment.
         * @{ */
        rose_addr_t get_buffer_offset() const { return buffer_offset; }
        rose_addr_t get_buffer_offset(const Extent &my_range, rose_addr_t va) const;
        void set_buffer_offset(rose_addr_t n);
        /** @} */

        /** Copy on write property.  If the copy-on-write property is set then the next write operation on this segment will
         *  cause its buffer to be copied first.  Once the buffer is copied the copy-on-write property is cleared. Doing a deep
         *  copy of a memory map will also clear the copy-on-write property in the new segments.
         * @{ */
        bool is_cow() const { return copy_on_write; }
        void set_cow(bool b=true) { copy_on_write = b; }
        void clear_cow() { set_cow(false); }
        /** @} */

        /** Segment equality.  Segments are equal if they point to the same buffer, have the same buffer offset, and have the
         *  same permissions.  Segment names are not used in the equality test. */
        bool operator==(const Segment &other) const;

        /** Name for debugging.
         * @{ */
        const std::string &get_name() const { return name; }
        void set_name(const std::string &s) { name = s; }
        /** @} */

        friend std::ostream& operator<<(std::ostream&, const Segment&);

    private:
        // Stuff for manipulating segment debug names
        typedef std::map<std::string, std::set<std::string> > NamePairings;
        void merge_names(const Segment &other);
        std::string get_name_pairings(NamePairings*) const;
        void set_name(const NamePairings&, const std::string &s1, const std::string &s2);

        // The following methods are part of the RangeMap interface.  See documentation in RangeMap::RangeMapVoid.
        friend class RangeMap<Extent, Segment>;
        void removing(const Extent &range);
        void truncate(const Extent &range, rose_addr_t new_end);
        bool merge(const Extent &range, const Extent &other_range, const Segment &other_segment);
        Segment split(const Extent &range, rose_addr_t new_end);
        void print(std::ostream&) const;

    private:
        BufferPtr buffer;               /**< The buffer holding data for this segment. */
        rose_addr_t buffer_offset;      /**< Starting byte offset into the buffer. */
        unsigned mapperms;              /**< Permissions for this segment. */
        std::string name;               /**< Name used for debugging purposes. */
        bool copy_on_write;             /**< Does the buffer need to be copied on the next write operation? */
    };

    /**************************************************************************************************************************
     *                                  RangeMap-related things
     **************************************************************************************************************************/
public:
    typedef RangeMap<Extent, Segment> Segments;
    typedef Segments::iterator iterator;
    typedef Segments::const_iterator const_iterator;
    typedef Segments::reverse_iterator reverse_iterator;
    typedef Segments::const_reverse_iterator const_reverse_iterator;

    /**************************************************************************************************************************
     *                                  Visitors
     **************************************************************************************************************************/
public:
    /** Visitor for traversing a memory map.  The return value is used when the visitor is employed by the prune() method. */
    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual bool operator()(const MemoryMap*, const Extent&, const Segment&) = 0;
    };

    /**************************************************************************************************************************
     *                                  Exceptions
     **************************************************************************************************************************/
public:
    /** Exception for MemoryMap operations. */
    class Exception {
    public:
        Exception(const std::string &mesg, const MemoryMap *map): mesg(mesg), map(map) {}
        virtual ~Exception() {}
        virtual std::string leader(std::string dflt="memory map problem") const;   /**< Leading part of the error message. */
        virtual std::string details(bool) const; /**< Details emitted on following lines, indented two spaces. */
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const Exception&);
    public:
        std::string mesg;               /**< Error message. Details of the exception. */
        const MemoryMap *map;           /**< Map that caused the exception if available, null otherwise. */
    };

    /** Exception for an inconsistent mapping. This exception occurs when an attemt is made to insert a new segment but the
     *  address range of the new segment is alread defined by an existing segment.  The @p new_range and @p new_segment are
     *  information about the segment that was being inserted, and the @p old_range and @p old_segment is information about
     *  an existing segment that conflicts with the new one. */
    struct Inconsistent : public Exception {
        Inconsistent(const std::string &mesg, const MemoryMap *map,
                     const Extent &new_range, const Segment &new_segment,
                     const Extent &old_range, const Segment &old_segment)
            : Exception(mesg, map),
              new_range(new_range), old_range(old_range),
              new_segment(new_segment), old_segment(old_segment) {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const Inconsistent&);
        Extent new_range, old_range;
        Segment new_segment, old_segment;
    };

    /** Exception for when we try to access a virtual address that isn't mapped. */
    struct NotMapped : public Exception {
        NotMapped(const std::string &mesg, const MemoryMap *map, rose_addr_t va)
            : Exception(mesg, map), va(va) {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const NotMapped&);
        rose_addr_t va;
    };

    /** Exception thrown by find_free() when there's not enough free space left. */
    struct NoFreeSpace : public Exception {
        NoFreeSpace(const std::string &mesg, const MemoryMap *map, size_t size)
            : Exception(mesg, map), size(size) {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const NoFreeSpace&);
        size_t size;
    };

    /** Exception thrown by load() when there's a syntax error in the index file. */
    struct SyntaxError: public Exception {
        SyntaxError(const std::string &mesg, const MemoryMap *map, const std::string &filename, unsigned linenum, int colnum=-1)
            : Exception(mesg, map), filename(filename), linenum(linenum), colnum(colnum) {}
        virtual void print(std::ostream&, bool verbose=true) const;
        friend std::ostream& operator<<(std::ostream&, const SyntaxError&);
        std::string filename;                   /**< Name of index file where error occurred. */
        unsigned linenum;                       /**< Line number (1 origin) where error occurred. */
        int colnum;                             /**< Optional column number (0-origin; negative if unknown). */
    };

    /**************************************************************************************************************************
     *                                  Public Methods for MemoryMap
     **************************************************************************************************************************/

    /** Constructs an empty memory map. */
    MemoryMap() {}

    /** Shallow copy constructor.  The new memory map describes the same mapping and points to shared copies of the underlying
     *  data.  In other words, changing the mapping of one map (clear(), insert(), erase()) does not change the mapping of the
     *  other, but changing the data (write()) in one map changes it in the other.  See also init(), which takes an argument
     *  describing how to copy. */
    MemoryMap(const MemoryMap &other, CopyLevel copy_level=COPY_SHALLOW) { init(other, copy_level); }

    /** Initialize this memory map with info from another.  This map is first cleared and then initialized with a copy of the
     *  @p source map.  A reference to this map is returned for convenience since init is often used in conjunction with
     *  constructors. */
    MemoryMap& init(const MemoryMap &source, CopyLevel copy_level=COPY_SHALLOW);

    /** Determines if a memory map is empty.  Returns true if this memory map contains no mappings. Returns false if at least
     *  one address is mapped. */
    bool empty() const { return p_segments.empty(); }

    /** Clear the entire memory map by erasing all addresses that are defined. */
    void clear();

    /** Define a new area of memory.  The @p segment is copied into the memory map and the reference count for the Buffer to
     *  which it points (if any) is incremented.  A check is performed to ensure that the @p range and @p segment are
     *  compatible (that the size of the range is not greater than the size of the segment's underlying buffer).  This
     *  operation is somewhat like the POSIX mmap() function.
     *
     *  If the @p range overlaps with existing segments and @p erase_prior is set (the default), then the overlapping parts of
     *  the virtual address space are first removed from the mapping.  Otherwise an overlap throws a MemoryMap::Inconsistent
     *  exception.   If an exception is thrown, then the memory map is not changed. */
    void insert(const Extent &range, const Segment &segment, bool erase_prior=true);

    /** Determines whether a virtual address is defined.  Returns true if the specified virtual address (or all addresses in a
     *  range of addresses) are defined, false otherwise.  An address is defined if it is associated with a Segment.  If @p
     *  required_perms is non-zero, then the address (or all addresses in the range) must be mapped with at least those
     *  permission bits.
     * @{ */
    bool exists(rose_addr_t va, unsigned required_perms=0) const { return exists(Extent(va), required_perms); }
    bool exists(Extent range, unsigned required_perms=0) const;
    /** @} */

    /** Erase parts of the mapping that correspond to the specified virtual address range. The addresses to be erased don't
     *  necessarily need to correspond to a similar add() call; for instance, it's possible to add a large address space and
     *  then erase parts of it to make holes.  This operation is somewhat like the POSIX munmap() function.
     *
     *  It is not an error to erase parts of the virtual address space that are not defined.  Note that it is more efficient to
     *  call clear() than to erase the entire virtual address space. */
    void erase(const Extent &range);

    /** Erase a single extent from a memory map.  Erasing by range is more efficient (O(ln N) vs O(N)) but sometimes it's more
     *  convenient to erase a single segment when we don't know it's range. */
    void erase(const Segment&);

    /** Get information about an address.  The return value is a pair containing the range of virtual addresses in the
     *  segment and a copy of the Segment object.  If the value is not found, then a RangeMap::NotMapped exception is thrown.
     *  See also, exists(). */
    std::pair<Extent, Segment> at(rose_addr_t va) const;

    /** Search for free space in the mapping.  This is done by looking for the lowest possible address not less than @p
     *  start_va and with the specified alignment where there are at least @p size free bytes. Throws a MemoryMap::NoFreeSpace
     *  exception if the search fails to find free space. */
    rose_addr_t find_free(rose_addr_t start_va, size_t size, rose_addr_t mem_alignment=1) const;

    /** Finds the highest area of unmapped addresses.  The return value is the starting address of the highest contiguous
     *  region of unmapped address space that starts at or below the specified maximum.  If no unmapped region exists then a
     *  MemoryMap::NoFreeSpace exception is thrown. */
    rose_addr_t find_last_free(rose_addr_t max=(rose_addr_t)(-1)) const;

    /** Traverses the segments of a map.  The visitor is called for each segment.  The visitor's return value is ignored. */
    void traverse(Visitor &visitor) const;

    /** Removes segments for which @p predicate returns true. If the predicate always returns false then nothing is removed
     *  from the map, and the predicate can be used for its side effect (such as counting how many map segments satisfy certain
     *  criteria. */
    void prune(Visitor &predicate);

    /** Removes segments based on permissions.  Keeps segments that have any of the required bits and none of the
     *  prohibited bits.   No bits are required if @p required is zero. */
    void prune(unsigned required, unsigned prohibited=MM_PROT_NONE);

    /** List of map segments. */
    const Segments &segments() const { return p_segments; }

    /** Copies data from a contiguous region of the virtual address space into a user supplied buffer. The portion of the
     *  virtual address space to copy begins at @p start_va and continues for @p desired bytes. The data is copied into the
     *  beginning of the @p dst_buf buffer. The return value is the number of bytes that were copied, which might be fewer
     *  than the number of  bytes desired if the mapping does not include part of the address space requested or part of the
     *  address space does not have MM_PROT_READ permission (or the specified permissions). The @p dst_buf bytes that do not
     *  correpond to mapped virtual addresses will be zero filled so that @p desired bytes are always initialized.
     *
     *  If @p dst_buf is the null pointer then this method only measures how many bytes could have been read.
     *
     *  The read() and read1() methods behave identically except read1() restricts the readable area to be from a single
     *  segment. Thus, read1() may return fewer bytes than read().
     *
     * @{ */
    size_t read(void *dst_buf, rose_addr_t start_va, size_t desired, unsigned req_perms=MM_PROT_READ) const;
    size_t read1(void *dst_buf, rose_addr_t start_va, size_t desired, unsigned req_perms=MM_PROT_READ) const;
    /** @} */

    /** Reads data from a memory map.  Reads data beginning at the @p start_va virtual address in the memory map and continuing
     *  for up to @p desired bytes, returning the result as an SgUnsignedCharList.  The read may be shorter than requested if
     *  we reach a point in the memory map that is not defined or which does not have the requested permissions.  The size of
     *  the return value indicates that number of bytes that were read (i.e., the return value is not zero-filled). */
    SgUnsignedCharList read(rose_addr_t start_va, size_t desired, unsigned req_perms=MM_PROT_READ) const;

    /** Copies data from a supplied buffer into the specified virtual addresses.  If part of the destination address space is
     *  not mapped, then all bytes up to that location are copied and no additional bytes are copied.  The write is also
     *  aborted early if a segment lacks the MM_PROT_WRITE bit (or specified bits) or its buffer is marked as read-only.  The
     *  return value is the number of bytes copied.
     *
     *  If @p src_buf is the null pointer then this method only measures how many bytes could have been written.
     *
     *  The write() and write1() methods behave identically, except write1() restricts the operation to a single segment. Thus,
     *  write1() may write fewer bytes than write().
     *
     *  @{ */
    size_t write(const void *src_buf, rose_addr_t start_va, size_t desired, unsigned req_perms=MM_PROT_WRITE);
    size_t write1(const void *src_buf, rose_addr_t start_va, size_t desired, unsigned req_perms=MM_PROT_WRITE);
    /** @} */

    /** Returns just the virtual address extents for a memory map. */
    ExtentMap va_extents() const;

    /** Sets protection bits for the specified address range.  The entire address range must already be mapped, but if @p
     *  relax is set then no exception is thrown if part of the range is not mapped (that part is just ignored).  This
     *  operation is somewhat like the POSIX mprotect() function. */
    void mprotect(Extent range, unsigned perms, bool relax=false);

    /** Prints the contents of the map for debugging. The @p prefix string is added to the beginning of every line of output
     *  and typically is used to indent the output.
     *  @{ */
    void dump(FILE*, const char *prefix="") const;
    void dump(std::ostream&, std::string prefix="") const;
    void print(std::ostream &o, std::string prefix="") const { dump(o, prefix); }
    /** @} */

    /** Dumps the entire map and its contents into a set of files.  The file names are constructed from the @p basename by
     *  appending a hypen and a hexadecimal address (without the leading "0x") and the extension ".data".  The text file whose
     *  name is constructed by appending ".index" to the @p basename contains an index of the memory map. */
    void dump(const std::string &basename) const;

    /** Read a memory map from a set of memory dump files. The argument should be the same basename that was given to an
     *  invocation of the dump() method. The memory map is adjusted according to the contents of the index file. Returns true
     *  if the data was successfully read in its entirety; note that when returning false, this memory map object might be
     *  partially changed (although still in a consistent state).
     *  
     *  This method also understands a more user-friendly dump index format. Each line of the index is either blank (containing
     *  only white space), a comment (introduced with a '#') or a segment specification.  A segment specification contains the
     *  following fields separated by white space (and/or a comma):
     * 
     *  <ul>
     *    <li>The virtual address for the start of this memory area.</li>
     *    <li>The size of this memory area in bytes.</li>
     *    <li>Mapping permissions consisting of the letters "r" (read), "w" (write), "x" (execute), or "p" (private).
     *        Hyphens also be present in this field and do not affect the permissions.</li>
     *    <li>The source of the data. This field consists of everything up to the next "0x" string (but leading and trailing
     *        white space is stripped). It may be the name of a file or the name of a buffer. Buffer names are only used for
     *        debugging.</li>
     *    <li>The byte offset of the start of data within the file. It allows a single file to contain multiple memory areas.</li>
     *    <li>An optional comment which will appear as the map element name for debugging.</li>
     *  </ul>
     *
     *  If an error occurs an exception is thrown. */
    bool load(const std::string &basename);

    friend std::ostream& operator<<(std::ostream&, const MemoryMap&);

    /**************************************************************************************************************************
     *                                  Data members
     **************************************************************************************************************************/
protected:
    Segments p_segments;
};

#endif
