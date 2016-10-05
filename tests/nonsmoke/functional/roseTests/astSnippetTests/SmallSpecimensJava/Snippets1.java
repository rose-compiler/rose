public class Snippets1 {
    public void basic() {
        int a;
        int b = 0;
        int c=1, d=2;
        a = b + c;
        ++a;
        a += d;
    }

    public void decrement(int a) {
        --a;
    }

    public void swap(int a, int b) {
        int tmp = a;
        a = b;
        b = tmp;
    }
}
