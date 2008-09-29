/* Based on OMP 3.0 A.10 Example A.10.1
 */
void foo()
{
  int j,k;
  int jlast, klast;
#pragma omp parallel for private(j,k), collapse(2), lastprivate (jlast, klast)
  for (k=1;k<=100;k++)
    for (j=1;j<=100;j++)
    {
      jlast = j;
      klast = k;
    } 
}
