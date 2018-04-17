// APP_ASSERT (c != NULL); is translated into an ugly mess
// (it generates a warning with the SUN CC compiler
// but it is not an error).  This is part of a more
// general issue, that CPP Macros are not preserved
// in the unparsed source code.

/*
Example Original code:
     APP_ASSERT (c != NULL);
Example Unparsed code:
     if (c != (const char * )0){ 0; } 
     else { APP_Assertion_Support("/home/dquinlan/A++P++/A++P++Source/A++P++/A++/src/array_src/domain.C",
     133); } 
 */

int
main()
   {
     return 0;
   }




