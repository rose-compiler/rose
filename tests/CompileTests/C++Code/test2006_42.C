#include "Bar.H"

//Check that we cover empty constructor block
Bar::Bar() {} /***Cov bb*/ /***Cov me*/

int Bar::moo() { /***Cov bb*/ /***Cov me*/
   try { /***Cov bb*/
      cout<<"Bar::moo()"<<endl;
      return 1;
   } catch(...) { /***Cov bb ~ */
      return 2;
   }
}

extern "C"
void bar() { /***Cov bb*/ /***Cov me*/
   Bar b;
   b.bar();
   b.moo();
}
