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

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.io.FileWriter;
import java.io.IOException;

import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.IFortranParserAction;


public class ToC {

   /**
    * @param args
    */
   public static void main(String[] args) {
      String jniPackage = "fortran_ofp_parser_c_jni";
      String className = "fortran.ofp.parser.java.IFortranParserAction";
      String newJavaClassName = "FortranParserActionJNI";
      generateCWrappers(jniPackage, className, newJavaClassName);
      generateJavaActionClass(jniPackage, className, newJavaClassName);
   }

   
   public static void generateJavaActionClass(String jniPackage, String interfaceName, 
                                              String newJavaClassName) {
      FileWriter javaFile = null;

      // Open a file for the generated Java action class.
      try {
         javaFile = new FileWriter(newJavaClassName + ".java");
      } catch(Exception e) {
         e.printStackTrace();
         System.exit(1);
      }

      // Generate the Java action class that will interface to the JNI.
      try {
         generateJavaJNIClass(jniPackage, interfaceName, 
										"IFortranParserAction", 
                              newJavaClassName, 
                              javaFile);
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
   }// end generateJavaActionClass()


   public static void generateJavaJNIClass(String jniPackage,
					   String interfaceName,
                                           String interfaceShortName,
                                           String className, 
                                           FileWriter javaFile) 
      throws IOException {
      try {
         // Print out the package and import statements.
         javaFile.write("package fortran.ofp.parser.c.jni;\n");
			javaFile.
				write("import fortran.ofp.parser.java.IFortranParserAction;\n");
			javaFile.write("import fortran.ofp.parser.java.FortranParser;\n");
         javaFile.write("import org.antlr.runtime.Token;\n");
         javaFile.write("\n");

         // Get all methods defined in the interface so we can generate the
         // native method signatures.
         Method[] methods = 
            Class.forName(interfaceName).getDeclaredMethods();

         // Print out the class signature
         javaFile.write("public class " + className + " implements " + 
                        interfaceShortName + " {\n");

         // Print out the constructor
         javaFile.write("\tpublic " + className  
                        + "(String[] args, FortranParser parser, "
                        + "String fileName)"
                        + " {\n" + "\t\tsuper();\n" + "\t}\n");

         for (int i = 0; i < methods.length; i++) {
            // Print out the method mangled name.
            javaFile.write("\tnative public void " + 
                           methods[i].getName() + "(");

            // Print out the args.
            printMethodArgsForJava(methods[i], javaFile);
            
            // print the closing paren for the function signature.
            javaFile.write(");\n");
         }

         // Print the static code block that loads the shared object with 
         // the C JNI code.
         javaFile.write("\n");
         javaFile.write("\tstatic {\n"+ "\t\tSystem.loadLibrary(" +
                        "\"" + jniPackage + "_" + className + "\");\n\t}\n");

         // Close the class definition
         javaFile.write("}\n");
      } catch (Exception e) {
         // InstantiationException, IllegalAccessException, 
         // IllegalArgumentException, InvocationTargetException
         // ClassNotFoundException, NoSuchMethodException
         System.err.println(e);
      }
      
      
      return;
   }// end generateJavaJNIClass()
   

   public static void generateCWrappers(String jniPackage, String className, 
                                        String newJavaClassName) {
      FileWriter cFile = null;

      // Open a file for the generated C code.
      try {
         cFile = new FileWriter("c_actions_jni.c");
      } catch(Exception e) {
         e.printStackTrace();
         System.exit(1);
      }

      // Generate the C JNI wrappers.
      try {
         generateCCode(jniPackage, className, newJavaClassName, cFile);
      } catch(Exception e) {
         // IOException
         e.printStackTrace();
         System.exit(1);
      }
      
      // Close the generated C file.
      try {
         cFile.close();
      } catch(Exception e) {
         e.printStackTrace();
         System.exit(1);
      }

      return;
   }
        
   public static void generateCCode(String jniPackage, String className, 
                                    String newJavaClassName, FileWriter cFile) 
      throws IOException {
      try {
         // Print the includes for the C file.
         cFile.write("#include <stdlib.h>\n");
         cFile.write("#include <string.h>\n");
         cFile.write("#include \"jni.h\"\n");
         cFile.write("#include \"token.h\"\n");
         cFile.write("#include \"jni_token_funcs.h\"\n");
         cFile.write("#include \"FortranParserAction.h\"\n");
         cFile.write("\n");

         Method[] methods = 
            Class.forName(className).getDeclaredMethods();
         for (int i = 0; i < methods.length; i++) {
            // Print out the method mangled name.
            cFile.write("void " + "Java_" + jniPackage + "_" + 
                        newJavaClassName + "_" + 
                        mangleMethodName(methods[i].getName()) + "(");

            // Print out the args.
            printMethodArgsForC(methods[i], cFile);
            
            // print the closing paren for the function header.
            cFile.write(")\n");

            // print out the opening curly for the function block.
            cFile.write("{\n");

            // print the function body, including marshalling of params into 
            // C types and calling the regular (non-JNI) C action.
            printFunctionBody(methods[i], cFile);

            // print out the closing curly for the funtion block.
            cFile.write("}\n");

            // Print a blank line between functions for readability.
            cFile.write("\n");
         }
      } catch (Exception e) {
         // InstantiationException, IllegalAccessException, 
         // IllegalArgumentException, InvocationTargetException
         // ClassNotFoundException, NoSuchMethodException
         System.err.println(e);
      }

   }

   private static String mangleMethodName(String methodName) {
      StringBuffer mangledName = new StringBuffer(methodName);

      for(int i = 0; i < mangledName.length(); i++) {
         if(mangledName.charAt(i) == '_') {
            // the '_' needs to be '_1' for the JNI mangling, so insert a '1'
            // after the '_'.
            mangledName = mangledName.insert(i+1, '1');
            // put us on the inserted '1' so the loop increment will move 
            // us past it.  
            i++;
         }
      }

      return mangledName.toString();
   }// end mangleMethodName()


   private static void printMethodArgsForC(Method currMethod, 
                                           FileWriter cFile) 
      throws IOException {
      Class[] paramTypes = currMethod.getParameterTypes();

      // Print out the required JNIEnv (env) and jobject (this) params.
      cFile.write("JNIEnv *env, jobject this");
      for (int ii = 0; ii < paramTypes.length; ii++) {
         if(paramTypes[ii].isPrimitive()) {
            cFile.write(", j" + paramTypes[ii].getCanonicalName()
                        + " jarg_" + ii);
         } else if(paramTypes[ii].getSimpleName().compareTo("String") == 0) {
				cFile.write(", jstring jarg_" + ii);
			} else {
            cFile.write(", jobject jarg_" + ii);
         }
      }
      
      return;
   }// end printMethodArgsForC()


   private static void printMethodArgsForJava(Method currMethod,
                                              FileWriter javaFile) 
      throws IOException {
      Class[] paramTypes = currMethod.getParameterTypes();

      // See if we have any arguments.
      if(paramTypes.length == 0)
         return;

      // Print out the arguments for the Java JNI prototype/decl.
      javaFile.write(paramTypes[0].getSimpleName() + " jarg_0");
      for(int i = 1; i < paramTypes.length; i++) {
         javaFile.write(", " + paramTypes[i].getSimpleName() + " jarg_" + i);
      }

      return;
   }

   
   private static void printFunctionBody(Method currMethod, FileWriter cFile) 
      throws IOException {
      Class[] paramTypes = currMethod.getParameterTypes();
      
      // Generate the local C decls.
      for(int i = 0; i < paramTypes.length; i++) {
         if(paramTypes[i].isPrimitive()) {
            String primitiveType = 
               new String(paramTypes[i].getCanonicalName());
            if(primitiveType.compareTo("boolean") == 0 ||
               primitiveType.compareTo("int") == 0) {
               // C has no boolean so convert to an int
               cFile.write("\tint carg_" + i + ";\n");
            } 
         } else {
				if(paramTypes[i].getSimpleName().compareTo("Token") == 0) {
					// print out a ptr decl of type Token_t which is our C struct
					// for a Token.
					cFile.write("\tToken_t *carg_" + i + " = NULL;\n");
				} else if(paramTypes[i].getSimpleName().compareTo("String") == 0) {
					cFile.write("\tconst char *carg_" + i + " = NULL;\n");
				} else {
               System.err.println("Unhandled Class in JNI generation: " + 
                                  paramTypes[i].getSimpleName());
               System.exit(1);
				}					
         }
      }

      // Put a blank line after the declarations.
      cFile.write("\n");

      // For debugging, put a print statement in each function for tracking.
      cFile.write("#ifdef DEBUG\n");
      cFile.write("\tprintf(\"hello from %s\\n\", \"" + currMethod.getName()
                  + "\");\n");
      cFile.write("#endif\n");
      cFile.write("\n");

      // Generate the initialization/casting for each C decl.
      for(int i = 0; i < paramTypes.length; i++) {
         if(paramTypes[i].isPrimitive()) {
            String primitiveType = 
               new String(paramTypes[i].getCanonicalName());
            if(primitiveType.compareTo("boolean") == 0 ||
               primitiveType.compareTo("int") == 0) {
               // C has no boolean so convert to an int
               cFile.write("\tcarg_" + i + " = (int)jarg_" + i + ";\n");
            } 
         } else if(paramTypes[i].getSimpleName().compareTo("Token") == 0) {
            // Generate the call to the marshalling function for Tokens.
            cFile.write("\tif(jarg_" + i + " != NULL)\n");
            cFile.write("\t\tcarg_" + i + 
                        " = convert_Java_token(env, jarg_" + i + ");\n");
//             // Just for debugging..remove later.
//             cFile.write("\telse\n" + 
//                         "\t\tprintf(\"jarg_"+i+" was NULL!\\n\");\n");
         } else if(paramTypes[i].getSimpleName().compareTo("String") == 0) {
				// Convert the jstring to a const char * in C.
				cFile.write("\tif(jarg_" + i + " != NULL)\n");
				cFile.write("\t{\n");
				cFile.write("\t\tif(env == NULL)\n");
				cFile.write("\t\t{\n");
				cFile.write("\t\t\tfprintf(stderr, \"env is NULL!\\n\");\n");
				cFile.write("\t\t\texit(1);\n");
				cFile.write("\t\t}\n");
				cFile.write("\t\tcarg_" + i + " = (*env)->GetStringUTFChars(env, " 
								+ "jarg_" + i + ", NULL);\n");
				cFile.write("\t}\n");
			}				
      }

      // Put a blank line after the initialization.
      cFile.write("\n");


      // Generate the call to the C action.
      cFile.write("\tc_action_" + currMethod.getName() + "(");
      for(int i = 0; i < paramTypes.length-1; i++) {
         cFile.write("carg_" + i + ", ");
      }
      // Print out the last (or first and only) arg.
      if(paramTypes.length > 0) {
         cFile.write("carg_" + (paramTypes.length-1));
      }
      cFile.write(");\n");
      // Put a blank line after the action call.
      cFile.write("\n");

		// Release the string args.
		for(int i = 0; i < paramTypes.length; i++) {
			if(paramTypes[i].getSimpleName().compareTo("String") == 0) {
				cFile.write("\t(*env)->ReleaseStringUTFChars(env, jarg_" + i + 
								", carg_" + i + ");\n");
			}
		}

      // Generate the free() call for each created C Token.
      for(int i = 0; i < paramTypes.length; i++) {
         if(paramTypes[i].getSimpleName().compareTo("Token") == 0) {
            // Generate the call to the marshalling function.
            cFile.write("\tif(carg_" + i + " != NULL)\n");
				// Register the token (store a reference to it) in the token list
				// so we can free it up after all actions have been called.  
				// The cleanUp method will free these tokens and is called by 
				// main() (or whatever routine is controlling the parser).
            cFile.write("\t\tregister_token(carg_" + i + ");\n");
         }
      }

		// if this is the cleanUp method, we need to generate the call to 
		// free up the list of C tokens created.
		if(currMethod.getName().compareTo("cleanUp") == 0)
			cFile.write("\tfree_token_list();\n");
      
      return;
   }// end printFunctionBody()

}
