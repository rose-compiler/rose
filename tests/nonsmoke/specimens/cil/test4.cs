using System;

namespace Test4 {

    class Test1 {

        static void Main(string[] args) {
            int x = -43;
            int y = 42;
            int z;

            z = x + y;

            for (int i = 0; i < 10; i++) {
                switch (z) {
                    case 0:
                        Console.WriteLine("case1");
                        break;
                    case 3:
                        Console.WriteLine("case2");
                        break;
                    case -1:
                        Console.WriteLine("case3");
                        break;
                    default:
                        Console.WriteLine("default");
                        break;
                }
                z++;
            }

        }
    }
}
