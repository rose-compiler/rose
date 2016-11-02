//
// Test import-on-demand and QualifiedNameReference in Type specification.
//
import java.io.PrintStream;

class pgc_test2011_19 {
    pgc_test2011_19(int i, double d) {
	try {
            test();
	}
	catch(ArrayIndexOutOfBoundsException e) {
            java.io.PrintStream out = System.out;
            System.out.println("An Exception Was Thrown !!!");
            out.println("Yes, I can confirm that !");
        }
    }

    void test() {
        throw new ArrayIndexOutOfBoundsException();
    }

    static public void main(String args[]) {
        pgc_test2011_19 t = new pgc_test2011_19(0, 0.0);
    }
}