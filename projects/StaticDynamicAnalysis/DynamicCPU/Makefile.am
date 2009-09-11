include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = traceCPU

traceCPU_SOURCES = traceCPU.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -f traceCPU ProjGen example

# EXTRA_DIST = introduction.tex design.tex appendix.tex \
#    bugSeeding.h bugSeedingSupport.h bugSeedingAttributes.h \
#    cloneGrainularity.h  vulnerabilityDetection.h \
#    bufferOverFlowSecurityFlaw.h inputCode_bufferOverflow_arrayIndexing.C


example: traceCPU
	$(CXX) $(srcdir)/ProjGen.C -o ProjGen
	./ProjGen example 4
	(cd example ; ln -s ../$(srcdir)/SegDB.txt SegDB.txt ; gmake srcdir=../$(srcdir) ; ./checktest >& /dev/null )

check-local: example

copy:
#	rm -rf examples
#	mkdir examples
#	cp $(srcdir)/*.[hC] examples
#	cp *.dot examples
#	cp *.pdf examples
