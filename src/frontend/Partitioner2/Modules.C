#include "sage3basic.h"
#include <Partitioner2/Modules.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Modules {

size_t
findSymbolFunctions(const Partitioner &partitioner, SgAsmGenericHeader *fileHeader, std::vector<Function::Ptr> &functions) {
    struct T1: AstSimpleProcessing {
        const Partitioner &partitioner;
        SgAsmGenericHeader *fileHeader;
        std::vector<Function::Ptr> &functions;
        size_t nInserted;
        T1(const Partitioner &p, SgAsmGenericHeader *fh, std::vector<Function::Ptr> &functions)
            : partitioner(p), fileHeader(fh), functions(functions), nInserted(0) {}
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
                        if (insertUnique(functions, function, sortFunctionsByAddress))
                            ++nInserted;
                    }

                    // Sometimes weak symbol values are offsets from a section (this code handles that), but other times
                    // they're the value is used directly (the above code handled that case). */
                    if (section && symbol->get_binding() == SgAsmGenericSymbol::SYM_WEAK)
                        value += section->get_mapped_actual_va();
                    if (partitioner.discoverInstruction(value)) {
                        Function::Ptr function = Function::instance(value, symbol->get_name()->get_string(),
                                                                    SgAsmFunction::FUNC_SYMBOL);
                        if (insertUnique(functions, function, sortFunctionsByAddress))
                            ++nInserted;
                    }
                }
            }
        }
    } t1(partitioner, fileHeader, functions);
    t1.traverse(fileHeader, preorder);
    return t1.nInserted;
}

std::vector<Function::Ptr>
findSymbolFunctions(const Partitioner &partitioner, SgAsmGenericHeader *fileHeader) {
    std::vector<Function::Ptr> functions;
    findSymbolFunctions(partitioner, fileHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findSymbolFunctions(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            findSymbolFunctions(partitioner, fileHeader, functions);
    }
    return functions;
}


} // namespace
} // namespace
} // namespace
} // namespace
