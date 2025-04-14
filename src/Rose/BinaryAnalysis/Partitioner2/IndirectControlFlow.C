#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/IndirectControlFlow.h>

#include <Rose/Affirm.h>
#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/X86.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmX86Instruction.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace P2DF = Rose::BinaryAnalysis::Partitioner2::DataFlow;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

using namespace Rose::BinaryAnalysis::InstructionSemantics;
using namespace Sawyer::Message::Common;

using DfCfg = P2DF::DfCfg;

using DfTransfer = P2DF::TransferFunction;
using DfNotConverging = Rose::BinaryAnalysis::DataFlow::NotConverging;

using Rose::StringUtility::addrToString;
using Rose::StringUtility::plural;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace IndirectControlFlow {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Rose::Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Partitioner2::IndirectControlFlow");
        mlog.comment("analyzing indirect control flow");
    }
}

static RegisterDescriptor
createPathRegister(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    const unsigned maj = partitioner->architecture()->registerDictionary()->firstUnusedMajor();
    const unsigned min = partitioner->architecture()->registerDictionary()->firstUnusedMinor(maj);
    return RegisterDescriptor(maj, min, 0, 1);
}

class DfMerge: public P2DF::MergeFunction {
    const RegisterDescriptor PATH, IP;
    const DfCfg &dfCfg;

public:
    using Super = P2DF::MergeFunction;

public:
    explicit DfMerge(const P2::Partitioner::ConstPtr &partitioner, const BS::RiscOperators::Ptr &ops, const DfCfg &dfCfg)
        : Super(ops),
          PATH(createPathRegister(partitioner)),
          IP(partitioner->architecture()->registerDictionary()->instructionPointerRegister()),
          dfCfg(dfCfg) {}

    bool operator()(size_t dstId, BS::State::Ptr &dst, size_t srcId, const BS::State::Ptr &src) const override {
        ASSERT_not_null(src);
        const auto dstVert = dfCfg.findVertex(dstId);
        ASSERT_require(dfCfg.isValidVertex(dstVert));
        const auto srcVert = dfCfg.findVertex(dstId);
        ASSERT_require(dfCfg.isValidVertex(srcVert));
        const BS::RiscOperators::Ptr ops = notnull(operators());

        // Symbolic expression for the expected successor(s)
        const auto srcIp = SymbolicSemantics::SValue::promote(src->peekRegister(IP, ops->undefined_(IP.nBits()), ops.get()))
                           ->get_expression();

        // Concrete address of the destination successor--the state we're merging into
        SymbolicExpression::Ptr dstAddr;
        if (const auto addr = dstVert->value().address()) {
            dstAddr = SymbolicExpression::makeIntegerConstant(srcIp->nBits(), *addr);
        }

        // Initial path constraints for the destination (before merging)
        SymbolicExpression::Ptr origConstraints;
        if (dst) {
            origConstraints = SymbolicSemantics::SValue::promote(dst->peekRegister(PATH, ops->boolean_(true), ops.get()))
                              ->get_expression();
        }

        bool retval = Super::operator()(dstId, dst, srcId, src);
        ASSERT_not_null(dst);

        // New constraints for the destination
        if (dstAddr) {
            auto newConstraints = SymbolicExpression::makeEq(srcIp, dstAddr);
            if (origConstraints)
                newConstraints = SymbolicExpression::makeOr(newConstraints, origConstraints);
            auto newConstraintsSVal = SymbolicSemantics::SValue::instance_symbolic(newConstraints);
            dst->writeRegister(PATH, newConstraintsSVal, ops.get());
        }

        return retval;
    }
};

// Test whether the specified function is one of the x86 get_pc_thunk functions. If so, return the register that gets the
// program counter for the machine instruction that occurs immediately after the call to this function.
static RegisterDescriptor
isGetPcThunk(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);

    // We could use semantics for this, but pattern matching is faster.
    const auto bbAddrs = function->basicBlockAddresses();
    if (bbAddrs.size() != 1)
        return {};
    const P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(*bbAddrs.begin());
    if (!bb || bb->nInstructions() != 2)
        return {};

    if (as<const Architecture::X86>(partitioner->architecture())) {
        // First instruction must be a "mov R1, ss:[esp]"
        auto insn1 = as<SgAsmX86Instruction>(bb->instructions()[0]);
        if (!insn1 || insn1->get_kind() != x86_mov || insn1->nOperands() != 2)
            return {};
        auto destReg = as<SgAsmDirectRegisterExpression>(insn1->operand(0));
        if (!destReg)
            return {};
        auto memRef = as<SgAsmMemoryReferenceExpression>(insn1->operand(1));
        if (!memRef)
            return {};
        auto segment = as<SgAsmDirectRegisterExpression>(memRef->get_segment());
        const RegisterDescriptor SS = partitioner->architecture()->registerDictionary()->find("ss");
        if (!segment || segment->get_descriptor() != SS)
            return {};
        auto memAddr = as<SgAsmDirectRegisterExpression>(memRef->get_address());
        const RegisterDescriptor SP = partitioner->architecture()->registerDictionary()->stackPointerRegister();
        if (!SP || !memAddr || memAddr->get_descriptor() != SP)
            return {};

        // Second instruction must be "ret"
        auto insn2 = as<SgAsmX86Instruction>(bb->instructions()[1]);
        if (!insn2 || insn2->get_kind() != x86_ret)
            return {};

        // Looks good.
        return destReg->get_descriptor();
    }

    // Other architectures aren't handled yet
    return {};
}

// Initialize the state with a reasonable concrete initial stack pointer.
static void
initializeStackPointer(const P2::Partitioner::ConstPtr &partitioner, const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(ops);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    const RegisterDescriptor SP = partitioner->architecture()->registerDictionary()->stackPointerRegister();
    ASSERT_require(SP);
    
    const size_t stackSize = 2 * 1024 * 1024;
    const size_t stackAlignment = 1024 * 1024;

    const AddressInterval search = AddressInterval::hull(0x80000000, 0xffffffff);
    const Sawyer::Optional<Address> stackBottom =
        partitioner->memoryMap()->findFreeSpace(stackSize, stackAlignment, search, Sawyer::Container::MATCH_BACKWARD);
    if (stackBottom) {
        const AddressInterval stackRegion = AddressInterval::baseSize(*stackBottom, stackSize);
        const Address stackPtr = *stackBottom + stackSize / 2;
        SAWYER_MESG(debug) <<"  initial state stack: region = " <<addrToString(stackRegion)
                           <<"; pointer = " <<addrToString(stackPtr) <<"\n";
        const BS::SValue::Ptr sp = ops->number_(SP.nBits(), stackPtr);
        ops->writeRegister(SP, sp);
    } else {
        SAWYER_MESG(debug) <<"initial state has abstract stack pointer\n";
    }
}

// Given a function, return the list of basic blocks that have indeterminate outgoing edges and return their starting addresses.
static Sawyer::Container::Set<Address>
findBlocksWithIndeterminateBranch(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);
    Sawyer::Container::Set<Address> retval;

    for (const Address bbAddr: function->basicBlockAddresses()) {
        const auto vertex = partitioner->findPlaceholder(bbAddr);
        if (partitioner->cfg().isValidVertex(vertex)) {
            for (const auto &edge: vertex->outEdges()) {
                if (edge.target()->value().type() == P2::V_INDETERMINATE) {
                    retval.insert(bbAddr);
                    break;
                }
            }
        }
    }
    return retval;
}

// Prune away parts of the dataflow graph that are not interesting. A vertex is not interesting if, by recursively following its
// outgoing edges, we cannot reach any of the `interestingBlocks`.
static void
pruneDfCfg(DfCfg &graph, const Sawyer::Container::Set<Address> &interestingBlocks) {
    if (interestingBlocks.isEmpty()) {
        graph.clear();
    } else {
        // Anything that can reach at least one interesting block is interesting.
        using namespace Sawyer::Container::Algorithm;
        using Traversal = DepthFirstForwardGraphTraversal<DfCfg>;
        std::vector<bool> isInteresting(graph.nVertices(), false);
        for (Traversal t(graph, graph.findVertex(0), ENTER_VERTEX | LEAVE_EDGE); t; ++t) {
            switch (t.event()) {
                case ENTER_VERTEX:
                    if (const auto addr = t.vertex()->value().address()) {
                        ASSERT_forbid(isInteresting[t.vertex()->id()]);
                        isInteresting[t.vertex()->id()] = interestingBlocks.exists(*addr);
                    }
                    break;
                case LEAVE_EDGE:
                    if (isInteresting[t.edge()->target()->id()])
                        isInteresting[t.edge()->source()->id()] = true;
                    break;
                default:
                    ASSERT_not_reachable("not handling this kind of traversal event");
            }
        }

        // Gather all the vertices that aren't interesting and which should be erased from the graph.
        std::vector<DfCfg::VertexIterator> toErase;
        for (const auto &vertex: graph.vertices()) {
            if (!isInteresting[vertex.id()])
                toErase.push_back(graph.findVertex(vertex.id()));
        }

        // Erase all vertices we don't need
        for (const auto &vertex: toErase)
            graph.eraseVertex(vertex);
    }
}

// The dataflow needs to be partly interprocedural.
class InterproceduralPredicate: public P2DF::InterproceduralPredicate {
    P2::Partitioner::ConstPtr partitioner_;

public:
    InterproceduralPredicate(const P2::Partitioner::ConstPtr &partitioner)
        : partitioner_(partitioner) {}

    bool operator()(const P2::ControlFlowGraph&, const P2::ControlFlowGraph::ConstEdgeIterator &edge, size_t /*depth*/) override {
        if (edge->value().type() == P2::E_FUNCTION_CALL) {
            if (const auto calleeAddr = edge->target()->value().optionalAddress()) {
                if (P2::Function::Ptr callee = partitioner_->functionExists(*calleeAddr)) {
                    return isGetPcThunk(partitioner_, callee);
                }
            }
        }
        return false;
    }
};

// matches "(add[u64] (sext 0x40, e1[u32]) c1)" and returns e1 and c1 if matched. If not matched, returns (nullptr, 0).
static std::pair<SymbolicExpression::Ptr, Address>
match1(const SymbolicExpression::Ptr &add) {
    using namespace SymbolicExpression;
    if (!add || !add->isOperator(OP_ADD) || add->nBits() != 64 || !add->isIntegerExpr())
        return {};

    const auto sext = add->child(0);
    if (!sext || !sext->isOperator(OP_SEXTEND) || sext->nBits() != 64)
        return {};

    const auto sixtyfour = sext->child(0);
    if (!sixtyfour || sixtyfour->toUnsigned().orElse(0) != 64)
        return {};

    const auto e1 = sext->child(1);
    if (!e1 || !e1->isIntegerVariable() || e1->nBits() != 32)
        return {};

    const auto c1 = add->child(1);
    if (!c1 || !c1->isIntegerConstant() || c1->nBits() != 64 || !c1->toUnsigned())
        return {};

    return {e1, *c1->toUnsigned()};
}

// matches (add[u32] e1[u32] c1[u32]) and returns e1 and c1 if matched. If not matched, returns (nullptr, 0).
static std::pair<SymbolicExpression::Ptr, Address>
match2(const SymbolicExpression::Ptr &expr) {
    using namespace SymbolicExpression;
    if (!expr || !expr->isOperator(OP_ADD) || expr->nBits() != 32 || !expr->isIntegerExpr())
        return std::make_pair(Ptr(), 0);

    const auto e1 = expr->child(0);
    if (!e1 || e1->nBits() != 32 || !e1->isIntegerExpr())
        return std::make_pair(Ptr(), 0);

    const auto c1 = expr->child(1);
    if (!c1 || c1->nBits() != 32 || !c1->toUnsigned())
        return std::make_pair(Ptr(), 0);
    
    return std::make_pair(e1, *c1->toUnsigned());
}

// matches e1[32] and if so, returns (e1, 0)
static std::pair<SymbolicExpression::Ptr, Address>
match3(const SymbolicExpression::Ptr &expr) {
    return std::make_pair(expr, 0);
}

// Looks at an instruction pointer and tries to figure out some information about the jump table. It returns the symbolic value
// read from the jump table, and a constant that needs to be added to each entry in the jump table in order to obtain a target
// address.
static std::pair<SymbolicExpression::Ptr, Address>
match(const SymbolicExpression::Ptr &expr) {
    const auto m1 = match1(expr);
    if (m1.first)
        return m1;

    const auto m2 = match2(expr);
    if (m2.first)
        return m2;

    return match3(expr);
}

// Find the memory address containing the specified value. The value can be more than one byte. The lowest address is returned.
static SymbolicExpression::Ptr
findAddressContaining(const BS::RiscOperators::Ptr &ops, const SymbolicExpression::Ptr &searchValue) {
    ASSERT_not_null(ops);
    ASSERT_not_null(searchValue);
    using namespace SymbolicExpression;

    if (auto mem = as<BS::MemoryCellState>(ops->currentState()->memoryState())) {
        // FIXME[Robb Matzke 2025-04-08]: need to handle big-endian also
        const auto searchByte = makeExtract(0, 8, searchValue);

        struct Visitor: public BS::MemoryCell::Visitor {
            Ptr searchValue, searchByte;
            Ptr foundAddr;

            Visitor(const Ptr &searchValue, const Ptr &searchByte)
                : searchValue(notnull(searchValue)), searchByte(notnull(searchByte)) {}

            void operator()(BS::MemoryCell::Ptr &cell) {
                auto cellValue = SymbolicSemantics::SValue::promote(cell->value())->get_expression();

                // Search for the byte directly. Unfortunately this doesn't always work due to minor differences. For instance, the
                // needle might be (extract[u8] 0[u32], 8[u32], expr[u64]) but the value in memory might be (extract[u8] 0[u64],
                // 8[u64], expr[u64]) -- a difference only in the size of the types used to extract the byte. Since memory cell
                // values are almost always extract expressions, we can also try looking "through" the extract to the value being
                // extracted.
                if (cellValue->isEquivalentTo(searchByte)) {
                    foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                } else if (cellValue->isOperator(OP_EXTRACT) &&
                           cellValue->child(0)->toUnsigned().orElse(1) == 0 &&
                           cellValue->child(1)->toUnsigned().orElse(1) == 8 &&
                           cellValue->child(2)->isEquivalentTo(searchValue)) {
                    foundAddr = SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                }
            }
        } visitor(searchValue, searchByte);

        mem->traverse(visitor);
        return visitor.foundAddr;
    }

    return {};
}

// Return the set of interesting constants found in an expression. Constants are interesting if:
//
//   1. A constant that is an operand of an `add` operation.
//
// The list may change in the future.
static std::set<Address>
findInterestingConstants(const SymbolicExpression::Ptr &expr) {
    using namespace SymbolicExpression;

    struct: Visitor {
        std::set<const Node*> seen;                     // avoid re-processing common subexpressions
        std::set<Address> found;

        VisitAction preVisit(const Node *expr) {
            if (seen.insert(expr).second) {
                if (expr->isOperator(OP_ADD)) {
                    for (const Ptr &child: expr->children()) {
                        if (const auto n = child->toUnsigned())
                            found.insert(*n);
                    }
                }
                return CONTINUE;
            } else {
                return TRUNCATE;
            }
        }

        VisitAction postVisit(const Node*) {
            return CONTINUE;
        }
    } visitor;
    expr->depthFirstTraversal(visitor);
    return visitor.found;
}

// True if the address is mapped and has all the required permissions and none of the prohibited permissions.
static bool
isMappedAccess(const MemoryMap::Ptr &map, const Address addr, const unsigned required, const unsigned prohibited) {
    ASSERT_not_null(map);
    return !map->at(addr).require(required).prohibit(prohibited).segments().empty();
}

// Return jump table target addresses for table entries whose addresses are satisfiable given the path constraints. The
// `entryAddrExpr` is the symbolic address that was read by the basic block and represents any table entry.
std::set<Address>
satisfiableTargets(const JumpTable::Ptr &table, const SymbolicExpression::Ptr &pathConstraint,
                   const SymbolicExpression::Ptr &entryAddrExpr) {
    ASSERT_not_null(table);
    ASSERT_not_null(pathConstraint);
    std::set<Address> retval;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SmtSolver::Ptr solver = SmtSolver::bestAvailable();
    ASSERT_not_null(solver);                            // FIXME[Robb Matzke 2025-04-14]
    solver->insert(pathConstraint);

    using namespace SymbolicExpression;
    for (size_t i = 0; i < table->nEntries(); ++i) {
        const Address target = table->targets()[i];
        const Address entryAddr = table->location().least() + i * table->bytesPerEntry();
        const auto entryConstraint = makeEq(makeIntegerConstant(entryAddrExpr->nBits(), entryAddr), entryAddrExpr);
        SAWYER_MESG(debug) <<"    entry #" <<i <<" at " <<addrToString(entryAddr)
                           <<" has target " <<addrToString(target) <<"\n";
        solver->push();
        solver->insert(entryConstraint);
        const auto isSatisfiable = solver->check();
        solver->pop();

        switch (isSatisfiable) {
            case SmtSolver::SAT_UNKNOWN:
                SAWYER_MESG(debug) <<"      SMT solver failed (assuming satisfiable)\n";
                // fall through
            case SmtSolver::SAT_YES: {
                const Address target = table->targets()[i];
                SAWYER_MESG(debug) <<"      satisfiable table entry address\n";
                retval.insert(target);
                break;
            }
            case SmtSolver::SAT_NO:
                SAWYER_MESG(debug) <<"      entry address is not satisfiable\n";
                break;
        }
    }
    return retval;
}

static bool
useJumpTable(const P2::Partitioner::Ptr &partitioner, const P2::Function::Ptr &function, const P2::BasicBlock::Ptr &bb,
             const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bb);
    ASSERT_not_null(ops);
    BS::State::Ptr state = ops->currentState();
    ASSERT_not_null(state);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    const Sawyer::Container::Set<Address> functionBlocks(function->basicBlockAddresses().begin(),
                                                         function->basicBlockAddresses().end());

    SAWYER_MESG(debug) <<"  results for " <<bb->printableName() <<"\n";
    const RegisterDescriptor IP = partitioner->architecture()->registerDictionary()->instructionPointerRegister();
    const SymbolicExpression::Ptr ip = SymbolicSemantics::SValue::promote(ops->peekRegister(IP))->get_expression();
    SAWYER_MESG(debug) <<"    ip = " <<*ip <<"\n";

    const RegisterDescriptor PATH = createPathRegister(partitioner);
    const SymbolicExpression::Ptr path = SymbolicSemantics::SValue::promote(ops->peekRegister(PATH))->get_expression();
    SAWYER_MESG(debug) <<"    path constraints = " <<*path <<"\n";

    // FIXME[Robb Matzke 2025-04-08]: We only handle specific things for now.
    if (as<const Architecture::X86>(partitioner->architecture())) {
        const auto matched = match(ip);
        const auto jumpTableEntry = matched.first;
        const Address perEntryOffset = matched.second;
        if (jumpTableEntry) {
            SAWYER_MESG(debug) <<"    value read from jump table: " <<*jumpTableEntry <<"\n";
            if (const auto jumpTableAddrExpr = findAddressContaining(ops, jumpTableEntry)) {
                SAWYER_MESG(debug) <<"    address from which it was read: " <<*jumpTableAddrExpr <<"\n";
                const std::set<Address> constants = findInterestingConstants(jumpTableAddrExpr);
                for (const Address tableAddr: constants) {
                    if (isMappedAccess(partitioner->memoryMap(), tableAddr, MemoryMap::READABLE, MemoryMap::WRITABLE)) {
                        SAWYER_MESG(debug) <<"    possible jump table at " <<addrToString(tableAddr) <<"\n"
                                           <<"    per-entry offset is " <<addrToString(perEntryOffset) <<"\n";

                        const auto tableLimits = AddressInterval::whole(); // will be refined
                        const size_t bytesPerEntry = (jumpTableEntry->nBits() + 7) / 8;
                        auto table = JumpTable::instance(partitioner, tableLimits, bytesPerEntry, perEntryOffset,
                                                         JumpTable::EntryType::ABSOLUTE);
                        table->maxPreEntries(0);
                        table->refineLocationLimits(bb, tableAddr);
                        SAWYER_MESG(debug) <<"    table limited to " <<addrToString(table->tableLimits()) <<"\n";

                        //table->targetLimits(partitioner->functionBasicBlockExtent(function).hull());
                        table->refineTargetLimits(bb);
                        SAWYER_MESG(debug) <<"    targets limited to " <<addrToString(table->targetLimits()) <<"\n";

                        SAWYER_MESG(debug) <<"    scanning table at " <<addrToString(tableAddr)
                                           <<" within " <<addrToString(table->tableLimits()) <<"\n";
                        table->scan(partitioner->memoryMap()->require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE), tableAddr);
                        if (table->location()) {
                            SAWYER_MESG(debug) <<"    parsed jump table with " <<plural(table->nEntries(), "entries") <<"\n";
                            std::set<Address> successors = satisfiableTargets(table, path, jumpTableAddrExpr);
                            if (debug) {
                                debug <<"    unique targets remaining: " <<successors.size() <<"\n";
                                for (const Address target: successors) {
                                    debug <<"      target " <<addrToString(target)
                                          <<(functionBlocks.exists(target) ? " present" : " not present") <<"\n";
                                }
                            }
                            
                            partitioner->detachBasicBlock(bb);
                            bb->successors().clear();
                            const size_t bitsPerWord = partitioner->architecture()->bitsPerWord();
                            for (const Address successor: successors)
                                bb->insertSuccessor(successor, bitsPerWord);
                            table->attachTableToBasicBlock(bb);
                            partitioner->attachBasicBlock(bb);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool
analyzeFunction(const P2::Partitioner::Ptr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    bool madeChanges = false;

    Sawyer::Container::Set<Address> interesting = findBlocksWithIndeterminateBranch(partitioner, function);
    SAWYER_MESG(debug) <<"analyzing indirect control flow for " <<function->printableName() <<"\n";
    if (debug) {
        debug <<"  indirect cfg from " <<interesting.size()
              <<" of " <<plural(function->basicBlockAddresses().size(), "basic blocks") <<"\n";
        debug <<"  basic blocks with unresolved indirect control flow: " <<plural(interesting.size(), "blocks") <<"\n";
        for (const auto addr: interesting.values())
            debug <<"    basic block " <<addrToString(addr) <<"\n";
    }

    // Data flow uses the CFG but our CFG is incomplete due to the very missing edges that we're trying to find. Therefore, we can
    // only analyze up to the first interesting blocks, then add the new edges that we find, then repeat until we make it through
    // all the interesting blocks.
    while (!interesting.isEmpty()) {
        // Control flow graph for dataflow. When building the dataflow graph, stop when we get to any of the interesting blocks
        // because we don't know where to go after that.
        const auto functionEntry = partitioner->findPlaceholder(function->address());
        ASSERT_require(partitioner->cfg().isValidVertex(functionEntry));
        InterproceduralPredicate ipp(partitioner);
        DfCfg dfCfg = P2DF::buildDfCfg(partitioner, partitioner->cfg(), functionEntry, ipp);
        pruneDfCfg(dfCfg, interesting);

    #if 1 // [Robb Matzke 2025-04-08]: debugging; remove before commit
        if (function->name() == "main") {
            debug <<"  saved dataflow cfg to x.dot\n";
            std::ofstream dot("x.dot");
            P2DF::dumpDfCfg(dot, dfCfg);
        }
    #endif

        // Transfer function
        BS::RiscOperators::Ptr ops = partitioner->newOperators();
        BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
        DfTransfer xfer(cpu);
        xfer.ignoringSemanticFailures(true);

        // Merge function
        DfMerge merge(partitioner, ops, dfCfg);

        // Initial state
        BS::State::Ptr initialState = xfer.initialState();
        ops->currentState(initialState);
        initializeStackPointer(partitioner, ops);

        // Dataflow engine
        using DfEngine = Rose::BinaryAnalysis::DataFlow::Engine<DfCfg, BS::State::Ptr, DfTransfer, DfMerge>;
        DfEngine dfEngine(dfCfg, xfer, merge);
        dfEngine.maxIterations(dfCfg.nVertices());
        dfEngine.insertStartingVertex(0, initialState);

        // Run to fixed point if possible
        try {
            dfEngine.runToFixedPoint();
        } catch (const DfNotConverging&) {
        }

        // Look at the outgoing instruction pointer registers for the interesting basic blocks. Use it to try to recover the indirect
        // control flow.
        for (const auto &vertex: dfCfg.vertices()) {
            if (P2::BasicBlock::Ptr bb = vertex.value().bblock()) {
                if (interesting.exists(bb->address())) {
                    interesting.erase(bb->address());
                    if (BS::State::Ptr state = dfEngine.getFinalState(vertex.id())) {
                        ops->currentState(state);
                        if (useJumpTable(partitioner, function, bb, ops))
                            madeChanges = true;
                    }
                }
            }
        }
    }
    return madeChanges;
}

bool
analyzeFunctions(const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    bool madeChanges = false;

    for (const P2::Function::Ptr &function: partitioner->functions()) {
#if 1 // [Robb Matzke 2025-04-11]
        if (function->name() != "main")
            continue;
#endif
    
        if (analyzeFunction(partitioner, function))
            madeChanges = true;
    }
    return madeChanges;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
