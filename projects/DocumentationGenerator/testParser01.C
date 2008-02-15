#include <doxygenComment.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv) 
   {
     DxNodeList *l = parseDoxygenComment(argv[1]);
     int count = 0;
     for (DxNodeList::iterator i = l->begin(); i != l->end(); ++i) 
        {
          cout << count << ". \"" << (*i)->unparse() << "\"" << endl;
          count++;
        }
   }
