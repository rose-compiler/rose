namespace X {

void foo1()
   {
     void foobar();
     foobar();
   }

// void foobar();
}

void foobar();

namespace X {

// void foobar();

void foo2()
   {
     ::foobar();
//     X::foobar();
   }

}
