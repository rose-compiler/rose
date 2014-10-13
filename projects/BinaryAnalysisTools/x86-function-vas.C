// Disassembles all bytes from a buffer and tries to find function entry addresses

#include "rose.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace rose::BinaryAnalysis;

int
main(int argc, char *argv[])
{
    // Parse command-line
    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else {
            std::cerr <<argv[0] <<": unrecognized switch: " <<argv[argno] <<"\n";
            exit(1);
        }
    }
    if (argno+1!=argc) {
        std::cerr <<"usage: " <<argv[0] <<" [SWITCHES] [--] SPECIMEN\n";
        exit(1);
    }
    std::string specimen_name = argv[argno++];

    // Open the file
    rose_addr_t start_va = 0;
    MemoryMap map;
    size_t file_size = map.insertFile(specimen_name, start_va);
    map.at(start_va).limit(file_size).changeAccess(MemoryMap::EXECUTABLE, 0);

    // Try to disassemble every byte, and print the CALL/FARCALL targets
    InstructionMap insns;
    size_t nerrors=0;
    Disassembler *disassembler = new DisassemblerX86(4);
    for (rose_addr_t offset=0; offset<file_size; ++offset) {
        try {
            rose_addr_t insn_va = start_va + offset;
            if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(disassembler->disassembleOne(&map, insn_va)))
                insns[insn_va] = insn;
        } catch (const Disassembler::Exception &e) {
            ++nerrors;
        }
    }

    // Partition those instructions into basic blocks and functions
    Partitioner partitioner;
    SgAsmBlock *gblock = partitioner.partition(NULL, insns, &map);

    // Print addresses of functions
    struct T1: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgAsmFunction *func = isSgAsmFunction(node))
                std::cout <<StringUtility::addrToString(func->get_entry_va()) <<"\n";
        }
    };
    T1().traverse(gblock, preorder);

    std::cerr <<specimen_name <<": " <<insns.size() <<" instructions; " <<nerrors <<" errors\n";
    return 0;
}
