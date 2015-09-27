include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = SPINSlice 

# DQ (5/5/2007): This does not link properly, need to talk to Christian
# CtoPromelaTransformation
# CtoPromelaTransformation_SOURCES = CtoPromelaTransformation.C 
# CtoPromelaTransformation_DEPENDENCIES = MPICallListHelper.h

SPINSlice_SOURCES = SPINSlice.C
SPINSlice_DEPENDENCIES = MPICallListHelper.h

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

include_HEADERS = \
    changeLoops.h      CToProMeLaASTAttrib.h  CToProMeLaUnparser.h     MarkAST4Unparse.h  \
    MPISpinReplacement.h  PropagatePromelaTargetTraversal.h  SpinPStructContainer.h  tools.h \
    CStateASTAttrib.h  CToProMeLa.h           IdentifyInlineTargets.h  MPI_ASTAttrib.h    \
    PromelaMarker.h       ReplaceCVarRefExp.h                StateVecVarAttrib.h     VisPromelaAST.h

more_source_files = \
    changeLoops.C CToProMeLaASTAttrib.C CtoPromelaTransformation.C  IdentifyInlineTargets.C \
    MPI_ASTAttrib.C  PromelaMarker.C  SpinPStructContainer.C  StateVecVarAttrib.C \
    unparser.C VisPromelaAST.C CStateASTAttrib.C  CToProMeLa.C  CToProMeLaUnparser.C  \
    MarkAST4Unparse.C MPISpinReplacement.C  PropagatePromelaTargetTraversal.C  \
    tools.C  varTransformation.C

EXTRA_DIST = $(TESTCODES) mpiMapGenerator spinMe mpiCallList.lst tests $(more_source_files)

# Generated header file for MPI prototypes
MPICallListHelper.h:	$(srcdir)/mpiCallList.lst $(srcdir)/mpiMapGenerator
	perl $(srcdir)/mpiMapGenerator


clean-local:
	rm -f *.o rose_*.C MPICallListHelper.h
