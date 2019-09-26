
// This works fine and the array type is properly placed into the function definition scope
static void sub1(int n, float a[n])
   {
     return;
   }

// This example appears to be inconsistant with how the array type is placed into a scope.
static void sub2(int n, float a[5][n])
   {
     return;
   }

// Example code required to run the two functions above.
int main(int argc, char **argv)
   {
     float fa[5];
     float fb[5][4];
     sub1(5, fa);
     sub2(4, fb);
   }

#if 0
Hi Dan,
I ran into a bad bug in how Rose processes variable-length arrays  (... which appears to be the cause of the problem I ran into that I mentioned to you yesterday).
Consider the following program:

  <CODE (listed above)>

The dimension "n" of the array "a" in the function "sub1" is correctly identified as the "n" in the scope of "sub1".  However, in the case of the function "sub2", the dimension "n" of the parameter array "a" is identified NOT as the parameter "n" in the function "sub2" but as some variable "n" in the global scope.

--thanks,
--philippe
#endif

