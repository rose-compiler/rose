echo ">>> Deleting class files"
rm *.class *.dot *.h ecjROSE.jar
echo ">>> Modify org/eclipse/jdt/internal/compiler/Compiler.java : Make internalBeginToCompile public"
sed -i 's/protected void internalBeginToCompile/public void internalBeginToCompile/g' org/eclipse/jdt/internal/compiler/Compiler.java
echo ">>> Create jar file"
jar -cf ecjROSE.jar org/ 
echo ">>> Compiling JavaTraversal.java: javac JavaTraversal.java"
javac -cp ecjROSE.jar JavaTraversal.java 
echo ">>> Creating JavaTraversal.h file"
javah -jni JavaTraversal
echo ">>> Compiling JavaTraversal.c"
g++ -fPIC -shared  JavaTraversal.c -o libJavaTraversal.so
echo ">>> Running JavaTraversal: java JavaTraversal HelloWorld.java HelloWorld2.java"
java -Djava.library.path=. -cp ecjROSE.jar:. JavaTraversal HelloWorld.java HelloWorld2.java
echo ">>> Running compiled file: java HelloWorld"
java -Djava.library.path=. HelloWorld