// Tests that DataBlock ownership rules work correctly
#include <rose.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Exception.h>
#include <Partitioner2/Partitioner.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

// Data blocks are identified by their extent (starting address and size). This function returns a new
// unique data block each time it's called.
static P2::DataBlock::Ptr
createUniqueDataBlock(size_t nBytes) {
    static rose_addr_t nextVa = 0;
    rose_addr_t va = nextVa;
    nextVa += nBytes;
    return P2::DataBlock::instanceBytes(va, nBytes);
}

// Attach a naked data block to the partitioner without the data block being owned by any basic block or function.
static void
testNoOwner() {
    P2::Partitioner p;
    P2::DataBlock::Ptr db = createUniqueDataBlock(8);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    p.checkConsistency();

    size_t n = p.nDataBlocks();
    p.attachDataBlock(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(p.nDataBlocks() == n+1);
    p.checkConsistency();

    p.detachDataBlock(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(p.nDataBlocks() == n);
    p.checkConsistency();
}

// Attach two data blocks having the same address and size to the partitioner without any owning basic block or function. The
// partitioner should only see one of the two blocks.
static void
testNoOwnerDuplicate() {
    // Create and attach the first data block
    P2::Partitioner p;
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    p.attachDataBlock(db1);
    p.checkConsistency();

    // Create a second data block having the same identification.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    ASSERT_always_require(db2 != db1);
    ASSERT_always_require(db2->address() == db1->address());
    ASSERT_always_require(db2->size() == db1->size());
    p.checkConsistency();

    // Attaching the second data block to the AUM is a no-op, returning the first block instead.
    P2::DataBlock::Ptr found = p.attachDataBlock(db2);
    ASSERT_always_not_null(found);
    ASSERT_always_require(found == db1);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(p.nDataBlocks() == 1);
    p.checkConsistency();
}

// Attach and detach data blocks to a detached basic block, including attempting to attach a data block that has the same
// address and size as an existing data block.
static void
testDetachedBasicBlock(P2::Partitioner &p) {
    // Find any old basic block and detach it for this test
    P2::BasicBlock::Ptr bb;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, p.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_always_not_null(bb);
    p.detachBasicBlock(bb);
    ASSERT_always_require(!bb->isFrozen());

    size_t n1 = p.nDataBlocks();
    size_t n2 = bb->nDataBlocks();

    // Attach a first data block to the detached basic block. The data block should not be part of the AUM since the basic
    // block is not attached to the CFG/AUM.
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    P2::DataBlock::Ptr db1b = p.attachDataBlockToBasicBlock(db1, bb);
    ASSERT_always_require(db1b == db1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Attach a second data block to the detached basic block.
    P2::DataBlock::Ptr db2 = createUniqueDataBlock(8);
    P2::DataBlock::Ptr db2b = p.attachDataBlockToBasicBlock(db2, bb);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(bb->nDataBlocks() == n2 + 2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Attach a third data block having the same address and size as the first block. This should be a no-op.
    P2::DataBlock::Ptr db3 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    P2::DataBlock::Ptr db3b = p.attachDataBlockToBasicBlock(db3, bb);
    ASSERT_always_require(db3b == db1);
    ASSERT_always_require(!db3->isFrozen());
    ASSERT_always_require(bb->nDataBlocks() == n2 + 2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Detach the data blocks from the basic block.
    db3b = bb->eraseDataBlock(db3);
    ASSERT_require(db3b == db1);                        // since db3 isn't owned by the basic block
    db1b = bb->eraseDataBlock(db1);
    ASSERT_always_require(NULL == db1b);                // already erased
    db2b = bb->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(bb->nDataBlocks() == n2);
    p.checkConsistency();

    // Re-attach the basic block
    p.attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();
}

// Attach and detach data blocks to a detached function, including attempting to attach a data block that has the same address
// and size as an existing data block.
static void
testDetachedFunction(P2::Partitioner &p) {
    // Find any function and detach it for this test
    ASSERT_always_require(p.nFunctions() >= 1);
    P2::Function::Ptr f = p.functions()[0];
    ASSERT_always_not_null(f);
    p.detachFunction(f);
    ASSERT_always_require(!f->isFrozen());
    
    size_t n1 = p.nDataBlocks();
    size_t n2 = f->nDataBlocks();

    // Attach a first data block to the detached function. The data block should not be part of the AUM since the function
    // is not attached to the CFG/AUM.
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    P2::DataBlock::Ptr db1b = p.attachDataBlockToFunction(db1, f);
    ASSERT_always_require(db1b == db1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Attach a second data block to the detached function.
    P2::DataBlock::Ptr db2 = createUniqueDataBlock(8);
    P2::DataBlock::Ptr db2b = p.attachDataBlockToFunction(db2, f);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(f->nDataBlocks() == n2 + 2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Attach a third data block having the same address and size as the first block. This should be a no-op.
    P2::DataBlock::Ptr db3 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    P2::DataBlock::Ptr db3b = p.attachDataBlockToFunction(db3, f);
    ASSERT_always_require(db3b == db1);
    ASSERT_always_require(!db3->isFrozen());
    ASSERT_always_require(f->nDataBlocks() == n2 + 2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Detach the data blocks from the function
    db3b = f->eraseDataBlock(db3);
    ASSERT_require(db3b == db1);                        // since db3 isn't owned by the basic block
    db1b = f->eraseDataBlock(db1);
    ASSERT_always_require(NULL == db1b);                // already erased
    db2b = f->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db2);
    ASSERT_always_require(f->nDataBlocks() == n2);
    p.checkConsistency();

    // Re-attach the function
    p.attachFunction(f);
    ASSERT_always_require(f->nDataBlocks() == n2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();
}

// Attach data blocks to a basic block that's attached to the CFG/AUM.
static void
testAttachedBasicBlock(P2::Partitioner &p) {
    // Find any old basic block
    P2::BasicBlock::Ptr bb;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, p.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_always_not_null(bb);

    // Create data block
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    ASSERT_not_null(db1);
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!bb->dataBlockExists(db1));
    p.checkConsistency();

    // Attach data block to an attached basic block
    size_t n1 = p.nDataBlocks();
    size_t n2 = bb->nDataBlocks();
    p.attachDataBlockToBasicBlock(db1, bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
    p.checkConsistency();

    // It's not legal to detach the data block because it has attached owners
    try {
        p.detachDataBlock(db1);
        ASSERT_not_reachable("should have failed");
    } catch (const P2::DataBlockError&) {
    }
    p.checkConsistency();

    // Detaching the basic block detaches the data block without changing ownership
    p.detachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Reattaching the basic block reattaches the data block
    p.attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
    p.checkConsistency();

    // Attaching a second data block having the same address and size as the first is a no-op.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    P2::DataBlock::Ptr db2b = p.attachDataBlockToBasicBlock(db2, bb);
    ASSERT_always_require(db2b == db1);                 // db1 is the canonical block
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb->dataBlockExists(db2) == db1);
    ASSERT_always_require(bb->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
    p.checkConsistency();

    // Detach first basic block using second basic block as the alias.
    p.detachBasicBlock(bb);
    db2b = bb->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);                 // actually removed db1
    ASSERT_always_require(bb->nDataBlocks() == n2);
    p.checkConsistency();

    p.attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    p.checkConsistency();
}

// Attach data blocks to a function that's attached to the CFG/AUM.
static void
testAttachedFunction(P2::Partitioner &p) {
    // Find any function
    ASSERT_always_require(p.nFunctions() >= 1);
    P2::Function::Ptr f = p.functions()[0];
    ASSERT_always_not_null(f);

    // Create data block
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    ASSERT_not_null(db1);
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db1));
    p.checkConsistency();

    // Attach data block to an attached function
    size_t n1 = p.nDataBlocks();
    size_t n2 = f->nDataBlocks();
    p.attachDataBlockToFunction(db1, f);
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) = db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
    p.checkConsistency();

    // It's not legal to detach the data block because it has attached owners
    try {
        p.detachDataBlock(db1);
        ASSERT_not_reachable("should have failed");
    } catch (const P2::DataBlockError&) {
    }
    p.checkConsistency();

    // Detaching the function detaches the data block without changing ownership
    p.detachFunction(f);
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) = db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(p.nDataBlocks() == n1);
    p.checkConsistency();

    // Reattaching the function reattaches the data block
    p.attachFunction(f);
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) == db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
    p.checkConsistency();

    // Attaching a second data block having the same address and size as the first is a no-op.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    P2::DataBlock::Ptr db2b = p.attachDataBlockToFunction(db2, f);
    ASSERT_always_require(db2b == db1);                 // db1 is the canonical block
    ASSERT_always_require(f->nDataBlocks() == n2 + 1);
    ASSERT_always_require(f->dataBlockExists(db1) == db1);
    ASSERT_always_require(f->dataBlockExists(db2) == db1);
    ASSERT_always_require(f->dataAddresses().contains(db1->address()));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
    p.checkConsistency();

    // Detach first basic block using second basic block as the alias.
    p.detachFunction(f);
    db2b = f->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);                 // actually removed db1
    ASSERT_always_require(f->nDataBlocks() == n2);
    p.checkConsistency();

    p.attachFunction(f);
    ASSERT_always_require(f->nDataBlocks() == n2);
    ASSERT_always_require(p.nDataBlocks() == n1);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    p.checkConsistency();
}

// Attach same data block to two different basic blocks
static void
testTwoBasicBlocks(P2::Partitioner &p) {
    // Find any two basic block
    P2::BasicBlock::Ptr bb1, bb2;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, p.cfg().vertices()) {
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

    // Create data block
    P2::DataBlock::Ptr db= createUniqueDataBlock(8);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb1->dataBlockExists(db));
    ASSERT_always_require(!bb2->dataBlockExists(db));
    p.checkConsistency();
    
    // Make the data block owned by both basic blocks
    p.attachDataBlockToBasicBlock(db, bb1);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(!bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();
    p.attachDataBlockToBasicBlock(db, bb2);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    p.checkConsistency();

    // Detaching only one basic block doesn't detach the data block or change any ownership
    p.detachBasicBlock(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();

    // But detaching both basic blocks does detach the data block
    p.detachBasicBlock(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    p.checkConsistency();

    // Reattaching a basic block reattaches the data block
    p.attachBasicBlock(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();

    // Reattaching the other basic block doesn't do much
    p.attachBasicBlock(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db) == db);
    ASSERT_always_require(bb2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    p.checkConsistency();

    // Detach the data block from both basic blocks.
    p.detachBasicBlock(bb1);
    p.detachBasicBlock(bb2);
    bb1->eraseDataBlock(db);
    bb2->eraseDataBlock(db);
    p.attachBasicBlock(bb1);
    p.attachBasicBlock(bb2);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    p.checkConsistency();
}

// Attach same data block to two functions.
static void
testTwoFunctions(P2::Partitioner &p) {
    // Find any two functions
    ASSERT_always_require(p.nFunctions() >= 2);
    P2::Function::Ptr f1 = p.functions()[0];
    P2::Function::Ptr f2 = p.functions()[1];

    // Create data block
    P2::DataBlock::Ptr db = createUniqueDataBlock(8);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!f1->dataBlockExists(db));
    ASSERT_always_require(!f2->dataBlockExists(db));
    p.checkConsistency();
    
    // Make the data block owned by both functions
    p.attachDataBlockToFunction(db, f1);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(!f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();
    p.attachDataBlockToFunction(db, f2);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    p.checkConsistency();

    // Detaching only one function doesn't detach the data block or change any ownership
    p.detachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();

    // But detaching both functions does detach the data block
    p.detachFunction(f2);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    p.checkConsistency();

    // Reattaching a function reattaches the data block
    p.attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();

    // Reattaching the other function doesn't do much
    p.attachFunction(f2);
    ASSERT_always_require(f1->dataBlockExists(db) == db);
    ASSERT_always_require(f2->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    p.checkConsistency();

    // Detach the data block from both functions
    p.detachFunction(f1);
    p.detachFunction(f2);
    f1->eraseDataBlock(db);
    f2->eraseDataBlock(db);
    p.attachFunction(f1);
    p.attachFunction(f2);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    p.checkConsistency();
}

// Attach same data block to both a basic block and a function
static void
testBasicBlockFunction(P2::Partitioner &p) {
    // Find any basic block and function
    P2::BasicBlock::Ptr bb;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, p.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_not_null(bb);
    ASSERT_always_require(p.nFunctions() >= 1);
    P2::Function::Ptr f = p.functions()[0];

    // Create data block
    P2::DataBlock::Ptr db = createUniqueDataBlock(8);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb->dataBlockExists(db));
    ASSERT_always_require(!f->dataBlockExists(db));
    p.checkConsistency();
    
    // Make the data block owned by the basic block and function
    p.attachDataBlockToBasicBlock(db, bb);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(!f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();
    p.attachDataBlockToFunction(db, f);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    p.checkConsistency();

    // Detaching one owner doesn't detach the data block or change any ownership
    p.detachBasicBlock(bb);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();

    // But detaching both owners does detach the data block
    p.detachFunction(f);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    p.checkConsistency();

    // Reattaching an owner reattaches the data block
    p.attachBasicBlock(bb);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.checkConsistency();

    // Reattaching the other owner doesn't do much
    p.attachFunction(f);
    ASSERT_always_require(bb->dataBlockExists(db) == db);
    ASSERT_always_require(f->dataBlockExists(db) == db);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
    p.checkConsistency();

    // Detach the data block from basic block and function.
    p.detachBasicBlock(bb);
    p.detachFunction(f);
    bb->eraseDataBlock(db);
    f->eraseDataBlock(db);
    p.attachBasicBlock(bb);
    p.attachFunction(f);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    p.checkConsistency();
}

// Test that two data blocks with the same address and size are treated as a single data block.
static void
testAliases(P2::Partitioner &p) {
    // Find any function
    ASSERT_always_require(p.nFunctions() >= 1);
    P2::Function::Ptr f = p.functions()[0];

    // Create two data blocks with the same address and size
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    ASSERT_always_not_null(db1);
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db1));
    p.checkConsistency();

    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    ASSERT_always_not_null(db2);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db2));
    ASSERT_always_require(db1 != db2);
    p.checkConsistency();
    
    // Make the first data block owned by the function
    p.attachDataBlockToFunction(db1, f);
    ASSERT_always_require(f->dataBlockExists(db1));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    p.checkConsistency();

    // Asking whether the second data block is owned by the function is the same as asking whether the first block is owned by
    // the function because the query is based on address and type (size).
    ASSERT_always_require(f->dataBlockExists(db1) == db1);
    ASSERT_always_require(f->dataBlockExists(db2) == db1);
    p.checkConsistency();

    // Make the second data block owned by the function. Since the second block is the same address and type (size) as the
    // first block, this doesn't do anything -- the function already owns an equivalent data block.
    P2::DataBlock::Ptr db2b = p.attachDataBlockToFunction(db2, f);
    ASSERT_always_require(db2b == db1);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    p.checkConsistency();

    // Remove the first data block using the equivalent second data block.
    p.detachFunction(f);
    db2b = f->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);
    p.attachFunction(f);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    p.checkConsistency();
}

// Test that two basic blocks having two different data blocks both with the same address and size can be attached to the
// CFG/AUM. When doing so, the second basic block's data blocks will be adjusted to point to the first basic block's data
// blocks so that the AUM has only one data block per unique (address, size) pair.
static void
testDelayedBasicBlock(P2::Partitioner &p) {
    // Find any two basic blocks
    P2::BasicBlock::Ptr bb1, bb2;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, p.cfg().vertices()) {
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

    // Attach a data block to the first basic block
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    p.attachDataBlockToBasicBlock(db1, bb1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    p.checkConsistency();

    // Attach a similar data block to the second basic block after detaching it.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    p.detachBasicBlock(bb2);
    p.attachDataBlockToBasicBlock(db2, bb2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    p.checkConsistency();

    // Attach the second basic block to the CFG/AUM. Since this basic block has a data block (db2) with the same address and
    // size as an existing data block, this basic block is rewritten to use the existing data block.
    p.attachBasicBlock(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db1) == db1);
    ASSERT_always_require(bb2->dataBlockExists(db2) == db1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    p.checkConsistency();

    // Remove the data block from both basic blocks
    p.detachBasicBlock(bb1);
    P2::DataBlock::Ptr db1b = bb1->eraseDataBlock(db1);
    ASSERT_always_require(db1b == db1);
    p.attachBasicBlock(bb1);
    p.detachBasicBlock(bb2);
    P2::DataBlock::Ptr db2b = bb2->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);
    p.attachBasicBlock(bb2);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    p.checkConsistency();
}

// Test that two functions having two different data blocks both with the same address and size can be attached to the
// CFG/AUM. When doing so, the second functions's data blocks will be adjusted to point to the first function's data blocks so
// that the AUM has only one data block per unique (address, size) pair.
static void
testDelayedFunction(P2::Partitioner &p) {
    // Find any two functions
    ASSERT_always_require(p.nFunctions() >= 2);
    P2::Function::Ptr f1 = p.functions()[0];
    P2::Function::Ptr f2 = p.functions()[1];

    // Attach a data block to the first function
    P2::DataBlock::Ptr db1 = createUniqueDataBlock(8);
    p.attachDataBlockToFunction(db1, f1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);
    p.checkConsistency();

    // Attach a similar data block to the second function after detaching it.
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(db1->address(), db1->size());
    p.detachFunction(f2);
    p.attachDataBlockToFunction(db2, f2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    p.checkConsistency();

    // Attach the second function to the CFG/AUM. Since this function has a data block (db2) with the same address and size as
    // an existing data block, this function is rewritten to use the existing data block.
    p.attachFunction(f2);
    ASSERT_always_require(f1->dataBlockExists(db1) == db1);
    ASSERT_always_require(f2->dataBlockExists(db2) == db1);
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 2);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    p.checkConsistency();

    // Remove the data block from both functions
    p.detachFunction(f1);
    P2::DataBlock::Ptr db1b = f1->eraseDataBlock(db1);
    ASSERT_always_require(db1b == db1);
    p.attachFunction(f1);
    p.detachFunction(f2);
    P2::DataBlock::Ptr db2b = f2->eraseDataBlock(db2);
    ASSERT_always_require(db2b == db1);
    p.attachFunction(f2);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!db2->isFrozen());
    p.checkConsistency();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    ASSERT_always_require(argc > 1);
    std::vector<std::string> names(argv+1, argv+argc);
    P2::Partitioner partitioner = P2::Engine().partition(names);

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
}
