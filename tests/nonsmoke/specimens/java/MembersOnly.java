class Base {
    public static int x = 4;
}
 
class Middle extends Base {
    public static int y = 5;
}
 
class Bottom extends Middle {
    public static int z = 6;
}
 
public class MembersOnly {
    public static void main(String args[]) {
        System.out.println(Bottom.x);
        System.out.println(Bottom.z);
    }
}
