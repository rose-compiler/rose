using System;

namespace Test1 {
    class Test2 {}
    class Test1 {
      public Test1() {}

      static void Main() 
      {
        Object t2 = new Test2();
        Test1 t1 = t2 as Test1;
      }
    }
}
