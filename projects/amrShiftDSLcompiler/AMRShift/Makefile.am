
# CPPFLAGS += -Wunknown-pragma

CXX11FLAG = -std=c++11 
CPPFLAGS += $(CXX11FLAG) -D DIM=2 -D MINBOXSIZE_EXP=2 -D MAXBLOCKSIZE_EXP=6 -D MYDEBUGFLAG=true -O3 -DNDEBUG 

noinst_LTLIBRARIES = libamrShift.la

libamrShift_la_SOURCES = \
   Multigrid/Multigrid.cpp Multigrid/MultigridStencils.cpp StencilTools/BoxLayout.cpp StencilTools/BoxLayout.cpp \
   StencilTools/PowerItoI.cpp StencilTools/Box.cpp StencilTools/BLIterator.cpp \
   VisitWriter/WriteRectMDArray.cpp VisitWriter/VisitWriter.cpp timer/CH_Timer.cpp

# multigridTest.cpp

AM_CPPFLAGS = -I$(srcdir)/Multigrid -I$(srcdir)/StencilTools -I$(srcdir)/VisitWriter -I$(srcdir)/timer


