// Lambda Capture of *this by Value as [=,*this]

#include<assert.h>
#include<future>

class Work {
private:
int value ;
public:
Work() : value(42) {}
  std::future<int> spawn()
{ return std::async( [=]()->int{ return value ; }); }
};

std::future<int> foo()
{
Work tmp ;
return tmp.spawn();
// The closure associated with the returned future
// has an implicit this pointer that is invalid.
}

int main()
{
  std::future<int> f = foo();
f.wait();
// The following fails due to the
// originating class having been destroyed
assert( 42 == f.get() );
return 0 ;
}

