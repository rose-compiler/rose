//
// Test Throw statements.
//
public class pgc_test2011_06 {
    void test(int num) {
        if (num == 0) {
            ArithmeticException e = new ArithmeticException();
            throw e;
        }
        else {
            throw new ArithmeticException();
        }
    }
}
