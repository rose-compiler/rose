include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la

librose_partial_la_SOURCES =			\
        BasicBlockUnit.C			\
        Engine.C				\
        ErrorTag.C				\
        ExecutionUnit.C				\
        ExternalFunctionUnit.C			\
        FailureUnit.C				\
        InstructionUnit.C			\
        NullDerefTag.C				\
	OobTag.C				\
        P2Model.C				\
        Path.C					\
        PathNode.C				\
        PathPredicate.C				\
        PathPrioritizer.C			\
        PathQueue.C				\
        Periodic.C				\
        Settings.C				\
        SemanticCallbacks.C			\
        SourceLister.C				\
        Tag.C

# Header files are installed from parent makefiles.
