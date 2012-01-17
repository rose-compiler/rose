/**
 * This is the token class in Java that we will hand as an object to the C++ JNI functions.
 * We also wrap source position information here.
 */
public class JavaToken
   {
  // We mostly just want the token to carry a string version
  // of the parsed code and source position.
	 public String text;
	 private JavaSourcePositionInformation posInfo;

     public JavaToken(String s, JavaSourcePositionInformation posInfo)
        {
          this.text = s;
          this.posInfo = posInfo;
        }

     public String getText()
        {
          return text;
        }

     public JavaSourcePositionInformation getJavaSourcePositionInformation() 
        {
          return this.posInfo;
        }
   }
