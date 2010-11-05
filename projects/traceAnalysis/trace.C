#include "rose.h"

/*
   This test program takes an input example code (trivial) and
   an instruction trace.

   1) Read the instruction trace into a data strucutre.

   2) Extends the main function with a block of nop's equal to the size
   required to insert the trace into the block.

   3) Compile the transformed source code to build an executable.

   4) Reads the generated executable and does analysis.  This could be
      a separate step since after step #3 the binary has been generated.

   5) Do transformations on the AST of the generated binary executable.



 */

// #define __STDC_FORMAT_MACROS
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>


using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


#if 0
// This code does not compile (documentation only).
struct trace_hdr
{
  unsigned short magic;  //Magic values:  "ET"
  unsigned short version;  //51 currently.
  unsigned long num_proc;  //number of processes in the trace
  
  /* the following are repeated num_proc times */
  unsigned long proc_name; //process name
  unsigned long pid;  //process id
  unsigned long num_mods;  //number of modules in the process, we don't use this.  so this will be zero for now.
  
};

struct instruction_x86_trace
{
  unsigned long exec; //eip
  unsigned long tid; //thread id
  unsigned char inst_size;  //instruction size
  unsigned char [inst_size]; //array of inst_size for the raw bytes.
  
  unsigned long eflags; //CPU flags
  
  /*registers values*/
  unsigned long eax;
  unsigned long ebx;
  unsigned long ecx;
  unsigned long edx;
  unsigned long esi;
  unsigned long edi;
  unsigned long ebp;
  unsigned long esp;

  unsigned short cs;
  unsigned short ss;
  unsigned short es;
  unsigned short ds;
  unsigned short fs;
  unsigned short gs;
}

struct trace_file
{
  struct trace_hdr HDR;//trace header
  struct instruction_x86_trace[]; //instructions until end of file
}
#endif


// Read the trace file (using example).
// void readTraceFile ( const string & traceFileName );
// read_file(const string & traceFileName, size_t *file_sz)
vector<unsigned char>
read_file(const string & traceFileName)
   {
     const size_t MAX_INST_SIZE = 10;
     uint64_t instructionAddress = 0;

     SgAsmGenericFile *file = new SgAsmGenericFile();
     SgAsmPEFileHeader *pe = new SgAsmPEFileHeader(file);
     Disassembler *disassembler = Disassembler::lookup(pe)->clone();
     ROSE_ASSERT(disassembler!=NULL);

     vector<unsigned char> opCodeBuffer;

     printf ("Opening trace file = %s \n",traceFileName.c_str());
     FILE * tf = fopen(traceFileName.c_str(), "rb");
     if (!tf)
        {
          fprintf(stderr, "Error opening trace file: %s\n",traceFileName.c_str());
          ROSE_ASSERT(false);
        }
#if 0
     FILE * tf = fopen(argv[1], "rb");
     if (!tf)
        {
          fprintf(stderr, "Error opening trace file: %s\n", argv[1]);
          return -1;
        }
#endif
     unsigned short magic=0;
     fread(&magic, 2, 1, tf);

     if (magic != 0x5445)
        {
          fprintf(stderr, "Trace magic value not recognized\n");
          fclose(tf);
          ROSE_ASSERT(false);
        }
#if 0
     if (magic != 0x5445)
        {
          fprintf(stderr, "Trace magic value not recognized\n");
          fclose(tf);
          return -1;
        }
#endif
     unsigned short version=0;
     unsigned long num_proc=0;
     fread(&version, 2, 1, tf);
     fread(&num_proc, 4, 1, tf);

     printf("header magic: %x\n", magic);
     printf("version: %d\n", version);
     printf("number of processes: %ld\n", num_proc);

     unsigned int i=0;
     for (i=0; i<num_proc; i++)
        {
          char proc_name[32];
          unsigned long pid;
          unsigned long num_mods;
          fread(proc_name, 32, 1, tf);
          fread(&pid, 4, 1, tf);
          fread(&num_mods, 4, 1, tf);
          printf("process name: %s\t PID: %ld\n", proc_name, pid);
          unsigned int j =0;
       // DQ (11/1/2010): Required change to run properly
       // for (j=0; j<num_mods; j++)
       // for (j = 0; j < (int) num_mods; j++)
          for (j = 0; j < (unsigned int) num_mods; j++)
             {
               char mod_name[32];
               unsigned long mod_base;
               unsigned long mod_size;
               fread(mod_name, 32, 1, tf);
               fread(&mod_base, 4, 1, tf);
               fread(&mod_size, 4, 1, tf);
               printf("\tmodule: %s @ %lx size: %ld\n", mod_name, mod_base, mod_size);
             }

          while (!feof(tf))
             {
               unsigned long exec, tid;
               unsigned long inst_size=0;
               unsigned char inst_bytes[MAX_INST_SIZE];
	  
               unsigned long eflags;
               unsigned long eax, ebx, ecx, edx, esi,edi,ebp,esp;
               unsigned short cs,ss,es,ds,fs,gs;

               fread(&exec, 4, 1, tf);
               fread(&tid, 4, 1, tf);
               fread(&inst_size, 1, 1, tf);
               fread(inst_bytes, inst_size,1,tf);

               printf("instruction: %lx, tid: %ld, inst_size: %ld. ", exec, tid, inst_size);

               printf("raw:");
               j=0;
               for (j=0; j<inst_size; j++)
                  {
                    printf("%x ", inst_bytes[j]);
                  }
               printf("\n");

            // insnData.second = isSgAsmx86Instruction(disassembler->disassembleOne(&insnBuf[0], addr, insnBuf.size(), addr, NULL));
            // SgAsmx86Instruction* instruction = disassembler->disassembleOne(&insnBuf[0], addr, insnBuf.size(), addr, NULL);
               SgAsmx86Instruction* instruction = isSgAsmx86Instruction(disassembler->disassembleOne(inst_bytes, instructionAddress, inst_size, instructionAddress, NULL));
               ROSE_ASSERT(instruction != NULL);

            // Increment the instruction address as though it were a linear stream of unique instructions (likely it is not but handle that detail later).
               instructionAddress += inst_size;

            // string stringifiedInstruction = instruction->unparseToString();
               string stringifiedInstruction = instruction->get_mnemonic();

               printf ("\tstringifiedInstruction = %s \n",stringifiedInstruction.c_str());

               fread(&eflags, 4, 1, tf);
               fread(&eax,4, 1, tf);
               fread(&ebx,4, 1, tf);
               fread(&ecx,4, 1, tf);
               fread(&edx,4, 1, tf);
               fread(&esi,4, 1, tf);
               fread(&edi,4, 1, tf);
               fread(&ebp,4, 1, tf);
               fread(&esp,4, 1, tf);

               fread(&cs,2, 1, tf);
               fread(&ss,2, 1, tf);
               fread(&es,2, 1, tf);
               fread(&ds,2, 1, tf);
               fread(&fs,2, 1, tf);
               fread(&gs,2, 1, tf);

               printf("\tCPU: eflags:%lx eax:%lx ebx:%lx ecx:%lx edx:%lx esi:%lx edi:%lx ebp:%lx esp:%lx\t cs:%x ss:%x es:%x ds:%x fs:%x gs:%x\n",
                    eflags, eax, ebx, ecx, edx, esi, edi, ebp, esp, cs, ss, es, ds, fs, gs);
             }
        }//end for all processes

     printf ("Closing file: %s \n",traceFileName.c_str());
     fclose(tf);

     return opCodeBuffer;
   }

#if 0
/* Read our example file into a memory buffer.  The file contains only the instructions and no PE file header or other
 * container information. */
static const unsigned char*
OLD_read_file(const char* filename, size_t *file_sz)
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
#endif

/* Define our own partitioner that doesn't do any real function detection analysis. It just takes all the instructions and
 * stuffs them into a single function. */
class MyPartitioner: public Partitioner
   {
     public:
          SgAsmBlock* partition(const Disassembler::InstructionMap& insns, rose_addr_t entry_va, const std::string& name)
             {
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
     if (argc != 2)
        {
          fprintf(stderr, "Usage: %s <trace file>\n", argv[1]);
          return -1;
        }

  // The 2nd parameter is the name of the trace file.
     string traceFileName = argv[1];

  // Read the trace file...
  // readTraceFile(traceFileName);
     vector<unsigned char> opCodeBuffer = read_file(traceFileName);

  // Code modified from disassembleBuffer.C
#if 0
     if (argc<3) {
        fprintf(stderr, "usage: %s FILENAME START_ADDR [ENTRY_ADDR]\n", argv[0]);
        exit(1);
     }
#endif

#if 0
  // This is a demonstration of how to build the build the trace into an executable (ELF first an then later a PE executable).

     const char* filename = argv[1];
     uint64_t start_va = strtoll(argv[2], NULL, 0);
     uint64_t entry_va = argc>3 ? strtol(argv[3], NULL, 0) : start_va;

  /* Read the supplied file into our memory, and then tell ROSE how that memory would have been mapped into
   * the process' memory by the loader. In this case, we just map the entire buffer to a contiguous region
   * of the process starting at the address specified on the command line. */
     size_t buf_sz;
  // const unsigned char *buf = read_file(filename, &buf_sz);
     vector<unsigned char> opCodeBuffer = read_file(filename);
     const unsigned char *buf = &(opCodeBuffer[0]);

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
#endif
   }

