#include <commentObj.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
   {
     DoxygenEntry *c = new DoxygenEntry(argv[1]);

     c->type() = DoxygenEntry::Function;
     c->prototype() = "int main(int argc)";
     c->brief() = "Main method";
     c->description() = "Invoked by C runtime";
     c->parameter("argc") = "Number of arguments";

     cout << "\"" << c->unparse() << "\"" << endl;
   }
