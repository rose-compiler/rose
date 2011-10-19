
public class cave3_labelBreak0{
	public static void test() {
		int i = 0;
		// break to the immediately enclosing
		l:{
			i += 1;
			break l;
		}
		i+=1;
	}
	
	public static void main(String[] args) {
		cave3_labelBreak0.test();
	}
}
