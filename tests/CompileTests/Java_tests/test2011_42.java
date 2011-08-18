import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

public class test2011_42
   {
     public void foo()
        {
       // List v; // = new ArrayList();
          List v; // = new ArrayList();
          List w = new ArrayList();

       // This is an error, since we don't include the member functions of List into the AST (must fix this).
       // w.add("test");

       // Integer i = (Integer)v.get(0); // Run time error
        }
   }
