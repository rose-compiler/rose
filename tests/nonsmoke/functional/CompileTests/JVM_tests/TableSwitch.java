class TableSwitch {
    public static int main(String args[]) {
        int x = -43;
        int y = 42;
        int z, cval=-13;

        z = x + y;
        for (int i = 0; i < 10; i++) {
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
                cval = 4;
                break;
          }
          z++;
        }
        return cval;
    }
}
