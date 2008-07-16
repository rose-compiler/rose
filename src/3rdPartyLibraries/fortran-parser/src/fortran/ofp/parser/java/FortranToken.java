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

package fortran.ofp.parser.java;

import org.antlr.runtime.*;
import fortran.ofp.parser.java.FortranStream;

public class FortranToken extends CommonToken {
	protected String whiteText = "";

	public FortranToken(CharStream input, int type, int channel,
							  int start, int stop) {
		super(input, type, channel, start, stop);
	}

// 	public FortranToken(int type, CharStream input) {
	public FortranToken(int type) {
		super(type);
// 		this.input = input;
	}

// 	public FortranToken(int type, String text, CharStream input) {
	public FortranToken(int type, String text) {
		super(type, text);
// 		this.input = input;
	}

// 	public FortranToken(Token token, CharStream input) {
	public FortranToken(Token token) {
		super(token);
// 		this.input = input;
	}

	public String getWhiteText() {
		return whiteText;
	}

	void setWhiteText(String text) {
		whiteText = text == null ? "" : text;
	}

	public FortranStream getInput() {
		return (FortranStream)this.input;
	}
}
