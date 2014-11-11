int main()
{
  int i,j, k;
  k=0; // k is used in the same scope in which it is declared
  // simplest use-def chain
  {
    {
      {
        i=0  ;
      }
    }
  }

  {
    // use with multiple defs
    {
      j=0  ;
      { // deeper scope, should be trimmed
        j =0;
      }
    }

    {
      {
        {
         j = 0;// initially built into scope tree, later trimmed. 
        }
      }
      j=2  ; //this should trigger trimming the previous path 
    }
  }

  return 0;
}
