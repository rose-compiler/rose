
// This is not required since I have added it to ROSE directly
// The whole issue of how to support __restrict__ needs to be
// visited since I can't get EDG to recognize it (though it says
// it is supported since 8/2002 and I have turned on all the required
// support (I think).
// #define __restrict__ 

#include<list>
#include<vector>

using namespace std;

class A
   {
     public:
          void foo();
   };

int main()
   {
  // DQ (11/19/2004): Temporarily commented out since this is a demonstrated bug now that we qualify everything!
     std::vector<int> integerVector;

     std::vector<int>::iterator i = integerVector.begin();

     bool b = i != integerVector.end();

     integerVector.push_back(1);
     int sumOverList = 0;
     for (i = integerVector.begin(); i != integerVector.end(); i++)
        {
          sumOverList += *i;
        }

     for (vector<int>::iterator i = integerVector.begin(); i != integerVector.end(); i++)
        {
          sumOverList += *i;
        }

     std::vector<A> vectorA;
     for (vector<A>::iterator i = vectorA.begin(); i != vectorA.end(); i++)
        {
       // This is similar to the statements below, but using different (overloaded) operators
          (*i).foo();

       // These are the same statements but using different syntax (short and long operator forms)
          i->foo();
          i.operator->()->foo();
          (*i.operator->()).foo();
        }

     return 0;
   }


