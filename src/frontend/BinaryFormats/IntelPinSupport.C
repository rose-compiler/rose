// This file includes support for handling Intel Pin data types and and
// other required code to support Intel Pin within ROSE.
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

// #include "IntelPinSupport.h"

// Later we can sue ROSE_ASSERT if appropriate.
#include <assert.h>

using namespace std;

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;


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

#endif
