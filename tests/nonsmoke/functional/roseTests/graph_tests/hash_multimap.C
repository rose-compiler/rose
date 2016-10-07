#include <iostream>
// #include  <ext/hash_map>

// DQ (9/12/2009): Required for GNU g++ 4.3.3 to use strcmp
#include <string.h>

// DQ (9/12/2009): Required for GNU g++ 4.4.1 to use strcmp
#include <stdio.h>

// DQ (2/12/2016): Fix for error specific to C++11 mode being used to compile this.
#if (__cplusplus >= 201103L)
   #include <unordered_map>
#else
   #include  <ext/hash_map>
#endif

using namespace __gnu_cxx;
using namespace std;

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

// DQ (2/12/2016): Fix for error specific to C++11 mode being used to compile this.
// Should be using: unordered_multimap when compiling with C++11 mode.
// typedef hash_multimap<const char*, int, hash<const char*>, eqstr> map_type;
#if (__cplusplus >= 201103L) 
// C++11 mode
typedef unordered_multimap<const char*, int, hash<const char*>, eqstr> map_type;
#else
// Not C++11 mode (C++03 mode and older)
typedef hash_multimap<const char*, int, hash<const char*>, eqstr> map_type;
#endif

void lookup(const map_type& Map, const char* str)
{
  cout << str << ": ";
  pair<map_type::const_iterator, map_type::const_iterator> p = Map.equal_range(str);
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " ";
  cout << endl;
}

int main()
{
  map_type M;

  M.insert(map_type::value_type("H", 1));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("H", 2));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("C", 12));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("C", 13));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("O", 16));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("O", 17));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("O", 18));
  printf ("M.size() = %zu \n",M.size());
  M.insert(map_type::value_type("I", 127));
  printf ("M.size() = %zu \n",M.size());

  cout << "Output all of Multimap M:" << endl;
  for (map_type::const_iterator i = M.begin(); i != M.end(); ++i)
     cout << i->first << " : " << (*i).second << endl;

  cout << "Output using lookup function with Multimap M:" << endl;
  lookup(M, "I");
  lookup(M, "O");
  lookup(M, "Rn");
}
