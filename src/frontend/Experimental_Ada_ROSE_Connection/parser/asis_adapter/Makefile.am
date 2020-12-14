include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


all:libdot_asis.so

libdot_asis.so:
	gprbuild -p -P$(srcdir)/dot_asis_library.gpr -v --relocate-build-tree

install-data-local: libdot_asis.so
	$(INSTALL_DATA) -D $(builddir)/lib/libdot_asis.so $(libdir)/libdot_asis.so

clean:
	rm -rf lib obj
