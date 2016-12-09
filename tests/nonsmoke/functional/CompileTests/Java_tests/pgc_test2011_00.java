//
// Test a Switch statement with a default block at the end.
//
public class pgc_test2011_00 {
    public void test(int num) {
        int i;
        switch(num) {
            case 0:
                i = 0;
                break;
            case 1:
                i = 1;
                break;
            case 2:
                i = 2;
                break;
            default:
                i = 10;
                break;
        }
    }
}
