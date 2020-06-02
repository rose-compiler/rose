include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

bin_PROGRAMS =
tests =

libbatSupport_la_SOURCES = batSupport.C
pkginclude_HEADERS = batSupport.h

bin_PROGRAMS += bat-ana
bat_ana_SOURCES = bat-ana.C
bat_ana_CPPFLAGS = $(ROSE_INCLUDES)
bat_ana_LDFLAGS = $(ROSE_RPATHS)
bat_ana_LDADD = $(ROSE_LIBS)
tests += bat-ana.passed

bin_PROGRAMS += bat-cc
bat_cc_SOURCES = bat-cc.C
bat_cc_CPPFLAGS = $(ROSE_INCLUDES)
bat_cc_LDFLAGS = $(ROSE_RPATHS)
bat_cc_LDADD = $(ROSE_LIBS)
tests += bat-cc.pased

bin_PROGRAMS += bat-cfg
bat_cfg_SOURCES = bat-cfg.C
bat_cfg_CPPFLAGS = $(ROSE_INCLUDES)
bat_cfg_LDFLAGS = $(ROSE_RPATHS)
bat_cfg_LDADD = $(ROSE_LIBS)
tests += bat-cfg.passed

bin_PROGRAMS += bat-container
bat_container_SOURCES = bat-container.C
bat_container_CPPFLAGS = $(ROSE_INCLUDES)
bat_container_LDFLAGS = $(ROSE_RPATHS)
bat_container_LDADD = $(ROSE_LIBS)
tests += bat-container.passed

bin_PROGRAMS += bat-dis
bat_dis_SOURCES = bat-dis.C
bat_dis_CPPFLAGS = $(ROSE_INCLUDES)
bat_dis_LDFLAGS = $(ROSE_RPATHS)
bat_dis_LDADD = $(ROSE_LIBS)
tests += bat-dis.passed

bin_PROGRAMS += bat-entropy
bat_entropy_SOURCES = bat-entropy.C
bat_entropy_CPPFLAGS = $(ROSE_INCLUDES)
bat_entropy_LDFLAGS = $(ROSE_RPATHS)
bat_entropy_LDADD = $(ROSE_LIBS)
tests += bat-entropy.passed

bin_PROGRAMS += bat-linear
bat_linear_SOURCES = bat-linear.C
bat_linear_CPPFLAGS = $(ROSE_INCLUDES)
bat_linear_LDFLAGS = $(ROSE_RPATHS)
bat_linear_LDADD = $(ROSE_LIBS)
tests += bat-linear.passed

bin_PROGRAMS += bat-lsb
bat_lsb_SOURCES = bat-lsb.C
bat_lsb_CPPFLAGS = $(ROSE_INCLUDES)
bat_lsb_LDFLAGS = $(ROSE_RPATHS)
bat_lsb_LDADD = $(ROSE_LIBS)
tests += bat-lsb.passed

bin_PROGRAMS += bat-lsf
bat_lsf_SOURCES = bat-lsf.C
bat_lsf_CPPFLAGS = $(ROSE_INCLUDES)
bat_lsf_LDFLAGS = $(ROSE_RPATHS)
bat_lsf_LDADD = $(ROSE_LIBS)
tests += bat-lsf.passed

bin_PROGRAMS += bat-mem
bat_mem_SOURCES = bat-mem.C
bat_mem_CPPFLAGS = $(ROSE_INCLUDES)
bat_mem_LDFLAGS = $(ROSE_RPATHS)
bat_mem_LDADD = $(ROSE_LIBS)
tests += bat-mem.passed

bin_PROGRAMS += bat-prop
bat_prop_SOURCES = bat-prop.C
bat_prop_CPPFLAGS = $(ROSE_INCLUDES)
bat_prop_LDFLAGS = $(ROSE_RPATHS)
bat_prop_LDADD = $(ROSE_LIBS)
tests += bat-prop.passed

bin_PROGRAMS += bat-stack-deltas
bat_stack_deltas_SOURCES = bat-stack-deltas.C
bat_stack_deltas_CPPFLAGS = $(ROSE_INCLUDES)
bat_stack_deltas_LDFLAGS = $(ROSE_RPATHS)
bat_stack_deltas_LDADD = $(ROSE_LIBS)
tests += bat-stack.passed

bin_PROGRAMS += bat-trace
bat_trace_SOURCES = bat-trace.C
bat_trace_CPPFLAGS = $(ROSE_INCLUDES)
bat_trace_LDFLAGS = $(ROSE_RPATHS)
bat_trace_LDADD = $(ROSE_LIBS)
tests += bat-trace.passed

$(tests): %.passed: %
	@$(RTH_RUN)						\
		TITLE="testing $* [$@]"				\
		CMD="$$(pwd)/$* --self-test"			\
		$(top_srcdir)/scripts/test_exit_status $@

endif
