int main() {
  int x=1;
  for(int i=1; i<10;i++)
    for(int j=i;j<10;j++)
      for(int k=i;k<10;k++)
	for(int l=i;l<10;l++)
	  for(int m=i;m<10;m++)
	    x++;

  int i=5,j=7;
  while(i>0) {
    while(j>0) {
      x++;
      j--;
      i--;
    }
  }

  i=10;
  do { 
    x++;
    i--; 
  } while (i>0);

  return x;
}
