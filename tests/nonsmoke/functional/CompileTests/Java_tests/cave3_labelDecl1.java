public class cave3_labelDecl1 {
	void test() {
		int num = 4;
		start: if (num < 0) {
			num = num + 1;
		}
	}
	
	public static void main(String[] args) {
		cave3_labelDecl1 l = new cave3_labelDecl1();
		l.test();
	}
}
