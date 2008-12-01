/* Pointers as array
 * */
void ser(int *a, int *b, int *c)
{
  int i;
  for (i=0; i<9900; i++)
    a[i] = a[i] + b[i] * c[i];
}
