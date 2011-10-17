
public class cave3_labelContinue0 {
	public static void test() {
		int i = 0;
		// continue to the immediately enclosing
		l: while (i < 10) {
			i += 1;
			continue l;
		}
		i+=1;
	}
}
