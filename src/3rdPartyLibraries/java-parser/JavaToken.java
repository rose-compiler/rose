// This is the token class in Java that will will hand as an object to the C++ JNI functions.

public class JavaToken // extends CommonToken
   {
  // We mostly just want the token to carry the source position.
  // We might alternatively implement a source position object 
  // to handle to the C++ JNI functions.

     public String text;
     public int line_number;
     public int column_number;

     public JavaToken(String s, int line, int col)
        {
          text          = s;
          line_number   = line;
          column_number = col;
        }

     public String getText()
        {
          return text;
        }

     public int getLine()
        {
          return line_number;
        }

     public int getColumn()
        {
          return column_number;
        }
   }
