// From Philippe, March 2014
// Modified so that snippet() is a sibling of the other types.
class Exotic2 {

    class Other {
	    public int field = 10;
    }

    class Extension {
	    class Inner extends Other {
	    }
    }

    public class TestQualifiedType extends Extension {
	    public void foo() {
            Inner inner = new Inner();
	        System.out.println("The field's value is " + inner.field);
	    }
    }

    void snippet() {}
}