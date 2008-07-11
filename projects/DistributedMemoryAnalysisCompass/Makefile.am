include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_MPI

INCLUDES = -DROSE_MPI $(ROSE_INCLUDES) -I$(top_srcdir)/projects/compass -I$(top_srcdir)/projects/compass/compassSupport -I../compass
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) ../compass/libCompassCheckers.la 

if ROSE_USE_GCC_OMP
 LDADD += -lgomp
 CPPFLAGS += -fopenmp
endif


.C.o:
	$(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
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

check-local: parallel_functionBased_dynamicBalance parallel_file_compass parallel_compass parallel_functionBased_ASTBalance $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C compass_parameters
	mpirun -l -np 1 ./parallel_functionBased_dynamicBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
	mpirun -l -np 2 ./parallel_functionBased_dynamicBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
#	mpirun -l -np 1 ./parallel_functionBased_dynamicBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C -shared
	mpirun -l -np 1 ./parallel_file_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
	mpirun -l -np 1 ./parallel_functionBased_ASTBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
	mpirun -l -np 2 ./parallel_functionBased_ASTBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C $(top_srcdir)/projects/compass/nullDeref/nullDerefTest2.C
	mpirun -l -np 1 ./parallel_functionBased_ASTBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C -shared
	mpirun -l -np 1 ./parallel_functionBased_ASTBalance$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C -combined

compass_parameters:
	$(LN_S) ../compass/compass_parameters compass_parameters

endif
