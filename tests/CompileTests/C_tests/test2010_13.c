/*
 * From spec_cpu2006's 403.gcc
 * Liao 11/11/2010
 * */
#define HARD_REG_SET int

void foo()
{
#ifdef HARD_REG_SET
  register              /* Declare it register if it's a scalar.  */
#endif  
    HARD_REG_SET used;
  int i, 
//test this  
  j;
}
