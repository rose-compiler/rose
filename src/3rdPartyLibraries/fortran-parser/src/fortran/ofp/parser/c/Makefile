# Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
# material was produced under U.S. Government contract DE-
# AC52-06NA25396 for Los Alamos National Laboratory (LANL), which is
# operated by the Los Alamos National Security, LLC (LANS) for the
# U.S. Department of Energy. The U.S. Government has rights to use,
# reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
# LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
# LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
# produce derivative works, such modified software should be clearly
# marked, so as not to confuse it with the version available from
# LANL.

# Additionally, this program and the accompanying materials are made
# available under the terms of the Eclipse Public License v1.0 which
# accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html

include ../../../../../make.inc

JAVAC=$(OFP_JAVAC) #javac
JAVA=$(OFP_JAVA) #java
JAR=$(OFP_JAR) #jar

COPIED_FILES=FortranParser.g FortranLexer.g
GENERATED_C_HEADER=FortranParserAction.h
GENERATED_C_NULL=FortranParserActionNull.c
GENERATED_C_PRINT=FortranParserActionPrint.c
GENERATED_C_ENUMS=ActionEnums.h

all: $(GENERATED_C_HEADER) $(GENERATED_C_PRINT) $(GENERATED_C_NULL) 

# the -Xconversiontimeout is to try and override the time you allow antlr 
# for converting NFA's.  i couldn't find documentation on what a good number
# would be for the argument.  
FortranLexer.c: FortranLexer.g 
	$(JAVA) -Xms256M -Xmx512M org.antlr.Tool -Xconversiontimeout 2000 $<
	sed "s/LexerLexer/Lexer/g" FortranLexerLexer.c > FortranLexer.c
	sed "s/LexerLexer/Lexer/g" FortranLexerLexer.h > FortranLexer.h
	rm FortranLexerLexer.c FortranLexerLexer.h

FortranParser.c: FortranParser.g FortranLexer.tokens
	$(JAVA) org.antlr.Tool $<
	sed "s/ParserParser/Parser/g" FortranParserParser.c > FortranParser.c
	sed "s/ParserParser/Parser/g" FortranParserParser.h > FortranParser.h
	rm FortranParserParser.c FortranParserParser.h

FortranParserAction.h:
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.CFiles header
FortranParserActionPrint.c:
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.CFiles print
FortranParserActionNull.c:
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.CFiles null

clean:
	rm -f $(GENERATED_C_PRINT)
	rm -f $(GENERATED_C_NULL)
	rm -f $(GENERATED_C_HEADER)
	rm -f $(GENERATED_C_ENUMS)

allclean:
	$(MAKE) clean

