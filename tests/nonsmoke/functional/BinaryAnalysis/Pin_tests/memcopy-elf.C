// Required header files for Intel Pin tool

// This is a code that is intended to become a tool to
// read the meemory image of an executable and reconstruct 
// the file format.  It is a modification of some open
// Intel Pin example code on thw web with the Windows 
// specific parts commented out.  This is part of a
// learning exercise for binary instrumentation support 
// in ROSE using Intel Pin.

// #include "pin.H"
// #include "portability.H"

// If we don't include "rose.h" then at least include "rose_config.h" so that
// the configure support can be seen and the tests in IntelPinSupport.h will not fail.
#include <rose_config.h>
#include "IntelPinSupport.h"

#include <iostream>
#include <map>

// Let "std" be the default.
using namespace std;

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

UINT64 ins_count = 0;
FILE *fout;
#define FILENAME_SIZE 128
// char filename[FILENAME_SIZE];
string filename;
map<unsigned int, unsigned int> writtenAddrs;
unsigned int dumpnum = 0;


INT32 Usage()
   {
  // This function is called when there is a problem in the initial parsing fo the Pin commandline
     cerr << "This tool prints out the memory image of an instrumented process.\n\n";

  // Prints a summary of all the knobs declared.
     cerr << KNOB_BASE::StringKnobSummary() << endl;

  // Return an error to the OS.
     return -1;
   }


int ReadPEHeader(unsigned int *ImageBase, unsigned int *ImageSize /* , WINDOWS::LPSTR fname */)
   {
  // Read the PE Optional Header in order to get ImageBase and ImageSize needed for dumper

#if 0
 // Commented out the Windows specific code.

    WINDOWS::HANDLE hFile = NULL;
    WINDOWS::HANDLE hFileMapping = NULL;
    WINDOWS::LPVOID lpFileBase = NULL;
    WINDOWS::PIMAGE_DOS_HEADER dosHeader = NULL;
    WINDOWS::PIMAGE_NT_HEADERS pNTHeader = NULL;
	
    hFile = WINDOWS::CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                    
    hFileMapping = WINDOWS::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
   
    lpFileBase = WINDOWS::MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
   
    dosHeader = (WINDOWS::PIMAGE_DOS_HEADER)lpFileBase;

    if ( dosHeader->e_magic == IMAGE_DOS_SIGNATURE )
    {
        pNTHeader = MakePtr( WINDOWS::PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew );
		
		if ( pNTHeader->Signature == IMAGE_NT_SIGNATURE )
        {
			WINDOWS::PIMAGE_OPTIONAL_HEADER pOptHeader = (WINDOWS::PIMAGE_OPTIONAL_HEADER)&pNTHeader->OptionalHeader;

			*ImageBase = (unsigned int) pOptHeader->ImageBase;
			*ImageSize = (unsigned int) pOptHeader->SizeOfImage;
        }
    }
	else 
	{
        printf("unrecognized file format\n");
	}

    WINDOWS::UnmapViewOfFile(lpFileBase);
    WINDOWS::CloseHandle(hFileMapping);
    WINDOWS::CloseHandle(hFile);
#endif

	  return 1;
   }


void DumpProcMem(unsigned int ImageBase, unsigned int ImageSize, unsigned int newoep /* , WINDOWS::DWORD pid */) 
   {
  // Dump PE process memory to a file. IAT and file still needs to be rebuilt
#if 0
 // Commented out the Windows specific code.

   char foutname[FILENAME_SIZE];

	WINDOWS::SIZE_T ReadBytes = 0;
	WINDOWS::SIZE_T WriteBytes = 0;
	WINDOWS::PIMAGE_DOS_HEADER dosHeader = NULL;
   WINDOWS::PIMAGE_NT_HEADERS pNTHeader = NULL;
	WINDOWS::PIMAGE_FILE_HEADER pImgFileHdr = NULL;
	WINDOWS::PIMAGE_OPTIONAL_HEADER pOptHeader = NULL;

	unsigned char * buffer = (unsigned char *) malloc( ImageSize );

	memset(foutname, 0, sizeof(foutname));
	memcpy(buffer, (void *)ImageBase, ImageSize);

	sprintf(foutname, "dump-%u-%s", dumpnum++, filename);

	WINDOWS::HANDLE hFile = WINDOWS::CreateFile(foutname,   
                    GENERIC_READ|GENERIC_WRITE,         
                    0,      
                    NULL,                 
                    OPEN_ALWAYS,        
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);                

	// Find the Entry Point
	dosHeader = (WINDOWS::PIMAGE_DOS_HEADER) buffer;
	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
        pNTHeader = MakePtr( WINDOWS::PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew );
		
		if ( pNTHeader->Signature == IMAGE_NT_SIGNATURE )
        {
			WINDOWS::PIMAGE_OPTIONAL_HEADER pOptHeader = (WINDOWS::PIMAGE_OPTIONAL_HEADER)&pNTHeader->OptionalHeader;
			
			fprintf(fout, "Current AddressOfEntryPoint 0x%8.8x setting to newoep (0x%x) RVA (0x%x)\n", 
				pOptHeader->AddressOfEntryPoint,
				newoep,
				newoep - ImageBase);
			pOptHeader->AddressOfEntryPoint = newoep - ImageBase;
        }
	}

    WINDOWS::WriteFile(hFile, buffer, ImageSize, &WriteBytes, NULL); 
    WINDOWS::CloseHandle(hFile);

	free(buffer);
#endif
}


VOID doDetach(void *ip)
   {
  // Dump the process image.

     unsigned long ImageBase = 0;
     unsigned long ImageSize = 0;

  // Build a character buffer for use with sprintf()
     char outfilename[FILENAME_SIZE];
     memset(outfilename, 0, sizeof(outfilename));

     fprintf(fout, "ImageBase 0x%8.8lx Halting EIP Addr: 0x%8.8lx (Diff: 0x%lx) ImageSize %lx\n",ImageBase,(unsigned long) ip,(unsigned long) ip - ImageBase,ImageSize);

  // WINDOWS::pedump ocpe(filename, (unsigned char *) ip);

     sprintf(outfilename, "dump-%u-%s", dumpnum++, filename.c_str());
  // ocpe.saveDumpFile(outfilename);

  // Pin can relinquish control of application any time when invoked via PIN_Detach. Control 
  // is returned to the original uninstrumented code and the application runs at native speed. 
  // Thereafter no instrumented code is ever executed.
     PIN_Detach();
   }


VOID measureip(void *ip)
   {
  // This function is executed for every instruction that is executed.

  // printf ("Instruction address = %p \n",ip);

     if (writtenAddrs.find((unsigned long) ip) != writtenAddrs.end())
        {
          doDetach(ip);
        }
   }


VOID MemoryWrite(void *ip, void *addr)
   {
  // Check if this is an address that has been written
     if (writtenAddrs.find((unsigned long) addr) == writtenAddrs.end())
        {
       // This address has not been written before, so save the address
          writtenAddrs[(unsigned long) addr] = (unsigned long)ip;
        }

  // Check if this is an instruction that has been written do before
     if (writtenAddrs.find((unsigned long) ip) != writtenAddrs.end())
        {
          doDetach(ip);
        }
   }


VOID Instruction(INS ins, VOID *v)
   {
  // Pin calls this function every time a new instruction is encountered

     printf ("Executing a new instruction v = %p \n",v);

  // Insert a call to measureip() before every instruction, and pass it the IP
     INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)measureip, IARG_INST_PTR, IARG_END);

  // DQ (3/6/2009): This function does not appear to compile (had to #define USING_XED).
     if (INS_IsMemoryWrite(ins))
        {
       // Generate a call to MemoryWrite() for any address except predicated addresses where the predicate is false.
       // So if the address was a part of the instruction but it was not used, then ignore the call to MemoryWrite().
          INS_InsertPredicatedCall(ins,IPOINT_BEFORE, (AFUNPTR) MemoryWrite,IARG_INST_PTR,IARG_MEMORYWRITE_EA,IARG_END);
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

     fprintf(fout, "Count: %lu\n", ins_count);
     fclose(fout);
   }


VOID detachCallback(VOID *v)
   {
  // Nothing goes here

     printf ("Inside of PIN_Detach() callback function \n");
   }


int
main(int argc, char *argv[])
   {
     printf ("Starting Intel Pin tool to generate memory image \n");

     if( PIN_Init(argc,argv) )
        {
          return Usage();
        }

     filename = argv[argc-1];
     printf ("Processing filename = %s \n",filename.c_str());

     string outputFilename = "memory-image.out";
     fout = fopen(outputFilename.c_str(), "w");

     fprintf(fout, "Debugging %s\n", argv[argc-1]);

  // Setup instrumentation callback function.
     INS_AddInstrumentFunction(Instruction, 0);

  // Setup the callback function to detach instumentation.
     PIN_AddDetachFunction(detachCallback, 0);

  // Register Fini to be called when the application exits (not required, but then Fini() won't be called).
     PIN_AddFiniFunction(Fini, 0);

  // Never returns
     PIN_StartProgram();

  // This is never called (because PIN_StartProgram() never returns).
     printf ("Leaving Intel Pin tool to generate memory image \n");

     return 0;
   }

