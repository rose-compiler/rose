public class T6_MultiArray {
    public static void main(String[] args) {
        int[][] matrix = new int[3][4]; // Creates a 3x4 integer matrix

        // Initializing the elements of the 2D array
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[i].length; j++) {
                matrix[i][j] = i * 10 + j;
            }
        }

        // Accessing and printing the elements of the 2D array
        System.out.println("Elements of the matrix:");
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[i].length; j++) {
                System.out.println("matrix[" + i + "][" + j + "] = " + matrix[i][j]);
            }
        }
    }
}
