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

PERL=perl #perl command
JAVAC=$(OFP_JAVAC) #javac
JAVA=$(OFP_JAVA) #java
JAR=$(OFP_JAR) #jar
SRCDIR=$(OFP_SRC_DIR)
BUILDDIR=$(OFP_BUILD_DIR)
PARDIR=$(OFP_PARSER_DIR)

CLASSDIR = $(BUILDDIR)/fortran/ofp/parser/java
JFLAGS = -d $(BUILDDIR) @sourcefiles $(OFP_JFLAGS)

ACTION_CLASS="null"

JARFILE=$(OFP_JARFILE)

CLASS_FILES = $(CLASSDIR)/FortranToken.class \
            $(CLASSDIR)/FortranLexer.class $(CLASSDIR)/FortranParser.class \
            $(CLASSDIR)/FortranStream.class \
            $(CLASSDIR)/FortranTokenStream.class \
            $(CLASSDIR)/FortranLexicalPrepass.class \
	    $(CLASSDIR)/IActionEnums.class \
	    $(CLASSDIR)/IFortranParserAction.class \
            $(CLASSDIR)/FortranParserActionNull.class \
	    $(CLASSDIR)/FortranParserActionFactory.class \
            $(CLASSDIR)/FortranParserActionPrint.class
GENERATED_JAVA_FILES=FortranLexer.java FortranParser.java
GENERATED_CLASS_FILES=$(CLASSDIR)/FortranLexer*DFA*.class \
                      $(CLASSDIR)/*$$*.class
GENERATED_TOKEN_FILES=FortranLexer.tokens FortranParser.tokens
COPIED_FILES=FortranParser.g FortranLexer.g

DRAFT_TEST_DIRS="../annex_c" "../chapter_11" "../chapter_15" "../chapter_16" \
                "../chapter_4" "../chapter_5" "../chapter_6" "../chapter_7" \
                "../chapter_8" "../chapter_9" "../chapter_10" "../chapter_12" \
                "../chapter_2"
F90_TEST_DIRS="../misc" "../program-unit" 

all: FortranParserActionNull.java $(GENERATED_JAVA_FILES) $(CLASS_FILES) 

runTests: $(CLASS_FILES)
	$(JAVA) fortran.ofp.FrontEnd ../test_files/*.f03 ../test_files/*.f90

runDraftTests: $(CLASS_FILES)
ifneq ("$(ACTION_CLASS)", "null")
	@for dir in $(DRAFT_TEST_DIRS) ; do \
           $(JAVA) fortran.ofp.FrontEnd --class $(ACTION_CLASS) $$dir/*.f03; \
        done;
else
	@for dir in $(DRAFT_TEST_DIRS) ; do \
           $(JAVA) fortran.ofp.FrontEnd --verbose $$dir/*.f03; \
        done;
endif

runAllTests: $(CLASS_FILES)
	$(MAKE) runTests
	$(MAKE) runDraftTests
	@for dir in $(F90_TEST_DIRS) ; do \
           $(JAVA) fortran.ofp.FrontEnd $$dir/*.f90; \
        done;

# the -Xconversiontimeout is to try and override the time you allow antlr 
# for converting NFA's.  i couldn't find documentation on what a good number
# would be for the argument.  
FortranLexer.java: FortranLexer.g 
	$(JAVA) -Xms256M -Xmx512M org.antlr.Tool -Xconversiontimeout 2000 $<
	sed "s/LexerLexer/Lexer/g" FortranLexerLexer.java > FortranLexer.java
	rm FortranLexerLexer.java

FortranParser.java: FortranParser.g FortranLexer.tokens
	$(JAVA) org.antlr.Tool $<
	sed "s/ParserParser/Parser/g" FortranParserParser.java > FortranParser.java
	rm FortranParserParser.java

# make sure *Null.java is up-to-date.
FortranParserActionNull.java: IFortranParserAction.java FAKE
	$(PERL) makNull.pl 

$(CLASSDIR)/FortranLexer.class: FortranLexer.java FortranLexer.tokens
	$(JAVAC) $(JFLAGS) FortranLexer.java

$(CLASSDIR)/FortranParser.class: FortranParser.java FortranLexer.tokens
	$(JAVAC) $(JFLAGS) FortranParser.java

$(CLASSDIR)/FortranStream.class: FortranStream.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/FortranTokenStream.class: FortranTokenStream.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/FortranLexicalPrepass.class: FortranLexicalPrepass.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/IActionEnums.class: IActionEnums.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/IFortranParserAction.class: IFortranParserAction.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/FortranParserActionNull.class: FortranParserActionNull.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/FortranParserActionFactory.class: FortranParserActionFactory.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/FortranParserActionPrint.class: FortranParserActionPrint.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/FortranToken.class: FortranToken.java
	$(JAVAC) $(JFLAGS) $<

jarfile: #$(CLASS_FILES)
	$(MAKE) all
	cd $(BUILDDIR) ; $(JAR) cf $(JARFILE) fortran/

FAKE:

############################ clean up ##############################
clean:
	rm -f $(CLASS_FILES) 
	rm -f $(GENERATED_JAVA_FILES) 
	rm -f $(GENERATED_TOKEN_FILES)
	rm -f $(GENERATED_CLASS_FILES)
	rm -f $(COPIED_FILES)
	rm -f FortranParserActionNull.java

allclean:
	$(MAKE) clean
	cd $(BUILDDIR) ; rm -f $(JARFILE)

