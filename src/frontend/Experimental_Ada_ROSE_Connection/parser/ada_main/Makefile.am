include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS := obj/run_parser_adapter

obj/run_asis_tool_2$(EXEEXT):
	gprbuild \
	-p \
	-aP $(srcdir)/../asis_adapter \
	-P$(srcdir)/ada_main.gpr \
	-v \
	--relocate-build-tree=.  \
	run_parser_adapter

clean:
	rm -rf obj

