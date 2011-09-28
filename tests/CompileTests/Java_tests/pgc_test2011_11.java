//
// Test Try statement with finally block.
//
public class pgc_test2011_11 {
    void test() {
        int i = 0;
        try {
            i = 1; // calculate
        }
        catch (ArithmeticException e1) {
            throw e1;
        }
        catch (ArrayIndexOutOfBoundsException e2) { // this can't happen
            throw e2;
        }
        finally  {
            i = 10;       // regardless, a small allowance.
        }
    }
}
