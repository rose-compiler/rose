/*
-rose:C_only does not work!
*/

int main(void)
{
int i,j;
#pragma omp for
 for (i=0;i<10;i++) j=0;

return 0;
}
