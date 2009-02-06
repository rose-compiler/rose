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

# Edited for ROSE by Jeremiah Willcock

OFP_INSTALL_DIR=@prefix@
OFP_ROOT_DIR=@top_srcdir@/src/3rdPartyLibraries/fortran-parser
OFP_SRC_DIR=$(OFP_ROOT_DIR)/src
OFP_FRONT_DIR=$(OFP_SRC_DIR)/fortran/ofp
OFP_PARSER_DIR=$(OFP_FRONT_DIR)/parser
OFP_C_DIR=$(OFP_FRONT_DIR)/parser/c
OFP_JNI_DIR=$(OFP_C_DIR)/jni
OFP_TOOLS_DIR=$(OFP_SRC_DIR)/fortran/tools

OFP_JARFILE=OpenFortranParser.jar
OFP_C_ACTION_SO=libfortran_ofp_parser_c_jni_FortranParserActionJNI.so 

OFP_CLASSPATH=-classpath @CLASSPATH@:./build:.
OFP_JFLAGS=$(OFP_CLASSPATH)

javadir=$(libdir)

JARFILE=$(OFP_JARFILE)

java-stuff:
	-mkdir build
	-mkdir build/fortran
	$(JAVAC) $(OFP_CLASSPATH) -d build $(srcdir)/src/fortran/ofp/parser/java/IActionEnums.java
	$(JAVAC) $(OFP_CLASSPATH) -d build $(srcdir)/src/fortran/ofp/parser/java/IFortranParserAction.java
	$(JAVA) $(OFP_CLASSPATH) org.antlr.Tool -lib $(srcdir)/src/fortran/ofp/parser/ -fo . $(srcdir)/src/fortran/ofp/parser/FortranLexer.g
	sed "s/LexerLexer/Lexer/g" FortranLexerLexer.java > FortranLexer.java
	rm FortranLexerLexer.java
	$(JAVA) $(OFP_CLASSPATH) org.antlr.Tool -fo . $(srcdir)/src/fortran/ofp/parser/FortranParser.g
	sed "s/ParserParser/Parser/g" FortranParserParser.java > FortranParser.java
	rm FortranParserParser.java FortranLexer.tokens FortranParser.tokens
	-mkdir tmp
	cp $(srcdir)/src/fortran/ofp/parser/java/IFortranParserAction.java tmp/
	cp $(srcdir)/src/fortran/ofp/parser/java/makNull.pl tmp/
	cd tmp; $(PERL) ./makNull.pl
	$(JAVAC) $(OFP_CLASSPATH) -d build $(srcdir)/src/fortran/ofp/FrontEnd.java FortranLexer.java FortranParser.java tmp/FortranParserActionNull.java $(srcdir)/src/fortran/ofp/parser/java/*.java $(srcdir)/src/fortran/tools/*.java
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.GenRuleStackTrace
	$(JAVAC) $(OFP_CLASSPATH) -d build RuleStackTrace.java
	$(JAVAC) $(OFP_CLASSPATH) -d build $(srcdir)/src/fortran/ofp/parser/java/*.java
	-mkdir tmp/java
	-mkdir tmp/c
	cp $(srcdir)/src/fortran/ofp/parser/java/IFortranParserAction.java $(srcdir)/src/fortran/ofp/parser/java/IActionEnums.java tmp/java/
	cp $(srcdir)/src/fortran/ofp/parser/java2c.pl tmp/
	cd tmp; $(PERL) ./java2c.pl
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.ToC
	$(JAVAC) $(OFP_CLASSPATH) -d build FortranParserActionJNI.java
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.CFiles header
	-rm OpenFortranParser.jar
	$(JAR) cf OpenFortranParser.jar -C build fortran/
	touch java-stuff

# lib_LTLIBRARIES = libfortran_ofp_parser_c_jni_FortranParserActionJNI.la
if ROSE_USE_OPEN_FORTRAN_PARSER
java_DATA = OpenFortranParser.jar
c_actions_jni.c: java-stuff
OpenFortranParser.jar: java-stuff
else
java_DATA =
endif

# libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES = c_actions_jni.c
# libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_CPPFLAGS = -I. -I./tmp/c -I$(srcdir)/src/fortran/ofp/parser/c -I$(srcdir)/src/fortran/ofp/parser/c/jni

CLEANFILES = java-stuff OpenFortranParser.jar c_actions_jni.c FortranParserActionJNI.java RuleStackTrace.java FortranLexer.java FortranParser.java FortranParserAction.h

EXTRA_DIST = INSTALL README get_antlr.sh config src tests

clean-local:
	rm -rf build/ tmp/


copy_as_before_fix:
	cp $(srcdir)/src/fortran/ofp/parser/FortranLexer.g.beforeFix $(srcdir)/src/fortran/ofp/parser/FortranLexer.g
	cp $(srcdir)/src/fortran/ofp/parser/FortranParser.g.beforeFix $(srcdir)/src/fortran/ofp/parser/FortranParser.g
	cp $(srcdir)/src/fortran/ofp/parser/java/FortranLexicalPrepass.java.beforeFix $(srcdir)/src/fortran/ofp/parser/java/FortranLexicalPrepass.java
	cp $(srcdir)/src/fortran/ofp/parser/java/FortranTokenStream.java.beforeFix $(srcdir)/src/fortran/ofp/parser/java/FortranTokenStream.java
	cp $(srcdir)/src/fortran/ofp/FrontEnd.java.beforeFix $(srcdir)/src/fortran/ofp/FrontEnd.java

copy_as_after_fix:
	cp $(srcdir)/src/fortran/ofp/parser/FortranLexer.g.afterFix $(srcdir)/src/fortran/ofp/parser/FortranLexer.g
	cp $(srcdir)/src/fortran/ofp/parser/FortranParser.g.afterFix $(srcdir)/src/fortran/ofp/parser/FortranParser.g
	cp $(srcdir)/src/fortran/ofp/parser/java/FortranLexicalPrepass.java.afterFix $(srcdir)/src/fortran/ofp/parser/java/FortranLexicalPrepass.java
	cp $(srcdir)/src/fortran/ofp/parser/java/FortranTokenStream.java.afterFix $(srcdir)/src/fortran/ofp/parser/java/FortranTokenStream.java
	cp $(srcdir)/src/fortran/ofp/FrontEnd.java.afterFix $(srcdir)/src/fortran/ofp/FrontEnd.java

