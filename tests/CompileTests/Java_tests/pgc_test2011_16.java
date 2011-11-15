//
// Test different kinds of function calls.
//
public class pgc_test2011_16 {
    public void test() {
        pgc_test2011_16 x = new pgc_test2011_16();

        x.field = 1;
        pgc_test2011_16.field = 2;
    }
    
    static int field = 0;
}
