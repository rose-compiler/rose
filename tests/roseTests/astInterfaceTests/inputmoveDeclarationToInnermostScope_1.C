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
    // use with two defs
    {
      j=0  ;
    }

    {
      j=2  ;
    }
  }

  return 0;
}
