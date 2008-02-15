// Bill's Example of a file which the preprocessor can't transform (reports that A++simple.h can't
// be found within rose_transformation.C when processing the BASIC_BLOCK transformation).

#include <A++.h>

int 
main()
{

  cout << "====== Test of A++ =====" << endl;

  Index::setBoundsCheck(on);  //  Turn on A++ array bounds checking

  int n=10;
  floatArray a(n), b(n);
  
  b=1.;
  a=1.;
  
  Index I(1,n-2);
  a(I)=b(I+1)+b(I-1);
  
  a.display("a");

  return 0;
}

