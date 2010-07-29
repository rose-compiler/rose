################################################################################
#
# compassVerifier
#
################################################################################

include $(top_srcdir)/projects/compass/src/compassSupport/compass.inc

BUILT_SOURCES = buildCheckers.C checkers.h

lib_LTLIBRARIES = libCompassVerifierCheckers.la
libCompassVerifierCheckers_la_SOURCES = buildCheckers.C checkers.h
libCompassVerifierCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

libCompassVerifierCheckers.la: $(libCompassVerifierCheckers_la_OBJECTS) $(libCompassVerifierCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libCompassVerifierCheckers_la_LDFLAGS) $(libCompassVerifierCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libCompassVerifierCheckers_la_LIBADD) $(LIBS)

bin_PROGRAMS = compassVerifier

compassVerifier_SOURCES = compassMain.C checkers.h
compassVerifier_LDADD = -lCompassVerifierCheckers $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

# This does not contribute to the link line
compassVerifier_DEPENDENCIES = $(compass_support_bdir)/libCompass.la $(compass_detector_libs) libCompassVerifierCheckers.la

test: compassVerifier compass_parameters $(compass_test_dir)/exampleTest_1.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -rose:skip_unparser -rose:skipfinalCompileStep $(compass_test_dir)/exampleTest_1.C 

check-local:
# DQ (9/10/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
	@echo "*******************************"
	@echo "*** Testing compassVerifier ***"
	@echo "*******************************"
if !OS_VENDOR_DEBIAN
	@$(MAKE) test
endif
	@echo "*****************************************************************************"
	@echo "*** compassVerifier: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

EXTRA_DIST = CHECKER_LIST RULE_SELECTION.in

clean-local:
	rm -f compass_parameters *.ti a.out *.lo *.la rose_*.[cC]
	rm -f *_tmp*.txt verify.makefile catChecker*

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_parameters buildCheckers.C checkers.h verify.inc verify.makefile compass_makefile.inc functions.allow compassTestMain.C RULE_SELECTION

################################################################################

TOOLBUILD=../compass/
FIND_ALL_CHECKERS=find $(compass_checker_dir) -type d -mindepth 1 -maxdepth 1 -exec basename {} \; | grep -v ".svn" | env LC_ALL=C sort | uniq

 include verify.inc

$(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS:
	cd $(TOOLBUILD) && make CHECKER_LIST_WITHOUT_COMMENTS

verify.inc:
	$(FIND_ALL_CHECKERS) | sed 's,^.*$$,verify_&: compassVerifier compass_parameters verify.makefile\n\t$$(MAKE) -f verify.makefile &\n,' > verify.inc

#oneBigVerify: compassVerifier compass_parameters $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS
#	rm -f catChecker.C catCheckerIncludes.txt catChecker.ti
#	@echo "Concatenating all checker sources"
#	@sh -c '\
#	for file in `cut -f 1 -d \  < $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS` ;\
#	do \
#		lin=`grep -n "static void run" $(compass_checker_dir)/$${file}/$${file}.C | cut -d: -f1` ;\
#		head -$$((lin-1)) $(compass_checker_dir)/$${file}/$${file}.C >> catChecker.C ;\
# 		echo "-I$(compass_checker_dir)/$${file}" >> catCheckerIncludes.txt ;\
# 	done ;\
# 	export CAT_CHECKER_INCLUDES=`cat catCheckerIncludes.txt` ;\
# 	echo "env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C" ;\
# 	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -rose:verbose 2 -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C ;\
# 	'


#for prefix in `cut -f 1 -d \  < $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS`;\
#

oneBigVerify: compassVerifier compass_parameters 
	rm -f catChecker.C catCheckerIncludes.txt catChecker.ti
	@sh -c '\
		for prefix in `cut -f 1 -d \  < $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS`;\
		do \
			sed -r "s/[^\.]\<(run\(Compass|createTraversal|getPrerequisites)\>/ $${prefix}___&/g" $(compass_checker_dir)/$$prefix/$$prefix.C |\
			sed -e "s@run,@$${prefix}___run,@g" |\
			sed -e "s@run);@$${prefix}___run);@g" |\
			sed -e "s@___[\ \t]@___@g" |\
			sed -e "s@$${prefix}___run(.*SgNode\*@run(SgNode\*@g" |\
			sed -e "s@Traversal: .*run@Traversal::run@g" \
			>> catChecker.C ;\
			echo "-I$(compass_checker_dir)/$${prefix}" >> catCheckerIncludes.txt ;\
		done ;\
		rm -f $$tmpfile ;\
		export CAT_CHECKER_INCLUDES=`cat catCheckerIncludes.txt` ;\
		env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -rose:verbose 2 $${CAT_CHECKER_INCLUDES} -I$(top_builddir) $(INCLUDES) $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) catChecker.C ;\
	'
 
verify.makefile: verify.inc
	@sh -c '\
	export checker_labels=`echo "$(COMPASS_CHECKER_TEST_LABELS)" | sed -e "s@test@@g"` ;\
	export directories=`$(FIND_ALL_CHECKERS)` ;\
	echo -e "COMPILER=compassVerifier" > verify.makefile ;\
	echo -e "all: `echo $${directories}`" >> verify.makefile ;\
	for dir in $${directories} ;\
	do \
		export tmpfile=`mktemp` ; rm -f $${tmpfile} ;\
		export f1=$${dir}_`basename $${tmpfile} | sed -e "s@\.@@g"`_out.txt ;\
		export f2=$${dir}_`basename $${tmpfile} | sed -e "s@\.@@g"`_err.txt ;\
		export files=`ls -R $(compass_checker_dir)/$${dir}/* | grep -v "$${dir}Test.*.C"` ;\
		echo -e "\n$${dir}:" >> verify.makefile ;\
		echo -e "\t@sh -c \"\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n./\$$(COMPILER) $(compass_checker_dir)/$${dir}/$${dir}.C\\\n\\\" ;\\" >> verify.makefile ;\
		echo -e "\t\tenv COMPASS_PARAMETERS=./compass_parameters ./\$$(COMPILER) -I$(top_builddir) $(INCLUDES) $(compass_checker_dir)/$${dir}/$${dir}.C &> $${f1} ;\\" >> verify.makefile ;\
		echo -e "\t\tdeclare -i err_code=\\\$$\$$? ;\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n***Command Line: \$$(COMPILER) -I$(top_builddir) $(INCLUDES) $(compass_checker_dir)/$${dir}/$${dir}.C \\\" >> $${f1} ;\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n***Exit Code: \\\$$\$$err_code\\\" >> $${f1} ;\\" >> verify.makefile ;\
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
		echo -e "\t\t#rm -f $${f1} $${f2} ;\\" >> verify.makefile ;\
		echo -e "\t\"" >> verify.makefile ;\
	done \
	'

new_allow_list_setup:
	cat $(srcdir)/RULE_SELECTION.in | sed -e 's@^+@-@g' | sed -e 's@-:AllowedFunctions@+:AllowedFunctions@g' > RULE_SELECTION
	echo -e "Compass.RuleSelection=`pwd`/RULE_SELECTION\nAllowedFunctions.OutFile=`pwd`/functions.allow" > compass_parameters
#	echo "AllowedFunctions.Library0=$(prefix)/include" >> compass_parameters
	echo "AllowedFunctions.Library0=$(top_builddir)/include-staging" >> compass_parameters
	echo "AllowedFunctions.Library1=$(top_srcdir)/src/ROSETTA/Grammar" >> compass_parameters
	echo "AllowedFunctions.FunctionNum=0" >> compass_parameters
#	declare -i count=`wc -l $(compass_srcdir)/allowedFunctions/custom_parameters | gawk '{print $$1}'` ;\
	echo "AllowedFunctions.FunctionNum=$$count" >> compass_parameters ;\
	while ((count > 0)) ;\
	do \
		echo "AllowedFunctions.Function$$((count-1))=`head -$$count $(compass_srcdir)/allowedFunctions/custom_parameters | tail -1`" >> compass_parameters ;\
		((count--)) ;\
	done

new_allow_list: compassVerifier $(srcdir)/RULE_SELECTION.in $(TOOLBUILD)/buildCheckers.C new_allow_list_setup
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $(compass_support_dir)/compass.C && cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $(TOOLBUILD)/buildCheckers.C && cp functions.allow compass_parameters
	$(MAKE) oneBigVerify && cp functions.allow compass_parameters
	sed '$$d' functions.allow | sed -e '1d' > $(compass_checker_dir)/allowedFunctions/compass_parameters ; tail -1 functions.allow | awk -F= '{print $$1 "=-" $$2}' >> $(compass_checker_dir)/allowedFunctions/compass_parameters
	rm -f compass_parameters RULE_SELECTION compassTestMain.*

new_allow_list_long: compassVerifier $(srcdir)/RULE_SELECTION.in $(TOOLBUILD)/buildCheckers.C new_allow_list_setup
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $(compass_support_dir)/compass.C && cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $(TOOLBUILD)/buildCheckers.C && cp functions.allow compass_parameters
	@sh -c '\
		for d in `$(FIND_ALL_CHECKERS)`;\
		do \
			checker=`basename $$d` ;\
			echo "COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $${d}/$${checker}.C && cp functions.allow compass_parameters" ;\
			env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $${d}/$${checker}.C && cp functions.allow compass_parameters ;\
		done ;\
	'
	sed '$$d' functions.allow | sed -e '1d' > $(compass_checker_dir)/allowedFunctions/compass_parameters ; tail -1 functions.allow | awk -F= '{print $$1 "=-" $$2}' >> $(compass_checker_dir)/allowedFunctions/compass_parameters
	rm -f compass_parameters RULE_SELECTION compassTestMain.*

$(TOOLBUILD)/buildCheckers.C:
	cd $(TOOLBUILD); make buildCheckers.C

verify: compassVerifier compass_parameters verify.makefile
	$(MAKE) -f verify.makefile -$${MAKEFLAGS}

foo:
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) /home/yuan5/ROSE/AUG0508/ROSE/projects/compass/extensions/checkers/allowedFunctions/allowedFunctions.C 

################################################################################
