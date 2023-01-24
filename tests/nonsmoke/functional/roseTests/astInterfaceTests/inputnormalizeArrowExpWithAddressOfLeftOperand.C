int main()
{
  struct tabletype 
  {
    unsigned int item1;
  };

  unsigned int item2;

  struct tabletype table1;
  item2 = 1;

// input pattern
  (&table1) -> item1 = 5;

// output pattern
  table1.item1 = 5;

  return 0;
}

