//
// Test different kinds of function calls.
//
public class pgc_test2011_15 {
    public void test() {
        pgc_test2011_15 x = new pgc_test2011_15();

        after(true);
        x.after(true);
        new pgc_test2011_15().after("nothing");
        after(true, 1);
        after(true, 1, 1.0);
        after("aha!");
        System.out.println("aha");
    }
    
    public void after() {}
    public void after(String s) {}
    public void after(boolean b) {}
    public void after(boolean b, int i) {}
    public void after(boolean b, int i, double f) {}
}
