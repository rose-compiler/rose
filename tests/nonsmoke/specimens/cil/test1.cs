using System;

namespace Test1 {

    class Worker {
	    public Worker() {
	    }

	    public int work(int x) {
		if (x > 10) { return x-1; } else {return x+1;}
	    }
    }

    class Test1 {

        static void Main(string[] args) {
	    int x = 5;
	    int y = 10;
	    int z;

	    z = x + y;

	    Worker w = new Worker();

	    for (int i = 0; i < y; i++) {
		z = w.work(z);
		Console.WriteLine(z);
            }
        }
    }
}
