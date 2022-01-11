include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
SUBDIRS = BaseSemantics
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la

librose_partial_la_SOURCES =			\
    ConcreteSemantics.C				\
    DataFlowSemantics.C				\
    DispatcherAarch32.C				\
    DispatcherAarch64.C				\
    DispatcherM68k.C				\
    DispatcherPowerpc.C				\
    DispatcherX86.C				\
    IntervalSemantics.C				\
    LlvmSemantics.C				\
    MultiSemantics.C				\
    NativeSemantics.C				\
    NullSemantics.C				\
    PartialSymbolicSemantics.C			\
    SourceAstSemantics.C			\
    StaticSemantics.C				\
    SymbolicSemantics.C				\
    TraceSemantics.C				\
    Util.C
