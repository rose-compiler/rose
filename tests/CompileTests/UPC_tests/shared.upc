/*
* Various usage of shared 
* Liao 
* 6/9/2008
*/
int main()
{ 
  int * p1;         //a private pointer to a private variable
  shared int *p2; //a private pointer to a shared variable, most useful
  shared[] int *p22; //a private pointer to a shared variable, most useful
  int *shared p3; //shared to private
  shared int* shared p4; //shared to shared
  shared[10] int* shared p44; //shared to shared
  return 0;
}
