// even_functor.cpp
// compile with: /EHsc
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

class Functor
{
public:
   // The constructor.
   explicit Functor(int& evenCount) 
      : _evenCount(evenCount)
   {
   }

   // The function-call operator prints whether the number is
   // even or odd. If the number is even, this method updates
   // the counter.
   void operator()(int n)
   {
      cout << n;

      if (n % 2 == 0) 
      {
         cout << " is even " << endl;

         // Increment the counter.
         _evenCount++;
      }
      else 
      {
         cout << " is odd " << endl;
      }
   }

private:
   int& _evenCount; // the number of even variables in the vector
};


int main() 
{
   // Create a vector object that contains 10 elements.
   vector<int> v;
   for (int i = 0; i < 10; ++i) 
   {
      v.push_back(i);
   }

   // Count the number of even numbers in the vector by 
   // using the for_each function and a function object.
   int evenCount = 0;
   for_each(v.begin(), v.end(), Functor(evenCount));

   // Print the count of even numbers to the console.
   cout << "There are " << evenCount 
        << " even numbers in the vector." << endl;
}

