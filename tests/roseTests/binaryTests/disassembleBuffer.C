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

#include <fcntl.h>
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
        add_instructions(insns);
        add_function(entry_va, SgAsmFunctionDeclaration::FUNC_ENTRY_POINT, name);
        return build_ast();
    }
};


int
main(int argc, char* argv[])
{
    if (argc<3) {
        fprintf(stderr, "usage: %s FILENAME VADDR\n", argv[0]);
        exit(1);
    }
    const char* filename = argv[1];
    uint64_t vaddr = strtoll(argv[2], NULL, 0);

    /* Read the supplied file into our memory, and then tell ROSE how that memory would have been mapped into
     * the process' memory by the loader. In this case, we just map the entire buffer to a contiguous region
     * of the process starting at the address specified on the command line. */
    size_t buf_sz;
    const unsigned char *buf = read_file(filename, &buf_sz);
    MemoryMap mm;
    mm.insert(MemoryMap::MapElement(vaddr, buf_sz, buf, 0, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC));

    /* Create the SgAsmGenericFile object that will be needed later. We could have used its parse() method to read the
     * contents of our file, but this example's purpose is to clearly demonstrate how to disassemble a buffer that might not
     * have even come from a file. */
    SgAsmGenericFile *file = new SgAsmGenericFile();

    /* In order to disassemble, we need a Disassembler object.  The most convenient way to do this is to ask the Disassembler
     * class to create a disassembler from its list of known disassembler subclasses.  We need to provide a information about
     * what kind of instructions will be disassembled, which is done through the instruction set architecutre (InsSetArchecture)
     * data member of an SgAsmGenericHeader object. Fortunately, the SgAsmPEFileHeader subclass' constructor fills in
     * reasonable values for the instruction set (i.e., 32-bit x86). */
    SgAsmPEFileHeader *pe = new SgAsmPEFileHeader(file);
    Disassembler *d = Disassembler::create(pe);
    
    /* Disassemble. The two null arguments can be used to obtain information about successor addresses (where the recursive
     * disassembly attempted to disassemble something outside the memory map) and failures (addresses and error messages where
     * disassembly failed. */
    Disassembler::InstructionMap insns = d->disassembleBuffer(&mm, vaddr, NULL, NULL);

    /* A partitioner can reorganize the instructions into an AST if you desire.  This is necessary if you plan to use any
     * ROSE's analysis or output functions since they operate exclusively on the tree representation. */
    SgAsmBlock *block = MyPartitioner().partition(insns, vaddr, "test_function");

    /* Produce human-readable output. */
    std::cout <<unparseAsmStatement(block);
}
