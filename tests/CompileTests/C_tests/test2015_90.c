/* Test code used in discussion of ROSE issues. */

int test()
{
 int x=1;
 int i=1;
 for (i=1;i<10;i=i+({x=1;}))
 {
  if (1)
  {
   break;
  }
 }
 return 1;
}

int main(int argc, char*argv[])
{
 return 0;
}
