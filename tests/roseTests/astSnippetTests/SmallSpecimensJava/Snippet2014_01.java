/**
 * Created by matzke on 3/26/14.
 */
public class Snippet2014_01 {

    public class Exception extends Throwable {
        public void printStackTrace() {
            System.out.println("the stack");
        }
    }

    public void basic() {
        try {
            throw new Exception();
        } catch (Exception e) {
               e.printStackTrace();
        }
    }
}

/*
public class Snippet2014_01 
   {
      public class XXX extends Throwable 
      {
        public void printStackTrace(){}
      }
      
     public void basic() 
        {
          int a = 0;
          try 
             {
               ++a;
			    } 
//        catch (Exception e) 
          catch (XXX e) 
             {
			      e.printStackTrace();
			    }
        }
   }
*/
