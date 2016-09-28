public class cave3_labelBreak3 {
	public static void test() {
		int i = 0;
		//breaks in the innermost labeled statement
		l: while(i < 10) {
			b:{
				i += 1;
				break b;
			}
		}
		i = 10;
	}
	
	public static void main(String[] args) {
		cave3_labelBreak3.test();
	}
}
