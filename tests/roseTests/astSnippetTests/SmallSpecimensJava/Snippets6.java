/* Tests for variable renaming.  Any variable whose name begins with "tmp" is renamed so it doesn't conflict with
 * other variables that might be visible at the insertion point. */
public class Snippets6 {

    void randomOffByOne(int arg1) {
        java.util.Random tmp_random = new java.util.Random();
        int tmp_offset = tmp_random.nextInt() % 3 - 1;
        arg1 += tmp_offset;
    }

    // Verify that copies of some things can be suppressed. See the doNotInsert calls in injectSnippet.C
    void shouldNotBeInserted() {}
    int someOtherInteger;
    class SomeOtherStruct {
        int dummy;
    };

    // Verify that some miscellaneous things are copied into the target.
    int thisIsAnInteger;
    class SomeStruct {
        int dummy;
    };

    // Verify that black list with overloading works
    void someFunction(int a) {}	// inserted
    void someFunction() {}	// not inserted

    void randomOffByOne() {
        // this snippet is not called recursively because it has a different number of
        // arguments than what is used at the call site.
    }

    public void notNeeded() {
        System.out.println("this function is not ever called");
    }

    void addWithError(int addend1, int addend2, int result) {
        randomOffByOne(addend1);
        randomOffByOne(addend2);
        result = addend1 + addend2;
    }
}
