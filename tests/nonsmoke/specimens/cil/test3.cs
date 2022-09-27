using System;

namespace Test3 {

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

        static void Main(string[] args) {
            Generic<int> g1 = new Generic<int>(1);
            Generic<char> g2 = new Generic<char>('c');
            char c = g2.foo();
            int i = g1.foo();
            Console.WriteLine(c);
            Console.WriteLine(i);
        }
    }
}
