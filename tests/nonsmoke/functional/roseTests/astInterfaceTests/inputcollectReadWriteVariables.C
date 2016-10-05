int a[100][100];
int i,k=1;

int main(int argc, char** argv)
{
  int b[100][100]={99};
  for (i=0;i<100;i++)
    for (int j=0; j<100;j++)
      a[i][j]=b[i][j]+k;
  if (argc>0)
  {
    i+=100;
    if (argv[0]=="-rose")
      i++;
  }
  return 0;
}

