import java.util.ArrayList;
import java.util.List;

public class cave3_foreach6 {
	public static void main(String[] args) {
		List<Integer> l = new ArrayList<Integer>();
		for (Integer i : l) {
			break;
		}

		int j = 0;
		while (j < 10) {
			break;
		}
	}
}