/*
examples for unshared pointers  in UPC
Liao, 9/3/2008
*/
/*shared one*/
shared int gsj;

int counter;
int quux2=100;
/* ---------- unshared data (TLD)--------------------------*/
/*regular pointer */
int * pint;

/*special case: pointer to shared */
shared[4] int * p2s_p1; 
extern shared int * p2s_p3;

// DQ (9/20/2010): This line has to be compiled with the -rose:UPCxx option.
shared int * p2s_p2 = &gsj ;  /* global pointer to shared data */

/*pointer to other TLD*/
int * pcounter = &counter;
int * pcounter2 = &quux2;

/*pointer to functions */
int (*fptr) (int i);

int main()
{ 
  int * p1;         
  shared int *lp2s_p1; /* a private pointer to a shared variable, most useful */
  shared[5] int *lp2s_p12; 

  static int * lpcounter = &counter; /* TLD involves address of other TLD*/
  shared int * lp2s_p2 = &gsj ; /*local pointer to shared data */
  static shared int * lp2s_p3 = &gsj ; /*static local pointer to shared data */

/* reference to local pointer-to-shared variables*/
  lp2s_p1 = lp2s_p2;

  return 0;

}
