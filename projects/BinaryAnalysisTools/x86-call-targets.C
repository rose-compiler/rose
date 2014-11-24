// Disassembles all bytes of a file looking for x86 CALL instructions that have absolute addresses and emits thos addresses on
// stdout as hexadecimal numbers.

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
    size_t ninsns=0, nerrors=0;
    Disassembler *disassembler = new DisassemblerX86(4);
    for (rose_addr_t offset=0; offset<file_size; ++offset) {
        try {
            rose_addr_t insn_va = start_va + offset;
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(disassembler->disassembleOne(&map, insn_va));
            if (insn && (x86_call==insn->get_kind() || x86_farcall==insn->get_kind())) {
                ++ninsns;
                rose_addr_t target_va;
                if (insn->getBranchTarget(&target_va))
                    std::cout <<StringUtility::addrToString(insn_va) <<": " <<StringUtility::addrToString(target_va) <<"\n";
            }
        } catch (const Disassembler::Exception &e) {
            ++nerrors;
        }
    }

    std::cerr <<specimen_name <<": " <<ninsns <<" instructions; " <<nerrors <<" errors\n";
    return 0;
}
