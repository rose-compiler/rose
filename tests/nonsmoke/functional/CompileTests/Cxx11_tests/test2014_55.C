// Examples of C++11 attributes

int x [[foo]];          // foo applies to variable x
void f [[foo, bar]] (); // foo and bar apply to function f

// An attribute name can be optionally qualified with a single-level attribute namespace 
// and followed by attribute arguments enclosed in parenthesis. The format of attribute 
// arguments is attribute-dependant. For example:

int y [[omp::shared]];

void foo()
   {
     int x;

     [[likely(true)]] if (x == 0)
        {
        }
   }
