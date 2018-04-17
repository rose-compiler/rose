//
// Test Labeled statements.
//
public class pgc_test2011_05 {
    void test(int num) {
        l: num =0;
a:b:c:xyz: num=2;
xyz:a:b:c: num=3;
        l: switch (num) {
              case 0:
                  break;
              case 1:
                  break;
              case 2:
                  break;
           }
    }
}
