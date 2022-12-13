// Tests that DataBlock ownership rules work correctly
#include <rose.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

using namespace Rose;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions for the testing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Data blocks are identified by their extent (starting address and size). This function returns a new
// unique data block each time it's called.
static P2::DataBlock::Ptr
createUniqueDataBlock(size_t nBytes) {
    static rose_addr_t nextVa = 0;
    rose_addr_t va = nextVa;
    nextVa += nBytes;
    return P2::DataBlock::instanceBytes(va, nBytes);
}

static void
checkAum(const P2::Partitioner::ConstPtr &partitioner, const P2::DataBlock::Ptr &db, const std::set<P2::BasicBlock::Ptr> &owners) {
    ASSERT_not_null(partitioner);
    ASSERT_always_not_null(db);
    for (rose_addr_t i = 0; i < db->size(); ++i) {
        P2::AddressUsers users = partitioner->aum().spanning(db->address() + i);
        std::set<P2::BasicBlock::Ptr> bblocks = owners;
        size_t nDataBlocksFound = 0;
        for (const P2::AddressUser &user : users.addressUsers()) {
            if (user.dataBlock() == db) {
                ++nDataBlocksFound;
                ASSERT_always_require(nDataBlocksFound == 1);
                BOOST_FOREACH (P2::BasicBlock::Ptr owningBasicBlock, db->attachedBasicBlockOwners()) {
                    std::set<P2::BasicBlock::Ptr>::iterator found = bblocks.find(owningBasicBlock);
                    ASSERT_always_require2(found != bblocks.end(),
                                           "at " + StringUtility::addrToString(db->address() + i) +
                                           " " + db->printableName() +
                                           " is unexpectedly owned by " + owningBasicBlock->printableName());
                    bblocks.erase(found);
                }
            }
            for (P2::BasicBlock::Ptr bb : bblocks) {
                ASSERT_not_reachable("at " + StringUtility::addrToString(db->address() + i) +
                                     " " + db->printableName() + " was expected be owned by " + bb->printableName());
            }
        }
    }
}

static void
checkAum(const P2::Partitioner::ConstPtr &partitioner, const P2::DataBlock::Ptr &db, std::set<P2::Function::Ptr> owners) {
    ASSERT_not_null(partitioner);
    ASSERT_always_not_null(db);
    for (rose_addr_t i = 0; i < db->size(); ++i) {
        P2::AddressUsers users = partitioner->aum().spanning(db->address() + i);
        std::set<P2::Function::Ptr> functions = owners;
        size_t nDataBlocksFound = 0;
        for (const P2::AddressUser &user : users.addressUsers()) {
            if (user.dataBlock() == db) {
                ++nDataBlocksFound;
                ASSERT_always_require(nDataBlocksFound == 1);
                for (P2::Function::Ptr owningFunction : db->attachedFunctionOwners()) {
                    std::set<P2::Function::Ptr>::iterator found = functions.find(owningFunction);
                    ASSERT_always_require2(found != functions.end(),
                                           "at " + StringUtility::addrToString(db->address() + i) +
                                           " " + db->printableName() +
                                           " is unexpectedly owned by " + owningFunction->printableName());
                    functions.erase(found);
                }
            }
        }
        for (P2::Function::Ptr function : functions) {
            ASSERT_not_reachable("at " + StringUtility::addrToString(db->address() + i) +
                                 " " + db->printableName() +
                                 " was expected be owned by " + function->printableName());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The actual testing functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Attach a naked data block to the partitioner without the data block being owned by any basic block or function.
static void
testNoOwner() {
    auto partitioner = P2::Partitioner::instance();
    std::set<P2::Function::Ptr> owners;
    P2::DataBlock::Ptr db = createUniqueDataBlock(8);
    ASSERT_not_null(db);
    db->comment("db");
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);

    size_t n = partitioner->nDataBlocks();
    partitioner->attachDataBlock(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(partitioner->nDataBlocks() == n+1);
    partitioner->checkConsistency();
    checkAum(partitioner, db, owners);

    partitioner->detachDataBlock(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(partitioner->nDataBlocks() == n);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);
}

// Attach two data blocks having the same address and size to the partitioner without any owning basic block or function. The
// partitioner should only see one of the two blocks.
static void
testNoOwnerDuplicate() {
    // Create and attach the first data block
    auto partitioner = P2::Partitioner::instance();
    std::set<P2::Function::Ptr> owners;
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    db1->comment("db1");
    partitioner->attachDataBlock(db1);
    partitioner->checkConsistency();
    checkAum(partitioner, db1, owners);

    // Create a second data block having the same identification.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db2->comment("db2");
    ASSERT_always_require(db2 != db1);
    ASSERT_always_require(db2->address() == db1->address());
    ASSERT_always_require(db2->size() == db1->size());
    partitioner->checkConsistency();

    // Attaching the second data block to the AUM is a no-op, returning the first block instead.
    P2::DataBlock::Ptr found = partitioner->attachDataBlock(db2);
    ASSERT_always_not_null(found);
    ASSERT_always_require(found == db1);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(partitioner->nDataBlocks() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);
}

// Attach and detach data blocks to a detached basic block, including attempting to attach a data block that has the same
// address and size as an existing data block.
static void
testDetachedBasicBlock(const P2::Partitioner::Ptr &partitioner) {
    // Find any old basic block and detach it for this test
    P2::BasicBlock::Ptr bb;
    for (const P2::ControlFlowGraph::Vertex &vertex : partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_always_not_null(bb);
    partitioner->detachBasicBlock(bb);
    ASSERT_always_require(!bb->isFrozen());

    size_t n1 = partitioner->nDataBlocks();
    size_t n2 = bb->nDataBlocks();

    // Attach a first data block to the detached basic block. The data block should not be part of the AUM since the basic
    // block is not attached to the CFG/AUM.
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    db1->comment("db1");
    P2::DataBlock::Ptr db1b = partitioner->attachDataBlockToBasicBlock(db1, bb);
    ASSERT_always_require(db1b == db1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    // Attach a second data block to the detached basic block.
    P2::DataBlock::Ptr db2 = createUniqueDataBlock(8);
    db2->comment("db2");
    P2::DataBlock::Ptr db2b = partitioner->attachDataBlockToBasicBlock(db2, bb);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(bb->nDataBlocks() == n2 + 2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);

    // Attach a third data block having the same address and size as the first block. This should be a no-op.
    P2::DataBlock::Ptr db3 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db3->comment("db3");
    P2::DataBlock::Ptr db3b = partitioner->attachDataBlockToBasicBlock(db3, bb);
    ASSERT_always_require(db3b == db1);
    ASSERT_always_require(!db3->isFrozen());
    ASSERT_always_require(bb->nDataBlocks() == n2 + 2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == nullptr);

    // Detach the data blocks from the basic block.
    db3b = bb->eraseDataBlock(db3);
    ASSERT_require(db3b == db1);                        // since db3 isn't owned by the basic block
    db1b = bb->eraseDataBlock(db1);
    ASSERT_always_require(nullptr == db1b);                // already erased
    db2b = bb->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(bb->nDataBlocks() == n2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == nullptr);

    // Re-attach the basic block
    partitioner->attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == nullptr);
}

// Attach and detach data blocks to a detached function, including attempting to attach a data block that has the same address
// and size as an existing data block.
static void
testDetachedFunction(const P2::Partitioner::Ptr &partitioner) {
    // Find any function and detach it for this test
    ASSERT_always_require(partitioner->nFunctions() >= 1);
    P2::Function::Ptr f = partitioner->functions()[0];
    ASSERT_always_not_null(f);
    partitioner->detachFunction(f);
    ASSERT_always_require(!f->isFrozen());
    
    size_t n1 = partitioner->nDataBlocks();
    size_t n2 = f->nDataBlocks();

    // Attach a first data block to the detached function. The data block should not be part of the AUM since the function
    // is not attached to the CFG/AUM.
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    db1->comment("db1");
    P2::DataBlock::Ptr db1b = partitioner->attachDataBlockToFunction(db1, f);
    ASSERT_always_require(db1b == db1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    // Attach a second data block to the detached function.
    P2::DataBlock::Ptr db2 = createUniqueDataBlock(8);
    db2->comment("db2");
    P2::DataBlock::Ptr db2b = partitioner->attachDataBlockToFunction(db2, f);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(f->nDataBlocks() == n2 + 2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);

    // Attach a third data block having the same address and size as the first block. This should be a no-op.
    P2::DataBlock::Ptr db3 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db3->comment("db3");
    P2::DataBlock::Ptr db3b = partitioner->attachDataBlockToFunction(db3, f);
    ASSERT_always_require(db3b == db1);
    ASSERT_always_require(!db3->isFrozen());
    ASSERT_always_require(f->nDataBlocks() == n2 + 2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == nullptr);

    // Detach the data blocks from the function
    db3b = f->eraseDataBlock(db3);
    ASSERT_require(db3b == db1);                        // since db3 isn't owned by the basic block
    db1b = f->eraseDataBlock(db1);
    ASSERT_always_require(nullptr == db1b);                // already erased
    db2b = f->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(f->nDataBlocks() == n2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == nullptr);

    // Re-attach the function
    partitioner->attachFunction(f);
    ASSERT_always_require(f->nDataBlocks() == n2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == nullptr);
}

// Attach data blocks to a basic block that's attached to the CFG/AUM.
static void
testAttachedBasicBlock(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::BasicBlock::Ptr> owners;

    // Find any old basic block
    P2::BasicBlock::Ptr bb;
    for (const P2::ControlFlowGraph::Vertex &vertex : partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_always_not_null(bb);
    bb->comment("bb");

    // Create data block
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    ASSERT_not_null(db1);
    db1->comment("db1");
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!bb->dataBlockExists(db1));
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    // Attach data block to an attached basic block
    size_t n1 = partitioner->nDataBlocks();
    size_t n2 = bb->nDataBlocks();
    partitioner->attachDataBlockToBasicBlock(db1, bb);
    owners.insert(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // It's not legal to detach the data block because it has attached owners
    try {
        partitioner->detachDataBlock(db1);
        ASSERT_not_reachable("should have failed");
    } catch (const P2::DataBlockError&) {
    }
    partitioner->checkConsistency();

    // Detaching the basic block detaches the data block without changing ownership
    partitioner->detachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    // Reattaching the basic block reattaches the data block
    partitioner->attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Attaching a second data block having the same address and size as the first is a no-op.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db2->comment("db2");
    P2::DataBlock::Ptr db2b = partitioner->attachDataBlockToBasicBlock(db2, bb);
    ASSERT_always_require(db2b == db1);                 // db1 is the canonical block
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataBlockExists(db2) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);

    // Detach first data block using second data block as the alias.
    partitioner->detachBasicBlock(bb);
    db2b = bb->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);                 // actually removed db1
    ASSERT_always_require(bb->nDataBlocks() == n2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);

    partitioner->attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    partitioner->checkConsistency();
}

// Attach data blocks to a function that's attached to the CFG/AUM.
static void
testAttachedFunction(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::Function::Ptr> owners;

    // Find any function
    ASSERT_always_require(partitioner->nFunctions() >= 1);
    P2::Function::Ptr f = partitioner->functions()[0];
    ASSERT_always_not_null(f);
    f->name("f");

    // Create data block
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    ASSERT_not_null(db1);
    db1->comment("db1");
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db1));
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    // Attach data block to an attached function
    size_t n1 = partitioner->nDataBlocks();
    size_t n2 = f->nDataBlocks();
    partitioner->attachDataBlockToFunction(db1, f);
    owners.insert(f);
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) = db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // It's not legal to detach the data block because it has attached owners
    try {
        partitioner->detachDataBlock(db1);
        ASSERT_not_reachable("should have failed");
    } catch (const P2::DataBlockError&) {
    }
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) = db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Detaching the function detaches the data block without changing ownership
    partitioner->detachFunction(f);
    owners.erase(f);
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) = db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    // Reattaching the function reattaches the data block
    partitioner->attachFunction(f);
    owners.insert(f);
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) == db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Attaching a second data block having the same address and size as the first is a no-op.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db2->comment("db2");
    P2::DataBlock::Ptr db2b = partitioner->attachDataBlockToFunction(db2, f);
    ASSERT_always_require(db2b == db1);                 // db1 is the canonical block
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) == db1);
    ASSERT_always_require(f->dataBlockExists(db2) == db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(partitioner->nDataBlocks() == n1 + 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);

    // Detach first basic block using second basic block as the alias.
    partitioner->detachFunction(f);
    db2b = f->eraseDataBlock(db2);
    owners.erase(f);
    ASSERT_always_require(db2b == db1);                 // actually removed db1
    ASSERT_always_require(f->nDataBlocks() == n2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    partitioner->checkConsistency();

    partitioner->attachFunction(f);
    ASSERT_always_require(f->nDataBlocks() == n2);
    ASSERT_always_require(partitioner->nDataBlocks() == n1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
    checkAum(partitioner, db1, owners);
}

// Attach same data block to two different basic blocks
static void
testTwoBasicBlocks(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::BasicBlock::Ptr> owners;

    // Find any two basic block
    P2::BasicBlock::Ptr bb1, bb2;
    for (const P2::ControlFlowGraph::Vertex &vertex : partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            if (!bb1) {
                bb1 = vertex.value().bblock();
            } else {
                bb2 = vertex.value().bblock();
                break;
            }
        }
    }
    ASSERT_always_not_null(bb1);
    ASSERT_always_not_null(bb2);
    bb1->comment("bb1");
    bb2->comment("bb2");

    // Create data block
    P2::DataBlock::Ptr db= createUniqueDataBlock(8);
    ASSERT_not_null(db);
    db->comment("db");
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb1->dataBlockExists(db));
    ASSERT_always_require(!bb2->dataBlockExists(db));
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);

    // Make the data block owned by both basic blocks
    partitioner->attachDataBlockToBasicBlock(db, bb1);
    owners.insert(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(!bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    partitioner->attachDataBlockToBasicBlock(db, bb2);
    owners.insert(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // Detaching only one basic block doesn't detach the data block or change any ownership
    partitioner->detachBasicBlock(bb1);
    owners.erase(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // But detaching both basic blocks does detach the data block
    partitioner->detachBasicBlock(bb2);
    owners.erase(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);

    // Reattaching a basic block reattaches the data block
    partitioner->attachBasicBlock(bb1);
    owners.insert(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // Reattaching the other basic block doesn't do much
    partitioner->attachBasicBlock(bb2);
    owners.insert(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // Detach the data block from both basic blocks.
    partitioner->detachBasicBlock(bb1);
    partitioner->detachBasicBlock(bb2);
    bb1->eraseDataBlock(db);
    bb2->eraseDataBlock(db);
    partitioner->attachBasicBlock(bb1);
    partitioner->attachBasicBlock(bb2);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);
}

// Attach same data block to two functions.
static void
testTwoFunctions(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::Function::Ptr> owners;

    // Find any two functions
    ASSERT_always_require(partitioner->nFunctions() >= 2);
    P2::Function::Ptr f1 = partitioner->functions()[0];
    P2::Function::Ptr f2 = partitioner->functions()[1];
    f1->name("f1");
    f2->name("f2");

    // Create data block
    P2::DataBlock::Ptr db = createUniqueDataBlock(8);
    ASSERT_not_null(db);
    db->comment("db");
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!f1->dataBlockExists(db));
    ASSERT_always_require(!f2->dataBlockExists(db));
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);
    
    // Make the data block owned by both functions
    partitioner->attachDataBlockToFunction(db, f1);
    owners.insert(f1);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(!f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    partitioner->attachDataBlockToFunction(db, f2);
    owners.insert(f2);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // Detaching only one function doesn't detach the data block or change any ownership
    partitioner->detachFunction(f1);
    owners.erase(f1);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // But detaching both functions does detach the data block
    partitioner->detachFunction(f2);
    owners.erase(f2);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);

    // Reattaching a function reattaches the data block
    partitioner->attachFunction(f1);
    owners.insert(f1);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // Reattaching the other function doesn't do much
    partitioner->attachFunction(f2);
    owners.insert(f2);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, owners);

    // Detach the data block from both functions
    partitioner->detachFunction(f1);
    partitioner->detachFunction(f2);
    f1->eraseDataBlock(db);
    f2->eraseDataBlock(db);
    partitioner->attachFunction(f1);
    partitioner->attachFunction(f2);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);
}

// Attach same data block to both a basic block and a function
static void
testBasicBlockFunction(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::BasicBlock::Ptr> bowners;
    std::set<P2::Function::Ptr> fowners;

    // Find any basic block and function
    P2::BasicBlock::Ptr bb;
    for (const P2::ControlFlowGraph::Vertex &vertex : partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_not_null(bb);
    ASSERT_always_require(partitioner->nFunctions() >= 1);
    P2::Function::Ptr f = partitioner->functions()[0];
    bb->comment("bb");
    f->name("f");

    // Create data block
    P2::DataBlock::Ptr db = createUniqueDataBlock(8);
    ASSERT_not_null(db);
    db->comment("db");
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb->dataBlockExists(db));
    ASSERT_always_require(!f->dataBlockExists(db));
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);
    
    // Make the data block owned by the basic block and function
    partitioner->attachDataBlockToBasicBlock(db, bb);
    bowners.insert(bb);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(!f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, bowners);
    checkAum(partitioner, db, fowners);

    partitioner->attachDataBlockToFunction(db, f);
    fowners.insert(f);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, bowners);
    checkAum(partitioner, db, fowners);

    // Detaching one owner doesn't detach the data block or change any ownership
    partitioner->detachBasicBlock(bb);
    bowners.erase(bb);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, bowners);
    checkAum(partitioner, db, fowners);

    // But detaching both owners does detach the data block
    partitioner->detachFunction(f);
    fowners.erase(f);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);

    // Reattaching an owner reattaches the data block
    partitioner->attachBasicBlock(bb);
    bowners.insert(bb);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, bowners);
    checkAum(partitioner, db, fowners);

    // Reattaching the other owner doesn't do much
    partitioner->attachFunction(f);
    fowners.insert(f);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == db);
    checkAum(partitioner, db, bowners);
    checkAum(partitioner, db, fowners);

    // Detach the data block from basic block and function.
    partitioner->detachBasicBlock(bb);
    partitioner->detachFunction(f);
    bb->eraseDataBlock(db);
    f->eraseDataBlock(db);
    partitioner->attachBasicBlock(bb);
    partitioner->attachFunction(f);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db) == nullptr);
}

// Test that two data blocks with the same address and size are treated as a single data block.
static void
testAliases(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::Function::Ptr> owners;

    // Find any function
    ASSERT_always_require(partitioner->nFunctions() >= 1);
    P2::Function::Ptr f = partitioner->functions()[0];
    f->name("f");

    // Create two data blocks with the same address and size
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    ASSERT_always_not_null(db1);
    db1->comment("db1");
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db1));
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);

    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    ASSERT_always_not_null(db2);
    db2->comment("db2");
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db2));
    ASSERT_always_require(db1 != db2);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);

    // Make the first data block owned by the function
    partitioner->attachDataBlockToFunction(db1, f);
    owners.insert(f);
    ASSERT_always_require(f->dataBlockExists(db1));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Asking whether the second data block is owned by the function is the same as asking whether the first block is owned by
    // the function because the query is based on address and type (size).
    ASSERT_always_require(f->dataBlockExists(db1) == db1);
    ASSERT_always_require(f->dataBlockExists(db2) == db1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);

    // Make the second data block owned by the function. Since the second block is the same address and type (size) as the
    // first block, this doesn't do anything -- the function already owns an equivalent data block.
    P2::DataBlock::Ptr db2b = partitioner->attachDataBlockToFunction(db2, f);
    ASSERT_always_require(db2b == db1);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);

    // Remove the first data block using the equivalent second data block.
    partitioner->detachFunction(f);
    db2b = f->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);
    partitioner->attachFunction(f);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
}

// Test that two basic blocks having two different data blocks both with the same address and size can be attached to the
// CFG/AUM. When doing so, the second basic block's data blocks will be adjusted to point to the first basic block's data
// blocks so that the AUM has only one data block per unique (address, size) pair.
static void
testDelayedBasicBlock(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::BasicBlock::Ptr> owners;

    // Find any two basic blocks
    P2::BasicBlock::Ptr bb1, bb2;
    for (const P2::ControlFlowGraph::Vertex &vertex : partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            if (!bb1) {
                bb1 = vertex.value().bblock();
            } else {
                bb2 = vertex.value().bblock();
                break;
            }
        }
    }
    ASSERT_always_not_null(bb1);
    ASSERT_always_not_null(bb2);
    bb1->comment("bb1");
    bb2->comment("bb2");

    // Attach a data block to the first basic block
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    db1->comment("db1");
    partitioner->attachDataBlockToBasicBlock(db1, bb1);
    owners.insert(bb1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Attach a similar data block to the second basic block after detaching it.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db2->comment("db2");
    partitioner->detachBasicBlock(bb2);
    partitioner->attachDataBlockToBasicBlock(db2, bb2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1); // db2 is an alias for db1
    checkAum(partitioner, db1, owners);

    // Attach the second basic block to the CFG/AUM. Since this basic block has a data block (db2) with the same address and
    // size as an existing data block, this basic block is rewritten to use the existing data block.
    partitioner->attachBasicBlock(bb2);
    owners.insert(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb2->dataBlockExists(db2) == db1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);

    // Remove the data block from both basic blocks
    partitioner->detachBasicBlock(bb1);
    P2::DataBlock::Ptr db1b = bb1->eraseDataBlock(db1);
    ASSERT_always_require(db1b == db1);
    partitioner->attachBasicBlock(bb1);

    partitioner->detachBasicBlock(bb2);
    P2::DataBlock::Ptr db2b = bb2->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);
    partitioner->attachBasicBlock(bb2);

    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
}

// Test that two functions having two different data blocks both with the same address and size can be attached to the
// CFG/AUM. When doing so, the second functions's data blocks will be adjusted to point to the first function's data blocks so
// that the AUM has only one data block per unique (address, size) pair.
static void
testDelayedFunction(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::Function::Ptr> owners;

    // Find any two functions
    ASSERT_always_require(partitioner->nFunctions() >= 2);
    P2::Function::Ptr f1 = partitioner->functions()[0];
    P2::Function::Ptr f2 = partitioner->functions()[1];
    f1->name("f1");
    f2->name("f2");

    // Attach a data block to the first function
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    db1->comment("db1");
    partitioner->attachDataBlockToFunction(db1, f1);
    owners.insert(f1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Attach a similar data block to the second function after detaching it.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db2->comment("db2");
    partitioner->detachFunction(f2);
    partitioner->attachDataBlockToFunction(db2, f2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1); // db2 is an alias for db1
    checkAum(partitioner, db1, owners);

    // Attach the second function to the CFG/AUM. Since this function has a data block (db2) with the same address and size as
    // an existing data block, this function is rewritten to use the existing data block.
    partitioner->attachFunction(f2);
    owners.insert(f2);
    ASSERT_always_require(f1->dataBlockExists(db1) == db1);
    ASSERT_always_require(f2->dataBlockExists(db2) == db1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);

    // Remove the data block from both functions
    partitioner->detachFunction(f1);
    P2::DataBlock::Ptr db1b = f1->eraseDataBlock(db1);
    ASSERT_always_require(db1b == db1);
    partitioner->attachFunction(f1);

    partitioner->detachFunction(f2);
    P2::DataBlock::Ptr db2b = f2->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);
    partitioner->attachFunction(f2);

    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == nullptr);
}

// Test partial data block overlaps
static void
testPartialOverlaps(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::Function::Ptr> owners;
    ASSERT_always_require(partitioner->nFunctions() >= 1);

    P2::Function::Ptr f1 = partitioner->functions()[0];
    f1->name("f1");

    // Attach first data block to function
    P2::DataBlock::Ptr base = createUniqueDataBlock(8);
    base->comment("base");
    partitioner->attachDataBlockToFunction(base, f1);
    owners.insert(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    checkAum(partitioner, base, owners);

    // Second data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    P2::DataBlock::Ptr left1 = P2::DataBlock::instanceBytes(base->address()-3, 8);
    left1->comment("left1");
    partitioner->attachDataBlockToFunction(left1, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);

    // Third data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    P2::DataBlock::Ptr left2 = P2::DataBlock::instanceBytes(base->address()-3, 4);
    left2->comment("left2");
    partitioner->attachDataBlockToFunction(left2, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    
    // Fourth data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    P2::DataBlock::Ptr left3 = P2::DataBlock::instanceBytes(base->address()-3, 11);
    left3->comment("left3");
    partitioner->attachDataBlockToFunction(left3, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    
    // Fifth data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    P2::DataBlock::Ptr left4 = P2::DataBlock::instanceBytes(base->address()-3, 14);
    left4->comment("left4");
    partitioner->attachDataBlockToFunction(left4, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    
    // Sixth data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    //   mid1  = ....|==|......
    P2::DataBlock::Ptr mid1 = P2::DataBlock::instanceBytes(base->address()+1, 4);
    mid1->comment("mid1");
    partitioner->attachDataBlockToFunction(mid1, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid1) == mid1);
    ASSERT_always_require(mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == mid1);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    checkAum(partitioner, mid1, owners);

    // Seventh data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    //   mid1  = ....|==|......
    //   mid2  = ....|=====|...
    P2::DataBlock::Ptr mid2 = P2::DataBlock::instanceBytes(base->address()+1, 7);
    mid2->comment("mid2");
    partitioner->attachDataBlockToFunction(mid2, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid1) == mid1);
    ASSERT_always_require(mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid2) == mid2);
    ASSERT_always_require(mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == mid1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == mid2);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    checkAum(partitioner, mid1, owners);
    checkAum(partitioner, mid2, owners);

    // Eigth data block:
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    //   mid1  = ....|==|......
    //   mid2  = ....|=====|...
    //   mid3  = ....|========|
    P2::DataBlock::Ptr mid3 = P2::DataBlock::instanceBytes(base->address()+1, 9);
    mid3->comment("mid3");
    partitioner->attachDataBlockToFunction(mid3, f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid1) == mid1);
    ASSERT_always_require(mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid2) == mid2);
    ASSERT_always_require(mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid3) == mid3);
    ASSERT_always_require(mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 1);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == mid1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == mid2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == mid3);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    checkAum(partitioner, mid1, owners);
    checkAum(partitioner, mid2, owners);
    checkAum(partitioner, mid3, owners);

    //---------------------
    // Now we test removals
    //---------------------
    
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    //   mid1  = ....|==|......
    //   mid2  = ....|=====|...
    //   mid3  = ....XXXXXXXXXX    <------ removed
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(mid3);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid1) == mid1);
    ASSERT_always_require(mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid2) == mid2);
    ASSERT_always_require(mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == mid1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == mid2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    checkAum(partitioner, mid1, owners);
    checkAum(partitioner, mid2, owners);
    
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    //   mid1  = ....|==|......
    //   mid2  = ....XXXXXXX...    <------ remove
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(mid2);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid1) == mid1);
    ASSERT_always_require(mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == mid1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    checkAum(partitioner, mid1, owners);
    
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = |============|
    //   mid1  = ....XXXX......    <------ remove
    //   mid2  = ....XXXXXXX...
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(mid1);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == left4);
    ASSERT_always_require(left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(!mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == left4);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    checkAum(partitioner, left4, owners);
    
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = |=========|...
    //   left4 = XXXXXXXXXXXXXX    <------ remove
    //   mid1  = ....XXXX......
    //   mid2  = ....XXXXXXX...
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(left4);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == left3);
    ASSERT_always_require(left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left4) == nullptr);
    ASSERT_always_require(!left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(!mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == left3);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    checkAum(partitioner, left3, owners);
    
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = |==|..........
    //   left3 = XXXXXXXXXXX...    <------ remove
    //   left4 = XXXXXXXXXXXXXX
    //   mid1  = ....XXXX......
    //   mid2  = ....XXXXXXX...
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(left3);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == left2);
    ASSERT_always_require(left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left3) == nullptr);
    ASSERT_always_require(!left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left4) == nullptr);
    ASSERT_always_require(!left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(!mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == left2);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    checkAum(partitioner, left2, owners);
    
    //   base  = ...|======|...
    //   left1 = |======|......
    //   left2 = XXXX..........    <------ remove
    //   left3 = XXXXXXXXXXX...
    //   left4 = XXXXXXXXXXXXXX
    //   mid1  = ....XXXX......
    //   mid2  = ....XXXXXXX...
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(left2);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == left1);
    ASSERT_always_require(left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left2) == nullptr);
    ASSERT_always_require(!left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left3) == nullptr);
    ASSERT_always_require(!left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left4) == nullptr);
    ASSERT_always_require(!left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(!mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == left1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    checkAum(partitioner, left1, owners);
    
    //   base  = ...|======|...
    //   left1 = XXXXXXXX......    <------ remove
    //   left2 = XXXX..........
    //   left3 = XXXXXXXXXXX...
    //   left4 = XXXXXXXXXXXXXX
    //   mid1  = ....XXXX......
    //   mid2  = ....XXXXXXX...
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(left1);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == base);
    ASSERT_always_require(base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 1);
    ASSERT_always_require(f1->dataBlockExists(left1) == nullptr);
    ASSERT_always_require(!left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left2) == nullptr);
    ASSERT_always_require(!left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left3) == nullptr);
    ASSERT_always_require(!left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left4) == nullptr);
    ASSERT_always_require(!left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(!mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == base);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
    checkAum(partitioner, base, owners);
    
    //   base  = ...XXXXXXXX...    <------ remove
    //   left1 = XXXXXXXX......
    //   left2 = XXXX..........
    //   left3 = XXXXXXXXXXX...
    //   left4 = XXXXXXXXXXXXXX
    //   mid1  = ....XXXX......
    //   mid2  = ....XXXXXXX...
    //   mid3  = ....XXXXXXXXXX
    partitioner->detachFunction(f1);
    f1->eraseDataBlock(base);
    partitioner->attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(base) == nullptr);
    ASSERT_always_require(!base->isFrozen());
    ASSERT_always_require(base->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left1) == nullptr);
    ASSERT_always_require(!left1->isFrozen());
    ASSERT_always_require(left1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left2) == nullptr);
    ASSERT_always_require(!left2->isFrozen());
    ASSERT_always_require(left2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left3) == nullptr);
    ASSERT_always_require(!left3->isFrozen());
    ASSERT_always_require(left3->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(left4) == nullptr);
    ASSERT_always_require(!left4->isFrozen());
    ASSERT_always_require(left4->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(!mid1->isFrozen());
    ASSERT_always_require(mid1->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(!mid2->isFrozen());
    ASSERT_always_require(mid2->nAttachedOwners() == 0);
    ASSERT_always_require(f1->dataBlockExists(mid3) == nullptr);
    ASSERT_always_require(!mid3->isFrozen());
    ASSERT_always_require(mid3->nAttachedOwners() == 0);
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(base) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left3) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(left4) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid1) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid2) == nullptr);
    ASSERT_always_require(partitioner->aum().dataBlockExists(mid3) == nullptr);
}

// Bug reported. AUM doesn't have correct ownrship count.
static void
testJiraRose2084(const P2::Partitioner::Ptr &partitioner) {
    std::set<P2::Function::Ptr> owners;
    ASSERT_always_require(partitioner->nFunctions() >= 3);

    P2::Function::Ptr functionA = partitioner->functions()[0];
    functionA->name("functionA");
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    db1->comment("db1");

    // Step 1: create 1st data block and attach to function A
    P2::DataBlock::Ptr dbResult = partitioner->attachDataBlockToFunction(db1, functionA);
    owners.insert(functionA);
    ASSERT_always_require(dbResult == db1);
    ASSERT_always_require(functionA->dataBlockExists(db1));
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(db1->isFrozen());
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    checkAum(partitioner, db1, owners);

    // Step 2: detach and reattach data block via Engine::attachBlocksToFunctions
    // (shouldn't affect test results)

    // Step 3: 2nd data block (same key) attached to function B
    P2::Function::Ptr functionB = partitioner->functions()[1];
    functionB->name("functionB");
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db2->comment("db2");
    dbResult = partitioner->attachDataBlockToFunction(db2, functionB);
    owners.insert(functionB);
    ASSERT_always_require(dbResult == db1);
    ASSERT_always_require(functionA->dataBlockExists(db1) == db1);
    ASSERT_always_require(functionB->dataBlockExists(db1) == db1);
    ASSERT_always_require(functionA->dataBlockExists(db2) == db1);
    ASSERT_always_require(functionB->dataBlockExists(db2) == db1);
    ASSERT_always_require(db1->nAttachedOwners() == 2);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    checkAum(partitioner, db1, owners);
    
    // Step 4: 3rd data (same key) block attached to function C
    P2::Function::Ptr functionC = partitioner->functions()[2];
    functionC->name("functionC");
    P2::DataBlock::Ptr db3 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    db3->comment("db3");
    dbResult = partitioner->attachDataBlockToFunction(db3, functionC);
    owners.insert(functionC);
    ASSERT_always_require(dbResult == db1);
    ASSERT_always_require(functionA->dataBlockExists(db1) == db1);
    ASSERT_always_require(functionB->dataBlockExists(db1) == db1);
    ASSERT_always_require(functionC->dataBlockExists(db1) == db1);
    ASSERT_always_require(functionA->dataBlockExists(db2) == db1);
    ASSERT_always_require(functionB->dataBlockExists(db2) == db1);
    ASSERT_always_require(functionC->dataBlockExists(db2) == db1);
    ASSERT_always_require(functionA->dataBlockExists(db3) == db1);
    ASSERT_always_require(functionB->dataBlockExists(db3) == db1);
    ASSERT_always_require(functionC->dataBlockExists(db3) == db1);
    ASSERT_always_require(db1->nAttachedOwners() == 3);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(db3->nAttachedOwners() == 0);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(!db3->isFrozen());
    partitioner->checkConsistency();
    ASSERT_always_require(partitioner->aum().dataBlockExists(db1) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db2) == db1);
    ASSERT_always_require(partitioner->aum().dataBlockExists(db3) == db1);
    checkAum(partitioner, db1, owners);
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    ASSERT_always_require(argc > 1);
    std::vector<std::string> names(argv+1, argv+argc);
    P2::Engine *engine = P2::Engine::instance();
    P2::Partitioner::Ptr partitioner = engine->partition(names);

    testNoOwner();
    testNoOwnerDuplicate();
    testDetachedBasicBlock(partitioner);
    testDetachedFunction(partitioner);
    testAttachedBasicBlock(partitioner);
    testAttachedFunction(partitioner);
    testTwoBasicBlocks(partitioner);
    testTwoFunctions(partitioner);
    testBasicBlockFunction(partitioner);
    testAliases(partitioner);
    testDelayedBasicBlock(partitioner);
    testDelayedFunction(partitioner);
    testPartialOverlaps(partitioner);
    testJiraRose2084(partitioner);

    delete engine;
}
