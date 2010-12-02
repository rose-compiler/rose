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

#define __STDC_FORMAT_MACROS
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* Define to get lots of debugging output. */
//#define DO_TRACE

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
// Warning: This won't work if this machine's endianess doesn't match the trace file's endianess. [RPM]
Disassembler::InstructionMap
read_trace_file(const string & traceFileName, uint64_t & entry_va )
   {
     const size_t MAX_INST_SIZE = 10;           // Warning: x86 instructions can be up to 15 bytes [RPM]
     entry_va = 0;

     SgAsmGenericFile *file = new SgAsmGenericFile();
     SgAsmPEFileHeader *pe = new SgAsmPEFileHeader(file);
     Disassembler *disassembler = Disassembler::lookup(pe)->clone();
     ROSE_ASSERT(disassembler!=NULL);

  // typedef std::map<rose_addr_t, SgAsmInstruction*> InstructionMap;
     Disassembler::InstructionMap instructionMapOfTrace;

  // vector<unsigned char> opCodeBuffer;

#ifdef DO_TRACE
     printf ("Opening trace file = %s \n",traceFileName.c_str());
#endif
     FILE * tf = fopen(traceFileName.c_str(), "rb");
     if (!tf)
        {
          fprintf(stderr, "Error opening trace file: %s\n",traceFileName.c_str());
          ROSE_ASSERT(false);
        }

     uint16_t magic;
     fread(&magic, 2, 1, tf);

     if (magic != 0x5445)
        {
          fprintf(stderr, "Trace magic value not recognized\n");
          fclose(tf);
          ROSE_ASSERT(false);
        }

     uint16_t version=0;
     uint32_t num_proc=0;
     fread(&version, 2, 1, tf);
     fread(&num_proc, 4, 1, tf);

#ifdef DO_TRACE
     printf("header magic: 0x%"PRIx16"\n", magic);
     printf("version: %"PRIu16"\n", version);
     printf("number of processes: %"PRIu32"\n", num_proc);
#endif

  // This allows us to debug on a small ssubset of the trace.
     //int counter = 0;

     uint32_t i=0;
     for (i=0; i<num_proc; i++)
        {
          char proc_name[32];
          uint32_t pid;
          uint32_t num_mods;
          fread(proc_name, 32, 1, tf);
          fread(&pid, 4, 1, tf);
          fread(&num_mods, 4, 1, tf);
#ifdef DO_TRACE
          printf("process name: %s\t PID: %"PRIu32"\n", proc_name, pid);
#endif
          for (uint32_t j = 0; j < num_mods; j++)
             {
               char mod_name[32];
               uint32_t mod_base;
               uint32_t mod_size;
               fread(mod_name, 32, 1, tf);
               fread(&mod_base, 4, 1, tf);
               fread(&mod_size, 4, 1, tf);
#ifdef DO_TRACE
               printf("\tmodule: %s @ 0x%"PRIx32" size: %"PRIu32"\n", mod_name, mod_base, mod_size);
#endif
             }

          while (!feof(tf))
             {
               uint32_t exec, tid;
               size_t inst_size;
               unsigned char inst_bytes[MAX_INST_SIZE];
               uint8_t byte;
	  
               uint32_t eflags;
               uint32_t eax, ebx, ecx, edx, esi,edi,ebp,esp;
               uint16_t cs,ss,es,ds,fs,gs;

               fread(&exec, 4, 1, tf);
               fread(&tid, 4, 1, tf);
               fread(&byte, 1, 1, tf); inst_size = byte;
               ROSE_ASSERT(inst_size <= sizeof inst_bytes);
               fread(inst_bytes, inst_size,1,tf);

#ifdef DO_TRACE
               printf("instruction: va=0x%"PRIx32", tid=%"PRIu32", inst_size=%zu, raw=0x", exec, tid, inst_size);
               for (size_t j=0; j<inst_size; j++)
                  {
                    printf("%02x", inst_bytes[j]);
                  }
               printf("\n");
#endif

               if (0==entry_va)
                   entry_va = exec;

               SgAsmInstruction* instruction = disassembler->disassembleOne(inst_bytes, exec, inst_size, exec);
               ROSE_ASSERT(instruction != NULL);
               instructionMapOfTrace[exec] = instruction;
#ifdef DO_TRACE
               printf("\t%s\n", unparseInstructionWithAddress(instruction).c_str());
#endif

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

#ifdef DO_TRACE
               printf("\tCPU: eflags:0x%"PRIx32" eax:0x%"PRIx32" ebx:0x%"PRIx32" ecx:0x%"PRIx32" edx:0x%"PRIx32
                      " esi:0x%"PRIx32" edi:0x%"PRIx32" ebp:0x%"PRIx32" esp:0x%"PRIx32"\n"
                      "\t     cs:0x%"PRIx16" ss:0x%"PRIx16" es:0x%"PRIx16" ds:0x%"PRIx16" fs:0x%"PRIx16" gs:0x%"PRIx16"\n",
                      eflags, eax, ebx, ecx, edx, esi, edi, ebp, esp, cs, ss, es, ds, fs, gs);
#endif
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

#ifdef DO_TRACE
     printf ("Closing file: %s \n",traceFileName.c_str());
#endif
     fclose(tf);

  // return opCodeBuffer;
     return instructionMapOfTrace;
   }


// Function prototypes...
SgAsmGenericFile* generateElfExecutable(const std::string &name, const Disassembler::InstructionMap&, rose_addr_t entry_va);
SgAsmGenericFile* generatePeExecutable(const std::string &name, const Disassembler::InstructionMap&, rose_addr_t entry_va);

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
     std::cerr <<"Reading and disassembling trace file...\n";
     uint64_t entry_va = 0;
     Disassembler::InstructionMap insns = read_trace_file(traceFileName,entry_va);
     printf("found %zu instruction%s\n", insns.size(), 1==insns.size()?"":"s");

  // Now generate the executable...
     std::cerr <<"Generating ELF file: a.out\n";
     generateElfExecutable("a.out", insns, entry_va);
     std::cerr <<"Generating PE file:  a.exe\n";
     generatePeExecutable("a.exe", insns, entry_va);

     return 0;
   }

