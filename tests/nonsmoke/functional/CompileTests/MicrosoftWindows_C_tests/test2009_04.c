/* Liao, 5/13/2009
 C allows implicit declaration of functions
 */
int foo()
{
/* MSVC does not allow implicit functions: GNU specific */
/*  bar(); */
  return 0;
}
