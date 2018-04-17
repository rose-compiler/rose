// This test the string concatination with a macro defined from the command line 
// as: -DSTRING_MACRO="some_text" 
// Note that the string appears to be required to not have any spaces....
// This will have to be fixed later.

void foobar()
   {
     char* s = STRING_MACRO " plus more text";
   }
