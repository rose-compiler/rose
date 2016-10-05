// in/t0363.cc
// some uses of implicit 'this' to check for elaboration

// for now I just inspect the output manually; in the future
// I'd like to add a post-tcheck walk that checks to be sure
// that this-> is used in the proper places

            struct S {
              int x;
              static int y;

              int foo();

              static int bar();

              // overloaded, nonstatic first
              int baz(int);
              static int baz(int,int);

              // overloaded, static first
              static int zoo(int,int);
              int zoo(int);
            };

            int S::foo()
            {
              int ret = 0;
              int (S::*ptm);
              int *p;

              ret += x;            // this->x
              ret += S::x;         // this->x
              ret += this->x;

              ptm = &S::x;         // S::x
              p = &(S::x);         // this->x

              ret += y;            // S::x
              ret += this->y;      // legal, but redundant

              ret += baz(1);       // this->baz
              ret += (baz)(1);     // this->baz
              ret += ((baz))(1);   // this->baz
              ret += baz(1,2);     // S::baz
              ret += (baz)(1,2);   // S::baz
              ret += ((baz))(1,2); // S::baz

              ret += zoo(1);       // this->baz
              ret += zoo(1,2);     // S::zoo

              return ret;
            }

            /*static*/ int S::bar()
            {
              int ret = 0;

//ERROR(1):   ret += x;
//ERROR(2):   ret += this->x;

              ret += y;            // S::x
//ERROR(3):   ret += this->y;

//ERROR(4):   ret += baz(1);
              ret += baz(1,2);     // S::baz

//ERROR(5):   ret += zoo(1);
              ret += zoo(1,2);     // S::zoo

              return ret;
            }

// EOF
