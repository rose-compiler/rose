// Define an interface named TheInterface
interface T10_TheInterface {
    void interfaceMethod();
}

// Create a class that implements the TheInterface interface
class T10_InterfaceImpl implements T10_TheInterface {
    @Override
    public void interfaceMethod() {
        System.out.println("This is the implementation of interfaceMethod in TestInterface class");
    }
}

class T10_InterfaceImplOther implements T10_TheInterface {
    @Override
    public void interfaceMethod() {
        System.out.println("This is the other.");
    }
}

abstract class T10_PartialParent implements T10_TheInterface {
}

class T10_CompleteChild extends T10_PartialParent {
    @Override
    public void interfaceMethod() {
        System.out.println("This is the complete child.");
    }
}

public class T10_TestInterface {
    public static void foo(T10_TheInterface o) {
        o.interfaceMethod();
    }

    public static void main(String[] args) {
        T10_InterfaceImpl obj = new T10_InterfaceImpl();
        obj.interfaceMethod();
        T10_InterfaceImplOther obj2 = new T10_InterfaceImplOther();
        obj2.interfaceMethod();
        T10_CompleteChild obj3 = new T10_CompleteChild();
        obj3.interfaceMethod();
        foo(obj);
        foo(obj2);
        foo(obj3);
    }
}
