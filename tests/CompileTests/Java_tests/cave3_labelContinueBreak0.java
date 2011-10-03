public class cave3_labelContinueBreak0 {
	public static void test() {
		int i = 0, j = 0;
		l: while (j < 10) {
			b:{
				i += 1;
				break b;
			}
			continue l;
		}
		i+=1;
	}
	
	public static void main(String[] args) {
		test();
	}
}
