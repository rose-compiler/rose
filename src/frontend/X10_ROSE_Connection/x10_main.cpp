// C++ JNI Wrapper around the x10 compiler
//
// Author: Justin Too <too1@llnl.gov>
// Date:   February 15, 2013
//
// This C++ application reproduces the Java invocation of the
// x10 compiler. Specifically, it reproduces the commandline
// created by the x10c script:
//
//   $ x10c HelloWholeWorld.x10
//
// This example command is generated:
//
//   /path/to/java
//       -Xmx1024m
//       -Dfile.encoding=UTF-8
//       -Dx10.dist=/path/to/x10-trunk/x10.dist
//       -Dx10c.ecj.jar=ecj.jar
//       -Dx10c.math.jar=commons-math3-3.0.jar
//       -classpath /path/to/x10-trunk/x10.dist/lib/x10c.jar
//                  /path/to/x10-trunk/x10.dist/lib/lpg.jar
//                  /path/to/x10-trunk/x10.dist/lib/com.ibm.wala.cast.java_1.0.0.201101071300.jar
//                  /path/to/x10-trunk/x10.dist/lib/com.ibm.wala.cast_1.0.0.201101071300.jar
//                  /path/to/x10-trunk/x10.dist/lib/com.ibm.wala.core_1.1.3.201101071300.jar
//                  /path/to/x10-trunk/x10.dist/lib/com.ibm.wala.shrike_1.3.1.201101071300.jar
//                  /path/to/x10-trunk/x10.dist/lib/x10wala.jar
//                  /path/to/x10-trunk/x10.dist/lib/org.eclipse.equinox.common_3.6.0.v20100503.jar
//       polyglot.main.Main
//       -extclass x10c.ExtensionInfo
//       -x10lib /path/to/x10-trunk/x10.dist/stdlib/libx10.properties
//       HelloWholeWorld.x10
//
// Note that only two of these commandline options are to be passed to
// polyglot.main.Main.main(String args[]):
//
//   -extclass x10c.ExtensionInfo
//   -x10lib /path/to/x10-trunk/x10.dist/stdlib/libx10.properties
//
// And, of course, the x10 input source code, HelloWholeWorld.x10.


//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------
#include "assert.h"
#include <iostream>
#include <jni.h>
#include <string>
#include <vector>

#include "rose_config.h" // X10_INSTALL_PATH

//-----------------------------------------------------------------------------
// Data Structures
//-----------------------------------------------------------------------------
struct JVM
{
  JavaVM          *jvm;      ///< denotes a Java VM
  JNIEnv          *env;      ///< pointer to native method interface
  JavaVMInitArgs  vm_args;   ///< JDK/JRE VM initialization arguments
};

struct Commandline
{
  std::vector <std::string> sources;
  std::string               jvm_max_heap;
  std::string               classpath;
  bool                      verbose;
};

//-----------------------------------------------------------------------------
// Method Declarations
//-----------------------------------------------------------------------------
static void
ShowUsage(std::string program_name);

struct JVM
CreateJvm(const struct Commandline a_cmdline);

int
Compile(const struct JVM a_jvm, const struct Commandline a_cmdline);

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
int
x10_main(int argc, char* argv[])
{
  std::string program_name(argv[0]);

  if (argc < 2)
  {
      ShowUsage(program_name);
      return 1;
  }
  else
  {
      struct Commandline cmdline;
      cmdline.jvm_max_heap = "1024"; // TODO: add -Xmx JVM option to ROSE

      for (int ii = 1; ii < argc; ++ii)
      {
          //std::cout
          //    << "[INFO] [x10_main] argv[" << ii << "] = "
          //    << argv[ii]
          //    << std::endl;

          std::string arg = argv[ii];
          if ((arg == "-h") || (arg == "--help"))
          {
              ShowUsage(program_name);
              return 0;
          }
          else if (arg.find("-Xmx") == 0)
          {
              arg.replace(0, 4, "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] [x10_main] -Xmx requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  cmdline.jvm_max_heap = arg;
              }
          }
          else if (arg.find("-classpath=") == 0)
          {
              arg.replace(0, 11, "");
              if (arg.empty())
              {
                  std::cerr
                      << "[ERROR] [x10_main] -classpath requires an argument, see --help"
                      << std::endl;
                  return 1;
              }
              else
              {
                  cmdline.classpath = arg;
              }
          }
          else if (arg.find("-verbose") == 0)
          {
              arg.replace(0, 8, "");
              if (arg.empty())
              {
                  cmdline.verbose = true;
              }
              else
              {
                  std::cerr
                      << "[ERROR] [x10_main] "
                      << "-verbose does not accept an argument, see --help"
                      << std::endl;
                  return 1;
              }
          }
          else if (arg.find("-") == 0)
          {
              std::cout
                  << "[WARN] [x10_main] "
                  << "Unknown commandline switch: "
                  << arg
                  << std::endl;
          }
          else
          {
              cmdline.sources.push_back(argv[ii]);
          }
      }

      struct JVM jvm = CreateJvm(cmdline);
      return Compile(jvm, cmdline);
  }
}

//-----------------------------------------------------------------------------
// method Definitions
//-----------------------------------------------------------------------------
static void
ShowUsage(std::string program_name)
{
  std::cerr
    << "Usage: " << program_name << " [--help] file.x10 [...]\n\n"
    << "Options:\n"
    << "  -Xmx<size>                  Specify the max heap memory size for the JVM (default: 1024m)\n"
    << "  -classpath=<path>           Specify the Java CLASSPATH\n"
    << "  -x10.dist=<path>            Specify path to your X10 installation (x10-trunk/x10.dist)\n"
    << "\n"
    << "  -h,--help                   Show this help message\n"
    << "  -verbose                    Enables verbose output\n"
    << std::endl;
}

struct JVM
CreateJvm(const struct Commandline a_cmdline)
{
  struct JVM jvm;

  JavaVMOption* options = new JavaVMOption[6];
  std::string jvm_max_heap = "-Xmx" + a_cmdline.jvm_max_heap;
  std::string file_encoding = "-Dfile.encoding=UTF-8";
  std::string x10_dist = "-Dx10.dist=" + std::string(X10_INSTALL_PATH);
  std::string x10c_ecj_jar = "-Dx10c.ecj.jar=ecj-4.3.2.jar";
  std::string x10c_math_jar = "-Dx10c.math.jar=commons-math3-3.2.jar";
  std::string classpath =
      "-Djava.class.path=" +
      std::string(X10_INSTALL_PATH) + "/lib/x10c.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/lpg.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/com.ibm.wala.cast.java_1.0.0.201101071300.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/com.ibm.wala.cast_1.0.0.201101071300.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/com.ibm.wala.core_1.1.3.201101071300.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/com.ibm.wala.shrike_1.3.1.201101071300.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/x10wala.jar" + ":" +
      std::string(X10_INSTALL_PATH) + "/lib/org.eclipse.equinox.common_3.6.0.v20100503.jar";
  std::string libpath = "-Djava.library.path=" + std::string(ROSE_INSTALLATION_PATH) + "/lib";

  options[0].optionString = (char*) file_encoding.c_str();
  options[1].optionString = (char*) x10_dist.c_str();
  options[2].optionString = (char*) x10c_ecj_jar.c_str();
  options[3].optionString = (char*) x10c_math_jar.c_str();
  options[4].optionString = (char*) classpath.c_str();
  options[5].optionString = (char*) libpath.c_str();
  //options[5].optionString = (char*) jvm_max_heap.c_str();
  //options[6].optionString = "-verbose:jni"; // print JNI-related messages

  jvm.vm_args.version = JNI_VERSION_1_6;
  jvm.vm_args.nOptions = 6;
  jvm.vm_args.options = options;
  jvm.vm_args.ignoreUnrecognized = true;

  // load and initialize a Java VM, return a JNI interface
  // pointer in env
  int ret = JNI_CreateJavaVM(&jvm.jvm, (void**)&jvm.env, &jvm.vm_args);
  if (ret < 0)
  {
      assert(! "[FATAL] [x10_main] Unable to Launch JVM");
  }

  delete options;

  return jvm;
}

int
Compile(const struct JVM a_jvm, const struct Commandline a_cmdline)
{
  std::vector <std::string> sources_vector = a_cmdline.sources;

  if (a_cmdline.verbose)
  {
      std::cout
          << "[INFO] [x10_main] Compiling "
          << sources_vector.size()
          << " file/s with x10c"
          << std::endl;
  }

  JavaVM *jvm = a_jvm.jvm;
  JNIEnv *env = a_jvm.env;

  std::string class_name = "polyglot/main/Main";
  std::string method_name = "main";
  std::string method_signature = "([Ljava/lang/String;)V";

  // Get handle on "Class polyglot.main.Main"
  jclass clazz = env->FindClass(class_name.c_str());
  if(env->ExceptionCheck())
  {
      // ClassNotFoundException
      env->ExceptionDescribe();
      env->ExceptionClear();
      return 2;
  }

  // Get handle on "public static void main(String args[])"
  jmethodID method =
      env->GetStaticMethodID(
          clazz,
          method_name.c_str(),
          method_signature.c_str());
  if(env->ExceptionCheck())
  {
      // NoSuchMethodError
      env->ExceptionDescribe();
      env->ExceptionClear();
      std::cerr
          << "[ERROR] [x10_main] Could not find method '"
          << method_name.c_str()
          << "' for class='"
          << class_name.c_str()
          << "'"
          << std::endl;
      return 2;
  }

  // Create commandline arguments to pass to
  // polyglot.main.Main.main(String args[])

  // -extclass
  jstring extclass = env->NewStringUTF("x10rose.ExtensionInfo");
  if (extclass == NULL)
  {
      std::cerr << "[ERROR] [x10_main] extclass is NULL" << std::endl;
      return 2;
  }

  // -x10lib
  jstring x10lib = env->NewStringUTF(
      (std::string(X10_INSTALL_PATH) + "/stdlib/libx10.properties").c_str());
  if (x10lib == NULL)
  {
      std::cerr << "[ERROR] [x10_main] x10lib is NULL" << std::endl;
      return 2;
  }

  // Create a Java String Array Primitive to hold the
  // commandline arguments for polyglot.main.Main.main(String args[])
  jclass stringClass = env->FindClass("java/lang/String");
  jobjectArray args =
      env->NewObjectArray(
          4 + sources_vector.size(),
          stringClass,
          NULL);
  if (args == NULL)
  {
      std::cerr
          << "[ERROR] [x10_main] "
          << "Could not create String Array for Polyglot commandline"
          << std::endl;
      return 2;
  }

  // Add the Polyglot commandline arguments
  env->SetObjectArrayElement(args, 0, env->NewStringUTF("-extclass"));
  env->SetObjectArrayElement(args, 1, extclass);
  env->SetObjectArrayElement(args, 2, env->NewStringUTF("-x10lib"));
  env->SetObjectArrayElement(args, 3, x10lib);
  for (size_t ii = 0; ii < sources_vector.size(); ++ii)
  {
      std::string filename = sources_vector[ii];
      jstring j_filename = env->NewStringUTF(filename.c_str());
      if (j_filename == NULL)
      {
          std::cerr
              << "[ERROR] [x10_main] "
              << "j_filename is NULL for filename="
              << "'" << filename << "'"
              << std::endl;
          return 2;
      }
      env->SetObjectArrayElement(args, 4 + ii, j_filename);
  }

  // Call the method!
  if (a_cmdline.verbose)
  {
      std::cout
          << "[INFO] [x10_main] Calling "
          << class_name.c_str()
          << "."
          << method_name.c_str()
          << std::endl;
  }

  env->CallStaticVoidMethod(clazz, method, args);

  if(env->ExceptionCheck())
  {
      env->ExceptionDescribe();
      env->ExceptionClear();
      return 2;
  }

  jvm->DestroyJavaVM();

  if (a_cmdline.verbose)
      std::cout << "[INFO] [x10_main] Finished executing x10c" << std::endl;

  return 0;
}

