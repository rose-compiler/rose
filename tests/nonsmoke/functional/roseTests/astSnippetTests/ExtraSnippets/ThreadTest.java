public class ThreadTest {
    void snippet() {
        Thread thread = new Thread("New Thread") {
            public void run(){
                System.out.println("foo");
            }
        };
    }
}
