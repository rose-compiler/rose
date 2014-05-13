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

import java.util.concurrent.Callable;

class JavaTraversal implements Callable<Boolean> {
    // The function JavaTraversal::main() is what is called using the JVM from ROSE.

    static {
        System.loadLibrary("JavaTraversal");
        ClassLoader.getSystemClassLoader().setDefaultAssertionStatus(true);
    }

    public static int verboseLevel = 0;
    static HashSet<String> processedFiles = new HashSet<String>();
    static JavaParserSupport javaParserSupport = null;

    // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
    // private static boolean hasErrorOccurred = false;
    public static boolean hasErrorOccurred = false;

    public static String languageLevel(long level) {
        String language_level = "";

        if (level == ClassFileConstants.JDK1_1)
            language_level = "JDK1_1";
        else if (level == ClassFileConstants.JDK1_2)
            language_level = "JDK1_2";
        else if (level == ClassFileConstants.JDK1_3)
            language_level = "JDK1_3";
        else if (level == ClassFileConstants.JDK1_4)
            language_level = "JDK1_4";
        else if (level == ClassFileConstants.JDK1_5)
            language_level = "JDK1_5";
        else if (level == ClassFileConstants.JDK1_6)
            language_level = "JDK1_6";
        else if (level == ClassFileConstants.JDK1_7)
            language_level = "JDK1_7";
        else
            language_level = "???";

        return language_level;
    }

    // Added support for command line processing to set the verbose level (passed directly from ROSE "--rose:verbose n").
    public static String[] filterCommandline(String args[]) {
        int commandlineErrorLevel = 0;

        if (commandlineErrorLevel > 0)
            System.out.println("Processing the command line in ECJ/ROSE connection ...");

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

                String verboseLevelString = args[j].substring(14, args[j].length());

                if (commandlineErrorLevel > 0)
                    System.out.println("Grab the integer values verbose level: " + verboseLevelString);

                try {
                    // Set the class level JavaTraversal.veboseLevel data member (convert the String to an integer).
                    verboseLevel = Integer.parseInt(verboseLevelString.trim());

                    // print out the value after the conversion
                    if (verboseLevel > 0 || commandlineErrorLevel > 0)
                        System.out.println("integer value = " + verboseLevel);
                }
                catch (NumberFormatException nfe) {
                    nfe.printStackTrace();
                    System.out.println("NumberFormatException: " + nfe.getMessage());

                    // It might be better to rethrow the exception
                    System.out.println("Error: -rose:verbose option specified with out an integer value: veboseLevelString = " + verboseLevelString);
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
        args = (String[]) argsList.toArray(new String[0]);

        if (commandlineErrorLevel > 0) {
            for (String arg : args) {
                System.out.println("ROSE Argument found (after removing ROSE options): " + arg);
            }
            System.out.println("Done with output of command line arguments (after removing ROSE options). ");
        }

        return args;
    }

    /**
     * This method was copied from Compiler.java as it is not directly accessible there.
     */
    static protected synchronized void addCompilationUnit(Compiler compiler, ICompilationUnit sourceUnit, CompilationUnitDeclaration parsedUnit) {
        if (compiler.unitsToProcess == null)
            return; // not collecting units

        // append the unit to the list of ones to process later on
        int size = compiler.unitsToProcess.length;
        if (compiler.totalUnits == size)
            // when growing reposition units starting at position 0
            System.arraycopy(compiler.unitsToProcess,
                             0,
                             (compiler.unitsToProcess = new CompilationUnitDeclaration[size * 2]),
                             0, compiler.totalUnits);
        compiler.unitsToProcess[compiler.totalUnits++] = parsedUnit;
    }

    /**
     * This method was copied from Compiler.java as it is not directly accessible there.
     */
    static protected void internalBeginToCompile(Compiler compiler, ICompilationUnit[] sourceUnits, int maxUnits) {
        if (!compiler.useSingleThread && maxUnits >= ReadManager.THRESHOLD)
            compiler.parser.readManager = new ReadManager(sourceUnits, maxUnits);

        // Switch the current policy and compilation result for this unit to the requested one.
        for (int i = 0; i < maxUnits; i++) {
            CompilationResult unitResult = null;
            try {
                /*
                if (compiler.options.verbose) {
                    compiler.out.println(
                        Messages.bind(Messages.compilation_request,
                        new String[] {
                            String.valueOf(i + 1),
                            String.valueOf(maxUnits), new
                            String(sourceUnits[i].getFileName()) 
                        }));
                }
                */

                // diet parsing for large collection of units
                CompilationUnitDeclaration parsedUnit;
                unitResult = new CompilationResult(sourceUnits[i], i, maxUnits, compiler.options.maxProblemsPerUnit);
                long parseStart = System.currentTimeMillis();
                if (compiler.totalUnits < compiler.parseThreshold) {
                    parsedUnit = compiler.parser.parse(sourceUnits[i], unitResult);
                }
                else {
                    parsedUnit = compiler.parser.dietParse(sourceUnits[i], unitResult);
                }
                long resolveStart = System.currentTimeMillis();
                compiler.stats.parseTime += resolveStart - parseStart;
                // initial type binding creation
                compiler.lookupEnvironment.buildTypeBindings(parsedUnit, null /*no access restriction*/);
                compiler.stats.resolveTime += System.currentTimeMillis() - resolveStart;
                addCompilationUnit(compiler, sourceUnits[i], parsedUnit);
                ImportReference currentPackage = parsedUnit.currentPackage;
                if (currentPackage != null) {
                    unitResult.recordPackageName(currentPackage.tokens);
                }
                // } catch (AbortCompilationUnit e) {
                // requestor.acceptResult(unitResult.tagAsAccepted());
            }
            catch (AbortCompilation a) {
                // best effort to find a way for reporting this problem:
                if (a.compilationResult == null)
                    a.compilationResult = unitResult;
/*
                System.out.println("ECJ error: " + a.getMessage());
                System.out.println("ECJ cause: " + a.getCause().getClass().getCanonicalName());
                StackTraceElement stack[] = a.getCause().getStackTrace();
                for (int k = 0; k < stack.length; k++) {
                    System.out.println(stack[k].toString());
                }
*/
                throw a;
            }
            finally {
                sourceUnits[i] = null; // no longer hold onto the unit
            }
        }
        if (compiler.parser.readManager != null) {
            compiler.parser.readManager.shutdown();
            compiler.parser.readManager = null;
        }
        // binding resolution
        compiler.lookupEnvironment.completeTypeBindings();
    }

    /**
     * Compile and generate an AST for each input file specified in args.
     * 
     * @param args
     */
    static Main generateAst(String args[]) {
        if (verboseLevel > 0) {
            System.out.println("[INFO] ECJ::generateAst arguments=" + Arrays.toString(args));
        }

        Main main = new Main(new PrintWriter(System.out), new PrintWriter(System.out), true/*systemExit*/, null/*options*/, null/*progress*/);

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
            System.out.println("(2) Error in main.configure(args): " + e.getMessage());
            System.exit(1);
        }

        main.compilerOptions = new CompilerOptions(main.options);
        main.compilerOptions.performStatementsRecovery = false;
        main.compilerOptions.generateClassFiles = false;

        main.batchCompiler = new Compiler(main.getLibraryAccess(),
                                          main.getHandlingPolicy(),
                                          main.compilerOptions,
                                          main.getBatchRequestor(),
                                          main.getProblemFactory(),
                                          null,
                                          main.progress
                                         );

        /**
         * Add the initial set of compilation units into the loop
         * -> build compilation unit declarations, their bindings and record their results.
         */
        /*
         * tps : handle compilation units--------------------------------------------- */
        /*
         * Expand above protected function from ECJ's Compiler class.
         */
        ICompilationUnit[] sourceUnits = main.getCompilationUnits();
        int maxUnits = sourceUnits.length;
        main.batchCompiler.totalUnits = 0;
        main.batchCompiler.unitsToProcess = new CompilationUnitDeclaration[maxUnits];
        internalBeginToCompile(main.batchCompiler, sourceUnits, maxUnits);

        return main;
    }

    static int totalCompilationUnitsProcessed = 0;

    static Runtime runtime = Runtime.getRuntime();
    static long r1, r2, f1, f2;

    public static void startJava() {
        System.gc();

        r1 = runtime.totalMemory();
        f1 = runtime.freeMemory();
    }

    public static void endJava(ArrayList<CompilationUnitDeclaration> units) {
        r2 = runtime.totalMemory();
        f2 = runtime.freeMemory();
        int size = units.size();

        if (verboseLevel > 0) {
            System.out.println();
            System.out.println("**** In this iteration, the following " + (size == 1 ? "unit was" : (size + " units were")) + " processed:");
            System.out.println();

            for (CompilationUnitDeclaration unit : units) {
                System.out.println("   " + new String(unit.getFileName()));
            }

            System.out.println();
            System.out.println("**** Initial Max Memory:          \t " + r1 + ", used: " + (r1 - f1));
            System.out.println("**** After Compilation Max Memory:\t " + r2 + ", used: " + (r2 - f2));
            System.out.println("**** Total Number of Units Processed: " + totalCompilationUnitsProcessed);
            System.out.println();
        }
    }

    // This is the "main" function called from the outside (via the JVM from ROSE).
    public static void main(String args[]) {
        System.out.println("[INFO] ECJ::main arguments=" + Arrays.toString(args));
        /* tps : set up and configure ---------------------------------------------- */

        startJava();

        assert (! processedFiles.contains(args[args.length - 1]));

        // Filter out ROSE specific options.
        args = filterCommandline(args);

        //
        // Generate all necessary classfiles if no errors are detected
        //
        Main main = generateAst(args); // get compiler to generate AST.
        if (main.globalErrorsCount > 0) { // errors were detected?
            System.out.flush();
            System.out.println();
            System.out.println(main.globalErrorsCount + " ECJ front-end errors detected in input java program with " + main.getCompilationUnits().length + " compilation units");
            System.exit(1);
        }

        Compiler batchCompiler = main.batchCompiler; // get compiler to generate AST.
        // int maxUnits = main.getCompilationUnits().length;

        // Calling the parser to build the ROSE AST from a traversal of the ECJ AST.
        ArrayList<CompilationUnitDeclaration> units = new ArrayList<CompilationUnitDeclaration>();
        try {
            if (verboseLevel > 2)
                System.out.println("test 7 ...");

            JavaParser.cactionCompilationUnitList();

            if (verboseLevel > 2)
                System.out.println("test 8 ...");

            //
            // To process the source files that were specified by the user iterate up to
            // maxUnits. To iterate over all units, including the ones that are pulled
            // in by closure, iterate up to batchCompiler.totalUnits.
            //
            System.out.println("[INFO] ECJ::batchCompiler.totalUnits=" + batchCompiler.totalUnits);
            for (int i = 0; i < /* maxUnits */ batchCompiler.totalUnits; i++) {
                CompilationUnitDeclaration unit = batchCompiler.unitsToProcess[i];
                assert (unit != null);

                if (verboseLevel > 2)
                    System.out.println("calling batchCompiler.process(unit, i) ..." + new String(unit.getFileName()));

                String filename = new String(unit.getFileName());
                if (! processedFiles.contains(filename) && JavaParser.cactionIsSpecifiedSourceFile(filename)) {
                    // Set the ::currentSourceFile via JNI [TOO1, 2014-04-02]
                    JavaParser.cactionSetupSourceFilename(filename);

                    processedFiles.add(filename);
                    batchCompiler.process(unit, i);
                    if (unit.compilationResult.hasMandatoryErrors()) {
                        System.out.flush();
                        System.out.println();
                        System.out.println("*** ECJ compilation errors detected in input java program:"); // errors detected in input java program:");
                        System.out.println(unit.compilationResult.toString());
                        System.out.println();

                        /*
                        System.out.println("****************");
                        System.out.println("***" + filename + ":");
                        if (unit.compilationResult.hasProblems())
                            System.out.println("    *** This unit has problems");
                        if (unit.hasErrors())
                            System.out.println("    *** This unit has errors");
                        if (unit.compilationResult.hasErrors())
                            System.out.println("    *** This unit's compilationResult has errors");
                        if (unit.compilationResult.hasSyntaxError)
                            System.out.println("    *** This unit has syntax errors");
                        if (unit.compilationResult.hasMandatoryErrors())
                            System.out.println("    *** This unit has mandatory errors");
                        if (unit.compilationResult.hasBeenAccepted)
                            System.out.println("    *** This unit has been accepted");
                        if (unit.ignoreFurtherInvestigation)
                            System.out.println("    *** This unit requires no further investigation");
                        if (unit.ignoreMethodBodies)
                            System.out.println("    *** This unit's method bodies should be ignored");
                        if (unit.hasErrors())
                            System.out.println("    *** This unit has errors");
                        if (unit.compilationResult.hasInconsistentToplevelHierarchies)
                            System.out.println("    *** This unit has inconsistent Top Level Hierarchyproblems");
                        System.out.println("****************");
                        */

                        JavaParser.cactionEcjFatalCompilationErrors(filename);                        
                    }
                    else units.add(unit);

                    // Reset the ::currentSourceFile via JNI [TOO1, 2014-04-02]
                    JavaParser.cactionClearSourceFilename();
                }
            }

            totalCompilationUnitsProcessed += units.size();

            //
            //
            //
            try {
                if (units.size() > 0) {
                    if (javaParserSupport == null) { // JavaParserSupport not yet allocated?
                        javaParserSupport = new JavaParserSupport(units.get(0));
                    }
                    javaParserSupport.translate(units, (units.size() == 1 && tempUnnamedFiles.contains(new String(units.get(0).getFileName()))));
                }
            }
            catch (Exception e) {
                e.printStackTrace();
                System.out.println("Error in JavaTraversal::main() (nested catch before finally): " + e.getMessage());

                // This should output the call stack.
                System.out.println("Error in JavaTraversal::main() (nested catch before finally): " + e);
            }

            //
            //
            //
            for (int i = 0; i < main.batchCompiler.totalUnits; i++) {
                CompilationUnitDeclaration unit = batchCompiler.unitsToProcess[i];
                unit.cleanUp();
                batchCompiler.unitsToProcess[i] = null; // release reference to processed unit declaration
                // TODO: Not sure why the next two lines are needed ... PC
                batchCompiler.stats.lineCount += unit.compilationResult.lineSeparatorPositions.length;
                batchCompiler.requestor.acceptResult(unit.compilationResult.tagAsAccepted());
            }
        }
        catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
            System.out.println("Ouch !!! Make sure that the signature of all native (jni?) methods match their corresponding Java headers. " + e.getMessage());
        }
        catch (Exception e) {
            // DQ (11/1/2010): Added more aggressive termination of program...
            e.printStackTrace();
            System.out.println("Error in JavaTraversal::main(): " + e.getMessage());
            // System.exit(1);

            hasErrorOccurred = true;
            return;
        }

        JavaParser.cactionCompilationUnitListEnd();

        endJava(units);

        if (verboseLevel > 2)
            System.out.println("Done compiling");
    }

    // DQ (10/12/2010): Implemented abstract baseclass "call()" member function (similar to OFP).
    public Boolean call() throws Exception {
        return Boolean.TRUE;
    }

    // DQ (10/12/2010): Added boolean value to report error to C++ calling program (similar to OFP).
    public static boolean getError() {
        return hasErrorOccurred;
    }

    public static boolean hasConflicts(String file_name, String package_name, String class_name) {
        return javaParserSupport.hasConflicts(file_name, package_name, class_name);
    }

    private static HashSet<String> tempUnnamedFiles = new HashSet<String>();

    private static final String baseTempPath = System.getProperty("java.io.tmpdir");
    private static int TEMP_DIR_ATTEMPTS = 10000;
    private static File temp_directory = null;
    private static String directory_name = null;

    /**
     * If a temporary directory does not yet exist, create it and return its name.
     * 
     * @return
     */
    public static String getTempDirectory() {
        if (directory_name == null) {
            String base_name = baseTempPath + File.separator + "dir-" + System.currentTimeMillis();
            for (int counter = 0; counter < TEMP_DIR_ATTEMPTS; counter++) { // variation from Google's Files.createTempDir().
                directory_name = base_name + counter;
                temp_directory = new File(directory_name);
                if (temp_directory.mkdir()) {
                    temp_directory.deleteOnExit(); // This does not work if the directory is not empty.
                    return directory_name;
                }
            }
            throw new IllegalStateException("Unable to create a temporary directory: " + directory_name + "... java");
        }

        return directory_name;
    }

    /**
     * 
     * Create a temporary directory for the given package name.
     * 
     * @param content
     * @return
     */
    public static void createTempNamedDirectory(String package_name) {
        assert (temp_directory != null);
        String directory_name = getTempDirectory() + File.separator;
        String suffix = package_name.replace('.', File.separatorChar);
        for (int dot_index = suffix.indexOf(File.separatorChar); dot_index != -1; dot_index = suffix.indexOf(File.separatorChar, dot_index + 1)) {
        	directory_name += suffix.substring(0, dot_index);
            File named_directory = new File(directory_name);
            if (! named_directory.mkdir()) {
                throw new IllegalStateException("Unable to create the directory: " + directory_name);
            }
        	suffix = suffix.substring(dot_index);
        }
    	directory_name += suffix;
        File named_directory = new File(directory_name);
        if (! named_directory.mkdir()) {
            throw new IllegalStateException("Unable to create the directory: " + directory_name);
        }
        named_directory.deleteOnExit(); // This does not work if the directory is not empty.
    }

    /**
     * 
     * The command should be either a package or an import statement. We write
     * it out to a temporary file and return the name of the temporary file.
     * 
     * @param content
     * @return
     */
    private static void createFile(File file, String content) throws IOException {
        file.deleteOnExit();
        Writer writer = null;
        try {
            writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file), "utf-8"));
            writer.write(content);
            writer.write("\n");
        }
        catch (IOException ex) {
            ex.printStackTrace();
            throw ex;
        }
        finally {
            try {
                writer.close();
            }
            catch (Exception ex) {
               assert (false);
            }
        }
    }

    /**
     * 
     * The content should represent either a package or an import statement. We
     * write it out to a temporary file and return the name of the temporary
     * file.
     * 
     * @param content
     * @return
     */
    public static String createTempFile(String content) {
        assert (temp_directory != null);
        String base_name = getTempDirectory() + File.separator + "tmp-";
        try {
            File temp_file = File.createTempFile(base_name, ".java", temp_directory); // new File(directory_name);
            createFile(temp_file, content);

            String filename = temp_file.getCanonicalPath();
            tempUnnamedFiles.add(filename);

            return filename;
        }
        catch (IOException e) {
            throw new IllegalStateException("Unable to create a temporary import file: " +
                                            temp_directory.getAbsolutePath() + File.separator +
                                            base_name + "... .java");
        }
    }

    /**
     * 
     * The content should represent a valid compilation unit. We write
     * it out to a temporary file and return the name of the temporary file.
     * 
     * @param content
     * @return
     */
    public static String createTempNamedFile(String simple_filename, String content) {
        assert (temp_directory != null);
        String filename = getTempDirectory() + File.separator + simple_filename + ".java";
        try {
            File named_file = new File(filename);
            named_file.createNewFile();
            createFile(named_file, content);

            return filename;
        }
        catch (IOException e) {
            throw new IllegalStateException("Unable to create the file: " + filename);
        }
    }
}
