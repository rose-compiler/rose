package fortran.tools;

import org.antlr.runtime.Token;

import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.FortranParserActionNull;


public class DeprecatedFeatures extends FortranParserActionNull {

	DeprecatedFeatures(String[] args, FortranParser parser, String filename) {
		super(args, parser, filename);
	}

	/** R424
	 * char-selector
	 */
	public void char_selector(int kindOrLen1, int kindOrLen2, boolean hasAsterisk) {
		if (hasAsterisk) {
			System.out.println("Deprecated Feature: CHARACTER* form of CHARACTER declaration (see B2.7) ");
			// TODO - need to get line and column information somehow
		}
	}
	
	/** R1220
	 * actual_arg_spec
	 */
	public void actual_arg_spec(Token keyword, boolean hasExpr, Token label) {
		if (label != null) {
			System.out.print("Deprecated Feature: alternative return (see B2.1), ");
			System.out.println("line=" + label.getLine() + " col=" + label.getCharPositionInLine());
			}
	}
	
}
