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

package fortran.ofp;

import java.io.*;

import fortran.ofp.parser.java.FortranLexer;
import fortran.ofp.parser.java.FortranLexicalPrepass;
import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.FortranParserActionPrint;
import fortran.ofp.parser.java.FortranStream;
import fortran.ofp.parser.java.FortranTokenStream;

import org.antlr.runtime.*;

import java.util.ArrayList;
import java.util.concurrent.Callable;
 
public class FrontEnd implements Callable<Boolean> {
	
   private FortranTokenStream              tokens;
   private FortranLexer                    lexer;
   private FortranParser                   parser;
   private FortranLexicalPrepass           prepass;
   private String fileName;
   private int sourceForm;
   private boolean verbose = false;
   private boolean silent = false;
	private static boolean hasErrorOccurred = false;
	private static ArrayList<String> includeDirs;

   public static final int UNKNOWN_SOURCE_FORM = -1;
   public static final int FREE_FORM = 1;
   public static final int FIXED_FORM = 2;
        
   public FrontEnd(String[] args, String filename, String type) 
		throws IOException {
      this.lexer = new FortranLexer(
			new FortranStream(filename, this.determineSourceForm(filename))); 
      this.tokens = new FortranTokenStream(lexer);
      this.parser = new FortranParser(args, tokens, type, filename);
      this.prepass = new FortranLexicalPrepass(lexer, tokens, parser);
      this.fileName = filename;
      this.sourceForm = UNKNOWN_SOURCE_FORM;
   }

   private static boolean parseMainProgram(FortranTokenStream tokens, 
                                           FortranParser parser, 
                                           int start) throws Exception {
      // try parsing the main program
      parser.main_program();

      return parser.hasErrorOccurred;
   }// end parseMainProgram()

   private static boolean parseModule(FortranTokenStream tokens, 
                                      FortranParser parser, int start) 
      throws Exception {
      parser.module();
      return parser.hasErrorOccurred;
   }// end parseModule()

   private static boolean parseBlockData(FortranTokenStream tokens, 
                                         FortranParser parser, 
                                         int start) throws Exception {
      parser.block_data();
      
      return parser.hasErrorOccurred;
   }// end parseBlockData()      

   private static boolean parseSubroutine(FortranTokenStream tokens, 
                                          FortranParser parser, 
                                          int start) throws Exception {
      parser.subroutine_subprogram();

      return parser.hasErrorOccurred;
   }// end parserSubroutine()

   private static boolean parseFunction(FortranTokenStream tokens, 
                                        FortranParser parser, 
                                        int start) throws Exception {
      parser.ext_function_subprogram();
      return parser.hasErrorOccurred;
   }// end parseFunction()

   private static boolean parseProgramUnit(FortranLexer lexer, 
                                           FortranTokenStream tokens, 
                                           FortranParser parser) 
      throws Exception {
      int firstToken;
      int lookAhead = 1;
      int start;
      boolean error = false;

      // first token on the *line*.  will check to see if it's
      // equal to module, block, etc. to determine what rule of 
      // the grammar to start with.
      try {
         lookAhead = 1;
         do {
            firstToken = tokens.LA(lookAhead);
            lookAhead++;
         } while(firstToken == FortranLexer.LINE_COMMENT || 
                 firstToken == FortranLexer.T_EOS);
         
         // mark the location of the first token we're looking at
         start = tokens.mark();
         
         // attempt to match the program unit
         // each of the parse routines called will first try and match
         // the unit they represent (function, block, etc.).  if that 
         // fails, they may or may not try and match it as a main
         // program; it depends on how it fails.
         //
         // due to Sale's algorithm, we know that if the token matches
         // then the parser should be able to successfully match.
         if(firstToken != FortranLexer.EOF) {
            if(firstToken == FortranLexer.T_MODULE &&
               tokens.LA(lookAhead) != FortranLexer.T_PROCEDURE) {
               // try matching a module
               error = parseModule(tokens, parser, start);
            } else if(firstToken == FortranLexer.T_BLOCK ||
                      firstToken == FortranLexer.T_BLOCKDATA) {
               // try matching block data
               error = parseBlockData(tokens, parser, start);
            } else if(tokens.lookForToken(FortranLexer.T_SUBROUTINE) == true) {
               // try matching a subroutine
               error = parseSubroutine(tokens, parser, start);
            } else if(tokens.lookForToken(FortranLexer.T_FUNCTION) == true) {
               // try matching a function
               error = parseFunction(tokens, parser, start);
            } else {
               // what's left should be a main program
               error = parseMainProgram(tokens, parser, start);
            }// end else(unhandled token)
         }// end if(file had nothing but comments empty)
      } catch(RecognitionException e) {
         e.printStackTrace();
         error = true;
      }// end try/catch(parsing program unit)
      
      return error;
   }// end parseProgramUnit()

	public static void main(String args[]) throws Exception {
		Boolean error = false;
		Boolean verbose = false;
		Boolean silent = true;
		ArrayList<String> newArgs = new ArrayList<String>(0);
		String type = "fortran.ofp.parser.java.FortranParserActionNull";
		int nArgs = 0;

		includeDirs = new ArrayList<String>();

		// Get the arguments. Use --silent --verbose, and --dump as shorthand
		// so we don't have to specify explicit class names on the command line.
		for (int i = 0; i < args.length; i++) {
			if (args[i].startsWith("--dump")) {
				type = "fortran.ofp.parser.java.FortranParserActionPrint";
				silent = false;
				nArgs += 1;
				continue;
			}
			else if (args[i].startsWith("--verbose")) {
				type = "fortran.ofp.parser.java.FortranParserActionPrint";
				verbose = true;
				silent = false;
				nArgs += 1;
				continue;
			}
			else if (args[i].startsWith("--silent")) {
				type = "fortran.ofp.parser.java.FortranParserActionNull";
				silent = true;
				nArgs += 1;
				continue;
			} 
			else if (args[i].startsWith("--class")) {
				i += 1;
				type = args[i];
				nArgs += 2;
				continue;
			} else if(args[i].startsWith("-I")) {
				/* Skip the include dir stuff; it's handled by the lexer.  */
				nArgs += 1;
				includeDirs.add(args[i].substring(2, args[i].length()));
			} else if (args[i].startsWith("--")) {
				newArgs.add(args[i]);
				newArgs.add(args[i+1]);
				i += 1;
				nArgs += 2;
				continue;
			}
		}

      if (args.length <= nArgs) {
    	  System.out.println("Usage: java fortran.ofp.FrontEnd " + 
									"[--dump] [--silent] [--class className] ");
    	  System.out.println("                                    " + 
									"[--user_option user_arg] file1 [file2..fileN]");
      }

      for (int i = 0; i < args.length; i++) {
	      if (args[i].startsWith("--dump") | args[i].startsWith("--silent") |
				args[i].startsWith("--verbose")) {
	    	  continue;
	      } else if(args[i].startsWith("-I")) {
				/* Skip the include dir stuff; it's handled by the lexer.  */
				continue;
			} else if (args[i].startsWith("--")) {
	    	  i += 1;
	    	  continue;
	      }
    	  if (verbose) {
    		  System.out.println("*******************************************");
    		  System.out.println("args[" + i + "]: " + args[i]);
    	  }

		  /* Make sure the file exists.  */
		  File srcFile = new File(args[i]);
		  if(srcFile.exists() == false) {
			  System.err.println("Error: " + args[i] + " does not exist!");
			  error = new Boolean(true);
		  } else {
			  FrontEnd ofp = new FrontEnd(
				  newArgs.toArray(new String[newArgs.size()]), args[i], type);
			  ofp.setVerbose(verbose,silent);
			  if (ofp.getParser().getAction().getClass().getName() 
					== "fortran.ofp.parser.java.FortranParserActionPrint") {
				  FortranParserActionPrint action = 
					  (FortranParserActionPrint) ofp.getParser().getAction();
				  
				  // "verbose" in Print() is either "normal" or "verbose" here..
				  action.setVerbose(!silent);
			  }
			  error |= ofp.call();
			  
		  }

		  if (verbose) {
			  System.out.println
				  ("********************************************");
		  }
		  
      }

		hasErrorOccurred = error.booleanValue();
   }// end main()
   
   public void setVerbose(Boolean vFlag, Boolean sFlag) {
	   this.verbose = vFlag;
	   this.silent = sFlag;
   }
   
   public FortranParser getParser() {
	   return this.parser;
   }
   
   public Boolean call() throws Exception {
      boolean error = false;

      if(determineSourceForm(this.fileName) == FIXED_FORM)
         if (verbose) System.out.println(this.fileName + " is FIXED FORM");
      else
         if (verbose) System.out.println(this.fileName + " is FREE FORM");

		lexer.setIncludeDirs(includeDirs);

		parser.initStreamInfo();

      // determine whether the file is fixed or free form and 
      // set the source form in the prepass so it knows how to handle lines.
      prepass.setSourceForm(determineSourceForm(this.fileName));

      // apply Sale's algorithm to the tokens to allow keywords
      // as identifiers.  also, fixup labeled do's, etc.
      prepass.performPrepass();

      // overwrite the old token stream with the (possibly) modified one
      tokens.finalizeTokenStream();
                 
      // parse each program unit in a given file
      while(tokens.LA(1) != FortranLexer.EOF) {
         // attempt to parse the current program unit
         error = parseProgramUnit(lexer, tokens, parser);
                 
         // see if we successfully parse the program unit or not
         if(error != false) {
            System.out.println("Parser failed");
         } else {
            if (verbose) System.out.println("Parser exiting normally");
         }// end else(parser exited normally)
      }// end while(not end of file) 

		// Call the cleanUp method for the give action class.  This is more 
		// important in the case of a C action *class* since it could easily 
		// have created memory that's outside of the jvm.
		getParser().getAction().cleanUp();

      return new Boolean(error);
   }// end call()

   private int determineSourceForm(String fileName) {
      if(fileName.endsWith(new String(".f")) == true ||
         fileName.endsWith(new String(".F")) == true) {
         this.sourceForm = FIXED_FORM;
         return FIXED_FORM;
      } else {
         this.sourceForm = FREE_FORM;
         return FREE_FORM;
      }
   }// end determineSourceForm()


   public int getSourceForm() {
      return this.sourceForm;
   }

	public static boolean getError() {
		return hasErrorOccurred;
	}

} //end class FrontEnd
