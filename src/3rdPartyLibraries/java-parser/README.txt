This directory has the code for the Java parser and front-end support used in ROSE.
The most relevent part of this is the ECJ Java compiler, of which we use the frontend
and then provide a translation of the ECJ AST into that used by ROSE.

ecjROSE.jar:
   The jar file of the ECJ Java compiler which we use as a parser and frontend.
   The jar file can be extracted to the modified EDJ source cdoe used in ROSE.
   There are only a few trivial modifications, we have atempted to use ECJ as is
   to simplify how we can update the Java langauge support in ROSE. ECJ is 
   available on the web at: 
        http://ftp.de.debian.org/debian/pool/main/e/ecj/ecj_3.5.1.orig.tar.gz
   The Use of ECJ is made available under the Eclipse Public License.

JavaTraversal.java:
   The Java application which traverses the ECJ specific AST and makes the JNI calls
   which we implment in ROSE to support the translation of the ECJ AST to the ROSE AST.

JavaParser.java:
   This is the new home of the Java support, it is currently called by JavaTraversal
   but it will be called directly in the near future.  It implementat the support for
   the translation of the ECJ AST to the ROSE AST.

JavaParserSupport.java
   This file contains the static functions used to support the JavaParser work in ROSE.

ecjASTVisitor.java:
   This class is derived from the abstract class ASTVisitor in ECJ and all of it's
   methods are defined so that we can do the analysis (in Java) and cll the functions
   in JNI (implemented in C++) to construct the ROSE AST.  The implementation in C++
   of the JNI functions is located in the ROSE/src/frontend/ECJ_ROSE_Connection directory.

Makefile.am:
   The Automake/autoconf build system file.

CMakeLists.txt:
   This is the required CMake file to support the CMake build process.  CMake is
   used for Windows Visual C++ compilation of ROSE, but this has not been tested with the
   Java language support.

An undistributed directory is ECJ
   This contains the directory: org.eclipse.jdt.core. In org.eclipse.jdt.core, building
   the ecjROSE.jar file is done using "ant" (just type "ant" in the top level 
   org.eclipse.jdt.core directory.


Changes to ECJ:
   1) Note that the permsion of a class was changed to support the use of ECJ within ROSE.

   2) Note that the build.xml file was changed to uses javac 1.5, specifically:
				source="1.5"
				target="1.5"
      Note that this change might not be required now.




Modifier support for Java in ROSE:

   These are the modified required to be supported in ROSE for Java.

                    Modifiers-Elements Matrix in Java element 	

Modifier Data field 	Method 	Constructor 	     Class            Interface
 	                                        top level 	nested  	top level nested
                                             (outer)  (inner)   (outer)  (inner)

abstract      no        yes 	       no 	        yes 	  yes 	   yes 	   yes
final        yes        yes          no           yes      yes        no       no
native 	     no        yes          no            no       no        no       no
private      yes        yes         yes            no      yes        no      yes
protected    yes        yes         yes            no      yes        no      yes
public       yes        yes         yes           yes      yes       yes      yes
static       yes        yes          no            no      yes        no      yes
synchronized  no        yes          no            no       no        no       no
transient    yes         no          no            no       no        no       no
volatile     yes         no          no            no       no        no       no
strictfp      no        yes          no           yes      yes       yes      yes


*abstract:     Should be supported in declaration modifier (already in type modifier for F90)
final:        Already supported in declaration modifier (used for PHP, but used similarly)
*native:       Part if JNI interface specification; should be part of function modifier.
private:      Already in access modifier.
protected:    Already in access modifier.
public:       Already in access modifier.
static:       Already in storage modifier
*synchronized: Should be added to function modifier
*transient:    Implies that the variable should not be serialized. Should be added to const-volatile modifier
volatile:     Already in const-volatile modifier
*strictfp:     Should be added to function modifier






