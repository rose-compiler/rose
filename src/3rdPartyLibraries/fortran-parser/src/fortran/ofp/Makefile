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

OFP_HOME=../../..

include $(OFP_HOME)/make.inc

CD=cd

JAVAC=$(OFP_JAVAC) #javac
JAVA=$(OFP_JAVA) #java
JAR=$(OFP_JAR) #jar
SRCDIR=$(OFP_SRC_DIR)/src
BUILDDIR=$(OFP_BUILD_DIR)
PARSERDIR=parser

CLASSDIR = $(BUILDDIR)/fortran/ofp
JFLAGS = -d $(BUILDDIR) $(OFP_JFLAGS)

JARFILE=$(OFP_JARFILE)

CLASS_FILES = $(CLASSDIR)/FrontEnd.class

all: 
	$(CD) $(PARSERDIR) ; $(MAKE) ; $(MAKE) jarfile
	$(MAKE) classes
	$(MAKE) jarfile

classes: $(CLASS_FILES) 

$(CLASSDIR)/FrontEnd.class: FrontEnd.java
	$(JAVAC) $(JFLAGS) $<

jarfile:
	cd $(BUILDDIR) ; $(JAR) cf $(JARFILE) fortran/

FAKE:

############################ clean up ##############################
clean:
	$(CD) $(PARSERDIR) ; $(MAKE) clean
	rm -f $(CLASS_FILES) 

allclean:
	$(MAKE) clean
	$(CD) $(PARSERDIR) ; $(MAKE) clean
	cd $(BUILDDIR) ; rm -f $(JARFILE)

