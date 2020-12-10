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


#if defined(__EDG_VERSION__) && __EDG_VERSION__ >= 61
// #error "EDG_VERSION >= 61"
     [[likely]] if (x == 0)
#else
// #error "EDG_VERSION < 61"
     [[likely(true)]] if (x == 0)
#endif
        {
        }
   }
