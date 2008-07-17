$(compass_builddir)/CHECKER_LIST_WITHOUT_COMMENTS:
	cd $(compass_builddir); $(MAKE) CHECKER_LIST_WITHOUT_COMMENTS



CHECKER_LIST_WITHOUT_COMMENTS: $(srcdir)/CHECKER_LIST
	grep -v '^#' < $(srcdir)/CHECKER_LIST | awk '{print $$0 " " toupper(substr($$0, 0, 1)) substr($$0, 2)}' > CHECKER_LIST_WITHOUT_COMMENTS



compass_makefile.inc: CHECKER_LIST_WITHOUT_COMMENTS $(compass_srcdir)/compass_scripts/buildCompassMakefileInc.sh
	$(compass_srcdir)/compass_scripts/buildCompassMakefileInc.sh $(compass_srcdir) > compass_makefile.inc



compass_parameters: CHECKER_LIST_WITHOUT_COMMENTS $(srcdir)/RULE_SELECTION
	echo "# this is an automatically generated file " > compass_parameters
	echo "Compass.RuleSelection=$(srcdir)/RULE_SELECTION" >> compass_parameters
	echo '' >> compass_parameters
	cat CHECKER_LIST_WITHOUT_COMMENTS | while read ch chupper; do if [ -f $(compass_srcdir)/$$ch/compass_parameters ]; then grep -v '^#' $(compass_srcdir)/$$ch/compass_parameters >> compass_parameters; echo ""; fi; done



buildCheckers.C: CHECKER_LIST_WITHOUT_COMMENTS
	sed -n '1,/@@@/p' < $(srcdir)/../buildCheckers.C.in > buildCheckers.C
	cut -f 2 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,BUILD_ONE_CHECKER(&),' >> buildCheckers.C
	sed -n '/@@@/,$$p' < $(srcdir)/../buildCheckers.C.in >> buildCheckers.C



checkers.h: CHECKER_LIST_WITHOUT_COMMENTS
	cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,#include "&/&.h",' > checkers.h



#$(srcdir)/RULE_SELECTION: CHECKER_LIST_WITHOUT_COMMENTS
dothis:
	test -f $(srcdir)/RULE_SELECTION || touch $(srcdir)/RULE_SELECTION
	sh -c '\
	for checker in `cut -f 2 -d \ < CHECKER_LIST_WITHOUT_COMMENTS` ;\
	do \
		echo $${checker} ;\
		if [[ `grep "$${checker}" $(srcdir)/RULE_SELECTION` == "" ]]; then\
			echo "Updating $${checker} to $(srcdir)/RULE_SELECTION" ;\
			echo "+:$${checker}" >> $(srcdir)/RULE_SELECTION ;\
		fi ;\
	done ;\
	'



  include compass_makefile.inc
