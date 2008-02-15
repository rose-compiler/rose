/*statement get lost between ifdef*/
int
main (void)
{
  int rvalue = 0;
#ifdef _OPENMP
  rvalue = 1;
#endif
  return rvalue;
}
