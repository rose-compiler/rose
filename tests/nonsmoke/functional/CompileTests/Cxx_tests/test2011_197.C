namespace N
{
class X
   {
   };
}

using namespace N;

template <typename T>
class X
   {
   };

void foo()
   {
     X abc;
     X<int> xyz;
   }


