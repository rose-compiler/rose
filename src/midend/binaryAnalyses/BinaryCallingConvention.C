#include <sage3basic.h>
#include <BinaryCallingConvention.h>

#include <BinaryDataFlow.h>                             // Dataflow engine
#include <boost/foreach.hpp>
#include <Partitioner2/DataFlow.h>                      // Dataflow components that we can re-use
#include <Partitioner2/Partitioner.h>                   // Fast binary analysis data structures
#include <Partitioner2/Function.h>                      // Fast function data structures

namespace rose {
namespace BinaryAnalysis {

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      CallingConvention
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Message::Facility CallingConvention::mlog;

void
CallingConvention::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::CallingConvention", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
CallingConvention::appendInputParameter(const ParameterLocation &newLocation) {
#ifndef NDEBUG
    BOOST_FOREACH (const ParameterLocation &existingLocation, inputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    inputParameters_.push_back(newLocation);
}

void
CallingConvention::appendOutputParameter(const ParameterLocation &newLocation) {
#ifndef NDEBUG
    BOOST_FOREACH (const ParameterLocation &existingLocation, outputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    outputParameters_.push_back(newLocation);
}

void
CallingConvention::print(std::ostream &out, const RegisterDictionary *regdict) const {
    using namespace StringUtility;
    ASSERT_not_null(regdict);
    RegisterNames regNames(regdict);

    out <<cEscape(name_);
    if (!comment_.empty())
        out <<" (" <<cEscape(comment_) <<")";
    out <<" = {" <<wordSize_ <<"-bit words";

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
            case CLEANUP_CALLER: out <<" cleaned up by caller"; break;
            case CLEANUP_CALLEE: out <<" cleaned up by callee"; break;
            case CLEANUP_UNSPECIFIED: out <<" with UNSPECIFIED cleanup"; break;
        }
        out <<"}";
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
    
    if (!calleeSavedRegisters_.empty()) {
        out <<", outputs={";
        BOOST_FOREACH (const ParameterLocation &loc, outputParameters_) {
            out <<" ";
            loc.print(out, regNames);
        }
        out <<"}";
    }

    if (!calleeSavedRegisters_.empty()) {
        out <<", saved={";
        BOOST_FOREACH (const RegisterDescriptor &loc, calleeSavedRegisters_)
            out <<" " <<regNames(loc);
        out <<"}";
    }
}

    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      CallingConventionAnalysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
void
CallingConventionAnalysis::init(Disassembler *disassembler) {
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
CallingConventionAnalysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzeFunction(" <<function->printableName() <<")\n";

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
    typedef DataFlow::Engine<DfCfg, BaseSemantics::StatePtr,
                             P2::DataFlow::TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(partitioner.newOperators());
    P2::DataFlow::MergeFunction merge(cpu);
    P2::DataFlow::TransferFunction xfer(cpu, partitioner.instructionProvider().stackPointerRegister());
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.maxIterations(dfCfg.nVertices() * 5);      // arbitrary

    // Build the initial state
    BaseSemantics::StatePtr initialState = xfer.initialState();
    BaseSemantics::RegisterStateGenericPtr initialRegState =
        BaseSemantics::RegisterStateGeneric::promote(initialState->get_register_state());
    initialRegState->initialize_large();

    // Run data flow analysis
    bool converged = true;
    try {
#if 1 // DEBUGGING [Robb P. Matzke 2015-08-13]
        std::cerr <<"ROBB: Data flow for " <<function->printableName() <<"\n";
        dfEngine.reset(startVertexId, initialState);
        for (size_t i=0; i<dfEngine.maxIterations(); ++i) {
            std::cerr <<"        iteration " <<i <<"\n";
            dfEngine.runOneIteration();
        }
#else
        dfEngine.runToFixedPoint(startVertexId, initialState);
#endif
    } catch (const std::runtime_error &e) {
        converged = false;                              // didn't converge, so just use what we have
    }

    // Get the final dataflow state
    BaseSemantics::StatePtr finalState = dfEngine.getInitialState(returnVertex->id());
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

#if 1 // DEBUGGING [Robb P. Matzke 2015-08-12]
    std::cerr <<"ROBB: calling convention results for " <<function->printableName() <<":\n";
    RegisterNames registerName(cpu->get_register_dictionary());

    RegisterSet readWithoutWrite = registersReadWithoutWrite(finalState);
    std::cerr <<"      registers read without being written first:\n";
    BOOST_FOREACH (const RegisterDescriptor &reg, readWithoutWrite.values())
        std::cerr <<"        " <<registerName(reg) <<"\n";

    RegisterSet calleeSaved = registersCalleeSaved(cpu, initialState, finalState, readWithoutWrite);
    std::cerr <<"      callee-saved registers (from the list above):\n";
    BOOST_FOREACH (const RegisterDescriptor &reg, calleeSaved.values())
        std::cerr <<"        " <<registerName(reg) <<"\n";

    RegisterSet returnValues = registersWriteWithoutRead(finalState);
    returnValues.erase(calleeSaved);
    std::cerr <<"      possible return values (registers written and then not read):\n";
    BOOST_FOREACH (const RegisterDescriptor &reg, returnValues.values())
        std::cerr <<"        " <<registerName(reg) <<"\n";
#endif
}

// List of registers read
CallingConventionAnalysis::RegisterSet
CallingConventionAnalysis::registersRead(const BaseSemantics::StatePtr &state) {
    RegisterSet retval;
    BaseSemantics::RegisterStateGenericPtr regState = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    BOOST_FOREACH (const RegisterDescriptor &reg, regState->findProperties(BaseSemantics::RegisterStateGeneric::READ))
        retval.insert(reg);
    return retval;
}

// List of registers written
CallingConventionAnalysis::RegisterSet
CallingConventionAnalysis::registersWritten(const BaseSemantics::StatePtr &state) {
    RegisterSet retval;
    BaseSemantics::RegisterStateGenericPtr regState = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    BOOST_FOREACH (const RegisterDescriptor &reg, regState->findProperties(BaseSemantics::RegisterStateGeneric::WRITTEN))
        retval.insert(reg);
    return retval;
}

// List of registers read and written
CallingConventionAnalysis::RegisterSet
CallingConventionAnalysis::registersReadWritten(const BaseSemantics::StatePtr &state) {
    RegisterSet retval;
    BaseSemantics::RegisterStateGenericPtr regState = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    BaseSemantics::RegisterStateGeneric::PropertySet props;
    props.insert(BaseSemantics::RegisterStateGeneric::READ);
    props.insert(BaseSemantics::RegisterStateGeneric::WRITTEN);
    BOOST_FOREACH (const RegisterDescriptor &reg, regState->findProperties(props))
        retval.insert(reg);
    return retval;
}

// List of registers that were read before they were written. The return value is largest, non-overlapping registers.
CallingConventionAnalysis::RegisterSet
CallingConventionAnalysis::registersReadWithoutWrite(const BaseSemantics::StatePtr &state) {
    RegisterSet retval;
    BaseSemantics::RegisterStateGenericPtr regState = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    BOOST_FOREACH (const RegisterDescriptor &reg,
                   regState->findProperties(BaseSemantics::RegisterStateGeneric::READ_BEFORE_WRITE))
        retval.insert(reg);
    return retval;
}

// List of registers written and not subsequently read
CallingConventionAnalysis::RegisterSet
CallingConventionAnalysis::registersWriteWithoutRead(const BaseSemantics::StatePtr &state) {
    RegisterSet retval;
    BaseSemantics::RegisterStateGenericPtr regState = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    BaseSemantics::RegisterStateGeneric::PropertySet required;
    required.insert(BaseSemantics::RegisterStateGeneric::READ);
    required.insert(BaseSemantics::RegisterStateGeneric::WRITTEN);
    BOOST_FOREACH (const RegisterDescriptor &reg,
                   regState->findProperties(required, BaseSemantics::RegisterStateGeneric::READ_AFTER_WRITE))
        retval.insert(reg);
    return retval;
}

// List of callee-saved registers.
CallingConventionAnalysis::RegisterSet
CallingConventionAnalysis::registersCalleeSaved(const BaseSemantics::DispatcherPtr &cpu,
                                                const BaseSemantics::StatePtr &initState,
                                                const BaseSemantics::StatePtr &finalState,
                                                const RegisterSet &readBeforeWrite) {
    RegisterSet retval;
    BaseSemantics::RiscOperatorsPtr ops = cpu->get_operators();
    BOOST_FOREACH (const RegisterDescriptor &reg, readBeforeWrite.values()) {
        BaseSemantics::SValuePtr initValue = initState->readRegister(reg, ops.get());
        BaseSemantics::SValuePtr finalValue = finalState->readRegister(reg, ops.get());
        InsnSemanticsExpr::TreeNodePtr initExpr = SymbolicSemantics::SValue::promote(initValue)->get_expression();
        InsnSemanticsExpr::TreeNodePtr finalExpr = SymbolicSemantics::SValue::promote(finalValue)->get_expression();
        if (finalExpr->get_flags() == initExpr->get_flags() && finalExpr->must_equal(initExpr, ops->get_solver()))
            retval.insert(reg);
    }
    return retval;
}

} // namespace
} // namespace
