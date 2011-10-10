import java.util.ArrayList;
import java.util.List;

public class cave3_foreach4 {
	public static void main(String[] args) {
		List<Integer> l = new ArrayList<Integer>();
		int k = 0;
		for (Integer i : l) {
			k = 4;
			continue;
		}
		k=5;
	}
}
