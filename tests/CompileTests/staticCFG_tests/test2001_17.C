
/*
Original code:
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
Unparsed code:
     enum Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = ;
 */

#define UNINITIALIZED_VARIABLE 10 
#define NEWLY_ALLOCATED_MEMORY 11
#define MEMORY_FROM_LHS        12
#define MEMORY_FROM_RHS        13
#define MEMORY_FROM_OPERAND    14

enum Memory_Source_Type { Uninitialized_Source   = UNINITIALIZED_VARIABLE , 
                          Newly_Allocated_Memory = NEWLY_ALLOCATED_MEMORY , 
                          Memory_From_Lhs        = MEMORY_FROM_LHS , 
                          Memory_From_Rhs        = MEMORY_FROM_RHS ,
                          Memory_From_Operand    = MEMORY_FROM_OPERAND };

int
main ()
   {
  // DQ (8/15/2006): Note that C++ does not require the "enum" qualification, but C does!
#if __cplusplus
     Memory_Source_Type X = Uninitialized_Source;
#else
     enum Memory_Source_Type X = Uninitialized_Source;
#endif
     return 0;
   }

