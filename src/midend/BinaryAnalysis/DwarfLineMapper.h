#ifndef ROSE_DwarfLineMapper_H
#define ROSE_DwarfLineMapper_H

namespace Rose {
namespace BinaryAnalysis {

// [Robb Matzke 2020-02-07]: Deprecated. Use Rose::BinaryAnalysis::SourceLocations from <BinarySourceLocations.h> instead.


/* Build mappings between source locations and virtual addresses.
 *
 *  This class reads DWARF debugging information from the AST and builds a bidirectional mapping between virtual addresses and
 *  source locations.  An address is associated with at most one source location, but a source location can have many
 *  addresses. This class is most efficient when the many addresses of a single source location are mostly contiguous. */
class DwarfLineMapper: public AstSimpleProcessing {
public:
    /* Mapping direction indicator. */
    enum Direction {
        ADDR2SRC = 0x01,                /* Build a mapping from virtual address to source location. */
        SRC2ADDR = 0x02,                /* Build a mapping from source location to virtual address. */
        BIDIRECTIONAL = 0x03            /* Build bidirectional mapping between virtual addresses and source locations. */
    };

    /*  Source location info. We use a struct rather than an SgAsmDwarfLine pointer because we might have many SgAsmDwarfLine
     *  objects that all have the same file and line number (because they need different SgNode::p_parent values). This makes
     *  it easier to use the file and line numbers as std::map keys and RangeMap values. */
    struct SrcInfo {
        SrcInfo(): file_id(Sg_File_Info::NULL_FILE_ID), line_num(0) {}
        SrcInfo(int file_id, size_t line_num): file_id(file_id), line_num(line_num) {}
        bool operator==(const SrcInfo &other) const { return file_id==other.file_id && line_num==other.line_num; }
        bool operator<(const SrcInfo &other) const {
            return file_id<other.file_id || (file_id==other.file_id && line_num<other.line_num);
        }
        void print(std::ostream &o) const { o <<Sg_File_Info::getFilenameFromID(file_id) <<":" <<line_num; }
        int file_id;
        size_t line_num;
    };

    /* Value stored by the addr2src RangeMap.
     * 
     *  A RangeMap is used to represent the mapping from addresses to source position because it is able to efficiently
     * represent cases when one source position corresponds to a large number of contiguous addresses. */
    class RangeMapLocationInfo: public RangeMapValue<Extent, SrcInfo> {
    public:
        typedef RangeMapValue<Extent, SrcInfo> Super;
        RangeMapLocationInfo() {}
        RangeMapLocationInfo(const SrcInfo &value): Super(value) {}
        RangeMapLocationInfo split(const Extent &my_range, rose_addr_t new_end) {
            assert(my_range.contains(Extent(new_end)));
            return *this;
        }
    };

    typedef RangeMap<Extent, RangeMapLocationInfo> AddressSourceMap;
    typedef std::map<SrcInfo, ExtentMap> SourceAddressMap;

public:
    /* Construct an empty line mapper. */
    explicit DwarfLineMapper(Direction d=BIDIRECTIONAL) ROSE_DEPRECATED("use SourceLocations")
        : p_direction(d), up_to_date(true) { init(); }

    /* Create a new mapping using Dwarf information found in the specified part of the AST. */
    explicit DwarfLineMapper(SgNode *ast, Direction d=BIDIRECTIONAL) ROSE_DEPRECATED("use SourceLocations") {
        init(ast, d);
    }

    /*  Replace the current mapping with a new mapping. The new mapping is constructed from Dwarf information found in the
     *  specified part of the AST. */
    void init(SgNode *ast, Direction d=BIDIRECTIONAL) ROSE_DEPRECATED("use SourceLocations");

    /*  Insert additional mapping information from an AST without first clearing existing mapping info.  Conflicts are resolved
     *  in favor of the new information (since an address can be associated with at most one source position). */
    void insert(SgNode *ast) ROSE_DEPRECATED("use SourceLocations");

    /* Clear all mapping information. */
    void clear() ROSE_DEPRECATED("use SourceLocations");

    /* Fill in small holes.  The dwarf line information stored in a file typically maps source location to only the first
     * virtual address for that source location.  This method fills in the mapping so that any unmapped virtual address within
     * a certain delta of a previous mapped address will map to the same source location as the previous mapped address.
     * Mappings will be added only for addresses where the distance between the next lower mapped address and the next higher
     * mapped address is less than or equal to @p max_hole_size. */
    void fix_holes(size_t max_hole_size=64) ROSE_DEPRECATED("use SourceLocations");

    /*  Given an address, return the (single) source location for that address. If the specified address is not in the domain
     *  of this mapping function, then return the pair (Sg_File_Info::NULL_FILE_ID,0). */
    SrcInfo addr2src(rose_addr_t) const ROSE_DEPRECATED("use SourceLocations");

    /* Given a source location, return the addresses that are associated with the location. */
    ExtentMap src2addr(const SrcInfo&) const ROSE_DEPRECATED("use SourceLocations");

    /*  Given a source location, return the first address associated with the location. Returns zero (null address) if
     *  no address is associated with that location.  See also src2addr(). */
    rose_addr_t src2first_addr(const SrcInfo&) const ROSE_DEPRECATED("use SourceLocations");

    /* Print the mapping.  The forward (addr2src) and/or reverse (src2addr) mapping is printed, with each map entry terminated
     * by a linefeed. */
    void print(std::ostream&) const ROSE_DEPRECATED("use SourceLocations");

    /* Print the address-to-source mapping in a human-readable format. */
    void print_addr2src(std::ostream&) const ROSE_DEPRECATED("use SourceLocations");

    /* Print the source-to-address mapping in a human-readable format. */
    void print_src2addr(std::ostream&) const ROSE_DEPRECATED("use SourceLocations");

    /* Change print direction. */
    DwarfLineMapper& operator()(Direction d)  ROSE_DEPRECATED("use SourceLocations") {
        p_direction = d;
        return *this;
    }

    /*  Return the set of all source files.  The items in the set are the file IDs stored in the SgAsmDwarfLine objects and can
     *  be converted to strings via Sg_File_Info::getFilenameFromID(). */
    std::set<int> all_files() const ROSE_DEPRECATED("use SourceLocations");

    /*  Given a source position, return the next source position that has a mapping.  When called with no argument, the first
     *  source position is returned.  When the end of the list is reached, a default-constructed SrcInfo object is returned. */
    SrcInfo next_src(const SrcInfo &srcinfo = SrcInfo()) const ROSE_DEPRECATED("use SourceLocations");

protected:
    Direction p_direction;                              // Direction to use when printing
    AddressSourceMap p_addr2src;                        // Forward mapping
    mutable SourceAddressMap p_src2addr;                // Reverse mapping
    mutable bool up_to_date;                            // Is reverse mapping up-to-date?
    virtual void visit(SgNode *node) ROSE_OVERRIDE;
    void update() const;                                // update p_src2addr if necessary
    void init();                                        // called by constructors
};

std::ostream& operator<<(std::ostream&, const DwarfLineMapper::SrcInfo&);
std::ostream& operator<<(std::ostream&, const DwarfLineMapper&);

} // namespace
} // namespace

#endif
