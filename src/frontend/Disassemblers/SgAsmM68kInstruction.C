/* SgAsmM68kInstruction member definitions. Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any other *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "Disassembler.h"

// see base class
bool
SgAsmM68kInstruction::terminates_basic_block()
{
    assert(!"not implemented yet"); // FIXME
    abort();
}

// see base class
bool
SgAsmM68kInstruction::is_function_call(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target)
{
    assert(!"not implemented yet"); // FIXME
    abort();
}

bool
SgAsmM68kInstruction::is_function_return(const std::vector<SgAsmInstruction*>& insns)
{
    assert(!"not implemented yet"); // FIXME
    abort();
}

bool
SgAsmM68kInstruction::is_unknown() const
{
    return m68k_unknown_instruction == get_kind();
}

Disassembler::AddressSet
SgAsmM68kInstruction::get_successors(bool *complete)
{
    assert(!"not implemented yet"); // FIXME
    abort();
}

bool
SgAsmM68kInstruction::get_branch_target(rose_addr_t *target)
{
    assert(!"not implemented yet"); // FIXME
    abort();
}
