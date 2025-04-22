#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>                                 // needed for `::SageInterface`
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <Rose/BinaryAnalysis/Partitioner2/AddressUsageMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPowerpc.h>
#include <Rose/BinaryAnalysis/Partitioner2/Reference.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Disassembler/Null.h>
#include <Rose/RecursionCounter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/config.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/thread.hpp>
#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/Stack.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/ThreadWorkers.h>

#ifndef BOOST_WINDOWS
    #include <errno.h>
    #include <fcntl.h>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
#endif

using namespace Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner::Thunk
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner::Thunk::Thunk(const BasicBlock::Ptr &bblock, Address target)
    : bblock(bblock), target(target) {}

Partitioner::Thunk::~Thunk() {}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Used only by boost::serialization
Partitioner::Partitioner() {}

Partitioner::Partitioner(const Architecture::Base::ConstPtr &architecture)
    : architecture_(architecture), solver_(SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver)),
      progress_(Progress::instance()) {
    init(MemoryMap::Ptr());
}

Partitioner::Partitioner(const Architecture::Base::ConstPtr &architecture, const MemoryMap::Ptr &map)
    : architecture_(architecture), memoryMap_(map), solver_(SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver)),
      progress_(Progress::instance()) {
    init(map);
}

Partitioner::Ptr
Partitioner::instance(const Architecture::Base::ConstPtr &architecture) {
    return Ptr(new Partitioner(architecture));
}

Partitioner::Ptr
Partitioner::instance(const Architecture::Base::ConstPtr &architecture, const MemoryMap::Ptr &memoryMap) {
    return Ptr(new Partitioner(architecture, memoryMap));
}

Partitioner::Ptr
Partitioner::instanceFromRbaFile(const boost::filesystem::path &name, SerialIo::Format fmt) {
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"reading RBA state from " <<name;
    Sawyer::Stopwatch timer;
    SerialInput::Ptr archive = SerialInput::instance();
    archive->format(fmt);
    archive->open(name);

    Partitioner::Ptr partitioner = archive->loadPartitioner();

    while (archive->objectType() == SerialIo::AST)
        archive->loadAst();

    info <<"; took " <<timer << "\n";
    return partitioner;
}

void
Partitioner::saveAsRbaFile(const boost::filesystem::path &name, SerialIo::Format fmt) const {
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"writing RBA state file to " <<name;
    Sawyer::Stopwatch timer;
    SerialOutput::Ptr archive = SerialOutput::instance();
    archive->format(fmt);
    archive->open(name);

    archive->savePartitioner(sharedFromThis());

    if (SgProject *project = SageInterface::getProject()) {
        for (SgBinaryComposite *file: SageInterface::querySubTree<SgBinaryComposite>(project))
            archive->saveAst(file);
    }

    info <<"; took " <<timer <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copy construction, assignment, destructor when move semantics are present
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// move constructor
Partitioner::Partitioner(BOOST_RV_REF(Partitioner) other)
    : interpretation_(nullptr), solver_(SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver)),
      autoAddCallReturnEdges_(false), assumeFunctionsReturn_(true), stackDeltaInterproceduralLimit_(1),
      semanticMemoryParadigm_(LIST_BASED_MEMORY), progress_(Progress::instance()), cfgProgressTotal_(0) {
    *this = boost::move(other);
}

// move assignment
Partitioner&
Partitioner::operator=(BOOST_RV_REF(Partitioner) other) {
    // FIXME[Robb Matzke 2019-06-21]: Some of the data members don't support move semantics, so we copy those and then
    // delete them from the source. The key thing we're trying to avoid is for other's destructor from doing anything
    // that might interfere with the new object.

    // FIXME[Robb Matzke 2019-06-21]: faked move semantics
    Sawyer::Attribute::Storage<>::operator=(other);
    settings_ = other.settings_;
    config_ = other.config_;
    architecture_ = other.architecture_;
    cfg_ = other.cfg_;
    aum_ = other.aum_;
    vertexIndex_.clear();                               // initialized by init(other)
    functions_ = other.functions_;
    addressNames_ = other.addressNames_;
    sourceLocations_ = other.sourceLocations_;

    // FIXME[Robb Matzke 2019-06-21]: faked move semantics and no way to clear the source
    cfgAdjustmentCallbacks_ = other.cfgAdjustmentCallbacks_;
    basicBlockCallbacks_ = other.basicBlockCallbacks_;
    functionPrologueMatchers_ = other.functionPrologueMatchers_;
    functionPaddingMatchers_ = other.functionPaddingMatchers_;

    // The rest are okay because they're PODs
    instructionProvider_ = other.instructionProvider_;
    other.instructionProvider_ = InstructionProvider::Ptr();

    memoryMap_ = other.memoryMap_;
    other.memoryMap_ = MemoryMap::Ptr();

    interpretation_ = other.interpretation_;
    other.interpretation_ = nullptr;

    solver_ = other.solver_;
    other.solver_ = SmtSolver::Ptr();

    autoAddCallReturnEdges_ = other.autoAddCallReturnEdges_;
    assumeFunctionsReturn_ = other.assumeFunctionsReturn_;
    stackDeltaInterproceduralLimit_ = other.stackDeltaInterproceduralLimit_;
    semanticMemoryParadigm_ = other.semanticMemoryParadigm_;

    unparser_ = other.unparser_;
    other.unparser_ = Unparser::Base::Ptr();

    insnUnparser_ = other.insnUnparser_;
    other.insnUnparser_ = Unparser::Base::Ptr();
    insnPlainUnparser_ = other.insnPlainUnparser_;
    other.insnPlainUnparser_ = Unparser::Base::Ptr();

    {
        SAWYER_THREAD_TRAITS::LockGuard2(mutex_, other.mutex_);
        cfgProgressTotal_ = other.cfgProgressTotal_;
        progress_ = other.progress_;
        other.progress_ = Progress::Ptr();
    }

    // Finish initializing the enew object before we totally wipe out the old one.
    init(other);

    // FIXME[Robb Matzke 2019-06-21]: For all the faked move semantics above, clear them now in other.
    other.clearAttributes();
    other.settings_ = BasePartitionerSettings();
    other.config_ = Configuration();
    other.cfg_ = ControlFlowGraph();
    other.aum_ = AddressUsageMap();
    other.functions_ = Functions();
    other.addressNames_ = AddressNameMap();
    other.sourceLocations_ = SourceLocations();
    
    return *this;
}

Partitioner::~Partitioner() {
    // Detaching all functions breaks any reference-counting pointer cycles
    std::vector<Function::Ptr> list = functions();      // make a copy so we can modify while iterating
    for (const Function::Ptr &function: list)
        detachFunction(function);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::init(const MemoryMap::Ptr &map) {
    // Start with a large hash table to reduce early rehashing. There's a high chance that we'll need this much.
    vertexIndex_.rehash(100000);

    instructionProvider_ = InstructionProvider::instance(architecture(), map);
    unparser_ = architecture()->newUnparser();
    insnUnparser_ = architecture()->newUnparser();
    configureInsnUnparser(insnUnparser_);
    insnPlainUnparser_ = architecture()->newUnparser();
    configureInsnPlainUnparser(insnPlainUnparser_);

    undiscoveredVertex_ = cfg_.insertVertex(CfgVertex(V_UNDISCOVERED));
    indeterminateVertex_ = cfg_.insertVertex(CfgVertex(V_INDETERMINATE));
    nonexistingVertex_ = cfg_.insertVertex(CfgVertex(V_NONEXISTING));
}

void
Partitioner::init(const Partitioner &other) {
    // All graph vertices need to be converted from other.cfg_ to this->cfg_
    for (const CfgVertexIndex::Node &node: other.vertexIndex_.nodes())
        vertexIndex_.insert(node.key(), convertFrom(other, node.value()));
    undiscoveredVertex_ = convertFrom(other, other.undiscoveredVertex_);
    indeterminateVertex_ = convertFrom(other, other.indeterminateVertex_);
    nonexistingVertex_ = convertFrom(other, other.nonexistingVertex_);
}

bool
Partitioner::isDefaultConstructed() const {
    return !instructionProvider_;
}

void
Partitioner::clear() {
    cfg_.clear();
    vertexIndex_.clear();
    aum_.clear();
    functions_.clear();
}

Configuration&
Partitioner::configuration() {
    return config_;
}

const Configuration&
Partitioner::configuration() const {
    return config_;
}

Architecture::Base::ConstPtr
Partitioner::architecture() const {
    ASSERT_not_null(architecture_);
    return architecture_;
}

InstructionProvider&
Partitioner::instructionProvider() {
    return *instructionProvider_;
}

const InstructionProvider&
Partitioner::instructionProvider() const {
    return *instructionProvider_;
}

MemoryMap::Ptr
Partitioner::memoryMap() const {
    return memoryMap_;
}

SgAsmInterpretation*
Partitioner::interpretation() const {
    return interpretation_;
}

void
Partitioner::interpretation(SgAsmInterpretation *interp) {
    interpretation_ = interp;
}

bool
Partitioner::addressIsExecutable(Address va) const {
    return memoryMap_ && memoryMap_->at(va).require(MemoryMap::EXECUTABLE).exists();
}

ControlFlowGraph::VertexIterator
Partitioner::convertFrom(const Partitioner &other, ControlFlowGraph::ConstVertexIterator otherIter) {
    if (otherIter==other.cfg_.vertices().end())
        return cfg_.vertices().end();
    ControlFlowGraph::VertexIterator thisIter = cfg_.findVertex(otherIter->id());
    ASSERT_forbid(thisIter == cfg_.vertices().end());
    return thisIter;
}

void
Partitioner::showStatistics() const {
    std::cout <<"Rose::BinaryAnalysis::Partitioner2::Parttioner statistics:\n";
    std::cout <<"  address to CFG vertex mapping:\n";
    std::cout <<"    size = " <<vertexIndex_.size() <<"\n";
    std::cout <<"    number of hash buckets =  " <<vertexIndex_.nBuckets() <<"\n";
    std::cout <<"    load factor = " <<vertexIndex_.loadFactor() <<"\n";
    instructionProvider().showStatistics();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unparsing -- functions that deal with creating assembly listings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
void
Partitioner::configureInsnUnparser(const Unparser::Base::Ptr &unparser) const {
    ASSERT_not_null(unparser);
    unparser->settings() = Unparser::Settings::minimal();
    unparser->settings().insn.address.showing = true;
    unparser->settings().insn.address.useLabels = false;
    unparser->settings().insn.address.fieldWidth = 1;
    unparser->settings().insn.mnemonic.fieldWidth = 1;
    unparser->settings().insn.operands.fieldWidth = 1;
}

void
Partitioner::configureInsnPlainUnparser(const Unparser::Base::Ptr &unparser) const {
    configureInsnUnparser(unparser);
    unparser->settings().insn.address.showing = false;
    unparser->settings().insn.bytes.showing = false;
    unparser->settings().insn.stackDelta.showing = false;
    unparser->settings().insn.frameDelta.showing = false;
    unparser->settings().insn.mnemonic.fieldWidth = 1;
    unparser->settings().insn.comment.showing = false;
    unparser->settings().insn.semantics.showing = false;
}

Unparser::Base::Ptr
Partitioner::unparser() const {
    return unparser_;
}

void
Partitioner::unparser(const Unparser::Base::Ptr &u) {
    unparser_ = u;
}

Unparser::Base::Ptr
Partitioner::insnUnparser() const {
    return insnUnparser_;
}

void
Partitioner::insnUnparser(const Unparser::Base::Ptr &u) {
    insnUnparser_ = u;
}

std::string
Partitioner::unparse(SgAsmInstruction *insn) const {
    std::ostringstream ss;
    unparse(ss, insn);
    return ss.str();
}

void
Partitioner::unparse(std::ostream &out, SgAsmInstruction *insn) const {
    if (!insn) {
        out <<"null instruction";
    } else {
        ASSERT_not_null(insnUnparser());
        (*insnUnparser())(out, sharedFromThis(), insn);
    }
}

std::string
Partitioner::unparsePlain(SgAsmInstruction *insn) const {
    if (!insn) {
        return "null instruction";
    } else {
        // We don't allow the user to modify the plain unparser but there are certain settings we want to copy from the main
        // instruction unparser. Since the user can modify the main instruction unparser any time, we need to always copy those
        // important settings the the plain unparser.
        ASSERT_not_null(insnUnparser_);
        ASSERT_not_null(insnPlainUnparser_);
        insnPlainUnparser_->settings().colorization = insnUnparser_->settings().colorization;
        return (*insnPlainUnparser_)(sharedFromThis(), insn);
    }
}

void
Partitioner::unparse(std::ostream &out, const BasicBlock::Ptr &bb) const {
    if (!bb) {
        out <<"null basic block";
    } else {
        ASSERT_not_null(unparser());
        (*unparser())(out, sharedFromThis(), bb);
    }
}

void
Partitioner::unparse(std::ostream &out, const DataBlock::Ptr &db) const {
    if (!db) {
        out <<"null data block";
    } else {
        ASSERT_not_null(unparser());
        (*unparser())(out, sharedFromThis(), db);
    }
}

void
Partitioner::unparse(std::ostream &out, const Function::Ptr &f) const {
    if (!f) {
        out <<"null function";
    } else {
        ASSERT_not_null(unparser());
        (*unparser())(out, sharedFromThis(), f);
    }
}

void
Partitioner::unparse(std::ostream &out) const {
    ASSERT_not_null(unparser());
    (*unparser())(out, sharedFromThis());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner CFG queries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Partitioner::nBytes() const {
    return aum_.size();
}

ControlFlowGraph::VertexIterator
Partitioner::undiscoveredVertex() {
    return undiscoveredVertex_;
}

ControlFlowGraph::ConstVertexIterator
Partitioner::undiscoveredVertex() const {
    return undiscoveredVertex_;
}

ControlFlowGraph::VertexIterator
Partitioner::indeterminateVertex() {
    return indeterminateVertex_;
}

ControlFlowGraph::ConstVertexIterator
Partitioner::indeterminateVertex() const {
    return indeterminateVertex_;
}

ControlFlowGraph::VertexIterator
Partitioner::nonexistingVertex() {
    return nonexistingVertex_;
}

ControlFlowGraph::ConstVertexIterator
Partitioner::nonexistingVertex() const {
    return nonexistingVertex_;
}

const ControlFlowGraph&
Partitioner::cfg() const {
    return cfg_;
}

const AddressUsageMap&
Partitioner::aum() const {
    return aum_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Progress reporting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Label the progress report and also show some other statistics.  It is okay for this to be slightly expensive since its only
// called when a progress report is actually emitted.
class ProgressBarSuffix {
    const Partitioner *partitioner_;
public:
    ProgressBarSuffix(): partitioner_(nullptr) {}
    explicit ProgressBarSuffix(const Partitioner *p): partitioner_(p) {}
    void print(std::ostream &out) const {
        ASSERT_not_null(partitioner_);
        out <<" bytes"                                  // units for progress report
            <<" " <<StringUtility::plural(partitioner_->nPlaceholders(), "blks")
            <<" " <<StringUtility::plural(partitioner_->nFunctions(), "funcs");
    }
};

static std::ostream&
operator<<(std::ostream &out, const ProgressBarSuffix &x) {
    x.print(out);
    return out;
};

Progress::Ptr
Partitioner::progress() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return progress_;
}

void
Partitioner::progress(const Progress::Ptr &p) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    progress_ = p;
}

void
Partitioner::updateProgress(const std::string &phase, double completion) const {
    Progress::Ptr progress;
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        progress = progress_;
    }
    if (progress)
        progress->update(Progress::Report(phase, completion));
}

// Updates progress information during the main partitioning phase. The progress is the ratio of the number of bytes
// represented in the CFG to the number of executable bytes in the memory map.
void
Partitioner::updateCfgProgress() {
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        if (!progress_)
            return;
    }

    // How many bytes are mapped with execute permission
    if (0 == cfgProgressTotal_) {
        for (const MemoryMap::Node &node: memoryMap_->nodes()) {
            if (0 != (node.value().accessibility() & MemoryMap::EXECUTABLE))
                cfgProgressTotal_ += node.key().size();
        }
    }

    double completion = cfgProgressTotal_ > 0 ? (double)nBytes() / cfgProgressTotal_ : 0.0;
    updateProgress("partition", completion);

    // All partitioners share a single progress bar for the CFG completion amount
    static Sawyer::ProgressBar<size_t, ProgressBarSuffix> *bar = nullptr;
    if (!bar)
        bar = new Sawyer::ProgressBar<size_t, ProgressBarSuffix>(cfgProgressTotal_, mlog[MARCH], "cfg");

    // Update the progress bar. If multiple partitioners are sharing the progress bar then also make sure that the lower and
    // upper limits are appropriate for THIS partitioner.  However, changing the limits is a configuration change, which also
    // immediately updates the progress bar (we don't want that, so update only if necessary).
    if (cfgProgressTotal_) {
        bar->suffix(ProgressBarSuffix(this));
        if (bar->domain().first!=0 || bar->domain().second!=cfgProgressTotal_) {
            bar->value(0, nBytes(), cfgProgressTotal_);
        } else {
            bar->value(nBytes());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instructions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Partitioner::nInstructions() const {
    size_t nInsns = 0;
    for (const CfgVertex &vertex: cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock())
                nInsns += bb->nInstructions();
        }
    }
    return nInsns;
}

AddressUser
Partitioner::instructionExists(Address startVa) const {
    return aum_.findInstruction(startVa);
}

AddressUser
Partitioner::instructionExists(SgAsmInstruction *insn) const {
    return aum_.findInstruction(insn);
}

CrossReferences
Partitioner::instructionCrossReferences(const AddressIntervalSet &restriction) const {
    CrossReferences xrefs;

    struct Accumulator: AstSimpleProcessing {
        const AddressIntervalSet &restriction;
        CrossReferences &xrefs;
        Reference to;
        Accumulator(const AddressIntervalSet &restriction, CrossReferences &xrefs): restriction(restriction), xrefs(xrefs) {}
        void target(const Reference &to) { this->to = to; }
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                Address n = ival->get_absoluteValue();
                if (restriction.exists(n))
                    xrefs.insertMaybeDefault(Reference(n)).insert(to);
            }
        }
    } accumulator(restriction, xrefs);

    for (const BasicBlock::Ptr &bblock: basicBlocks()) {
        std::vector<Function::Ptr> functions = functionsOwningBasicBlock(bblock);
        for (SgAsmInstruction *insn: bblock->instructions()) {
            for (const Function::Ptr &function: functions) {
                accumulator.target(Reference(function, bblock, insn));
                accumulator.traverse(insn, preorder);
            }
        }
    }
    return xrefs;
}

AddressInterval
Partitioner::instructionExtent(SgAsmInstruction *insn) const {
    return insn ? AddressInterval::baseSize(insn->get_address(), insn->get_size()) : AddressInterval();
}

SgAsmInstruction *
Partitioner::discoverInstruction(Address startVa) const {
    return (*instructionProvider_)[startVa];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Placeholders -- i.e., control flow graph vertices
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Partitioner::nPlaceholders() const {
    ASSERT_require(cfg_.nVertices() >= nSpecialVertices);
    return cfg_.nVertices() - nSpecialVertices;
}

bool
Partitioner::placeholderExists(Address startVa) const {
    return vertexIndex_.exists(startVa);
}

ControlFlowGraph::VertexIterator
Partitioner::findPlaceholder(Address startVa) {
    if (Sawyer::Optional<ControlFlowGraph::VertexIterator> found = vertexIndex_.getOptional(startVa))
        return *found;
    return cfg_.vertices().end();
}

ControlFlowGraph::ConstVertexIterator
Partitioner::findPlaceholder(Address startVa) const {
    if (Sawyer::Optional<ControlFlowGraph::VertexIterator> found = vertexIndex_.getOptional(startVa))
        return *found;
    return cfg_.vertices().end();
}

BasicBlock::Ptr
Partitioner::erasePlaceholder(const ControlFlowGraph::ConstVertexIterator &placeholder) {
    BasicBlock::Ptr bblock;
    if (placeholder!=cfg_.vertices().end() && placeholder->value().type()==V_BASIC_BLOCK) {
        Address startVa = placeholder->value().address();
        if ((bblock = placeholder->value().bblock()))
            detachBasicBlock(placeholder);              // removes self edges, notifies subclasses of CFG changes
        if (placeholder->nInEdges()!=0) {
            throw PlaceholderError(startVa, "cannot erase placeholder " + StringUtility::addrToString(startVa) +
                                   " that has " + StringUtility::plural(placeholder->nInEdges(), "incoming edges"));
        }
        vertexIndex_.erase(placeholder->value().address());
        cfg_.eraseVertex(placeholder);
        bblockDetached(startVa, BasicBlock::Ptr());     // null bblock indicates placeholder erasure
    }
    return bblock;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Partitioner::basicBlockSemanticsAutoDrop() const {
    return settings_.basicBlockSemanticsAutoDrop;
}

void
Partitioner::basicBlockSemanticsAutoDrop(bool b) {
    settings_.basicBlockSemanticsAutoDrop = b;
}

void
Partitioner::basicBlockDropSemantics() const {
    for (const ControlFlowGraph::VertexValue &vertex: cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bblock = vertex.bblock())
                bblock->dropSemantics(sharedFromThis());
        }
    }
}

size_t
Partitioner::nBasicBlocks() const {
    size_t nBasicBlocks = 0;
    for (const CfgVertex &vertex: cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK && vertex.bblock())
            ++nBasicBlocks;
    }
    return nBasicBlocks;
}

BasicBlock::Ptr
Partitioner::basicBlockExists(Address startVa) const {
    ControlFlowGraph::ConstVertexIterator vertex = findPlaceholder(startVa);
    if (vertex!=cfg_.vertices().end())
        return vertex->value().bblock();
    return BasicBlock::Ptr();
}

BasicBlock::Ptr
Partitioner::basicBlockExists(const BasicBlock::Ptr &bblock) const {
    return bblock==nullptr ? BasicBlock::Ptr() : basicBlockExists(bblock->address());
}

SemanticMemoryParadigm
Partitioner::semanticMemoryParadigm() const {
    return semanticMemoryParadigm_;
}

void
Partitioner::semanticMemoryParadigm(SemanticMemoryParadigm p) {
    semanticMemoryParadigm_ = p;
}

SmtSolver::Ptr
Partitioner::smtSolver() const {
    return solver_;
}

BaseSemantics::RiscOperators::Ptr
Partitioner::newOperators() const {
    return newOperators(semanticMemoryParadigm_);
}

BaseSemantics::RiscOperators::Ptr
Partitioner::newOperators(SemanticMemoryParadigm memType) const {
    //  Create a new SMT solver each call because this might be called from different threads and each thread should have its
    //  own SMT solver.
    SmtSolver::Ptr solver = solver_ ? solver_->create() : SmtSolver::Ptr();

    Semantics::RiscOperators::Ptr ops =
        Semantics::RiscOperators::instance(instructionProvider_->registerDictionary(), solver, memType);
    BaseSemantics::MemoryState::Ptr mem = ops->currentState()->memoryState();
    if (Semantics::MemoryListState::Ptr ml = as<Semantics::MemoryListState>(mem)) {
        ml->memoryMap(memoryMap_);
    } else if (Semantics::MemoryMapState::Ptr mm = as<Semantics::MemoryMapState>(mem)) {
        mm->memoryMap(memoryMap_);
    }
    return ops;
}

BaseSemantics::Dispatcher::Ptr
Partitioner::newDispatcher(const BaseSemantics::RiscOperators::Ptr &ops) const {
    ASSERT_not_null(ops);
    return architecture()->newInstructionDispatcher(ops);
}

BasicBlock::Ptr
Partitioner::detachBasicBlock(const ControlFlowGraph::ConstVertexIterator &constPlaceholder) {
    BasicBlock::Ptr bblock;
    if (constPlaceholder != cfg_.vertices().end() && constPlaceholder->value().type()==V_BASIC_BLOCK) {
        ASSERT_require(cfg_.isValidVertex(constPlaceholder));
        ControlFlowGraph::VertexIterator placeholder = cfg_.findVertex(constPlaceholder->id());
        bblock = placeholder->value().bblock();
        placeholder->value().nullify();
        adjustPlaceholderEdges(placeholder);
        bblock->thaw();

        // Remove its instructions from the AUM if there are no other basic blocks owning the instruction.
        for (SgAsmInstruction *insn: bblock->instructions())
            aum_.eraseInstruction(insn, bblock);

        // Remove th basic block from all its data blocks' attached owners lists, and for any data blocks that no longer
        // have attached owners, detach them from this partitioner.
        for (const DataBlock::Ptr &dblock: bblock->dataBlocks()) {
            dblock->eraseOwner(bblock);
            if (dblock->nAttachedOwners() == 0)
                detachDataBlock(dblock);
        }

        // Run callbacks for detached basic blocks
        bblockDetached(bblock->address(), bblock);
    }
    return bblock;
}

BasicBlock::Ptr
Partitioner::detachBasicBlock(const BasicBlock::Ptr &bblock) {
    if (bblock!=nullptr) {
        ControlFlowGraph::VertexIterator placeholder = findPlaceholder(bblock->address());
        if (placeholder!=cfg_.vertices().end() && placeholder->value().bblock()==bblock)
            return detachBasicBlock(placeholder);
    }
    return BasicBlock::Ptr();
}

BasicBlock::Ptr
Partitioner::detachBasicBlock(Address startVa) {
    return detachBasicBlock(findPlaceholder(startVa));
}

ControlFlowGraph::EdgeIterator
Partitioner::adjustPlaceholderEdges(const ControlFlowGraph::VertexIterator &placeholder) {
    ASSERT_require(placeholder!=cfg_.vertices().end());
    ASSERT_require2(nullptr==placeholder->value().bblock(), "vertex must be strictly a placeholder");
    cfg_.clearOutEdges(placeholder);
    return cfg_.insertEdge(placeholder, undiscoveredVertex_);
}

ControlFlowGraph::EdgeIterator
Partitioner::adjustNonexistingEdges(const ControlFlowGraph::VertexIterator &vertex) {
    ASSERT_require(vertex!=cfg_.vertices().end());
    ASSERT_not_null2(vertex->value().bblock(), "vertex must have been discovered");
    ASSERT_require2(vertex->value().bblock()->isEmpty(), "vertex must be non-existing");
    cfg_.clearOutEdges(vertex);
    return cfg_.insertEdge(vertex, nonexistingVertex_);
}

BasicBlock::Ptr
Partitioner::discoverBasicBlock(const ControlFlowGraph::ConstVertexIterator &placeholder) const {
    ASSERT_require2(placeholder != cfg_.vertices().end(), "invalid basic block placeholder");
    BasicBlock::Ptr bb = placeholder->value().bblock();
    return bb!=nullptr ? bb : discoverBasicBlockInternal(placeholder->value().address());
}

BasicBlock::Ptr
Partitioner::discoverBasicBlock(Address startVa) const {
    ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(startVa);
    return placeholder==cfg_.vertices().end() ? discoverBasicBlockInternal(startVa) : discoverBasicBlock(placeholder);
}

BasicBlock::Ptr
Partitioner::discoverBasicBlockInternal(Address startVa) const {
    // If the first instruction of this basic block already exists (in the middle of) some other basic blocks then the other
    // basic blocks are called "conflicting blocks".  This only applies for the first instruction of this block, but is used in
    // the termination conditions below.
    AddressUser startVaOwners = instructionExists(startVa);
    if (startVaOwners)
        ASSERT_forbid(startVaOwners.isBlockEntry());                    // handled in discoverBasicBlock

    // Keep adding instructions until we reach a termination condition.  The termination conditions are enumerated in detail in
    // the doxygen documentation for this function. READ IT AND KEEP IT UP TO DATE!!!
    BasicBlock::Ptr retval = BasicBlock::instance(startVa, sharedFromThis());
    Address va = startVa;
    while (1) {
        SgAsmInstruction *insn = discoverInstruction(va);
        if (insn==nullptr)                                              // case: no instruction available
            goto done;
        retval->append(sharedFromThis(), insn);
        if (architecture()->isUnknown(insn) && !settings_.ignoringUnknownInsns) // case: "unknown" instruction
            goto done;

        // Give user chance to adjust basic block successors and/or pre-compute cached analysis results
        BasicBlockCallback::Results userResult;
        basicBlockCallbacks_.apply(true, BasicBlockCallback::Args(sharedFromThis(), retval, userResult));

        for (Address successorVa: basicBlockConcreteSuccessors(retval)) {
            if (successorVa!=startVa && retval->instructionExists(successorVa)) { // case: successor is inside our own block
                retval->pop();
                goto done;
            }
        }

        if (userResult.terminate == BasicBlockCallback::TERMINATE_NOW)  // case: user wants us to terminate block here
            goto done;
        if (userResult.terminate == BasicBlockCallback::TERMINATE_PRIOR) {
            retval->pop();                                              // case: user wants to terminate at prior insn
            goto done;
        }

        if (config_.basicBlockFinalInstructionVa(startVa).orElse(va+1)==va) // case: terminated by configuration
            goto done;

        if (basicBlockIsFunctionCall(retval))                           // case: bb looks like a function call
            goto done;
        BasicBlock::Successors successors = basicBlockSuccessors(retval);

        if (successors.size()!=1)                                       // case: not exactly one successor
            goto done;
        SValue::Ptr successorExpr = successors.front().expr();

        if (!successorExpr->isConcrete())                               // case: successor is indeterminate
            goto done;
        Address successorVa = successorExpr->toUnsigned().get();

        if (successorVa == startVa)                                     // case: successor is our own basic block
            goto done;

        if (findPlaceholder(successorVa)!=cfg_.vertices().end())        // case: successor is an existing block
            goto done;

        AddressUser succVaOwners = instructionExists(successorVa);
        if (succVaOwners &&                                             // case: successor is inside an existing block that
            !isSupersetUnique(startVaOwners.basicBlocks(),              //       doesn't own startVa
                              succVaOwners.basicBlocks(),
                              sortBasicBlocksByAddress)) {
            goto done;
        }

        va = successorVa;
    }
done:

    // We're terminating the basic block.  If the configuration specifies successors for this block at this instruction then
    // use them instead of what we might have already calculated.
    if (!retval->instructions().empty()) {
        Address finalInsnVa = retval->instructions().back()->get_address();
        if (config_.basicBlockFinalInstructionVa(startVa).orElse(finalInsnVa+1)==finalInsnVa) {
            retval->clearSuccessors();
            size_t nBits = instructionProvider_->instructionPointerRegister().nBits();
            std::set<Address> successorVas = config_.basicBlockSuccessorVas(startVa);
            for (Address successorVa: successorVas)
                retval->insertSuccessor(successorVa, nBits);
        }
    }

    retval->freeze();
    return retval;
}

ControlFlowGraph::VertexIterator
Partitioner::truncateBasicBlock(const ControlFlowGraph::ConstVertexIterator &placeholder, SgAsmInstruction *insn) {
    ASSERT_require(placeholder != cfg_.vertices().end());
    ASSERT_not_null(insn);
    BasicBlock::Ptr bblock = placeholder->value().bblock();
    if (bblock==nullptr) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " has no basic block for truncation");
    }
    if (insn==bblock->instructions().front())
        throw BasicBlockError(bblock, basicBlockName(bblock) + " cannot be truncated at its initial instruction");
    if (!bblock->instructionExists(insn)) {
        mlog[WARN] <<basicBlockName(bblock) <<" does not contain instruction \"" <<insn->toString() + "\""
                   <<" for truncation\n";
        return cfg_.vertices().end();
    }

    // For now we do a naive approach; this could be faster [Robb P. Matzke 2014-08-02]
    detachBasicBlock(placeholder);                      // throw away the original block
    ControlFlowGraph::VertexIterator newPlaceholder = insertPlaceholder(insn->get_address());
    BasicBlock::Ptr newBlock = discoverBasicBlock(placeholder); // rediscover original block, but terminate at newPlaceholder
    attachBasicBlock(placeholder, newBlock);            // insert new block at original placeholder and insert successor edge
    return newPlaceholder;
}

ControlFlowGraph::VertexIterator
Partitioner::insertPlaceholder(Address startVa) {
    ControlFlowGraph::VertexIterator placeholder = findPlaceholder(startVa);
    if (placeholder == cfg_.vertices().end()) {
        if (AddressUser addressUser = instructionExists(startVa)) {
            // This placeholder is in the middle of some other basic block(s), so we must truncate them.
            for (const BasicBlock::Ptr &existingBlock: addressUser.basicBlocks()) {
                ControlFlowGraph::VertexIterator conflictBlock = findPlaceholder(existingBlock->address());
                ControlFlowGraph::VertexIterator maybeCreated = truncateBasicBlock(conflictBlock, addressUser.insn());
                if (maybeCreated != cfg_.vertices().end()) {
                    placeholder = maybeCreated;
                    ASSERT_require(placeholder->value().address() == startVa);
                }
            }
        } else {
            placeholder = cfg_.insertVertex(CfgVertex(startVa));
            vertexIndex_.insert(startVa, placeholder);
            adjustPlaceholderEdges(placeholder);
            bblockAttached(placeholder);
        }
    }
    ASSERT_forbid(placeholder == cfg_.vertices().end());
    return placeholder;
}

void
Partitioner::attachBasicBlock(const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);
    ControlFlowGraph::VertexIterator placeholder = insertPlaceholder(bblock->address()); // insert or find existing
    attachBasicBlock(placeholder, bblock);
}

void
Partitioner::attachBasicBlock(const ControlFlowGraph::ConstVertexIterator &constPlaceholder, const BasicBlock::Ptr &bblock) {
    ASSERT_require(cfg_.isValidVertex(constPlaceholder));
    ASSERT_require(constPlaceholder->value().type() == V_BASIC_BLOCK);
    ASSERT_not_null(bblock);
    ControlFlowGraph::VertexIterator placeholder = cfg_.findVertex(constPlaceholder->id());

    if (placeholder->value().address() != bblock->address()) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " cannot hold " + basicBlockName(bblock));
    }
    if (placeholder->value().bblock() == bblock)
        return;                                         // nothing to do since basic block is already in the CFG
    if (placeholder->value().bblock() != nullptr) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " already holds a different basic block");
    }

    if (bblock->sourceLocation().isEmpty())
        bblock->sourceLocation(sourceLocations_(bblock->address()));

    // Adjust the basic block according to configuration information. Configuration overrides automatically-detected values.
    const BasicBlockConfiguration &c = config_.basicBlock(bblock->address());
    if (!c.comment().empty())
        bblock->comment(c.comment());
    if (!c.sourceLocation().isEmpty())
        bblock->sourceLocation(c.sourceLocation());

    bblock->freeze();

    // Are any edges marked as function calls?  If not, and this is a function call, then we'll want to convert all the normal
    // edges to function call edges.  Similarly for function return edges.
    bool hasFunctionCallEdges = false, hasFunctionReturnEdges = false, hasCallReturnEdges = false;
    for (const BasicBlock::Successor &successor: basicBlockSuccessors(bblock)) {
        switch (successor.type()) {
            case E_FUNCTION_CALL:       hasFunctionCallEdges = true;    break;
            case E_FUNCTION_RETURN:     hasFunctionReturnEdges = true;  break;
            case E_CALL_RETURN:         hasCallReturnEdges = true;      break;
            default:                                                    break;
        }
    }
    bool convertEdgesToFunctionCalls = !hasFunctionCallEdges && basicBlockIsFunctionCall(bblock);
    bool convertEdgesToFunctionReturns = !hasFunctionReturnEdges && basicBlockIsFunctionReturn(bblock);

    // Make sure placeholders exist for the concrete successors
    bool hadIndeterminate = false;
    typedef std::pair<ControlFlowGraph::VertexIterator, CfgEdge> VertexEdgePair;
    std::vector<VertexEdgePair> successors;
    for (const BasicBlock::Successor &successor: basicBlockSuccessors(bblock)) {
        EdgeType edgeType = successor.type();
        if (edgeType == E_NORMAL) {
            if (convertEdgesToFunctionCalls) {
                edgeType = E_FUNCTION_CALL;
            } else if (convertEdgesToFunctionReturns) {
                edgeType = E_FUNCTION_RETURN;
            }
        }

        CfgEdge edge(edgeType, successor.confidence());
        if (auto sucval = successor.expr()->toUnsigned()) {
            successors.push_back(VertexEdgePair(insertPlaceholder(*sucval), edge));
        } else if (!hadIndeterminate) {
            successors.push_back(VertexEdgePair(indeterminateVertex_, edge));
            hadIndeterminate = true;
        }
    }

    // Call-return edges indicate where a function call eventually returns since the CFG doesn't have such edges from function
    // return statements (return statements usually have an indeterminate successor).  We'll only add the call-return edge if
    // the user didn't already specify one and if the may-return analysis is positive.
    if (autoAddCallReturnEdges_ && !hasCallReturnEdges && basicBlockIsFunctionCall(bblock)) {
        for (const VertexEdgePair &successor: successors) {
            if (successor.second.type() == E_FUNCTION_CALL) {
                bool mayReturn = false;
                if (basicBlockOptionalMayReturn(bblock).assignTo(mayReturn)) {
                    // may-return is provably true or false
                    CfgEdge edge(E_CALL_RETURN, PROVED);
                    successors.push_back(VertexEdgePair(insertPlaceholder(bblock->fallthroughVa()), edge));
                    break;
                } else if (assumeFunctionsReturn_) {
                    // assume functions return without proving it
                    CfgEdge edge(E_CALL_RETURN, ASSUMED);
                    successors.push_back(VertexEdgePair(insertPlaceholder(bblock->fallthroughVa()), edge));
                    break;
                }
            }
        }
    }

    // Make CFG edges
    cfg_.clearOutEdges(placeholder);
    for (const VertexEdgePair &pair: successors)
        cfg_.insertEdge(placeholder, pair.first, pair.second);

    // Insert the basic block instructions into the AUM
    placeholder->value().bblock(bblock);
    for (SgAsmInstruction *insn: bblock->instructions())
        aum_.insertInstruction(insn, bblock);
    if (bblock->isEmpty())
        adjustNonexistingEdges(placeholder);

    // Insert the basic block static data. If the AUM contains an equivalent data block already, then use that one instead.
    for (const DataBlock::Ptr &dblock: bblock->dataBlocks()) {
        ASSERT_not_null(dblock);
        DataBlock::Ptr insertedDb = attachDataBlock(dblock);
        ASSERT_not_null(insertedDb);
        if (insertedDb != dblock)
            bblock->replaceOrInsertDataBlock(insertedDb);
        insertedDb->insertOwner(bblock);
    }

    if (basicBlockSemanticsAutoDrop())
        bblock->dropSemantics(sharedFromThis());

    bblockAttached(placeholder);
}

AddressIntervalSet
Partitioner::basicBlockInstructionExtent(const BasicBlock::Ptr &bblock) const {
    AddressIntervalSet retval;
    if (bblock!=nullptr) {
        for (SgAsmInstruction *insn: bblock->instructions())
            retval.insert(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
    }
    return retval;
}

AddressIntervalSet
Partitioner::basicBlockDataExtent(const BasicBlock::Ptr &bblock) const {
    AddressIntervalSet retval;
    if (bblock!=nullptr) {
        for (const DataBlock::Ptr &dblock: bblock->dataBlocks())
            retval.insert(dblock->extent());
    }
    return retval;
}

BasicBlock::Successors
Partitioner::basicBlockSuccessors(const BasicBlock::Ptr &bb, Precision::Level precision) const {
    ASSERT_not_null(bb);
    BasicBlock::Successors successors;

    if (bb->isEmpty() || bb->successors().getOptional().assignTo(successors))
        return successors;

    SgAsmInstruction *lastInsn = bb->instructions().back();

    BasicBlockSemantics sem = bb->semantics();
    BaseSemantics::State::Ptr state;
    if (settings_.ignoringUnknownInsns && architecture()->isUnknown(lastInsn)) {
        // Special case for "unknown" instructions... the successor is assumed to be the fall-through address.
        Address va = lastInsn->get_address() + lastInsn->get_size();
        size_t nBits = instructionProvider_->wordSize();
        BaseSemantics::RiscOperators::Ptr ops = newOperators();
        successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->number_(nBits, va))));
    } else if (precision > Precision::LOW && (state = sem.finalState())) {
        // Use our own semantics if we have them.
        ASSERT_not_null(sem.dispatcher);
        ASSERT_not_null(sem.operators);
        RegisterDescriptor REG_IP = instructionProvider_->instructionPointerRegister();
        std::vector<Semantics::SValue::Ptr> worklist(1, Semantics::SValue::promote(sem.operators->peekRegister(REG_IP)));
        while (!worklist.empty()) {
            Semantics::SValue::Ptr pc = worklist.back();
            worklist.pop_back();

            // Special handling for if-then-else expressions
            if (SymbolicExpression::InteriorPtr ifNode = pc->get_expression()->isInteriorNode()) {
                if (ifNode->getOperator()==SymbolicExpression::OP_ITE) {
                    Semantics::SValue::Ptr expr = Semantics::SValue::promote(sem.operators->undefined_(ifNode->nBits()));
                    expr->set_expression(ifNode->child(1));
                    worklist.push_back(expr);
                    expr = Semantics::SValue::promote(sem.operators->undefined_(ifNode->nBits()));
                    expr->set_expression(ifNode->child(2));
                    worklist.push_back(expr);
                    continue;
                }
            }

            successors.push_back(BasicBlock::Successor(pc));
        }
    } else {
        // We don't have semantics, so naively look at just the last instruction.
        bool complete = true;
        size_t nBits = instructionProvider_->wordSize();
        AddressSet successorVas = architecture()->getSuccessors(lastInsn, complete/*out*/);

        BaseSemantics::RiscOperators::Ptr ops = newOperators();
        for (Address va: successorVas.values())
            successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->number_(nBits, va))));
        if (!complete)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->undefined_(nBits))));
    }

    // We don't want parallel edges in the CFG, so remove duplicates.
    std::sort(successors.begin(), successors.end(), sortByExpression);
    for (size_t i=1; i<successors.size(); /*void*/) {
        if (successors[i-1].expr()->get_expression()->isEquivalentTo(successors[i].expr()->get_expression())) {
            ASSERT_require(successors[i-1].type() == successors[i].type());
            successors.erase(successors.begin()+i);
        } else {
            ++i;
        }
    }

    // Cache the result, but not for low-precision results since that would mean a later call for high-precision results would
    // only return low-precision results.
    if (precision > Precision::LOW)
        bb->successors() = successors;
    return successors;
}

std::set<Address>
Partitioner::basicBlockGhostSuccessors(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    std::set<Address> ghosts;

    if (bb->isEmpty() || bb->ghostSuccessors().getOptional().assignTo(ghosts))
        return ghosts;

    // Addresses of all instructions in this basic block
    Sawyer::Container::Set<Address> insnVas;
    for (SgAsmInstruction *insn: bb->instructions())
        insnVas.insert(insn->get_address());

    // Find naive per-instruction successors that are not pointing to another instruction and which aren't in the set of basic
    // block successors.  "Naive" successors are obtained by considering each insn in isolation (e.g., conditional branches
    // have two naive successors even if the predicate is opaque.
    const BasicBlock::Successors &bblockSuccessors = basicBlockSuccessors(bb);
    for (SgAsmInstruction *insn: bb->instructions()) {
        bool complete = true;
        AddressSet vas = architecture()->getSuccessors(insn, complete/*out*/);
        for (Address naiveSuccessor: vas.values()) {
            if (!insnVas.exists(naiveSuccessor)) {
                // Is naiveSuccessor also a basic block successor?
                bool found = false;
                for (const BasicBlock::Successor &bblockSuccessor: bblockSuccessors) {
                    if (bblockSuccessor.expr()->toUnsigned().isEqual(naiveSuccessor)) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    ghosts.insert(naiveSuccessor);
            }
        }
    }
    bb->ghostSuccessors() = ghosts;
    return ghosts;
}

std::vector<Address>
Partitioner::basicBlockConcreteSuccessors(const BasicBlock::Ptr &bb, bool *isComplete/*=NULL*/) const {
    ASSERT_not_null(bb);
    std::vector<Address> retval;
    if (isComplete)
        *isComplete = true;
    for (const BasicBlock::Successor &successor: basicBlockSuccessors(bb)) {
        if (auto sucval = successor.expr()->toUnsigned()) {
            retval.push_back(*sucval);
        } else if (isComplete) {
            *isComplete = false;
        }
    }
    return retval;
}

bool
Partitioner::basicBlockPopsStack(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);

    do {
        if (bb->popsStack().isCached())
            break;

        // We need instruction semantics, or return false
        BasicBlockSemantics sem = bb->undropSemantics(sharedFromThis());
        if (!sem.dispatcher) {
            bb->popsStack() = false;
            break;
        }
        ASSERT_not_null(sem.operators);

        // Get the block initial and final states
        BaseSemantics::State::Ptr state0 = sem.initialState;
        BaseSemantics::State::Ptr stateN = sem.finalState();
        if (!state0 || !stateN) {
            bb->popsStack() = false;
            break;
        }

        // Get initial and final stack pointer values
        const RegisterDescriptor REG_SP = instructionProvider_->stackPointerRegister();
        BaseSemantics::SValue::Ptr sp0 =
            state0->peekRegister(REG_SP, sem.operators->undefined_(REG_SP.nBits()), sem.operators.get());
        BaseSemantics::SValue::Ptr spN =
            stateN->peekRegister(REG_SP, sem.operators->undefined_(REG_SP.nBits()), sem.operators.get());

        // Did the basic block pop the return value from the stack?  This impossible to determine unless we assume that the stack
        // has an initial value that's not near the minimum or maximum possible value.  Therefore, we'll substitute a concrete
        // value for the stack pointer.
        SymbolicExpression::Ptr sp0ExprOrig = Semantics::SValue::promote(sp0)->get_expression();
        SymbolicExpression::Ptr sp0ExprNew = SymbolicExpression::makeIntegerConstant(REG_SP.nBits(), 0x8000); // arbitrary
        SymbolicExpression::Ptr spNExpr =
            Semantics::SValue::promote(spN)->get_expression()->substitute(sp0ExprOrig, sp0ExprNew, sem.operators->solver());

        // FIXME[Robb P Matzke 2016-11-15]: assumes stack grows down.
        // SPn > SP0 == true implies at least one byte popped.
        SymbolicExpression::Ptr cmpExpr = SymbolicExpression::makeGt(spNExpr, sp0ExprNew, sem.operators->solver());
        bb->popsStack() = cmpExpr->mustEqual(SymbolicExpression::makeBooleanConstant(true));
    } while (0);

#if 0 // [Robb Matzke 2019-01-16]: commented out to debug race
    if (sem.wasDropped)
        bb->dropSemantics(*this);
#endif
    ASSERT_require(bb->popsStack().isCached());
    return bb->popsStack().get();
}

bool
Partitioner::basicBlockIsFunctionCall(const BasicBlock::Ptr &bb, Precision::Level precision) const {
    ASSERT_not_null(bb);
    bool retval = false;

    if (bb->isEmpty() || bb->isFunctionCall().getOptional().assignTo(retval))
        return retval;                                  // already cached

    // Avoid infinite recursion
    static size_t recursionDepth = 0;
    RecursionCounter recursion(recursionDepth);
    if (recursionDepth > 2 /*arbitrary*/)
        return false;      // if we can't prove it, assume false

    SgAsmInstruction *lastInsn = bb->instructions().back();

    // Use our own semantics if we have them.
    if (precision > Precision::LOW) {
        BasicBlockSemantics sem = bb->semantics();
        if (BaseSemantics::State::Ptr state = sem.finalState()) {
            ASSERT_not_null(sem.dispatcher);
            ASSERT_not_null(sem.operators);
            Address returnVa = bb->fallthroughVa();
            const RegisterDescriptor REG_IP = instructionProvider_->instructionPointerRegister();

            // Check whether the last instruction is a CALL (or similar) instruction.
            bool isInsnCall = architecture()->isFunctionCallFast(bb->instructions(), nullptr, nullptr);

            // Check whether the basic block has the semantics of a function call.
            //
            // For stack-based calling, after the call the top of the stack will contain the address of the instruction
            // immediately following the call.  Depending on the memory state, if the stack pointer is not a concrete value
            // then reading the top of the stack might not return the same thing we just wrote there (due to trying to resolve
            // aliasing in the memory state).
            //
            // FIXME[Robb P Matzke 2016-11-15]: This only works for stack-based calling conventions.
            // Is the block fall-through address equal to the value on the top of the stack?
            bool isSemanticCall = false;
            if (!isInsnCall) {
                const RegisterDescriptor REG_SP = instructionProvider_->stackPointerRegister();
                const RegisterDescriptor REG_SS = instructionProvider_->stackSegmentRegister();
                BaseSemantics::SValue::Ptr returnExpr = sem.operators->number_(REG_IP.nBits(), returnVa);
                BaseSemantics::SValue::Ptr sp = sem.operators->peekRegister(REG_SP);
                BaseSemantics::SValue::Ptr topOfStack = sem.operators->undefined_(REG_IP.nBits());
                topOfStack = sem.operators->peekMemory(REG_SS, sp, topOfStack);
                BaseSemantics::SValue::Ptr z =
                    sem.operators->equalToZero(sem.operators->add(returnExpr,
                                                                  sem.operators->negate(topOfStack)));
                isSemanticCall = z->isTrue();
            }

            // Defintely not a function call if it neither has semantics or a call or looks like a call.
            if (!isInsnCall && !isSemanticCall) {
                bb->isFunctionCall() = false;
                return false;
            }

            // If the only successor is also the fall-through address then this isn't a function call.  This case handles code
            // that obtains the code address in position independent code. For example, x86 "A: CALL B; B: POP EAX" where A and
            // B are consecutive instruction addresses.
            BasicBlock::Successors successors = basicBlockSuccessors(bb);
            if (1 == successors.size() && successors[0].expr()->toUnsigned().isEqual(returnVa)) {
                bb->isFunctionCall() = false;
                return false;
            }

            // If all callee blocks pop the return address without returning, then this perhaps isn't a function call after
            // all.
            if (checkingCallBranch()) {
                bool allCalleesPopWithoutReturning = true;   // all callees pop return address but don't return?
                for (const BasicBlock::Successor &successor: successors) {
                    // Find callee basic block
                    if (!successor.expr() || !successor.expr()->toUnsigned()) {
                        allCalleesPopWithoutReturning = false;
                        break;
                    }
                    Address calleeVa = successor.expr()->toUnsigned().get();
                    BasicBlock::Ptr calleeBb = basicBlockExists(calleeVa);
                    if (!calleeBb)
                        calleeBb = discoverBasicBlock(calleeVa); //  could cause recursion
                    if (!calleeBb) {
                        allCalleesPopWithoutReturning = false;
                        break;
                    }

                    // If the called block is also a function return (i.e., we're calling a function which is only one block
                    // long), then of course the block will pop the return address even though it's a legitimate function.
                    if (basicBlockIsFunctionReturn(calleeBb)) {
                        allCalleesPopWithoutReturning = false;
                        break;
                    }

                    if (!basicBlockPopsStack(calleeBb)) {
                        allCalleesPopWithoutReturning = false;
                        break;
                    }

                    // Did the callee return to somewhere other than caller's return address?
                    if (BaseSemantics::State::Ptr calleeStateN = calleeBb->semantics().finalState()) {
                        BaseSemantics::SValue::Ptr ipN =
                            calleeStateN->peekRegister(REG_IP, sem.operators->undefined_(REG_IP.nBits()), sem.operators.get());
                        if (ipN->toUnsigned().isEqual(returnVa)) {
                            allCalleesPopWithoutReturning = false;
                            break;
                        }
                    }
                }
                if (allCalleesPopWithoutReturning) {
                    bb->isFunctionCall() = false;
                    return false;
                }
            }

            // This appears to be a function call
            bb->isFunctionCall() = true;
            return true;
        }
    }

    // An x86 idiom is two consecutive instructions "call next: pop ebx" where "next" is the address of the POP instruction
    // that immediately follows the CALL instruction.
    if (SgAsmX86Instruction *i1 = isSgAsmX86Instruction(lastInsn)) {
        if (i1->get_kind() == x86_call) {
            Address callFallThroughVa = i1->get_address() + i1->get_size();
            bool complete = true;
            AddressSet callSuccVas = architecture()->getSuccessors(i1, complete/*out*/);
            if (callSuccVas.size() == 1 && callSuccVas.least() == callFallThroughVa) {
                if (SgAsmX86Instruction *i2 = isSgAsmX86Instruction(discoverInstruction(callFallThroughVa))) {
                    if (i2->get_kind() == x86_pop) {
                        bb->isFunctionCall() = false;
                        return false;
                    }
                }
            }
        }
    }

    // We don't have semantics, so delegate to the SgAsmInstruction subclass.
    retval = architecture()->isFunctionCallFast(bb->instructions(), nullptr, nullptr);
    bb->isFunctionCall() = retval;
    return retval;
}

bool
Partitioner::basicBlockIsFunctionReturn(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    bool retval = false;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SAWYER_MESG(debug) <<"basicBlockIsFunctionReturn " <<bb->printableName()
                       <<" with " <<StringUtility::plural(bb->nInstructions(), "instructions") <<"\n";
    if (bb->isEmpty() || bb->isFunctionReturn().getOptional().assignTo(retval)) {
        SAWYER_MESG(debug) <<"  using cached is-function-return value: " <<(retval ? "true" : "false") <<"\n";
        return retval;
    }

    SgAsmInstruction *lastInsn = bb->instructions().back();
    SAWYER_MESG(debug) <<"  last instruction of block: " <<lastInsn->toString() <<"\n";

    // Use our own semantics if we have them.
    boost::logic::tribool isReturn;
    if (isSgAsmPowerpcInstruction(lastInsn)) {
        isReturn = ModulesPowerpc::isFunctionReturn(sharedFromThis(), bb);
    } else {
        isReturn = Modules::isStackBasedReturn(sharedFromThis(), bb);
    }
    if (isReturn || !isReturn) {
        bool retval = isReturn ? true : false;
        bb->isFunctionReturn() = retval;
        return retval;
    }

    // No semantics, so delegate to SgAsmInstruction subclasses
    SAWYER_MESG(debug) <<"  block does not have semantic information\n";
    retval = architecture()->isFunctionReturnFast(bb->instructions());
    bb->isFunctionReturn() = retval;
    SAWYER_MESG(debug) <<"  returning " <<(retval ? "true" : "false") <<"\n";
    return retval;
}

BaseSemantics::SValue::Ptr
Partitioner::basicBlockStackDeltaIn(const BasicBlock::Ptr &bb, const Function::Ptr &function) const {
    ASSERT_not_null(bb);
    ASSERT_not_null(function);

    (void) functionStackDelta(function);
    return function->stackDeltaAnalysis().basicBlockInputStackDeltaWrtFunction(bb->address());
}

BaseSemantics::SValue::Ptr
Partitioner::basicBlockStackDeltaOut(const BasicBlock::Ptr &bb, const Function::Ptr &function) const {
    ASSERT_not_null(bb);
    ASSERT_not_null(function);

    (void) functionStackDelta(function);
    return function->stackDeltaAnalysis().basicBlockOutputStackDeltaWrtFunction(bb->address());
}

size_t
Partitioner::stackDeltaInterproceduralLimit() const {
    return stackDeltaInterproceduralLimit_;
}

void
Partitioner::stackDeltaInterproceduralLimit(size_t n) {
    stackDeltaInterproceduralLimit_ = std::max(size_t(1), n);
}

ControlFlowGraph::ConstVertexIterator
Partitioner::instructionVertex(Address insnVa) const {
    if (BasicBlock::Ptr bblock = basicBlockContainingInstruction(insnVa))
        return findPlaceholder(bblock->address());
    return cfg().vertices().end();
}

std::vector<SgAsmInstruction*>
Partitioner::instructionsOverlapping(const AddressInterval &interval) const {
    return aum_.overlapping(interval, AddressUsers::selectBasicBlocks).instructions();
}

std::vector<BasicBlock::Ptr>
Partitioner::basicBlocks() const {
    std::vector<BasicBlock::Ptr> bblocks;
    for (const ControlFlowGraph::VertexValue &vertex: cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bblock = vertex.bblock())
                bblocks.push_back(bblock);
        }
    }
    std::sort(bblocks.begin(), bblocks.end(), sortBasicBlocksByAddress);
    return bblocks;
}

std::vector<BasicBlock::Ptr>
Partitioner::basicBlocksOverlapping(const AddressInterval &interval) const {
    return aum_.overlapping(interval, AddressUsers::selectBasicBlocks).instructionOwners();
}

BasicBlock::Ptr
Partitioner::basicBlockContainingInstruction(Address insnVa) const {
    std::vector<BasicBlock::Ptr> bblocks = basicBlocksOverlapping(insnVa);
    for (const BasicBlock::Ptr &bblock: bblocks) {
        for (SgAsmInstruction *insn: bblock->instructions()) {
            if (insn->get_address() == insnVa)
                return bblock;
        }
    }
    return BasicBlock::Ptr();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Partitioner::nDataBlocks() const {
    return dataBlocks().size();
}

std::vector<DataBlock::Ptr>
Partitioner::dataBlocks() const {
    return dataBlocksOverlapping(aum_.hull());
}

DataBlock::Ptr
Partitioner::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (nullptr == dblock)
        return DataBlock::Ptr();

    // If this data block has attached owners, then this data block must already exist in the AUM.
    if (dblock->nAttachedOwners() > 0) {
        ASSERT_require(aum_.dataBlockExists(dblock) == dblock);
        return dblock;
    }

    // If any data block has the exact same extent as the specified data block, then return that already-existing, equivalent
    // data block.
    return aum_.dataBlockExists(dblock);
}

// Attach data block without any new owners.
DataBlock::Ptr
Partitioner::attachDataBlock(const DataBlock::Ptr &toInsert) {
    ASSERT_not_null(toInsert);
    DataBlock::Ptr inserted = aum_.insertDataBlock(toInsert).dataBlock();
    ASSERT_not_null(inserted);
    inserted->freeze();
    return inserted;
}

// Detach data block if it has no owners
void
Partitioner::detachDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock != nullptr && dblock->isFrozen()) {
        if (dblock->nAttachedOwners() > 0) {
            throw DataBlockError(dblock, dataBlockName(dblock) + " cannot be detached because it has " +
                                 StringUtility::plural(dblock->nAttachedOwners(), "basic block and/or function owners"));
        }
        DataBlock::Ptr erased = aum_.eraseDataBlock(dblock);
        ASSERT_always_require(erased == dblock);
        dblock->thaw();
    }
}

std::vector<DataBlock::Ptr>
Partitioner::dataBlocksOverlapping(const AddressInterval &interval) const {
    return aum_.overlapping(interval, AddressUsers::selectDataBlocks).dataBlocks();
}

std::vector<DataBlock::Ptr>
Partitioner::dataBlocksSpanning(const AddressInterval &interval) const {
    return aum_.spanning(interval, AddressUsers::selectDataBlocks).dataBlocks();
}

std::vector<DataBlock::Ptr>
Partitioner::dataBlocksContainedIn(const AddressInterval &interval) const {
    std::vector<DataBlock::Ptr> retval;
    std::vector<DataBlock::Ptr> overlapping = dataBlocksOverlapping(interval);
    for (const DataBlock::Ptr &db: overlapping) {
        if (interval.contains(db->extent()))
            retval.push_back(db);
    }
    return retval;
}

AddressInterval
Partitioner::dataBlockExtent(const DataBlock::Ptr &dblock) const {
    AddressInterval extent;
    if (dblock!=nullptr)
        extent = dblock->extent();
    return extent;
}

DataBlock::Ptr
Partitioner::findBestDataBlock(const AddressInterval &interval) const {
    DataBlock::Ptr existing;
    if (!interval.isEmpty()) {
        std::vector<DataBlock::Ptr> found = dataBlocksSpanning(interval);
        for (const DataBlock::Ptr &dblock: boost::adaptors::reverse(found)) { // choose the smallest one (highest address if tied)
            if (existing==nullptr || dblock->size() < existing->size())
                existing = dblock;
        }
        if (existing==nullptr) {
            found = dataBlocksOverlapping(AddressInterval(interval.least()));
            for (const DataBlock::Ptr &dblock: boost::adaptors::reverse(found)) { // choose the largest one (highest address if tied)
                if (existing==nullptr || dblock->size() > existing->size())
                    existing = dblock;
            }
        }
    }
    return existing;
}

// attachDataBlockToFunction and attachDataBlockToBasicBlock should have very similar semantics.
DataBlock::Ptr
Partitioner::attachDataBlockToFunction(const DataBlock::Ptr &dblock, const Function::Ptr &function) {
    ASSERT_not_null(function);                          // the function can be attached or detached
    ASSERT_not_null(dblock);                            // the data block can be attached or detached
    DataBlock::Ptr canonicalDataBlock;

    if (DataBlock::Ptr existingDataBlock = function->dataBlockExists(dblock)) {
        // Either this very dblock is already attached to the function, or some equivalent data block is already attached to
        // the function.
        canonicalDataBlock = existingDataBlock;

    } else if (functionExists(function) == function) {
        // The specified data block (or equivalent) is not attached to the specified function yet, but the specified function
        // is already in the CFG/AUM. The AUM might already contain an equivalent data block that we should use instead, and if
        // so, that's the one we'll return.
        canonicalDataBlock = attachDataBlock(dblock);
        function->replaceOrInsertDataBlock(canonicalDataBlock);
        canonicalDataBlock->insertOwner(function);

    } else if (DataBlock::Ptr existingDataBlock = function->dataBlockExists(dblock)) {
        // The function is not attached to the CFG/AUM, therefore we don't need to attach the specified dblock to the
        // AUM. Since the function already has an equivalent data block, return the one that exists.
        canonicalDataBlock = existingDataBlock;

    } else {
        // The function is not attached to the CFG/AUM and doesn't contain the specified dblock or an equivalent data block.
        canonicalDataBlock = dblock;
        function->insertDataBlock(dblock);
    }

    ASSERT_not_null(canonicalDataBlock);
    return canonicalDataBlock;                          // an existing equivalent data block, or the specified dblock
}

// attachDataBlockToFunction and attachDataBlockToBasicBlock should have very similar semantics.
DataBlock::Ptr
Partitioner::attachDataBlockToBasicBlock(const DataBlock::Ptr &dblock, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);                            // the basic block can be attached or detached
    ASSERT_not_null(dblock);                            // the data block can be attached or detached
    DataBlock::Ptr canonicalDataBlock;

    if (DataBlock::Ptr existingDataBlock = bblock->dataBlockExists(dblock)) {
        // Either this dblock is already attached to the basic block, or some equivalent data block is already attached to the
        // basic block.
        canonicalDataBlock = existingDataBlock;

    } else if (basicBlockExists(bblock) == bblock) {
        // The specified data block (or equivalent) is not attached to the specified basic block yet, but the specified basic
        // block is already in the CFG/AUM. The AUM might already contain an equivalent data block that we should use instead,
        // and if so, that's the one we'll return.
        canonicalDataBlock = attachDataBlock(dblock);
        bblock->replaceOrInsertDataBlock(canonicalDataBlock);
        canonicalDataBlock->insertOwner(bblock);

    } else if (DataBlock::Ptr existingDataBlock = bblock->dataBlockExists(dblock)) {
        // The basic block is not attached to the CFG/AUM, therefore we don't need to attach the specified dblock to the
        // AUM. Since the basic block already has an equivalent data block, return the one that exists.
        canonicalDataBlock = existingDataBlock;

    } else {
        // The basic block is not attached to the CFG/AUM and doesn't contain the specified dblock or an equivalent data block.
        canonicalDataBlock = dblock;
        bblock->insertDataBlock(dblock);
    }

    ASSERT_not_null(canonicalDataBlock);
    return canonicalDataBlock;                          // an existing equivalent data block, or the specified dblock
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Partitioner::nFunctions() const {
    return functions_.size();
}

Function::Ptr
Partitioner::functionExists(Address entryVa) const {
    return functions_.getOptional(entryVa).orDefault();
}

Function::Ptr
Partitioner::functionExists(const BasicBlock::Ptr &entryBlock) const {
    if (entryBlock == nullptr)
        return Function::Ptr();
    return functionExists(entryBlock->address());
}

Function::Ptr
Partitioner::functionExists(const Function::Ptr &function) const {
    if (function != nullptr) {
        Function::Ptr found = functionExists(function->address());
        if (found == function)
            return function;
    }
    return Function::Ptr();
}

std::vector<Function::Ptr>
Partitioner::functionsOwningBasicBlock(const ControlFlowGraph::Vertex &vertex, bool doSort) const {
    std::vector<Function::Ptr> retval;
    for (const Function::Ptr &function: vertex.value().owningFunctions().values())
        retval.push_back(function);
    if (doSort)
        std::sort(retval.begin(), retval.end(), sortFunctionsByAddress);
    return retval;
}

std::vector<Function::Ptr>
Partitioner::functionsOwningBasicBlock(const ControlFlowGraph::ConstVertexIterator &placeholder, bool doSort) const {
    if (!cfg_.isValidVertex(placeholder) || placeholder->value().type() != V_BASIC_BLOCK)
        return std::vector<Function::Ptr>();
    return functionsOwningBasicBlock(*placeholder, doSort);
}

std::vector<Function::Ptr>
Partitioner::functionsOwningBasicBlock(Address bblockVa, bool doSort) const {
    ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(bblockVa);
    return functionsOwningBasicBlock(placeholder, doSort);
}

std::vector<Function::Ptr>
Partitioner::functionsOwningBasicBlock(const BasicBlock::Ptr &bblock, bool doSort) const {
    if (bblock==nullptr)
        return std::vector<Function::Ptr>();
    return functionsOwningBasicBlock(bblock->address(), doSort);
}

// We have a number of choices for the algorithm:
//  (1) iterate over all functions, compute each function's extent and compare it to the interval.  Probably not too fast since
//      most functions would probably be excluded (intervals are often small, even single addresses), and also not fast because
//      it computes the full function extent even when only part of it could cause the function to be selected.
//  (2) iterate over all functions, descending into basic blocks and data blocks.  This is faster because we don't have to
//      compute a full function extent and we can short circuit once we find an overlap.  But it requires looking up each basic
//      block by address, which will be O(n log(n)) in total.
//  (3) Use the fact that the interval is probably small and therefore the list of basic blocks and data blocks that overlap
//      with it is small, and that the list can be returned quite quickly from the AUM.  For each instruction and data block
//      returned by the AUM, look at its function ownership list and merge it into the return value.  This is the approach we
//      take here.
std::vector<Function::Ptr>
Partitioner::functionsOverlapping(const AddressInterval &interval) const {
    std::vector<Function::Ptr> functions;

    AddressUsers overlapping = aum_.overlapping(interval);
    for (const AddressUser &user: overlapping.addressUsers()) {
        if (user.insn()) {
            for (const BasicBlock::Ptr &bb: user.basicBlocks()) {
                ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(bb->address());
                ASSERT_require(placeholder != cfg_.vertices().end());
                ASSERT_require(placeholder->value().bblock() == bb);
                for (const Function::Ptr &function: placeholder->value().owningFunctions().values())
                    insertUnique(functions, function, sortFunctionsByAddress);
            }
        } else {
            ASSERT_not_null(user.dataBlock());
            for (const Function::Ptr &function: user.dataBlock()->attachedFunctionOwners())
                insertUnique(functions, function, sortFunctionsByAddress);
        }
    }
    return functions;
}

std::vector<Function::Ptr>
Partitioner::functionsSpanning(const AddressInterval &interval) const {
    std::vector<Function::Ptr> retval = functionsOverlapping(interval);
    std::vector<Function::Ptr>::iterator iter = retval.begin();
    while (iter != retval.end()) {
        if (functionExtent(*iter).contains(interval)) {
            ++iter;
        } else {
            iter = retval.erase(iter);
        }
    }
    return retval;
}

std::vector<Function::Ptr>
Partitioner::functionsContainedIn(const AddressInterval &interval) const {
    std::vector<Function::Ptr> retval = functionsOverlapping(interval);
    AddressIntervalSet addressSet;
    addressSet.insert(interval);
    std::vector<Function::Ptr>::iterator iter = retval.begin();
    while (iter != retval.end()) {
        if (addressSet.contains(functionExtent(*iter))) {
            ++iter;
        } else {
            iter = retval.erase(iter);
        }
    }
    return retval;
}

void
Partitioner::functionBasicBlockExtent(const Function::Ptr &function, AddressIntervalSet &retval /*in,out*/) const {
    ASSERT_not_null(function);
    for (Address bblockVa: function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(bblockVa);
        if (placeholder != cfg_.vertices().end()) {
            if (BasicBlock::Ptr bblock = placeholder->value().bblock()) {
                retval.insertMultiple(basicBlockInstructionExtent(bblock));
            } else {
                retval.insert(AddressInterval(bblockVa)); // all we know is the placeholder address
            }
        }
    }
}

AddressIntervalSet
Partitioner::functionBasicBlockExtent(const Function::Ptr &function) const {
    AddressIntervalSet retval;
    functionBasicBlockExtent(function, retval);
    return retval;
}

void
Partitioner::functionDataBlockExtent(const Function::Ptr &function, AddressIntervalSet &retval /*in,out*/) const {
    for (const DataBlock::Ptr &dblock: function->dataBlocks())
        retval.insert(dataBlockExtent(dblock));
}

AddressIntervalSet
Partitioner::functionDataBlockExtent(const Function::Ptr &function) const {
    AddressIntervalSet retval;
    functionDataBlockExtent(function, retval);
    return retval;
}

void
Partitioner::functionExtent(const Function::Ptr &function, AddressIntervalSet &retval /*in,out*/) const {
    functionBasicBlockExtent(function, retval);
    functionDataBlockExtent(function, retval);
}

AddressIntervalSet
Partitioner::functionExtent(const Function::Ptr &function) const {
    AddressIntervalSet retval;
    functionExtent(function, retval);
    return retval;
}


DataBlock::Ptr
Partitioner::matchFunctionPadding(const Function::Ptr &function) {
    ASSERT_not_null(function);
    Address anchor = function->address();
    for (const FunctionPaddingMatcher::Ptr &matcher: functionPaddingMatchers_) {
        Address paddingVa = matcher->match(sharedFromThis(), anchor);
        if (paddingVa < anchor) {
            DataBlock::Ptr paddingBlock = DataBlock::instanceBytes(paddingVa, anchor - paddingVa);
            paddingBlock->comment("function padding");
            return attachDataBlockToFunction(paddingBlock, function);
        }
    }
    return DataBlock::Ptr();
}

size_t
Partitioner::attachFunctions(const Functions &functions) {
    size_t nNewBlocks = 0;
    for (const Function::Ptr &function: functions.values())
        nNewBlocks += attachFunction(function);
    return nNewBlocks;
}

size_t
Partitioner::attachFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);

    size_t nNewBlocks = 0;
    Function::Ptr exists;
    if (functions_.getOptional(function->address()).assignTo(exists)) {
        if (exists != function)
            throw FunctionError(function, functionName(function) + " is already attached with a different function pointer");
        ASSERT_require(function->isFrozen());
    } else {
        // Give the function a name and comment and make other adjustments according to user configuration files.
        const FunctionConfiguration &c = config_.function(function->address());
        if (!c.name().empty())
            function->name(c.name());                   // forced name from configuration
        if (function->name().empty())
            function->name(c.defaultName());            // default name if function has none
        if (function->name().empty())
            function->name(addressName(function->address())); // use address name if nothing else
        if (!c.comment().empty())
            function->comment(c.comment());
        if (!c.sourceLocation().isEmpty()) {
            function->sourceLocation(c.sourceLocation());
        } else if (function->sourceLocation().isEmpty()) {
            function->sourceLocation(sourceLocations_(function->address()));
        }

        // Insert function into the table, and make sure all its basic blocks see that they're owned by the function.
        functions_.insert(function->address(), function);
        nNewBlocks = attachFunctionBasicBlocks(function);

        // Attach function data blocks. If the AUM contains an equivalent data block already, then use that one instead.
        for (const DataBlock::Ptr &dblock: function->dataBlocks()) {
            ASSERT_not_null(dblock);
            DataBlock::Ptr insertedDb = attachDataBlock(dblock);
            ASSERT_not_null(insertedDb);
            if (insertedDb != dblock)
                function->replaceOrInsertDataBlock(insertedDb);
            insertedDb->insertOwner(function);
        }

        // Prevent the function connectivity from changing while the function is in the CFG.  Non-frozen functions
        // can have basic blocks added and erased willy nilly because the basic blocks don't need to know that they're owned by
        // a function.  But we can't have that when the function is one that's part of the CFG.
        function->freeze();
    }
    return nNewBlocks;
}

void
Partitioner::fixInterFunctionEdges() {
    for (ControlFlowGraph::Edge &edge: cfg_.edges())
        fixInterFunctionEdge(cfg_.findEdge(edge.id()));
}

void
Partitioner::fixInterFunctionEdge(const ControlFlowGraph::ConstEdgeIterator &constEdge) {
    ASSERT_require(cfg_.isValidEdge(constEdge));
    ControlFlowGraph::EdgeIterator edge = cfg_.findEdge(constEdge->id());
    if (edge->value().type() != E_NORMAL)
        return;

    FunctionSet callers, callees;
    if (edge->source()->value().type() == V_BASIC_BLOCK)
        callers = edge->source()->value().owningFunctions();
    if (edge->target()->value().type() == V_BASIC_BLOCK)
        callees = edge->target()->value().owningFunctions();


    bool isIntraEdge = !(callers & callees).isEmpty();
    bool isCallEdge = basicBlockIsFunctionCall(edge->source()->value().bblock());

    bool isCallerThunk = false;
    for (const Function::Ptr &function: callers.values()) {
        if (functionIsThunk(function)) {
            isCallerThunk = true;
            break;
        }
    }

    if (isCallerThunk) {
        if (isCallEdge) {
            SAWYER_MESG(mlog[WARN]) <<"edge " <<edgeName(edge) <<" is both a call and a thunk transfer (assuming call)\n";
            edge->value() = ControlFlowGraph::EdgeValue(E_FUNCTION_CALL);
        } else {
            edge->value() = ControlFlowGraph::EdgeValue(E_FUNCTION_XFER);
        }
    } else if (isCallEdge) {
        if (isIntraEdge) {
            // This is an intra-function recursive call (with stack frame)
            edge->value() = ControlFlowGraph::EdgeValue(E_FUNCTION_CALL);
        } else {
            edge->value() = ControlFlowGraph::EdgeValue(E_FUNCTION_CALL);
        }
    }
}

Function::Ptr
Partitioner::attachOrMergeFunction(const Function::Ptr &newFunction) {
    ASSERT_not_null(newFunction);

    // If this function is already in the CFG then we have nothing to do.
    if (newFunction->isFrozen())
        return newFunction;

    // If there is not some other function with the same entry address, then degenerate to a plain attach.
    Function::Ptr existingFunction = functionExists(newFunction->address());
    if (existingFunction == nullptr) {
        attachFunction(newFunction);
        return newFunction;
    }

    // Perhaps use this new function's name.  If the names are the same except for the "@plt" part then use the version with
    // the "@plt".
    if (existingFunction->name().empty()) {
        existingFunction->name(newFunction->name());
    } else {
        size_t atSign = newFunction->name().find_last_of('@');
        if (atSign != std::string::npos && existingFunction->name()==newFunction->name().substr(0, atSign))
            existingFunction->name(newFunction->name());
    }

    // If the new function has basic blocks or data blocks that aren't in the existing function, then update the existing
    // function.
    bool needsUpdate = false;
    if (existingFunction->basicBlockAddresses().size() != newFunction->basicBlockAddresses().size()) {
        needsUpdate = true;
    } else {
        const std::set<Address> &s1 = newFunction->basicBlockAddresses();
        const std::set<Address> &s2 = existingFunction->basicBlockAddresses();
        if (!std::equal(s1.begin(), s1.end(), s2.begin()))
            needsUpdate = true;
    }
    if (!needsUpdate) {
        if (existingFunction->dataBlocks().size() != newFunction->dataBlocks().size()) {
            needsUpdate = true;
        } else {
            const std::vector<DataBlock::Ptr> &v1 = newFunction->dataBlocks();
            const std::vector<DataBlock::Ptr> &v2 = existingFunction->dataBlocks();
            if (!std::equal(v1.begin(), v1.end(), v2.begin()))
                needsUpdate = true;
        }
    }

    if (needsUpdate) {
        detachFunction(existingFunction);

        // Add this function's basic blocks to the existing function.
        for (Address bblockVa: newFunction->basicBlockAddresses())
            existingFunction->insertBasicBlock(bblockVa);
        attachFunctionBasicBlocks(existingFunction);

        // Add this function's data blocks to the existing function. They well be attached to the AUM shortly when we reattach
        // the function.
        for (const DataBlock::Ptr &dblock: newFunction->dataBlocks())
            existingFunction->insertDataBlock(dblock);

        attachFunction(existingFunction);
    }

    return existingFunction;
}

size_t
Partitioner::attachFunctionBasicBlocks(const Functions &functions) {
    size_t nNewBlocks = 0;
    for (const Function::Ptr &function: functions.values())
        nNewBlocks += attachFunctionBasicBlocks(function);
    return nNewBlocks;
}

size_t
Partitioner::attachFunctionBasicBlocks(const Function::Ptr &function) {
    ASSERT_not_null(function);
    size_t nNewBlocks = 0;
    bool functionExists = functions_.exists(function->address());
    for (Address blockVa: function->basicBlockAddresses()) {
        ControlFlowGraph::VertexIterator placeholder = findPlaceholder(blockVa);
        if (placeholder == cfg_.vertices().end()) {
            placeholder = insertPlaceholder(blockVa);
            ++nNewBlocks;
        }
        if (functionExists)
            placeholder->value().insertOwningFunction(function);
    }
    return nNewBlocks;
}

void
Partitioner::detachFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);
    if (functionExists(function->address()) != function)
        return;                                         // already detached

    // Unlink basic block ownership, but do not detach basic blocks from CFG/AUM
    for (Address blockVa: function->basicBlockAddresses()) {
        ControlFlowGraph::VertexIterator placeholder = findPlaceholder(blockVa);
        ASSERT_require(placeholder != cfg_.vertices().end());
        ASSERT_require(placeholder->value().type() == V_BASIC_BLOCK);
        ASSERT_require(placeholder->value().isOwningFunction(function));
        placeholder->value().eraseOwningFunction(function);
    }

    // Unlink data block ownership, but do not detach data blocks from CFG/AUM unless ownership count hits zero.
    for (const DataBlock::Ptr &dblock: function->dataBlocks()) {
        dblock->eraseOwner(function);
        if (dblock->nAttachedOwners() == 0)
            detachDataBlock(dblock);
    }

    // Unlink the function itself
    functions_.erase(function->address());
    function->thaw();
}

std::vector<Function::Ptr> 
Partitioner::entryFunctions() {
    std::vector<Function::Ptr> funs;
    for (const auto & func : functions()) {
        if (func->reasons() & SgAsmFunction::FunctionReason::FUNC_ENTRY_POINT)
            funs.push_back(func);
    }

    return funs;
}

const CallingConvention::Analysis&
Partitioner::functionCallingConvention(const Function::Ptr &function) const {
    return functionCallingConvention(function, CallingConvention::Definition::Ptr());
}

const CallingConvention::Analysis&
Partitioner::functionCallingConvention(const Function::Ptr &function,
                                       const CallingConvention::Definition::Ptr &dfltCc/*=NULL*/) const {
    ASSERT_not_null(function);
    if (!function->callingConventionAnalysis().hasResults()) {
        function->callingConventionDefinition(CallingConvention::Definition::Ptr());
        BaseSemantics::RiscOperators::Ptr ops = newOperators(MAP_BASED_MEMORY); // map works better for calling convention
        function->callingConventionAnalysis() = CallingConvention::Analysis(newDispatcher(ops));
        function->callingConventionAnalysis().defaultCallingConvention(dfltCc);
        function->callingConventionAnalysis().analyzeFunction(sharedFromThis(), function);
    }
    return function->callingConventionAnalysis();
}

CallingConvention::Dictionary
Partitioner::functionCallingConventionDefinitions(const Function::Ptr &function) const {
    return functionCallingConventionDefinitions(function, CallingConvention::Definition::Ptr());
}

CallingConvention::Dictionary
Partitioner::functionCallingConventionDefinitions(const Function::Ptr &function,
                                                  const CallingConvention::Definition::Ptr &dfltCc/*=NULL*/) const {
    SAWYER_MESG(CallingConvention::mlog[DEBUG]) <<"calling convention analysis for " <<function->printableName() <<"\n";
    const CallingConvention::Analysis &ccAnalysis = functionCallingConvention(function, dfltCc);
    const CallingConvention::Dictionary &archConventions = instructionProvider().callingConventions();
    return ccAnalysis.match(archConventions);
}

// Worker function for analyzing the calling convention of one function.
struct CallingConventionWorker {
    const Partitioner &partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    CallingConvention::Definition::Ptr dfltCc;

    CallingConventionWorker(const Partitioner &partitioner, Sawyer::ProgressBar<size_t> &progress,
                            const CallingConvention::Definition::Ptr dfltCc)
        : partitioner(partitioner), progress(progress), dfltCc(dfltCc) {}

    void operator()(size_t /*workId*/, const Function::Ptr &function) {
        Sawyer::Stopwatch t;
        partitioner.functionCallingConvention(function, dfltCc);

        // Show some results. We're using Rose::BinaryAnalysis::CallingConvention::mlog[TRACE] for the messages, so the mutex
        // here doesn't really protect it. However, since that analysis doesn't produce much output on that stream, this mutex
        // helps keep the output lines separated from one another where there's lots of worker threads, especially when they're
        // all first starting up.
        if (CallingConvention::mlog[TRACE]) {
            static boost::mutex mutex;
            boost::lock_guard<boost::mutex> lock(mutex);
            Sawyer::Message::Stream trace(CallingConvention::mlog[TRACE]);
            trace <<"calling-convention for " <<function->printableName() <<" took " <<t <<"\n";
        }

        // Update progress reports
        ++progress;
        partitioner.updateProgress("call-conv", progress.ratio());
    }
};

void
Partitioner::allFunctionCallingConvention() const {
    allFunctionCallingConvention(CallingConvention::Definition::Ptr());
}

void
Partitioner::allFunctionCallingConvention(const CallingConvention::Definition::Ptr &dfltCc/*=NULL*/) const {
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    FunctionCallGraph::Graph cg = functionCallGraph(AllowParallelEdges::NO).graph();
    Sawyer::Container::Algorithm::graphBreakCycles(cg);
    Sawyer::ProgressBar<size_t> progress(cg.nVertices(), mlog[MARCH], "call-conv analysis");
    progress.suffix(" functions");
    Sawyer::Message::FacilitiesGuard guard;
    if (nThreads != 1)                                  // lots of threads doing progress reports won't look too good!
        Rose::BinaryAnalysis::CallingConvention::mlog[MARCH].disable();
    Sawyer::workInParallel(cg, nThreads, CallingConventionWorker(*this, progress, dfltCc));
}

void
Partitioner::allFunctionCallingConventionDefinition() const {
    allFunctionCallingConventionDefinition(CallingConvention::Definition::Ptr());
}

void
Partitioner::allFunctionCallingConventionDefinition(const CallingConvention::Definition::Ptr &dfltCc/*=NULL*/) const {
    allFunctionCallingConvention(dfltCc);

    // Compute the histogram for calling convention definitions.
    typedef Sawyer::Container::Map<std::string, size_t> Histogram;
    Histogram histogram;
    Sawyer::Container::Map<Address, CallingConvention::Dictionary> allMatches;
    for (const Function::Ptr &function: functions()) {
        CallingConvention::Dictionary functionCcDefs = functionCallingConventionDefinitions(function, dfltCc);
        allMatches.insert(function->address(), functionCcDefs);
        for (const CallingConvention::Definition::Ptr &ccdef: functionCcDefs)
            ++histogram.insertMaybe(ccdef->name(), 0);
    }

    // For each function, choose the calling convention definition with the highest frequencey in the histogram.
    for (const Function::Ptr &function: functions()) {
        CallingConvention::Dictionary &functionCcDefs = allMatches[function->address()];
        CallingConvention::Definition::Ptr bestCcDef = dfltCc;
        if (!functionCcDefs.empty()) {
            bestCcDef = functionCcDefs[0];
            for (size_t i=1; i<functionCcDefs.size(); ++i) {
                if (histogram[functionCcDefs[i]->name()] > histogram[bestCcDef->name()])
                    bestCcDef = functionCcDefs[i];
            }
        }
        function->callingConventionDefinition(bestCcDef);
    }
}

std::vector<Function::Ptr>
Partitioner::functions() const {
    std::vector<Function::Ptr> functions;
    functions.reserve(functions_.size());
    for (const Function::Ptr &function: functions_.values())
        functions.push_back(function);
    ASSERT_require(isSorted(functions, sortFunctionsByAddress, true));
    return functions;
}

std::vector<Function::Ptr>
Partitioner::discoverCalledFunctions() const {
    std::vector<Function::Ptr> functions;
    for (const ControlFlowGraph::Vertex &vertex: cfg_.vertices()) {
        if (vertex.value().type() == V_BASIC_BLOCK && !functions_.exists(vertex.value().address())) {
            for (const ControlFlowGraph::Edge &edge: vertex.inEdges()) {
                if (edge.value().type() == E_FUNCTION_CALL || edge.value().type() == E_FUNCTION_XFER) {
                    Address entryVa = vertex.value().address();
                    Function::Ptr function = Function::instance(entryVa, SgAsmFunction::FUNC_CALL_TARGET);
                    function->reasonComment("called along CFG edge " + edgeName(edge));
                    insertUnique(functions, function, sortFunctionsByAddress);
                    break;
                }
            }
        }
    }
    return functions;
}

std::vector<Function::Ptr>
Partitioner::discoverFunctionEntryVertices() const {
    std::vector<Function::Ptr> functions = discoverCalledFunctions();
    for (const Function::Ptr &knownFunction: functions_.values())
        insertUnique(functions, knownFunction, sortFunctionsByAddress);
    return functions;
}

Sawyer::Optional<Partitioner::Thunk>
Partitioner::functionIsThunk(const Function::Ptr &function) const {
    if (function==nullptr || 0==(function->reasons() & SgAsmFunction::FUNC_THUNK) || function->nBasicBlocks()!=1)
        return Sawyer::Nothing();

    // Find the basic block for the thunk
    BasicBlock::Ptr bblock = basicBlockExists(function->address());
    if (!bblock)
        bblock = discoverBasicBlock(function->address());
    if (!bblock)
        return Sawyer::Nothing();

    // Basic block should have only one successor, which must be concrete
    BasicBlock::Successors succs = basicBlockSuccessors(bblock);
    if (succs.size()!=1)
        return Sawyer::Nothing();
    std::vector<Address> concreteSuccessors = basicBlockConcreteSuccessors(bblock);
    if (concreteSuccessors.size()!=1)
        return Sawyer::Nothing();

    // Make sure the successor is of type E_FUNCTION_XFER
    if (succs[0].type() != E_FUNCTION_XFER) {
        succs[0] = BasicBlock::Successor(succs[0].expr(), E_FUNCTION_XFER);
        bblock->successors_.set(succs);                 // okay even if bblock is locked since we only change edge type
    }

    return Thunk(bblock, concreteSuccessors.front());
}

void
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    if (function->isFrozen())
        throw FunctionError(function, functionName(function) +
                            " is frozen or attached to CFG/AUM when discovering basic blocks");

    Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"discoverFunctionBlocks(" <<functionName(function) <<")\n";

    // Thunks are handled specially. They only ever contain one basic block. As a side effect, the thunk's outgoing edge is
    // changed to type E_FUNCTION_XFER.
    if (functionIsThunk(function))
        return;

    typedef Sawyer::Container::Map<size_t /*vertexId*/, Function::Ownership> VertexOwnership;
    VertexOwnership ownership;                          // contains only OWN_EXPLICIT and OWN_PROVISIONAL entries

    // Find the vertices that this function already owns
    for (Address startVa: function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(startVa);
        if (placeholder == cfg_.vertices().end()) {
            throw Exception("block " + StringUtility::addrToString(startVa) + " of " + functionName(function) +
                            " must exist in the CFG");
        }
        ownership.insert(placeholder->id(), Function::OWN_EXPLICIT);
        SAWYER_MESG(debug) <<"  explicitly owns vertex " <<placeholder->id() <<"\n";
    }

    // Find all unowned vertices that we can reach from the previously owned vertices by following edges in any direction and
    // not crossing edges that lead to the entry of another function (or any function call or function transfer edge) or edges
    // that are function returns.
    Sawyer::Container::Stack<size_t> worklist(ownership.keys());
    while (!worklist.isEmpty()) {
        const ControlFlowGraph::Vertex &vertex = *cfg_.findVertex(worklist.pop());

        // Find vertex neighbors that could be in the same function.  The neighbors are sorted by ID instead of vertex iterator
        // because sorting by ID is reproducible across runs, but vertex comparisons are based on addresses, which are not
        // reproducible.
        Sawyer::Container::Map<size_t, ControlFlowGraph::ConstVertexIterator> neighbors;
        for (const ControlFlowGraph::Edge &edge: vertex.outEdges()) {
            if (edge.value().type() != E_FUNCTION_CALL &&
                edge.value().type() != E_FUNCTION_XFER &&
                edge.value().type() != E_FUNCTION_RETURN && !edge.isSelfEdge() &&
                !edge.target()->value().isEntryBlock()) {
                SAWYER_MESG(debug) <<"  following edge " <<edgeName(edge) <<"\n";
                neighbors.insert(edge.target()->id(), edge.target());
            }
        }
        for (const ControlFlowGraph::Edge &edge: vertex.inEdges()) {
            if (edge.value().type() != E_FUNCTION_CALL &&
                edge.value().type() != E_FUNCTION_XFER &&
                edge.value().type() != E_FUNCTION_RETURN && !edge.isSelfEdge() &&
                !edge.source()->value().isEntryBlock()) {
                SAWYER_MESG(debug) <<"  following edge " <<edgeName(edge) <<"\n";
                neighbors.insert(edge.target()->id(), edge.target());
            }
        }

        // Mark the neighbors as being provisionally owned by this function and be sure to visit them recursively.
        for (const ControlFlowGraph::ConstVertexIterator &neighbor: neighbors.values()) {
            if (neighbor->value().type() != V_BASIC_BLOCK)
                continue;                               // functions don't own things like the indeterminate vertex
            if (ownership.exists(neighbor->id()))
                continue;                               // we already know about this neighbor
            ownership.insertMaybe(neighbor->id(), Function::OWN_PROVISIONAL);
            worklist.push(neighbor->id());
        }
    }

    // If there were no errors then add all the provisional vertices to this function. This does not modify the CFG.
    for (const VertexOwnership::Node &node: ownership.nodes()) {
        if (node.value() == Function::OWN_PROVISIONAL)
            function->insertBasicBlock(cfg_.findVertex(node.key())->value().address());
    }
}

std::set<Address>
Partitioner::functionGhostSuccessors(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    std::set<Address> ghosts;
    for (Address blockVa: function->basicBlockAddresses()) {
        if (BasicBlock::Ptr bb = basicBlockExists(blockVa)) {
            for (Address ghost: basicBlockGhostSuccessors(bb)) {
                ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(ghost);
                if (placeholder == cfg_.vertices().end())
                    ghosts.insert(ghost);
            }
        }
    }
    return ghosts;
}

FunctionCallGraph
Partitioner::functionCallGraph(AllowParallelEdges::Type allowParallelEdges) const {
    FunctionCallGraph cg;
    size_t edgeCount = allowParallelEdges == AllowParallelEdges::YES ? 0 : 1;

    // Create a vertex for every function.  This is optional -- if commented out then only functions that have incoming or
    // outgoing edges will be present.
    for (const Function::Ptr &function: functions())
        cg.insertFunction(function);

    for (const ControlFlowGraph::Edge &edge: cfg_.edges()) {
        if (edge.source()->value().type()==V_BASIC_BLOCK && edge.target()->value().type()==V_BASIC_BLOCK) {
            for (const Function::Ptr &source: edge.source()->value().owningFunctions().values()) {
                for (const Function::Ptr &target: edge.target()->value().owningFunctions().values()) {
                    if (source != target || edge.value().type() == E_FUNCTION_CALL || edge.value().type() == E_FUNCTION_XFER) {
                        cg.insertCall(source, target, edge.value().type(), edgeCount);
                    }
                }
            }
        }
    }
    return cg;
}

std::set<Address>
Partitioner::functionDataFlowConstants(const Function::Ptr &function) const {
    using namespace Rose::BinaryAnalysis::InstructionSemantics;

    std::set<Address> retval;
    BaseSemantics::RiscOperators::Ptr ops = newOperators();
    BaseSemantics::Dispatcher::Ptr cpu = newDispatcher(ops);
    if (!cpu)
        return retval;

    // Build the data flow engine. We're using parts from a variety of locations.
    typedef DataFlow::DfCfg DfCfg;
    typedef DataFlow::TransferFunction TransferFunction;
    typedef BinaryAnalysis::DataFlow::SemanticsMerge MergeFunction;
    typedef BinaryAnalysis::DataFlow::Engine<DfCfg, State::Ptr, TransferFunction, MergeFunction> Engine;

    ControlFlowGraph::ConstVertexIterator startVertex = findPlaceholder(function->address());
    ASSERT_require2(cfg_.isValidVertex(startVertex), "function does not exist in partitioner");
    DfCfg dfCfg = DataFlow::buildDfCfg(sharedFromThis(), cfg_, startVertex); // not interprocedural
    size_t dfCfgStartVertexId = 0; // dfCfg vertex corresponding to function's entry ponit.
    TransferFunction xfer(cpu);
    MergeFunction mergeFunction(cpu);
    Engine dfEngine(dfCfg, xfer, mergeFunction);
    dfEngine.name("find-constants");
    dfEngine.maxIterations(2 * dfCfg.nVertices());        // arbitrary limit for non-convergent flow

    State::Ptr initialState = xfer.initialState();
    const RegisterDescriptor SP = cpu->stackPointerRegister();
    const RegisterDescriptor memSegReg;
    BaseSemantics::SValue::Ptr initialStackPointer = ops->peekRegister(SP);

    // Run the data flow
    try {
        dfEngine.runToFixedPoint(dfCfgStartVertexId, initialState);
    } catch (const BaseSemantics::NotImplemented &e) {
        mlog[WHERE] <<function->printableName() <<": " <<e <<"\n";
        return retval;
    } catch (const BaseSemantics::Exception &e) {
        mlog[ERROR] <<function->printableName() <<": " <<e <<"\n";
        return retval;
    } catch (const BinaryAnalysis::DataFlow::NotConverging &e) {
        mlog[WARN] <<function->printableName() <<": " <<e.what() <<"\n";
    } catch (const BinaryAnalysis::DataFlow::Exception &e) {
        mlog[ERROR] <<function->printableName() <<": " <<e.what() <<"\n";
        return retval;
    }

    // Scan all outgoing states and accumulate any concrete values we find.
    for (State::Ptr state: dfEngine.getFinalStates()) {
        if (state) {
            ops->currentState(state);
            BaseSemantics::RegisterStateGeneric::Ptr regs =
                BaseSemantics::RegisterStateGeneric::promote(state->registerState());
            for (const BaseSemantics::RegisterStateGeneric::RegPair &kv: regs->get_stored_registers()) {
                if (kv.value->isConcrete() && kv.value->nBits() <= SP.nBits())
                    retval.insert(kv.value->toUnsigned().get());
            }

            BaseSemantics::MemoryCellState::Ptr mem = BaseSemantics::MemoryCellState::promote(state->memoryState());
            std::set<Address> vas = Variables::VariableFinder::instance()->findAddressConstants(mem);
            retval.insert(vas.begin(), vas.end());
        }
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner::CfgAdjustmentCallbacks&
Partitioner::cfgAdjustmentCallbacks() {
    return cfgAdjustmentCallbacks_;
}

const Partitioner::CfgAdjustmentCallbacks&
Partitioner::cfgAdjustmentCallbacks() const {
    return cfgAdjustmentCallbacks_;
}

Partitioner::BasicBlockCallbacks&
Partitioner::basicBlockCallbacks() {
    return basicBlockCallbacks_;
}

const Partitioner::BasicBlockCallbacks&
Partitioner::basicBlockCallbacks() const {
    return basicBlockCallbacks_;
}

Partitioner::FunctionPrologueMatchers&
Partitioner::functionPrologueMatchers() {
    return functionPrologueMatchers_;
}

const Partitioner::FunctionPrologueMatchers&
Partitioner::functionPrologueMatchers() const {
    return functionPrologueMatchers_;
}

Partitioner::FunctionPaddingMatchers&
Partitioner::functionPaddingMatchers() {
    return functionPaddingMatchers_;
}

const Partitioner::FunctionPaddingMatchers&
Partitioner::functionPaddingMatchers() const {
    return functionPaddingMatchers_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::bblockAttached(const ControlFlowGraph::VertexIterator &newVertex) {
    ASSERT_require(newVertex!=cfg_.vertices().end());
    ASSERT_require(newVertex->value().type() == V_BASIC_BLOCK);
    updateCfgProgress();
    Address startVa = newVertex->value().address();
    BasicBlock::Ptr bblock = newVertex->value().bblock();

    Stream debug(mlog[DEBUG]);
    if (debug) {
        if (bblock) {
            if (bblock->isEmpty()) {
                debug <<"attached empty basic block at " <<StringUtility::addrToString(startVa) <<"\n";
            } else {
                debug <<"attached basic block:\n";
                for (SgAsmInstruction* insn: bblock->instructions()) {
                  if (isSgAsmCilInstruction(insn) || isSgAsmJvmInstruction(insn)) {
                    debug <<"  + " << insn->get_mnemonic() <<"\n";
                  }
                  else {
                    debug <<"  + " <<unparse(insn) <<"\n";
                  }
                }
            }
        } else {
            debug <<"inserted basic block placeholder at " <<StringUtility::addrToString(startVa) <<"\n";
        }
    }

#if !defined(NDEBUG) && ROSE_PARTITIONER_EXPENSIVE_CHECKS == 1
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::AttachedBasicBlock(sharedFromThis(), startVa, bblock));
#if !defined(NDEBUG) && ROSE_PARTITIONER_EXPENSIVE_CHECKS == 1
    checkConsistency();
#endif
}

void
Partitioner::bblockDetached(Address startVa, const BasicBlock::Ptr &bblock) {
    Stream debug(mlog[DEBUG]);
    if (debug) {
        if (bblock) {
            if (bblock->isEmpty()) {
                debug <<"detached empty basic block at " <<StringUtility::addrToString(startVa) <<"\n";
            } else {
                debug <<"detached basic block:\n";
                for (SgAsmInstruction* insn: bblock->instructions())
                  if (isSgAsmCilInstruction(insn)) {
                    debug <<"  + " << insn->get_mnemonic() <<"\n";
                  }
                  else {
                    debug <<"  + " <<unparse(insn) <<"\n";
                  }
            }
        } else {
            debug <<"erased basic block placeholder at " <<StringUtility::addrToString(startVa) <<"\n";
        }
    }

#if !defined(NDEBUG) && ROSE_PARTITIONER_EXPENSIVE_CHECKS == 1
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::DetachedBasicBlock(sharedFromThis(), startVa, bblock));
#if !defined(NDEBUG) && ROSE_PARTITIONER_EXPENSIVE_CHECKS == 1
    checkConsistency();
#endif
}

void
Partitioner::checkConsistency() const {
    static const bool extraDebuggingOutput = false;
    using namespace StringUtility;
    Stream debug(mlog[DEBUG]);
    if (extraDebuggingOutput)
        debug <<"checking partitioner consistency...\n";
    for (const ControlFlowGraph::Vertex &vertex: cfg_.vertices()) {
        if (vertex.value().type() == V_BASIC_BLOCK) {
            if (extraDebuggingOutput && debug) {
                debug <<"  basic block" <<(vertex.value().bblock()?"":" placeholder")
                      <<" " <<addrToString(vertex.value().address()) <<"\n";
                debug <<"    outgoing edges:";
                for (const ControlFlowGraph::Edge &edge: vertex.outEdges()) {
                    const ControlFlowGraph::Vertex &target = *edge.target();
                    switch (target.value().type()) {
                        case V_BASIC_BLOCK:
                            debug <<" " <<addrToString(target.value().address());
                            break;
                        case V_INDETERMINATE:
                            debug <<" indeterminate";
                            break;
                        case V_NONEXISTING:
                            debug <<" nonexisting";
                            break;
                        case V_UNDISCOVERED:
                            debug <<" undiscovered";
                            break;
                        case V_USER_DEFINED:
                            debug <<" user-defined";
                            break;
                    }
                }
                debug <<"\n";
            }

            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                ASSERT_always_require(bb->isFrozen());
                ASSERT_always_require(bb->address() == vertex.value().address());
                if (bb->isEmpty()) {
                    // Non-existing basic block
                    ASSERT_always_require2(vertex.nOutEdges()==1,
                                           "nonexisting block " + addrToString(bb->address()) + " must have one outgoing edge");
                    ControlFlowGraph::ConstEdgeIterator edge = vertex.outEdges().begin();
                    ASSERT_always_require2(edge->target() == nonexistingVertex_,
                                           "nonexisting block " + addrToString(bb->address()) + " edges must go to a special vertex");
                } else {
                    // Existing basic block
                    for (SgAsmInstruction *insn: bb->instructions()) {
                        AddressUser insnAddrUser = aum_.findInstruction(insn);
                        ASSERT_always_require2(insnAddrUser.insn() == insn,
                                               "instruction " + addrToString(insn->get_address()) + " in block " +
                                               addrToString(bb->address()) + " must be present in the AUM");
                        bool foundBlock = false;
                        for (const BasicBlock::Ptr &bb2: insnAddrUser.basicBlocks()) {
                            if ((foundBlock = bb2 == bb))
                                break;
                        }
                        ASSERT_always_require2(foundBlock,
                                               "instruction " + addrToString(insn->get_address()) + " in " +
                                               bb->printableName() + " does not have correct ownership in AUM\n");
                        AddressInterval insnInterval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
                        AddressUsers addressUsers = aum_.spanning(insnInterval);
                        ASSERT_always_require2(addressUsers.instructionExists(insn->get_address()),
                                               "instruction " + addrToString(insn->get_address()) + " in block " +
                                               addrToString(bb->address()) + " must span its own address interval in the AUM");
                    }
                }
            } else {
                // Basic block placeholder
                ASSERT_always_require2(vertex.nOutEdges() == 1,
                                       "placeholder " + addrToString(vertex.value().address()) +
                                       " must have exactly one outgoing edge");
                ControlFlowGraph::ConstEdgeIterator edge = vertex.outEdges().begin();
                ASSERT_always_require2(edge->target() == undiscoveredVertex_,
                                       "placeholder " + addrToString(vertex.value().address()) +
                                       " edge must go to a special vertex");
            }

            ASSERT_always_require2(vertexIndex_.exists(vertex.value().address()),
                                   "bb/placeholder " + addrToString(vertex.value().address()) +
                                   " must exist in the vertex index");

        } else {
            // Special vertices
            ASSERT_always_require2(vertex.nOutEdges()==0,
                                   "special vertices must have no outgoing edges");
        }
    }

    aum_.checkConsistency();
}

SgAsmGenericSection*
Partitioner::elfGot(SgAsmElfFileHeader *elfHeader) {
    if (!elfHeader)
        return nullptr;
    SgAsmGenericSection *found = nullptr;

    // Get the section pointed to by the DT_PLTGOT entry of the .dynamic section.
    if (SgAsmElfDynamicSection *dynamic = isSgAsmElfDynamicSection(elfHeader->get_sectionByName(".dynamic"))) {
        if (SgAsmElfDynamicEntryList *dentriesNode = dynamic->get_entries()) {
            for (SgAsmElfDynamicEntry *dentry: dentriesNode->get_entries()) {
                if (dentry->get_d_tag() == SgAsmElfDynamicEntry::DT_PLTGOT) {
                    RelativeVirtualAddress rva = dentry->get_d_val();
                    found = rva.boundSection();
                    break;
                }
            }
        }
    }

    // If that failed, just try some common names.
    if (!found)
        found = elfHeader->get_sectionByName(".got.plt");
    if (!found)
        found = elfHeader->get_sectionByName(".plt.got");

    if (found) {
        elfGotVa_ = found->get_mappedActualVa();
    } else {
        elfGotVa_.reset();
    }

    return found;
}

Sawyer::Optional<Address>
Partitioner::elfGotVa() const {
    return elfGotVa_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const BasePartitionerSettings&
Partitioner::settings() const {
    return settings_;
}

void
Partitioner::settings(const BasePartitionerSettings &s) {
    settings_ = s;
}

void
Partitioner::enableSymbolicSemantics(bool b) {
    settings_.usingSemantics = b;
}

void
Partitioner::disableSymbolicSemantics() {
    settings_.usingSemantics = false;
}

bool
Partitioner::usingSymbolicSemantics() const {
    return settings_.usingSemantics;
}

void
Partitioner::autoAddCallReturnEdges(bool b) {
    autoAddCallReturnEdges_ = b;
}

bool
Partitioner::autoAddCallReturnEdges() const {
    return autoAddCallReturnEdges_;
}

void
Partitioner::assumeFunctionsReturn(bool b) {
    assumeFunctionsReturn_ = b;
}

bool
Partitioner::assumeFunctionsReturn() const {
    return assumeFunctionsReturn_;
}

const SourceLocations&
Partitioner::sourceLocations() const {
    return sourceLocations_;
}

SourceLocations&
Partitioner::sourceLocations() {
    return sourceLocations_;
}

void
Partitioner::sourceLocations(const SourceLocations &locs) {
    sourceLocations_ = locs;
}

bool
Partitioner::checkingCallBranch() const {
    return settings_.checkingCallBranch;
}

void
Partitioner::checkingCallBranch(bool b) {
    settings_.checkingCallBranch = b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFG utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::expandIndeterminateCalls() {
    std::vector<ControlFlowGraph::VertexIterator> allFunctions;
    for (ControlFlowGraph::VertexIterator vertex = cfg_.vertices().begin(); vertex != cfg_.vertices().end(); ++vertex) {
        // Look for indeterminate function calls
        std::vector<ControlFlowGraph::EdgeIterator> indeterminateEdges;
        for (ControlFlowGraph::EdgeIterator edge = vertex->outEdges().begin(); edge != vertex->outEdges().end(); ++edge) {
            if (edge->value().type() == E_FUNCTION_CALL && edge->target()->value().type() == V_INDETERMINATE)
                indeterminateEdges.push_back(edge);
        }

        if (!indeterminateEdges.empty()) {
            for (ControlFlowGraph::EdgeIterator edge: indeterminateEdges)
                cfg_.eraseEdge(edge);

            if (allFunctions.empty()) {
                for (ControlFlowGraph::VertexIterator v = cfg_.vertices().begin(); v != cfg_.vertices().end(); ++v) {
                    if (v->value().isEntryBlock())
                        allFunctions.push_back(v);
                }
            }

            for (ControlFlowGraph::VertexIterator target: allFunctions)
                cfg_.insertEdge(vertex, target, CfgEdge(E_FUNCTION_CALL));
        }
    }
}

// class method
std::string
Partitioner::basicBlockName(const BasicBlock::Ptr &bblock) {
    return bblock==nullptr ? "no basic block" : bblock->printableName();
}

// class method
std::string
Partitioner::dataBlockName(const DataBlock::Ptr &dblock) {
    return dblock==nullptr ? "no data block" : dblock->printableName();
}

// class method
std::string
Partitioner::functionName(const Function::Ptr &function) {
    return function==nullptr ? "no function" : function->printableName();
}

// class method
std::string
Partitioner::vertexName(const ControlFlowGraph::Vertex &vertex) {
    switch (vertex.value().type()) {
        case V_BASIC_BLOCK: {
            std::string retval = StringUtility::addrToString(vertex.value().address()) +
                                 "<" + StringUtility::numberToString(vertex.id());
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                if (bb->isEmpty())
                    retval += ",X";                     // non-existing
            } else {
                retval += ",P";                         // placeholder
            }
            return retval + ">";
        }
        case V_INDETERMINATE:
            return "indeterminate<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_NONEXISTING:
            return "non-existing<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_UNDISCOVERED:
            return "undiscovered<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_USER_DEFINED:
            return "user-defined<" + StringUtility::numberToString(vertex.id()) + ">";
    }
    ASSERT_not_reachable("invalid vertex type");
}

std::string
Partitioner::vertexName(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    if (vertex != cfg_.vertices().end())
        return vertexName(*vertex);
    return "no-vertex";
}

// class method
std::string
Partitioner::vertexNameEnd(const ControlFlowGraph::Vertex &vertex) {
    if (vertex.value().type() == V_BASIC_BLOCK) {
        if (BasicBlock::Ptr bb = vertex.value().bblock()) {
            if (!bb->isEmpty()) {
                return vertexName(vertex) + ":" + StringUtility::addrToString(bb->instructions().back()->get_address());
            }
        }
    }
    return vertexName(vertex);
}

// class method
std::string
Partitioner::edgeNameDst(const ControlFlowGraph::Edge &edge) {
    std::string retval;
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FUNCTION_CALL:
            retval += "<fcall>";
            break;
        case E_FUNCTION_XFER:
            retval += "<fxfer>";
            break;
        case E_FUNCTION_RETURN:
            retval += "<return>";
            break;
        case E_CALL_RETURN:
            retval += "<callret>";
            break;
        case E_USER_DEFINED:
            retval += "<user>";
            break;
    }
    return retval + vertexName(*edge.target());
}

std::string
Partitioner::edgeNameDst(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    if (edge != cfg_.edges().end())
        return edgeNameDst(*edge);
    return "no-edge";
}

// class method
std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::Edge &edge) {
    const ControlFlowGraph::Vertex &source = *edge.source();
    std::string retval = vertexNameEnd(source);
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FUNCTION_CALL:
            retval += "<fcall>";
            break;
        case E_FUNCTION_XFER:
            retval += "<fxfer>";
            break;
        case E_FUNCTION_RETURN:
            retval += "<return>";
            break;
        case E_CALL_RETURN:
            retval += "<callret>";
            break;
        case E_USER_DEFINED:
            retval += "<user>";
            break;
    }
    return retval;
}

std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    if (edge != cfg_.edges().end())
        return edgeNameSrc(*edge);
    return "no-edge";
}

// class method
std::string
Partitioner::edgeName(const ControlFlowGraph::Edge &edge) {
    std::string retval = vertexNameEnd(*edge.source()) + " ";
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FUNCTION_CALL:
            retval += "(fcall)";
            break;
        case E_FUNCTION_XFER:
            retval += "(fxfer)";
            break;
        case E_FUNCTION_RETURN:
            retval += "(return)";
            break;
        case E_CALL_RETURN:
            retval += "(callret)";
            break;
        case E_USER_DEFINED:
            retval += "(user)";
            break;
    }
    return retval + "-> " + vertexName(*edge.target());
}

std::string
Partitioner::edgeName(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    if (edge != cfg_.edges().end())
        return edgeName(*edge);
    return "no-edge";
}

void
Partitioner::dumpCfg(std::ostream &out, const std::string &prefix, bool showBlocks, bool computeProperties) const {
    const std::string insnPrefix = prefix + "    ";

    // Sort the vertices according to basic block starting address.
    std::vector<ControlFlowGraph::ConstVertexIterator> sortedVertices;
    for (ControlFlowGraph::ConstVertexIterator vi=cfg_.vertices().begin(); vi!=cfg_.vertices().end(); ++vi) {
        if (vi->value().type() == V_BASIC_BLOCK)
            sortedVertices.push_back(vi);
    }
    std::sort(sortedVertices.begin(), sortedVertices.end(), sortVerticesByAddress);
    for (const ControlFlowGraph::ConstVertexIterator &vertex: sortedVertices) {
        out <<prefix <<"basic block " <<vertexName(*vertex) <<"\n";

        // Function ownership
        for (const Function::Ptr &function: vertex->value().owningFunctions().values()) {
            if (function->address() == vertex->value().address()) {
                out <<prefix <<"  entry block for " <<functionName(function) <<"\n";
            } else {
                out <<prefix <<"  owned by " <<functionName(function) <<"\n";
            }
        }

        // Function properties
        if (Function::Ptr function = vertex->value().isEntryBlock()) {
            if (computeProperties) {
                out <<"    " <<function->printableName() <<" stack delta: ";
                if (BaseSemantics::SValue::Ptr delta = functionStackDelta(function)) {
                    if (auto n = delta->toSigned()) {
                        out <<*n <<"\n";
                    } else {
                        out <<*delta <<"\n";
                    }
                } else {
                    out <<"not computed\n";
                }
            }
        }

        // Sort incoming edges according to source (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeIterator> sortedInEdges;
        for (ControlFlowGraph::ConstEdgeIterator ei=vertex->inEdges().begin(); ei!=vertex->inEdges().end(); ++ei)
            sortedInEdges.push_back(ei);
        std::sort(sortedInEdges.begin(), sortedInEdges.end(), sortEdgesBySrc);
        out <<prefix <<"  predecessors:";
        if (sortedInEdges.empty()) {
            out <<" none\n";
        } else {
            for (const ControlFlowGraph::ConstEdgeIterator &edge: sortedInEdges)
                out <<" " <<edgeNameSrc(*edge);
            out <<"\n";
        }

        // Pre-block properties
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (computeProperties) {
                for (const Function::Ptr &function: vertex->value().owningFunctions().values()) {
                    out <<"    incoming stack delta w.r.t. " <<function->printableName() <<": ";
                    if (BaseSemantics::SValue::Ptr delta = basicBlockStackDeltaIn(bb, function)) {
                        if (auto n = delta->toSigned()) {
                            out <<*n <<"\n";
                        } else {
                            out <<*delta <<"\n";
                        }
                    } else {
                        out <<"not computed\n";
                    }
                }
            }
        }

        // Show instructions in execution order
        if (showBlocks) {
            if (BasicBlock::Ptr bb = vertex->value().bblock()) {
                for (SgAsmInstruction *insn: bb->instructions())
                    out <<insnPrefix <<insn->toString() <<"\n";
            }
        }

        // Show some basic block properties
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            BasicBlockSemantics sem = bb->semantics();
            if (sem.finalState()==nullptr)
                out <<prefix <<"  no semantics (discarded already, or failed)\n";

            // call semantics?
            out <<prefix <<"  is function call? ";
            if (computeProperties)
                basicBlockIsFunctionCall(bb);
            bool b=false;
            if (bb->isFunctionCall().getOptional().assignTo(b)) {
                out <<(b ? "yes" : "no") <<"\n";
            } else {
                out <<"not computed\n";
            }

            // return semantics?
            out <<prefix <<"  is function return? ";
            if (computeProperties)
                basicBlockIsFunctionReturn(bb);
            if (bb->isFunctionReturn().getOptional().assignTo(b)) {
                out <<(b ? "yes" : "no") <<"\n";
            } else {
                out <<"not computed\n";
            }

            // Outgoing stack delta w.r.t. owning functions
            if (computeProperties) {
                for (const Function::Ptr &function: vertex->value().owningFunctions().values()) {
                    out <<"    outgoing stack delta w.r.t. " <<function->printableName() <<": ";
                    if (BaseSemantics::SValue::Ptr delta = basicBlockStackDeltaOut(bb, function)) {
                        if (auto n = delta->toSigned()) {
                            out <<*n <<"\n";
                        } else {
                            out <<*delta <<"\n";
                        }
                    } else {
                        out <<"not computed\n";
                    }
                }
            }

            // may-return?
            out <<prefix <<"  may eventually return to caller? ";
            if (computeProperties)
                basicBlockOptionalMayReturn(vertex);
            if (bb->mayReturn().getOptional().assignTo(b)) {
                out <<(b ? "yes" : "no") <<"\n";
            } else {
                out <<"unknown\n";
            }
        }

        // Sort outgoing edges according to destination (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeIterator> sortedOutEdges;
        for (ControlFlowGraph::ConstEdgeIterator ei=vertex->outEdges().begin(); ei!=vertex->outEdges().end(); ++ei)
            sortedOutEdges.push_back(ei);
        std::sort(sortedOutEdges.begin(), sortedOutEdges.end(), sortEdgesByDst);
        out <<prefix <<"  successors:";
        if (sortedOutEdges.empty()) {
            out <<" none\n";
        } else {
            for (const ControlFlowGraph::ConstEdgeIterator &edge: sortedOutEdges)
                out <<" " <<edgeNameDst(*edge);
            out <<"\n";
        }

        // Ghost edges
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            std::set<Address> ghosts = basicBlockGhostSuccessors(bb);
            if (!ghosts.empty()) {
                out <<prefix <<"  ghost successors:";
                for (Address ghost: ghosts)
                    out <<" " <<StringUtility::addrToString(ghost);
                out <<"\n";
            }
        }
    }
}

void
Partitioner::cfgGraphViz(std::ostream &out, const AddressInterval &restrict,
                         bool showNeighbors) const {
    GraphViz::CfgEmitter gv(sharedFromThis());
    gv.useFunctionSubgraphs(true);
    gv.showReturnEdges(false);
    gv.showInstructions(true);
    gv.showInNeighbors(showNeighbors);
    gv.showOutNeighbors(showNeighbors);
    gv.emitIntervalGraph(out, restrict);
}

std::vector<Function::Ptr>
Partitioner::nextFunctionPrologue(Address startVa) {
    return nextFunctionPrologue(startVa, startVa);
}

std::vector<Function::Ptr>
Partitioner::nextFunctionPrologue(Address startVa, Address &lastSearchedVa) {
    lastSearchedVa = startVa;
    while (memoryMap_->atOrAfter(startVa).require(MemoryMap::EXECUTABLE).next().assignTo(startVa)) {
        Sawyer::Optional<Address> unmappedVa = aum_.leastUnmapped(startVa);
        lastSearchedVa = startVa;
        if (!unmappedVa)
            return std::vector<Function::Ptr>();        // empty; no higher unused address
        if (startVa == *unmappedVa) {
            for (const FunctionPrologueMatcher::Ptr &matcher: functionPrologueMatchers_) {
                if (matcher->match(sharedFromThis(), startVa)) {
                    std::vector<Function::Ptr> newFunctions = matcher->functions();
                    ASSERT_forbid(newFunctions.empty());
                    return newFunctions;
                }
            }
            ++startVa;
        } else {
            startVa = *unmappedVa;
        }
    }
    return std::vector<Function::Ptr>();
}

AddressUsageMap
Partitioner::aum(const Function::Ptr &function) const {
    AddressUsageMap retval;
    for (Address blockVa: function->basicBlockAddresses()) {
        if (BasicBlock::Ptr bb = basicBlockExists(blockVa)) {
            for (SgAsmInstruction *insn: bb->instructions())
                retval.insertInstruction(insn, bb);
            for (const DataBlock::Ptr &dblock: bb->dataBlocks())
                retval.insertDataBlock(dblock);
        }
    }
    for (const DataBlock::Ptr &dblock: function->dataBlocks())
        retval.insertDataBlock(dblock);
    return retval;
}

std::vector<AddressUser>
Partitioner::users(Address va) const {
    return aum_.overlapping(va).addressUsers();
}

std::set<Address>
Partitioner::ghostSuccessors() const {
    std::set<Address> ghosts;
    for (const CfgVertex &vertex: cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock()) {
                for (Address ghost: basicBlockGhostSuccessors(bb)) {
                    ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(ghost);
                    if (placeholder == cfg_.vertices().end())
                        ghosts.insert(ghost);
                }
            }
        }
    }
    return ghosts;
}

bool
Partitioner::isEdgeIntraProcedural(ControlFlowGraph::ConstEdgeIterator edge, const Function::Ptr &function) const {
    ASSERT_require(cfg_.isValidEdge(edge));
    return isEdgeIntraProcedural(*edge, function);
}

bool
Partitioner::isEdgeIntraProcedural(ControlFlowGraph::ConstEdgeIterator edge) const {
    return isEdgeIntraProcedural(edge, Function::Ptr());
}

bool
Partitioner::isEdgeIntraProcedural(const ControlFlowGraph::Edge &edge, const Function::Ptr &function) const {
    if (edge.value().type() == E_FUNCTION_CALL ||
        edge.value().type() == E_FUNCTION_XFER ||
        edge.value().type() == E_FUNCTION_RETURN)
        return false;

    if (function)
        return edge.source()->value().isOwningFunction(function) && edge.target()->value().isOwningFunction(function);

    if (edge.source()->value().nOwningFunctions() == 0 && edge.target()->value().nOwningFunctions() == 0)
        return true;

    return !(edge.source()->value().owningFunctions() & edge.target()->value().owningFunctions()).isEmpty();
}

bool
Partitioner::isEdgeIntraProcedural(const ControlFlowGraph::Edge &edge) const {
    return isEdgeIntraProcedural(edge, Function::Ptr());
}

bool
Partitioner::isEdgeInterProcedural(ControlFlowGraph::ConstEdgeIterator edge) const {
    return isEdgeInterProcedural(edge, Function::Ptr());
}

bool
Partitioner::isEdgeInterProcedural(ControlFlowGraph::ConstEdgeIterator edge, const Function::Ptr &sourceFunction) const {
    return isEdgeInterProcedural(edge, sourceFunction, Function::Ptr());
}

bool
Partitioner::isEdgeInterProcedural(ControlFlowGraph::ConstEdgeIterator edge,
                                   const Function::Ptr &sourceFunction, const Function::Ptr &targetFunction) const {
    ASSERT_require(cfg_.isValidEdge(edge));
    return isEdgeInterProcedural(*edge, sourceFunction, targetFunction);
}

bool
Partitioner::isEdgeInterProcedural(const ControlFlowGraph::Edge &edge) const {
    return isEdgeInterProcedural(edge, Function::Ptr());
}

bool
Partitioner::isEdgeInterProcedural(const ControlFlowGraph::Edge &edge, const Function::Ptr &sourceFunction) const {
    return isEdgeInterProcedural(edge, sourceFunction, Function::Ptr());
}

bool
Partitioner::isEdgeInterProcedural(const ControlFlowGraph::Edge &edge,
                                   const Function::Ptr &sourceFunction, const Function::Ptr &targetFunction) const {
    if (edge.value().type() == E_FUNCTION_CALL ||
        edge.value().type() == E_FUNCTION_XFER ||
        edge.value().type() == E_FUNCTION_RETURN)
        return true;

    if (sourceFunction != nullptr && targetFunction != nullptr) {
        if (sourceFunction == targetFunction)
            return false;
        return (edge.source()->value().isOwningFunction(sourceFunction) &&
                edge.target()->value().isOwningFunction(targetFunction));
    } else if (sourceFunction != nullptr) {
        ASSERT_require(targetFunction == nullptr);
        return (edge.source()->value().isOwningFunction(sourceFunction) &&
                !edge.target()->value().isOwningFunction(sourceFunction));
    } else if (targetFunction != nullptr) {
        ASSERT_require(sourceFunction == nullptr);
        return (!edge.source()->value().isOwningFunction(targetFunction) &&
                edge.target()->value().isOwningFunction(targetFunction));
    } else {
        ASSERT_require(sourceFunction == nullptr && targetFunction == nullptr);
        if (edge.source()->value().nOwningFunctions() == 0 && edge.target()->value().nOwningFunctions() == 0)
            return true;
        return edge.source()->value().owningFunctions() != edge.target()->value().owningFunctions();
    }
}

void
Partitioner::addressName(Address va, const std::string &name) {
    if (name.empty()) {
        addressNames_.erase(va);
    } else {
        addressNames_.insert(va, name);
    }
}

const std::string&
Partitioner::addressName(Address va) const {
    return addressNames_.getOrDefault(va);
}

const Partitioner::AddressNameMap&
Partitioner::addressNames() const {
    return addressNames_;
}

void
Partitioner::rebuildVertexIndices() {
    vertexIndex_.clear();
    for (ControlFlowGraph::VertexIterator vertex = cfg_.vertices().begin(); vertex != cfg_.vertices().end(); ++vertex) {
        switch (vertex->value().type()) {
            case V_BASIC_BLOCK:
                ASSERT_forbid(vertexIndex_.exists(vertex->value().address()));
                vertexIndex_.insert(vertex->value().address(), vertex);
                break;
            case V_INDETERMINATE:
                indeterminateVertex_ = vertex;
                break;
            case V_NONEXISTING:
                nonexistingVertex_ = vertex;
                break;
            case V_UNDISCOVERED:
                undiscoveredVertex_ = vertex;
                break;
            case V_USER_DEFINED:
                ASSERT_not_reachable("user-defined vertices cannot be saved or restored");
        }
    }
    if (!isDefaultConstructed() && instructionProvider().disassembler()) {
        unparser_ = instructionProvider().disassembler()->unparser()->copy();
        insnUnparser_ = instructionProvider().disassembler()->unparser()->copy();
        insnPlainUnparser_ = instructionProvider().disassembler()->unparser()->copy();
    }
    if (insnUnparser_)
        configureInsnUnparser(insnUnparser_);
    if (insnPlainUnparser_)
        configureInsnPlainUnparser(insnPlainUnparser_);
}

const IndirectControlFlow::State&
Partitioner::icf() const {
    return icf_;
}

IndirectControlFlow::State&
Partitioner::icf() {
    return icf_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Python
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_PYTHON_API
void
Partitioner::pythonUnparse() const {
    if (Unparser::Base::Ptr u = unparser()) {
        u->unparse(std::cout, *this);
    } else {
        std::cout <<"no unparser for this architecture\n";
    }
}
#endif

} // namespace
} // namespace
} // namespace

#endif
