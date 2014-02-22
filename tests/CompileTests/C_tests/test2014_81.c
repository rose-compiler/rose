// The only ROSE bug required to be fixed to support processing of 2.5 million line Wireshark appliction.
// The issue was demonstrated twice in the wireshark appliction.

// This seconedary declaration must come before the declaration of the array bound and the array bound is an 
// enum declared after the secondary declaration.
// The bug in the ROSE failed for the case where the constant was an enum value cast to a unsigned long integer.
// Enums had up to this point in all applications seen by ROSE been used as enum types constants and not cast to 
// integers. Note that the cast is required because the type of the constant bound for an array is required to  
// be an unsigned long integer for C/C++.

extern int array[];  // The assertion fails in ROSE at the processing of this secondary declaration.
extern int array[2];  // The assertion fails in ROSE at the processing of this secondary declaration.

// The array bound must be declared an an enum value after the secondary array declaration.
enum EnumType
   {
     enum_value_0, // default value will be 0
     enum_value_1, // default value will be 1
     enum_value_2, // default value will be 2
   };

// Then we must declare the array with a primary declaration (initializer is not required to demonstrate the bug).
// int array[enum_value_2] = { 1, 2 };
int array[enum_value_2];

