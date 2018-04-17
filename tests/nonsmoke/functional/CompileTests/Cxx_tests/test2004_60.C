// This test will only appear to compile, but the code will not be output because the 
// attached file information is the logical file name (in this case "foo") and not
// the physical file name (in this case "test2004_60.C").  ROSE does not currently
// handle logical vs. physical file names.

#line 999 "foo"
int main()
   {
     return 0;
   }


