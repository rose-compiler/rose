#if 0
The following code
compiled with ROSE gives the following error:
rose_roseScopingError.cpp: In function `int main()':
rose_roseScopingError.cpp:25: error: parse error before `::' token

The output from ROSE is:
#include <iostream>

struct super
{
  virtual int operator[](char *) = 0;
}

;

struct dummy : public super
{


  virtual inline int operator[](char *lala)
{
    return 0;
  }

}

;

int main()
{
  dummy::dummy()dummy::operator[](((char *)"Kuh"));
}

I guess the problem lies in the unparser, where it does not output the
correct code because of the way the object is constructed. This is some
funky code.  :) 

Thanks
Jochen and Andreas
#endif



// #include <iostream>
struct super
   {
     virtual int operator [] (char*) = 0;
   };

struct dummy : public super
   {
     virtual int operator [] (char* lala) {return 0;}
   };

int main()
   {
  // dummy()["Kuh"];
     dummy::dummy()["Kuh"];
   }
