package fortran.tools;

import org.antlr.runtime.Token;

import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.FortranParserActionNull;


public class IOUnitNumber extends FortranParserActionNull {
	
	private Token intLiteral;
	private Integer unitNumber;
	
	public IOUnitNumber(String[] args, FortranParser parser, String filename) {
		super(args, parser, filename);
		intLiteral = null;
		unitNumber = null;
		if (args[0].compareToIgnoreCase("--unit") == 0) {
			unitNumber = Integer.parseInt(args[1]);
		}

	}

	/** R406
	 * int_literal_constant
	 */
	public void int_literal_constant(Token digitString, Token kindParam) {
		intLiteral = digitString;
	}

	/** R913
	 * io_control_spec
	 * 
	 * Unless this is a unit number, nullify the possible int-literal-constant
	 */
	 public void io_control_spec(boolean hasExpression, Token keyword, boolean hasAsterisk) {
		 if (hasExpression) {
			 if (keyword != null) {
				 if (keyword.getText().compareToIgnoreCase("unit") != 0) {
					 intLiteral = null;
				 }
			 }
		 } else {
			 intLiteral = null;
		 }
	 }

	public void write_stmt(Token label, boolean hasOutputList) {
		if (intLiteral != null & unitNumber != null) {
			if (unitNumber.compareTo(Integer.parseInt(intLiteral.getText())) == 0) {
				System.out.print("Write statement: unit number=" + intLiteral.getText());
				System.out.println(", at line=" + intLiteral.getLine() + " col=" + intLiteral.getCharPositionInLine());
			}
		}
		intLiteral = null;
	}

}


