//
// Test Switch statement with default case at the beginning.
//
public class pgc_test2011_01 {
    void test(int num) {
    int i;
    switch (num) {
        default:
            i = 10;
            break;
        case 0:
            i = 0;
            break;
        case 1:
            i = 1;            
            break;
        case 2:
            i = 2;
            break;
        }
    }
}
