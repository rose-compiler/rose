public class T7_TestNull {
    public static void main(String[] args) {
        // Declare a reference variable without initializing it
        String myString = null;

        // Test for null
        if (myString == null) {
            System.out.println("myString is null.");
        } else {
            System.out.println("myString is not null.");
        }

        // Initialize the variable with a value
        myString = "Hello, World!";

        // Test for not null
        if (myString != null) {
            System.out.println("myString is not null.");
        } else {
            System.out.println("myString is null.");
        }
    }
}
