/*
* Various usage of shared 
* Liao 
* 6/10/2008
*/
strict shared int y;
relaxed shared[] int x;
int main()
{ 
  strict shared int *p2; /*a private pointer to a shared variable, most useful */
  relaxed shared int *p3; 
  return 0;
}
