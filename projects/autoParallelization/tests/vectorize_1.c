/* Vectorization example
 * */
float a[256], b[256], c[256];
int i;
foo ()
{
  for (i = 0; i < 256; i++)
    a[i] = b[i] + c[i];
}
