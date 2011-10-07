public class cave3_labelContinueBreak1 {
	public static void test() {
		int i = 0, j = 0;
		// Exercizes multi-labels
		k:l:while (j < 10) {
			b:{
				i += 1;
				break b;
			}
			continue l;
		}
		i+=1;
	}
	
	public static void main(String[] args) {
		cave3_labelContinueBreak1.test();
	}
}
