public class cave3_synchronized0 {
	
	public static void main(String [] args) {
		cave3_synchronized0 obj = new cave3_synchronized0();
		int a = 0;
		synchronized(obj) {
			a++;
		}
	}
}
