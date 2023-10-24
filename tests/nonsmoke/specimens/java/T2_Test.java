interface T2_WorkerInterface {
    int work(int x);
}

class T2_Worker1 implements T2_WorkerInterface {
    public T2_Worker1() { }

    public int work(int x) {
        if (x > 10) {
            return x-1;
        } else {
            return x+1;
        }
    }
}

class T2_Worker2 implements T2_WorkerInterface {
    public T2_Worker2() { }

    public int work(int x) {
        return x * 42;
    }
}

class T2_Test {
    static int doWork(T2_WorkerInterface w, int i) {
        return w.work(i);
    }

    public static void main(String args[]) {
            int x = 5;
            int y = 10;
            int z;

            z = x + y;

            T2_WorkerInterface w1 = new T2_Worker1();
            T2_WorkerInterface w2 = new T2_Worker2();

            for (int i = 0; i < y; i++) {
                z = doWork(w1, z);
                z = doWork(w2, z);
                System.out.println(z);
            }
    }
}
