class Worker {
    public Worker() {
    }

    public int work(int x) {
        if (x > 10) {
            return x-1;
        } else {
            return x+1;
        }
    }
}

class Test1 {
    public static void main(String args[]) {
        int x = 5;
        int y = 10;
        int z;

        z = x + y;

        Worker w = new Worker();
        for (int i = 0; i < y; i++) {
            z = w.work(z);
            System.out.println(z);
        }
    }
}
