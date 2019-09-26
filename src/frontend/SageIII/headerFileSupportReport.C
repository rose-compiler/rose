
// This header file defines a data structure to support the reporting of 
// details specific to the header file unparsing support in ROSE.

#error "THIS FILE CAN BE REMOVED!"

#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

// Associated header file for the class and member function declarations defined in this file.
#include "headerFileSupportReport.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
// using namespace Rose;

list<SgSourceFile*> &
HeaderFileSupportReport::get_headerFileList()
   {
     return headerFileList;
   }

HeaderFileSupportReport::HeaderFileSupportReport( SgSourceFile* sourceFile )
   {
  // Nothing to do here (so far).
   }


void
HeaderFileSupportReport::display( string label)
   {

     printf ("In HeaderFileSupportReport::display(): label = %s \n",label.c_str());

     list<SgSourceFile*>::iterator i = headerFileList.begin();
     while (i != headerFileList.end())
        {
          SgSourceFile* includedFile = *i;

          printf ("   --- includedFile: name = %s \n",includedFile->getFileName().c_str());
          printf ("   --- --- # of statements in global scope = %zu \n",includedFile->get_globalScope()->get_declarations().size());


        }

   }

