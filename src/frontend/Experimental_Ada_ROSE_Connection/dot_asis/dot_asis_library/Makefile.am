include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


all:libdot_asis.so

libdot_asis.so:
	gprbuild -p -P$(srcdir)/dot_asis_library.gpr -v --relocate-build-tree

clean:
	rm -rf lib obj
