using System;

namespace Test1 {
    class Test2
    {
      void GenMeth<T>() {}
    }

    class Test1 {
      public Test1() {}

//public static void PrintMethodInfo(RuntimeMethodHandle methodHandle)
//{
//    var mi = (MethodInfo) MethodBase.GetMethodFromHandle(methodHandle);
//    Console.WriteLine("Method: "+mi.ToString());
//}

      static void Main() 
      {
        var methodInfo = typeof(Test2).GetMethod("GenMeth");
      }
    }
}
