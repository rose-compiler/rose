
//Your test file code goes here.

void foo(){
  int i;
  int j = 0;
  int k = 0;

  for(i = 0; i != 10; ++i)
  {
    if( 0 == i % 3)
    {
      i = 3;
      ++i;
      i++;
    }
  }

  while(j++ < 10)
  {
    if(1 == j %3)
    {
      j = j + 2;
    }
    j++;
  }

  do {
    if(2 == k % 3)
    {
      k +=1;
    }
  } while(++k < 10);
}

