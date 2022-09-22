using System;

namespace Test1 {

    class Test1 {
      public Test1() {}

//public static void PrintMethodInfo(RuntimeMethodHandle methodHandle)
//{
//    var mi = (MethodInfo) MethodBase.GetMethodFromHandle(methodHandle);
//    Console.WriteLine("Method: "+mi.ToString());
//}

      static void Main() 
      {
        int i = 1;
        object o = i;
        int x = (int)o;
      }
    }
}
