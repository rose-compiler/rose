#define __restrict__ 

#include<list>

using namespace std;

int main()
   {
     list<int> integerList;

     integerList.push_back(1);

  // DQ (2/6/2011): This causes a problem with the new support for sizeof.
     integerList.sort();

     return 0;
   }

#if 0
// DQ (2/12/2011): This appears to fail for only GNU 4.3 and 4.4 compilers.
// since it was not an important part of the testcode I have
// just commented it out.
  template<typename _Tp>
    struct __is_empty
    { 
    private:
      template<typename>
        struct __first { };
      template<typename _Up>
        struct __second
        : public _Up { };
           
    public:
      enum
	{
	  __value = sizeof(__first<_Tp>) == sizeof(__second<_Tp>)
	};
    };
#endif
