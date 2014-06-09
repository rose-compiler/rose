public class Specimen1 {
    static private int INSERT_HERE;

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
        return f3;
    }

    public static void main(String args[]) {
	Specimen1 specimen = new Specimen1();
	specimen.ipoint1();
    }
}
