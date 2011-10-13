public class cave3_labelBreak5 {
	public static void test() {
		int i = 0;
		int j = 0;
		// Exercizes multi-labels
		k:l: while(j < 10) {
			b:{
				while(j < 10) {
					i += 1;
					break l;
				}
			}
		}
		i+=1;
	}

	public static void main(String[] args) {
		cave3_labelBreak5.test();
	}
}
