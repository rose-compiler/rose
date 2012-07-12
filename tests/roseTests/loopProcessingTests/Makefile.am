include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/loopProcessing/driver -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/astSupport

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = LoopProcessor

LoopProcessor_SOURCES = LoopProcessor.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

ROSE_FLAGS =

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND = valgrind $(VALGRIND_OPTIONS)

# This file is an object file created by the misinterpretation of a -opt flag
CLEANFILES = pt

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:
if USING_GNU_COMPILER
#	This fails for the Intel icc or icpc compiler (restrict pointers are unparsed differently which causes dif to fail).
	$(srcdir)/TestDriver $(srcdir)
endif
	echo "Commented out loopProcessor due to internal problems..."

EXTRA_DIST = TestDriver mm.C fusiontest1.C lufac.C tridvpk.C rmatmult3.C dgemm.C rose_mm.C.wave-save rose_mm.C.withoutwave-save rose_mm.C.save rose_lufac.C.save rose_lufac_split.C.save rose_tridvpk.C.save rose_rmatmult3.C.save rose_dgemm.C.save rose_fusiontest1.C.save rose_mm_cp0.C.save rose_lufac_cp0.C.save rose_mm_cp2_bk3.C.save funcs.annot rose_mm.C.wave-save rose_mm.C.withoutwave-save rose_lufac.C.wave-save rose_lufac.C.withoutwave-save rose_lufac_split.C.wave-save rose_lufac_split.C.withoutwave-save rose_tridvpk.C.wave-save rose_tridvpk.C.withoutwave-save rose_rmatmult3.C.wave-save rose_rmatmult3.C.withoutwave-save rose_mm.C.wave-save rose_mm.C.withoutwave-save rose_mm_cp0.C.wave-save rose_mm_cp0.C.withoutwave-save rose_lufac_cp0.C.wave-save rose_lufac_cp0.C.withoutwave-save rose_mm_cp2_bk3.C.wave-save rose_mm_cp2_bk3.C.withoutwave-save  dgemvT.C rose_dgemvT.C.save dgemm_test.C rose_dgemm_test.C.save rose_lufac_12.C.save

test:
	$(VALGRIND) ./LoopProcessor --edg:no_warnings -w -bs 60 -fs01 $(srcdir)/rmatmult3.C

# DQ (3/28/2006): These test fail becuase g++ version 4.x causes too 
# many zeros to be output in strings (different from g++ version 3.3.2)
# This will be fixed once we can output the constant literals as strings 
# using Andres's work using Wave.

# DQ (7/25/2005): Commented out so I can work on separately with Qing
# to deactivate tests comment out check-local
check-local:
#	chmod +w $(srcdir)
if ROSE_USE_BOOST_WAVE
#	cp $(srcdir)/rose_mm.C.wave-save $(srcdir)/rose_mm.C.save
#	cp $(srcdir)/rose_lufac.C.wave-save $(srcdir)/rose_lufac.C.save
#	cp $(srcdir)/rose_lufac_split.C.wave-save $(srcdir)/rose_lufac_split.C.save
#	cp $(srcdir)/rose_tridvpk.C.wave-save $(srcdir)/rose_tridvpk.C.save
#	cp $(srcdir)/rose_rmatmult3.C.wave-save $(srcdir)/rose_rmatmult3.C.save
#	cp $(srcdir)/rose_mm.C.wave-save $(srcdir)/rose_mm.C.save
#	cp $(srcdir)/rose_mm_cp0.C.wave-save $(srcdir)/rose_mm_cp0.C.save
#	cp $(srcdir)/rose_lufac_cp0.C.wave-save $(srcdir)/rose_lufac_cp0.C.save
#	cp $(srcdir)/rose_mm_cp2_bk3.C.wave-save $(srcdir)/rose_mm_cp2_bk3.C.save
else
#	DQ (1/3/2008): Commented out for now. The distcheck directory is readonly and this is a problem!
#	cp $(srcdir)/rose_mm.C.withoutwave-save $(srcdir)/rose_mm.C.save
#	cp $(srcdir)/rose_lufac.C.withoutwave-save $(srcdir)/rose_lufac.C.save
#	cp $(srcdir)/rose_lufac_split.C.withoutwave-save $(srcdir)/rose_lufac_split.C.save
#	cp $(srcdir)/rose_tridvpk.C.withoutwave-save $(srcdir)/rose_tridvpk.C.save
#	cp $(srcdir)/rose_rmatmult3.C.withoutwave-save $(srcdir)/rose_rmatmult3.C.save
#	cp $(srcdir)/rose_mm.C.withoutwave-save $(srcdir)/rose_mm.C.save
#	cp $(srcdir)/rose_mm_cp0.C.withoutwave-save $(srcdir)/rose_mm_cp0.C.save
#	cp $(srcdir)/rose_lufac_cp0.C.withoutwave-save $(srcdir)/rose_lufac_cp0.C.save
#	cp $(srcdir)/rose_mm_cp2_bk3.C.withoutwave-save $(srcdir)/rose_mm_cp2_bk3.C.save
endif
	$(MAKE) FORCE_TEST_CODES_TO_RUN
	@echo "*******************************************************************************************"
	@echo "*** ROSE/tests/roseTests/loopProcessing: make check rule complete (terminated normally) ***"
	@echo "*******************************************************************************************"

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

#dist-hook:
#	ln -s ../STL-link ${distdir}
# AUTOMAKE_OPTIONS = foreign







