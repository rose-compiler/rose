#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Semantics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics {

namespace BaseSemantics = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace SymbolicSemantics = Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Risc Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators() {}

RiscOperators::RiscOperators(const InstructionSemantics::BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : InstructionSemantics::SymbolicSemantics::RiscOperators(protoval, solver) {
    name("PartitionerSemantics");
    (void)SValue::promote(protoval);                // make sure its dynamic type is appropriate
    trimThreshold(TRIM_THRESHOLD_DFLT);
}

RiscOperators::RiscOperators(const InstructionSemantics::BaseSemantics::State::Ptr &state, const SmtSolverPtr &solver)
    : InstructionSemantics::SymbolicSemantics::RiscOperators(state, solver) {
    name("PartitionerSemantics");
    (void)SValue::promote(state->protoval());
    trimThreshold(TRIM_THRESHOLD_DFLT);
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instance(const RegisterDictionary::Ptr &regdict) {
    return instance(regdict, SmtSolver::Ptr(), LIST_BASED_MEMORY);
}

RiscOperators::Ptr
RiscOperators::instance(const RegisterDictionary::Ptr &regdict, const SmtSolver::Ptr &solver,
                        SemanticMemoryParadigm memoryParadigm) {
    InstructionSemantics::BaseSemantics::SValue::Ptr protoval = SValue::instance();
    InstructionSemantics::BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    InstructionSemantics::BaseSemantics::MemoryState::Ptr memory;
    switch (memoryParadigm) {
        case LIST_BASED_MEMORY:
            memory = MemoryListState::instance(protoval, protoval);
            break;
        case MAP_BASED_MEMORY:
            memory = MemoryMapState::instance(protoval, protoval);
            break;
    }
    InstructionSemantics::BaseSemantics::State::Ptr state = State::instance(registers, memory);
    return Ptr(new RiscOperators(state, solver));
}

RiscOperators::Ptr
RiscOperators::instance(const InstructionSemantics::BaseSemantics::SValue::Ptr &protoval) {
    return Ptr(new RiscOperators(protoval, SmtSolver::Ptr()));
}

RiscOperators::Ptr
RiscOperators::instance(const InstructionSemantics::BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(protoval, solver));
}

RiscOperators::Ptr
RiscOperators::instance(const InstructionSemantics::BaseSemantics::StatePtr &state) {
    return Ptr(new RiscOperators(state, SmtSolver::Ptr()));
}

RiscOperators::Ptr
RiscOperators::instance(const InstructionSemantics::BaseSemantics::StatePtr &state, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(state, solver));
}

RiscOperators::Ptr
RiscOperators::promote(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(currentState());
    ASSERT_not_null(currentState()->memoryState());

    BaseSemantics::MemoryState::Ptr mem = currentState()->memoryState();
    if (MemoryListState::Ptr ml = boost::dynamic_pointer_cast<MemoryListState>(mem)) {
        ml->addressesRead().clear();
    } else if (MemoryMapState::Ptr mm = boost::dynamic_pointer_cast<MemoryMapState>(mem)) {
        mm->addressesRead().clear();
    }
    SymbolicSemantics::RiscOperators::startInstruction(insn);
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryListState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryMapState);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Partitioner2::Semantics::RiscOperators);
#endif

#endif
