// Tests that DataBlock ownership rules work correctly
#include <rose.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Exception.h>
#include <Partitioner2/Partitioner.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static void
testNoOwner() {
    P2::Partitioner p;
    P2::DataBlock::Ptr db = P2::DataBlock::instanceBytes(0, 10);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());

    size_t n = p.nDataBlocks();
    p.attachDataBlock(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(p.nDataBlocks() == n+1);

    p.detachDataBlock(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(p.nDataBlocks() == n);
}

static void
testBasicBlockOwner(P2::Partitioner &p) {
    // Find any old basic block
    P2::BasicBlock::Ptr bb;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, p.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            bb = vertex.value().bblock();
            break;
        }
    }
    ASSERT_not_null(bb);

    // Create data block
    P2::DataBlock::Ptr db = P2::DataBlock::instanceBytes(10, 10);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb->dataBlockExists(db));

    // Attach data block to an attached basic block
    size_t n1 = p.nDataBlocks();
    size_t n2 = bb->nDataBlocks();
    p.attachDataBlockToBasicBlock(db, bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(bb->dataAddresses().contains(db->address()));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);

    // It's not legal to detach the data block because it has attached owners
    try {
        p.detachDataBlock(db);
        ASSERT_not_reachable("should have failed");
    } catch (const P2::DataBlockError&) {
    }

    // Detaching the basic block detaches the data block without changing ownership
    p.detachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(bb->dataAddresses().contains(db->address()));
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(p.nDataBlocks() == n1);

    // Reattaching the basic block reattaches the data block
    p.attachBasicBlock(bb);
    ASSERT_always_require(bb->nDataBlocks() == n2 + 1);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(bb->dataAddresses().contains(db->address()));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    ASSERT_always_require(p.nDataBlocks() == n1 + 1);
}

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
    ASSERT_not_null(bb1);
    ASSERT_not_null(bb2);

    // Create data block
    P2::DataBlock::Ptr db = P2::DataBlock::instanceBytes(20, 10);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb1->dataBlockExists(db));
    ASSERT_always_require(!bb2->dataBlockExists(db));
    
    // Make the data block owned by both basic blocks
    p.attachDataBlockToBasicBlock(db, bb1);
    ASSERT_always_require(bb1->dataBlockExists(db));
    ASSERT_always_require(!bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.attachDataBlockToBasicBlock(db, bb2);
    ASSERT_always_require(bb1->dataBlockExists(db));
    ASSERT_always_require(bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);

    // Detaching only one basic block doesn't detach the data block or change any ownership
    p.detachBasicBlock(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db));
    ASSERT_always_require(bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // But detaching both basic blocks does detach the data block
    p.detachBasicBlock(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db));
    ASSERT_always_require(bb2->dataBlockExists(db));
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);

    // Reattaching a basic block reattaches the data block
    p.attachBasicBlock(bb1);
    ASSERT_always_require(bb1->dataBlockExists(db));
    ASSERT_always_require(bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // Reattaching the other basic block doesn't do much
    p.attachBasicBlock(bb2);
    ASSERT_always_require(bb1->dataBlockExists(db));
    ASSERT_always_require(bb2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
}

static void
testFunctionOwner(P2::Partitioner &p) {
    // Find any function
    ASSERT_always_require(p.nFunctions() >= 1);
    P2::Function::Ptr f = p.functions()[0];

    // Create data block
    P2::DataBlock::Ptr db = P2::DataBlock::instanceBytes(20, 10);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db));

    // Make the data block owned by the function
    p.attachDataBlockToFunction(db, f);
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // Detaching the function detaches the data block
    p.detachFunction(f);
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);

    // Reattaching the function reattaches the data block
    p.attachFunction(f);
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
}

static void
testTwoFunctions(P2::Partitioner &p) {
    // Find any two functions
    ASSERT_always_require(p.nFunctions() >= 2);
    P2::Function::Ptr f1 = p.functions()[0];
    P2::Function::Ptr f2 = p.functions()[1];

    // Create data block
    P2::DataBlock::Ptr db = P2::DataBlock::instanceBytes(30, 10);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!f1->dataBlockExists(db));
    ASSERT_always_require(!f2->dataBlockExists(db));
    
    // Make the data block owned by both functions
    p.attachDataBlockToFunction(db, f1);
    ASSERT_always_require(f1->dataBlockExists(db));
    ASSERT_always_require(!f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.attachDataBlockToFunction(db, f2);
    ASSERT_always_require(f1->dataBlockExists(db));
    ASSERT_always_require(f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);

    // Detaching only one function doesn't detach the data block or change any ownership
    p.detachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(db));
    ASSERT_always_require(f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // But detaching both functions does detach the data block
    p.detachFunction(f2);
    ASSERT_always_require(f1->dataBlockExists(db));
    ASSERT_always_require(f2->dataBlockExists(db));
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);

    // Reattaching a function reattaches the data block
    p.attachFunction(f1);
    ASSERT_always_require(f1->dataBlockExists(db));
    ASSERT_always_require(f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // Reattaching the other function doesn't do much
    p.attachFunction(f2);
    ASSERT_always_require(f1->dataBlockExists(db));
    ASSERT_always_require(f2->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
}

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
    P2::DataBlock::Ptr db = P2::DataBlock::instanceBytes(40, 10);
    ASSERT_not_null(db);
    ASSERT_always_require(db->nAttachedOwners() == 0);
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(!bb->dataBlockExists(db));
    ASSERT_always_require(!f->dataBlockExists(db));
    
    // Make the data block owned by the basic block and function
    p.attachDataBlockToBasicBlock(db, bb);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(!f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);
    p.attachDataBlockToFunction(db, f);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);

    // Detaching one owner doesn't detach the data block or change any ownership
    p.detachBasicBlock(bb);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // But detaching both owners does detach the data block
    p.detachFunction(f);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(!db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 0);

    // Reattaching an owner reattaches the data block
    p.attachBasicBlock(bb);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 1);

    // Reattaching the other owner doesn't do much
    p.attachFunction(f);
    ASSERT_always_require(bb->dataBlockExists(db));
    ASSERT_always_require(f->dataBlockExists(db));
    ASSERT_always_require(db->isFrozen());
    ASSERT_always_require(db->nAttachedOwners() == 2);
}

void
testOverlaps(P2::Partitioner &p) {
    // Find any function
    ASSERT_always_require(p.nFunctions() >= 1);
    P2::Function::Ptr f = p.functions()[0];

    // Create two data blocks with the same address and size
    P2::DataBlock::Ptr db1 = P2::DataBlock::instanceBytes(50, 10);
    ASSERT_not_null(db1);
    ASSERT_always_require(db1->nAttachedOwners() == 0);
    ASSERT_always_require(!db1->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db1));
    P2::DataBlock::Ptr db2 = P2::DataBlock::instanceBytes(50, 10);
    ASSERT_not_null(db2);
    ASSERT_always_require(db2->nAttachedOwners() == 0);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(!f->dataBlockExists(db2));
    ASSERT_always_require(db1 != db2);
    
    // Make the first data block owned by the function
    p.attachDataBlockToFunction(db1, f);
    ASSERT_always_require(f->dataBlockExists(db1));
    ASSERT_always_require(db1->isFrozen());
    ASSERT_always_require(db1->nAttachedOwners() == 1);

    // Asking whether the second data block is owned by the function is the same as asking whether the first block is owned by
    // the function because the query is based on address and type (size).
    P2::DataBlock::Ptr tmp = f->dataBlockExists(db2);
    ASSERT_require(tmp == db1);

    // Make the second data block owned by the function. Since the second block is the same address and type (size) as the
    // first block, this doesn't do anything -- the function already owns an equivalent data block.
    tmp = p.attachDataBlockToFunction(db2, f);
    ASSERT_always_require(tmp == db1);
    ASSERT_always_require(!db2->isFrozen());
    ASSERT_always_require(db2->nAttachedOwners() == 0);
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    ASSERT_always_require(argc > 1);
    std::vector<std::string> names(argv+1, argv+argc);
    P2::Partitioner partitioner = P2::Engine().partition(names);

    testNoOwner();
    testBasicBlockOwner(partitioner);
    testTwoBasicBlocks(partitioner);
    testFunctionOwner(partitioner);
    testTwoFunctions(partitioner);
    testBasicBlockFunction(partitioner);
    testOverlaps(partitioner);
}
