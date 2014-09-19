#include "sage3basic.h"
#include "BinaryLoader.h"
#include "DisassemblerM68k.h"
#include "DisassemblerX86.h"
#include <Partitioner2/Engine.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/ModulesX86.h>
#include <Partitioner2/Utility.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner-making functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner
Engine::createBarePartitioner(Disassembler *disassembler, const MemoryMap &map) {
    Partitioner p(disassembler, map);
    p.enableSymbolicSemantics();
    return p;
}

Partitioner
Engine::createGenericPartitioner(Disassembler *disassembler, const MemoryMap &map) {
    Partitioner p = createBarePartitioner(disassembler, map);
    p.functionPrologueMatchers().push_back(ModulesX86::MatchHotPatchPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchStandardPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchAbbreviatedPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesX86::MatchEnterPrologue::instance());
    p.functionPrologueMatchers().push_back(ModulesM68k::MatchLink::instance());
    p.basicBlockCallbacks().append(ModulesX86::FunctionReturnDetector::instance());
    return p;
}

Partitioner
Engine::createTunedPartitioner(Disassembler *disassembler, const MemoryMap &map) {
    if (dynamic_cast<DisassemblerM68k*>(disassembler)) {
        Partitioner p = createBarePartitioner(disassembler, map);
        p.functionPrologueMatchers().push_back(ModulesM68k::MatchLink::instance());
        return p;
    }
    
    if (dynamic_cast<DisassemblerX86*>(disassembler)) {
        Partitioner p = createBarePartitioner(disassembler, map);
        p.functionPrologueMatchers().push_back(ModulesX86::MatchHotPatchPrologue::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchStandardPrologue::instance());
        p.functionPrologueMatchers().push_back(ModulesX86::MatchEnterPrologue::instance());
        p.basicBlockCallbacks().append(ModulesX86::FunctionReturnDetector::instance());
        return p;
    }

    return createGenericPartitioner(disassembler, map);
}

Disassembler*
Engine::loadSpecimen(SgAsmInterpretation *interp, Disassembler *disassembler) {
    ASSERT_not_null(interp);
    // Load the specimen to map it into memory if necessary, but do not link (the user would have done that if they wanted it)
    if (NULL==interp->get_map()) {
        BinaryLoader *loader = BinaryLoader::lookup(interp);
        if (!loader)
            throw std::runtime_error("unable to find an appropriate loader for binary interpretation");
        loader = loader->clone();                       // in case we change any settings
        loader->remap(interp);                          // simulate loading without linking
        ASSERT_not_null2(interp->get_map(), "BinaryLoader failed to produce a memory map");
        delete loader;
    }

    // Obtain a disassembler to provide us with instructions and architecture information
    if (!disassembler) {
        disassembler = Disassembler::lookup(interp);
        if (!disassembler)
            throw std::runtime_error("unable to find an appropriate disassembler");
    }
    return disassembler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      High-level stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
Engine::partition(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);

    Disassembler *disassembler = loadSpecimen(interp);
    disassembler->set_progress_reporting(-1.0);         // turn it off

    MemoryMap map = *interp->get_map();
    Modules::deExecuteZeros(map, 256);

    Partitioner partitioner = createTunedPartitioner(disassembler, map);
    partition(partitioner, interp);

    SgAsmBlock *gblock = partitioner.buildGlobalBlockAst();
    delete disassembler;
    return gblock;
}

void
Engine::partition(Partitioner &partitioner, SgAsmInterpretation *interp) {
    makeContainerFunctions(partitioner, interp);
    discoverFunctions(partitioner);
    attachDeadCodeToFunctions(partitioner);
    attachPaddingToFunctions(partitioner);
    attachSurroundedDataToFunctions(partitioner);
    attachBlocksToFunctions(partitioner, true);         // to emit warnings about CFG problems
    postPartitionFixups(partitioner, interp);
}

void
Engine::discoverBasicBlocks(Partitioner &partitioner) {
    while (makeNextBasicBlock(partitioner)) /*void*/;
}

std::vector<Function::Ptr>
Engine::discoverFunctions(Partitioner &partitioner) {
    rose_addr_t nextPrologueVa = 0;                     // where to search for function prologues

    while (1) {
        // Find as many basic blocks as possible by recursively following the CFG as we build it.
        discoverBasicBlocks(partitioner);

        // No pending basic blocks, so look for a function prologue. This creates a pending basic block for the function's
        // entry block, so go back and look for more basic blocks again.
        if (Function::Ptr function = makeNextPrologueFunction(partitioner, nextPrologueVa)) {
            nextPrologueVa = function->address();       // avoid "+1" because it may overflow
            continue;
        }

        // Nothing more to do
        break;
    }

    // Try to attach basic blocks to functions and return the list of failures.
    makeCalledFunctions(partitioner);
    return attachBlocksToFunctions(partitioner);
}

void
Engine::postPartitionFixups(Partitioner &partitioner, SgAsmInterpretation *interp) {
    ModulesPe::nameImportThunks(partitioner, interp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Function-making functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make functions from ELF/PE containers
std::vector<Function::Ptr>
Engine::makeContainerFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;

    BOOST_FOREACH (const Function::Ptr &function, makeEntryFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeErrorHandlingFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeImportFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeExportFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    BOOST_FOREACH (const Function::Ptr &function, makeSymbolFunctions(partitioner, interp))
        insertUnique(retval, function, sortFunctionsByAddress);
    return retval;
}

// Make functions at specimen entry addresses
std::vector<Function::Ptr>
Engine::makeEntryFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            BOOST_FOREACH (const rose_rva_t &rva, fileHeader->get_entry_rvas()) {
                rose_addr_t va = rva.get_rva() + fileHeader->get_base_va();
                Function::Ptr function = Function::instance(va, SgAsmFunction::FUNC_ENTRY_POINT);
                insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
            }
        }
    }
    return retval;
}

// Make functions at error handling addresses
std::vector<Function::Ptr>
Engine::makeErrorHandlingFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, ModulesElf::findErrorHandlingFunctions(interp))
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    return retval;
}

// Make functions at import trampolines
std::vector<Function::Ptr>
Engine::makeImportFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;

    // Windows PE imports
    ModulesPe::rebaseImportAddressTables(partitioner, ModulesPe::getImportIndex(partitioner, interp));
    BOOST_FOREACH (const Function::Ptr &function, ModulesPe::findImportFunctions(partitioner, interp))
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    
    // ELF imports
    BOOST_FOREACH (const Function::Ptr &function, ModulesElf::findPltFunctions(partitioner, interp))
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);

    return retval;
}

// Make functions at export addresses
std::vector<Function::Ptr>
Engine::makeExportFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, ModulesPe::findExportFunctions(partitioner, interp))
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    return retval;
}

// Make functions that have symbols
std::vector<Function::Ptr>
Engine::makeSymbolFunctions(Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, Modules::findSymbolFunctions(partitioner, interp))
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    return retval;
}

// Make functions according to CFG
std::vector<Function::Ptr>
Engine::makeCalledFunctions(Partitioner &partitioner) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.discoverCalledFunctions())
        insertUnique(retval, partitioner.attachOrMergeFunction(function), sortFunctionsByAddress);
    return retval;
}

// Looks for a function prologue at or above the specified starting address and makes a function there.
Function::Ptr
Engine::makeNextPrologueFunction(Partitioner &partitioner, rose_addr_t startVa) {
    if (Function::Ptr function = partitioner.nextFunctionPrologue(startVa)) {
        partitioner.attachFunction(function);
        return function;
    }
    return Function::Ptr();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Basic-blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// make a new basic block for an arbitrary placeholder
BasicBlock::Ptr
Engine::makeNextBasicBlock(Partitioner &partitioner) {
    ControlFlowGraph::VertexNodeIterator worklist = partitioner.undiscoveredVertex();
    if (worklist->nInEdges() > 0) {
        ControlFlowGraph::VertexNodeIterator placeholder = worklist->inEdges().begin()->source();
        BasicBlock::Ptr bb = partitioner.discoverBasicBlock(placeholder);
        partitioner.attachBasicBlock(placeholder, bb);
        return bb;
    }
    return BasicBlock::Ptr();
}

// sophomoric attempt to assign basic blocks to functions.
std::vector<Function::Ptr>
Engine::attachBlocksToFunctions(Partitioner &partitioner, bool emitWarnings) {
    std::vector<Function::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        partitioner.detachFunction(function);           // must be detached in order to modify block ownership
        EdgeList inwardConflictEdges, outwardConflictEdges;
        size_t nFailures = partitioner.discoverFunctionBasicBlocks(function, &inwardConflictEdges, &outwardConflictEdges);
        if (nFailures > 0) {
            insertUnique(retval, function, sortFunctionsByAddress);
            if (mlog[WARN]) {
                mlog[WARN] <<"discovery for " <<partitioner.functionName(function)
                               <<" had " <<StringUtility::plural(inwardConflictEdges.size(), "inward conflicts")
                               <<" and " <<StringUtility::plural(outwardConflictEdges.size(), "outward conflicts") <<"\n";
                BOOST_FOREACH (const ControlFlowGraph::EdgeNodeIterator &edge, inwardConflictEdges) {
                    mlog[WARN] <<"  inward conflict " <<*edge
                               <<" from " <<partitioner.functionName(edge->source()->value().function()) <<"\n";
                }
                BOOST_FOREACH (const ControlFlowGraph::EdgeNodeIterator &edge, outwardConflictEdges) {
                    mlog[WARN] <<"  outward conflict " <<*edge
                               <<" to " <<partitioner.functionName(edge->target()->value().function()) <<"\n";
                }
            }
        }
        partitioner.attachFunction(function);           // reattach even if we didn't modify it due to failure
    }
    return retval;
}

std::set<rose_addr_t>
Engine::attachDeadCodeToFunction(Partitioner &partitioner, const Function::Ptr &function, size_t maxIterations) {
    ASSERT_not_null(function);
    std::set<rose_addr_t> retval;

    for (size_t i=0; i<maxIterations; ++i) {
        // Find ghost edges
        std::set<rose_addr_t> ghosts = partitioner.functionGhostSuccessors(function);
        if (ghosts.empty())
            break;

        // Insert placeholders for all ghost edge targets
        partitioner.detachFunction(function);           // so we can modify its basic block ownership list
        BOOST_FOREACH (rose_addr_t ghost, ghosts) {
            if (retval.find(ghost)==retval.end()) {
                partitioner.insertPlaceholder(ghost);   // ensure a basic block gets created here
                function->insertBasicBlock(ghost);      // the function will own this basic block
                retval.insert(ghost);
            }
        }
        
        // If we're about to do more iterations then we should recursively discover instructions for pending basic blocks. Once
        // we've done that we should traverse the function's CFG to see if some of those new basic blocks are reachable and
        // should also be attached to the function.
        if (i+1 < maxIterations) {
            while (makeNextBasicBlock(partitioner)) /*void*/;
            size_t nFailures = partitioner.discoverFunctionBasicBlocks(function, NULL, NULL);
            if (nFailures > 0)
                break;
        }
    }

    partitioner.attachFunction(function);               // no-op if still attached
    return retval;
}

// Finds dead code and adds it to the function to which it seems to belong.
std::set<rose_addr_t>
Engine::attachDeadCodeToFunctions(Partitioner &partitioner, size_t maxIterations) {
    std::set<rose_addr_t> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        std::set<rose_addr_t> deadVas = attachDeadCodeToFunction(partitioner, function, maxIterations);
        retval.insert(deadVas.begin(), deadVas.end());
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Data block functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataBlock::Ptr
Engine::attachPaddingToFunction(Partitioner &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(function);
    if (DataBlock::Ptr padding = partitioner.matchFunctionPadding(function)) {
        partitioner.attachFunctionDataBlock(function, padding);
        return padding;
    }
    return DataBlock::Ptr();
}

std::vector<DataBlock::Ptr>
Engine::attachPaddingToFunctions(Partitioner &partitioner) {
    std::vector<DataBlock::Ptr> retval;
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        if (DataBlock::Ptr padding = attachPaddingToFunction(partitioner, function))
            insertUnique(retval, padding, sortDataBlocks);
    }
    return retval;
}

std::vector<DataBlock::Ptr>
Engine::attachSurroundedDataToFunctions(Partitioner &partitioner) {
    // Find executable addresses that are not yet used in the CFG/AUM
    AddressIntervalSet executableSpace;
    BOOST_FOREACH (const MemoryMap::Node &node, partitioner.memoryMap().nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE) != 0)
            executableSpace.insert(node.key());
    }
    AddressIntervalSet unused = partitioner.aum().unusedExtent(executableSpace);

    // Iterate over the larged unused address intervals and find their surrounding functions
    std::vector<DataBlock::Ptr> retval;
    BOOST_FOREACH (const AddressInterval &interval, unused.intervals()) {
        if (interval.least()<=executableSpace.least() || interval.greatest()>=executableSpace.greatest())
            continue;
        typedef std::vector<Function::Ptr> Functions;
        Functions beforeFuncs = partitioner.functionsOverlapping(interval.least()-1);
        Functions afterFuncs = partitioner.functionsOverlapping(interval.greatest()+1);

        // What functions are in both sets?
        Functions enclosingFuncs(beforeFuncs.size());
        Functions::iterator final = std::set_intersection(beforeFuncs.begin(), beforeFuncs.end(),
                                                          afterFuncs.begin(), afterFuncs.end(), enclosingFuncs.begin());
        enclosingFuncs.resize(final-enclosingFuncs.begin());

        // Add the data block to all enclosing functions
        if (!enclosingFuncs.empty()) {
            BOOST_FOREACH (const Function::Ptr &function, enclosingFuncs) {
                DataBlock::Ptr dblock = partitioner.attachFunctionDataBlock(function, interval.least(), interval.size());
                insertUnique(retval, dblock, sortDataBlocks);
            }
        }
    }
    return retval;
}

} // namespace
} // namespace
} // namespace
