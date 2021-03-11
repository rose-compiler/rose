include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS := obj/run_asis_tool_2

obj/run_asis_tool_2$(EXEEXT):
	gprbuild \
	-p \
	-aP $(srcdir)/../dot_asis_library \
	-P$(srcdir)/dot_asis.gpr \
	-v \
	--relocate-build-tree=.  \
	run_asis_tool_2

clean:
	rm -rf obj

