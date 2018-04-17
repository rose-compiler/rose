//
// Access field in super class
//
class Ok {
    public Ok(String s) {
    	this.str = s;
    }

    String str;
}

public class pgc_test2011_20 extends Ok {
    pgc_test2011_20() {
        super("what?");
    }

    static public void main(String args[]) {
        pgc_test2011_20 t = new pgc_test2011_20();
        t.test("Say ");
    }

    public void test(String prefix) {
        System.out.println(prefix + str);
    }
}