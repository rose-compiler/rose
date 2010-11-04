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
void
readTraceFile ( const string & traceFileName )
   {
     const size_t MAX_INST_SIZE = 10;

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
   }


int main( int argc, char * argv[] )
   {
     if (argc != 2)
        {
          fprintf(stderr, "Usage: %s <trace file>\n", argv[1]);
          return -1;
        }

  // The 2nd parameter is the name of the trace file.
     string traceFileName = argv[1];

  // Read the trace file...
     readTraceFile(traceFileName);


#if 0
  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     NopTransform t;
     t.traverse(project,preorder);

  // regenerate the original executable.
     return backend(project);
#endif
   }
