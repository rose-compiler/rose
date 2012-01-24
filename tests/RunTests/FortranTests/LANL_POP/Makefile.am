# This makefile provides targets for testing ROSE Fortran support.  It is tested by compiling LANL POP, a fortran application,
# and running the resulting "pop" executable.  It also runs certain POP source files through a ROSE-based translator,
# compiles that modified copy of POP, and runs its "pop" executable.  The output from the two executables is compared (subject
# to certain filter) and if they match then the test has passed.
#
# Some things we had to work around:
#    - NetCDF cannot be compiled into a separate build directory when its source directory is read-only.
#    - The ROSE-based translator needs write permission in the directory containing the input Fortran sources.
#    - Modification of POP is performed in place (i.e., input==output), which renders makefile dependencies a bit complex.
#    - The output from the translator is stored in the current working directory, regardless of the input path.
#
# Problems with previous implementation:
#    - Silent failure if csh was not installed--failure happens later. Resolved by replacing csh scripts.
#    - Translator could not be run in parallel since it was invoked one-at-a-time in a shell script. Resolved by moving
#      translation commands into the makefile and adding more dependencies.
#    - Silent failure when translator failed.  Intentionally not fixed since the translator currently fails on some files (see
#      the target for $(pop_modified_sources).
#
# Some useful targets:
#    - check:     	runs the whole test   (not contained here, but provided by automake)
#    - clean:     	removes all test files from this build directory (see clean-local)
#    - netcdf:    	configure, build, and locally install NetCDF
#    - netcdf_modified: modify the local NetCDF installation by adding a couple *.rmod files to the install directory
#    - pop_pristine:	configure a pristine version of POP in preparation for building the "pop" executable.
#    - pop_modified:    configure a version of POP that has some source files modified by the ROSE-based translator.
#    - DIR/run/pop:     compile "pop" executable, where X is "pop_pristine_blddir" or "pop_modified_blddir"
#    - X.ans:           run "pop" to generate a filtered answer file, where X is "pristine" or "modified".
#    - compare_answers: filtered comparison of the output from pristine and modified "pop" executables.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To see commands as they execute, set this to "-x"
#SETX=-x
SETX=

# Note that these have been modified from their respective distributions by someone prior to me.  It looks like
# build systems have been adjusted to not generate documentation, and who know what else. Unfortunately, those
# changes were never registered with our revision control system, so it might be hard to upgrade. [RPM 2011-05-18]
netcdf_srcdir=$(srcdir)/netcdf-4.1.1
pop_srcdir=$(srcdir)/pop-distro

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
if !ROSE_USING_GFORTRAN_VERSION_4_0

netcdf_blddir=$(abspath netcdf-4.1.1)
netcdf_prefix=$(netcdf_blddir)/installed
netcdf_lib=$(netcdf_prefix)/lib/libnetcdf.a

translator = $(top_builddir)/tests/testTranslator
translator_flags = -rose:verbose 0 -rose:skip_syntax_check -rose:skipfinalCompileStep


# Shell code to set up the environment.  If any of these environment variables are already set then that value
# will be kept, otherwise we'll use a sane default.  We generate this as a file because it's used by a bunch of
# other targets.
hudson-environment.sh:
	@>$@
	@echo "export HUDSON_NUM_JOBS='$${HUDSON_NUM_JOBS-1}'"						>>$@
	@echo "export HUDSON_FC='$${HUDSON_FC-$(GFORTRAN_PATH)}'"					>>$@
	@echo "export HUDSON_FFLAGS='$${HUDSON_FFLAGS--ffree-line-length-0}'"				>>$@
	@echo "export HUDSON_LD='$${HUDSON_LD-$(GFORTRAN_PATH)}'"					>>$@
	@echo "export HUDSON_CC='$${HUDSON_CC-$(CC)}'"							>>$@
	@echo "export HUDSON_Cp='$${HUDSON_Cp-$$(which cp)}'"						>>$@
	@echo "export HUDSON_Cpp='$${HUDSON_Cpp-$(CPP)}'"						>>$@
	@echo "export HUDSON_AWK='$${HUDSON_AWK-$(AWK)}'"						>>$@
	@echo "export HUDSON_LDLIBS='$${HUDSON_LDLIBS-\$$(LIBS) -lcurl}'"				>>$@
	@echo "export HUDSON_DIFF='$${HUDSON_DIFF-/usr/bin/diff}'"					>>$@
	@echo "export HUDSON_MAKE='$${HUDSON_MAKE-make}'"						>>$@
	@echo "export HUDSON_SED='$${HUDSON_SED-$(SED)}'"						>>$@
	@echo "Hudson environment: "; source $@; printenv |grep HUDSON_ |sed 's/^/    /'



###############################################################################################################################
# 					NetCDF
###############################################################################################################################

# Builds the NetCDF library required by POP. The library is configured according to the environment variables
# in hudson-environment.sh, which will be created with default values if necessary.  It is currently not possible to
# build NetCDF in a separate build directory when the source directory is read-only. Therefore, we copy the NetCDF
# source directory and add write permission to all files. [Update: It's also apparently not possible to have a build directory
# that's separate from the source directory because doing so fails with certain source files not found by make on at least one
# Hudson platform. RPM 2011-05-18]
netcdf: $(netcdf_lib)
$(netcdf_lib): hudson-environment.sh
	@set -e $(SETX);						\
	echo "  BUILD   NetCDF";					\
	source hudson-environment.sh;					\
	cp -pPR $(netcdf_srcdir) $(netcdf_blddir);			\
	chmod -R u+w $(netcdf_blddir);					\
	cd $(netcdf_blddir);						\
        ./configure --prefix=$(netcdf_prefix) --disable-netcdf-4	\
		FC=$$HUDSON_FC CC=$$HUDSON_CC CXX=$$HUDSON_CXX;		\
	$$HUDSON_MAKE -j1 all;						\
	: $$HUDSON_MAKE -j1 check;					\
	$$HUDSON_MAKE -j1 install;

# Rules to build NetCDF typesizes.rmod and netcdf.rmod and install them into the NetCDF include directory. Note that
# the typesizes.rmod must be built before netcdf.rmod since the latter depends on the former.
netcdf_modified: $(netcdf_modified_sources)
netcdf_modified_sources = $(netcdf_prefix)/include/typesizes.rmod $(netcdf_prefix)/include/netcdf.rmod

$(netcdf_prefix)/include/typesizes.rmod: $(netcdf_lib)
	$(translator) $(translator_flags) $(netcdf_blddir)/f90/typeSizes.f90
	cp typesizes.rmod $(netcdf_prefix)/include/.

$(netcdf_prefix)/include/netcdf.rmod: $(netcdf_lib) $(netcdf_prefix)/include/typesizes.rmod
	$(translator) $(translator_flags) $(netcdf_blddir)/f90/netcdf.f90
	mv netcdf.rmod $(netcdf_prefix)/include/.
	rm -f typesizes.rmod



###############################################################################################################################
#					BUILDING LANL POP (pristine and modified)
###############################################################################################################################



# Create a Hudson template file for configuring POP.  This file will be copied into POP source directories and
# is generated based on the environment stored in hudson-environment.sh (which will be created with default values
# if necessary).
hudson.gnu: hudson-environment.sh
	@set -e $(SETX);			     \
	source hudson-environment.sh;		     \
	sed < $(srcdir)/hudson.template >hudson.gnu  \
	    -e "s%_HUDSON_FC_%$$HUDSON_FC%"	     \
	    -e "s%_HUDSON_FFLAGS_%$$HUDSON_FFLAGS%"  \
	    -e "s%_HUDSON_LD_%$$HUDSON_LD%"	     \
	    -e "s%_HUDSON_CC_%$$HUDSON_CC%"	     \
	    -e "s%_HUDSON_Cp_%$$HUDSON_Cp%"	     \
	    -e "s%_HUDSON_Cpp_%$$HUDSON_Cpp%"	     \
	    -e "s%_HUDSON_AWK_%$$HUDSON_AWK%"	     \
	    -e "s%_HUDSON_LDLIBS_%$$HUDSON_LDLIBS%";


# The pop_pristine and pop_translated targets create POP build directories.  The pristine version contains source
# code directly from the POP distribution (stored in ROSE's src), while the translated version will run certain
# source files through a translator.
pop_pristine_blddir_stamp pop_modified_blddir_stamp pop_modified_srcdir_stamp: hudson-environment.sh hudson.gnu
	@set -e $(SETX);							\
	source hudson-environment.sh;						\
	blddir=$(patsubst %_stamp,%,$@);					\
	cp -pPR $(pop_srcdir) $$blddir;						\
	chmod -R u+w $$blddir;							\
	ARCHDIR=hudson $(srcdir)/setup_run_dir.ROSE $(pop_srcdir) $$blddir/run;	\
	touch $@

pop_pristine: pop_pristine_blddir_stamp
pop_modified: $(pop_modified_sources)

# A pattern rule to generate POP's translated sources.  The old mechanism ignored all failures, so we'll do the same.
# Also, the translator does not allow the F90 sources to be in a read-only directory, so we copy them to a temporary
# location first. (see pop_modified_srcdir_stamp)
$(pop_modified_sources): \
pop_modified_blddir/%.rmod: \
   $(pop_srcdir)/%.F90 $(translator) pop_modified_blddir_stamp pop_modified_srcdir_stamp $(netcdf_modified_sources)
	@set -e $(SETX);							  \
	base=$(patsubst pop_modified_blddir/%.rmod,%,$@);			  \
	echo "  GEN     $$base.rmod";						  \
	$(translator) $(translator_flags) -I$(netcdf_prefix)/include -rose:output \
	    pop_modified_blddir/$$base.F90 pop_modified_srcdir/$$base.F90 ||	  \
	echo "  ERROR   Generation of $@ failed (ignored)";			  \
	[ ! -f $(notdir $@) ] || ln -sf ../../$(notdir $@) $@

# This is the list of source files that need to be translated. Some sources depend on others, and that dependency information
# is generated automatically (but triggered manually) and placed in $(srcdir)/rmod_dependencies.  This allows us to run the
# translator in parallel.
#
# The POP modified source rules are written in terms of *.rmod files because the translator does in-place modification of
# the source file. So it's easier to depend on the newly-generated *.rmod files rather than trying to figure out if a source
# file has been modified or not.  In other words, the presence of a *.F90 file doesn't necessarily mean it's been translated,
# the the presence of FOO.rmod means FOO.F90 has been translated.
rmod_dependencies: $(srcdir)/make_rmod_deps.pl
	$(srcdir)/make_rmod_deps.pl $(pop_srcdir) $(pop_modified_sources) >$@
	@echo "Manually copy rmod_dependencies to $(srcdir) for inclusion into this makefile."
include $(srcdir)/rmod_dependencies

pop_modified_sources =					    \
	pop_modified_blddir/source/kinds_mod.rmod	    \
	pop_modified_blddir/run/domain_size.rmod	    \
	pop_modified_blddir/serial/communicate.rmod	    \
	pop_modified_blddir/source/constants.rmod	    \
	pop_modified_blddir/serial/broadcast.rmod	    \
	pop_modified_blddir/source/exit_mod.rmod	    \
	pop_modified_blddir/source/blocks.rmod		    \
	pop_modified_blddir/source/distribution.rmod	    \
	pop_modified_blddir/source/io_types.rmod	    \
	pop_modified_blddir/serial/boundary.rmod	    \
	pop_modified_blddir/source/domain.rmod		    \
	pop_modified_blddir/serial/global_reductions.rmod   \
	pop_modified_blddir/serial/gather_scatter.rmod	    \
	pop_modified_blddir/source/io_netcdf.rmod	    \
	pop_modified_blddir/source/io_binary.rmod	    \
	pop_modified_blddir/source/io.rmod		    \
	pop_modified_blddir/source/timers.rmod		    \
	pop_modified_blddir/source/grid.rmod		    \
	pop_modified_blddir/source/time_management.rmod	    \
	pop_modified_blddir/source/prognostic.rmod	    \
	pop_modified_blddir/source/solvers.rmod		    \
	pop_modified_blddir/source/forcing_tools.rmod	    \
	pop_modified_blddir/source/forcing_ws.rmod	    \
	pop_modified_blddir/source/forcing_shf.rmod	    \
	pop_modified_blddir/source/forcing_sfwf.rmod	    \
	pop_modified_blddir/source/forcing_pt_interior.rmod \
	pop_modified_blddir/source/forcing_s_interior.rmod  \
	pop_modified_blddir/source/forcing_ap.rmod	    \
	pop_modified_blddir/source/ice.rmod		    \
	pop_modified_blddir/source/forcing_coupled.rmod	    \
	pop_modified_blddir/source/tavg.rmod		    \
	pop_modified_blddir/source/forcing.rmod		    \
	pop_modified_blddir/source/diagnostics.rmod	    \
	pop_modified_blddir/source/state_mod.rmod	    \
	pop_modified_blddir/source/xdisplay.rmod	    \
	pop_modified_blddir/source/operators.rmod	    \
	pop_modified_blddir/source/vmix_const.rmod	    \
	pop_modified_blddir/source/vmix_rich.rmod	    \
	pop_modified_blddir/source/vmix_kpp.rmod	    \
	pop_modified_blddir/source/vertical_mix.rmod	    \
	pop_modified_blddir/source/hmix_gm.rmod		    \
	pop_modified_blddir/source/advection.rmod	    \
	pop_modified_blddir/source/pressure_grad.rmod	    \
	pop_modified_blddir/source/topostress.rmod	    \
	pop_modified_blddir/source/hmix_del2.rmod	    \
	pop_modified_blddir/source/hmix_del4.rmod	    \
	pop_modified_blddir/source/hmix_aniso.rmod	    \
	pop_modified_blddir/source/horizontal_mix.rmod	    \
	pop_modified_blddir/source/baroclinic.rmod	    \
	pop_modified_blddir/source/barotropic.rmod	    \
	pop_modified_blddir/source/surface_hgt.rmod	    \
	pop_modified_blddir/source/step_mod.rmod	    \
	pop_modified_blddir/source/restart.rmod		    \
	pop_modified_blddir/source/history.rmod		    \
	pop_modified_blddir/source/movie.rmod		    \
	pop_modified_blddir/source/output.rmod		    \
	pop_modified_blddir/source/initial.rmod		    \
	pop_modified_blddir/source/POP.rmod		    \
	pop_modified_blddir/source/current_meters.rmod	    \
	pop_modified_blddir/source/drifters.rmod	    \
	pop_modified_blddir/source/hydro_sections.rmod

# Build the "pop" executables
pop_pristine_blddir/run/pop pop_modified_blddir/run/pop: \
%_blddir/run/pop: hudson-environment.sh netcdf % $(top_builddir)/src/librose.la
	@set -e $(SETX);							 \
	type=$(patsubst pop_%_blddir/run/pop,%,$@);				 \
	echo "  BUILD   LANL POP ($$type)";					 \
	source hudson-environment.sh;						 \
	cd $(dir $@);								 \
	ARCHDIR=hudson NETCDF=$(netcdf_prefix) $$HUDSON_MAKE -j$$HUDSON_NUM_JOBS

# Run the "pop" executables
pristine.ans modified.ans: %.ans: pop_%_blddir/run/pop
	@set -e $(SETX);				       \
	source hudson-environment.sh;			       \
	type=$(patsubst %.ans,%,$@);			       \
	echo "  RUN     LANL POP ($$type)";		       \
	(cd pop_$${type}_blddir/run && ./pop) > $@.unfiltered; \
	$$HUDSON_SED <$@.unfiltered >$@			       \
	    -e "s%Timer number.* seconds%%"		       \
	    -e "s%Timer stats.* seconds%%"		       \
	    -e "s%max = .* seconds%%"			       \
	    -e "s%mean= .* seconds%%"

# Compare filtered output from "pop" executables
compare_answers: pristine.ans modified.ans
	diff -u $^
	@echo "  PASSED  LANL POP tests"

check-local: compare_answers

endif
endif

###############################################################################################################################
#					automake rules for all configs
###############################################################################################################################

# Note: netcdf-4.1.1 and pop-distro are directories whose content is not part of ROSE, but which have been downloaded
#       from their respective distribution sites (and perhaps having some local modificatons).  We need to copy _everything_
#       in those directories into the ROSE distribution because NetCDF and POP will be configured and built as part of testing
#       ROSE.
# Note: Specifiying $(netcdf_srcdir) and $(pop_srcidr) in the $(EXTRA_DIST) list does not work, contrary to automake
#       documentation.  When we do that, we get errors about directories not existing (it seems that automake's makefile
#       tries to copy the files before creating the destination directories.  Therefore, we're using the dist-hook to
#       explicitly copy these directories.
EXTRA_DIST = hudson.template make_rmod_deps.pl setup_run_dir.ROSE
dist-hook:
	cp -pPRv $(netcdf_srcdir) $(pop_srcdir) $(distdir)

clean-local:
	rm -rf $(netcdf_blddir)
	rm -f  hudson-environment.sh hudson.gnu
	rm -f  pristine.ans.unfiltered pristine.ans modified.ans.unfiltered modified.ans
	rm -rf pop_pristine_blddir_stamp pop_pristine_blddir
	rm -rf pop_modified_blddir_stamp pop_modified_blddir
	rm -rf pop_modified_srcdir_stamp pop_modified_srcdir
	rm -f  $(notdir $(pop_modified_sources))
	rm -f  $(patsubst %.rmod,%_postprocessed.f90,$(notdir $(pop_modified_sources)))
	rm -f  rose_netcdf.f90 rose_typeSizes.f90

