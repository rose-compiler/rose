#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/DataFlow.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>

#include <Sawyer/ProgressBar.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics;
using namespace Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::CallingConvention");
        mlog.comment("computing function calling conventions");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Definition::Definition() {}

Definition::Definition(size_t wordWidth, const std::string &name, const std::string &comment,
                       const RegisterDictionary::Ptr &regDict)
    : name_(name), comment_(comment), wordWidth_(wordWidth), regDict_(regDict) {
    ASSERT_require2(0 == (wordWidth & 7) && wordWidth > 0, "word size must be a positive multiple of eight");
    ASSERT_not_null(regDict);
}

Definition::~Definition() {}

RegisterDictionary::Ptr
Definition::registerDictionary() const {
    return regDict_;
}

void
Definition::registerDictionary(const RegisterDictionary::Ptr &dict) {
    regDict_ = dict;
}

void
Definition::appendInputParameter(const ConcreteLocation &newLocation) {
#ifndef NDEBUG
    for (const ConcreteLocation &existingLocation: inputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    inputParameters_.push_back(newLocation);
}

void
Definition::appendOutputParameter(const ConcreteLocation &newLocation) {
#ifndef NDEBUG
    for (const ConcreteLocation &existingLocation: outputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    outputParameters_.push_back(newLocation);
}

RegisterParts
Definition::outputRegisterParts() const {
    RegisterParts retval;
    for (const ConcreteLocation &loc: outputParameters_) {
        if (loc.type() == ConcreteLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::inputRegisterParts() const {
    RegisterParts retval;
    for (const ConcreteLocation &loc: inputParameters_) {
        if (loc.type() == ConcreteLocation::REGISTER)
            retval.insert(loc.reg());
    }
    for (const ConcreteLocation &loc: nonParameterInputs_) {
        if (loc.type() == ConcreteLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::scratchRegisterParts() const {
    RegisterParts retval;
    for (RegisterDescriptor reg: scratchRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::calleeSavedRegisterParts() const {
    RegisterParts retval;
    for (RegisterDescriptor reg: calleeSavedRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::getUsedRegisterParts() const {
    RegisterParts retval = inputRegisterParts();
    retval |= outputRegisterParts();
    if (!stackPointerRegister_.isEmpty())
        retval.insert(stackPointerRegister_);
    if (thisParameter_.type() == ConcreteLocation::REGISTER)
        retval.insert(thisParameter_.reg());
    retval |= calleeSavedRegisterParts();
    retval |= scratchRegisterParts();
    return retval;
}

void
Definition::print(std::ostream &out) const {
    print(out, RegisterDictionary::Ptr());
}

void
Definition::print(std::ostream &out, const RegisterDictionary::Ptr &regDictOverride/*=NULL*/) const {
    using namespace StringUtility;
    ASSERT_require(regDictOverride || regDict_);
    RegisterDictionary::Ptr regDict = regDictOverride ? regDictOverride : regDict_;
    RegisterNames regNames(regDict);

    out <<cEscape(name_);
    if (!comment_.empty())
        out <<" (" <<cEscape(comment_) <<")";
    out <<" = {" <<wordWidth_ <<"-bit words";

    if (instructionPointerRegister_)
        out <<", instructionAddressLocation=" <<regNames(instructionPointerRegister_);

    if (returnAddressLocation_.isValid()) {
        out <<", returnAddress=";
        returnAddressLocation_.print(out, regDict_);
    }

    if (!inputParameters_.empty()) {
        out <<", input-parameters={";
        for (const ConcreteLocation &loc: inputParameters_) {
            out <<" ";
            loc.print(out, regDict ? regDict : loc.registerDictionary());
        }
        out <<" }";
    }

    if (!nonParameterInputs_.empty()) {
        out <<", non-parameter-inputs={";
        for (const ConcreteLocation &loc: nonParameterInputs_) {
            out <<" ";
            loc.print(out, regDict ? regDict : loc.registerDictionary());
        }
        out <<" }";
    }

    if (stackParameterOrder_ != StackParameterOrder::UNSPECIFIED) {
        out <<", implied={";
        switch (stackParameterOrder_) {
            case StackParameterOrder::LEFT_TO_RIGHT: out <<" left-to-right"; break;
            case StackParameterOrder::RIGHT_TO_LEFT: out <<" right-to-left"; break;
            case StackParameterOrder::UNSPECIFIED: ASSERT_not_reachable("invalid stack parameter order");
        }

        if (!stackPointerRegister_.isEmpty()) {
            out <<" " <<regNames(stackPointerRegister_) <<"-based stack";
        } else {
            out <<" NO-STACK-REGISTER";
        }

        switch (stackCleanup_) {
            case StackCleanup::BY_CALLER: out <<" cleaned up by caller"; break;
            case StackCleanup::BY_CALLEE: out <<" cleaned up by callee"; break;
            case StackCleanup::UNSPECIFIED: out <<" with UNSPECIFIED cleanup"; break;
        }
        out <<" }";
    }

    if (nonParameterStackSize_ > 0)
        out <<", " <<nonParameterStackSize_ <<"-byte return";

    if (stackParameterOrder_ != StackParameterOrder::UNSPECIFIED || nonParameterStackSize_ > 0) {
        switch (stackDirection_) {
            case StackDirection::GROWS_UP: out <<", upward-growing stack"; break;
            case StackDirection::GROWS_DOWN: out <<", downward-growing stack"; break;
        }
    }

    if (thisParameter_.isValid()) {
        out <<", this=";
        thisParameter_.print(out, regDict ? regDict : thisParameter_.registerDictionary());
    }

    if (!outputParameters_.empty()) {
        out <<", outputs={";
        for (const ConcreteLocation &loc: outputParameters_) {
            out <<" ";
            loc.print(out, regDict ? regDict : loc.registerDictionary());
        }
        out <<" }";
    }

    if (!scratchRegisters_.empty()) {
        out <<", scratch={";
        for (RegisterDescriptor loc: scratchRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }

    if (!calleeSavedRegisters_.empty()) {
        out <<", saved={";
        for (RegisterDescriptor loc: calleeSavedRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }
}

std::ostream&
operator<<(std::ostream &out, const Definition &x) {
    x.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Analysis::Analysis()
    : hasResults_(false), didConverge_(false) {}

Analysis::Analysis(const Disassembler::BasePtr &d)
    : hasResults_(false), didConverge_(false) {
    init(d);
}

Analysis::Analysis(const InstructionSemantics::BaseSemantics::Dispatcher::Ptr &cpu)
    : cpu_(cpu), hasResults_(false), didConverge_(false) {}

Analysis::~Analysis() {}

void
Analysis::init(const Disassembler::Base::Ptr &disassembler) {
    if (disassembler) {
        Architecture::Base::ConstPtr arch = disassembler->architecture();
        RegisterDictionary::Ptr registerDictionary = arch->registerDictionary();
        ASSERT_not_null(registerDictionary);

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        auto ops = SymbolicSemantics::RiscOperators::instanceFromRegisters(registerDictionary, solver);

        cpu_ = arch->newInstructionDispatcher(ops);
    }
}

void
Analysis::clearResults() {
    hasResults_ = didConverge_ = false;
    restoredRegisters_.clear();
    inputRegisters_.clear();
    outputRegisters_.clear();
    inputStackParameters_.clear();
    outputStackParameters_.clear();
    stackDelta_ = Sawyer::Nothing();
}

void
Analysis::clearNonResults() {
    cpu_ = Dispatcher::Ptr();
}

RegisterDictionary::Ptr
Analysis::registerDictionary() const {
    return regDict_;
}

void
Analysis::registerDictionary(const RegisterDictionary::Ptr &d) {
    regDict_ = d;
}

class TransferFunction: public P2::DataFlow::TransferFunction {
    using Super = P2::DataFlow::TransferFunction;
    P2::Partitioner::ConstPtr partitioner_;

public:
    std::string dfEngineName;

public:
    TransferFunction(const P2::Partitioner::ConstPtr &partitioner, const Dispatcher::Ptr &cpu)
        : Super(cpu), partitioner_(partitioner) {
        ASSERT_not_null(partitioner);
    }

    // Just add some debugging to the P2::DataFlow::TransferFunction.
    State::Ptr operator()(const P2::DataFlow::DfCfg &dfCfg, size_t vertexId, const State::Ptr &incomingState) const {
        Sawyer::Message::Stream out(Rose::BinaryAnalysis::DataFlow::mlog[DEBUG]);
        if (out) {
            P2::DataFlow::DfCfg::ConstVertexIterator vertex = dfCfg.findVertex(vertexId);
            const std::string prefix = dfEngineName + (dfEngineName.empty()?"":": ") + "  ";
            ASSERT_require(vertex != dfCfg.vertices().end());
            switch (vertex->value().type()) {
                case P2::DataFlow::DfCfgVertex::BBLOCK: {
                    out <<prefix <<"vertex #" <<vertex->id() <<": " <<vertex->value().bblock()->printableName() <<"\n";
                    if (auto parentFunction = vertex->value().parentFunction())
                        out <<prefix <<"  in " <<parentFunction->printableName() <<"\n";
                    for (const auto &edge: vertex->inEdges())
                        out <<prefix <<"  cfg from vertex #" <<edge.source()->id() <<"\n";
                    auto unparser = partitioner_->unparser()->copy();
                    unparser->settings().linePrefix = prefix + "    ";
                    unparser->settings().bblock.cfg.showingPredecessors = false;
                    unparser->settings().bblock.cfg.showingSuccessors = false;
                    unparser->settings().bblock.cfg.showingSharing = false;
                    unparser->settings().bblock.cfg.showingArrows = false;
                    unparser->unparse(out, partitioner_, vertex->value().bblock());
                    for (const auto &edge: vertex->outEdges())
                        out <<prefix <<"  cfg to vertex #" <<edge.target()->id() <<"\n";
                    break;
                }

                case P2::DataFlow::DfCfgVertex::FAKED_CALL:
                    out <<prefix <<"vertex #" <<vertex->id() <<": faked call to ";
                    if (auto callee = vertex->value().callee()) {
                        out <<callee->printableName() <<"\n";
                    } else {
                        out <<"indeterminate address\n";
                    }
                    if (auto parentFunction = vertex->value().parentFunction())
                        out <<prefix <<"  called from " <<parentFunction->printableName() <<"\n";
                    for (const auto &edge: vertex->inEdges())
                        out <<prefix <<"  cfg from vertex #" <<edge.source()->id() <<"\n";
                    for (const auto &edge: vertex->outEdges())
                        out <<prefix <<"  cfg to vertex #" <<edge.target()->id() <<"\n";
                    break;

                case P2::DataFlow::DfCfgVertex::FUNCRET:
                    out <<prefix <<"vertex #" <<vertex->id() <<": function return from "
                        <<vertex->value().parentFunction()->printableName() <<"\n";
                    for (const auto &edge: vertex->inEdges())
                        out <<prefix <<"  cfg from vertex #" <<edge.source()->id() <<"\n";
                    for (const auto &edge: vertex->outEdges())
                        out <<prefix <<"  cfg to vertex #" <<edge.target()->id() <<"\n";
                    break;

                case P2::DataFlow::DfCfgVertex::INDET:
                    out <<prefix <<"vertex #" <<vertex->id() <<": indeterminate address\n";
                    for (const auto &edge: vertex->inEdges())
                        out <<prefix <<"  cfg from vertex #" <<edge.source()->id() <<"\n";
                    for (const auto &edge: vertex->outEdges())
                        out <<prefix <<"  cfg to vertex #" <<edge.target()->id() <<"\n";
                    break;
            }
        }
        return Super::operator()(dfCfg, vertexId, incomingState);
    }
};

void
Analysis::analyzeFunction(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    mlog[DEBUG] <<"analyzing " <<function->printableName() <<"\n";
    clearResults();

    // Build the CFG used by the dataflow: dfCfg.  The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), partitioner->findPlaceholder(function->address()));
#if 0 // DEBUGGING [Robb P Matzke 2017-02-24]
    {
        boost::filesystem::path debugDir = "./rose-debug/BinaryAnalysis/CallingConvention";
        boost::filesystem::create_directories(debugDir);
        boost::filesystem::path fileName = debugDir /
                                           ("F_" + StringUtility::addrToString(function->address()).substr(2) + ".dot");
        std::ofstream f(fileName.string().c_str());
        P2::DataFlow::dumpDfCfg(f, dfCfg);
    }
#endif
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    for (const DfCfg::Vertex &vertex: dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
            returnVertex = dfCfg.findVertex(vertex.id());
            break;
        }
    }
    if (returnVertex == dfCfg.vertices().end()) {
        mlog[DEBUG] <<"  function CFG has no return vertex\n";
        return;
    }

    // Build the dataflow engine.  If an instruction dispatcher is already provided then use it, otherwise create one and store
    // it in this analysis object.
    typedef DataFlow::Engine<DfCfg, State::Ptr, TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    if (!cpu_ && NULL==(cpu_ = partitioner->newDispatcher(partitioner->newOperators()))) {
        mlog[DEBUG] <<"  no instruction semantics\n";
        return;
    }
    P2::DataFlow::MergeFunction merge(cpu_);
    TransferFunction xfer(partitioner, cpu_);
    xfer.defaultCallingConvention(defaultCc_);
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.name("calling-convention");
    xfer.dfEngineName = dfEngine.name();
    size_t maxIterations = dfCfg.nVertices() * 5;       // arbitrary
    dfEngine.maxIterations(maxIterations);
    regDict_ = cpu_->registerDictionary();

    // Build the initial state
    State::Ptr initialState = xfer.initialState();
    RegisterStateGeneric::Ptr initialRegState = RegisterStateGeneric::promote(initialState->registerState());
    initialRegState->initialize_large();
#if 0 // [Robb Matzke 2022-07-12]
    // Initializing the stack pointer register to a constant value will interfere with detecting local variables, since the
    // detection looks for the stack pointer's initial value plus a constant. If the stack pointer were initialized to a
    // constant here, then all such offsets from the stack pointer would be simplified to just constants and the local variable
    // detection wouldn't be able to find anything. However, it might also be possible to change the local variable detector
    // so it looks for constants that are near the stack pointer's concrete value. [Robb Matzke 2022-07-12]
    const RegisterDescriptor SP = partitioner->instructionProvider().stackPointerRegister();
    rose_addr_t initialStackPointer = 0xcf000000;       // arbitrary
    initialRegState->writeRegister(SP, cpu_->operators()->number_(SP.nBits(), initialStackPointer),
                                   cpu_->operators().get());
#endif
    // x86 has segment registers ss, ds, and cs that should be initialized to zero.  The local variable detector gets confused
    // when local variable addresses have a segment component. Setting them to zero causes them to be simplified out of the
    // addresses.
    if (RegisterDescriptor ss = regDict_->find("ss"))
        initialState->writeRegister(ss, cpu_->operators()->number_(ss.nBits(), 0), cpu_->operators().get());
    if (RegisterDescriptor cs = regDict_->find("cs"))
        initialState->writeRegister(cs, cpu_->operators()->number_(cs.nBits(), 0), cpu_->operators().get());
    if (RegisterDescriptor ds = regDict_->find("ds"))
        initialState->writeRegister(ds, cpu_->operators()->number_(ds.nBits(), 0), cpu_->operators().get());

    // Run data flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(maxIterations, mlog[MARCH], function->printableName());
        progress.suffix(" iterations");
        dfEngine.reset(State::Ptr());
        dfEngine.insertStartingVertex(startVertexId, initialState);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    } catch (const BaseSemantics::NotImplemented &e) {
        mlog[WHERE] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    } catch (const BaseSemantics::Exception &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    }

    // Get the final dataflow state
    State::Ptr finalState = dfEngine.getInitialState(returnVertex->id());
    if (finalState == NULL) {
        mlog[DEBUG] <<"  data flow analysis did not reach final state\n";
        return;
    }
    if (mlog[DEBUG]) {
        if (!converged) {
            mlog[DEBUG] <<"  data flow analysis did not converge to a solution (using partial solution)\n";
        } else {
            SymbolicSemantics::Formatter fmt;
            fmt.set_line_prefix("    ");
            fmt.expr_formatter.max_depth = 10;          // prevent really long output
            mlog[DEBUG] <<"  final state:\n" <<(*finalState+fmt);
        }
    }
    RegisterStateGeneric::Ptr finalRegs = RegisterStateGeneric::promote(finalState->registerState());

    // Update analysis results
    updateRestoredRegisters(initialState, finalState);
    updateInputRegisters(finalState);
    updateOutputRegisters(finalState);
    updateStackParameters(function, initialState, finalState);
    updateStackDelta(initialState, finalState);
    hasResults_ = true;
    didConverge_ = converged;

    SAWYER_MESG(mlog[DEBUG]) <<"  analysis results for " <<function->printableName() <<": " <<*this <<"\n";
}

void
Analysis::updateRestoredRegisters(const State::Ptr &initialState, const State::Ptr &finalState) {
    restoredRegisters_.clear();

    RegisterStateGeneric::Ptr initialRegs = RegisterStateGeneric::promote(initialState->registerState());
    RegisterStateGeneric::Ptr finalRegs = RegisterStateGeneric::promote(finalState->registerState());
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperators::Ptr ops = cpu_->operators();

    InputOutputPropertySet props;
    props.insert(IO_READ_BEFORE_WRITE);
    props.insert(IO_WRITE);
    for (RegisterDescriptor reg: finalRegs->findProperties(props)) {
        SValue::Ptr initialValue = initialRegs->peekRegister(reg, ops->undefined_(reg.nBits()), ops.get());
        SValue::Ptr finalValue = finalRegs->peekRegister(reg, ops->undefined_(reg.nBits()), ops.get());
        SymbolicExpression::Ptr initialExpr = SymbolicSemantics::SValue::promote(initialValue)->get_expression();
        SymbolicExpression::Ptr finalExpr = SymbolicSemantics::SValue::promote(finalValue)->get_expression();
        if (finalExpr->flags() == initialExpr->flags() && finalExpr->mustEqual(initialExpr, ops->solver()))
            restoredRegisters_.insert(reg);
    }
}

void
Analysis::updateInputRegisters(const State::Ptr &state) {
    inputRegisters_.clear();
    RegisterStateGeneric::Ptr regs = RegisterStateGeneric::promote(state->registerState());
    for (RegisterDescriptor reg: regs->findProperties(IO_READ_BEFORE_WRITE))
        inputRegisters_.insert(reg);
    inputRegisters_ -= restoredRegisters_;
}

void
Analysis::updateOutputRegisters(const State::Ptr &state) {
    outputRegisters_.clear();
    RegisterStateGeneric::Ptr regs = RegisterStateGeneric::promote(state->registerState());
    for (RegisterDescriptor reg: regs->findProperties(IO_WRITE))
        outputRegisters_.insert(reg);
    outputRegisters_ -= restoredRegisters_;
}

void
Analysis::updateStackParameters(const P2::Function::Ptr &function, const State::Ptr &initialState, const State::Ptr &finalState) {
    inputStackParameters_.clear();
    outputStackParameters_.clear();

    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperators::Ptr ops = cpu_->operators();
    RegisterDescriptor SP = cpu_->stackPointerRegister();
    SValue::Ptr initialStackPointer = initialState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    ops->currentState(finalState);
    Variables::StackVariables vars = P2::DataFlow::findFunctionArguments(function, ops, initialStackPointer);
    for (const Variables::StackVariable &var: vars.values()) {
        if (var.ioProperties().exists(IO_READ_BEFORE_WRITE)) {
            inputStackParameters_.insert(var.interval(), var);
        } else if (var.ioProperties().exists(IO_WRITE) && var.ioProperties().exists(IO_READ_AFTER_WRITE)) {
            outputStackParameters_.insert(var.interval(), var);
        }
    }
}

void
Analysis::updateStackDelta(const State::Ptr &initialState, const State::Ptr &finalState) {
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperators::Ptr ops = cpu_->operators();
    RegisterDescriptor SP = cpu_->stackPointerRegister();
    SValue::Ptr initialStackPointer = initialState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    SValue::Ptr finalStackPointer = finalState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    SValue::Ptr stackDelta = ops->subtract(finalStackPointer, initialStackPointer);
    stackDelta_ = stackDelta->toSigned();
}

void
Analysis::print(std::ostream &out, bool multiLine) const {
    RegisterNames regName(regDict_);
    std::string separator;

    if (!inputRegisters_.isEmpty() || !inputStackParameters_.isEmpty()) {
        out <<separator <<"inputs={";
        if (!inputRegisters_.isEmpty()) {
            for (RegisterDescriptor reg: inputRegisters_.listAll(regDict_))
                out <<" " <<regName(reg);
        }
        if (!inputStackParameters_.isEmpty()) {
            Variables::StackVariables vars = inputStackParameters();
            for (const Variables::StackVariable &var: vars.values())
                out <<" stack[" <<var.frameOffset() <<"]+" <<var.maxSizeBytes();
        }
        out <<" }";
        separator = multiLine ? "\n" : ", ";
    }

    if (!outputRegisters_.isEmpty() || !outputStackParameters_.isEmpty()) {
        out <<separator <<"outputs={";
        if (!outputRegisters_.isEmpty()) {
            for (RegisterDescriptor reg: outputRegisters_.listAll(regDict_))
                out <<" " <<regName(reg);
        }
        if (!outputStackParameters_.isEmpty()) {
            Variables::StackVariables vars = outputStackParameters();
            for (const Variables::StackVariable &var: vars.values())
                out <<" stack[" <<var.frameOffset() <<"]+" <<var.maxSizeBytes();
        }
        out <<" }";
        separator = multiLine ? "\n" : ", ";
    }

    if (!restoredRegisters_.isEmpty()) {
        out <<separator <<"saved={";
        for (RegisterDescriptor reg: restoredRegisters_.listAll(regDict_))
            out <<" " <<regName(reg);
        out <<" }";
        separator = multiLine ? "\n" : ", ";
    }

    if (stackDelta_) {
        out <<separator <<"stackDelta=" <<(*stackDelta_>=0?"+":"") <<*stackDelta_;
        separator = multiLine ? "\n" : ", ";
    }

    if (separator.empty())
        out <<"no I/O";
}

bool
Analysis::match(const Definition::Ptr &cc) const {
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    ASSERT_not_null(cc);

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"matching calling convention definition to analysis\n"
                       <<"  definition: " <<*cc <<"\n"
                       <<"  analysis results: " <<*this <<"\n";

    if (!hasResults_) {
        SAWYER_MESG(debug) <<"  mismatch: no analysis results\n";
        return false;
    }

    if (cc->wordWidth() != cpu_->stackPointerRegister().nBits()) {
        SAWYER_MESG(debug) <<"  mismatch: defn word size (" <<cc->wordWidth() <<") != analysis word size ("
                           <<cpu_->stackPointerRegister().nBits() <<")\n";
        return false;
    }

    // Gather up definition's input registers. We always add EIP (or similar) because the analysis will have read it to obtain
    // the function's first instruction before ever writing to it.  Similarly, we add ESP (or similar) because pushing,
    // popping, aligning, and allocating local variable space all read ESP before writing to it.
    RegisterParts ccInputRegisters = cc->inputRegisterParts();
    ccInputRegisters.insert(cpu_->instructionPointerRegister());
    ccInputRegisters.insert(cpu_->stackPointerRegister());
    if (cc->thisParameter().type() == ConcreteLocation::REGISTER)
        ccInputRegisters.insert(cc->thisParameter().reg());

    // Gather up definition's output registers.  We always add EIP (or similar) because the final RET instruction will write
    // the return address into the EIP register and not subsequently read it. The stack pointer register is not added by
    // default because not all functions use the stack (e.g., architectures that have link registers); it must be added (or
    // not) when the definition is created.
    RegisterParts ccOutputRegisters = cc->outputRegisterParts() | cc->scratchRegisterParts();
    ccOutputRegisters.insert(cpu_->instructionPointerRegister());

    // Stack delta checks
    if (stackDelta_) {
        int64_t normalization = (cc->stackDirection() == StackDirection::GROWS_UP ? -1 : +1);
        int64_t normalizedStackDelta = *stackDelta_ * normalization; // in bytes

        // All callees must pop the non-parameter area (e.g., return address) of the stack.
        if (normalizedStackDelta < 0 || (uint64_t)normalizedStackDelta < cc->nonParameterStackSize()) {
            SAWYER_MESG(debug) <<"  mismatch: callee did not pop " <<cc->nonParameterStackSize() <<"-byte"
                               <<" non-parameter area from stack\n";
            return false;
        }
        normalizedStackDelta -= cc->nonParameterStackSize();

        switch (cc->stackCleanup()) {
            case StackCleanup::BY_CALLER:
                if (normalizedStackDelta != 0) {
                    // Any stack arguments were already pushed by the caller and will be popped by the caller. If instead the
                    // callee pops them, then the stack delta will be non-zero.
                    SAWYER_MESG(debug) <<"  mismatch: callee popped stack parameters but definition is caller-cleanup\n";
                    return false;
                }
                break;

            case StackCleanup::BY_CALLEE: {
                // The callee must pop all the stack variables. It's required to pop all its arguments, even those it didn't
                // use.
                int64_t normalizedEnd = 0; // one-past first-pushed argument normalized for downward-growing stack
                for (const Variables::StackVariable &var: inputStackParameters_.values())
                    normalizedEnd = std::max(normalizedEnd, (int64_t)(var.frameOffset() * normalization + var.maxSizeBytes()));
                for (const Variables::StackVariable &var: outputStackParameters_.values())
                    normalizedEnd = std::max(normalizedEnd, (int64_t)(var.frameOffset() * normalization + var.maxSizeBytes()));
                if (normalizedStackDelta < normalizedEnd) {
                    SAWYER_MESG(debug) <<"  mismatch: callee failed to pop callee-cleanup stack parameters\n";
                    return false;
                }
                break;
            }

            case StackCleanup::UNSPECIFIED:
                ASSERT_not_reachable("invalid stack cleanup");
        }
    } else {
        SAWYER_MESG(debug) <<"  stack delta checks not performed\n";
    }

    // All analysis output registers must be a definition's output or scratch register.
    if (!(outputRegisters_ - ccOutputRegisters).isEmpty()) {
        if (debug) {
            RegisterNames regName(registerDictionary());
            debug <<"  mismatch: actual outputs are not defined outputs or scratch registers: ";
            RegisterParts parts = outputRegisters_ - ccOutputRegisters;
            for (RegisterDescriptor reg: parts.listAll(registerDictionary()))
                debug <<" " <<regName(reg);
            debug <<"\n";
        }
        return false;
    }

    // All analysis input registers must be a definition's input parameters, non-parameter inputs, or "this" register.
    if (!(inputRegisters_ - ccInputRegisters).isEmpty()) {
        if (debug) {
            RegisterNames regName(registerDictionary());
            debug <<"  mismatch: actual inputs are not defined inputs or \"this\" register: ";
            RegisterParts parts = inputRegisters_ - ccInputRegisters;
            for (RegisterDescriptor reg: parts.listAll(registerDictionary()))
                debug <<" " <<regName(reg);
            debug <<"\n";
        }
        return false;
    }

    // All analysis restored registers must be a definition's callee-saved register.
    if (!(restoredRegisters_ - cc->calleeSavedRegisterParts()).isEmpty()) {
        if (debug) {
            debug <<"  mismatch: restored registers that are not defined as callee-saved:";
            RegisterParts parts = restoredRegisters_ - cc->calleeSavedRegisterParts();
            RegisterNames regName(registerDictionary());
            for (RegisterDescriptor reg: parts.listAll(registerDictionary()))
                debug <<" " <<regName(reg);
            debug <<"\n";
        }
        return false;
    }

    // If the definition has an object pointer ("this" parameter) then it should not be an anlysis output or scratch register,
    // but must be an analysis input register.
    if (cc->thisParameter().type() == ConcreteLocation::REGISTER) {
        if (ccOutputRegisters.existsAny(cc->thisParameter().reg())) {
            SAWYER_MESG(debug) <<"  mismatch: actual output defined as \"this\" register: "
                               <<RegisterNames(registerDictionary())(cc->thisParameter().reg()) <<"\n";
            return false;
        }
        if (!ccInputRegisters.existsAll(cc->thisParameter().reg())) {
            SAWYER_MESG(debug) <<"  mismatch: actual input does not include \"this\" register: "
                               <<RegisterNames(registerDictionary())(cc->thisParameter().reg()) <<"\n";
            return false;
        }
    }

    // If the analysis has stack inputs or outputs then the definition must have a valid stack parameter direction.
    if ((!inputStackParameters().isEmpty() || !outputStackParameters().isEmpty()) &&
        cc->stackParameterOrder() == StackParameterOrder::UNSPECIFIED) {
        SAWYER_MESG(debug) <<"  mismatch: stack parameters detected but not allowed by definition\n";
        return false;
    }

    SAWYER_MESG(debug) <<"  analysis matches definition " <<cc->name() <<"\n";
    return true;
}

Dictionary
Analysis::match(const Dictionary &conventions) const {
    Dictionary retval;
    for (const Definition::Ptr &cc: conventions) {
        if (match(cc))
            retval.push_back(cc);
    }
    return retval;
}

std::ostream&
operator<<(std::ostream &out, const Analysis &x) {
    x.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::Ptr
readArgument(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef, size_t argNumber) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("reading function argument #" + boost::lexical_cast<std::string>(argNumber) +
                 " using calling convention " + ccDef->name());
    const size_t nBits = ccDef->wordWidth();
    SValue::Ptr retval;
    if (argNumber < ccDef->inputParameters().size()) {
        // Argument is explicit in the definition
        const ConcreteLocation &loc = ccDef->inputParameters()[argNumber];

        switch (loc.type()) {
            case ConcreteLocation::REGISTER:
                retval = ops->readRegister(loc.reg());
                break;

            case ConcreteLocation::RELATIVE: {
                const auto base = ops->readRegister(loc.reg());
                const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
                const auto address = ops->add(base, offset);
                const auto dflt = ops->undefined_(nBits);
                retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
                break;
            }

            case ConcreteLocation::ABSOLUTE: {
                auto address = ops->number_(64, loc.address());
                const auto dflt = ops->undefined_(nBits);
                retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
                break;
            }

            case ConcreteLocation::NO_LOCATION:
                break;
        }

    } else {
        // Argument is at an implied stack location
        argNumber = argNumber - ccDef->inputParameters().size();
        switch (ccDef->stackParameterOrder()) {
            case StackParameterOrder::RIGHT_TO_LEFT:
                break;
            case StackParameterOrder::LEFT_TO_RIGHT:
                ASSERT_not_implemented("we need to know how many parameters were pushed");
            case StackParameterOrder::UNSPECIFIED:
                ASSERT_not_implemented("invalid stack paramter order");
        }
        int64_t stackOffset = ccDef->nonParameterStackSize() + argNumber * nBits/8;
        switch (ccDef->stackDirection()) {
            case StackDirection::GROWS_DOWN:
                break;
            case StackDirection::GROWS_UP:
                stackOffset = -stackOffset - nBits/8;
                break;
        }

        const RegisterDescriptor reg_sp = ccDef->stackPointerRegister();
        const auto base = ops->readRegister(reg_sp);
        const auto offset = ops->signExtend(ops->number_(64, stackOffset), base->nBits());
        const auto address = ops->add(base, offset);
        const auto dflt = ops->undefined_(nBits);
        retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
    }

    ASSERT_always_not_null2(retval, "invalid parameter location");
    ops->comment("argument value is " + retval->toString());
    return retval;
}

void
writeArgument(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef, size_t argNumber, const SValue::Ptr &value) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("writing function argument #" + boost::lexical_cast<std::string>(argNumber) +
                 " using calling convention " + ccDef->name());
    const size_t nBits = ccDef->wordWidth();
    if (argNumber < ccDef->inputParameters().size()) {
        // Argument is explicit in the definition
        const ConcreteLocation &loc = ccDef->inputParameters()[argNumber];

        switch (loc.type()) {
            case ConcreteLocation::REGISTER:
                ops->writeRegister(loc.reg(), value);
                ops->comment("argument written");
                return;

            case ConcreteLocation::RELATIVE: {
                const auto base = ops->readRegister(loc.reg());
                const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
                const auto address = ops->add(base, offset);
                ops->writeMemory(RegisterDescriptor(), address, value, ops->boolean_(true));
                ops->comment("argument written");
                return;
            }

            case ConcreteLocation::ABSOLUTE: {
                auto address = ops->number_(64, loc.address());
                ops->writeMemory(RegisterDescriptor(), address, value, ops->boolean_(true));
                ops->comment("argument written");
                return;
            }

            case ConcreteLocation::NO_LOCATION:
                break;
        }
        ASSERT_not_reachable("invalid parameter location type");

    } else {
        // Argument is at an implied stack location
        ASSERT_require(argNumber >= ccDef->inputParameters().size());
        argNumber = argNumber - ccDef->inputParameters().size();
        switch (ccDef->stackParameterOrder()) {
            case StackParameterOrder::RIGHT_TO_LEFT:
                break;
            case StackParameterOrder::LEFT_TO_RIGHT:
                ASSERT_not_implemented("we need to know how many parameters were pushed");
            case StackParameterOrder::UNSPECIFIED:
                ASSERT_not_implemented("invalid stack paramter order");
        }
        int64_t stackOffset = ccDef->nonParameterStackSize() + argNumber * nBits/8;
        switch (ccDef->stackDirection()) {
            case StackDirection::GROWS_DOWN:
                break;
            case StackDirection::GROWS_UP:
                stackOffset = -stackOffset - nBits/8;
                break;
        }

        const RegisterDescriptor reg_sp = ccDef->stackPointerRegister();
        const auto base = ops->readRegister(reg_sp);
        const auto offset = ops->signExtend(ops->number_(64, stackOffset), base->nBits());
        const auto address = ops->add(base, offset);
        ops->writeMemory(RegisterDescriptor(), address, value, ops->boolean_(true));
        ops->comment("argument written");
    }
}

SValue::Ptr
readReturnValue(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("reading function return value using calling convention " + ccDef->name());

    if (ccDef->outputParameters().empty())
        throw Exception("calling convention has no output parameters");

    // Assume that the first output parameter is the main integer return location.
    const ConcreteLocation &loc = ccDef->outputParameters()[0];
    SValue::Ptr retval;
    switch (loc.type()) {
        case ConcreteLocation::REGISTER:
            retval = ops->readRegister(loc.reg());
            break;

        case ConcreteLocation::RELATIVE: {
            const auto base = ops->readRegister(loc.reg());
            const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
            const auto address = ops->add(base, offset);
            const auto dflt = ops->undefined_(loc.reg().nBits());
            retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::ABSOLUTE: {
            auto address = ops->number_(64, loc.address());
            const auto dflt = ops->undefined_(loc.reg().nBits());
            retval = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_always_not_null2(retval, "invalid parameter location type");
    ops->comment("return value is " + retval->toString());
    return retval;
}

void
writeReturnValue(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef, const SValue::Ptr &returnValue) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ASSERT_not_null(returnValue);
    ops->comment("writing function return value using calling convention " + ccDef->name());

    if (ccDef->outputParameters().empty())
        throw Exception("calling convention has no output parameters");

    // Assume that the first output parameter is the main integer return location.
    const ConcreteLocation &loc = ccDef->outputParameters()[0];
    switch (loc.type()) {
        case ConcreteLocation::REGISTER:
            ops->writeRegister(loc.reg(), returnValue);
            ops->comment("return value written");
            return;

        case ConcreteLocation::RELATIVE: {
            const auto base = ops->readRegister(loc.reg());
            const auto offset = ops->signExtend(ops->number_(64, loc.offset()), base->nBits());
            const auto address = ops->add(base, offset);
            ops->writeMemory(RegisterDescriptor(), address, returnValue, ops->boolean_(true));
            ops->comment("return value written");
            return;
        }

        case ConcreteLocation::ABSOLUTE: {
            auto address = ops->number_(64, loc.address());
            ops->writeMemory(RegisterDescriptor(), address, returnValue, ops->boolean_(true));
            ops->comment("return value written");
            return;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_reachable("invalid parameter location type");
}

void
simulateFunctionReturn(const RiscOperators::Ptr &ops, const Definition::Ptr &ccDef) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ccDef);
    ops->comment("simulating function return using calling convention " + ccDef->name());

    // Assume that the current stack pointer is the same as it was when this function was entered.
    const RegisterDescriptor SP = ccDef->stackPointerRegister();
    ASSERT_require(SP);
    const auto originalSp = ops->readRegister(SP);

    // How many input parameters to be popped from the stack by the callee?
    //
    // FIXME[Robb Matzke 2022-07-13]: This assumes that each parameter is one word, which is often not the case, but
    // we don't currently have any information about parameter sizes.
    int64_t nArgBytes = 0;
    if (StackCleanup::BY_CALLEE == ccDef->stackCleanup()) {
        for (const ConcreteLocation &loc: ccDef->inputParameters()) {
            if (ConcreteLocation::RELATIVE == loc.type())
                nArgBytes += ccDef->wordWidth() / 8;
        }
    }

    // How many other things need to be popped from the stack by the callee as part of returning?
    int64_t nNonArgBytes = ccDef->nonParameterStackSize();

    // Obtain the return address
    const ConcreteLocation &retVaLoc = ccDef->returnAddressLocation();
    SValue::Ptr retVa;
    switch (retVaLoc.type()) {
        case ConcreteLocation::REGISTER:
            retVa = ops->readRegister(retVaLoc.reg());
            break;

        case ConcreteLocation::RELATIVE: {
            const auto base = ops->readRegister(retVaLoc.reg());
            const auto offset = ops->signExtend(ops->number_(64, retVaLoc.offset()), base->nBits());
            const auto address = ops->add(base, offset);
            const auto dflt = ops->undefined_(ccDef->wordWidth());
            retVa = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::ABSOLUTE: {
            auto address = ops->number_(64, retVaLoc.address());
            const auto dflt = ops->undefined_(ccDef->wordWidth());
            retVa = ops->readMemory(RegisterDescriptor(), address, dflt, ops->boolean_(true));
            break;
        }

        case ConcreteLocation::NO_LOCATION:
            break;
    }
    ASSERT_not_null2(retVa, "unknown location for fuction return address");

    // Pop things from the stack
    SValue::Ptr newSp;
    switch (ccDef->stackDirection()) {
        case StackDirection::GROWS_DOWN:
            newSp = ops->add(originalSp, ops->number_(originalSp->nBits(), nArgBytes + nNonArgBytes));
            break;
        case StackDirection::GROWS_UP:
            newSp = ops->subtract(originalSp, ops->number_(originalSp->nBits(), nArgBytes + nNonArgBytes));
            break;
    }
    ASSERT_not_null2(newSp, "invalid stack growth direction");
    ops->writeRegister(SP, newSp);

    // Change the instruction pointer to be the return address.
    const RegisterDescriptor IP = ccDef->instructionPointerRegister();
    ops->writeRegister(IP, retVa);
    ops->comment("function return has been simulated");
}

} // namespace
} // namespace
} // namespace

#endif
