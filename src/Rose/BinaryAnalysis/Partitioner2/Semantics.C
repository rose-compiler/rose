#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
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
