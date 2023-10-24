class T9_Base {
    public static int x = 4;
}
 
class T9_Middle extends T9_Base {
    public static int y = 5;
}
 
class T9_Bottom extends T9_Middle {
    public static int z = 6;
}
 
public class T9_MembersOnly {
    public static void main(String args[]) {
        System.out.println(T9_Bottom.x);
        System.out.println(T9_Bottom.z);
    }
}
