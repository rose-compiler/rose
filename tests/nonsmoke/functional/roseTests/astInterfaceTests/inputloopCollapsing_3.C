int a[11][11][11];
int main(void)
{

    int lb = 1;
    int ub = 10;
    int inc = 3;

  for (int i=lb;i<ub;i+=inc)
  {
     for (int j=lb;j<ub;j+=inc)
        {
     		for (int l=lb;l<ub;l+=inc)
	    	{
                int k=3;
                int z=3;
                a[i][j][l]=i+j+l+k+z;	
        	}
	 }
  }

  return 0;
}

