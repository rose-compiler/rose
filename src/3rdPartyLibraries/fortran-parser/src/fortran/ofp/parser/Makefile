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

include ../../../../make.inc

JAVAC=$(OFP_JAVAC) #javac
JAVA=$(OFP_JAVA) #java
JAR=$(OFP_JAR) #jar
CD=cd
CP=cp
SRCDIR=$(OFP_SRC_DIR)
JAVADIR=java
CDIR=$(OFP_C_DIR)
JNIDIR=$(OFP_JNI_DIR)
TOOLSDIR=$(OFP_TOOLS_DIR)
COPYFILES=FortranParser.g FortranLexer.g
CLASSPATH:=$(CLASSPATH):$(JAVADIR)/

JARFILE=$(OFP_JARFILE)

ACTION_CLASS="null"

GENERATED_JAVA_FILES=FortranLexer.java FortranParser.java
CLASS_FILES=FortranLexer.class FortranParser.class
GENERATED_CLASS_FILES=FortranLexer*DFA*.class *$$*.class
GENERATED_TOKEN_FILES=FortranLexer.tokens FortranParser.tokens

# this needs to test whether the files exist in the subdirs or not and then
# decide whether to copy the files into the subdirs
all: 
	$(MAKE) copy
	$(CD) $(JAVADIR) ; $(MAKE) 

copy:
	$(CP) $(COPYFILES) $(JAVADIR)

runTests: 
	$(CD) $(JAVADIR) ; $(MAKE) runTests

runDraftTests: 
	$(CD) $(JAVADIR) ; $(MAKE) ACTION_CLASS=$(ACTION_CLASS) runDraftTests 

runAllTests: 
	$(CD) $(JAVADIR) ; $(MAKE) runAllTests

jarfile: 
	$(CD) $(JAVADIR); $(MAKE) jarfile;

clean:
	$(CD) $(JAVADIR); $(MAKE) clean;
	@for grammarFile in $(COPYFILES) ; do \
           rm -f $(JAVADIR)/$$grammarFile ; \
        done;

# TODO::
# this rule needs fixed.  since there is a root Makefile (in fortran-parser) 
# this rule should no longer call everyone elses 'make allclean' rules.  
allclean:
	$(CD) $(JAVADIR); $(MAKE) allclean
	$(CD) $(CDIR); $(MAKE) allclean
ifeq ($(OFP_ENABLE_JNI), "yes")
	$(CD) $(JNIDIR); $(MAKE) allclean
endif
	$(CD) $(TOOLSDIR); $(MAKE) allclean

