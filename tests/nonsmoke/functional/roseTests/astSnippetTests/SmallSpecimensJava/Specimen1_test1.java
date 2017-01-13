// package Number_Handling.CWE_190;
// package NumberFormatException;

import java.lang.RuntimeException;
import java.lang.NumberFormatException;

public class Specimen1_test1 {
    static private int INSERT_HERE;

 // To force the symbol printStackTrace() to be available the target 
 // file needs to reference it, is this a reasonable constrant for 
 // the snippet mechanism.
    NumberFormatException a;
 // Throwable b;
 // java.lang.RuntimeException c;
   
    public int ipoint1() {
        int x___FOO = 1;
        int y___FOO = 2;
        int z___FOO = 3;
        INSERT_HERE=0;
        return x___FOO;
    }

    protected double ipoint3() {
        double f1 = 3.14;
        double f2 = 299792458.0;
        double f3 = f1 + f2;
        INSERT_HERE=0;

        a.printStackTrace(System.err);
     // b.printStackTrace(System.err);
     // c.printStackTrace(System.err);

        return f3;
    }

    public static void main(String args[]) {
	Specimen1_test1 specimen = new Specimen1_test1();
	specimen.ipoint1();
    }
}
