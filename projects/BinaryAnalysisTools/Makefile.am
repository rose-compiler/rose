include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
EXTRA_DIST =

#------------------------------------------------------------------------------------------------------------------------
# linear disassembly at specified addresses

bin_PROGRAMS += linearDisassemble
linearDisassemble_SOURCES = linearDisassemble.C
linearDisassemble_CPPFLAGS = $(ROSE_INCLUDES)
linearDisassemble_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# x86-call-targets

bin_PROGRAMS += x86-call-targets
x86_call_targets_SOURCES = x86-call-targets.C
x86_call_targets_CPPFLAGS = $(ROSE_INCLUDES)
x86_call_targets_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# x86-function-vas

bin_PROGRAMS += x86-function-vas
x86_function_vas_SOURCES = x86-function-vas.C
x86_function_vas_CPPFLAGS = $(ROSE_INCLUDES)
x86_function_vas_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

