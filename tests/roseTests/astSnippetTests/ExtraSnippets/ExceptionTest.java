public class ExceptionTest {

    public class Exception extends Throwable {
        public void printStackTrace() {
            System.out.println("the stack");
        }
    }

    public void foo() {
        try {
            throw new Exception();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
