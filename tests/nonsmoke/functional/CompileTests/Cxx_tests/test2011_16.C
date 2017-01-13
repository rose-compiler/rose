/*
On many occasions, when an iterator is declared and initialized within the for-loop itself, ROSE will not correctly
unparse the initialization. I've seen this under many circumstances; the example below can be used to reproduce the
error.

The following code compiles with g++ but not with ROSE. It seems like an unparser error, because the character "="
is simply missing from the output, producing the string "itereventQueue" instead
of "iter=eventQueue"

setIteratorBug.C
---------------------------------
#define _GLIBCXX_DEBUG
#include <set>

class Event {};

using namespace std;

void foo()
{
    set<Event> eventQueue;
    for(set<Event>::iterator iter=eventQueue.begin(); ; )
    {
    }
}
---------------------------------




ROSE Unparsed output:
---------------------------------
#define _GLIBCXX_DEBUG
#include <set>

class Event
{
}
;
using namespace std;

void foo()
{
  class std::__debug::set< Event  , std::less< Event  >  , std::allocator< Event  >  > eventQueue;
  for (std::__debug::set < Event , std::less< Event > , std::allocator< Event > > ::iterator
itereventQueue. begin (); ; ) {
  }
}
---------------------------------
*/

#define _GLIBCXX_DEBUG
#include <set>

class Event {};

using namespace std;

void foo()
   {
     set<Event> eventQueue;
     for(set<Event>::iterator iter=eventQueue.begin(); ; )
        {
        }
   }
