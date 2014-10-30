#include "sage3basic.h"
#include "Diagnostics.h"

#include <Partitioner2/Modules.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Modules {

bool
Modules::AddGhostSuccessors::operator()(bool chain, const Args &args) {
    if (chain) {
        size_t nBits = args.partitioner->instructionProvider().instructionPointerRegister().get_nbits();
        BOOST_FOREACH (rose_addr_t successorVa, args.partitioner->basicBlockGhostSuccessors(args.bblock))
            args.bblock->insertSuccessor(successorVa, nBits);
    }
    return chain;
}

bool
PreventDiscontiguousBlocks::operator()(bool chain, const Args &args) {
    if (chain) {
        bool complete;
        std::vector<rose_addr_t> successors = args.partitioner->basicBlockConcreteSuccessors(args.bblock, &complete);
        if (complete && 1==successors.size() && successors[0]!=args.bblock->fallthroughVa())
            args.results.terminate = TERMINATE_NOW;
    }
    return chain;
}

AddressIntervalSet
deExecuteZeros(MemoryMap &map /*in,out*/, size_t threshold) {
    AddressIntervalSet changes;
    if (0==threshold)
        return changes;
    rose_addr_t va = map.hull().least();
    AddressInterval zeros;
    uint8_t buf[4096];
    while (AddressInterval accessed = map.atOrAfter(va).limit(sizeof buf).require(MemoryMap::EXECUTABLE).read(buf)) {
        size_t nRead = accessed.size();
        size_t firstZero = 0;
        while (firstZero < nRead) {
            while (firstZero<nRead && buf[firstZero]!=0) ++firstZero;
            if (firstZero < nRead) {
                size_t nZeros = 1;
                while (firstZero+nZeros < nRead && buf[firstZero+nZeros]==0) ++nZeros;

                if (zeros.isEmpty()) {
                    zeros = AddressInterval::baseSize(va+firstZero, nZeros);
                } else if (zeros.greatest()+1 == va+firstZero) {
                    zeros = AddressInterval::baseSize(zeros.least(), zeros.size()+nZeros);
                } else {
                    if (zeros.size() >= threshold) {
                        map.within(zeros).changeAccess(0, MemoryMap::EXECUTABLE);
                        changes.insert(zeros);
                    }
                    zeros = AddressInterval::baseSize(va+firstZero, nZeros);
                }

                firstZero += nZeros+1;
            }
        }
        va += nRead;
    }
    if (zeros.size()>=threshold) {
        map.within(zeros).changeAccess(0, MemoryMap::EXECUTABLE);
        changes.insert(zeros);
    }
    return changes;
}

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
