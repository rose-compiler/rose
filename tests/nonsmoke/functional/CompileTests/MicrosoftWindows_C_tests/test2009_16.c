/*Liao, 5/16/2009
 * A test case coined from spec_cpu2006_v1.1's bzip2
 *
 * Macro definition ending with \
 *
 * ROSE had problem to keep consecutive '\n'
 * and caused tricky runtime errors.
 * */
int main()
{
#define BZ_ITER(nn) \
  int nn;\

  BZ_ITER(i);
  i=10;

}
