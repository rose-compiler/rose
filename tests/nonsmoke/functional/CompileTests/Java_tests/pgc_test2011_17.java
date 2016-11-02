//
// Test class with super class and interfaces.
//
interface I {
    void test();
}

interface I2 {
    int array_rows = 10;
}

class Ok implements I2 {
    String str = "abc";
}

public class pgc_test2011_17 extends Ok implements I {
    static public void main(String args[]) {
        pgc_test2011_17 t = new pgc_test2011_17();
    }
    public void test() {}
}