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

/* Based on examples/docs from:
 *	http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/invocation.html#wp9502
 * http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/jniTOC.html
 * http://java.sun.com/docs/books/jni/html/invoke.html 
 */
// Include rose.h so that we can reference SgSourceFile::build_classpath() 
#include "sage3basic.h"

#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "rose_config.h"

/* This is defined if ROSE is configured to use the Java Open Fortran Parser */
#ifndef USE_ROSE_JAVA_SUPPORT
#error "openFortranParser_main.C should not be compiled when Java support is disabled"
#endif

#include "jni.h"
#include "fortran_error_handler.h"

using namespace std;

// DQ (9/6/2010): Allow this to be commented out to simplify debugging using gdb.
// #define ENABLE_FORTRAN_ERROR_HANDLER

#define ERROR_CODE 4

#if 0 //FMZ
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

  jobjectArray getJavaStringArray(JNIEnv *env, int argc, char **argv);
  jclass getJavaStringClass(JNIEnv *env);
  void handleException(JavaVM *jvm, JNIEnv *env);
  void handleExceptionMaybe(JavaVM *jvm, JNIEnv *env);

#endif

int runOFP(int argc, char **argv);


/* Include the paths to the source tree and build tree for ROSE */
#include "rose_config.h"

#include "rose_paths.h"
#include "commandline_processing.h"
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
    string new_value = findRoseSupportPathFromBuild("src/frontend/OpenFortranParser_SAGE_Connection/.libs", "lib");

 /* Save the old value */
    const char* old_value = getenv(ROSE_SHLIBPATH_VAR); // Might be null

#if 0
    printf ("Old value: LD_LIBRARY_PATH = %s \n",old_value);
    printf ("New value: LD_LIBRARY_PATH = %s \n",new_value.c_str());
 /* This value can be empty if the configuration was done using a relative path, as I recall! */
    printf ("ROSE_AUTOMAKE_TOP_BUILDDIR = %s \n",ROSE_AUTOMAKE_TOP_BUILDDIR);
    printf ("ROSE_COMPILE_TREE_PATH     = %s \n",ROSE_COMPILE_TREE_PATH);
#endif

#if OVERWRITE_LD_LIBRARY_PATH
    int overwrite = 1;
    int env_status = setenv(ROSE_SHLIBPATH_VAR,new_value.c_str(),overwrite);
    assert(env_status == 0);
#endif

    if (SgProject::get_verbose() > 0)
       {
         printf ("Call the function that will start a JVM and call the OFP \n\n");

      // DQ (10/12/2010): Added debugging support.
         string JVM_command_line = CommandlineProcessing::generateStringFromArgList(CommandlineProcessing::generateArgListFromArgcArgv(argc, argv));
         printf ("Java JVM commandline = %s \n",JVM_command_line.c_str());
         printf ("ROSE modified %s = %s \n",ROSE_SHLIBPATH_VAR,new_value.c_str());
       }

#ifdef ENABLE_FORTRAN_ERROR_HANDLER
    fortran_error_handler_begin();
#endif

    int status = runOFP(argc, argv);

#ifdef ENABLE_FORTRAN_ERROR_HANDLER
    fortran_error_handler_end();
#endif

    if (SgProject::get_verbose() > 0)
       {
         printf ("JVM processing done.\n\n");
       }

 /* Reset to the saved value */
#if OVERWRITE_LD_LIBRARY_PATH
    env_status = setenv(ROSE_SHLIBPATH_VAR,old_value,overwrite);
    assert(env_status == 0);
#endif

	 return status;
  }

string getenvString(const string& key, const string& def = "") {
  const char* val = getenv(key.c_str());
  if (!val) return def;
  return val;
}
// FMZ (5/19/2008)
#include "jserver.h"
#include "ofp.h"

int runOFP(int argc, char **argv)
  {
#if 0 //FMZ (5/19/2008)
	 JavaVM *jvm;   /* The Java VM.  */
	 JNIEnv *env;   /* The environment for retrieving class objects, etc.  */
	 JavaVMInitArgs jvm_args;  /* VM initialization args.  */
	 jboolean retval = JNI_TRUE; // Error by default

#if 0
 // This needs to be setup anytime we call OFP, so it is setup outside of the OFP main function since OFP is frequently called directly via java (using the system command).
	 string classpath = "-Djava.class.path=";
         classpath += findRoseSupportPathFromBuild("/src/3rdPartyLibraries/fortran-parser/OpenFortranParser.jar", "lib/OpenFortranParser.jar") + ":";
         classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-2.7.7.jar", "lib/antlr-2.7.7.jar") + ":";
         classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-3.0.1.jar", "lib/antlr-3.0.1.jar") + ":";
         classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-runtime-3.0.1.jar", "lib/antlr-runtime-3.0.1.jar") + ":";
         classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/stringtemplate-3.1b1.jar", "lib/stringtemplate-3.1b1.jar") + ":";
         classpath += ".";
#else
    string classpath = SgSourceFile::build_classpath();
#endif

	 /* Set up the VM initialization args.  */
    jvm_args.version = JNI_VERSION_1_4;
#if 1
	 jvm_args.nOptions = 1;
	 jvm_args.options = new JavaVMOption[jvm_args.nOptions];
	 jvm_args.options[0].optionString = strdup(classpath.c_str());
#else
 // The class path is not passed in as input anymore
	 jvm_args.nOptions = 0;
	 jvm_args.options = NULL;
#endif
	 jvm_args.ignoreUnrecognized = JNI_FALSE;

	 /* Create and load the Java VM.  */
	 JNI_CreateJavaVM(&jvm, (void **)&env, &jvm_args);

         handleExceptionMaybe(jvm, env);

	 /* Get the FortranMain class.  */
	 jclass ofp_class = env->FindClass("fortran/ofp/FrontEnd");
	 if(ofp_class == NULL)
		handleException(jvm, env);

	 /* Get the constructor for FortranMain(String[] args, String fileName,
		 String type).  */
	 jmethodID cons_method_id =	env->GetMethodID
		(ofp_class, "<init>",
		 "([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	 if(cons_method_id == NULL)
		handleException(jvm, env);

	 /* Create a Java String[] out of argv (everything after the first arg).  */
	 jobjectArray args = getJavaStringArray(env, argc, argv);
         handleExceptionMaybe(jvm, env);

	 /* The filename of the Fortran source to parse MUST be the LAST
		 argument!  The setting of this and the type below are probably not
		 needed since we call main() and give it all args to parse itself.  */
	 jstring fileName = env->NewStringUTF(argv[argc-1]);
         handleExceptionMaybe(jvm, env);

	 jstring type = env->NewStringUTF("fortran.ofp.parser.java.FortranParserActionNull");
         handleExceptionMaybe(jvm, env);

	 if(fileName == NULL || args == NULL || type == NULL)
		handleException(jvm, env);
	 else
	 {
		jmethodID mainMethodID = NULL;
		jmethodID errorMethodID = NULL;
		
		/* Get a FortranMain by calling the constructor referred to by
			cons_method_id (String[] argc, String filename, String type).  */
		jobject parserObj = env->NewObject(ofp_class, cons_method_id, args, fileName, type);

                handleExceptionMaybe(jvm, env);

		/* Get the method ID for the main(String[] args) method in
			FortranMain, which will call the call() method after parsing 
			the args we give it.  */
		mainMethodID = env->GetStaticMethodID(ofp_class, "main",
															  "([Ljava/lang/String;)V");
		if(mainMethodID == NULL)
		  handleException(jvm, env);
		else
       {
      // DQ (1/19/2008): Suggested fix by Jeremiah (in previous version of OFP) to permit the Java debugging to be used.
      // env->CallStaticVoidMethod(env, parserObj, mainMethodID, args);
         env->CallStaticVoidMethod(ofp_class, mainMethodID, args);
         handleExceptionMaybe(jvm, env);

       }
      
		/* Get the error status from the 'boolean getError()' method.  */
		errorMethodID = env->GetStaticMethodID(ofp_class,
																"getError", "()Z");
                handleExceptionMaybe(jvm, env);
		retval = env->CallBooleanMethod(parserObj, errorMethodID);
                handleExceptionMaybe(jvm, env);
	 }
	 
	 /* We're done; destroy the Java VM.  */
	 jvm->DestroyJavaVM();

	 return (retval == JNI_TRUE ? ERROR_CODE : 0);

#else // FMZ (5/19/2008)
        int retval= 0;
        /* start/find the Java VM with ofp method loaded,run ofp on the args */
        retval = jvm_ofp_processing(argc, argv);
  
        return retval;
  }
#endif
  
#if 0  //FMZ (5/19/2008)
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
	 argsStringArray = env->NewObjectArray((argc-1), stringClass, NULL);
	 if(argsStringArray == NULL)
		return NULL;

	 /* Put all args from argv, after the first (which is this program's
		 name) into the array of Strings for FortranMain.  The args array
		 for Java does not include the program name.  */
	 for(i = 1; i < argc; i++)
		env->SetObjectArrayElement(argsStringArray, (jsize)i-1,
												env->NewStringUTF(argv[i]));
	 
	 return argsStringArray;
  }


  jclass getJavaStringClass(JNIEnv *env)
  {
	 return (env->FindClass("java/lang/String"));
  }

  void handleExceptionMaybe(JavaVM* jvm, JNIEnv* env) {
    if (env->ExceptionOccurred()) handleException(jvm, env);
  }


  void handleException(JavaVM *jvm, JNIEnv *env)
  {
	 if(env->ExceptionOccurred())
		env->ExceptionDescribe();

	 /* Destroy the Java VM.  */
	 jvm->DestroyJavaVM();

	 /* Exit since the exception should mean we can't recover.  */
	 exit(1);
  }
#endif

