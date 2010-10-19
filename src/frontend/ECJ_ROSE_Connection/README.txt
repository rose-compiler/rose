***********************************************************
** README For ECJ/JINI Traversal Hookup to ROSE
** tps (10/08/10)
***********************************************************

FILES:
- JavaTraversal.java  (main traversal java component)
- HelloWorld.java and HelloWorld2.java (test code)
- test.sh (test script which shows each step from compiling java code + jni stuff)
- JavaTraversal.c  (C++ implementation that is called from java via JNI)
- README


PREREQUISITES:
- Make the following change to ECJ: org/eclipse/jdt/internal/compiler/Compiler.java
  protected void internalBeginToCompile --> change protected to public

HOW TO RUN: type ./test.sh

WHAT IT DOES:
- Compile Java code: javac JavaTraversal.java
- javah -jni JavaTraversal: Create the JNI Header for C++
- Create C++ library: g++ -fPIC -shared  JavaTraversal.c -o libJavaTraversal.so   // here we would need to link against ROSE or find a better way.
- RUN the traversal: java -Djava.library.path=. JavaTraversal HelloWorld.java HelloWorld2.java
- Test output: java -Djava.library.path=. HelloWorld 

MORE IN DETAIL:
- JavaTraversal will traverse the input AST and create a ast.dot file which is the DOT representation of the AST
- IT ALSO will call the JNI interfaces (marked as native in JavaTraversal.java) and create a astC.dot graph on the C side.
  BOTH ASTs must be identical.

ALGORITHM:
- I use the AST traversal and the visit and endvisit functions from ECJ. Whenever we visit a node we push it on the stack and print it (DOT).
  When we leave the scope of a node we pop it from the stack and whatever is on the top of the stack is the parent node.
  When we call the print function, we check if a parent is present, if it is null we know it is the root, else we print a node and an edge.

