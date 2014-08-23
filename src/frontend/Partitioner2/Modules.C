#include "sage3basic.h"
#include <Partitioner2/Modules.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Modules {

std::vector<Function::Ptr>
findSymbolFunctions(const Partitioner &partitioner, SgAsmGenericHeader *fileHeader) {
    struct T1: AstSimpleProcessing {
        const Partitioner &partitioner;
        SgAsmGenericHeader *fileHeader;
        std::vector<Function::Ptr> functions;
        T1(const Partitioner &p, SgAsmGenericHeader *fh): partitioner(p), fileHeader(fh) {}
        void visit(SgNode *node) {
            if (SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node)) {
                if (symbol->get_def_state() == SgAsmGenericSymbol::SYM_DEFINED &&
                    symbol->get_type()      == SgAsmGenericSymbol::SYM_FUNC &&
                    symbol->get_value()     != 0) {
                    rose_addr_t value = fileHeader->get_base_va() + symbol->get_value();
                    SgAsmGenericSection *section = symbol->get_bound();

                    // Add a function at the symbol's value. If the symbol is bound to a section and the section is mapped at a
                    // different address than it expected to be mapped, then adjust the symbol's value by the same amount.
                    rose_addr_t va = value;
                    if (section!=NULL && section->is_mapped() &&
                        section->get_mapped_preferred_va() != section->get_mapped_actual_va()) {
                        va += section->get_mapped_actual_va() - section->get_mapped_preferred_va();
                    }
                    if (partitioner.discoverInstruction(va)) {
                        Function::Ptr function = Function::instance(va, symbol->get_name()->get_string(),
                                                                    SgAsmFunction::FUNC_SYMBOL);
                        insertUnique(functions, function, sortFunctionsByAddress);
                    }

                    // Sometimes weak symbol values are offsets from a section (this code handles that), but other times
                    // they're the value is used directly (the above code handled that case). */
                    if (section && symbol->get_binding() == SgAsmGenericSymbol::SYM_WEAK)
                        value += section->get_mapped_actual_va();
                    if (partitioner.discoverInstruction(value)) {
                        Function::Ptr function = Function::instance(value, symbol->get_name()->get_string(),
                                                                    SgAsmFunction::FUNC_SYMBOL);
                        insertUnique(functions, function, sortFunctionsByAddress);
                    }
                }
            }
        }
    } t1(partitioner, fileHeader);
    t1.traverse(fileHeader, preorder);
    return t1.functions;
}

} // namespace
} // namespace
} // namespace
} // namespace
