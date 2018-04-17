// DQ (9/16/2010): Added this test code to demonstrate problem with adding 
// statments around statments with comments or CPP directives attached.

// This header file declares a variable that will be used by the inserted code.
// By design, if the inserted code is input into the wronge location then it 
// will cause an error.
#include<inputinsertStatementBeforeFunction_1.h>

// Use this function to test inserting something before a (function) declaration.
// Cause gneration of "x = variable_hidden_in_header_file;" which is only legal after the #include directory.
int InsertStatementBeforeThisFunction()
   {
     return 0;
   }

// Use this function to test inserting something after a (function) declaration.
int InsertStatementAfterThisFunction()
   {
     return 0;
   }

// Cause generation of: "int y = InsertStatementAfterThisFunction();" which is only 
// legal after after declaration of InsertStatementAfterThisFunction().  The header 
// file below will require this variable to exist when compiled using the backend compiler.
#include<inputinsertStatementBeforeFunction_2.h>
