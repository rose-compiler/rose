/******************************************************************************
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL), which
 * is operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *****************************************************************************/

package fortran.tools;

import java.util.Stack;
import org.antlr.runtime.Token;

import fortran.ofp.parser.java.FortranParser;
import fortran.ofp.parser.java.FortranParserActionNull;


public class DetectIncludeStmt extends FortranParserActionNull {
	private FortranParser parser;
	private Stack<String> streamNames = null;

	public DetectIncludeStmt(String[] args, FortranParser parser, 
									 String filename) {
		super(args, parser, filename);
		
		this.parser = parser;
		this.streamNames = new Stack<String>();
	}// end DetectIncludeStmt()

	public void end_of_stmt(Token eos) {
		System.err.println("hello from DetectIncludeStmt#end_of_stmt");
	}// end end_of_stmt()


	public void start_of_file(String fileName) {
		System.err.println("pushing stream: " + fileName + 
								 " in DetectIncludeStmt#start_of_file");
		streamNames.push(fileName);
	}

	public void end_of_file() {
		String oldStreamName;

		oldStreamName = streamNames.pop();
		System.err.println("popped stream named: " + oldStreamName + 
								 " in DetectIncludeStmt#end_of_file");
	}

}// end class DetectIncludeStmt
