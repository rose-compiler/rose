int main()
{
  int i, n, a, b, c, d;
#pragma scop
  for (i = 0; i < n; ++i)
    {
      if (i > 1)
	a = 0;
      b = 0;
      if (i > 2)
	{
	  if (i < n - 1)
	    c = 0;
	  d = 0;
	}
    }
#pragma endscop
}
