include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


all:libdot_asis.so

libdot_asis.so:
	gprls -v
	gprbuild \
	-p \
	-aP `which gprbuild`/../../share/gpr \
	-P$(srcdir)/asis_adapter.gpr \
	-v \
	--relocate-build-tree

install-data-local: libdot_asis.so
	$(INSTALL_DATA) -D $(builddir)/lib/asis_adapter.so $(libdir)/asis_adapter.so

clean:
	rm -rf lib obj
