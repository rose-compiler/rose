// explicit_specialization1.cpp
// compile with: /EHsc
#include <iostream>
using namespace std;

// Template class declaration and definition
template <class T> class Formatter
{
   T* m_t;
public:
   Formatter(T* t) : m_t(t) { }
   void print()
   {
      cout << *m_t << endl;
   }
};

// Specialization of template class for type char*
template<> class Formatter<char*>
{
   char** m_t;
public:
   Formatter(char** t) : m_t(t) { }
   void print()
   {
      cout << "Char value: " << **m_t << endl;
   }
};

int main()
{
   int i = 157;
   // Use the generic template with int as the argument.
   Formatter<int>* formatter1 = new Formatter<int>(&i);

   char str[10] = "string1";
   char* str1 = str;
   // Use the specialized template.
   Formatter<char*>* formatter2 = new Formatter<char*>(&str1);

   formatter1->print();
   formatter2->print();
}


