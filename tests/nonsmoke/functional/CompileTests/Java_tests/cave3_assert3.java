public class cave3_assert3 {
    public static boolean test() {
        return false;
    }
    
    public static void fct() {
        int i = 3;
        // can give any expression on the right hand side
        assert test() : (i==3);
    }

	public static void main(String[] args) {
	    fct();
	}
}
