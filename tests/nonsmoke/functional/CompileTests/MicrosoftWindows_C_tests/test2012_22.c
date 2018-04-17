
struct XYZ 
   {
     int abc;
   };

/* It is an error for the initializer to be output with the cast to (struct builtin)
// static struct XYZ bintab[] = { { 0 } };
// static struct XYZ bintab[] = { (struct XYZ){ 0 } };
*/

static struct XYZ bintab[] = { { 0 } };

