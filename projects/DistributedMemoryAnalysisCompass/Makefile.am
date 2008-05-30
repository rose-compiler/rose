include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_MPI

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/compass -I$(top_srcdir)/projects/compass/compassSupport -I../compass

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) ../compass/libCompassCheckers.la

.C.o:
	$(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
        $(AM_CXXFLAGS) $(CXXFLAGS) $< -c -o $@

parallel_compass$(EXEEXT): $(parallel_compass_OBJECTS) $(parallel_compass_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_compass$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_compass_LDFLAGS) $(parallel_compass_OBJECTS) $(parallel_compass_LDADD) $(LIBS) \
	-o parallel_compass$(EXEEXT)

parallel_file_compass$(EXEEXT): $(parallel_file_compass_OBJECTS) $(parallel_file_compass_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_file_compass$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_file_compass_LDFLAGS) $(parallel_file_compass_OBJECTS) $(parallel_file_compass_LDADD) $(LIBS) \
	-o parallel_file_compass$(EXEEXT)

parallel_defuse_compass$(EXEEXT): $(parallel_defuse_compass_OBJECTS) $(parallel_defuse_compass_DEPENDENCIES)
	@echo copying parse files
	cp $(srcdir)/*.parse .
	cp $(srcdir)/*.run .
	@rm -f parallel_defuse_compass$(EXEEXT)
	$(LIBTOOL) --mode=link --tag=CXX $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) \
	$(parallel_defuse_compass_LDFLAGS) $(parallel_defuse_compass_OBJECTS) $(parallel_defuse_compass_LDADD) $(LIBS) \
	-o parallel_defuse_compass$(EXEEXT)

bin_PROGRAMS = parallel_compass parallel_file_compass parallel_defuse_compass

parallel_compass_SOURCES = parallel_compass.C 
parallel_file_compass_SOURCES = parallel_file_compass.C
parallel_defuse_compass_SOURCES = parallel_defuse_compass.C

nodist_noinst_DATA = compass_parameters

CLEANFILES = gmon.out *.parse *.run *.ast *.ti

check-local: parallel_compass parallel_file_compass $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C compass_parameters
	mpirun -l -np 1 ./parallel_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
	mpirun -l -np 2 ./parallel_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
#	mpirun -l -np 1 ./parallel_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C -shared
	mpirun -l -np 1 ./parallel_file_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C
	mpirun -l -np 1 ./parallel_defuse_compass$(EXEEXT) $(top_srcdir)/projects/compass/nullDeref/nullDerefTest1.C

compass_parameters: $(srcdir)/compass_parameters_x
	$(LN_S) $(srcdir)/compass_parameters_x compass_parameters

endif
