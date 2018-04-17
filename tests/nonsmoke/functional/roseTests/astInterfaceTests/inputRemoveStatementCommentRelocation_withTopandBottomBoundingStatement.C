int abc;

// DQ (9/16/2010): Added this test code to demonstrate problem with removing 
// statments with comments or CPP directives attached.

// This header file declares a variable that will be used by the inserted code.
// By design, if the function it is attached to removes the #include then the
// variable declaration below it will fail to compile and will cause an error.
#include<inputRemoveStatementCommentRelocation_1.h>

// Use this function to test removal, if it cause removal of the #include then
// there will be an error (since variable_hidden_in_header_file will not be defined).
int removeThisFunctionToTestAttachedInfoBeforeStatement()
   {
     return 0;
   }

int removeThisFunctionToTestAttachedInfoBeforeStatement_var = variable_hidden_in_header_file;

// Use this function to test removing statements to avoid an error in the next
// statement (which reuses the function name as a variable name).  If the function
// is not removed then the variable declaration will be legal code.  If the
// #include directive is removed then the variable declaration after the #include
// will not be a legal declaration.
int removeThisFunctionToTestAttachedInfoAfterStatement()
   {
     return 0;
   }

// Cause generation of: "int y = InsertStatementAfterThisFunction();" which is only 
// legal after after declaration of InsertStatementAfterThisFunction().  The header 
// file below will require this variable to exist when compiled using the backend compiler.
#include<inputRemoveStatementCommentRelocation_2.h>

int tempVar = variable_hidden_in_header_file_2;

int xyz;

