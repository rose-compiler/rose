package fortran.tools;

import org.antlr.runtime.Token;

import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.FortranParserActionNull;


public class SinglePrecisionConstant extends FortranParserActionNull {
	
	SinglePrecisionConstant(String[] args, FortranParser parser, String filename) {
		super(args, parser, filename);
	}

	/** R417
	 * real_literal_constant
	 */
	public void real_literal_constant(Token digits, Token fractionExp, Token kindParam) {
		if (fractionExp != null) {
			String constant = fractionExp.getText();
			if (digits != null) {
				constant = digits.getText() + constant;
			}
			if (constant.contains("d")) return;
			if (constant.contains("D")) return;
			
			System.out.print("Single precision literal constant, " + constant);
			System.out.println(", at line=" + fractionExp.getLine() + " col=" + fractionExp.getCharPositionInLine());
		}
	}

}
