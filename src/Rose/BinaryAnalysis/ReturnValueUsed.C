#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ReturnValueUsed.h>

#include <Rose/As.h>
#include <Rose/CommandLine.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Semantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics;
using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace ReturnValueUsed {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::ReturnValueUsed");
        mlog.comment("determining whether a return value is used");
    }
}

static std::string
locationNames(const RegisterParts &parts, const RegisterDictionary::Ptr &regdict) {
    std::vector<std::string> retval;
    RegisterNames regNames(regdict);
    for (RegisterDescriptor reg: parts.listAll(regdict))
        retval.push_back(regNames(reg));
    return boost::join(retval, ", ");
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Instruction Semantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// We use mostly the Partitioner2 semantics because (1) they're symbolic-based, (2) they have a built-in size limiter to
// prevent expressions from becoming too big and are therefore fast but less precise, (3) the memory state knows about the
// concrete values of initialized memory, (4) it can use list- or map-based memory states.
typedef P2::Semantics::SValue SValue;
typedef P2::Semantics::RegisterState RegisterState;
typedef P2::Semantics::MemoryMapState MemoryState;
typedef P2::Semantics::State State;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

// Extend the Partitioner2 RISC operators so that register and memory I/O looks for reading of callee output values before
// those locations are written.
class RiscOperators: public P2::Semantics::RiscOperators {
    RegisterParts calleeOutputRegisters_;
    Variables::StackVariables calleeOutputParameters_;
    CallSiteResults *results_;
    RegisterDictionary::Ptr registerDictionary_;

public:
    using Super = P2::Semantics::RiscOperators;
    using Ptr = RiscOperatorsPtr;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(calleeOutputRegisters_);
        s & BOOST_SERIALIZATION_NVP(calleeOutputParameters_);
        s & BOOST_SERIALIZATION_NVP(results_);
        s & BOOST_SERIALIZATION_NVP(registerDictionary_);
    }
#endif

protected:
    explicit RiscOperators(const S2::BaseSemantics::SValue::Ptr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : Super(protoval, solver), results_(NULL) {}

    explicit RiscOperators(const S2::BaseSemantics::State::Ptr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : Super(state, solver), results_(NULL) {}

public:
    static RiscOperators::Ptr instance(const RegisterDictionary::Ptr &regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        SValue::Ptr protoval = SValue::instance();
        RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
        MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
        State::Ptr state = State::instance(registers, memory);
        return RiscOperators::Ptr(new RiscOperators(state, solver));
    }

    static RiscOperators::Ptr instance(const S2::BaseSemantics::SValue::Ptr &protoval,
                                     const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperators::Ptr(new RiscOperators(protoval, solver));
    }
    
    static RiscOperators::Ptr instance(const S2::BaseSemantics::State::Ptr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperators::Ptr(new RiscOperators(state, solver));
    }
    
public:
    virtual S2::BaseSemantics::RiscOperators::Ptr
    create(const S2::BaseSemantics::SValue::Ptr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) const override {
        return instance(protoval, solver);
    }

    virtual S2::BaseSemantics::RiscOperators::Ptr
    create(const S2::BaseSemantics::State::Ptr &state, const SmtSolverPtr &solver = SmtSolverPtr()) const override {
        return instance(state, solver);
    }

public:
    static RiscOperators::Ptr promote(const S2::BaseSemantics::RiscOperators::Ptr &x) {
        RiscOperators::Ptr retval = as<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    // Sets the list of registers and stack locations that a callee uses as return value locations
    void insertOutputs(const RegisterParts &regs, const Variables::StackVariables &params) {
        calleeOutputRegisters_ = regs;
        calleeOutputParameters_ = params;
    }

    // Register outputs that haven't been referenced during the instruction semntics phase.
    const RegisterParts& unreferencedRegisterOutputs() const {
        return calleeOutputRegisters_;
    }

    // Stack parameter locations that haven't been referenced during the instruction semantics phase.
    const Variables::StackVariables& unreferencedStackOutputs() const {
        return calleeOutputParameters_;
    }
    
    // True if a callee has return values
    bool hasOutputs() const {
        return !calleeOutputRegisters_.isEmpty() || !calleeOutputParameters_.isEmpty();
    }

    // Set reference to the object that holds the analysis results for a call site.  These results are updated as instructions
    // are processed.
    void callSiteResults(CallSiteResults *results) {
        results_ = results;
    }

    /** Property: Register dictionary for debugging.
     *
     *  The register dictionary is optional, and used only to convert register descriptors to register names in diagnostic
     *  output.
     *
     * @{ */
    RegisterDictionary::Ptr registerDictionary() const { return registerDictionary_; }
    void registerDictionary(const RegisterDictionary::Ptr &rd) { registerDictionary_ = rd; }
    /** @} */

public:
    virtual S2::BaseSemantics::SValue::Ptr readRegister(RegisterDescriptor reg,
                                                      const S2::BaseSemantics::SValue::Ptr &dflt) override {
        // Reading from a register that's still listed as an output means that it's definitely a used return value.
        RegisterParts found = calleeOutputRegisters_ & RegisterParts(reg);
        if (!found.isEmpty()) {
            ASSERT_not_null(results_);
            SAWYER_MESG(mlog[DEBUG]) <<"  in readRegister:  used   return locations: " // extra spaces intentional
                                     <<locationNames(found, registerDictionary_) <<"\n";
            results_->returnRegistersUsed() |= found;
            calleeOutputRegisters_ -= found;
        }
        return Super::readRegister(reg, dflt);
    }

    virtual void writeRegister(RegisterDescriptor reg, const S2::BaseSemantics::SValue::Ptr &value) override {
        // Writing to a register means that the callee's return value is definitely not used.
        RegisterParts found = calleeOutputRegisters_ & RegisterParts(reg);
        if (!found.isEmpty()) {
            ASSERT_not_null(results_);
            SAWYER_MESG(mlog[DEBUG]) <<"  in writeRegister: unused return locations: "
                                     <<locationNames(found, registerDictionary_) <<"\n";
            results_->returnRegistersUnused() |= found;
            calleeOutputRegisters_ -= found;
        }
        Super::writeRegister(reg, value);
    }

    virtual S2::BaseSemantics::SValue::Ptr readMemory(RegisterDescriptor segreg, const S2::BaseSemantics::SValue::Ptr &addr,
                                                    const S2::BaseSemantics::SValue::Ptr &dflt,
                                                    const S2::BaseSemantics::SValue::Ptr &cond) override {
        // TODO
        return Super::readMemory(segreg, addr, dflt, cond);
    }

    virtual void writeMemory(RegisterDescriptor segreg, const S2::BaseSemantics::SValue::Ptr &addr,
                             const S2::BaseSemantics::SValue::Ptr &value, const S2::BaseSemantics::SValue::Ptr &cond) override {
        // TODO
        Super::writeMemory(segreg, addr, value, cond);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CallSiteResults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CallSiteResults::CallSiteResults()
    : didConverge_(false) {}

CallSiteResults::~CallSiteResults() {}

bool
CallSiteResults::didConverge() const {
    return didConverge_;
}

void
CallSiteResults::didConverge(bool b) {
    didConverge_ = b;
}

const std::vector<Partitioner2::Function::Ptr>
CallSiteResults::callees() const {
    return callees_;
}

const RegisterParts&
CallSiteResults::returnRegistersUsed() const {
    return returnRegistersUsed_;
}

RegisterParts&
CallSiteResults::returnRegistersUsed() {
    return returnRegistersUsed_;
}

void
CallSiteResults::returnRegistersUsed(const RegisterParts &regs) {
    returnRegistersUsed_ = regs;
}

const RegisterParts&
CallSiteResults::returnRegistersUnused() const {
    return returnRegistersUnused_;
}

RegisterParts&
CallSiteResults::returnRegistersUnused() {
    return returnRegistersUnused_;
}

void
CallSiteResults::returnRegistersUnused(const RegisterParts &regs) {
    returnRegistersUnused_ = regs;
}

void
CallSiteResults::callees(const std::vector<Partitioner2::Function::Ptr> &functions) {
    callees_ = functions;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Analysis methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Analysis::Analysis()
    : assumeCallerReturnsValue_(true) {}

Analysis::~Analysis() {}

CallingConvention::Definition::Ptr
Analysis::defaultCallingConvention() const {
    return defaultCallingConvention_;
}

void
Analysis::defaultCallingConvention(const CallingConvention::Definition::Ptr &defn) {
    defaultCallingConvention_ = defn;
}

bool
Analysis::assumeCallerReturnsValue() const {
    return assumeCallerReturnsValue_;
}

void
Analysis::assumeCallerReturnsValue(bool b) {
    assumeCallerReturnsValue_ = b;
}

void
Analysis::clearResults() {
    callSites_.clear();
}

std::vector<P2::Function::Ptr>
Analysis::findCallees(const P2::Partitioner::ConstPtr &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &callSite) {
    ASSERT_not_null(partitioner);
    std::vector<P2::Function::Ptr> callees;
    for (const P2::ControlFlowGraph::Edge &edge: callSite->outEdges()) {
        if (edge.value().type() == P2::E_FUNCTION_CALL) {
            for (const P2::Function::Ptr &f: partitioner->functionsOwningBasicBlock(edge.target()))
                P2::insertUnique(callees, f, P2::sortFunctionsByAddress);
        }
    }
    return callees;
}

std::vector<P2::ControlFlowGraph::ConstVertexIterator>
Analysis::findReturnTargets(const P2::Partitioner::ConstPtr&, const P2::ControlFlowGraph::ConstVertexIterator &callSite) {
    std::vector<P2::ControlFlowGraph::ConstVertexIterator> returnVertices;
    for (const P2::ControlFlowGraph::Edge &edge: callSite->outEdges()) {
        if (edge.value().type() == P2::E_CALL_RETURN)
            returnVertices.push_back(edge.target());
    }
    return returnVertices;
}
    
CallSiteResults
Analysis::analyzeCallSite(const P2::Partitioner::ConstPtr &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &callSite) {
    ASSERT_not_null(partitioner);
    ASSERT_require(partitioner->cfg().isValidVertex(callSite));
    CallSiteResults retval;

    SAWYER_MESG(mlog[DEBUG]) <<"analyzing " <<partitioner->vertexName(callSite) <<"\n";

    // Find callers, callees, and return points in the CFG
    std::vector<P2::Function::Ptr> callers = partitioner->functionsOwningBasicBlock(callSite);
    retval.callees(findCallees(partitioner, callSite));
    std::vector<P2::ControlFlowGraph::ConstVertexIterator> returnTargets = findReturnTargets(partitioner, callSite);

    // FIXME[Robb P Matzke 2017-03-03]: To simplify things for now, handle only the case where the call site is owned by a
    // single function, a single callee is called, and it returns to a single point in the caller.
    if (callers.size() > 1) {
        mlog[ERROR] <<"multiple callers not implemented yet at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }
    if (retval.callees().size() > 1) {
        mlog[ERROR] <<retval.callees().size() <<"-call not implemented yet at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }
    if (returnTargets.size() > 1) {
        mlog[ERROR] <<returnTargets.size() <<"-return not implemented yet at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }

    // Handle the no-op cases.
    if (callers.empty()) {
        mlog[WARN] <<"no caller at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }
    if (retval.callees().empty()) {
        mlog[WARN] <<"no callee at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }
    if (returnTargets.empty()) {
        mlog[WARN] <<"no return target at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }

    // Simplified version assumes one caller owning the call site, one callee, and one return target.
    P2::Function::Ptr caller = callers[0];
    P2::Function::Ptr callee = retval.callees()[0];
    P2::ControlFlowGraph::ConstVertexIterator returnTarget = returnTargets[0];

    // Get cached calling convention properties for the callee; avoid doing a calling convention analysis here -- the user
    // should have already done that.
    const CallingConvention::Analysis &calleeBehavior = callee->callingConventionAnalysis();
    if (!calleeBehavior.hasResults() || !calleeBehavior.didConverge()) {
        mlog[WARN] <<"no calling convention behavior for " <<callee->printableName()
                   <<" called at vertex " <<partitioner->vertexName(callSite) <<"\n";
    }
    const CallingConvention::Definition::Ptr calleeDefinition = callee->callingConventionDefinition();
    if (!calleeDefinition) {
        mlog[ERROR] <<"no calling convention definition for " <<callee->printableName()
                    <<" called at vertex " <<partitioner->vertexName(callSite) <<"\n";
        return retval;
    }
    
    // Find the intersection of the calling convention definition's return value locations and the outputs based on callee
    // behavior.  This takes care of two issues: (1) some behavior-based outputs are only scratch locations according to the
    // definition, and (2) some return locations according to the definition might not have been outputs according to the
    // callee behavior.
    RegisterParts calleeReturnRegs = calleeDefinition->outputRegisterParts();
    if (calleeBehavior.hasResults() && calleeBehavior.didConverge())
        calleeReturnRegs &= calleeBehavior.outputRegisters();
    calleeReturnRegs -= RegisterParts(partitioner->instructionProvider().stackPointerRegister());
    Variables::StackVariables calleeReturnMem;
#if 0 // [Robb Matzke 2019-08-14]: turning off warning
    for (const ConcreteLocation &location: calleeDefinition->outputParameters()) {
        // FIXME[Robb P Matzke 2017-03-20]: todo
    }
#endif
    
    // Build the instruction semantics that will look for which of the callee's return values are used by the caller. "Used"
    // means (1) the caller reads the callee output location without first writing to it, or (2) the caller calls a second
    // function whose input is one of the original callee outputs with no intervening write, or (3) one of the caller's own
    // return values is one of the calle's return values with no intervening write.
    RegisterDictionary::Ptr regdict = partitioner->instructionProvider().registerDictionary();
    ASSERT_not_null(regdict);
    SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
    RiscOperators::Ptr ops = RiscOperators::instance(regdict, solver);
    ops->registerDictionary(regdict);
    ops->insertOutputs(calleeReturnRegs, calleeReturnMem);
    if (!ops->hasOutputs())
        return retval;
    S2::BaseSemantics::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
    if (!cpu) {
        mlog[ERROR] <<"no instruction semantics for this architecture\n";
        return retval;
    }

    // Build a CFG to analyze. We use a data-flow CFG even though we're not doing a true data flow, because it's convenient.
    P2::DataFlow::DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), returnTarget);
    if (mlog[DEBUG]) {
        boost::filesystem::path debugDir = "./rose-debug/BinaryAnalysis/ReturnValueUsed";
        boost::filesystem::create_directories(debugDir);
        boost::filesystem::path fileName = debugDir /
                                           ("B_" + StringUtility::addrToString(callSite->value().address()).substr(2) + ".dot");
        std::ofstream f(fileName.string().c_str());
        P2::DataFlow::dumpDfCfg(f, dfCfg);
        mlog[DEBUG] <<"  dfCfg (" <<StringUtility::plural(dfCfg.nVertices(), "vertices", "vertex") <<")"
                    <<" saved in " <<fileName <<"\n";
    }

    // Build the state transfer function
    P2::DataFlow::TransferFunction xfer(cpu);
    xfer.defaultCallingConvention(defaultCallingConvention_);

    // Build the input state for the call return vertex and allocate space for the rest. The rest are filled in during the
    // traversal.
    std::vector<State::Ptr> inputStates(dfCfg.nVertices());
    inputStates[0] = xfer.initialState();
    
    // Do a depth first-traversal of the CFG to visit all vertices reachable from the return target exactly one time each.
    ops->callSiteResults(&retval);
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<const P2::DataFlow::DfCfg> Traversal;
    for (Traversal t(dfCfg, dfCfg.findVertex(0)); t; ++t) {
        State::Ptr inputState = inputStates[t.vertex()->id()];
        ASSERT_not_null(inputState);
        if (mlog[DEBUG]) {
            std::ostringstream ss;
            ss <<*inputState;
            mlog[DEBUG] <<"  incoming state for vertex #" <<t.vertex()->id() <<"\n"
                        <<"    unresolved locations:    " <<locationNames(ops->unreferencedRegisterOutputs(), regdict) <<"\n"
                        <<"    used   return locations: " <<locationNames(retval.returnRegistersUsed(), regdict) <<"\n"
                        <<"    unused return locations: " <<locationNames(retval.returnRegistersUnused(), regdict) <<"\n"
                        <<StringUtility::prefixLines(ss.str(), "    ");
        }

        State::Ptr outputState;
        try {
            outputState = State::promote(xfer(dfCfg, t.vertex()->id(), inputState));
        } catch (const S2::BaseSemantics::NotImplemented &e) {
            mlog[WHERE] <<e.what() <<" at call site vertex " <<partitioner->vertexName(callSite) <<"\n";
            retval.didConverge(false);
            return retval;
        } catch (const S2::BaseSemantics::Exception &e) {
            mlog[WARN] <<e.what() <<" at call site vertex " <<partitioner->vertexName(callSite) <<"\n";
            retval.didConverge(false);
            return retval;
        }
        
        if (mlog[DEBUG]) {
            std::ostringstream ss;
            ss <<*outputState;
            mlog[DEBUG] <<"  outgoing state for vertex #" <<t.vertex()->id() <<"\n"
                        <<"    unresolved locations:    " <<locationNames(ops->unreferencedRegisterOutputs(), regdict) <<"\n"
                        <<"    used   return locations: " <<locationNames(retval.returnRegistersUsed(), regdict) <<"\n"
                        <<"    unused return locations: " <<locationNames(retval.returnRegistersUnused(), regdict) <<"\n"
                        <<StringUtility::prefixLines(ss.str(), "    ");
        }

        // Forward output state to the input states for vertices we have yet to traverse.
        for (const P2::DataFlow::DfCfg::Edge &edge: t.vertex()->outEdges()) {
            if (!inputStates[edge.target()->id()])
                inputStates[edge.target()->id()] = outputState;
        }
    }

    // If the caller has outputs, then read them. This is to handle situations where the caller implicitly uses the callee's
    // return value by virtue of the caller returning it. For example:
    //   callee: push ebp
    //           mov ebp, esp
    //           mov eax, 1    ; the return value
    //           leave
    //           ret
    //
    //   caller: push ebp
    //           mov ebp, esp
    //           call callee
    //           leave
    //           ret
    //
    // If caller's calling convention returns a value in EAX, then callee's return value (also in EAX) is implicitly used.
    if (assumeCallerReturnsValue_) {
        mlog[DEBUG] <<"  marking (reading) callee returns implicitly returned by caller\n";
        P2::DataFlow::DfCfg::ConstVertexIterator returnVertex = P2::DataFlow::findReturnVertex(dfCfg);
        if (dfCfg.isValidVertex(returnVertex)) {
            State::Ptr returnState = inputStates[returnVertex->id()];
            ASSERT_always_not_null(returnState);
            ops->currentState(returnState);
            for (const P2::Function::Ptr &caller: callers) {
                const CallingConvention::Analysis &callerBehavior = caller->callingConventionAnalysis();
                if (callerBehavior.didConverge()) {
                    SAWYER_MESG(mlog[DEBUG]) <<"  return from " <<caller->printableName() <<" implicitly uses: "
                                             <<locationNames(callerBehavior.outputRegisters(), regdict) <<"\n";
                    for (RegisterDescriptor reg: callerBehavior.outputRegisters().listAll(regdict))
                        (void) ops->readRegister(reg, ops->undefined_(reg.nBits()));
                }
            }
        }
    }

    SAWYER_MESG(mlog[DEBUG]) <<"  final unused return locations: "
                             <<locationNames(retval.returnRegistersUnused(), regdict) <<"\n";
    
    // Assume all unresolved return locations of the callee(s) are unused return values.
    retval.returnRegistersUnused() |= ops->unreferencedRegisterOutputs();

    retval.didConverge(true);
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
