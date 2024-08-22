include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

bin_PROGRAMS =
tests =

lib_LIBRARIES = libbatSupport.a
libbatSupport_a_SOURCES = batSupport.C
libbatSupport_a_CPPFLAGS = $(ROSE_INCLUDES)
pkginclude_HEADERS = batSupport.h

bin_PROGRAMS += bat-ana
bat_ana_SOURCES = bat-ana.C
bat_ana_CPPFLAGS = $(ROSE_INCLUDES)
bat_ana_LDFLAGS = $(ROSE_RPATHS)
bat_ana_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-ana.passed

bin_PROGRAMS += bat-cc
bat_cc_SOURCES = bat-cc.C
bat_cc_CPPFLAGS = $(ROSE_INCLUDES)
bat_cc_LDFLAGS = $(ROSE_RPATHS)
bat_cc_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-cc.passed

bin_PROGRAMS += bat-cfg
bat_cfg_SOURCES = bat-cfg.C
bat_cfg_CPPFLAGS = $(ROSE_INCLUDES)
bat_cfg_LDFLAGS = $(ROSE_RPATHS)
bat_cfg_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-cfg.passed

bin_PROGRAMS += bat-cg
bat_cg_SOURCES = bat-cg.C
bat_cg_CPPFLAGS = $(ROSE_INCLUDES)
bat_cg_LDFLAGS = $(ROSE_RPATHS)
bat_cg_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-cg.passed

bin_PROGRAMS += bat-check-execution
bat_check_execution_SOURCES = bat-check-execution.C
bat_check_execution_CPPFLAGS = $(ROSE_INCLUDES)
bat_check_execution_LDFLAGS = $(ROSE_RPATHS)
bat_check_execution_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-check-execution.passed

bin_PROGRAMS += bat-container
bat_container_SOURCES = bat-container.C
bat_container_CPPFLAGS = $(ROSE_INCLUDES)
bat_container_LDFLAGS = $(ROSE_RPATHS)
bat_container_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-container.passed

bin_PROGRAMS += bat-dataflow
bat_dataflow_SOURCES = bat-dataflow.C
bat_dataflow_CPPFLAGS = $(ROSE_INCLUDES)
bat_dataflow_LDFLAGS = $(ROSE_RPATHS)
bat_dataflow_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-dataflow.passed

bin_PROGRAMS += bat-debug-semantics
bat_debug_semantics_SOURCES = bat-debug-semantics.C
bat_debug_semantics_CPPFLAGS = $(ROSE_INCLUDES)
bat_debug_semantics_LDFLAGS = $(ROSE_RPATHS)
bat_debug_semantics_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-debug-semantics.passed

bin_PROGRAMS += bat-delta-bijection
bat_delta_bijection_SOURCES = bat-delta-bijection.C
bat_delta_bijection_CPPFLAGS = $(ROSE_INCLUDES)
bat_delta_bijection_LDFLAGS = $(ROSE_RPATHS)
bat_delta_bijection_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-delta-bijection.passed

bin_PROGRAMS += bat-dis
bat_dis_SOURCES = bat-dis.C
bat_dis_CPPFLAGS = $(ROSE_INCLUDES)
bat_dis_LDFLAGS = $(ROSE_RPATHS)
bat_dis_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-dis.passed

bin_PROGRAMS += bat-dwarf-lines
bat_dwarf_lines_SOURCES = bat-dwarf-lines.C
bat_dwarf_lines_CPPFLAGS = $(ROSE_INCLUDES)
bat_dwarf_lines_LDFLAGS = $(ROSE_RPATHS)
bat_dwarf_lines_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-dwarf-lines.passed

bin_PROGRAMS += bat-entropy
bat_entropy_SOURCES = bat-entropy.C
bat_entropy_CPPFLAGS = $(ROSE_INCLUDES)
bat_entropy_LDFLAGS = $(ROSE_RPATHS)
bat_entropy_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-entropy.passed

bin_PROGRAMS += bat-insnfreq
bat_insnfreq_SOURCES = bat-insnfreq.C
bat_insnfreq_CPPFLAGS = $(ROSE_INCLUDES)
bat_insnfreq_LDFLAGS = $(ROSE_RPATHS)
bat_insnfreq_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-insnfreq.passed

bin_PROGRAMS += bat-insnfreq-cmp
bat_insnfreq_cmp_SOURCES = bat-insnfreq-cmp.C
bat_insnfreq_cmp_CPPFLAGS = $(ROSE_INCLUDES)
bat_insnfreq_cmp_LDFLAGS = $(ROSE_RPATHS)
bat_insnfreq_cmp_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-insnfreq-cmp.passed

bin_PROGRAMS += bat-flir-ascribe
bat_flir_ascribe_SOURCES = bat-flir-ascribe.C
bat_flir_ascribe_CPPFLAGS = $(ROSE_INCLUDES)
bat_flir_ascribe_LDFLAGS = $(ROSE_RPATHS)
bat_flir_ascribe_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-flir-ascribe.passed

bin_PROGRAMS += bat-flir-insert
bat_flir_insert_SOURCES = bat-flir-insert.C
bat_flir_insert_CPPFLAGS = $(ROSE_INCLUDES)
bat_flir_insert_LDFLAGS = $(ROSE_RPATHS)
bat_flir_insert_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-flir-insert.passed

bin_PROGRAMS += bat-flir-lib
bat_flir_lib_SOURCES = bat-flir-lib.C
bat_flir_lib_CPPFLAGS = $(ROSE_INCLUDES)
bat_flir_lib_LDFLAGS = $(ROSE_RPATHS)
bat_flir_lib_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-flir-lib.passed

bin_PROGRAMS += bat-linear
bat_linear_SOURCES = bat-linear.C
bat_linear_CPPFLAGS = $(ROSE_INCLUDES)
bat_linear_LDFLAGS = $(ROSE_RPATHS)
bat_linear_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-linear.passed

bin_PROGRAMS += bat-lsb
bat_lsb_SOURCES = bat-lsb.C
bat_lsb_CPPFLAGS = $(ROSE_INCLUDES)
bat_lsb_LDFLAGS = $(ROSE_RPATHS)
bat_lsb_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-lsb.passed

bin_PROGRAMS += bat-lsd
bat_lsd_SOURCES = bat-lsd.C
bat_lsd_CPPFLAGS = $(ROSE_INCLUDES)
bat_lsd_LDFLAGS = $(ROSE_RPATHS)
bat_lsd_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-lsd.passed

bin_PROGRAMS += bat-lsf
bat_lsf_SOURCES = bat-lsf.C
bat_lsf_CPPFLAGS = $(ROSE_INCLUDES)
bat_lsf_LDFLAGS = $(ROSE_RPATHS)
bat_lsf_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-lsf.passed

bin_PROGRAMS += bat-lsv
bat_lsv_SOURCES = bat-lsv.C
bat_lsv_CPPFLAGS = $(ROSE_INCLUDES)
bat_lsv_LDFLAGS = $(ROSE_RPATHS)
bat_lsv_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-lsv.passed

bin_PROGRAMS += bat-mem
bat_mem_SOURCES = bat-mem.C
bat_mem_CPPFLAGS = $(ROSE_INCLUDES)
bat_mem_LDFLAGS = $(ROSE_RPATHS)
bat_mem_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-mem.passed

bin_PROGRAMS += bat-native-trace
bat_native_trace_SOURCES = bat-native-trace.C
bat_native_trace_CPPFLAGS = $(ROSE_INCLUDES)
bat_native_trace_LDFLAGS = $(ROSE_RPATHS)
bat_native_trace_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-native-trace.passed

bin_PROGRAMS += bat-pointers
bat_pointers_SOURCES = bat-pointers.C
bat_pointers_CPPFLAGS = $(ROSE_INCLUDES)
bat_pointers_LDFLAGS = $(ROSE_RPATHS)
bat_pointers_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-pointers.passed

bin_PROGRAMS += bat-prop
bat_prop_SOURCES = bat-prop.C
bat_prop_CPPFLAGS = $(ROSE_INCLUDES)
bat_prop_LDFLAGS = $(ROSE_RPATHS)
bat_prop_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-prop.passed

bin_PROGRAMS += bat-scan-magic
bat_scan_magic_SOURCES = bat-scan-magic.C
bat_scan_magic_CPPFLAGS = $(ROSE_INCLUDES)
bat_scan_magic_LDFLAGS = $(ROSE_RPATHS)
bat_scan_magic_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-scan-magic.passed

bin_PROGRAMS += bat-similar-functions
bat_similar_functions_SOURCES = bat-similar-functions.C
bat_similar_functions_CPPFLAGS = $(ROSE_INCLUDES)
bat_similar_functions_LDFLAGS = $(ROSE_RPATHS)
bat_similar_functions_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-similar-functions.passed

bin_PROGRAMS += bat-simplify
bat_simplify_SOURCES = bat-simplify.C
bat_simplify_CPPFLAGS = $(ROSE_INCLUDES)
bat_simplify_LDFLAGS = $(ROSE_RPATHS)
bat_simplify_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-simplify.passed

bin_PROGRAMS += bat-stack-deltas
bat_stack_deltas_SOURCES = bat-stack-deltas.C
bat_stack_deltas_CPPFLAGS = $(ROSE_INCLUDES)
bat_stack_deltas_LDFLAGS = $(ROSE_RPATHS)
bat_stack_deltas_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-stack-deltas.passed

bin_PROGRAMS += bat-to-c
bat_to_c_SOURCES = bat-to-c.C
bat_to_c_CPPFLAGS = $(ROSE_INCLUDES)
bat_to_c_LDFLAGS = $(ROSE_RPATHS)
bat_to_c_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-to-c.passed

bin_PROGRAMS += bat-trace
bat_trace_SOURCES = bat-trace.C
bat_trace_CPPFLAGS = $(ROSE_INCLUDES)
bat_trace_LDFLAGS = $(ROSE_RPATHS)
bat_trace_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-trace.passed

bin_PROGRAMS += bat-var
bat_var_SOURCES = bat-var.C
bat_var_CPPFLAGS = $(ROSE_INCLUDES)
bat_var_LDFLAGS = $(ROSE_RPATHS)
bat_var_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-var.passed

bin_PROGRAMS += bat-xml-to-json
bat_xml_to_json_SOURCES = bat-xml-to-json.C
bat_xml_to_json_CPPFLAGS = $(ROSE_INCLUDES)
bat_xml_to_json_LDFLAGS = $(ROSE_RPATHS)
bat_xml_to_json_LDADD = libbatSupport.a $(ROSE_LIBS)
tests += bat-xml-to-json.passed

$(tests): %.passed: %
	@$(RTH_RUN)							\
		TITLE="testing $* [$@]"					\
		CMD="$$(pwd)/$* --self-test --no-error-if-disabled"	\
		$(top_srcdir)/scripts/test_exit_status $@

check-local: $(tests) bat-insnfreq.passed bat-insnfreq-cmp.passed

MOSTLYCLEANFILES = $(tests)
endif
