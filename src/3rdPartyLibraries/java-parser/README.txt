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

Makefile.am:
   The Automake/autoconf build system file.

CMakeLists.txt:
   This is the required CMake file to support the CMake build process.


An undistributed directory is ECJ
   This contains the directory: org.eclipse.jdt.core
In org.eclipse.jdt.core building the ecj.jar file is
done using "ant".

Note that the build.xml file was changed to uses javac 1.5, specifically:
				source="1.5"
				target="1.5"


