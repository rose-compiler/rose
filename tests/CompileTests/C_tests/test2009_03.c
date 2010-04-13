//Liao, 5/13/2009
//Extracted from setup.c of spec_cpu2006's 433.milc

//  int initial_set();
 void foo();
int setup ()
{
  //forward declaration of a function
  void foo();
  int initial_set();
  return 0;
}
void foo(){}
// defining declaration of a function
int initial_set()
{
  return 0;
}
