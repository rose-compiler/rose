#include <iostream>
#include <fstream>
#include <string>
// #include "pin.H"

// If we don't include "rose.h" then at least include "rose_config.h" so that
// the configure support can be seen and the tests in IntelPinSupport.h will not fail.
#include <rose_config.h>
#include "IntelPinSupport.h"
#include <assert.h>

using namespace std;

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

/* ===================================================================== */
/* Global Variables and Definitions */
/* ===================================================================== */

std::ofstream trace;

/* ===================================================================== */

string output (IMG_TYPE image_type)
   {
  // Outputs a string with the image enum type (for display)

     string s;
     switch (image_type)
        {
          case IMG_TYPE_STATIC:      s = "IMG_TYPE_STATIC";      break;
          case IMG_TYPE_SHARED:      s = "IMG_TYPE_SHARED";      break;
          case IMG_TYPE_SHAREDLIB:   s = "IMG_TYPE_SHAREDLIB";   break;
          case IMG_TYPE_RELOCATABLE: s = "IMG_TYPE_RELOCATABLE"; break;

          default:
             {
               printf ("Error: default reached in switch image_type = %u \n",image_type);
               assert(false);
             }
        }

     return s;
   }


string output (SEC_TYPE section_type)
   {
  // Outputs a string with the section enum type (for display)

     string s;
     switch (section_type)
        {
          case SEC_TYPE_INVALID:            s = "SEC_TYPE_INVALID";    break;
          case SEC_TYPE_UNUSED:             s = "SEC_TYPE_UNUSED";     break;
          case LEVEL_CORE::SEC_TYPE_REGREL: s = "SEC_TYPE_REGREL";     break;
          case LEVEL_CORE::SEC_TYPE_DYNREL: s = "SEC_TYPE_DYNREL";     break;
          case LEVEL_CORE::SEC_TYPE_EXEC:   s = "SEC_TYPE_EXEC";       break;
          case LEVEL_CORE::SEC_TYPE_DATA:   s = "SEC_TYPE_DATA";       break;
          case SEC_TYPE_DYNAMIC:            s = "SEC_TYPE_DYNAMIC";    break;
          case SEC_TYPE_OPD:                s = "SEC_TYPE_OPD";        break;
          case SEC_TYPE_GOT:                s = "SEC_TYPE_GOT";        break;
          case SEC_TYPE_STACK:              s = "SEC_TYPE_STACK";      break;
          case SEC_TYPE_PLTOFF:             s = "SEC_TYPE_PLTOFF";     break;
          case SEC_TYPE_HASH:               s = "SEC_TYPE_HASH";       break;
          case LEVEL_CORE::SEC_TYPE_LSDA:   s = "SEC_TYPE_LSDA";       break;
          case SEC_TYPE_UNWIND:             s = "SEC_TYPE_UNWIND";     break;
          case SEC_TYPE_UNWINDINFO:         s = "SEC_TYPE_UNWINDINFO"; break;
          case SEC_TYPE_REGSYM:             s = "SEC_TYPE_REGSYM";     break;
          case SEC_TYPE_DYNSYM:             s = "SEC_TYPE_DYNSYM";     break;
          case SEC_TYPE_DEBUG:              s = "SEC_TYPE_DEBUG";      break;
          case LEVEL_CORE::SEC_TYPE_BSS:    s = "SEC_TYPE_BSS";        break;
          case SEC_TYPE_SYMSTR:             s = "SEC_TYPE_SYMSTR";     break;
          case SEC_TYPE_DYNSTR:             s = "SEC_TYPE_DYNSTR";     break;
          case SEC_TYPE_SECSTR:             s = "SEC_TYPE_SECSTR";     break;
          case SEC_TYPE_COMMENT:            s = "SEC_TYPE_COMMENT";    break;
          case LEVEL_CORE::SEC_TYPE_LOOS:   s = "SEC_TYPE_LOOS";       break;
          case SEC_TYPE_LAST:               s = "SEC_TYPE_LAST";       break;

          default:
             {
               printf ("Error: default reached in switch section_type = %u \n",section_type);
               assert(false);
             }
        }

     return s;
   }


VOID ImageLoad(IMG img, VOID * v)
   {
  // Output information about each image (main application or shared library, 
  // typically) as it is loaded during execution.
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));

     ADDRINT image_entry = IMG_Entry(img);
     printf ("   image_entry         = 0x%zx \n",image_entry);

  // Global pointer (GP) of image, if a GP is used to address global data
     ADDRINT image_globalPointer = IMG_Gp(img);
     printf ("   image_globalPointer = 0x%zx \n",image_globalPointer);

     ADDRINT image_loadOffset   = IMG_LoadOffset(img);
     printf ("   image_loadOffset    = 0x%zx \n",image_loadOffset);

     ADDRINT image_lowAddress   = IMG_LowAddress(img);
     printf ("   image_lowAddress    = 0x%zx \n",image_lowAddress);

     ADDRINT image_highAddress  = IMG_HighAddress(img);
     printf ("   image_highAddress   = 0x%zx \n",image_highAddress);

     ADDRINT image_startAddress = IMG_StartAddress(img);
     printf ("   image_startAddress  = 0x%zx \n",image_startAddress);

     USIZE image_sizeMapped     = IMG_SizeMapped(img);
     printf ("   image_sizeMapped    = %u \n",image_sizeMapped);

     bool isMainExecutable = IMG_IsMainExecutable(img);
     printf ("   isMainExecutable    = %s \n",isMainExecutable ? "true" : "false");

     IMG_TYPE image_type = IMG_Type(img);
     printf ("   section_type        = %s \n",output(image_type).c_str());

  // Output information about each section
     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
        {
       // trace << "Section " << SEC_Name(sec) << " found" << endl;

          string section_name = SEC_Name(sec);
          printf ("***section_name = %s \n",section_name.c_str());

          bool isReadable   = SEC_IsReadable(sec);
          bool isWritable   = SEC_IsWriteable(sec);
          bool isExecutable = SEC_IsExecutable(sec);
          bool isMapped     = SEC_Mapped(sec);

          printf ("      isReadable   = %s \n",isReadable   ? "true" : "false");
          printf ("      isWritable   = %s \n",isWritable   ? "true" : "false");
          printf ("      isExecutable = %s \n",isExecutable ? "true" : "false");
          printf ("      isMapped     = %s \n",isMapped     ? "true" : "false");

          SEC_TYPE section_type    = SEC_Type(sec);
          printf ("      section_type = %s \n",output(section_type).c_str());

          ADDRINT section_address  = SEC_Address(sec);
          printf ("      section_address = 0x%zx \n",section_address);

          const VOID* section_data = SEC_Data(sec);
          printf ("      section_data = %p \n",section_data);

          USIZE section_size       = SEC_Size(sec);
          printf ("      section_size = %u \n",section_size);
        }
   }


VOID Fini(INT32 code, VOID *)
   {
  // trace.close();
   }


int
main(INT32 argc, CHAR **argv)
   {
  // Optional behavior to output data in a file.
  // trace.open("dynamicBinaryInfo.out");

  // Intel Pin initialization function.
     PIN_Init(argc, argv);

  // Specify callback function when each image is loaded (main program or shared libraries).
     IMG_AddInstrumentFunction(ImageLoad, 0);

  // Specify the callback function on exit.
     PIN_AddFiniFunction(Fini, 0);

  // Never returns
     PIN_StartProgram();

     return 0;
   }
