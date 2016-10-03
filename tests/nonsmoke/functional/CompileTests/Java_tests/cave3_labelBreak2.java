
public class cave3_labelBreak2 {
	void test() {
		int i = 0;
		int j = 10;
		// breaks in a children of the labeled statement
		// and cfg has to traverse enclosing labeled statements
		l:{
			p: while (i < j) {
				i = i + 1;
				break l;
			}
		}
	}
}
