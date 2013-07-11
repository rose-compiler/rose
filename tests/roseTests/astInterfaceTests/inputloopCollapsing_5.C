int a[11][11][11][11][11];
int main(void)
{

    int lb = 4;
    int ub = 11;
    int inc = 3;
  
  for (int i=lb;i<ub;i+=inc)
  {
     for (int j=lb;j<ub;j+=inc)
        {
     		for (int l=lb;l<ub;l+=inc)
	    	{
     		    for (int k=lb;k<ub;k+=inc)
			    {
                    for (int z=lb;z<ub;z+=inc)
			        {
		                a[i][j][l][k][z]=i+j+l+k+z;	
                    }
                }
        	}
	 }
  }
   
  return 0;
}

