/* This test demonstrates:
 *     1. How to disassemble a buffer containing the bare instructions of a single function
 *     2. How to convert an InstructionMap to an AST when you don't have an SgAsmInterpretation
 *     3. How to display the resulting instructions in human-readable form.
 * 
 * Usage: $0 FILE VADDR
 *
 * Where FILE contains the machine instructions of the function and VADDR is the virtual memory address to which those
 * instructions would be mapped by a loader. */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>


/* Read our example file into a memory buffer.  The file contains only the instructions and no PE file header or other
 * container information. */
static const unsigned char*
read_file(const char* filename, size_t *file_sz)
{
    int fd = open(filename, O_RDONLY);
    if (fd<0) {
        perror(filename);
        exit(1);
    }

    struct stat sb;
    if (fstat(fd, &sb)<0) {
        perror("fstat");
        exit(1);
    }

    const unsigned char* buf = (const unsigned char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!buf) {
        perror("mmap");
        exit(1);
    }

    close(fd);
    *file_sz = sb.st_size;
    return buf;
}

/* Define our own partitioner that doesn't do any real function detection analysis. It just takes all the instructions and
 * stuffs them into a single function. */
class MyPartitioner: public Partitioner {
public:
    SgAsmBlock* partition(const Disassembler::InstructionMap& insns, rose_addr_t entry_va, const std::string& name) {
        clear();
        set_search(SgAsmFunctionDeclaration::FUNC_DEFAULT & ~SgAsmFunctionDeclaration::FUNC_LEFTOVERS);
        add_instructions(insns);
        add_function(entry_va, SgAsmFunctionDeclaration::FUNC_ENTRY_POINT, name);
        analyze_cfg();
        return build_ast();
    }
};


int
main(int argc, char* argv[])
{
    if (argc<3) {
        fprintf(stderr, "usage: %s FILENAME START_ADDR [ENTRY_ADDR]\n", argv[0]);
        exit(1);
    }
    const char* filename = argv[1];
    uint64_t start_va = strtoll(argv[2], NULL, 0);
    uint64_t entry_va = argc>3 ? strtol(argv[3], NULL, 0) : start_va;

    /* Read the supplied file into our memory, and then tell ROSE how that memory would have been mapped into
     * the process' memory by the loader. In this case, we just map the entire buffer to a contiguous region
     * of the process starting at the address specified on the command line. */
    size_t buf_sz;
    const unsigned char *buf = read_file(filename, &buf_sz);
    MemoryMap::MapElement melmt(start_va, buf_sz, buf, 0, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC);
    melmt.set_name(filename);
    MemoryMap mm;
    mm.insert(melmt);

    /* Create the SgAsmGenericFile object that will be needed later. We could have used its parse() method to read the
     * contents of our file, but this example's purpose is to clearly demonstrate how to disassemble a buffer that might not
     * have even come from a file. */
    SgAsmGenericFile *file = new SgAsmGenericFile();

    /* In order to disassemble, we need a Disassembler object.  The most convenient way to get one is to ask the Disassembler
     * class to look up a suitable disassembler from those which have been registered.  We then clone that disassembler so
     * that any changes we make (like changing its search heuristics) are local.  We need to provide information about
     * what kind of instructions will be disassembled, which is done through the instruction set architecutre (InsSetArchecture)
     * data member of an SgAsmGenericHeader object. Fortunately, the SgAsmPEFileHeader subclass' constructor fills in
     * reasonable values for the instruction set (i.e., 32-bit x86). */
    SgAsmPEFileHeader *pe = new SgAsmPEFileHeader(file);
    Disassembler *d = Disassembler::lookup(pe)->clone();
    d->set_search(Disassembler::SEARCH_FOLLOWING | Disassembler::SEARCH_DEADEND);
    
    /* Disassemble the mapped buffer. The last two arguments are optional, but we show how to use them here. */
    Disassembler::BadMap errors;
    Disassembler::AddressSet successors;
    Disassembler::InstructionMap insns = d->disassembleBuffer(&mm, entry_va, &successors, &errors);
    printf("Disassembled %zu instruction%s\n", insns.size(), 1==insns.size()?"":"s");

    /* Report about any disassembly errors. */
    for (Disassembler::BadMap::iterator ei=errors.begin(); ei!=errors.end(); ++ei)
        printf("Error at 0x%08"PRIx64": %s\n", ei->second.ip, ei->second.mesg.c_str());

    /* Report which additional addresses the disassembler would like to have seen. */
    fputs("Successors:", stdout);
    if (!successors.empty()) {
        for (Disassembler::AddressSet::iterator si=successors.begin(); si!=successors.end(); ++si)
            printf(" 0x%08"PRIx64, *si);
        fputs("\n\n", stdout);
    } else {
        fputs(" buffer is self contained.\n\n", stdout);
    }

    /* A partitioner can reorganize the instructions into an AST if you desire.  This is necessary if you plan to use any
     * ROSE's analysis or output functions since they operate exclusively on the tree representation. */
    SgAsmBlock *block = MyPartitioner().partition(insns, entry_va, "entry_function");

    /* Produce human-readable output.  The output can be customized by subclassing AsmUnparser (see disassemble.C for an
     * example). This method of output is also more efficient than calling the old unparseAsmStatement() since there's no need
     * to buffer the string representation in memory first. */
    AsmUnparser().unparse(std::cout, block);
}
