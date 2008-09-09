/*-----------global shared data (SSD)-------------------------------*/
/*  shared scalar */
shared int global_counter; 
shared int global_counter1 = 0; 
shared int global_counter2 = 2; 

/* shared arrays */
shared[5] double garray[100*THREADS];
/* Berkeley UPC compiler does not yet fully implement this: their bug 36 
shared[5] double garray2[10*THREADS]={1.1, 2.2};
*/

/* shared pointers */
shared int* shared[10] s2s_p4; /*shared to shared */
shared[10] int* shared s2s_p44; 
shared[5] int* shared[8] s2s_p444; 

int *shared s2p_p3; /*shared to private */
int *shared[5] s2p_p33; /*shared to private */

int foo()
{
/* -------local shared static data -----------*/
  static shared int lsscounter; /* static shared scalar */
  static shared int lsscounter1 =0; /* static shared scalar with initializer */
  static shared int lsscounter2 =77; /* static shared scalar with initializer */

  /*static shared array */
  static shared int lssfooArray3[5*THREADS];
 /* The translation is not implemented by the Berkeley UPC
  static shared int lssfooArray5[5*THREADS] = {1,2,3,4,5}; 
  */

/*Write reference to a shared variable */
  lsscounter=99;
  return 0;
}
int main()
{ 
 return 0;
}
