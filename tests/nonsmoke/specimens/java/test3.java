class Generic<T> {
    private T x;

    public Generic(T t) {
        x = t;
    }

    public T foo() {
        return x;
    }
}

class Test3 {
    public static void main(String args[]) {
        // note: need to use Integer and Character vs int and char.
        Generic<Integer> g1 = new Generic<Integer>(1);
        Generic<Character> g2 = new Generic<Character>('c');
        char c = g2.foo();
        int i = g1.foo();
        System.out.println(c);
        System.out.println(i);
    }
}
