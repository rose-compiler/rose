#include "rose.h"

/*
   This test program takes an input example code (trivial) and
   an instruction trace.

   1) Reads the instruction trace (trace file) into a data structure.

   2) Generates a SgAsmBlock of instructions.

   3) Builds an Elf executable from scratch (as a.out).


      *****  Steps beyond this point are not finished  *****

   4) Inserts the instruction block into the Elf executable.

   5) Generate the Elf binary with the generated instruction block.

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


// Read the trace file (using the example code for reading a trace).
Disassembler::InstructionMap
read_trace_file(const string & traceFileName, uint64_t & entry_va )
   {
     const size_t MAX_INST_SIZE = 10;
     uint64_t instructionAddress = 0;

     SgAsmGenericFile *file = new SgAsmGenericFile();
     SgAsmPEFileHeader *pe = new SgAsmPEFileHeader(file);
     Disassembler *disassembler = Disassembler::lookup(pe)->clone();
     ROSE_ASSERT(disassembler!=NULL);

  // typedef std::map<rose_addr_t, SgAsmInstruction*> InstructionMap;
     Disassembler::InstructionMap instructionMapOfTrace;

  // vector<unsigned char> opCodeBuffer;

     printf ("Opening trace file = %s \n",traceFileName.c_str());
     FILE * tf = fopen(traceFileName.c_str(), "rb");
     if (!tf)
        {
          fprintf(stderr, "Error opening trace file: %s\n",traceFileName.c_str());
          ROSE_ASSERT(false);
        }

     unsigned short magic=0;
     fread(&magic, 2, 1, tf);

     if (magic != 0x5445)
        {
          fprintf(stderr, "Trace magic value not recognized\n");
          fclose(tf);
          ROSE_ASSERT(false);
        }

     unsigned short version=0;
     unsigned long num_proc=0;
     fread(&version, 2, 1, tf);
     fread(&num_proc, 4, 1, tf);

     printf("header magic: %x\n", magic);
     printf("version: %d\n", version);
     printf("number of processes: %ld\n", num_proc);

  // This allows us to debug on a small ssubset of the trace.
     int counter = 0;

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

            // Set the entry point with the first instruction (note instructionAddress is initialized to 0 at start of function).
               if (instructionAddress == 0)
                  {
                    entry_va = exec;
                  }

               instructionAddress = exec;

               SgAsmx86Instruction* instruction = isSgAsmx86Instruction(disassembler->disassembleOne(inst_bytes, instructionAddress, inst_size, instructionAddress, NULL));
               ROSE_ASSERT(instruction != NULL);

               if (instructionMapOfTrace.find(instructionAddress) == instructionMapOfTrace.end())
                  {
                    printf ("New instruction at this address instructionAddress = %p \n",(void*) instructionAddress);
                    instructionMapOfTrace.insert(pair<rose_addr_t,SgAsmInstruction*>(instructionAddress,instruction));
                  }
                 else
                  {
                    printf ("An instruction at this address has been seen instructionAddress = %p \n",(void*) instructionAddress);
                  }

            // Increment the instruction address as though it were a linear stream of unique instructions (likely it is not but handle that detail later).
            // instructionAddress += inst_size;

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
#if 0
               counter++;
               if (counter > 10)
                  {
                    printf ("Exiting eary as a test: closing file: %s \n",traceFileName.c_str());
                    fclose(tf);
                    return instructionMapOfTrace;
                  }
#endif
             }
        }//end for all processes

     printf ("Closing file: %s \n",traceFileName.c_str());
     fclose(tf);

  // return opCodeBuffer;
     return instructionMapOfTrace;
   }


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


// Function prototype...
SgAsmGenericFile* generateExecutable(SgAsmBlock *block);

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
  // vector<unsigned char> opCodeBuffer = read_trace_file(traceFileName);
     uint64_t entry_va = 0;
     Disassembler::InstructionMap insns = read_trace_file(traceFileName,entry_va);

  /* A partitioner can reorganize the instructions into an AST if you desire.  This is necessary if you plan to use any
   * ROSE's analysis or output functions since they operate exclusively on the tree representation. */
     SgAsmBlock *block = MyPartitioner().partition(insns, entry_va, "entry_function");

  /* Produce human-readable output.  The output can be customized by subclassing AsmUnparser (see disassemble.C for an
   * example). This method of output is also more efficient than calling the old unparseAsmStatement() since there's no need
   * to buffer the string representation in memory first. */
     AsmUnparser().unparse(std::cout, block);

  // This work is incomplete, the block is not yet assembled into a function in the file.
     SgAsmGenericFile* file = generateExecutable(block);
     ROSE_ASSERT(file != NULL);

#if 0
  // This code does not yet work...

  // Now generate the executable...
     SgProject* project = new SgProject();
     SgBinaryFile* binaryFile = new SgBinaryFile(file);

  // Old deprecated interface...
  // project->set_file(*binaryFile);
     project->get_fileList().push_back(binaryFile);

  // Call the backend to generate an executable.
     backend(project); 
#endif

     return 0;
   }

