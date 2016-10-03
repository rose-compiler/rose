public class cave3_labelBreak1 {
	void test(int num) {
		// breaks in a children of the labeled statement
		start: if (num < 0) {
		
		} else {
			num = num - 1;
			break start;
		}
	}	
	
//	public static void main(String[] args) {
//		cave3_label1 l = new cave3_label1();
//		l.test(4);
//	}
}
