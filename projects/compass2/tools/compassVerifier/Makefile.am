################################################################################
#
# compassVerifier
#
################################################################################

include $(top_srcdir)/projects/compass2/src/compassSupport/compass.inc

bin_PROGRAMS = compassVerifier

compassVerifier_SOURCES = compassMain.C checkers.h
compassVerifier_LDADD = -lCompassCheckers $(ROSE_LIBS)

# This does not contribute to the link line
compassVerifier_DEPENDENCIES = $(compass_support_bdir)/libCompass.la $(compass_detector_libs) libCompassCheckers.la

test: compassVerifier compass_parameters $(compass_test_dir)/exampleTest_1.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier $(compass_test_dir)/exampleTest_1.C 

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** compassVerifier: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

EXTRA_DIST = CHECKER_LIST RULE_SELECTION

clean-local:
	rm -f compass_parameters *.ti a.out *.lo *.la
	rm -f *_tmp*.txt verify.makefile catChecker*

CLEANFILES = CHECKER_LIST_WITHOUT_COMMENTS compass_parameters buildCheckers.C checkers.h verify.inc verify.makefile compass_makefile.inc functions.allow compassTestMain.C RULE_SELECTION

################################################################################

TOOLBUILD=../compass/

 include verify.inc

verify.inc: 
	cut -f 1 -d \  < $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,verify_&: compassVerifier compass_parameters verify.makefile\n\t$$(MAKE) -f verify.makefile &\n,' > verify.inc

oneBigVerify: compassVerifier compass_parameters
	rm -f catChecker.C catCheckerIncludes.txt catChecker.ti
	@echo "Concatenating all checker sources"
	@sh -c '\
	for file in `cut -f 1 -d \  < $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS` ;\
	do \
 		cat $(compass_checker_dir)/$${file}/$${file}.C >> catChecker.C ;\
 		echo "-I$(compass_checker_dir)/$${file}" >> catCheckerIncludes.txt ;\
 	done ;\
 	export CAT_CHECKER_INCLUDES=`cat catCheckerIncludes.txt` ;\
 	echo "env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C" ;\
 	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -rose:verbose 2 -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C ;\
 	'
 
verify.makefile: verify.inc
	@sh -c '\
 	export checker_labels=`echo "$(COMPASS_CHECKER_TEST_LABELS)" | sed -e "s@test@@g"` ;\
 	export directories=`cut -f 1 -d \  < $(TOOLBUILD)/CHECKER_LIST_WITHOUT_COMMENTS` ;\
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

new_allow_list_setup:
	cat $(srcdir)/RULE_SELECTION | sed -e 's@^+@-@g' | sed -e 's@-:AllowedFunctions@+:AllowedFunctions@g' > RULE_SELECTION
	echo -e "Compass.RuleSelection=`pwd`/RULE_SELECTION\nAllowedFunctions.OutFile=`pwd`/functions.allow" > compass_parameters
	echo "AllowedFunctions.Library0=$(prefix)/include" >> compass_parameters
	echo "AllowedFunctions.Library1=$(top_builddir)/include-staging" >> compass_parameters
	echo "AllowedFunctions.FunctionNum=0" >> compass_parameters
#	declare -i count=`wc -l $(compass_srcdir)/allowedFunctions/custom_parameters | gawk '{print $$1}'` ;\
	echo "AllowedFunctions.FunctionNum=$$count" >> compass_parameters ;\
	while ((count > 0)) ;\
	do \
		echo "AllowedFunctions.Function$$((count-1))=`head -$$count $(compass_srcdir)/allowedFunctions/custom_parameters | tail -1`" >> compass_parameters ;\
		((count--)) ;\
	done

new_allow_list: compassVerifier $(srcdir)/RULE_SELECTION $(TOOLBUILD)/buildCheckers.C new_allow_list_setup
	sed -e 's@new Checker@0; //@' $(compass_support_dir)/compassTestMain.C > compassTestMain.C
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) compassTestMain.C ; cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $(compass_support_dir)/compass.C ; cp functions.allow compass_parameters
#	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass-srcdir) $(INCLUDES) $(compass_srcdir)/compassMain.C ; cp functions.allow compass_parameters
	env COMPASS_PARAMETERS=./compass_parameters ./compassVerifier -I$(top_builddir) -I$(compass_srcdir) $(INCLUDES) $(TOOLBUILD)/buildCheckers.C ; cp functions.allow compass_parameters
	$(MAKE) oneBigVerify
	sed '$$d' functions.allow | sed -e '1d' > $(compass_checker_dir)/allowedFunctions/compass_parameters ; tail -1 functions.allow | gawk -F= '{print $$1 "=-" $$2}' >> $(compass_checker_dir)/allowedFunctions/compass_parameters
	rm -f compass_parameters RULE_SELECTION compassTestMain.*

$(TOOLBUILD)/buildCheckers.C:
	cd $(TOOLBUILD); make buildCheckers.C

verify: compassVerifier compass_parameters verify.makefile
	$(MAKE) -f verify.makefile -$${MAKEFLAGS}

################################################################################
