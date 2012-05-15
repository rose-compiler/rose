//
// Test interfaces, super classes, etc...
//
import java.io.*;

interface I {
    void test(String s);
}

interface I2 {
    int array_rows = 10;
}

class Ok implements I2 {
    public Ok(String s) {
    	this.str = s;
    }

    String str;
}

public class pgc_test2011_21 extends Ok implements I {
    pgc_test2011_21() {
        super("what?");
    }

    static public void main(String args[]) {
        pgc_test2011_21 t = new pgc_test2011_21();
        t.test("Say ");
    }

    int x = 12;

    public void test(String prefix) {
        int num = this.x;
        int num2 = super.array_rows;
        int num3 = array_rows;
        int num4 = num;
        System.out.println(prefix + str);
        System.out.println("array_rows = " + super.array_rows);
        p.println("num = " + num);
        p.println("num2 = " + num2);
        p.println("num3 = " + num3);
        p.println("num4 = " + num4);
    }

    PrintStream p = java.lang.System.out;
}