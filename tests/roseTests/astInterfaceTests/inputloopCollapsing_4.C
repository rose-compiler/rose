int a[11][11][11][11];
int main(void)
{

    int lb1 = 10, lb2 = 1;
    int ub1 = 0, ub2 = 10;
    int inc1 = -4, inc2 = 3;
  
  for (int i=lb1;i>ub1;i+=inc1)
  {
     for (int j=lb1;j>ub1;j+=inc1)
        {
     		for (int l=lb2;l<ub2;l+=inc2)
	    	{
                for (int z=lb1;z>ub1;z+=inc1)
		        {
                    int k=3;
		                a[i][j][l][z]=i+j+l+k+z;	
                }
        	}
	 }
  }
  return 0;
}

