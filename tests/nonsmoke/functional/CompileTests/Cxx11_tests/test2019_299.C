#include <new>

typedef unsigned long size_t;

class over
   {
     public:
          void * operator new(size_t n);
          over();
          over(int ii);
   };

void foobar()
   {
     struct c 
        {
          enum v { v2 = -2 };

       // BUG: "inline" is unparsed instead of "explicit"
       // Original code: explicit operator v() { return v2; }
       // Unparsed code: inline operator c::v() { return v2; }
          explicit operator v() { return v2; }

          operator int() { return -2; }
	};

     c c2;
     over (*p2)[2][3];
     int i = 0;

     try {
       // Original code: p2 = new over [c2][2][3];
       // Unparsed as:   p2 = (new over [(c2)][2][3]);
          p2 = new over [c2][2][3];
        } catch (std::bad_array_new_length) {
          i = 1;
        } catch (...) {
          i = 2;
        }
   }
