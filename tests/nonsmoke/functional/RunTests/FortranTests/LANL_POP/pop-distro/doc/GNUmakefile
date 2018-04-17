#
# File:  GNUmakefile
#
#----------------------------------------------------------------------------
#
#
#----------------------------------------------------------------------------
#
#  The bourne shell is safest for gmake
SHELL = /bin/sh
#
#  First clean out current list of suffixes, then define them
.SUFFIXES: 
.SUFFIXES: .tex .dvi .eps .ps

all: IOdesign.ps

IOdesign.ps: IOdesign.dvi
	@dvips -f IOdesign.dvi > IOdesign.ps

IOdesign.dvi: IOdesign.tex io.eps
	@latex IOdesign

