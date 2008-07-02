include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#include $(srcdir)/compass_makefile.inc

 include compass_makefile.inc
 include verify.inc

SUBDIRS = compassSupport

# I hope this is not needed!
#	util 

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/compassSupport $(COMPASS_INCLUDES) -I. 

bin_PROGRAMS = compassVerifier

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) compassSupport/libCompass.la

lib_LTLIBRARIES = libCompassCheckers.la
libCompassCheckers_la_SOURCES = buildCheckers.C checkers.h
libCompassCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) compassSupport/libCompass.la

BUILT_SOURCES = buildCheckers.C checkers.h

libCompassCheckers.la: $(libCompassCheckers_la_OBJECTS) $(libCompassCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libCompassCheckers_la_LDFLAGS) $(libCompassCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libCompassCheckers_la_LIBADD) $(LIBS)

compassVerifier_SOURCES = compassMain.C checkers.h
compassVerifier_LDADD = -lCompassCheckers $(ROSE_LIBS)

# This does not contribute to the link line
compassVerifier_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) compassSupport/libCompass.la $(compass_detector_libs) libCompassCheckers.la


CHECKER_LIST_WITHOUT_COMMENTS: $(srcdir)/CHECKER_LIST
	grep -v '^#' < $(srcdir)/CHECKER_LIST | awk '{print $$0 " " toupper(substr($$0, 0, 1)) substr($$0, 2)}' > CHECKER_LIST_WITHOUT_COMMENTS

compass_makefile.inc: CHECKER_LIST_WITHOUT_COMMENTS $(srcdir)/../compass_scripts/buildCompassMakefileInc.sh
	$(srcdir)/../compass_scripts/buildCompassMakefileInc.sh $(srcdir) > compass_makefile.inc

buildCheckers.C: CHECKER_LIST_WITHOUT_COMMENTS
	sed -n '1,/@@@/p' < $(srcdir)/../buildCheckers.C.in > buildCheckers.C
	cut -f 2 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,BUILD_ONE_CHECKER(&),' >> buildCheckers.C
	sed -n '/@@@/,$$p' < $(srcdir)/../buildCheckers.C.in >> buildCheckers.C

checkers.h: CHECKER_LIST_WITHOUT_COMMENTS
	cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,#include "&/&.h",' > checkers.h

# for some reason this is necessary to correctly build from projects/compass ??
buildMe:
	cd compassSupport ; $(MAKE)

compass_parameters: CHECKER_LIST_WITHOUT_COMMENTS
	echo "# this is an automatically generated file " > compass_parameters
	echo "Compass.RuleSelection=$(srcdir)/RULE_SELECTION" >> compass_parameters
	echo '' >> compass_parameters
	cat CHECKER_LIST_WITHOUT_COMMENTS | while read ch chupper; do if [ -f $(srcdir)/$$ch/compass_parameters ]; then grep -v '^#' $(srcdir)/$$ch/compass_parameters >> compass_parameters; echo ""; fi; done
#	$(MAKE) append_allowed_namespaces

oneBigVerify: buildMe compassVerifier compass_parameters
	rm -f catChecker.C catCheckerIncludes.txt catChecker.ti
	@echo "Concatenating all checker sources"
	@sh -c '\
	for file in `grep -v "#" $(srcdir)/../CHECKER_LIST` ;\
	do \
		cat $(srcdir)/../$${file}/$${file}.C >> catChecker.C ;\
		echo "-I$(srcdir)/../$${file}" >> catCheckerIncludes.txt ;\
	done ;\
	export CAT_CHECKER_INCLUDES=`cat catCheckerIncludes.txt` ;\
	echo "env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C" ;\
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -rose:verbose 2 -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C ;\
	'

../CHECKER_LIST_WITHOUT_COMMENTS:
	cd ..; $(MAKE) CHECKER_LIST_WITHOUT_COMMENTS

verify.inc: ../CHECKER_LIST_WITHOUT_COMMENTS
	cut -f 1 -d \  < ../CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,verify_&: buildMe compassVerifier compass_parameters verify.makefile\n\t$$(MAKE) -f verify.makefile &\n,' > verify.inc

verify.makefile: verify.inc
	@sh -c '\
	export checker_labels=`echo "$(COMPASS_CHECKER_TEST_LABELS)" | sed -e "s@test@@g"` ;\
	export directories=`grep -v "#" $(srcdir)/../CHECKER_LIST` ;\
	echo -e "COMPILER=compassVerifier" > verify.makefile ;\
	echo -e "all: `echo $${directories}`" >> verify.makefile ;\
	for dir in $${directories} ;\
	do \
                export tmpfile=`mktemp` ; rm -f $${tmpfile} ;\
                export f1=$${dir}_`basename $${tmpfile} | sed -e "s@\.@@g"`_out.txt ;\
                export f2=$${dir}_`basename $${tmpfile} | sed -e "s@\.@@g"`_err.txt ;\
                export files=`ls -R $(srcdir)/../$${dir}/* | grep -v "$${dir}Test.*.C"` ;\
		echo -e "\n$${dir}:" >> verify.makefile ;\
		echo -e "\t@sh -c \"\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n./\$$(COMPILER) `echo $${files}`\\\n\\\" ;\\" >> verify.makefile ;\
		echo -e "\t\tenv COMPASS_PARAMETERS=./compass_parameters ./\$$(COMPILER) -I$(top_builddir) $(INCLUDES) `echo $${files}` &> $${f1} ;\\" >> verify.makefile ;\
		echo -e "\t\tdeclare -i err_code=\\\$$\$$? ;\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n***Command Line: \$$(COMPILER) -I$(top_builddir) $(INCLUDES) `echo $${files}` \\\" >> $${f1} ;\\" >> verify.makefile ;\
		echo -e "\t\tif((\\\$$\$$err_code!=0)); then \\" >> verify.makefile ;\
		echo -e "\t\t\techo -e \\\"\\\nError: ./\$$(COMPILER) exit FAILURE, see `pwd`/$${f1}\\\n\\\" ;\\" >> verify.makefile ;\
		echo -e "\t\t\texit 1 ;\\" >> verify.makefile ;\
		echo -e "\t\tfi ;\\" >> verify.makefile ;\
		for checker in `echo $${checker_labels}` ;\
		do \
			echo -e "\t\tgrep \\\"$${checker}:\\\" $${f1} >> $${f2} ;\\" >> verify.makefile ;\
		done ;\
		echo -e "\t\tif test -s $${f2} ; then \\" >> verify.makefile ;\
		echo -e "\t\t\techo \\\"Error during compass verification: $${dir} does not comply, see `pwd`/$${f2}\\\" ;\\" >> verify.makefile ;\
		echo -e "\t\t\texit 1 ;\\" >> verify.makefile ;\
		echo -e "\t\telse \\" >> verify.makefile ;\
		echo -e "\t\t\techo \\\"Success: $${dir} passed\\\" ;\\" >> verify.makefile ;\
		echo -e "\t\tfi ;\\" >> verify.makefile ;\
		echo -e "\t\trm -f $${f1} $${f2} ;\\" >> verify.makefile ;\
		echo -e "\t\"" >> verify.makefile ;\
	done \
	'

verify: buildMe compassVerifier compass_parameters verify.makefile 
	$(MAKE) -f verify.makefile -$${MAKEFLAGS}

#allow_wrapper.sh:
#	echo -e "env COMPASS_PARAMETERS=`pwd`/compass_parameters `pwd`/compassVerifier \$$@\ncp `pwd`/functions.allow `pwd`/compass_parameters\nexit 0" > $@
#	chmod +x $@

../buildCheckers.C:
	cd ..; make buildCheckers.C

append_allowed_namespaces: ../CHECKER_LIST_WITHOUT_COMMENTS
	sh -c '\
        declare -i count=0 ;\
        for namespace in `cut -f 2 -d \ < ../CHECKER_LIST_WITHOUT_COMMENTS` ;\
        do \
                echo "AllowedFunctions.Namespace$$count=::CompassAnalyses::$${namespace}" ;\
                ((count++)) ;\
        done ;\
	' >> compass_parameters

new_allow_list_setup: $(srcdir)/allowedFunctions/custom_parameters 
	cat $(srcdir)/RULE_SELECTION | sed -e 's@^+@-@g' | sed -e 's@-:AllowedFunctions@+:AllowedFunctions@g' > RULE_SELECTION	
	echo -e "Compass.RuleSelection=`pwd`/RULE_SELECTION\nAllowedFunctions.OutFile=`pwd`/functions.allow" > compass_parameters
	declare -i count=`wc -l $(srcdir)/allowedFunctions/custom_parameters | gawk '{print $$1}'`;\
	echo "AllowedFunctions.FunctionNum=$$((count))" >> compass_parameters ;\
	while((count > 0)) ;\
	do \
		echo "AllowedFunctions.Function$$((count-1))=`head -$$((count)) $(srcdir)/allowedFunctions/custom_parameters | tail -1`" >> compass_parameters ;\
		((count--)) ;\
	done

new_allow_list: buildMe compassVerifier $(srcdir)/RULE_SELECTION ../buildCheckers.C new_allow_list_setup
	sed -e 's@new Checker@0; //@' $(srcdir)/compassSupport/compassTestMain.C > compassTestMain.C
#	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(srcdir)/.. $(INCLUDES) $(srcdir)/../compassSupport/compass.C $(srcdir)/../compassMain.C ../buildCheckers.C compassTestMain.C ; cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(srcdir)/.. $(INCLUDES) $(srcdir)/../compassSupport/compass.C ; cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(srcdir)/.. $(INCLUDES) $(srcdir)/../compassMain.C ; cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(srcdir)/.. $(INCLUDES) ../buildCheckers.C ; cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(srcdir)/.. $(INCLUDES) compassTestMain.C ; cp functions.allow compass_parameters
	$(MAKE) oneBigVerify
	sed '$$d' functions.allow | sed -e '1d' > $(srcdir)/allowedFunctions/compass_parameters ; tail -1 functions.allow | gawk -F= '{print $$1 "=-" $$2}' >> $(srcdir)/allowedFunctions/compass_parameters
	rm -f compass_parameters RULE_SELECTION compassTestMain.*

#new_long_allow_list: compassVerifier $(srcdir)/RULE_SELECTION allow_wrapper.sh verify.makefile
#	cat $(srcdir)/RULE_SELECTION | sed -e 's@^+@-@g' | sed -e 's@-:AllowedFunctions@+:AllowedFunctions@g' > RULE_SELECTION
#	echo -e "Compass.RuleSelection=`pwd`/RULE_SELECTION\nAllowedFunctions.OutFile=`pwd`/functions.allow\nAllowedFunctions.FunctionNum=0" > compass_parameters
#	./allow_wrapper.sh -I$(top_builddir) -I$(srcdir)/.. $(INCLUDES) $(srcdir)/../compassSupport/compass.C $(srcdir)/../compassMain.C
#	$(MAKE) -f verify.makefile COMPILER=allow_wrapper.sh
#	sed '$$d' functions.allow | sed -e '1d' > $(srcdir)/allowedFunctions/compass_parameters ; tail -1 functions.allow | gawk -F= '{print $$1 "=-" $$2}' >> $(srcdir)/allowedFunctions/compass_parameters
#	rm -f compass_parameters RULE_SELECTION

test: compassVerifier compass_parameters 
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier $(srcdir)/tests/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** ROSE/projects/compass/compassVerifier: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

# We can't include $(COMPASS_CHECKER_DIRECTORIES) since it appears to make the list too long!
EXTRA_DIST = CHECKER_LIST RULE_SELECTION tests verifiedRepository

dist-hook:
	for i in $(COMPASS_CHECKER_DIRECTORIES); do \
		cp -rf $(srcdir)/$$i $(distdir); \
	done;

clean-local:
	rm -f compass.* *.sty compass_parameters *.ti a.out *.tex *.lo *.la
	rm -f *_tmp*.txt verify.makefile catChecker*

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_parameters buildCheckers.C checkers.h verify.inc verify.makefile compass_makefile.inc functions.allow #allow_wrapper.sh
