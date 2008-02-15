#include <commentObj.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv) 
   {
     DoxygenEntry *c = new DoxygenEntry(argv[1]);
     cout << "\"" << c->unparse() << "\"" << endl;
   }

