// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/WorkList.h>
#include <boost/random.hpp>
#include <iostream>

// Generate unique ID numbers
static size_t nextId() {
    static size_t n = 0;
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    return ++n;
}

// Define a unit of work.
struct WorkItem {
    size_t id;
    size_t level;
    size_t nChildren;

    WorkItem(size_t level, size_t nChildren)
        : id(nextId()), level(level), nChildren(nChildren) {}
};

// Define the work list. This stores a list of WorkItem objects and provides a "next" member function that returns (and
// removes) the next item of work to be performed.  For simplicity, we'll just use the FIFO defined in Sawyer.
typedef Sawyer::WorkFifo<WorkItem> WorkItems;

// Define the work that needs to be performed on each wrok item.
static void doWork(const WorkItem &item, WorkItems &pending) {
    // Simulate work
    std::cerr <<"working on " <<item.id <<"\n";
#if SAWYER_MULTI_THREADED
    static boost::random::mt19937 rng;
    static boost::random::uniform_int_distribution<> uniform(0, 1000);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(uniform(rng)));
#endif
    std::cerr <<"finished " <<item.id <<"\n";

    // Create additional work
    for (size_t i = 0; i < item.nChildren; ++i)
        pending.insert(WorkItem(item.id+1, item.nChildren-1));
}

// Define work as a functor
struct DoWork {
    void operator()(const WorkItem &item, WorkItems &pending) {
        doWork(item, pending);
    }
};

// Test using a function
static void
testWithFunction() {
    std::cerr <<"\n\nTesting with a function...\n";

    // Create an innitial item on which to work.
    WorkItems workList;
    workList.insert(WorkItem(0, 5));

    // Perform the work in parallel
    processWorkList(workList, 20, doWork);
}

// Test using a functor class
static void
testWithObject() {
    std::cerr <<"\n\nTesting with an object...\n";

    // Create an innitial item on which to work.
    WorkItems workList;
    workList.insert(WorkItem(0, 5));

    // Perform the work in parallel
    processWorkList(workList, 20, DoWork());
}

// Test using a lambda
static void
testWithLambda() {
    std::cerr <<"\n\nTesting with a lambda...\n";
#if __cplusplus < 201103L
    std::cerr <<"Not supported before C++11\n";
#else
    // Create an innitial item on which to work.
    WorkItems workList;
    workList.insert(WorkItem(0, 5));

    // Perform the work in parallel
    processWorkList(workList, 20, [](const WorkItem &item, WorkItems &pending) {
            doWork(item, pending);
        });
#endif
}

int main() {
    testWithFunction();
    testWithObject();
    testWithLambda();
}
