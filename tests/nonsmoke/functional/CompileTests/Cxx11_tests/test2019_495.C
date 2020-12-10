// This issue corresponds to the assertion failing at line 2514 of the AstConsistancyTests.C file.
// This is tied to be the 3rd most numerous issue in testing (causing failure in 3 files).

bool ba_foo();

bool ba_foo()
   {
     42;
     return false;
   }

