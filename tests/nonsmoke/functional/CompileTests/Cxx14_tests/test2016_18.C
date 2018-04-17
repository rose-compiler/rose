
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
// #include <sort>

using namespace std;

template <class T> class shared_ptr;

typedef int some_type;

template<typename Func>
int LambdaTest(Func f) 
   {
     f(10);
   }


void foobar()
   {
     vector<int> v,w;

  // C++11: have to state the parameter type
     for_each( begin(v), end(v), [](const decltype(*begin(v))& x) { cout << x; } );

  // sort( begin(w), end(w), [](const shared_ptr<some_type>& a, const shared_ptr<some_type>& b) { return *a<*b; } );
  // LambdaTest( [](const shared_ptr<some_type>& a, const shared_ptr<some_type>& b) { return true; } );
     LambdaTest( [](int x) { return true; } );

     auto size = [](const unordered_map<wstring, vector<string>>& m) { return m.size(); };
   }

