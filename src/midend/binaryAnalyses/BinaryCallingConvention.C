#include <sage3basic.h>
#include <BinaryCallingConvention.h>

#include <BinaryDataFlow.h>                             // Dataflow engine
#include <boost/foreach.hpp>
#include <Diagnostics.h>
#include <MemoryCellList.h>
#include <Partitioner2/DataFlow.h>                      // Dataflow components that we can re-use
#include <Partitioner2/Partitioner.h>                   // Fast binary analysis data structures
#include <Partitioner2/Function.h>                      // Fast function data structures
#include <Sawyer/ProgressBar.h>

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {
namespace CallingConvention {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::CallingConvention", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dictionaries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Dictionary&
dictionaryArm() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-21]: none defind yet
    return dict;
}

const Dictionary&
dictionaryM68k() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-20]: none defined yet
    return dict;
}

const Dictionary&
dictionaryMips() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-21]: none defind yet
    return dict;
}

const Dictionary&
dictionaryPowerpc() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-21]: none defind yet
    return dict;
}

const Dictionary&
dictionaryX86() {
    static Dictionary dict;
    if (dict.empty()) {
        //--------
        // 32-bit
        //--------

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        Definition cc = Definition::x86_32bit_cdecl();
        cc.comment(cc.comment() + " 4-byte alignment");
        cc.stackAlignment(4);
        dict.push_back(cc);

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        cc = Definition::x86_32bit_cdecl();
        cc.comment(cc.comment() + " 16-byte alignment");
        cc.stackAlignment(16);
        dict.push_back(cc);
#else
        dict.push_back(Definition::x86_32bit_cdecl());
#endif

        // other conventions
        dict.push_back(Definition::x86_32bit_stdcall());
        dict.push_back(Definition::x86_32bit_fastcall());

        //--------
        // 64-bit
        //--------

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        cc = Definition::x86_64bit_cdecl();
        cc.comment(cc.comment() + " 4-byte alignment");
        cc.stackAlignment(4);
        dict.push_back(cc);

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        cc = Definition::x86_64bit_cdecl();
        cc.comment(cc.comment() + " 16-byte alignment");
        cc.stackAlignment(16);
        dict.push_back(cc);
#else
        dict.push_back(Definition::x86_64bit_cdecl());
#endif

        // other conventions
        dict.push_back(Definition::x86_64bit_stdcall());
    }
    return dict;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
const Definition&
Definition::x86_32bit_cdecl() {
    static Definition cc;
    if (cc.name().empty())
        cc = x86_cdecl(RegisterDictionary::dictionary_pentium4());
    return cc;
}

// class method
const Definition&
Definition::x86_64bit_cdecl() {
    static Definition cc;
    if (cc.name().empty())
        cc = x86_cdecl(RegisterDictionary::dictionary_amd64());
    return cc;
}

// class method
Definition
Definition::x86_cdecl(const RegisterDictionary *regDict) {
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
    Definition cc(SP.get_nbits(), "cdecl", 
                  "x86-" + StringUtility::numberToString(SP.get_nbits()) + " cdecl",
                  regDict);

    // Stack characteristics
    cc.stackPointerRegister(SP);
    cc.stackDirection(GROWS_DOWN);
    cc.nonParameterStackSize(cc.wordWidth() >> 3);      // return address

    // All parameters are passed on the stack.
    cc.stackParameterOrder(RIGHT_TO_LEFT);
    cc.stackCleanup(CLEANUP_BY_CALLER);

    // Return values
    cc.appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
    cc.appendOutputParameter(regDict->findLargestRegister(x86_regclass_st, x86_st_0));
    cc.appendOutputParameter(SP);                       // final value is usually one word greater than initial value

    // Scratch registers (i.e., modified, not callee-saved, not return registers)
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc.getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc.calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

// class method
const Definition&
Definition::x86_32bit_stdcall() {
    static Definition cc;
    if (cc.name().empty())
        cc = x86_stdcall(RegisterDictionary::dictionary_pentium4());
    return cc;
}

// class method
const Definition&
Definition::x86_64bit_stdcall() {
    static Definition cc;
    if (cc.name().empty())
        cc = x86_stdcall(RegisterDictionary::dictionary_amd64());
    return cc;
}

// class method
Definition
Definition::x86_stdcall(const RegisterDictionary *regDict) {
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
    Definition cc(SP.get_nbits(), "stdcall", 
                  "x86-" + StringUtility::numberToString(SP.get_nbits()) + " stdcall",
                  regDict);

    // Stack characteristics
    cc.stackPointerRegister(SP);
    cc.stackDirection(GROWS_DOWN);
    cc.nonParameterStackSize(cc.wordWidth() >> 3);      // return address

    // All parameters are passed on the stack
    cc.stackParameterOrder(RIGHT_TO_LEFT);
    cc.stackCleanup(CLEANUP_BY_CALLEE);

    // Return values
    cc.appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
    cc.appendOutputParameter(SP);

    // Scratch registers (i.e., modified, not callee-saved, not return registers)
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc.getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc.calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

// class method
const Definition&
Definition::x86_32bit_fastcall() {
    static Definition cc;
    if (cc.name().empty())
        cc = x86_fastcall(RegisterDictionary::dictionary_pentium4());
    return cc;
}

// class method
Definition
Definition::x86_fastcall(const RegisterDictionary *regDict) {
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
    static Definition cc(SP.get_nbits(), "fastcall",
                         "x86-" + StringUtility::numberToString(cc.wordWidth()) + " fastcall",
                         regDict);

    // Stack characteristics
    cc.stackPointerRegister(SP);
    cc.stackDirection(GROWS_DOWN);
    cc.nonParameterStackSize(cc.wordWidth() >> 3);      // return address

    // Uses ECX and EDX for first args that fit; all other parameters are passed on the stack.
    cc.appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc.appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc.stackParameterOrder(RIGHT_TO_LEFT);
    cc.stackCleanup(CLEANUP_BY_CALLEE);

    // Return values
    cc.appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
    cc.appendOutputParameter(SP);

    // Scratch registers (i.e., modified, not callee-saved, not return registers)
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
    cc.scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc.getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc.calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}
        
void
Definition::appendInputParameter(const ParameterLocation &newLocation) {
#ifndef NDEBUG
    BOOST_FOREACH (const ParameterLocation &existingLocation, inputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    inputParameters_.push_back(newLocation);
}

void
Definition::appendOutputParameter(const ParameterLocation &newLocation) {
#ifndef NDEBUG
    BOOST_FOREACH (const ParameterLocation &existingLocation, outputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    outputParameters_.push_back(newLocation);
}

RegisterParts
Definition::outputRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const ParameterLocation &loc, outputParameters_) {
        if (loc.type() == ParameterLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::inputRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const ParameterLocation &loc, inputParameters_) {
        if (loc.type() == ParameterLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::scratchRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const RegisterDescriptor &reg, scratchRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::calleeSavedRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const RegisterDescriptor &reg, calleeSavedRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::getUsedRegisterParts() const {
    RegisterParts retval = inputRegisterParts();
    retval |= outputRegisterParts();
    if (stackPointerRegister_.is_valid())
        retval.insert(stackPointerRegister_);
    if (thisParameter_.type() == ParameterLocation::REGISTER)
        retval.insert(thisParameter_.reg());
    retval |= calleeSavedRegisterParts();
    retval |= scratchRegisterParts();
    return retval;
}

void
Definition::print(std::ostream &out, const RegisterDictionary *regDict/*=NULL*/) const {
    using namespace StringUtility;
    RegisterNames regNames(regDict ? regDict : regDict_);

    out <<cEscape(name_);
    if (!comment_.empty())
        out <<" (" <<cEscape(comment_) <<")";
    out <<" = {" <<wordWidth_ <<"-bit words";

    if (!inputParameters_.empty()) {
        out <<", inputs={";
        BOOST_FOREACH (const ParameterLocation &loc, inputParameters_) {
            out <<" ";
            loc.print(out, regNames);
        }
        out <<" }";
    }

    if (stackParameterOrder_ != ORDER_UNSPECIFIED) {
        out <<", implied={";
        switch (stackParameterOrder_) {
            case LEFT_TO_RIGHT: out <<" left-to-right"; break;
            case RIGHT_TO_LEFT: out <<" right-to-left"; break;
            case ORDER_UNSPECIFIED: ASSERT_not_reachable("invalid stack parameter order");
        }

        if (stackPointerRegister_.is_valid()) {
            out <<" " <<regNames(stackPointerRegister_) <<"-based stack";
        } else {
            out <<" NO-STACK-REGISTER";
        }

        switch (stackCleanup_) {
            case CLEANUP_BY_CALLER: out <<" cleaned up by caller"; break;
            case CLEANUP_BY_CALLEE: out <<" cleaned up by callee"; break;
            case CLEANUP_UNSPECIFIED: out <<" with UNSPECIFIED cleanup"; break;
        }
        out <<" }";
    }
    
    if (nonParameterStackSize_ > 0)
        out <<", " <<(nonParameterStackSize_ >> 3) <<"-byte return";

    if (stackParameterOrder_ != ORDER_UNSPECIFIED || nonParameterStackSize_ > 0) {
        switch (stackDirection_) {
            case GROWS_UP: out <<", upward-growing stack"; break;
            case GROWS_DOWN: out <<", downward-growing stack"; break;
        }
    }

    if (thisParameter_.isValid()) {
        out <<", this=";
        thisParameter_.print(out, regNames);
    }
    
    if (!outputParameters_.empty()) {
        out <<", outputs={";
        BOOST_FOREACH (const ParameterLocation &loc, outputParameters_) {
            out <<" ";
            loc.print(out, regNames);
        }
        out <<" }";
    }

    if (!scratchRegisters_.empty()) {
        out <<", scratch={";
        BOOST_FOREACH (const RegisterDescriptor &loc, scratchRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }
    
    if (!calleeSavedRegisters_.empty()) {
        out <<", saved={";
        BOOST_FOREACH (const RegisterDescriptor &loc, calleeSavedRegisters_)
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

    
void
Analysis::init(Disassembler *disassembler) {
    if (disassembler) {
        const RegisterDictionary *registerDictionary = disassembler->get_registers();
        ASSERT_not_null(registerDictionary);
        size_t addrWidth = disassembler->instructionPointerRegister().get_nbits();

        SMTSolver *solver = NULL;
        SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(registerDictionary, solver);

        cpu_ = disassembler->dispatcher()->create(ops, addrWidth, registerDictionary);
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
    cpu_ = DispatcherPtr();
}

void
Analysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzeFunction(" <<function->printableName() <<")\n";
    clearResults();

    // Build the CFG used by the dataflow: dfCfg.  The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), partitioner.findPlaceholder(function->address()));
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
            returnVertex = dfCfg.findVertex(vertex.id());
            break;
        }
    }
    if (returnVertex == dfCfg.vertices().end()) {
        mlog[DEBUG] <<"  function CFG has no return vertex\n";
        return;
    }

    // Build the dataflow engine.
    typedef DataFlow::Engine<DfCfg, StatePtr, P2::DataFlow::TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    DispatcherPtr cpu = partitioner.newDispatcher(partitioner.newOperators());
    P2::DataFlow::MergeFunction merge(cpu);
    P2::DataFlow::TransferFunction xfer(cpu);
    xfer.defaultCallingConvention(defaultCc_);
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.maxIterations(dfCfg.nVertices() * 5);      // arbitrary
    regDict_ = cpu->get_register_dictionary();

    // Build the initial state
    StatePtr initialState = xfer.initialState();
    RegisterStateGenericPtr initialRegState = RegisterStateGeneric::promote(initialState->get_register_state());
    initialRegState->initialize_large();

    // Run data flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(mlog[MARCH], function->printableName());
        dfEngine.reset(startVertexId, initialState);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    }

    // Get the final dataflow state
    StatePtr finalState = dfEngine.getInitialState(returnVertex->id());
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
    RegisterStateGenericPtr finalRegs = RegisterStateGeneric::promote(finalState->get_register_state());

    // Update analysis results
    updateRestoredRegisters(initialState, finalState);
    updateInputRegisters(finalState);
    updateOutputRegisters(finalState);
    updateStackParameters(initialState, finalState);
    updateStackDelta(initialState, finalState);
    hasResults_ = true;
    didConverge_ = converged;
}

void
Analysis::updateRestoredRegisters(const StatePtr &initialState, const StatePtr &finalState) {
    restoredRegisters_.clear();

    RegisterStateGenericPtr initialRegs = RegisterStateGeneric::promote(initialState->get_register_state());
    RegisterStateGenericPtr finalRegs = RegisterStateGeneric::promote(finalState->get_register_state());
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperatorsPtr ops = cpu_->get_operators();

    InputOutputPropertySet props;
    props.insert(IO_READ_BEFORE_WRITE);
    props.insert(IO_WRITE);
    BOOST_FOREACH (const RegisterDescriptor &reg, finalRegs->findProperties(props)) {
        SValuePtr initialValue = initialRegs->readRegister(reg, ops.get());
        SValuePtr finalValue = finalRegs->readRegister(reg, ops.get());
        SymbolicExpr::Ptr initialExpr = SymbolicSemantics::SValue::promote(initialValue)->get_expression();
        SymbolicExpr::Ptr finalExpr = SymbolicSemantics::SValue::promote(finalValue)->get_expression();
        if (finalExpr->flags() == initialExpr->flags() && finalExpr->mustEqual(initialExpr, ops->get_solver()))
            restoredRegisters_.insert(reg);
    }
}

void
Analysis::updateInputRegisters(const StatePtr &state) {
    inputRegisters_.clear();
    RegisterStateGenericPtr regs = RegisterStateGeneric::promote(state->get_register_state());
    BOOST_FOREACH (const RegisterDescriptor &reg, regs->findProperties(IO_READ_BEFORE_WRITE))
        inputRegisters_.insert(reg);
    inputRegisters_ -= restoredRegisters_;
}

void
Analysis::updateOutputRegisters(const StatePtr &state) {
    outputRegisters_.clear();
    RegisterStateGenericPtr regs = RegisterStateGeneric::promote(state->get_register_state());
    BOOST_FOREACH (const RegisterDescriptor &reg, regs->findProperties(IO_WRITE))
        outputRegisters_.insert(reg);
    outputRegisters_ -= restoredRegisters_;
}

void
Analysis::updateStackParameters(const StatePtr &initialState, const StatePtr &finalState) {
    inputStackParameters_.clear();
    outputStackParameters_.clear();

    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperatorsPtr ops = cpu_->get_operators();
    MemoryCellListPtr memState = MemoryCellList::promote(finalState->get_memory_state());
    SValuePtr initialStackPointer = initialState->readRegister(cpu_->stackPointerRegister(), ops.get());
    ops->set_state(finalState);
    StackVariables vars = P2::DataFlow::findFunctionArguments(ops, initialStackPointer);
    BOOST_FOREACH (const StackVariable &var, vars) {
        if (var.meta.ioProperties.exists(IO_READ_BEFORE_WRITE)) {
            inputStackParameters_.push_back(var);
        } else if (var.meta.ioProperties.exists(IO_WRITE) && var.meta.ioProperties.exists(IO_READ_AFTER_WRITE)) {
            outputStackParameters_.push_back(var);
        }
    }
}

void
Analysis::updateStackDelta(const StatePtr &initialState, const StatePtr &finalState) {
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperatorsPtr ops = cpu_->get_operators();
    SValuePtr initialStackPointer = initialState->readRegister(cpu_->stackPointerRegister(), ops.get());
    SValuePtr finalStackPointer = finalState->readRegister(cpu_->stackPointerRegister(), ops.get());
    SValuePtr stackDelta = ops->subtract(finalStackPointer, initialStackPointer);
    if (stackDelta->is_number() && stackDelta->get_width()<=64) {
        stackDelta_ = IntegerOps::signExtend2(stackDelta->get_number(), stackDelta->get_width(), 64);
    } else {
        stackDelta_ = Sawyer::Nothing();
    }
}

void
Analysis::print(std::ostream &out) const {
    RegisterNames regName(regDict_);
    std::string separator;

    if (!inputRegisters_.isEmpty() || !inputStackParameters_.empty()) {
        out <<separator <<"inputs={";
        if (!inputRegisters_.isEmpty()) {
            BOOST_FOREACH (const RegisterDescriptor &reg, inputRegisters_.listAll(regDict_))
                out <<" " <<regName(reg);
        }
        if (!inputStackParameters_.empty()) {
            BOOST_FOREACH (const StackVariable &var, inputStackParameters())
                out <<" stack[" <<var.location.offset <<"]+" <<var.location.nBytes;
        }
        out <<" }";
        separator = ", ";
    }

    if (!outputRegisters_.isEmpty() || !outputStackParameters_.empty()) {
        out <<separator <<"outputs={";
        if (!outputRegisters_.isEmpty()) {
            BOOST_FOREACH (const RegisterDescriptor &reg, outputRegisters_.listAll(regDict_))
                out <<" " <<regName(reg);
        }
        if (!outputStackParameters_.empty()) {
            BOOST_FOREACH (const StackVariable &var, outputStackParameters())
                out <<" stack[" <<var.location.offset <<"]+" <<var.location.nBytes;
        }
        out <<" }";
        separator = ", ";
    }

    if (!restoredRegisters_.isEmpty()) {
        out <<separator <<"saved={";
        BOOST_FOREACH (const RegisterDescriptor &reg, restoredRegisters_.listAll(regDict_))
            out <<" " <<regName(reg);
        out <<" }";
        separator = ", ";
    }

    if (stackDelta_) {
        out <<separator <<"stackDelta=" <<(*stackDelta_>=0?"+":"") <<*stackDelta_;
        separator = ", ";
    }
    
    if (separator.empty())
        out <<"no I/O";
}

bool
Analysis::match(const Definition &cc) const {
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    if (!hasResults_)
        return false;
    if (cc.wordWidth() != cpu_->stackPointerRegister().get_nbits())
        return false;

    // Gather up definition's input registers. We always add EIP (or similar) because the analysis will have read it to obtain
    // the function's first instruction before ever writing to it.  Similarly, we add ESP (or similar) because pushing,
    // popping, aligning, and allocating local variable space all read ESP before writing to it.
    RegisterParts ccInputRegisters = cc.inputRegisterParts();
    ccInputRegisters.insert(cpu_->instructionPointerRegister());
    ccInputRegisters.insert(cpu_->stackPointerRegister());
    if (cc.thisParameter().type() == ParameterLocation::REGISTER)
        ccInputRegisters.insert(cc.thisParameter().reg());

    // Gather up definition's output registers.  We always add EIP (or similar) because the final RET instruction will write
    // the return address into the EIP register and not subsequently read it. The stack pointer register is not added by
    // default because not all functions use the stack (e.g., architectures that have link registers); it must be added (or
    // not) when the definition is created.
    RegisterParts ccOutputRegisters = cc.outputRegisterParts() | cc.scratchRegisterParts();
    ccOutputRegisters.insert(cpu_->instructionPointerRegister());

    // Stack delta checks
    if (stackDelta_) {
        int64_t normalization = (cc.stackDirection() == GROWS_UP ? -1 : +1);
        int64_t normalizedStackDelta = *stackDelta_ * normalization; // in bytes

        // All callees must pop the non-parameter area (e.g., return address) of the stack.
        if (normalizedStackDelta < 0 || (uint64_t)normalizedStackDelta < cc.nonParameterStackSize())
            return false;
        normalizedStackDelta -= cc.nonParameterStackSize();

        // The callee must not pop stack parameters if the caller cleans them up.
        if (cc.stackCleanup() == CLEANUP_BY_CALLER && normalizedStackDelta != 0)
            return false;

        // For callee cleanup, the callee must pop all the stack variables. It may pop more than what it used (i.e., it must
        // pop even unused arguments).
        if (cc.stackCleanup() == CLEANUP_BY_CALLEE) {
            int64_t normalizedEnd = 0; // one-past first-pushed argument normlized for downward-growing stack
            BOOST_FOREACH (const StackVariable &var, inputStackParameters_)
                normalizedEnd = std::max(normalizedEnd, (int64_t)(var.location.offset * normalization + var.location.nBytes));
            BOOST_FOREACH (const StackVariable &var, outputStackParameters_)
                normalizedEnd = std::max(normalizedEnd, (int64_t)(var.location.offset * normalization + var.location.nBytes));
            if (normalizedStackDelta < normalizedEnd)
                return false;
        }
    }
    
    // All analysis output registers must be a definition's output or scratch register.
    if (!(outputRegisters_ - ccOutputRegisters).isEmpty())
        return false;

    // All analysis input registers must be a definition's input or "this" register.
    if (!(inputRegisters_ - ccInputRegisters).isEmpty())
        return false;

    // All analysis restored registers must be a definition's callee-saved register.
    if (!(restoredRegisters_ - cc.calleeSavedRegisterParts()).isEmpty())
        return false;

    // If we modified registers we were not allowed to have modified then we're not this calling convention.
    if (!(outputRegisters_ & ccOutputRegisters).isEmpty())
        return false;

    // If the definition has an object pointer ("this" parameter) then it should not be an anlysis output or scratch register,
    // but must be an analysis input register.
    if (cc.thisParameter().type() == ParameterLocation::REGISTER) {
        if (ccOutputRegisters.existsAny(cc.thisParameter().reg()))
            return false;
        if (!ccInputRegisters.existsAll(cc.thisParameter().reg()))
            return false;
    }

    // If the analysis has stack inputs or outputs then the definition must have a valid stack parameter direction.
    if ((!inputStackParameters().empty() || !outputStackParameters().empty()) && cc.stackParameterOrder() == ORDER_UNSPECIFIED)
        return false;

    return true;
}

Dictionary
Analysis::match(const Dictionary &conventions) const {
    Dictionary retval;
    BOOST_FOREACH (const Definition &cc, conventions) {
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

} // namespace
} // namespace
} // namespace
