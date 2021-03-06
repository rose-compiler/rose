include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(srcdir)/Specimens.inc

EXTRA_DIST =						\
	CMakeLists.txt					\
	$(nonsmoke_specimens_binary_all)		\
	$(nonsmoke_specimens_binary_other_distributed)

###############################################################################################################################
# Larger collections of specimens.  These follow the same naming convention except "any" can appear in one of the positions.

# This rule regenerates the makefile fragment that can be included into other makefiles. I had to do it this round about way
# because I couldn't get GNU make's "foreach" function to generate new variables.  After you generate a new SpecimenSets.inc
# you must re-run the automake and configure scripts in order to incorporate the new file into the generated makefiles.
.PHONY: build-SpecimenSets.inc
build-SpecimenSets.inc:
	@(													\
		echo '# Collections of binary specimens                    -*- makefile -*-';			\
		echo '# This file is automatically generated by running';					\
		echo '#   $$ make build-SpecimenSets.inc';							\
		echo '# in the $$ROSE_BLD/tests/nonsmoke/specimens/binary directory';				\
		echo;												\
		echo 'include $$(top_srcdir)/tests/nonsmoke/specimens/binary/Specimens.inc';			\
														\
		echo;												\
		echo '# Container agnostic';									\
		for filetype in $(nonsmoke_specimens_binary_filetypes); do					\
		    for arch in $(nonsmoke_specimens_binary_architectures); do					\
			echo -n "nonsmoke_specimens_binary_any_$${filetype}_$${arch} =" ;			\
			for container in $(nonsmoke_specimens_binary_containers); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
			echo;											\
		    done;											\
		done;												\
														\
		echo;												\
		echo '# Filetype agnostic';									\
		for container in $(nonsmoke_specimens_binary_containers); do					\
		    for arch in $(nonsmoke_specimens_binary_architectures); do					\
			echo -n "nonsmoke_specimens_binary_$${container}_any_$${arch} =";			\
			for filetype in $(nonsmoke_specimens_binary_filetypes); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
			echo;											\
		    done;											\
		done;												\
														\
		echo;												\
		echo '# Architecture agnostic';									\
		for container in $(nonsmoke_specimens_binary_containers); do					\
		    for filetype in $(nonsmoke_specimens_binary_filetypes); do					\
			echo -n "nonsmoke_specimens_binary_$${container}_$${filetype}_any =";			\
			for arch in $(nonsmoke_specimens_binary_architectures); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
			echo;											\
		    done;											\
		done;												\
														\
		echo;												\
		echo '# Filetype and architecture agnostic';							\
		for container in $(nonsmoke_specimens_binary_containers); do					\
		    echo -n "nonsmoke_specimens_binary_$${container}_any_any =";				\
		    for filetype in $(nonsmoke_specimens_binary_filetypes); do					\
			for arch in $(nonsmoke_specimens_binary_architectures); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
		    done;											\
		    echo;											\
		done;												\
														\
		echo;												\
		echo '# Container and architecture agnostic';							\
		for filetype in $(nonsmoke_specimens_binary_filetypes); do					\
		    echo -n "nonsmoke_specimens_binary_any_$${filetype}_any =";					\
		    for container in $(nonsmoke_specimens_binary_containers); do				\
			for arch in $(nonsmoke_specimens_binary_architectures); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
		    done;											\
		    echo;											\
		done;												\
														\
		echo;												\
		echo '# Container and filetype agnostic';							\
		for arch in $(nonsmoke_specimens_binary_architectures); do					\
		    echo -n "nonsmoke_specimens_binary_any_any_$${arch} =";					\
		    for container in $(nonsmoke_specimens_binary_containers); do				\
			for filetype in $(nonsmoke_specimens_binary_filetypes); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
		    done;											\
		    echo;											\
		done;												\
														\
		echo;												\
		echo '# All binary specimens';									\
		echo -n "nonsmoke_specimens_binary_any_any_any =";						\
		for container in $(nonsmoke_specimens_binary_containers); do					\
		    for filetype in $(nonsmoke_specimens_binary_filetypes); do					\
			for arch in $(nonsmoke_specimens_binary_architectures); do				\
			    echo -n " %(nonsmoke_specimens_binary_$${container}_$${filetype}_$${arch})";	\
			done;											\
		    done;											\
		done;												\
		echo;												\
	) |sed 's/%/$$/g' >SpecimenSets.inc
	mv SpecimenSets.inc $(srcdir)/.

