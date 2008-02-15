// The following functions are used as targets for the replacement mechanism

// #include<stdio.h>

#if 0
// This function should be replaced with a global variable when the "replacementTarget_GlobalScope"
// variable is replaced in the global scope with the global variable ???.
void testReplacementGlobalScope()
   {
     for (int i=0; i < 0; i++)
        {
          for (int j=0; j < 0; j++)
             {
               int replacementTarget_GlobalScope;
             }
        }
   }

// This function should be replaced with a global variable when the "replacementTarget_FileScope"
// variable is replaced in the global scope with the global variable ???.
void testReplacementFileScope()
   {
     for (int i=0; i < 0; i++)
        {
          for (int j=0; j < 0; j++)
             {
               int replacementTarget_FileScope;
             }
        }
   }

void testReplacementFunctionScope()
   {
  // This statement in the function scope should be replaced with a local variable variable when the
  // "replacementTarget_FunctionScope" variable is replaced in the function scope with the local
  // variable ???.
     for (int i=0; i < 0; i++)
        {
          for (int j=0; j < 0; j++)
             {
               int replacementTarget_FunctionScope;
             }
        }
   }


void testReplacementNestedConditionalScope()
   {
     int i,j;
     if (i == 0)
        {
       // This statement in the function scope should be replaced with a local variable variable
       // when the "replacementTarget_FunctionScope" variable is replaced in the function scope with
       // the local variable ???.
          if (j == 0)
             {
               int replacementTarget_NestedConditionalScope;
             }
        }
   }

void testReplacementNestedLoopScope()
   {
     for (int i=0; i < 0; i++)
        {
       // This statement in the function scope should be replaced with a local variable variable
       // when the "replacementTarget_FunctionScope" variable is replaced in the function scope with
       // the local variable ???.
          for (int j=0; j < 0; j++)
             {
               int replacementTarget_NestedLoopScope;
             }
        }
   }

void testReplacementParentScope()
   {
     for (int i=0; i < 0; i++)
        {
       // This statement in the parent scope should be replaced with a local variable variable
       // when the "replacementTarget_ParentScope" variable is replaced in the parent scope with
       // the local variable ???.
          for (int j=0; j < 0; j++)
             {
               int replacementTarget_ParentScope;
             }
        }
   }

void testReplacementLocalScope()
   {
     for (int i=0; i < 0; i++)
        {
          for (int j=0; j < 0; j++)
             {
            // This statement in the local scope should be replaced with a local variable variable
            // when the "replacementTarget_LocalScope" variable is replaced in the local scope with
            // the local variable ???.
               int replacementTarget_LocalScope;
             }
        }
   }

int main()
   {
     int y;
     for (int i=0; i < 0; i++)
        {
          int a;
          for (int j=0; j < 0; j++)
             {
               int b;
               int x;
             }
        }
     return 0;
   }

     int replacementTarget_LocalScope; 
     int replacementTarget_ParentScope; 
     int replacementTarget_NestedLoopScope; 
     int replacementTarget_NestedConditionalScope; 
     int replacementTarget_FunctionScope; 
     int replacementTarget_FileScope; 
     int replacementTarget_GlobalScope; 

#endif


// #if 1

int globalVariableX;

int main()
   {
     int localVariableY;
     for (int i=0; i < 1; i++)
        {
          for (int j=0; j < 1; j++)
             {
            /* comment before int x; */
               int x;
             }
        }

     return 0;
   }
// #endif

#if 0

int globalVariableX;

int main()
   {
     int localVariableY;
     for (int j=0; j < 1; j++)
        {
          int x;
        }
          
     return 0;
   }
#endif

#if 0
// Works!

int globalVariableX;

int main()
   {
     int localVariableY;
     int y;
     int x;

     return 0;
   }
#endif



#if 0
// Fails!
int globalVariableX;

int main()
   {
     int localVariableY;
        {
          int y;
          int x;
        }

     return 0;
   }
#endif

#if 0
// int globalVariableX;
int main()
   {
     int localVariableY;
      {
        int x; 
      }
      

     return 0;
   }
#endif

#if 0
int main()
   {
   int localVariableY;
      {
        int x; 
      }
      

     return 0;
   }
#endif





































