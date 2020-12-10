class Other {
    static public int field = 10;
}

class Extension {
    class Inner extends Other {
    }
}

public class TestQualifiedType extends Extension {
    static public void main(String args[]) {
        System.out.println("The fied's value is " + TestQualifiedType.Inner.field);
    }
}