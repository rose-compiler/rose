class T1_Worker {
    public T1_Worker() {
    }

    public int work(int x) {
        if (x > 10) {
            return x-1;
        } else {
            return x+1;
        }
    }
}

class T1_Test {
    public static void main(String args[]) {
        int x = 5;
        int y = 10;
        int z;

        z = x + y;

        T1_Worker w = new T1_Worker();
        for (int i = 0; i < y; i++) {
            z = w.work(z);
            System.out.println(z);
        }
    }
}
