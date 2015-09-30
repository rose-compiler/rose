include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

if ROSE_USE_GCC_OMP
 AM_CPPFLAGS_OMP = -DROSE_GCC_OMP 
endif

if ROSE_MPI

AM_CPPFLAGS = -DROSE_MPI $(ROSE_INCLUDES) -I. -I$(compass_tooldir)/compass -I$(compass_support_dir) -I$(compass_build_tooldir)/compass $(AM_CPPFLAGS_OMP) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_build_tooldir)/compass/libCompassCheckers.la -lrose

if ROSE_USE_GCC_OMP
 LDADD += -lgomp
 CPPFLAGS += -fopenmp
endif


.C.o:
	$(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $(AM_CPPFLAGS) $(CPPFLAGS) \
        $(AM_CXXFLAGS) $(CXXFLAGS) $< -c -o $@

parallel_functionBased_dynamicBalance$(EXEEXT): $(parallel_functionBased_dynamicBalance_OBJECTS) $(parallel_functionBased_dynamicBalance_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_functionBased_dynamicBalance$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_functionBased_dynamicBalance_LDFLAGS) $(parallel_functionBased_dynamicBalance_OBJECTS) $(parallel_functionBased_dynamicBalance_LDADD) $(LIBS) \
	-o parallel_functionBased_dynamicBalance$(EXEEXT)

parallel_file_compass$(EXEEXT): $(parallel_file_compass_OBJECTS) $(parallel_file_compass_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_file_compass$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_file_compass_LDFLAGS) $(parallel_file_compass_OBJECTS) $(parallel_file_compass_LDADD) $(LIBS) \
	-o parallel_file_compass$(EXEEXT)

parallel_compass$(EXEEXT): $(parallel_compass_OBJECTS) $(parallel_compass_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_compass$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_compass_LDFLAGS) $(parallel_compass_OBJECTS) $(parallel_compass_LDADD) $(LIBS) \
	-o parallel_compass$(EXEEXT)

parallel_functionBased_ASTBalance$(EXEEXT): $(parallel_functionBased_ASTBalance_OBJECTS) $(parallel_functionBased_ASTBalance_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_functionBased_ASTBalance$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_functionBased_ASTBalance_LDFLAGS) $(parallel_functionBased_ASTBalance_OBJECTS) $(parallel_functionBased_ASTBalance_LDADD) $(LIBS) \
	-o parallel_functionBased_ASTBalance$(EXEEXT)

bin_PROGRAMS = parallel_functionBased_dynamicBalance parallel_file_compass parallel_compass parallel_functionBased_ASTBalance

parallel_functionBased_dynamicBalance_SOURCES = parallel_functionBased_dynamicBalance.C 
parallel_file_compass_SOURCES = parallel_file_compass.C
parallel_compass_SOURCES = parallel_compass.C
parallel_functionBased_ASTBalance_SOURCES = parallel_functionBased_ASTBalance.C

nodist_noinst_DATA = compass_parameters

CLEANFILES = gmon.out *.parse *.run *.ast *.ti compass_parameters

include_HEADERS = LoadSaveAST.h parallel_compass.h

check-local: parallel_functionBased_dynamicBalance parallel_file_compass parallel_compass $(srcdir)/buffer2.c compass_parameters
	mpirun -l -np 1 ./parallel_functionBased_dynamicBalance$(EXEEXT) $(srcdir)/buffer2.c
	mpirun -l -np 2 ./parallel_functionBased_dynamicBalance$(EXEEXT) $(srcdir)/buffer2.c
#	mpirun -l -np 1 ./parallel_functionBased_dynamicBalance$(EXEEXT) $(srcdir)/buffer2.c -shared
	mpirun -l -np 1 ./parallel_file_compass$(EXEEXT) $(srcdir)/buffer2.c
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) $(srcdir)/buffer2.c
	mpirun -l -np 1 ./parallel_functionBased_ASTBalance$(EXEEXT) $(srcdir)/buffer2.c
	mpirun -l -np 2 ./parallel_functionBased_ASTBalance$(EXEEXT) $(srcdir)/buffer2.c 
	mpirun -l -np 1 ./parallel_functionBased_ASTBalance$(EXEEXT) $(srcdir)/buffer2.c -shared
	mpirun -l -np 1 ./parallel_functionBased_ASTBalance$(EXEEXT) $(srcdir)/buffer2.c -combined
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) -save test.ast $(srcdir)/buffer2.c 
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) -load test.ast -sdfa
	mpirun -l -np 2 ./parallel_compass$(EXEEXT) -load test.ast -sdfa
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) -load test.ast -ldfa
	mpirun -l -np 2 ./parallel_compass$(EXEEXT) -load test.ast -ldfa
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) -load test.ast 
	mpirun -l -np 2 ./parallel_compass$(EXEEXT) -load test.ast 

compass_parameters:
	cd $(compass_build_tooldir)/compass && $(MAKE) compass_parameters
	cp $(compass_build_tooldir)/compass/compass_parameters .
#	$(LN_S) ../compass/compass_parameters compass_parameters

endif

EXTRA_DIST = create_checkers.py run.sh LoadSaveAST.h parallel_compass.h parallel_functionBased_dynamicBalance.C parallel_file_compass.C parallel_compass.C parallel_functionBased_ASTBalance.C exampleBuildcxxgrammar.run parseLine_irs.parse exampleBuildIRS.run parseLine_smg2000.parse exampleBuildSMG2000.run

