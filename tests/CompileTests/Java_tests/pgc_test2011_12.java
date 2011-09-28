//
// Test Try statement with finally block.
//
public class pgc_test2011_12 {
    void test() {
        int i = 0;
        try {
            i = 1; // calculate
        }
        catch (ArithmeticException e1) {
            i = 0;        // I get nothing
           }
        catch (ArrayIndexOutOfBoundsException e2) { // this can't happen
            i = 1000000;  // One million dollars!
           }
        finally  {
            i = 10;       // regardless, a small allowance.
        }
    }
}
