// This required the Java 1.5 to 1.7 support (default in ROSE is now Java 1.6)
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
