include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

run_asis_tool_2:
	gprbuild -p \
	-aP $(srcdir)/../dot_asis_library \
	-P$(srcdir)/dot_asis.gpr -v \
	-XLIBRARY_TYPE=stati \
	-XASIS_BUILD=static \
	--relocate-build-tree  \
	$@

clean:
	rm -rf obj

