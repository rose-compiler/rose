/*
* Various usage of shared 
* Liao 
* 6/10/2008
*/
shared[*] int p4[100*THREADS]; /* shared to shared */
shared[] int* shared p44; /* shared to shared */
int main()
{ 
  shared int *p2; 
  shared[] int *p22;
/* shared[*] int *p222; */ /*Illegal, layout qualifier [*] shall not appear for a pointer type*/
/* int *shared p3;*/ /*  shared variables must be static or extern */
  return 0;
}
