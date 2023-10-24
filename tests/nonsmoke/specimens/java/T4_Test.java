class T4_Test {
    public static void main(String args[]) {
        int x = -43;
        int y = 42;
        int z;

        z = x + y;
        for (int i = 0; i < 10; i++) {
            switch (z) {
            case 0:
                System.out.println("case1");
                break;
            case 3:
                System.out.println("case2");
                break;
            case -1:
                System.out.println("case3");
                break;
            default:
                System.out.println("default");
                break;
            }
            z++;
        }
    }
}
