public class cave3_instanceof0 {
	public static boolean check(cave3_instanceof0 obj) {
		
		 return (obj instanceof cave3_instanceof0);
	}
	
	public static void main(String [] args) {
		cave3_instanceof0 obj = new cave3_instanceof0();
		
		boolean res = check(obj);
		
		if (res) {
			System.out.println("instanceof succeed");
		}
	}
}
