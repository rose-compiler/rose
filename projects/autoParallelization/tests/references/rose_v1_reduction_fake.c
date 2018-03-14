/*
 *  Test for automatic recognition of reduction variables
 * */
extern int bar();
int a[100];
int sum;

void foo()
{
  int i;
  int sum2;
  int xx;
  int yy;
  int zz;
  sum = 0;
  for (i = 0; i <= 99; i += 1) {
    a[i] = i;
    sum = a[i] + sum + bar();
//    sum = a[i]+ sum ;    
  }
  sum2 = sum;
  a[1] = 1;
}
