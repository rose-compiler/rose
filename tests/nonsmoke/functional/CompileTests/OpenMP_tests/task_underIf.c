/* test pragma under the true body of if statement 
 *  two cases: inside {} or directly attached to true/false body
 *
 *  Liao, 10/1/2008
 * */
extern void process(int);
extern void process2(int);
int item[100];
int cutoff = 100;
void foo(int i)
{
/*pragma needs scope fixes */
  if (i%2==0)
#pragma omp task if (i < cutoff)
    process (item[i]);
  else
#pragma omp task
    process2(item[i]);

/*pragma within explicit scopes */
  if (i%2==0)
  {
#pragma omp task
    process (item[i]);
  }
  else
  {
#pragma omp task
    process2(item[i]);
  }
}


