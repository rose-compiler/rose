import java.util.ArrayList;
import java.util.List;

public class cave3_foreach3 {
    public static List<Integer> fct() {
        return new ArrayList<Integer>();
    }
	
    public static void main(String[] args) {
        for (Integer i : fct()) {
            System.out.println(i.toString());
        }
    }
}
