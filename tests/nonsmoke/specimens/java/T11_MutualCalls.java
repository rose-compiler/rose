class T11_WorkerA {
    int value = 0;
    public int doWorkA(T11_WorkerB other) {
        if (value < 10) {
            value++;
            return other.doWorkB(this);
        } else {
            return value;
        }
    }
}

class T11_WorkerB {
    int value = 0;
    public int doWorkB(T11_WorkerA other) {
        if (value < 10) {
            value++;
            return other.doWorkA(this);
        } else {
            return value;
        }
    }
}

class T11_MutualCalls {
    public static void main(String args[]) {
        T11_WorkerA a = new T11_WorkerA();
        T11_WorkerB b = new T11_WorkerB();

        System.out.println(a.doWorkA(b));
        System.out.println(b.doWorkB(a));
    }
}
