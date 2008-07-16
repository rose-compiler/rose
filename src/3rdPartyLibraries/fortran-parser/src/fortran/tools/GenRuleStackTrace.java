/******************************************************************************
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL), which
 * is operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *****************************************************************************/

package fortran.tools;

import java.util.List;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.io.FileWriter;
import java.io.IOException;

import org.antlr.runtime.Token;

import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.FortranParserActionNull;


public class GenRuleStackTrace extends FortranParserActionNull {
	private FortranParser parser;

	public GenRuleStackTrace(String[] args, FortranParser parser, 
								 String filename) {
		super(args, parser, filename);
		
		this.parser = parser;
	}// end GenRuleStackTrace()

   /**
    * @param args
    */
   public static void main(String[] args) {
      String interfaceName = "fortran.ofp.parser.java.IFortranParserAction";
      
      // Generate the action methods that build the stack trace.
      genActionClass(interfaceName);
   }// end main()

	public static void genActionClass(String interfaceName) {
      FileWriter javaFile = null;
		String newJavaClassName = "RuleStackTrace";

      // Open a file for the generated Java action class.
      try {
         javaFile = new FileWriter("RuleStackTrace.java");
      } catch(Exception e) {
         e.printStackTrace();
         System.exit(1);
      }

      // Generate the Java action class that will build the stack trace.
      try {
			// Print out the package this class will belong to and the 
			// import statements.
			javaFile.write("package fortran.tools;\n\n");
			javaFile.write("import java.util.List;\n");
			javaFile.write("import java.util.Stack;\n");
			javaFile.write("import org.antlr.runtime.Token;\n");
			javaFile.write("import fortran.ofp.parser.java.FortranParser;\n");
			javaFile.write("import fortran.ofp.parser.java.FortranParserActionNull;\n\n");

			// Print out the class name, etc.
			javaFile.write("public class " + newJavaClassName + " extends ");
			javaFile.write("FortranParserActionNull {\n");
			
			// Print out our private variables.
			javaFile.write("\tprivate FortranParser parser;\n");
			javaFile.write("\tprivate Stack<String> ruleStack;\n");

			// Print out the constructor and initialize the private variables.
			javaFile.write("\tpublic " + newJavaClassName + 
								"(String[] args, ");
			javaFile.write("FortranParser parser,\n");
			javaFile.write("\t\t\t\tString filename) {\n");
			javaFile.write("\t\tsuper(args, parser, filename);\n");
			javaFile.write("\t\tthis.parser = parser;\n");
			javaFile.write("\t\tthis.ruleStack = new Stack<String>();\n");
			javaFile.write("\t}\n\n");

			// Generate the action methods that will build the rule stack.
         genActions(interfaceName, javaFile);

			javaFile.write("}\n");
      } catch(Exception e) {
         e.printStackTrace();
         System.exit(1);
      }

      // Close the generated Java file.
      try {
         javaFile.close();
      } catch(Exception e) {
         e.printStackTrace();
         System.exit(1);
      }

      return;
	}// end genActionClass()

	
   public static void genActions(String interfaceName, FileWriter javaFile) {
		try {
			String methodName = null;

         // Get all methods defined in the interface so we can generate the
         // action methods that build the rule stack.
         Method[] methods = 
            Class.forName(interfaceName).getDeclaredMethods();

         for (int i = 0; i < methods.length; i++) {
				methodName = methods[i].getName();

            // Print out the method name.
            javaFile.write("\tpublic void " + methodName + "(");

            // Print out the args.
            printMethodArgsForJava(methods[i], javaFile);
            
            // Print the closing paren for the function signature.
            javaFile.write(") {\n");
				
				// Print out the body, which is simply to push the function name, 
				// for everything that is not a stmt (except end_of_stmt).
				if(methodName.endsWith("_stmt") == false 
					|| methodName.compareTo("end_of_stmt") == 0) {
					javaFile.write("\t\tthis.ruleStack.push(new String(\"" + 
										methodName + "\"));\n");
				} else {
					// Reverse the stack by popping into another stack so we 
					// can see the calls in order and then print it out.
					javaFile.write("\t\tint stackSize = ruleStack.size();\n");
					javaFile.write("\t\tStack<String> inOrderStack = " + 
										"new Stack<String>();\n");
					javaFile.write("\t\tfor(int i = 0; i < stackSize; ");
					javaFile.write("i++) {\n");
					javaFile.write("\t\t\tinOrderStack.push(ruleStack.pop());\n");
					javaFile.write("\t\t}\n");
					javaFile.write("\t\tSystem.out.print(\"[\");\n");
					javaFile.write("\t\tfor(int i = 0; i < stackSize; ");
					javaFile.write("i++) {\n");
					javaFile.write("\t\t\tSystem.out.print(" + 
										"inOrderStack.pop() + \", \");\n");
					javaFile.write("\t\t}\n");
					javaFile.write("\t\tSystem.out.print(\"" + methodName
										+ "\" + \"]\\n\");\n");
				}
				
				// Write out the closing curly-brace for the method.
				javaFile.write("\t}\n\n");
         }
			
      } catch (Exception e) {
         System.err.println(e);
			e.printStackTrace();
      }
		
		return;
	}// end genActions()


   private static void printMethodArgsForJava(Method currMethod,
                                              FileWriter javaFile) 
      throws IOException {
      Class[] paramTypes = currMethod.getParameterTypes();

      // See if we have any arguments.
      if(paramTypes.length == 0)
         return;

      // Print out the arguments for the Java action method.
      javaFile.write(paramTypes[0].getSimpleName() + " jarg_0");
      for(int i = 1; i < paramTypes.length; i++) {
         javaFile.write(", " + paramTypes[i].getSimpleName() + " jarg_" + i);
      }

      return;
   }// end printMethodArgsForJava()

}// end class GenRuleStackTrace

