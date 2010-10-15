include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


INCLUDES = $(ROSE_INCLUDES)

# Location of Thrifty simulator
THRIFTY_SIMULATOR = /home/dquinlan/PROJECTS/THRIFTY/SVN

bin_PROGRAMS = thrifty

thrifty_SOURCES = thrifty.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

testThrifty: thrifty
	./thrifty -rose:verbose 0 -c $(srcdir)/inputCode_thrifty_01.c

# Example of running the simulator
testSimulator:
	$(THRIFTY_SIMULATOR)/build/sesc.tst -h0x800000 -c../sesc/confs/mem.conf ../sesc/tests/crafty < ../sesc/tests/tt.in
	$(THRIFTY_SIMULATOR)/build/sesc.mem -h0x6000000 -c../sesc/confs/mem.conf ../sesc/tests/mcf ../sesc/tests/mcf.in


check-local: 

clean-local:
	rm -f *.o rose_*.c

EXTRA_DIST = 

