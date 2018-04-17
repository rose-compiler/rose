// This is a bug from the test/testCodeGeneration translator runing on ROSE source code file: FileNameClassifier.C
// This file was rewritten to be more oranized and uniform with the rest of the ROSE source.
// The the new code does not have the current problem, so this test code captures the problem 
// with the older version of the source file.

// All of the following code appears to be required to reproduce this error.

#include "string_functions.h"

#include "boost/filesystem.hpp"

using namespace std;

namespace StringUtility
   {
//   namespace {
          void foo()
             {
            // DQ (2/8/2010): This is an error for ROSE
               using namespace boost::filesystem;
             }
//      }
   }
