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

TRTEST         = trtest
GRTEST         = grtest
ALLOBJS        = BaseGraph.o Graph.o DGraph.o Tree.o UnionFindUniverse.o
GRTESTOBJS     = grtest.o $(ALLOBJS)
TRTESTOBJS     = trtest.o Tree.o
PARSEROBJS     = Tree.o
#---------------------------------------------------------------------------------------------------------------------


#---------------------------------------------------------------------------------------------------------------------

all: $(ALLOBJS)

alltests: $(GRTEST) $(TRTEST)

parser: $(PARSEROBJS)

$(TRTEST): $(TRTESTOBJS)
	$(CC) -o $(TRTEST) $(TRTESTOBJS) $(LIBS)

$(GRTEST): $(GRTESTOBJS)
	$(CC) -o $(GRTEST) $(GRTESTOBJS) $(LIBS)

.C.o:
	$(CC) -c $<

clean:
	/bin/rm -f $(GRTEST) $(GRTEST).o $(TRTEST) $(TRTEST).o $(ALLOBJS)

listobjs:
	@echo $(ALLOBJS)

# header file dependencies
BaseGraph.o : BaseGraph.h Iterator.h Exception.h
Graph.o : Graph.h Iterator.h Exception.h BaseGraph.h
DGraph.o : DGraph.h Iterator.h Exception.h BaseGraph.h
Tree.o : Tree.h Iterator.h Exception.h
UnionFindUniverse.o : UnionFindUniverse.h

#---------------------------------------------------------------------------------------------------------------------
