#include <typeinfo>

/*
When compiling the following code:

class nsString2
 {
   public:
     explicit
     nsString2(int length )
       {
       }
 };

void foo()
{
const nsString2 myString(sizeof(L"A DOM String, Just For
You")/sizeof(wchar_t));
}


I get the following error:

rose_xpctest_domstring.cpp:16: error: expected primary-expression before ) token
*/

class nsString2
 {
   public:
     explicit
     nsString2(int length )
       {
       }
 };

class nsString3
 {
   public:
     explicit
     nsString3(const std::type_info& a, const std::type_info& b)
       {
       }
 };

void foo()
{
const nsString2 myString(sizeof(L"A DOM String, Just For
You")/sizeof(wchar_t));
const nsString3 myStringX(typeid(L"A DOM String, Just For
You"), typeid(wchar_t));
}
