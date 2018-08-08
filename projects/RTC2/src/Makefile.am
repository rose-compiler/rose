#
# Runtime Error Checking C Programs (RTC)
#

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = $(ROSE_INCLUDES)
rtc_LDADD = $(ROSE_LIBS)

bin_PROGRAMS = rtc

rtc_SOURCES = \
    instrumenter/funcDecl.C \
    instrumenter/instrCallExps.C \
    instrumenter/instrNewExps.C \
    instrumenter/instrScopeStmts.C \
    instrumenter/instrUnaryOps.C \
    instrumenter/RTC.C \
    instrumenter/typechecking.C \
    instrumenter/instrBinaryOps.C \
    instrumenter/instrDeclStmts.C \
    instrumenter/instrRefExps.C \
    instrumenter/instrSizeOfOps.C \
    instrumenter/instrVarRefs.C \
    instrumenter/strDecl.C \
    instrumenter/util.C \
    instrumenter/instr.C \
    instrumenter/instrDeleteExps.C \
    instrumenter/instrRetStmts.C \
    instrumenter/instrThisExps.C \
    instrumenter/preProcessAST.C \
    instrumenter/traverse.C
