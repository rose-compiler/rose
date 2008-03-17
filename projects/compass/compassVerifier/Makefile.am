include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

include $(srcdir)/compass_makefile.inc

SUBDIRS = compassSupport

# I hope this is not needed!
#	util 

# INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/compassSupport -I$(srcdir)/util $(COMPASS_INCLUDES)
INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/compassSupport $(COMPASS_INCLUDES)

bin_PROGRAMS = compassVerifier

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) compassSupport/libCompass.la $(compass_detector_libs)

compassVerifier_SOURCES = compassMain.C buildCheckers.C $(COMPASS_CHECKER_SRCS)
compassVerifier_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) compassSupport/libCompass.la $(compass_detector_libs)

# for some reason this is necessary to correctly build from projects/compass ??
buildMe:
	cd compassSupport ; $(MAKE)
	cd util ; $(MAKE)

param:
	cp -f $(srcdir)/compass_parameters compass_parameters

oneBigVerify: buildMe compassVerifier $(srcdir)/compass_parameters
	rm -f catChecker.C catCheckerIncludes.txt catChecker.ti
	@echo "Concatenating all checker sources"
	@sh -c '\
	for file in `grep -v "#" $(srcdir)/../CHECKER_LIST` ;\
	do \
		cat $(srcdir)/../$${file}/$${file}.C >> catChecker.C ;\
		echo "-I$(srcdir)/../$${file}" >> catCheckerIncludes.txt ;\
	done ;\
	export CAT_CHECKER_INCLUDES=`cat catCheckerIncludes.txt` ;\
	echo "env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassVerifier -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C" ;\
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassVerifier -rose:verbose 2 -I$(top_builddir) $(INCLUDES) $${CAT_CHECKER_INCLUDES} catChecker.C ;\
	'

verify: buildMe compassVerifier param
	@sh -c '\
	export checker_labels=`echo "$(COMPASS_CHECKER_TEST_LABELS)" | sed -e "s@test@@g"` ;\
	export directories=`grep -v "#" $(srcdir)/../CHECKER_LIST` ;\
	echo -e "all: `echo $${directories}`" > verify.makefile ;\
	for dir in $${directories} ;\
	do \
                export tmpfile=`mktemp` ; rm -f $${tmpfile} ;\
                export f1=$${dir}_`basename $${tmpfile} | sed -e "s@\.@@g"`_out.txt ;\
                export f2=$${dir}_`basename $${tmpfile} | sed -e "s@\.@@g"`_err.txt ;\
                export files=`ls -R $(srcdir)/../$${dir}/*` ;\
		echo -e "\n$${dir}:" >> verify.makefile ;\
		echo -e "\t@sh -c \"\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n./compassVerifier `echo $${files}`\\\n\\\" ;\\" >> verify.makefile ;\
		echo -e "\t\t./compassVerifier -I$(top_builddir) $(INCLUDES) `echo $${files}` &> $${f1} ;\\" >> verify.makefile ;\
		echo -e "\t\tdeclare -i err_code=\\\$$\$$? ;\\" >> verify.makefile ;\
		echo -e "\t\techo -e \\\"\\\n***Command Line: compassVerifier -I$(top_builddir) $(INCLUDES) `echo $${files}` \\\" >> $${f1} ;\\" >> verify.makefile ;\
		echo -e "\t\tif((\\\$$\$$err_code!=0)); then \\" >> verify.makefile ;\
		echo -e "\t\t\techo -e \\\"\\\nError: ./compassVerifier exit FAILURE, see `pwd`/$${f1}\\\n\\\" ;\\" >> verify.makefile ;\
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
	$(MAKE) -f verify.makefile -$${MAKEFLAGS}

test: compassVerifier
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassVerifier $(srcdir)/tests/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** ROSE/projects/compass/compassVerifier: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

# We can't include $(COMPASS_CHECKER_DIRECTORIES) since it appears to make the list too long!
EXTRA_DIST = compass_parameters tests checkers.h verifiedRepository CHECKER_LIST compassCheckerDocs.tex 

dist-hook:
	for i in $(COMPASS_CHECKER_DIRECTORIES); do \
		cp -rf $(srcdir)/$$i $(distdir); \
	done;

clean-local:
	rm -f compass.* *.sty compass_parameters *.ti a.out *.tex *.lo *.la
	rm -f *_tmp*.txt verify.makefile catChecker*
