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


