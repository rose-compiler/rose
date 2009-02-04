include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS=-rose:verbose 2 -g3 

ROSE_FLAGS+=-rose:hpct:prof $(srcdir)/jacobi-raw.xml 
# the eqpath has to be the absolute path 
ROSE_FLAGS+=-rose:hpct:eqpath .=`cd $(srcdir) && pwd`

# test command line options for outliner
ROSE_FLAGS+= -rose:outline:parameter_wrapper -rose:outline:new_file


rose_jacobi.c:../autoTuning
	../autoTuning -c  $(ROSE_FLAGS) $(srcdir)/jacobi.c

../autoTuning:
	make -C ../.

EXTRA_DIST = jacobi-raw.xml jacobi.c

clean-local:
	rm -f *.o rose_*.[cC] *.dot 

#-------------------------------
if ROSE_BUILD_ROSEHPCT
check-local:
	@echo "Test for auto tuning... "
	@$(MAKE) rose_jacobi.c
else
#-------------------------------
check-local:
	@echo "Test for auto tuning is skipped"
endif 	

