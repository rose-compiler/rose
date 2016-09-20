//
// Test synchronized statement.
//
public class pgc_test2011_04 {
    void test(pgc_test2011_04 t) {
	pgc_test2011_04 t2 = null;
        synchronized(t) {
            synchronized(t) {
                t2 = new pgc_test2011_04();
            }
        }
    }
}
