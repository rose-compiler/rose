class T3_Generic<T> {
    private T x;

    public T3_Generic(T t) {
        x = t;
    }

    public T foo() {
        return x;
    }
}

class T3_Test {
    public static void main(String args[]) {
        // note: need to use Integer and Character vs int and char.
        T3_Generic<Integer> g1 = new T3_Generic<Integer>(1);
        T3_Generic<Character> g2 = new T3_Generic<Character>('c');
        char c = g2.foo();
        int i = g1.foo();
        System.out.println(c);
        System.out.println(i);
    }
}
