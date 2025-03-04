#ifndef ROSE_BinaryAnalysis_Partitioner2_DataBlock_H
#define ROSE_BinaryAnalysis_Partitioner2_DataBlock_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/SourceLocation.h>
#include <SageBuilderAsm.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/SharedPointer.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

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
    /** Shared pointer to a data block. */
    typedef Sawyer::SharedPointer<DataBlock> Ptr;

private:
    bool isFrozen_;                                     // true if object is read-only because it's in the CFG
    Address startVa_;                                   // starting address
    SgAsmType *type_;                                   // type of data stored in this block
    std::string comment_;                               // arbitrary comment, shown by printableName.
    std::vector<BasicBlockPtr> attachedBasicBlockOwners_; // attached basic blocks that own this data block, sorted and unique
    std::vector<FunctionPtr> attachedFunctionOwners_;   // attached functions that own this data block, sorted and unique
    SourceLocation sourceLocation_;                     // optional location of data in source code
    

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;
    template<class S> void serialize(S&, unsigned version);
#endif
    
protected:
    // needed for serialization
    DataBlock();
    DataBlock(Address startVa, SgAsmType *type);

public:
    ~DataBlock();

public:
    /** Static allocating constructor. */
    static Ptr instance(Address startVa, SgAsmType*);

    /** Static allocating constructor.
     *
     *  Creates a data block whose type is just an array of bytes.
     *
     *  The @p startVa is the starting address of the data block. */
    static Ptr instanceBytes(Address startVa, size_t nBytes);

    /** Determine if data block is read-only.
     *
     *  Returns true if read-only, false otherwise. */
    bool isFrozen() const { return isFrozen_; }

    /** Returns the starting address. */
    Address address() const { return startVa_; }

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
    const SourceLocation& sourceLocation() const;
    void sourceLocation(const SourceLocation&);
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
    std::vector<uint8_t> read(const MemoryMapPtr&) const;

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
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::DataBlock, 3);
#endif

#endif
#endif
