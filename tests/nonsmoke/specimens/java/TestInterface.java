// Define an interface named TheInterface
interface TheInterface {
    void interfaceMethod();
}

// Create a class that implements the TheInterface interface
class InterfaceImpl implements TheInterface {
    @Override
    public void interfaceMethod() {
        System.out.println("This is the implementation of interfaceMethod in TestInterface class");
    }
}

class InterfaceImplOther implements TheInterface {
    @Override
    public void interfaceMethod() {
        System.out.println("This is the other.");
    }
}

abstract class PartialParent implements TheInterface {
}

class CompleteChild extends PartialParent {
    @Override
    public void interfaceMethod() {
        System.out.println("This is the complete child.");
    }
}

public class TestInterface {
    public static void foo(TheInterface o) {
        o.interfaceMethod();
    }

    public static void main(String[] args) {
        InterfaceImpl obj = new InterfaceImpl();
        obj.interfaceMethod();
        InterfaceImplOther obj2 = new InterfaceImplOther();
        obj2.interfaceMethod();
        CompleteChild obj3 = new CompleteChild();
        obj3.interfaceMethod();
        foo(obj);
        foo(obj2);
        foo(obj3);
    }
}
