#include<iostream>
#include<boost/algorithm/string.hpp>

using namespace std; 
using namespace boost;

int main(int argc, char*argv[]) 
   {
     string str = "hello world!"; 
     to_upper(str); 
     cout << str << endl;

     return 0;
   }
