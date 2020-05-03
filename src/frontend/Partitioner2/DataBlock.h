#ifndef ROSE_Partitioner2_DataBlock_H
#define ROSE_Partitioner2_DataBlock_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/BasicTypes.h>
#include <SageBuilderAsm.h>
#include <SourceLocation.h>

#include <boost/serialization/access.hpp>
#include <Sawyer/Attribute.h>
#include <Sawyer/SharedPointer.h>

#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Data block information.
 *
 *  A data block represents static data with a type.  Data blocks are identified by a key consisting of their starting address
 *  and total size. Data blocks can be either attached to the partitioner or in a detached state. A block that is attached will
 *  be present in the Address Usage Map (AUM), but the AUM will store only one @ref DataBlock object per distinct key.
 *
 *  A data block can be owned by multiple basic blocks and/or functions. If an owning basic block or function is attached to
 *  the partitioner then the data block will also be attached. However, data blocks can be attached when some or all of their
 *  owners are in the detached state.  When attaching a basic block or function that owns a data block, if the AUM already has
 *  a different but equivaent (by keys) data block then the owning basic block or function will be adjusted to point to the
 *  equivalent data block. */
class DataBlock: public Sawyer::SharedObject, public Sawyer::Attribute::Storage<> {
public:
    /** Shared pointer to a data block. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<DataBlock> Ptr;

private:
    bool isFrozen_;                                     // true if object is read-only because it's in the CFG
    rose_addr_t startVa_;                               // starting address
    SgAsmType *type_;                                   // type of data stored in this block
    std::string comment_;                               // arbitrary comment, shown by printableName.
    std::vector<BasicBlockPtr> attachedBasicBlockOwners_; // attached basic blocks that own this data block, sorted and unique
    std::vector<FunctionPtr> attachedFunctionOwners_;   // attached functions that own this data block, sorted and unique
    SourceLocation sourceLocation_;                     // optional location of data in source code
    

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
        if (version < 2) {
            ASSERT_not_reachable("Rose::BinaryAnalysis::Partitioner2::DataBlock version 2 is no longer supported");
        } else {
            s & BOOST_SERIALIZATION_NVP(attachedBasicBlockOwners_);
            s & BOOST_SERIALIZATION_NVP(attachedFunctionOwners_);
        }
        if (version >= 3)
            s & BOOST_SERIALIZATION_NVP(sourceLocation_);
    }
#endif
    
protected:
    // needed for serialization
    DataBlock();
    DataBlock(rose_addr_t startVa, SgAsmType *type);

public:
    ~DataBlock();

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
    const std::string& comment() const;
    void comment(const std::string& s);
    /** @} */

    /** Property: Optional location of data in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    void sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; }
    /** @} */

    /** Number of attached basic block and function owners.
     *
     *  Returns the number of basic blocks and functions that are attached to the CFG/AUM and that own this data block. */
    size_t nAttachedOwners() const;

    /** Functions that are attached to the partitioner and own this data block.
     *
     *  The returned vector is sorted and has unique elements. */
    const std::vector<FunctionPtr>& attachedFunctionOwners() const;

    /** Basic blocks that are attached to the partitioner and own this data block.
     *
     *  The returned vector is sorted and has unique elements. */
    const std::vector<BasicBlockPtr>& attachedBasicBlockOwners() const;    
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
    void freeze();
    void thaw();

    // Insert the specified owner into this data block.
    void insertOwner(const BasicBlockPtr&);
    void insertOwner(const FunctionPtr&);

    // Erase the specified owner of this data block.
    void eraseOwner(const BasicBlockPtr&);
    void eraseOwner(const FunctionPtr&);
};


} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::DataBlock, 3);

#endif
#endif
