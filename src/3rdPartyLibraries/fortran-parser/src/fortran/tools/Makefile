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

include ../../../make.inc

JAVAC=$(OFP_JAVAC) #javac
JAVA=$(OFP_JAVA) #java
JAR=$(OFP_JAR) #jar
BUILDDIR=$(OFP_BUILD_DIR)
PARDIR=$(OFP_PARSER_DIR)
#JARDIR=../lib

CLASSDIR = $(BUILDDIR)/fortran/tools
JFLAGS = -d $(BUILDDIR) @sourcefiles $(OFP_JFLAGS)

JARFILE=OpenFortranParser.jar

GEN_JAVA_FILES=RuleStackTrace.java

CLASS_FILES = $(CLASSDIR)/DeprecatedFeatures.class \
            $(CLASSDIR)/IOUnitNumber.class \
            $(CLASSDIR)/SinglePrecisionConstant.class $(CLASSDIR)/ToC.class \
            $(CLASSDIR)/CPrint.class \
            $(CLASSDIR)/GenRuleStackTrace.class \
            $(CLASSDIR)/CFiles.class \
            $(CLASSDIR)/DetectIncludeStmt.class 

all: $(CLASS_FILES) 

$(CLASSDIR)/DeprecatedFeatures.class: DeprecatedFeatures.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/IOUnitNumber.class: IOUnitNumber.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/SinglePrecisionConstant.class: SinglePrecisionConstant.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/ToC.class: ToC.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/CPrint.class: CPrint.java $(PARDIR)/java/IFortranParserAction.java 
	cd $(PARDIR)/java ; $(MAKE) jarfile
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/CFiles.class: CFiles.java $(PARDIR)/java/IFortranParserAction.java 
	cd $(PARDIR)/java ; $(MAKE) jarfile
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/RuleStackTrace.class: RuleStackTrace.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/GenRuleStackTrace.class: GenRuleStackTrace.java
	$(JAVAC) $(JFLAGS) $<

$(CLASSDIR)/DetectIncludeStmt.class: DetectIncludeStmt.java
	$(JAVAC) $(JFLAGS) $<

RuleStackTrace.java: GenRuleStackTrace.java \
                     $(PARDIR)/java/IFortranParserAction.java 
	$(JAVA) $(OFP_CLASSPATH) fortran.tools.GenRuleStackTrace
	$(MAKE) $(CLASSDIR)/RuleStackTrace.class
	$(MAKE) jarfile

jarfile: #$(CLASS_FILES)
	$(MAKE) all
	cd $(BUILDDIR) ; $(JAR) cf $(JARFILE) fortran/

clean:
	rm -f $(CLASS_FILES) $(CLASSDIR)/RuleStackTrace.class 
	rm -f RuleStackTrace.java

allclean:
	$(MAKE) clean

