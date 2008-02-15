# $Id: Makefile,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
# -*-makefile-*-
## * BeginRiceCopyright *****************************************************
## 
## Copyright ((c)) 2002, Rice University 
## All rights reserved.
## 
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## 
## * Redistributions of source code must retain the above copyright
##   notice, this list of conditions and the following disclaimer.
## 
## * Redistributions in binary form must reproduce the above copyright
##   notice, this list of conditions and the following disclaimer in the
##   documentation and/or other materials provided with the distribution.
## 
## * Neither the name of Rice University (RICE) nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
## 
## This software is provided by RICE and contributors "as is" and any
## express or implied warranties, including, but not limited to, the
## implied warranties of merchantability and fitness for a particular
## purpose are disclaimed. In no event shall RICE or contributors be
## liable for any direct, indirect, incidental, special, exemplary, or
## consequential damages (including, but not limited to, procurement of
## substitute goods or services; loss of use, data, or profits; or
## business interruption) however caused and on any theory of liability,
## whether in contract, strict liability, or tort (including negligence
## or otherwise) arising in any way out of the use of this software, even
## if advised of the possibility of such damage. 
## 
## ******************************************************* EndRiceCopyright *

# Best seen in 120-column wide window (or print in landscape mode).
#---------------------------------------------------------------------------------------------------------------------
# This file is part of Mint.
# Arun Chauhan (achauhan@cs.rice.edu), Dept of Computer Science, Rice University, 2000, 2001.
#---------------------------------------------------------------------------------------------------------------------

# This Makefile must be invoked from the root Makefile


#---------------------------------------------------------------------------------------------------------------------
# file related definitions
ALLOBJS        = CFG.o  OARIFG.o  TarjanIntervals.o
#---------------------------------------------------------------------------------------------------------------------

OPENROOT = /home/qingyi/OpenAnalysis/src
CC = gcc -c -I${OPENROOT} -DOA_IRHANDLETYPE_UL


#---------------------------------------------------------------------------------------------------------------------
# compilation rules

all: $(ALLOBJS)

.C.o:
	$(CC) -c $<

clean:
	/bin/rm -f $(ALLOBJS)

listobjs:
	@echo $(ALLOBJS)

# header file dependencies
DomTree.o : DomTree.h $(OPENROOT)/OpenAnalysis/Utils/Tree.h \
	$(OPENROOT)/OpenAnalysis/Utils/DGraph.h \
	$(OPENROOT)/OpenAnalysis/Utils/BaseGraph.h \
	$(OPENROOT)/OpenAnalysis/Utils/Iterator.h \
	$(OPENROOT)/OpenAnalysis/Utils/Exception.h

Phi.o: Phi.h $(OPENROOT)/OpenAnalysis/CFG/CFG.h \
	$(OPENROOT)/OpenAnalysis/Utils/Tree.h \
	$(OPENROOT)/OpenAnalysis/Utils/DGraph.h \
	$(OPENROOT)/OpenAnalysis/Utils/BaseGraph.h \
	$(OPENROOT)/OpenAnalysis/Utils/Iterator.h \
	$(OPENROOT)/OpenAnalysis/Utils/Exception.h

SSA.o: SSA.h DomTree.h Phi.h $(OPENROOT)/OpenAnalysis/CFG/CFG.h \
	$(OPENROOT)/OpenAnalysis/Utils/Tree.h \
	$(OPENROOT)/OpenAnalysis/Utils/DGraph.h \
	$(OPENROOT)/OpenAnalysis/Utils/BaseGraph.h \
	$(OPENROOT)/OpenAnalysis/Utils/Iterator.h \
	$(OPENROOT)/OpenAnalysis/Utils/Exception.h \
	$(OPENROOT)/OpenAnalysis/Interface/IRInterface.h


#---------------------------------------------------------------------------------------------------------------------
