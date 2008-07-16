/*******************************************************************************
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.
 * This material was produced under U.S. Government contract DE-AC52-06NA25396
 * for Los Alamos National Laboratory (LANL), which is operated by the Los Alamos
 * National Security, LLC (LANS) for the U.S. Department of Energy. The U.S. Government has
 * rights to use, reproduce, and distribute this software. NEITHER THE
 * GOVERNMENT NOR LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified
 * to produce derivative works, such modified software should be clearly marked,
 * so as not to confuse it with the version available from LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

package fortran.ofp.parser.java;

import java.lang.reflect.Constructor;

public class FortranParserActionFactory {

	static IFortranParserAction newAction(String[] args, FortranParser parser, String kind, String filename) {
		IFortranParserAction action = null;
		/*
		if (kind.compareToIgnoreCase("dump") == 0) {
			action = new FortranParserActionPrint(args, parser, filename);
		} else if (kind.compareToIgnoreCase("null") == 0) {
			action = new FortranParserActionNull(args, parser, filename);
		} else {
		*/
		// Look up the class name. Could be FortranParserActionPrint, or
		// FortranParserActionNull, or maybe somethig else.
		try {
			Constructor[] cons = Class.forName(kind).getDeclaredConstructors();
			for (int i = 0; i < cons.length; i++) {
				Class[] types = cons[i].getParameterTypes();
				if ( types.length == 3 &
						types[0] == String[].class & 
						types[1] == FortranParser.class & 
						types[2] == java.lang.String.class ) {
					Object[] actionArgs = {args, parser, filename};
					action = (IFortranParserAction) cons[i].newInstance(actionArgs);
					break;
				}
			}
		} catch (Exception e) {
			// InstantiationException, IllegalAccessException,
			// IllegalArgumentException, InvocationTargetException
			// ClassNotFoundException, NoSuchMethodException
			System.out.println(e);
		}
		
		// Had to eliminate error case because we don't want to instantiate
		// any explicit objects here.
		/*
		if (action == null)
			action = new FortranParserActionNull(args, parser, filename);
		*/

		return action;
	}

}
