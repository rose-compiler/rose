include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS=-rose:verbose 0 -g3 

ROSE_PROF1 =-rose:hpct:prof $(srcdir)/jacobi-raw.xml 
ROSE_PROF2 =-rose:hpct:prof $(srcdir)/jacobi-raw-omp.xml 

ROSE_PROF3 =-rose:gprof:linebyline $(srcdir)/jacobi.gprof.txt
# the eqpath has to be the absolute path 
EQUAL_PATH1=-rose:hpct:eqpath .=`cd $(srcdir) && pwd`
# gprof result's equavilent path
# The performance data can be collected from another machine with different paths
EQUAL_PATH3=-rose:hpct:eqpath "/home/liao6/temp"=`cd $(srcdir) && pwd`

# test command line options for outliner, They are set automatically by the translator now
#ROSE_FLAGS+= -rose:outline:parameter_wrapper -rose:outline:new_file -rose:outline:temp_variable -rose:outline:exclude_headers 
ROSE_FLAGS+=-rose:outline:enable_liveness -I$(top_srcdir)/projects/autoTuning
# source file annotation with performance metrics
# -rose:hpct:enable_debug -rose:autotuning:enable_debug 
#ROSE_FLAGS+=-rose:autotuning:triage_threshold 0.75

AT_BUILD=$(top_builddir)/projects/autoTuning/tests
AT_FLAGS=-rose:outline:output_path "$(AT_BUILD)"
AT_INCLUDES=-I$(top_srcdir)/src/frontend/SageIII
# experimental flags. not well sorted out for the interaction between OpenMP and outlining
# TODO reduction clause with omp for  should ignored if all reduction variables are no longer shared
# this should be fixed in outliner's Transform.cc: 
# OmpSupport::generatePragmaFromOmpAttribute(firstloop);
#
ROSE_FLAGS2 = -rose:autotuning:aggressive_triage -rose:openmp

# basic code triage based on inner-most consecutive loops
rose_jacobi.c:../autoTuning
	../autoTuning -c  $(ROSE_FLAGS) $(AT_FLAGS) $(EQUAL_PATH1) $(ROSE_PROF1) $(srcdir)/jacobi.c

# aggressive code triage based on outer most OpenMP for loop
rose_jacobi-omp.c:../autoTuning
	../autoTuning -c  $(ROSE_FLAGS) $(AT_FLAGS) $(AT_INCLUDES) $(EQUAL_PATH1) $(ROSE_PROF2) $(ROSE_FLAGS2) $(srcdir)/jacobi-omp.c

# alternative performance information collected by GNU gprof -L -l, line-by-line profiling results
rose_jacobi2.c:../autoTuning
	../autoTuning  -c -rose:output rose_jacobi2.c $(ROSE_FLAGS) $(AT_FLAGS) $(EQUAL_PATH3) $(ROSE_PROF3) $(srcdir)/jacobi.c

# And using outlining support dlopen(): this is not necessary now since autoTuning will turn on -rose:outline:use_dlopen transparently
rose_jacobi3.c:../autoTuning
	../autoTuning  -c -rose:outline:use_dlopen -rose:output $@ $(ROSE_FLAGS) $(AT_FLAGS) $(EQUAL_PATH3) $(ROSE_PROF3) $(srcdir)/jacobi.c 

# test code triage only
rose_jacobi4.c:../autoTuning
	../autoTuning  -c -rose:output rose_jacobi4.c -rose:autotuning:triage_only $(ROSE_FLAGS) $(EQUAL_PATH3) $(ROSE_PROF3) $(srcdir)/jacobi.c

../autoTuning:
	make -C ../.

EXTRA_DIST = jacobi-raw.xml jacobi.c jacobi.gprof.txt makelib jacobi-omp.c

clean-local:
	rm -f *.o rose_*.[cC] *.dot  OUT_*

#-------------------------------
if ROSE_BUILD_ROSEHPCT

# DQ (2/5/2011): These could be run in parallel...or is there a problem with that?
check-local:
	@echo "Test for auto tuning... "
	@$(MAKE) rose_jacobi.c
	@$(MAKE) rose_jacobi-omp.c
	@$(MAKE) rose_jacobi2.c
	@$(MAKE) rose_jacobi3.c
	@$(MAKE) rose_jacobi4.c
	@echo "Test for auto tuning terminated normally"
else
#-------------------------------
check-local:
	@echo "Test for auto tuning is skipped"
endif 	

