//
// This tool prints a trace of image load and unload events
//

// If we don't include "rose.h" then at least include "rose_config.h" so that
// the configure support can be seen and the tests in IntelPinSupport.h will not fail.
#if 0
#include "rose.h"
#else
#include <rose_config.h>
#include "IntelPinSupport.h"

#include <stdio.h>
#endif

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

FILE * trace;

// Pin calls this function every time a new img is loaded
// It can instrument the image, but this example does not
// Note that imgs (including shared libraries) are loaded lazily

VOID ImageLoad(IMG img, VOID *v)
   {
     printf("Loading %s, Image id = %d\n", IMG_Name(img).c_str(), IMG_Id(img));
     fprintf(trace, "Loading %s, Image id = %d\n", IMG_Name(img).c_str(), IMG_Id(img));
   }

// Pin calls this function every time a new img is unloaded
// You can't instrument an image that is about to be unloaded
VOID ImageUnload(IMG img, VOID *v)
   {
     printf("Unloading %s\n", IMG_Name(img).c_str());
     fprintf(trace, "Unloading %s\n", IMG_Name(img).c_str());
   }

// This function is called when the application exits
// It prints the name and count for each procedure
VOID Fini(INT32 code, VOID *v)
   {
     fclose(trace);
   }


#include "sys/fcntl.h"
#include "elf.h"
#include "libelf.h"
#include "assert.h"
#include "libdwarf.h"

void
process_dwarf(string file_name)
   {
    int f;
    Elf_Cmd cmd;
    Elf* elf;
    Elf *arf;
    int archive = 0;

    (void) elf_version(EV_NONE);
    if (elf_version(EV_CURRENT) == EV_NONE) {
        (void) fprintf(stderr, "dwarfdump: libelf.a out of date.\n");
        assert(false);
    }

    f = open(file_name.c_str(), O_RDONLY);
    if (f == -1) {
        fprintf(stderr, "ERROR:  can't open %s \n",file_name.c_str());
        assert(false);
    }

    cmd = ELF_C_READ;
    arf = elf_begin(f, cmd, (Elf *) 0);
    if (elf_kind(arf) == ELF_K_AR)
       {
         archive = 1;
       }

          Dwarf_Debug dbg;
          Dwarf_Error err;

     while ((elf = elf_begin(f, cmd, arf)) != 0)
        {
          Elf32_Ehdr *eh32;

#ifdef HAVE_ELF64_GETEHDR
          Elf64_Ehdr *eh64;
#endif /* HAVE_ELF64_GETEHDR */
          eh32 = elf32_getehdr(elf);
          if (!eh32)
             {
#ifdef HAVE_ELF64_GETEHDR
            /* not a 32-bit obj */
               eh64 = elf64_getehdr(elf);
               if (!eh64) {
                /* not a 64-bit obj either! */
                /* dwarfdump is quiet when not an object */
               }
#endif /* HAVE_ELF64_GETEHDR */
             }

          printf ("Calling dwarf_elf_init() \n");
          int dres = dwarf_elf_init(elf, DW_DLC_READ, NULL, NULL, &dbg, &err);

          if (dres == DW_DLV_NO_ENTRY)
             {
               printf("No DWARF information present in %s\n", file_name.c_str());
               assert(false);
             }

          if (dres != DW_DLV_OK)
             {
               printf ("Dwarf error: dwarf_elf_init \n");
               assert(false);
             }

          cmd = elf_next(elf);
          elf_end(elf);
        }

     elf_end(arf);

     int dwarf_init_status = dwarf_init (f, DW_DLC_READ, NULL, NULL, &dbg, &err);

     printf ("Leaving process_dwarf() for file = %s \n",file_name.c_str());
   }

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int
main(int argc, char * argv[])
   {
     trace = fopen("imageload.out", "w");

  // DQ (use an explicit name for testing.
     process_dwarf("input_testcode");
     process_dwarf("input_testcode");

#if 0
  // Set the verbose level of ROSE (for debugging).
     SgProject::set_verbose(2);

     printf ("argc = %d \n",argc);
     for (int i=0; i < argc; i++)
        {
          printf ("argv[%d] = %s \n",i,argv[i]);
        }

  // This is the typical location of the executable file name in a simple Pin commandline
     string image_name = (char*) argv[argc-1];
     printf ("Executable to analyze: %s \n",image_name.c_str());

  // Need to process the commandline...
     vector<string> sourceCommandline;
     sourceCommandline.push_back("intel_pin_wrapper");
     sourceCommandline.push_back(image_name);

     printf ("Now build AST for binary executable: %s \n",image_name.c_str());

  // After gathering dynamic information, run the static analysis...using ROSE
     SgProject* project = frontend(sourceCommandline);
     ROSE_ASSERT (project != NULL);

     printf ("Running tests on AST for binary executable: %s \n",image_name.c_str());
  // Internal AST consistancy tests.
     AstTests::runAllTests(project);


  // Add any interesting static binary analysis here...or anywhere else...


  // Regenerate binary or source code
     int status = backend(project);
     ROSE_ASSERT(status == 0);
#endif

  // Initialize symbol processing
     PIN_InitSymbols();
    
  // Initialize pin
     PIN_Init(argc, argv);
    
  // Register ImageLoad to be called when an image is loaded
     IMG_AddInstrumentFunction(ImageLoad, 0);

  // Register ImageUnload to be called when an image is unloaded
     IMG_AddUnloadFunction(ImageUnload, 0);

  // Register Fini to be called when the application exits
     PIN_AddFiniFunction(Fini, 0);
    
  // Start the program, never returns
     PIN_StartProgram();
    
     return 0;
   }
