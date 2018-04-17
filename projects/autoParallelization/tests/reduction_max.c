double a[10];

int foo ()
{
  double max_val=-1e99;
  double min_val=1e99;

  int i;

  for( i=0;i<10; i++)
  {
    if(a[i] > max_val)
    {
      max_val = a[i];   
    }

    if(a[i] < min_val)
      min_val = a[i];   
  }
}

