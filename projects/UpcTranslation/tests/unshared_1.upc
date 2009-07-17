/*
examples for unshared data in UPC
Liao, 9/3/2008
*/
/*shared one*/
shared int gsj;

/* ---------- unshared data (TLD)--------------------------*/
/*Unshared global variables , with extern */
 extern int quux;

/*unshared global variables: scalar */
int counter;
int counter1 = 0;
int counter2 = 100;

/*unshared global arrays, w or w/o initializer */
double myarray[10];
double myarray2[5]={0.0, 1.1, 2.2,3.3,4.4};

/* structure */
struct mystruct
	{
	char name[50];
	float calibre;
	};
struct mystruct ms1;

/* union , with embedded declaration*/
union item_u
{
  int i;
  float f;
  char c;
} item;

int foo()
{
/* -------unshared static data -----------*/
  static int lscounter; /* static scalar */
  static int lscounter1 =0; /* static scalar with initializer */
  static int lscounter2 =77; /* static scalar with initializer */

  static double lsfooArray [2]; /* static array */
  static double lsifooArray2 [2] = {3.1, 1.3}; /* static array */
  
/*reference to local static unshared variable*/  
  lscounter =99;
  return 0;
}
int bar()
{
  return 0;
 }
int main()
{ 
/* references to global unshared variables*/
  counter ++;

  item.i = 9;
  ms1.calibre = 0.7;
  return 0;
}
