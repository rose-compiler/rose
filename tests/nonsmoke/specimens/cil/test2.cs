using System;

namespace Test1 {

    interface WorkerInterface {
        int work(int x);
    }

    class Worker1 : WorkerInterface {
	    public Worker1() {
	    }

	    public int work(int x) {
            if (x > 10) {
                return x-1;
            } else {
                return x+1;
            }
	    }
    }

    class Worker2 : WorkerInterface {
        public Worker2() {
        }

        public int work(int x) {
            return x * 42;
        }
    }

    class Test2 {

        static int doWork(WorkerInterface w, int i) {
            return w.work(i);
        }

        static void Main(string[] args) {
            int x = 5;
            int y = 10;
            int z;

            z = x + y;

            WorkerInterface w1 = new Worker1();
            WorkerInterface w2 = new Worker2();

            for (int i = 0; i < y; i++) {
                z = doWork(w1, z);
                z = doWork(w2, z);
                Console.WriteLine(z);
            }
        }
    }
}
