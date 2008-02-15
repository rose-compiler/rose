/**
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract DE-
 * AC52-06NA25396 for Los Alamos National Laboratory (LANL), which is
 * operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *  
 * Additionally, this program and the accompanying materials are made
 * available under the terms of the Eclipse Public License v1.0 which
 * accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Based on examples/docs from:
 *	http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/invocation.html#wp9502
 * http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/jniTOC.html
 * http://java.sun.com/docs/books/jni/html/invoke.html 
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "jni.h"

#define ERROR_CODE 4

/* These should be defined in jni.h.  */  
#ifndef JNI_VERSION_1_6
#define JNI_VERSION_1_6 6
#endif
#ifndef JNI_VERSION_1_4
#define JNI_VERSION_1_4 4
#endif
#ifndef JNI_VERSION_1_2
#define JNI_VERSION_1_2 2
#endif
#ifndef JNI_VERSION_1_1
#define JNI_VERSION_1_1 1
#endif  

  jstring getJavaString(JNIEnv *env, const char *string);
  jobjectArray getJavaStringArray(JNIEnv *env, int argc, char **argv);
  jchar *convertToJChar(const char *c_string);
  jclass getJavaStringClass(JNIEnv *env);
  void handleException(JavaVM *jvm, JNIEnv *env);
  int runOFP(int argc, char **argv);
  

/* Include the patsh to the source tree and build tree for ROSE */
#include "rose_config.h"

/* This is defined if ROSE is configured to use the Java Open Fortran Parser */
#ifdef USE_ROSE_JAVA_SUPPORT

#include "rose_paths.h"
#include "assert.h"

/* This is critical to permitting the correct library (the one just built by the user)
// to be used instead of requiring the LD_LIBRARY_PATH to beexplicitly set by the user
// before running any tests.  It is alos important to proper testing using "make distcheck"
// since we want the new library built using that rule to be tested over any other.
*/
#define OVERWRITE_LD_LIBRARY_PATH 1

/* DQ (8/16/2007): This is what we want ROSE to call so that we can run the Java based OpenFortranParser from C++. */
int openFortranParser_main(int argc, char **argv)
  {
 /* Call the function that will start a JVM and call the OFP.  */

 /* To use different versions of the LD_LIBRARY_PATH, get the value, 
    change it to the path in the build tree, and then reset it to the 
    old value after the call to JVM.  We need to do this because the
    libparser_java_FortranParserActionJNI.so that we need is in the 
    build tree and it is specific to the configuration of ROSE (else 
    V_<class name> enum values will be different as a result of 
    configuration options that might trigger different numbers of IR 
    nodes to be use.
  */

 /* Overwite to a new value. It is not clear when to use the install path and when to use the build path! */
    const char* new_value = ROSE_AUTOMAKE_TOP_BUILDDIR "/src/frontend/OpenFortranParser_SAGE_Connection/.libs";

 /* Save the old value */
    const char* old_value = getenv("LD_LIBRARY_PATH");

#if 0
    printf ("Old value: LD_LIBRARY_PATH = %s \n",old_value);
    printf ("New value: LD_LIBRARY_PATH = %s \n",new_value);
 /* This value can be empty if the configuration was done using a relative path, as I recall! */
    printf ("ROSE_AUTOMAKE_TOP_BUILDDIR = %s \n",ROSE_AUTOMAKE_TOP_BUILDDIR);
    printf ("ROSE_COMPILE_TREE_PATH     = %s \n",ROSE_COMPILE_TREE_PATH);
#endif

#if OVERWRITE_LD_LIBRARY_PATH
    int overwrite = 1;
    int env_status = setenv("LD_LIBRARY_PATH",new_value,overwrite);
    assert(env_status == 0);
#endif

 /* printf ("Call the function that will start a JVM and call the OFP \n"); */
    int status = runOFP(argc, argv);
 /* printf ("DONE: Call the function that will start a JVM and call the OFP status = %d \n",status); */

 /* Reset to the saved value */
#if OVERWRITE_LD_LIBRARY_PATH
    env_status = setenv("LD_LIBRARY_PATH",old_value,overwrite);
    assert(env_status == 0);
#endif

	 return status;
  }

#else

int main(int argc, char **argv)
  {
	 int retval;
	 
	 /* Call the function that will start a JVM and call the OFP.  */
	 retval = runOFP(argc, argv);
	 
	 return retval;
  }
#endif

int runOFP(int argc, char **argv)
  {
	 JavaVM *jvm;   /* The Java VM.  */
	 JNIEnv *env;   /* The environment for retrieving class objects, etc.  */
	 JavaVMInitArgs jvm_args;  /* VM initialization args.  */
	 jclass ofp_class;  /* The OpenFortranParser main class.  */
	 jmethodID cons_method_id;
	 jmethodID tmp_method_id = NULL;
	 jclass new_ofp_class = NULL;
	 jclass action_class = NULL;
	 jobject action_obj = NULL;
	 jobject parser_obj = NULL;
	 jclass parser_class = NULL;
	 jstring fileName;
	 jstring type;
	 jobjectArray args;
	 char *classpathEnvVar = NULL;
	 char *jni_version = NULL;
	 char *classpath;
	 const char *javaCPOption = "-Djava.class.path=";
	 int len;
	 jboolean retval;

	 classpathEnvVar = getenv("CLASSPATH");
	 len = strlen(classpathEnvVar) + strlen(javaCPOption) + 1;
	 classpath = malloc(sizeof(char)*len);
	 snprintf(classpath, len, "%s%s\0", javaCPOption, classpathEnvVar);

	 /* Set up the VM initialization args.  */
	 jni_version = getenv("JNI_VERSION");
	 if(jni_version)
	 {
		if(strcmp(jni_version, "1.6") == 0)
		  jvm_args.version = JNI_VERSION_1_6;
		else if(strcmp(jni_version, "1.4") == 0)
		  jvm_args.version = JNI_VERSION_1_4;
		else if(strcmp(jni_version, "1.2") == 0)
		  jvm_args.version = JNI_VERSION_1_2;
		else if(strcmp(jni_version, "1.1") == 0)
		  jvm_args.version = JNI_VERSION_1_1;
		else
		{
		  /* We got an *invalid* JNI_VERSION.  Try defaulting to 1_4.  */
		  fprintf(stderr, "Warning: Invalid JNI_VERSION.\nDefaulting to 1.4!\n");
		  jvm_args.version = JNI_VERSION_1_4;
		}
	 }
	 else
	 {
   /* The user did not set JNI_VERSION in their environment.  Warn them and then try defaulting to 1_4.  */
		fprintf(stderr, "Warning: JNI_VERSION environment variable is not set! Defaulting to 1.4!\n");
		jvm_args.version = JNI_VERSION_1_4;
	 }

	 jvm_args.options = malloc(sizeof(JavaVMOption));
	 jvm_args.nOptions = 1;
	 jvm_args.options->optionString = strdup(classpath);
	 jvm_args.ignoreUnrecognized = JNI_FALSE;

	 /* Create and load the Java VM.  */
	 JNI_CreateJavaVM(&jvm, (void **)&env, &jvm_args);

	 /* Get the FortranMain class.  */
	 ofp_class = NULL;
	 ofp_class = (*env)->FindClass(env, "fortran/ofp/FrontEnd");
	 if(ofp_class == NULL)
		handleException(jvm, env);

	 /* Get the constructor for FortranMain(String[] args, String fileName,
		 String type).  */
	 cons_method_id =	(*env)->GetMethodID
		(env, ofp_class, "<init>",
		 "([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	 if(cons_method_id == NULL)
		handleException(jvm, env);

	 /* Create a Java String[] out of argv (everything after the first arg).  */
	 args = getJavaStringArray(env, argc, argv);
	 /* The filename of the Fortran source to parse MUST be the LAST
		 argument!  The setting of this and the type below are probably not
		 needed since we call main() and give it all args to parse itself.  */
	 fileName = getJavaString(env, argv[argc-1]);
	 type = getJavaString(env, "fortran.ofp.parser.java.FortranParserActionNull");
	 if(fileName == NULL || args == NULL || type == NULL)
		handleException(jvm, env);
	 else
	 {
		jmethodID mainMethodID = NULL;
		jmethodID errorMethodID = NULL;
		
		/* Get a FortranMain by calling the constructor referred to by
			cons_method_id (String[] argc, String filename, String type).  */
		new_ofp_class = (*env)->NewObject(env, ofp_class, cons_method_id,
													 args, fileName, type);

		/* Get the method ID for the main(String[] args) method in
			FortranMain, which will call the call() method after parsing 
			the args we give it.  */
		mainMethodID = (*env)->GetStaticMethodID(env, ofp_class, "main",
															  "([Ljava/lang/String;)V");
		if(mainMethodID == NULL)
		  handleException(jvm, env);
		else
       {
      // DQ (1/19/2008): Suggested fix by Jeremiah (in previous version of OFP) to permit the Java debugging to be used.
      // (*env)->CallStaticVoidMethod(env, new_ofp_class, mainMethodID, args);
         (*env)->CallStaticVoidMethod(env, ofp_class, mainMethodID, args);
       }
      
		/* Get the error status from the 'boolean getError()' method.  */
		errorMethodID = (*env)->GetStaticMethodID(env, ofp_class,
																"getError", "()Z");
		retval = (*env)->CallBooleanMethod(env, new_ofp_class, errorMethodID);
	 }
	 
	 /* We're done; destroy the Java VM.  */
	 (*jvm)->DestroyJavaVM(jvm);

	 /* Cleanup any memory we allocated.  */
	 free(classpath);

	 return (retval == JNI_TRUE ? ERROR_CODE : 0);
  }

  
  jstring getJavaString(JNIEnv *env, const char *string)
  {
	 /* Based on example from:
	  * http://java.sun.com/docs/books/jni/html/fldmeth.html#11202  */
	 jclass stringClass;
	 jmethodID consID;
	 jstring javaString;
	 jcharArray elemArray;

	 /* Get the java.lang.String class.  */
	 stringClass = getJavaStringClass(env);
	 if(stringClass == NULL)
		return NULL;

	 /* Get the String(char[]) constructor.  */
	 consID = (*env)->GetMethodID(env, stringClass, "<init>", "([C)V");
	 if(consID == NULL)
		return NULL;

	 /* Create the char[] that holds the chars in 'string'.  */
	 elemArray = (*env)->NewCharArray(env, strlen(string));
	 if(elemArray == NULL)
		return NULL;
	 (*env)->SetCharArrayRegion(env, elemArray, 0, strlen(string),
										 convertToJChar(string));

	 /* Create the java.lang.String object by invoking the given constructor:
		 String(char[]).  */
	 javaString = (*env)->NewObject(env, stringClass, consID, elemArray);

	 /* Free local references.  */
	 (*env)->DeleteLocalRef(env, elemArray);
	 (*env)->DeleteLocalRef(env, stringClass);

	 return javaString;
  }


  jobjectArray getJavaStringArray(JNIEnv *env, int argc, char **argv)
  {
	 jobjectArray argsStringArray = NULL;
	 jclass stringClass;
	 int i;

	 /* We need the String class because that is the underlying type of
		 the array.  */
	 stringClass = getJavaStringClass(env);
	 if(stringClass == NULL)
		return NULL;

	 /* Build a new object array.  Params are: env, length, class type of the 
		 array, initial object(?).  */
	 argsStringArray = (*env)->NewObjectArray(env, (argc-1), stringClass, NULL);
	 if(argsStringArray == NULL)
		return NULL;

	 /* Put all args from argv, after the first (which is this program's
		 name) into the array of Strings for FortranMain.  The args array
		 for Java does not include the program name.  */
	 for(i = 1; i < argc; i++)
		(*env)->SetObjectArrayElement(env, argsStringArray, (jsize)i-1,
												getJavaString(env, argv[i]));
	 
	 return argsStringArray;
  }


  jchar *convertToJChar(const char *c_string)
  {
	 int i;
	 int len;
	 jchar *jchar_string;

	 len = strlen(c_string);
	 jchar_string = malloc(sizeof(jchar)*len);

	 for(i = 0; i < len; i++)
		jchar_string[i] = (jchar)(*(c_string+i));

	 return jchar_string;
  }


  jclass getJavaStringClass(JNIEnv *env)
  {
	 return ((*env)->FindClass(env, "java/lang/String"));
  }


  void handleException(JavaVM *jvm, JNIEnv *env)
  {
	 if((*env)->ExceptionOccurred(env))
		(*env)->ExceptionDescribe(env);

	 /* Destroy the Java VM.  */
	 (*jvm)->DestroyJavaVM(jvm);

	 /* Exit since the exception should mean we can't recover.  */
	 exit(1);
  }

#ifdef __cplusplus
}/* end extern "C" */
#endif
  


