// This required the Java 1.7 support (default is Java 1.4), use option "-ecj:1.7".
public class test2011_45<T>
   {
     private T t; // T stands for "Type"

     public void add(T t)
        {
          this.t = t;
        }

     public T get()
        {
          return t;
        }
   }
