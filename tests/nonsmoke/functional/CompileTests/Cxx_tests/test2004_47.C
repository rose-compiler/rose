// (7/7/2004): Bug submitted by Andreas (processing DiffPack)

#include <stdlib.h>

/* This problem occurs when the operator-> is defined.
   Because of precedence -> == *ptr.
       ptr->someMethod()
   if ptr is a pointer.
   If you have a reference instead
   -> == operator-> if operator-> is defined.
      ref->someMethod() 
*/


class Test_base
{
public:
int getFormat(){return 1;};
};

class Test

{
private:
 Test_base* ptr;
public:
 Test()
 {
    ptr = new Test_base();
 };

 Test_base* operator -> ();
};


Test_base* Test:: operator -> ()
{

  if (ptr == 0) 
      exit(1); 

  return ptr;
}




int main()
{
   Test ref ;
   ref->getFormat();

   return 0;

}

