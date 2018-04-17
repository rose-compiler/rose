// TOO1 (2/3/2014): Example from jetty.project/jetty-util/src/main/java/org/eclipse/jetty/util/resource/JarResource.java
//
// == Regression Test Description
//
// ROSE did not unparse the resource variable declarations in the correct order:
//
//    try (JarInputStream jin = new JarInputStream(is); InputStream is = null)
//
// Notice that `is` is being referenced before it is declared.
//
// See Java specification:
// http://docs.oracle.com/javase/specs/jls/se7/html/jls-14.html#jls-14.20.3

import java.io.IOException;
import java.io.InputStream;
import java.util.jar.JarInputStream;

public class TryWithResources001 {
  public static void main(String[] args)
        throws IOException
  {
      try (InputStream is = null;
              JarInputStream jin = new JarInputStream(is))
      {
      }
  }
}
