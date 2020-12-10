#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryReachability.h>

#include <BinaryDataFlow.h>
#include <CommandLine.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/ThreadWorkers.h>
#include <Sawyer/Tracker.h>
#include <stringify.h>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility Reachability::mlog;

// class method
void
Reachability::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("Rose::BinaryAnalysis::Reachability", Diagnostics::destination);
        mlog.comment("propagating reachability through a CFG");
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command-line parsing
//
// Command-line parsing of switches whose arguments are lists of Reason enums is a little complicated because:
//
//   1. The token is a string but the value is an enum constant, so we need a translation from string to enum. Fortunately
//      this is built into Sawyer::CommandLine as the EnumParser class template.
//
//   2. Since the settings being adjusted are sets of enum constants (BitFlags<T>), the documentation describing the defaults
//      must be constructed from the set.
//
//   3. If the user specifies an argument (list of enum names) for the switch, then the default value of the switch should
//      be discarded before the specified enums are unioned into the result. The clearing and unioning are handled by the
//      ReasonSaver class.
//
//   4. The names of the enum constants in C++ are not the same as the names we want users to use on the command-line. Since
//      the command-line is smaller than the API, we can use abbreviated terms in the command-line.
//
//   5. Since the switches are also used to turn features on and off, we need to support specifying the switch with no argument
//      in which case a default should be used, but a different default than not specifying the switch at all. E.g., the
//      switch-not-specified value might be the empty set, and the switch-specified-with-no-argument value is probably a set
//      of one or more enum constants.  The ability to not have an argument needs to be optional per switch.
//
//   6. We need some way for the user to turn off a feature. It's customary to use switches that start with "no-" to accomplish
//      this, as in "--foo" and "--no-foo".
//
//   7. Large parts of the documentation must be dynamically generated and should be consistent between all switches.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ReasonSaver: public Sawyer::CommandLine::ValueSaver {
    Reachability::ReasonFlags &storage_;
    bool isDefault_;
protected:
    ReasonSaver(Reachability::ReasonFlags &storage)
        : storage_(storage), isDefault_(true) {}
public:
    typedef Sawyer::SharedPointer<ReasonSaver> Ptr;
    static Ptr instance(Reachability::ReasonFlags &storage) {
        return Ptr(new ReasonSaver(storage));
    }
    void save(const boost::any &value, const std::string &/*switchKey*/) ROSE_OVERRIDE {
        Reachability::Reason reason = boost::any_cast<Reachability::Reason>(value);
        if (isDefault_)
            storage_.clear();
        isDefault_ = false;

        if (Reachability::NOT_REACHABLE == reason) {
            storage_.clear();
        } else {
            storage_.set(reason);
        }
    }
};

// class method
std::string
Reachability::reasonArgument(Reason reason) {
    switch (reason) {
        case NOT_REACHABLE:          return "none";
        case PROGRAM_ENTRY_POINT:    return "entry";
        case EXPORTED_FUNCTION:      return "export";
        case SIGNAL_HANDLER:         return "sig-handler";
        case ASSUMED:                return "assumed";
        case EXPLICIT_MEM_CONSTANT:  return "mem-scan";
        case EXPLICIT_INSN_CONSTANT: return "insn-scan";
        case IMPLICIT_FUNC_CONSTANT: return "func-scan";
        case USER_DEFINED_0:         return "user-0";
        case USER_DEFINED_1:         return "user-1";
        case USER_DEFINED_2:         return "user-2";
        case USER_DEFINED_3:         return "user-3";
        case USER_DEFINED_4:         return "user-4";
        case USER_DEFINED_5:         return "user-5";
        case USER_DEFINED_6:         return "user-6";
        case USER_DEFINED_7:         return "user-7";
        case USER_DEFINED_8:         return "user-8";
        case USER_DEFINED_9:         return "user-9";
        case USER_DEFINED_10:        return "user-10";
        case USER_DEFINED_11:        return "user-11";
        case USER_DEFINED_12:        return "user-12";
        case USER_DEFINED_13:        return "user-13";
        case USER_DEFINED_14:        return "user-14";
        case USER_DEFINED_15:        return "user-15";
    }
    ASSERT_not_reachable("invalid reason");
}

// class method
Sawyer::CommandLine::ValueParser::Ptr
Reachability::reasonParser(ReasonFlags &reasons) {
    ReasonSaver::Ptr saver = ReasonSaver::instance(reasons);
    return Sawyer::CommandLine::listParser(Sawyer::CommandLine::EnumParser<Reason>::instance(saver)
                                           ->with(reasonArgument(NOT_REACHABLE), NOT_REACHABLE)
                                           ->with(reasonArgument(PROGRAM_ENTRY_POINT), PROGRAM_ENTRY_POINT)
                                           ->with(reasonArgument(EXPORTED_FUNCTION), EXPORTED_FUNCTION)
                                           ->with(reasonArgument(SIGNAL_HANDLER), SIGNAL_HANDLER)
                                           ->with(reasonArgument(ASSUMED), ASSUMED)
                                           ->with(reasonArgument(EXPLICIT_MEM_CONSTANT), EXPLICIT_MEM_CONSTANT)
                                           ->with(reasonArgument(EXPLICIT_INSN_CONSTANT), EXPLICIT_INSN_CONSTANT)
                                           ->with(reasonArgument(IMPLICIT_FUNC_CONSTANT), IMPLICIT_FUNC_CONSTANT)
                                           ->with(reasonArgument(USER_DEFINED_0), USER_DEFINED_0)
                                           ->with(reasonArgument(USER_DEFINED_1), USER_DEFINED_1)
                                           ->with(reasonArgument(USER_DEFINED_2), USER_DEFINED_2)
                                           ->with(reasonArgument(USER_DEFINED_3), USER_DEFINED_3)
                                           ->with(reasonArgument(USER_DEFINED_4), USER_DEFINED_4)
                                           ->with(reasonArgument(USER_DEFINED_5), USER_DEFINED_5)
                                           ->with(reasonArgument(USER_DEFINED_6), USER_DEFINED_6)
                                           ->with(reasonArgument(USER_DEFINED_7), USER_DEFINED_7)
                                           ->with(reasonArgument(USER_DEFINED_8), USER_DEFINED_8)
                                           ->with(reasonArgument(USER_DEFINED_9), USER_DEFINED_9)
                                           ->with(reasonArgument(USER_DEFINED_10), USER_DEFINED_10)
                                           ->with(reasonArgument(USER_DEFINED_11), USER_DEFINED_11)
                                           ->with(reasonArgument(USER_DEFINED_12), USER_DEFINED_12)
                                           ->with(reasonArgument(USER_DEFINED_13), USER_DEFINED_13)
                                           ->with(reasonArgument(USER_DEFINED_14), USER_DEFINED_14)
                                           ->with(reasonArgument(USER_DEFINED_15), USER_DEFINED_15));
}

// class method
std::string
Reachability::reasonArgumentDocumentation() {
    return
        "The @v{reasons} may be one or more of the following strings (comma-separated): "
        "\"" + reasonArgument(PROGRAM_ENTRY_POINT) + "\", "
        "\"" + reasonArgument(EXPORTED_FUNCTION) + "\", "
        "\"" + reasonArgument(SIGNAL_HANDLER) + "\", "
        "\"" + reasonArgument(ASSUMED) + "\", "
        "\"" + reasonArgument(EXPLICIT_MEM_CONSTANT) + "\", "
        "\"" + reasonArgument(EXPLICIT_INSN_CONSTANT) + "\", "
        "\"" + reasonArgument(IMPLICIT_FUNC_CONSTANT) + "\", "
        "\"" + reasonArgument(USER_DEFINED_0) + "\" where the final integer can be zero through 15 (inclusive). "
        "The value \"none\" is special in that it clears the vector, thus all preceding words are ignored.";
}

// class method
void
Reachability::insertReasonSwitch(Sawyer::CommandLine::SwitchGroup &sg, const std::string &switchName, ReasonFlags &storage,
                                 Reason dfltArg, const std::string &doc) {

    std::string noSwitchDefaults = "The default if this switch is not specified is ";
    if (storage.isAnySet()) {
        ReasonFlags::Vector leftovers(0);
        std::vector<Reason> dfltReasons = storage.split(stringify::Rose::BinaryAnalysis::Reachability::Reason(), leftovers /*out*/);
        ASSERT_require2(leftovers == ReasonFlags::Vector(0), "default value has unnamed bits set");
        std::vector<std::string> dfltNames;
        BOOST_FOREACH (Reason reason, dfltReasons)
            dfltNames.push_back("\"" + reasonArgument(reason) + "\"");
        noSwitchDefaults += StringUtility::joinEnglish(dfltNames) + ".";
    } else {
        noSwitchDefaults += "\"" + reasonArgument(NOT_REACHABLE) + "\".";
    }

    if (dfltArg != NOT_REACHABLE) {
        sg.insert(Sawyer::CommandLine::Switch(switchName)
                  .argument("reasons", reasonParser(storage), reasonArgument(dfltArg))
                  .doc(doc + " " + reasonArgumentDocumentation() + " "

                       "If the switch is specified but no argument given, then an argument of \"" +
                       reasonArgument(dfltArg) + "\" is assumed. "

                       "The @s{no-" + switchName + "} switch disables this feature, as does giving "
                       "\"" + reasonArgument(NOT_REACHABLE) + "\" as the argument. " +

                       noSwitchDefaults));
    } else {
        sg.insert(Sawyer::CommandLine::Switch(switchName)
                  .argument("reasons", reasonParser(storage))
                  .doc(doc + " " + reasonArgumentDocumentation() + " "

                       "The @s{no-" + switchName + "} switch disables this feature, as does giving "
                       "\"" + reasonArgument(NOT_REACHABLE) + "\" as the argument. " +

                       noSwitchDefaults));
    }

    sg.insert(Sawyer::CommandLine::Switch("no-" + switchName)
              .key(switchName)
              .intrinsicValue(reasonArgument(NOT_REACHABLE), reasonParser(storage))
              .hidden(true));
}

// class method
Sawyer::CommandLine::SwitchGroup
Reachability::commandLineSwitches(Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("Reachability switches");
    sg.name("reachabilitity");

    insertReasonSwitch(sg, "mark-entry-points", settings.markingEntryFunctions, PROGRAM_ENTRY_POINT,
                       "Mark all program entry points as intrinsically reachable.");

    insertReasonSwitch(sg, "mark-exports", settings.markingExportFunctions, EXPORTED_FUNCTION,
                       "Mark the entry points of all exported functions since we can assume that other modules (libraries, "
                       "programs) may invoke these.");

    insertReasonSwitch(sg, "mark-memory-referents", settings.markingExplicitMemoryReferents, EXPLICIT_MEM_CONSTANT,
                       "Scan virtual memory to find values that might be references to basic blocks and mark those blocks "
                       "as intrinsically reachable.");

    insertReasonSwitch(sg, "mark-insn-referents", settings.markingExplicitInstructionReferents, EXPLICIT_INSN_CONSTANT,
                       "Scan reachable instructions looking for constants that might be references to other basic blocks "
                       "and mark those blocks as intrinsically reachable. This occurs iteratively while reachability is "
                       "being propagated through the control flow graph. Note that this scan does not occur for blocks "
                       "that are not reachable.");

    insertReasonSwitch(sg, "mark-func-referents", settings.markingImplicitFunctionReferents, IMPLICIT_FUNC_CONSTANT,
                       "During reachability propagation, if a basic block becomes reachable then a symbolic data-flow analysis "
                       "is performed on the function(s) to which it belongs. The semantic states for all the instructions in the "
                       "function are examined to find constants, and any constants that correspond to basic block addresses "
                       "cause that basic block to be marked as intrinsically reachable.  This analysis happens iteratively "
                       "while reachability is being propagated through the control flow graph, but happens at most once per "
                       "function. Note that this analysis does not occur for functions with no reachable blocks.");

    sg.insert(Switch("address-alignment")
              .argument("nbytes", nonNegativeIntegerParser(settings.addressAlignment))
              .doc("Alignment requirements for reading addresses from memory. A value of zero means that addresses have "
                   "the same alignment as their size. The default is " +
                   StringUtility::plural(settings.addressAlignment, "bytes") + "."));

    sg.insert(Switch("address-size")
              .argument("nbytes", nonNegativeIntegerParser(settings.addressNBytes))
              .doc("Size of addresses in bytes. A value of zero means that the address size is the same as the specimen "
                   "architecture word size. The default is " + StringUtility::plural(settings.addressNBytes, "bytes") + "."));

    sg.insert(Switch("byte-order")
              .argument("order", enumParser<ByteOrder::Endianness>(settings.byteOrder)
                        ->with("little-endian", ByteOrder::ORDER_LSB)
                        ->with("big-endian", ByteOrder::ORDER_MSB)
                        ->with("none", ByteOrder::ORDER_UNSPECIFIED))
              .doc("Order that bytes of a multi-byte value are stored in memory. The choices for @v{order} are "
                   "\"little-endian\", \"big-endian\", and \"none\", where \"none\" means that the order should be obtained "
                   "from the specimen's architecture description. The default is " +
                   std::string(ByteOrder::ORDER_UNSPECIFIED == settings.byteOrder ? "\"none\"" :
                               ByteOrder::ORDER_LSB == settings.byteOrder ? "\"little-endian\"" : "\"big-endian\"") + "."));

    sg.insert(Switch("analysis-threads")
              .argument("n", nonNegativeIntegerParser(settings.nThreads))
              .doc("Amount of parallelism to use for reachability analysis. If this switch is not specified then the globally-set "
                   "parallelism amount is used. If @v{n} is zero then the parallelism will be chosen based on hardware."));

    return sg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Reachability functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Reachability::clear() {
    clearReachability();
    dfReferents_.clear();
    scannedVertexIds_.clear();
}

void
Reachability::clearReachability() {
    intrinsicReachability_.assign(intrinsicReachability_.size(), ReasonFlags());
    reachability_.assign(reachability_.size(), ReasonFlags());
}

void
Reachability::resize(const P2::Partitioner &partitioner) {
    intrinsicReachability_.resize(partitioner.cfg().nVertices(), ReasonFlags());
    reachability_.resize(partitioner.cfg().nVertices(), ReasonFlags());
}

size_t
Reachability::markStartingPoints(const P2::Partitioner &partitioner, MemoryMap::Ptr map/*=NULL*/) {
    Sawyer::Message::Stream trace(mlog[TRACE]);
    SAWYER_MESG(trace) <<"marking starting addresss:";
    Sawyer::Stopwatch timer;

    resize(partitioner);
    size_t nChanges = 0;

    if (settings_.markingEntryFunctions.isAnySet())
        nChanges += markEntryFunctions(partitioner, settings_.markingEntryFunctions);

    if (settings_.markingExportFunctions.isAnySet())
        nChanges += markExportFunctions(partitioner, settings_.markingExportFunctions);

    if (settings_.markingExplicitMemoryReferents.isAnySet()) {
        if (!map)
            map = partitioner.memoryMap();
        nChanges += markExplicitMemoryReferents(partitioner, map, 0, 0, ByteOrder::ORDER_UNSPECIFIED,
                                                settings_.markingExplicitMemoryReferents);
    }

    SAWYER_MESG(trace) <<"; took " <<timer <<" seconds\n";
    return nChanges;
}

size_t
Reachability::markEntryFunctions(const P2::Partitioner &partitioner, ReasonFlags how) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking entry functions";
    Sawyer::Stopwatch timer;

    resize(partitioner);
    size_t nChanges = 0;
    BOOST_FOREACH (const P2::Function::Ptr &func, partitioner.functions()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(func->address());
        ASSERT_require(partitioner.cfg().isValidVertex(vertex));
        if (0 != (func->reasons() & SgAsmFunction::FUNC_ENTRY_POINT)) {
            ReasonFlags wasReachable = isIntrinsicallyReachable(vertex->id());
            ReasonFlags reachable = wasReachable | how;
            if (reachable != wasReachable) {
                intrinsicallyReachable(vertex->id(), reachable);
                ++nChanges;
            }
        }
    }

    SAWYER_MESG(debug) <<"; " <<StringUtility::plural(nChanges, "changes") <<" in " <<timer <<" seconds\n";
    return nChanges;
}

size_t
Reachability::markExportFunctions(const P2::Partitioner &partitioner, ReasonFlags how) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking export functions";
    Sawyer::Stopwatch timer;

    resize(partitioner);
    size_t nChanges = 0;
    BOOST_FOREACH (const P2::Function::Ptr &func, partitioner.functions()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(func->address());
        ASSERT_require(partitioner.cfg().isValidVertex(vertex));
        if (0 != (func->reasons() & SgAsmFunction::FUNC_EXPORT)) {
            ReasonFlags wasReachable = isIntrinsicallyReachable(vertex->id());
            ReasonFlags reachable = wasReachable | how;
            if (reachable != wasReachable) {
                intrinsicallyReachable(vertex->id(), reachable);
                ++nChanges;
            }
        }
    }
    SAWYER_MESG(debug) <<"; " <<StringUtility::plural(nChanges, "changes") <<" in " <<timer <<" seconds\n";
    return nChanges;
}

size_t
Reachability::markExplicitInstructionReferents(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bb,
                                               ReasonFlags how) {
    ASSERT_not_null(bb);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking explicit instruction referents for " <<bb->printableName() <<"\n";
    resize(partitioner);
    std::set<size_t> vertexIds = findExplicitInstructionReferents(partitioner, bb);
    size_t nChanges = intrinsicallyReachable(vertexIds.begin(), vertexIds.end(), how);
    return nChanges;
}

size_t
Reachability::markExplicitMemoryReferents(const P2::Partitioner &partitioner, const MemoryMap::Ptr &map, size_t bytesPerWord,
                                          size_t alignment, ByteOrder::Endianness sex, ReasonFlags how) {
    ASSERT_not_null(map);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking explicit memory referents";
    Sawyer::Stopwatch timer;
    resize(partitioner);
    std::set<size_t> vertexIds = findExplicitMemoryReferents(partitioner, map, bytesPerWord, alignment, sex);
    size_t nChanges = intrinsicallyReachable(vertexIds.begin(), vertexIds.end(), how);
    SAWYER_MESG(debug) <<"; took " <<timer <<" seconds\n";
    return nChanges;
}

size_t
Reachability::markImplicitFunctionReferents(const P2::Partitioner &partitioner, const P2::Function::Ptr &function,
                                            ReasonFlags how) {
    ASSERT_not_null(function);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking implicit function referents for " <<function->printableName();
    Sawyer::Stopwatch timer;
    resize(partitioner);
    std::set<size_t> vertexIds = findImplicitFunctionReferents(partitioner, function);
    size_t nChanges = intrinsicallyReachable(vertexIds.begin(), vertexIds.end(), how);
    SAWYER_MESG(debug) <<"; took " <<timer <<" seconds\n";
    return nChanges;
}

size_t
Reachability::markSpecifiedVas(const P2::Partitioner &partitioner, const std::vector<AddressInterval> &where, ReasonFlags how) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking specified addresses";
    Sawyer::Stopwatch timer;

    resize(partitioner);
    size_t nChanges = 0;
    BOOST_FOREACH (const AddressInterval &interval, where) {
        std::vector<SgAsmInstruction*> insns = partitioner.instructionsOverlapping(interval);
        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
            P2::BasicBlock::Ptr bb = partitioner.basicBlockContainingInstruction(insn->get_address());
            ASSERT_not_null(bb);
            if (intrinsicallyReachable(partitioner.findPlaceholder(bb->address())->id(), how))
                ++nChanges;
        }
    }
    SAWYER_MESG(debug) <<"; " <<StringUtility::plural(nChanges, "changes") <<" in " <<timer <<" seconds\n";
    return nChanges;
}

size_t
Reachability::intrinsicallyReachable(size_t vertexId, ReasonFlags how) {
    if (vertexId >= intrinsicReachability_.size())
        intrinsicReachability_.resize(vertexId+1, ReasonFlags());

    if (how == intrinsicReachability_[vertexId]) {
        return 0;
    } else {
        intrinsicReachability_[vertexId] = how;
        return 1;
    }
}

bool
Reachability::hasReasons(ReasonFlags reasons, ReasonFlags any, ReasonFlags all, ReasonFlags none) {
    if (any.isAnySet()) {
        if (!(reasons & any).isAnySet())
            return false;
    } else {
        if (!reasons.isAnySet())
            return false;
    }

    if ((reasons & all) != all)
        return false;

    if ((reasons & none).isAnySet())
        return false;
    return true;
}

bool
Reachability::isIntrinsicallyReachable(size_t vertexId, ReasonFlags any, ReasonFlags all, ReasonFlags none) const {
    return hasReasons(intrinsicReachability(vertexId), any, all, none);
}

bool
Reachability::isReachable(size_t vertexId, ReasonFlags any, ReasonFlags all, ReasonFlags none) const {
    return hasReasons(reachability(vertexId), any, all, none);
}

Reachability::ReasonFlags
Reachability::intrinsicReachability(size_t vertexId) const {
    return vertexId < intrinsicReachability_.size() ? intrinsicReachability_[vertexId] : ReasonFlags();
}

const std::vector<Reachability::ReasonFlags>&
Reachability::intrinsicReachability() const {
    return intrinsicReachability_;
}

Reachability::ReasonFlags
Reachability::reachability(size_t vertexId) const {
    return vertexId < reachability_.size() ? reachability_[vertexId] : ReasonFlags();
}

const std::vector<Reachability::ReasonFlags>&
Reachability::reachability() const {
    return reachability_;
}

std::vector<size_t>
Reachability::reachableVertices(ReasonFlags any, ReasonFlags all, ReasonFlags none) const {
    std::vector<size_t> vertexIds;
    for (size_t vertexId = 0; vertexId < reachability_.size(); ++vertexId) {
        if (hasReasons(reachability_[vertexId], any, all, none))
            vertexIds.push_back(vertexId);
    }
#ifndef NDEBUG
    for (size_t i = 1; i < vertexIds.size(); ++i)
        ASSERT_require(vertexIds[i] > vertexIds[i-1]);
#endif
    return vertexIds;
}

size_t
Reachability::propagate(const P2::Partitioner &partitioner) {
    return propagateImpl(partitioner, NULL);
}

size_t
Reachability::propagate(const P2::Partitioner &partitioner, std::vector<size_t> &vertexIds) {
    return propagateImpl(partitioner, &vertexIds);
}

std::set<size_t>
Reachability::findExplicitInstructionReferents(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    std::set<size_t> vertexIds;
    BOOST_FOREACH (rose_addr_t constant, bb->explicitConstants()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(constant);
        if (vertex != partitioner.cfg().vertices().end() && vertex->value().type() == P2::V_BASIC_BLOCK)
            vertexIds.insert(vertex->id());
    }
    return vertexIds;
}

// Scan memory for reachable addresses. This is a little more complex than just reading one address at a time because:
//   1. We want it to be efficient, so we read a whole buffer at once and look for addresses in the buffer.
//   2. We need to handle alignment, byte order, and various sizes of addresses.
//   3. Depending on alignment and size, addresses can overlap each other in memory.
//   4. We need to watch out for addresses that would span two of our buffer-sized windows read from memory
//   5. We need to be careful of addresses that are 2^32 (or 2^64) to avoid overflow when incrementing
std::set<size_t>
Reachability::findExplicitMemoryReferents(const P2::Partitioner &partitioner, const MemoryMap::Ptr &map, size_t bytesPerWord,
                                          size_t alignment, ByteOrder::Endianness sex) {
    ASSERT_not_null(map);

    if (0 == bytesPerWord)
        bytesPerWord = settings_.addressNBytes;
    if (0 == bytesPerWord)
        bytesPerWord = partitioner.instructionProvider().instructionPointerRegister().nBits() / 8;

    if (0 == alignment)
        alignment = settings_.addressAlignment;
    if (0 == alignment)
        alignment = bytesPerWord;

    if (ByteOrder::ORDER_UNSPECIFIED == sex)
        sex = settings_.byteOrder;
    if (ByteOrder::ORDER_UNSPECIFIED == sex)
        sex = partitioner.instructionProvider().defaultByteOrder();

    ASSERT_require(bytesPerWord <= sizeof(rose_addr_t));
    ASSERT_not_null(map);
    alignment = std::max(alignment, (size_t)1);
    uint8_t buf[4096];                                  // arbitrary size
    rose_addr_t bufVa = map->hull().least();
    std::set<size_t> vertexIds;

    while (map->atOrAfter(bufVa).next().assignTo(bufVa)) {
        {
            rose_addr_t tmp = alignUp(bufVa, alignment);
            if (tmp < bufVa)
                break;                                  // overflow
            bufVa = tmp;
        }
        size_t bufSize = map->at(bufVa).limit(sizeof buf).read(buf).size();
        if (bufSize < bytesPerWord) {
            bufVa += bytesPerWord;
            if (bufVa <= map->hull().least())
                break;                                  // overflow
            continue;
        }

        size_t bufOffset = 0;
        while (1) {
            bufOffset = alignUp(bufOffset, alignment);
            if (bufOffset + bytesPerWord > bufSize)
                break;

            rose_addr_t targetVa = 0;
            switch (sex) {
                case ByteOrder::ORDER_UNSPECIFIED:
                    static bool messageShown = false;
                    if (!messageShown) {
                        mlog[WARN] <<"memory byte order is unspecified; assuming little endian\n";
                        messageShown = true;
                    }
                    // fall through
                case ByteOrder::ORDER_LSB:
                    for (size_t i=0; i<bytesPerWord; i++)
                        targetVa |= buf[bufOffset+i] << (8*i);
                    break;
                case ByteOrder::ORDER_MSB:
                    for (size_t i=0; i<bytesPerWord; i++)
                        targetVa = (targetVa << 8) | buf[bufOffset+i];
                    break;
            }
            P2::ControlFlowGraph::ConstVertexIterator targetVertex = partitioner.findPlaceholder(targetVa);
            if (targetVertex != partitioner.cfg().vertices().end())
                vertexIds.insert(targetVertex->id());
            ++bufOffset;
        }
        bufVa += bufOffset;
    }
    return vertexIds;
}

std::set<size_t>
Reachability::findImplicitFunctionReferents(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    std::set<size_t> vertexIds;

    BOOST_FOREACH (rose_addr_t constant, partitioner.functionDataFlowConstants(function)) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(constant);
        if (vertex != partitioner.cfg().vertices().end() && vertex->value().type() == P2::V_BASIC_BLOCK)
            vertexIds.insert(vertex->id());
    }
    return vertexIds;
}

struct DataFlowReferences {
    const P2::Partitioner &partitioner;
    Reachability::FunctionToVertexMap &byFunction;

    DataFlowReferences(const P2::Partitioner &partitioner, Reachability::FunctionToVertexMap &byFunction)
        : partitioner(partitioner), byFunction(byFunction) {}

    void operator()(size_t taskId, P2::Function::Ptr &function) {
        ASSERT_not_null(function);
        std::set<size_t> targetVertexIds = Reachability::findImplicitFunctionReferents(partitioner, function);

        static SAWYER_THREAD_TRAITS::Mutex mutex;
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
        byFunction.insert(function, targetVertexIds);
    }
};

void
Reachability::cacheAllImplicitFunctionReferents(const P2::Partitioner &partitioner) {
    std::vector<P2::Function::Ptr> functions = partitioner.functions();
    std::set<P2::Function::Ptr> functionSet(functions.begin(), functions.end());
    cacheImplicitFunctionReferents(partitioner, functionSet);
}

void
Reachability::cacheImplicitFunctionReferents(const P2::Partitioner &partitioner,
                                             const std::set<P2::Function::Ptr> &functions) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"finding implicit function referents";
    DataFlowReferences analyzer(partitioner, dfReferents_);
    Sawyer::Stopwatch timer;
    Sawyer::Container::Graph<P2::Function::Ptr> depgraph;
    BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
        if (!dfReferents_.exists(function))
            depgraph.insertVertex(function);
    }
    size_t nThreads = settings_.nThreads.orElse(Rose::CommandLine::genericSwitchArgs.threads);
    Sawyer::workInParallel(depgraph, nThreads, analyzer);
    debug <<"; took " <<timer <<" seconds\n";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The reachability propagation data-flow functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TransferFunction {
    Reachability::ReasonFlags operator()(const P2::ControlFlowGraph&, size_t vertexId, Reachability::ReasonFlags state) {
        return state;
    }

    std::string toString(Reachability::ReasonFlags state) {
        return StringUtility::toHex2(state.vector(), 32, false, false);
    }
};

struct MergeFunction {
    bool operator()(Reachability::ReasonFlags &a, Reachability::ReasonFlags b) {
        if (a == b)
            return false;
        a.set(b);
        return true;
    }
};

size_t
Reachability::propagateImpl(const P2::Partitioner &partitioner, std::vector<size_t> *vertexIds) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"propagating";
    Sawyer::Stopwatch timer;
    resize(partitioner);

    typedef DataFlow::Engine<const P2::ControlFlowGraph, ReasonFlags, TransferFunction, MergeFunction> DfEngine;
    TransferFunction xfer;
    MergeFunction merge;
    DfEngine dfEngine(partitioner.cfg(), xfer, merge);
    dfEngine.reset(NOT_REACHABLE);

    // Initialize the data-flow states for vertices with intrinsic reachability.
    for (size_t i=0; i<partitioner.cfg().nVertices(); ++i) {
        ReasonFlags r = intrinsicReachability_[i];
        if (r.isAnySet())
            dfEngine.insertStartingVertex(i, r);
    }

    // Run a data-flow analysis to propagate reachability from intrinsically reachable vertices to other vertices.
    dfEngine.runToFixedPoint();

    // Copy out all the vertex final states from the data-flow engine.
    size_t nChanges = 0;
    if (vertexIds != NULL)
        vertexIds->reserve(partitioner.cfg().nVertices());
    for (size_t vertexId = 0; vertexId < partitioner.cfg().nVertices(); ++vertexId) {
        if (dfEngine.getFinalState(vertexId) != reachability_[vertexId]) {
            reachability_[vertexId] = dfEngine.getFinalState(vertexId);
            ++nChanges;
            if (vertexIds)
                vertexIds->push_back(vertexId);
        }
    }

    SAWYER_MESG(debug) <<"; " <<StringUtility::plural(nChanges, "changes") <<" in " <<timer <<" seconds\n";
    return nChanges;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Iterative propagation and marking, in parallel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Reachability::iterationMarking(const P2::Partitioner &partitioner, const std::vector<size_t> &vertexIds) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    size_t nMarked = 0;
    std::set<P2::Function::Ptr> functionsToAnalyze;

    BOOST_FOREACH (size_t vertexId, vertexIds) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.cfg().findVertex(vertexId);
        P2::BasicBlock::Ptr bblock;
        if (vertex->value().type() == P2::V_BASIC_BLOCK)
            bblock = vertex->value().bblock();

        // Explicit constants in instructions
        if (settings_.markingExplicitInstructionReferents.isAnySet() && bblock)
            nMarked += markExplicitInstructionReferents(partitioner, bblock, settings_.markingExplicitInstructionReferents);

        // Find functions that need to have the data-flow constants calculated
        if (settings_.markingImplicitFunctionReferents.isAnySet() && bblock) {
            std::vector<P2::Function::Ptr> functions = partitioner.functionsOwningBasicBlock(bblock);
            BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
                if (!dfReferents_.exists(function))
                    functionsToAnalyze.insert(function);
            }
        }
    }

    // Implicit constants in function data-flow
    cacheImplicitFunctionReferents(partitioner, functionsToAnalyze); // done in parallel
    BOOST_FOREACH (const P2::Function::Ptr &function, functionsToAnalyze) {
        const std::set<size_t> referents = dfReferents_.get(function); // must exist by now, but may be empty
        nMarked += intrinsicallyReachable(referents.begin(), referents.end(), settings_.markingImplicitFunctionReferents);
    }
    return nMarked;
}

void
Reachability::iterate(const P2::Partitioner &partitioner) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    resize(partitioner);

    // Run the data-flow to find constants in all the functions. We do this up front because we can do it in parallel,
    // although if the reachability set is small, it might be better to calculate this on demand in the loop below.
    if (settings_.precomputeImplicitFunctionReferents &&
        settings_.markingImplicitFunctionReferents.isAnySet() &&
        dfReferents_.isEmpty())
        cacheAllImplicitFunctionReferents(partitioner);

    // Before starting, make sure that we've scanned the vertices that are already reachable.
    SAWYER_MESG(debug) <<"iteration[-1] marking intrinsic reachability";
    std::vector<size_t> ids = reachableVertices();
    scannedVertexIds_.removeIfSeen(ids /*in,out*/);
    size_t nMarked = iterationMarking(partitioner, ids);
    SAWYER_MESG(debug) <<" changed " <<StringUtility::plural(nMarked, "vertices") <<"\n";

    // Iterative propgate and mark
    for (size_t passNumber = 0; true; ++passNumber) {
        SAWYER_MESG(debug) <<"iteration " <<passNumber <<"\n";

        // Propagate reachability
        SAWYER_MESG(debug) <<"iteration[" <<passNumber <<"] propagating reachability";
        propagate(partitioner, ids /*out*/);
        SAWYER_MESG(debug) <<" changed " <<StringUtility::plural(ids.size(), "vertices") <<"\n";

        // Mark other vertices by scanning those changed by the propagate step
        SAWYER_MESG(debug) <<"iteration[" <<passNumber <<"] marking intrinsic reachability";
        scannedVertexIds_.removeIfSeen(ids /*in,out*/);
        nMarked = iterationMarking(partitioner, ids);
        SAWYER_MESG(debug) <<" changed " <<StringUtility::plural(nMarked, "vertices") <<"\n";

        if (0 == nMarked)
            break;
    }
}

} // namespace
} // namespace

#endif
