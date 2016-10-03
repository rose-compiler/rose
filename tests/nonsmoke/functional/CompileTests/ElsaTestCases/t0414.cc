// t0414.cc
// what does 3.4.4p2,3 "ignoring non-type names" mean?


struct S {
  int x;
};

namespace N {
  struct S {
    int y;
  };
  
  int S(int);    // non-type names
  
  void foo()
  {        
    // what does "ignoring ..." means if we see a non-type in the 'variables'
    // space?  two possibilities:
    //   - skip the scope, which would find ::S
    //   - first check the tag space, which would find N::S
    struct S s;                                            
    
    //ERROR(1): s.x;    // legal?
                s.y;    // legal?     icc and gcc like this one, i.e., N::S

    // so at least according to icc and gcc, you look in tags
    // before going to the next level
  }
}

namespace N2 {
  struct S {
    int y;
  };

  // same thing as above, it seems
  typedef S S;

  void foo()
  {
    // Actually, I question whether this is legal; is it not the case
    // that 'S' is a typedef-name here?  Consequently, 3.4.4p2,3 would
    // say this is ill-formed.  But both icc and gcc allow it, so Elsa
    // will too ...
    struct S s;
    //ERROR(2): s.x;
                s.y;
  }
}

namespace N3 {
  struct S {
    int y;
  };

  // invalid redeclaration
  //ERROR(3): typedef int S;
}


typedef struct S S;
void foo()
{
  struct S s;
}
