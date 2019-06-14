#ifndef ROSE_Partitioner2_DataBlock_H
#define ROSE_Partitioner2_DataBlock_H

#include <Partitioner2/BasicTypes.h>
#include <SageBuilderAsm.h>

#include <boost/serialization/access.hpp>
#include <Sawyer/Attribute.h>
#include <Sawyer/SharedPointer.h>

#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Data block information.
 *
 *  A data block represents data with a type. */
class DataBlock: public Sawyer::SharedObject, public Sawyer::Attribute::Storage<> {
public:
    /** Shared pointer to a data block. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<DataBlock> Ptr;

private:
    bool isFrozen_;                                     // true if object is read-only because it's in the CFG
    rose_addr_t startVa_;                               // starting address
    SgAsmType *type_;                                   // type of data stored in this block
    size_t nAttachedOwners_;                            // number of attached basic blocks and functions that own this data
    std::string comment_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        // s & boost::serialization::base_object<Sawyer::Attribute::Storage>(*this); -- not serialized
        s & BOOST_SERIALIZATION_NVP(isFrozen_);
        s & BOOST_SERIALIZATION_NVP(startVa_);
        if (version >= 1) {
            s & BOOST_SERIALIZATION_NVP(type_);
            s & BOOST_SERIALIZATION_NVP(comment_);
        } else if (S::is_loading::value) {
            size_t nBytes = 0;
            s & boost::serialization::make_nvp("size_", nBytes);
            type_ = SageBuilderAsm::buildTypeVector(nBytes, SageBuilderAsm::buildTypeU8());
        }
        s & BOOST_SERIALIZATION_NVP(nAttachedOwners_);
    }
#endif
    
protected:
    // needed for serialization
    DataBlock()
        : isFrozen_(false), startVa_(0), type_(NULL), nAttachedOwners_(0) {}

    DataBlock(rose_addr_t startVa, SgAsmType *type)
        : isFrozen_(false), startVa_(startVa), type_(type), nAttachedOwners_(0) {}

public:
    /** Static allocating constructor. */
    static Ptr instance(rose_addr_t startVa, SgAsmType*);

    /** Static allocating constructor.
     *
     *  Creates a data block whose type is just an array of bytes.
     *
     *  The @p startVa is the starting address of the data block. */
    static Ptr instanceBytes(rose_addr_t startVa, size_t nBytes);

    /** Determine if data block is read-only.
     *
     *  Returns true if read-only, false otherwise. */
    bool isFrozen() const { return isFrozen_; }

    /** Returns the starting address. */
    rose_addr_t address() const { return startVa_; }

    /** Returns the size in bytes. */
    size_t size() const;

    /** Property: Type of data stored in this block.
     *
     *  The type can only be changed when the data block is in a detached state (i.e., when @ref isFrozen is false).
     *
     * @{ */
    SgAsmType* type() const { return type_; }
    void type(SgAsmType *t);
    /** @} */

    /** Property: Comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    void comment(const std::string& s) { comment_ = s; }
    /** @} */

    /** Number of attached basic block and function owners.
     *
     *  Returns the number of data blocks and functions that are attached to the CFG/AUM and that own this data block. */
    size_t nAttachedOwners() const { return nAttachedOwners_; }

    /** Addresses represented. */
    AddressInterval extent() const;

    /** A printable name for this data block.  Returns a string like 'data block 0x10001234'. */
    std::string printableName() const;

    /** Read static data from a memory map.
     *
     *  This is a simple wrapper around memory map reading functionality so that the data for this static data block can
     *  be retrieved from the memory map without needing to know how to use the quite extensive memory map API.  This
     *  method reads this object's data from the provided memory map and returns a vector of the bytes.  The returned
     *  vector will be truncated if any of the bytes of this data block are not present in the map. */
    std::vector<uint8_t> read(const MemoryMap::Ptr&) const;

private:
    friend class Partitioner;
    void freeze() { isFrozen_ = true; }
    void thaw() { isFrozen_ = false; }
    size_t incrementOwnerCount();
    size_t decrementOwnerCount();
    void nAttachedOwners(size_t);
};


} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::DataBlock, 1);

#endif
