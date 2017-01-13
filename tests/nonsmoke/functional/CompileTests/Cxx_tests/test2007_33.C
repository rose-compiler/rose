// Example from Robert Preissl, demonstrates 
// error in name qualification of hidden variables.

// See test2007_39.C for an example of the exact opposent error.
// Likely the predicate in the condition is reversed!

// input:

int x = 1;

int main()
   {
     int x = 2;
     int a;
     a = ::x;
     return 0;
   }

/*
// output of identity-translator:

int x = 1;

int main()
{
  int x = 2;
  int a;
  a = x;
  return 0;
}

*/
