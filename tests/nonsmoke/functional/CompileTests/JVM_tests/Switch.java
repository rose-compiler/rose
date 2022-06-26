class Switch {
    public static int main(String args[]) {
        int x = -43;
        int y = 42;
        int z;

        z = x + y;
        for (int i = 0; i < 10; i++) {
            int cval;
            switch (z) {
            case 0:
                cval = 0;
                break;
            case 3:
                cval = 3;
                break;
            case -1:
                cval = -1;
                break;
            default:
                cval = 666;
                break;
            }
            System.out.println("case value is " + cval);
            z++;
        }
        return 0;
    }
}
