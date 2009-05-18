/* A test case for search order for include paths 
 * The current path for the .c file should be searched first
 *
 * Liao, 5/15/2009
 * 
 */
#include "test2009_515.h"
int bar(int i)
{
  return i;
 }
int main()
{
  int r = Aaa+ bar(1);
  return r;
}
