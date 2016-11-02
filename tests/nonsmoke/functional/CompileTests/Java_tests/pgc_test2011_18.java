//
// Test different kinds of name resolutions involving field references.
//
interface I {
    void test();
}

interface I2 {
    int array_rows = 10;
}

class Ok implements I2 {
    String str = "abc";
}

public class pgc_test2011_18 extends Ok implements I {
    static public void main(String args[]) {
        pgc_test2011_18 t = new pgc_test2011_18();
        t.test();
        int num = t.x;
    }

    int x = 10;

    public void test() {
        int num = this.x;
        int num2 = super.array_rows;
        int num3 = array_rows;
        int num4 = num;
        System.out.println("array_rows = " + super.array_rows);
        System.out.println("str = " + super.str);
    }
}