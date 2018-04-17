
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>

using namespace std;

template <class T> class shared_ptr;

typedef int some_type;

#if 0
template<typename Func>
int LambdaTest(Func f) 
   {
     f(10,11);
   }
#else
template<typename T1, typename Func>
int LambdaTest(T1 x, T1 y, Func f) 
   {
     f(x,y);
   }
#endif

void foobar()
   {
     vector<int> v,w;

  // C++11: have to state the parameter type
     for_each( begin(v), end(v), [](const decltype(*begin(v))& x) { cout << x; } );

  // sort( begin(w), end(w), [](const shared_ptr<some_type>& a, const shared_ptr<some_type>& b) { return *a<*b; } );
  // LambdaTest( [](const shared_ptr<some_type>& a, const shared_ptr<some_type>& b) { return true; } );
  // LambdaTest( [](int x) { return true; } );

     auto size = [](const unordered_map<wstring, vector<string>>& m) { return m.size(); };

  // C++14: just deduce the type
     for_each( begin(v), end(v), [](const auto& x) { cout << x; } );
  // sort( begin(w), end(w), [](const auto& a, const auto& b) { return true; } );
     LambdaTest( 1, 2, [](const auto& a, const auto& b) { return a+b; } );
   }

