import org.eclipse.jdt.internal.compiler.batch.*;

import java.io.*;
import java.text.*;
import java.util.*;

import java.util.ArrayList;

import org.eclipse.jdt.core.compiler.*;
import org.eclipse.jdt.core.compiler.batch.*;
import org.eclipse.jdt.internal.compiler.*;
import org.eclipse.jdt.internal.compiler.Compiler;
import org.eclipse.jdt.internal.compiler.env.*;
import org.eclipse.jdt.internal.compiler.impl.*;
import org.eclipse.jdt.internal.compiler.ast.*;
import org.eclipse.jdt.internal.compiler.lookup.*;
import org.eclipse.jdt.internal.compiler.parser.*;
import org.eclipse.jdt.internal.compiler.problem.*;
import org.eclipse.jdt.internal.compiler.util.*;

import org.eclipse.jdt.internal.compiler.classfmt.ClassFileConstants;

// import JavaParser.*;

// DQ (10/12/2010): Make more like the OFP implementation (using Callable<Boolean> abstract base class). 
// class JavaTraversal {

import java.util.concurrent.Callable;
class JavaTraversal implements Callable<Boolean> {
    // The function JavaTraversal::main() is what is called using the JVM from ROSE.

    // This class generates a DOT file of the ECJ AST.  It is not used for any ROSE specific translation
    // of the AST.  As a result it should be renamed.

// TODO: remove this !!!
// Not clear what this means!
// static Main main;
    static BufferedWriter out;

    static int verboseLevel = 0;

    // -------------------------------------------------------------------------------------------
    /* tps: Invoke C Code , the int nr represents a unique nr for a node which is used for DOT representation*/
    private native void invokeINIT();
    private native void invokeEND ();
    private native void invokeNODE(String className, int nr);
    private native void invokeEDGE(String className1, int nr, String className2, int nr2);

    // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
    // private static boolean hasErrorOccurred = false;
    public static boolean hasErrorOccurred = false;

    static {
        System.loadLibrary("JavaTraversal");
    }

    // -------------------------------------------------------------------------------------------
    /* tps: Creating a hashmap to keep track of nodes that we have already seen. Needed for DOT graph */
    static HashMap<Integer, Integer> hashm = new HashMap<Integer, Integer>();
    static int hashcounter = 0;

    // -------------------------------------------------------------------------------------------
    /* tps: A class is represented as x.y.z.f.g --> this function returns the last string after the last dot */
    public String getLast(String s) {
        int index = s.lastIndexOf(".");
        if (index == -1)
            return s;
        return s.substring(index + 1, s.length());
    }

    // -------------------------------------------------------------------------------------------
    /* tps: For any Node we print its String representation. This function returns up to 30 characters of the String for DOT */
    public String getLine(ASTNode current) {
        int length = current.toString().length();
        int pos = current.toString().indexOf("\n");
        if (pos != -1)
            length = pos - 1;
        if (length > 30)
            length = 30;
        String substr = (current.toString()).substring(0, length); 
        return substr.replace("\"", "'");
    } 

    // -------------------------------------------------------------------------------------------
    /* tps: Add nodes into a HashMap. We need this to connect edges between Nodes for DOT */
    public int getHashValue(ASTNode current) {
        int valc = 0;
        Integer hashc = (Integer) hashm.get(current.hashCode()); 
        if (hashc != null) {
            valc = (hashc).intValue();
        }
        else {
            valc = hashcounter;
            hashm.put(current.hashCode(), hashcounter++);
        }
        return valc;
    }

    // -------------------------------------------------------------------------------------------
    /* tps : print to DOT   */
    public void printToDOT(ASTNode current) {
        ASTNode before=null;
        if (! stack.empty())
            before = (ASTNode)stack.peek();
        try {
            if (before == null) {
                // System.out.println("---------------------------------------------");
                // System.out.println("******** Root Node: "+getLast(current.getClass().toString())+"   Adding node to hashmap: "+hashcounter);
                int valc = getHashValue(current);
                out.write(" \""+valc+getLast(current.getClass().toString())+"\" [label=\""+getLast(current.getClass().toString())+"\\n"+getLine(current) +"\"];\n");
                invokeNODE(getLast(current.getClass().toString()), valc);
            }
            else {
                // System.out.println("-----------------------");
                // System.out.println("New Node: "+getLast(current.getClass().toString()));
                // System.out.println("New Edge: "+getLast(before.getClass().toString())+" -- "+getLast(current.getClass().toString()));
                Integer valbint = ((Integer)hashm.get(before.hashCode()));
                if (valbint == null) {
                    System.err.println("Error : hashvalue valbint == null for node :" + before.getClass()); 
                    System.exit(1); 
                }
                int valb = valbint.intValue();
                int valc = getHashValue(current);
                out.write(" \"" + valc + "" + getLast(current.getClass().toString()) +
                         "\" [label=\"" + getLast(current.getClass().toString()) + "\\n" + getLine(current) + "\"];\n");
                out.write("    \"" + valb + "" + getLast(before.getClass().toString()) + "\" -> \"" + valc + "" + getLast(current.getClass().toString()) + "\";\n");
                invokeEDGE(getLast(before.getClass().toString()), valb, getLast(current.getClass().toString()), valc);
                invokeNODE(getLast(current.getClass().toString()), valc);
            }

        }
        catch (Exception e) {
            e.printStackTrace();
            System.err.println("Error: " + e.getMessage());
        }
    }

    // -------------------------------------------------------------------------------------------
    /* tps: Stack that is keeping track of the traversal we perform to connect children with parents in the DOT graph */
    private Stack<ASTNode> stack = new Stack<ASTNode>();

    public void pushNode(ASTNode node) {
        stack.push(node);
    }

    public ASTNode popNode() {
        if (! stack.empty())
            stack.pop();
        else {
            System.err.println("!!!!!!!!!!!! ERROR trying to access empty stack");
            System.exit(1);
        }

        if (! stack.empty())
            return (ASTNode)stack.peek();

        return null;
    }


    // Added support for command line processing to set the verbose level (passed directly from ROSE "--rose:verbose n").
    public static String [] filterCommandline(String args[]) {
        int commandlineErrorLevel = 0;

        if (commandlineErrorLevel > 0)
            System.out.println("Processing the command line in ECJ/ROSE connection ...");

        String argsForECJ[];
        // int ROSE_veboseLevel = 0;

        // ArrayList<String> argsList = CreateStringList(args);
        ArrayList<String> argsList = new ArrayList<String>();
        Collections.addAll(argsList, args);

        // Output the arguments from the command line.
        // for (String arg: args)
        int max = args.length;
        for (int j = 0; j < max; j++) {
             if (commandlineErrorLevel > 0)
                 System.out.println("ROSE Argument found: " + args[j]);

            // String matchingString = "-rose";
            // Java substring uses index 0 ... 5 (the upper bound "6" is not used.
            // System.out.println("     substring = " + arg.substring(0,6) + " matchingString = " + matchingString);
            // if (arg.substring(0,6) == matchingString.substring(0,6))
            if (args[j].startsWith("-rose:verbose") == true) {
                if (commandlineErrorLevel > 0)
                    System.out.println("Clear this ROSE specific argument #" + j + ": " + args[j]);

                // Remove the entry from the list
                argsList.remove(j);

                // String veboseLevelString = args[j+1];
                String veboseLevelString = args[j].substring(14,args[j].length());

                if (commandlineErrorLevel > 0)
                    System.out.println("Grab the integer values verbose level: " + veboseLevelString);

                try {
                    // Set the class level JavaTraversal.veboseLevel data member (convert the String to an integer).
                    verboseLevel = Integer.parseInt(veboseLevelString.trim());

                    // print out the value after the conversion
                    if (verboseLevel > 0 || commandlineErrorLevel > 0)
                        System.out.println("integer value = " + verboseLevel);
                }
                catch (NumberFormatException nfe) {
                    nfe.printStackTrace();
                    System.out.println("NumberFormatException: " + nfe.getMessage());

                    // It might be better to rethrow the exception
                    System.out.println("Error: -rose:verbose option specified with out an integer value: veboseLevelString = " + veboseLevelString);
                    System.exit(1);
                }
            }
            else {
                if (commandlineErrorLevel > 0)
                    System.out.println("Not a matching ROSE option: " + args[j]);
            }
        }

        if (commandlineErrorLevel > 0)
            System.out.println("Done with output of command line arguments. ");

        // Rebuild the array from the edited list.
        // args = ConvertToStringArray(argsList);
        args = (String[])argsList.toArray(new String[0]);

        if (commandlineErrorLevel > 0) {
            for (String arg : args) {
                System.out.println("ROSE Argument found (after removing ROSE options): " + arg);
            }
            System.out.println("Done with output of command line arguments (after removing ROSE options). ");
        }

        return args;
    }

    /**
     * Compile the source files specified and all its dependent source files and generate
     * all the associatd class files.
     *  
     * @param args
     */
    static boolean compile(String args[]) {
        if (verboseLevel > 0)
            System.out.println("Compiling ...");

        // This line of code will run, but the first use of "main" fails ...working now!
        Main main = new Main(new PrintWriter(System.out), new PrintWriter(System.err), true/*systemExit*/,  null/*options*/, null/*progress*/);

        //
        try {
            main.configure(args);
        }
        catch (Exception e) {
            e.printStackTrace();
            System.err.println("Error in main.configure(args): " + e.getMessage()); 
            System.exit(1);
        }
        main.compilerOptions = new CompilerOptions(main.options);
        main.compilerOptions.performMethodsFullRecovery = false;
        main.compilerOptions.performStatementsRecovery = false;
        // main.compilerOptions.verbose = true;
        main.compilerOptions.sourceLevel = ClassFileConstants.JDK1_7;
        main.compilerOptions.generateClassFiles = true;

        main.batchCompiler =  new Compiler(main.getLibraryAccess(),
                                           main.getHandlingPolicy(),
                                           main.compilerOptions,
                                           main.getBatchRequestor(),
                                           main.getProblemFactory(),
                                           null,
                                           main.progress
                                          );

        ICompilationUnit[] sourceUnits = main.getCompilationUnits();
        if (verboseLevel > 2)
            System.out.println("We got " + sourceUnits.length + " compilation units");

        main.batchCompiler.compile(sourceUnits); // generate all class files that are needed for these source units.

        return main.globalErrorsCount == 0; // return whether or not the compilation was successful.
    }

    /**
     * Compile and generate an AST for each input file specified in args.
     * 
     * @param args
     */
    static Main generateAst(String args[]) {
        Main main = new Main(new PrintWriter(System.out), new PrintWriter(System.err), true/*systemExit*/,  null/*options*/, null/*progress*/);

        // This is the last message printed to the console ...
        if (verboseLevel > 0)
            System.out.println("(2) test 1 .... (note call to main.configure(args); fails silently)");

        // DQ (4/1/2011): Added try...catch to debug command line handling.
        // We want to support the ECJ command line options where possible.
        // This line of code will fail when run with unknown arguments...working now only with just the filename as an argument!
        // main.configure(args);
        try {
            main.configure(args);
        }
        catch (Exception e) {
            e.printStackTrace();
            System.err.println("(2) Error in main.configure(args): " + e.getMessage()); 
            System.exit(1);
        }

        main.compilerOptions = new CompilerOptions(main.options);
        main.compilerOptions.performMethodsFullRecovery = false;
        main.compilerOptions.performStatementsRecovery = false;
     // main.compilerOptions.verbose = true;
        main.compilerOptions.sourceLevel = ClassFileConstants.JDK1_7;

        main.batchCompiler =  new Compiler(main.getLibraryAccess(),
                                           main.getHandlingPolicy(),
                                           main.compilerOptions,
                                           main.getBatchRequestor(),
                                           main.getProblemFactory(),
                                           null,
                                           main.progress
                                          );

        /* tps : handle compilation units--------------------------------------------- */
        ICompilationUnit[] sourceUnits = main.getCompilationUnits();
        int maxUnits = sourceUnits.length;

        main.batchCompiler.totalUnits = 0;
        main.batchCompiler.unitsToProcess = new CompilationUnitDeclaration[maxUnits];
        main.batchCompiler.internalBeginToCompile(sourceUnits, maxUnits);

        try {
            // writing to the DOT file
            FileWriter fstream = new FileWriter("ast.dot");
            out = new BufferedWriter(fstream);
            out.write("Digraph G {\n");
        }
        catch (Exception e) {
            e.printStackTrace();
            System.err.println("Error: " + e.getMessage()); 
        }

        return main;
    }


    // This is the "main" function called from the outside (via the JVM from ROSE).
    public static void main(String args[]) {
        /* tps : set up and configure ---------------------------------------------- */

        // Filter out ROSE specific options.
        args = filterCommandline(args);

        //
        // Generate all necessary classfiles if no errors are detected
        //
        if (! compile(args)) { // errors are detected?
            System.out.flush();
            System.err.flush();

            System.err.println();
            System.err.println();
            System.err.println("ECJ front-end errors detected in input java program");
            System.exit(1);
        }
        
        Main main = generateAst(args); // get compiler to generate AST.
        Compiler batchCompiler = main.batchCompiler; // get compiler to generate AST.
        int maxUnits = main.getCompilationUnits().length;
        
        // Calling the parser to build the ROSE AST from a traversal of the ECJ AST.
        try {
            if (verboseLevel > 2)
                System.out.println("test 7 ...");

            JavaParser.cactionCompilationUnitList(maxUnits, args);

            if (verboseLevel > 2)
                System.out.println("test 8 ...");

            //
            // What is the class path for these compilation units?
            //
            String classpath = "";
            for (int i = 0; i < args.length; i++) {
                if (args[i].equals("-classpath") || args[i].equals("-cp")) {
                    classpath = args[i+1];
                    break;
                }
            }
            JavaParserSupport java_parser_support = new JavaParserSupport(classpath, verboseLevel);

            //
            // Note the original commented out limit.  This is wrong!  We only want to process
            // the source files that were specified by the user and not the ones that were 
            // pulled in by closure.
            //
            for (int i = 0; i < maxUnits /*batchCompiler.totalUnits*/; i++) {
                CompilationUnitDeclaration unit = batchCompiler.unitsToProcess[i];
                assert(unit != null);

                if (verboseLevel > 2)
                    System.out.println("calling batchCompiler.process(unit, i) ..." + new String(unit.getFileName()));

                    batchCompiler.process(unit, i);

                if (unit.compilationResult.hasSyntaxError || unit.compilationResult.hasErrors()) {
                    System.out.flush();
                    System.err.flush();

                    System.err.println();
                    System.err.println();
                    System.err.println("ECJ front-end errors detected in input java program");
                    System.exit(1);
                }

                java_parser_support.preprocess(unit);
            }

            //
            // We only process the main unit to prevent the Unparser from generating multiple
            // compilation units in the same file.
            //
            /* for ( */ int i = 0; /* i < main.batchCompiler.totalUnits; i++)*/
            {
                CompilationUnitDeclaration unit = batchCompiler.unitsToProcess[i];
                try {
                    // **************************************************
                    // This is where the traveral of the ECJ AST is done.
                    // **************************************************
                    java_parser_support.translate(unit);

                    if (verboseLevel > 2)
                        System.out.println("test 10 ...");
                }
                catch (Exception e) {
                    e.printStackTrace();
                    System.err.println("Error in JavaTraversal::main() (nested catch before finally): " + e.getMessage());

                    // This should output the call stack.
                    System.err.println("Error in JavaTraversal::main() (nested catch before finally): " + e);
                }
                finally {
                    // cleanup compilation unit result
                    unit.cleanUp();
                }
                batchCompiler.unitsToProcess[i] = null; // release reference to processed unit declaration
                batchCompiler.stats.lineCount += unit.compilationResult.lineSeparatorPositions.length;
                batchCompiler.requestor.acceptResult(unit.compilationResult.tagAsAccepted());
            }
        }
        catch (Exception e) {
            // DQ (11/1/2010): Added more aggressive termination of program...
            e.printStackTrace();
            System.err.println("Error in JavaTraversal::main(): " + e.getMessage());
            // System.exit(1);

            hasErrorOccurred = true;
            return;
        }

        JavaParser.cactionCompilationUnitListEnd();

// TODO: REMOVE THIS !
//        jt.invokeEND();
        try {
            // closing the DOT file
            out.write("}\n");
            out.close();
        } 
        catch (Exception e) {
            e.printStackTrace();
            System.err.println("Error: " + e.getMessage());
        }

        if (verboseLevel > 2)
            System.out.println("Done compiling");
    }

    // DQ (10/12/2010): Implemented abstract baseclass "call()" member function (similar to OFP).
    public Boolean call() throws Exception {
        if (verboseLevel > 0)
            System.out.println("Parser exiting normally");

        // return new Boolean(error);
        return Boolean.TRUE;
    }// end call()

    // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
    public static boolean getError() {
        return hasErrorOccurred;
    }
}