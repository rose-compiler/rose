// If we don't include "rose.h" then at least include "rose_config.h" so that
// the configure support can be seen and the tests in IntelPinSupport.h will not fail.
#include <rose_config.h>
#include "IntelPinSupport.h"

#include <iostream>
#include <map>
#include <set>

#include <assert.h>

// Let "std" be the default.
using namespace std;

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

UINT64 ins_count = 0;
FILE *fout;

string filename;

set<void*> instructions;
set<pair<void*,void*> > instructionEdges;

map<void*, string> disassembledInstructions;

void *previousInstructionIP = NULL;

unsigned long instructionCounter = 0;

void* lowerBoundAddessRange = NULL;
void* upperBoundAddessRange = NULL;

INT32 Usage()
   {
  // This function is called when there is a problem in the initial parsing fo the Pin commandline
     cerr << "This Intel Pin tool to generate a graph of the memory trace.\n\n";

  // Prints a summary of all the knobs declared.
     cerr << KNOB_BASE::StringKnobSummary() << endl;

  // Return an error to the OS.
     return -1;
   }

VOID graphInstruction(void *ip)
   {
  // This function is executed for every instruction that is executed.

  // printf ("Instruction address = %p \n",ip);

  // Limit the size of the generated dot file (to avoid generating too large of a graph for the layout algorithm).
  // if (instructionCounter < 4000)

  // Make sure these have ben set.
     assert(lowerBoundAddessRange != NULL);
     assert(upperBoundAddessRange != NULL);

  // Now we just limit the instructions to those from the ".text" segment of the main problem (where IMG_IsMainExecutable(img) == true).
     if (ip >= lowerBoundAddessRange && ip <= upperBoundAddessRange)
        {
          assert(disassembledInstructions.find(ip) != disassembledInstructions.end());
          string disassembledInstructionString = disassembledInstructions[ip];

          if (instructions.find(ip) == instructions.end())
             {
               instructions.insert(ip);
            // fprintf(fout,"\"%p\"[label=\"%p\"];\n",ip,ip);
               fprintf(fout,"\"%p\"[label=\"  %p : %s  \" color=lightblue];\n",ip,ip,disassembledInstructionString.c_str());
             }

          if (previousInstructionIP == NULL)
             {
            // Make the starting node more obvious.
               fprintf(fout,"\"%p\"[label=\"START\" shape=polygon,regular=0,sides=5,peripheries=1,color=Green,fillcolor=Green,fontcolor=black,style=filled];\n",previousInstructionIP);
             }

       // "0x680660" -> "0x2a99aff010"[label="header" dir=both ];
          pair<void*,void*> edge = pair<void*,void*>(previousInstructionIP,ip);

          if (instructionEdges.find(edge) == instructionEdges.end())
             {
               instructionEdges.insert(edge);
               fprintf(fout,"\"%p\" -> \"%p\"[label=\"%lu\" color=\"lightblue\" decorate dir=forward ]\n",previousInstructionIP,ip,instructionCounter);
             }

          previousInstructionIP = ip;
        }

     instructionCounter++;
   }


VOID Instruction(INS ins, VOID *v)
   {
  // Pin calls this function every time a new instruction is encountered

  // printf ("Executing a new instruction v = %p \n",v);

  // What we really want to do is pass this to the graphInstruction() function (or build a map of strings)...
  // OR, could alternatively use: INS_Disassemble(INS ins)
     void* ip = (void*) INS_Address(ins);
     string disassembledInstructionString = INS_Disassemble(ins);
#if 0
     printf ("Executing a new instruction ins at %p = %s = %s \n",ip,INS_Mnemonic(ins).c_str(),disassembledInstructionString.c_str());
#endif
     if (disassembledInstructions.find(ip) == disassembledInstructions.end())
        {
       // printf ("Adding %p = %s to map \n",ip,disassembledInstructionString.c_str());
          disassembledInstructions[ip] = INS_Disassemble(ins);
        }

  // Insert a call to measureip() before every instruction, and pass it the IP
     INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) graphInstruction, IARG_INST_PTR, IARG_END);
   }


VOID ImageLoad(IMG img, VOID * v)
   {
  // Output information about each image (main application or shared library, 
  // typically) as it is loaded during execution.
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));

     ADDRINT image_entry         = IMG_Entry(img);
  // Global pointer (GP) of image, if a GP is used to address global data
     ADDRINT image_globalPointer = IMG_Gp(img);
     ADDRINT image_loadOffset    = IMG_LoadOffset(img);
     ADDRINT image_lowAddress    = IMG_LowAddress(img);
     ADDRINT image_highAddress   = IMG_HighAddress(img);
     ADDRINT image_startAddress  = IMG_StartAddress(img);
     USIZE image_sizeMapped      = IMG_SizeMapped(img);
     bool isMainExecutable       = IMG_IsMainExecutable(img);
     IMG_TYPE image_type         = IMG_Type(img);

     if (isMainExecutable == true)
        {
          printf ("   image_entry         = 0x%zx \n",image_entry);
          printf ("   image_globalPointer = 0x%zx \n",image_globalPointer);
          printf ("   image_loadOffset    = 0x%zx \n",image_loadOffset);
          printf ("   image_lowAddress    = 0x%zx \n",image_lowAddress);
          printf ("   image_highAddress   = 0x%zx \n",image_highAddress);
          printf ("   image_startAddress  = 0x%zx \n",image_startAddress);
          printf ("   image_sizeMapped    = %u \n",image_sizeMapped);
          printf ("   isMainExecutable    = %s \n",isMainExecutable ? "true" : "false");
          printf ("   section_type        = %s \n",output(image_type).c_str());
        }

  // Output information about each section
     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
        {
       // trace << "Section " << SEC_Name(sec) << " found" << endl;

          string section_name = SEC_Name(sec);
          if (isMainExecutable == true)
             {
               printf ("***section_name = %s \n",section_name.c_str());
             }

          bool isReadable   = SEC_IsReadable(sec);
          bool isWritable   = SEC_IsWriteable(sec);
          bool isExecutable = SEC_IsExecutable(sec);
          bool isMapped     = SEC_Mapped(sec);

          SEC_TYPE section_type    = SEC_Type(sec);
          ADDRINT section_address  = SEC_Address(sec);
          const VOID* section_data = SEC_Data(sec);
          USIZE section_size       = SEC_Size(sec);

          bool listAvailableFunctions = false;
          if (isMainExecutable == true && section_name == ".text")
             {
               lowerBoundAddessRange = (void*) section_address;
               upperBoundAddessRange = (void*) (section_address + section_size);

               RTN rtn = LEVEL_PINCLIENT::RTN_FindByName (img,"main");
               if (RTN_Valid(rtn) == true)
                  {
                    ADDRINT function_address = RTN_Address(rtn);
                    USIZE function_size      = RTN_Size(rtn);

                    cout << "      main routine: " << RTN_Name(rtn) << " address: " << function_address << " size: " << function_size << endl;

                 // If we find function "main" then set the range to that function.
                    lowerBoundAddessRange = (void*) function_address;
                    upperBoundAddessRange = (void*) (function_address + function_size);

                    listAvailableFunctions = true;
                  }
                 else
                  {
                    printf ("No main function found \n");
                  }
             }

          if (isMainExecutable == true && isExecutable == true)
             {
               printf ("      isReadable   = %s \n",isReadable   ? "true" : "false");
               printf ("      isWritable   = %s \n",isWritable   ? "true" : "false");
               printf ("      isExecutable = %s \n",isExecutable ? "true" : "false");
               printf ("      isMapped     = %s \n",isMapped     ? "true" : "false");
               printf ("      section_type = %s \n",output(section_type).c_str());
               printf ("      section_address = 0x%zx \n",section_address);
               printf ("      section_data = %p \n",section_data);
               printf ("      section_size = %u \n",section_size);
             }

          if (listAvailableFunctions == true)
             {
               for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
                  {
                    cout << "      routine: " << RTN_Name(rtn) << " address: " << RTN_Address(rtn) << " size: " << RTN_Size(rtn) << endl;

                    RTN_Open(rtn);
            
                    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
                       {
                      // Just print first and last
                         if (!INS_Valid(INS_Prev(ins)) || !INS_Valid(INS_Next(ins)))
                            {
                              cout << "         " << INS_Address(ins);
#if 0
                              cout << " " << INS_Disassemble(ins) << " read:";
                    
                              for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++)
                                 {
                                   cout << " " << REG_StringShort(INS_RegR(ins, i));
                                 }
                              cout << " writes:";
                              for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
                                 {
                                   cout << " " << REG_StringShort(INS_RegW(ins, i));
                                 }
#endif
                              cout << endl;
                            }
                       }

                    RTN_Close(rtn);
                  }
             }
        }
   }


VOID Fini(INT32 code, VOID *v)
   {
  // DQ (3/7/2009): Note from Pin 2.6 documentation:
  // Due to a compatibility issue between operating systems pin does *not*
  // provide support for registering atexit functions inside pintools
  // (which means that the behavior of a pintool that does so is
  // undefined). If you need such functionality, please register a Fini
  // function.

  // Close the dot graph.
     fprintf(fout,"}\n");
     fclose(fout);
   }


int
main(int argc, char *argv[])
   {
     printf ("Starting Intel Pin tool to generate a graph of the memory trace \n");

  // Must call this, before PIN_Init(), so that function symbols can be interpreted.
     PIN_InitSymbols();

  // Calling initialization for Pin.
     if( PIN_Init(argc,argv) )
        {
          return Usage();
        }

     filename = argv[argc-1];
     printf ("Processing filename = %s \n",filename.c_str());

     string outputFilename = "trace.dot";
     fout = fopen(outputFilename.c_str(), "w");

  // Start the dot graph
     fprintf(fout,"digraph \"G./%s.dot\" {\n", argv[argc-1]);

  // Setup instrumentation callback function.
     INS_AddInstrumentFunction(Instruction, 0);

  // Specify callback function when each image is loaded (main program or shared libraries).
     IMG_AddInstrumentFunction(ImageLoad, 0);

  // Register Fini to be called when the application exits (not required, but then Fini() won't be called).
  // We use the Fini() function to close off the DOT representation and close the file.
     PIN_AddFiniFunction(Fini, 0);

  // Never returns
     PIN_StartProgram();

  // This is never called (because PIN_StartProgram() never returns).
     printf ("Leaving Intel Pin tool to generate a graph of the memory trace \n");

     return 0;
   }

