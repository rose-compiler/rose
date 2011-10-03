import java.util.ArrayList;
import java.util.List;

public class cave3_labelBreak4 {
	public static void main(String[] args) {
		List<Integer> l = new ArrayList<Integer>();
		int k = 0;
		// Breaks a foreach loop
		st: for (Integer i : l) {
			k = 4;
			break st;
		}
		k=5;
	}
}
