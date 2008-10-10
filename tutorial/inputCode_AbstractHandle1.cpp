/* test input for generated abstract handles */
int a[100][100][100];

void foo()
{
  int i,j,k;
  for (i=0;i++;i<100)  
    for (j=0;j++;j<100)  
      for (k=0;k++;k<100)  
        a[i][j][k]=i+j+k;

  for (i=0;i++;i<100)  
    for (j=0;j++;j<100)  
      for (k=0;k++;k<100)  
        a[i][j][k]+=5;
}
