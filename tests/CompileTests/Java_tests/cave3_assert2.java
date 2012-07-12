public class cave3_assert2 {
    public static boolean test() {
        return true;
    }
    
    public static void fct() {
        assert test() : "Assertion triggered";
    }

	public static void main(String[] args) {
	    fct();
	}
}
