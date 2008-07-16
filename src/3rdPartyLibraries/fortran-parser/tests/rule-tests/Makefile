# Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
# material was produced under U.S. Government contract DE-
# AC52-06NA25396 for Los Alamos National Laboratory (LANL), which is
# operated by the Los Alamos National Security, LLC (LANS) for the
# U.S. Department of Energy. The U.S. Government has rights to use,
# reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
# LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
# LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
# produce derivative works, such modified software should be clearly
# marked, so as not to confuse it with the version available from
# LANL.

# Additionally, this program and the accompanying materials are made
# available under the terms of the Eclipse Public License v1.0 which
# accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html

JAVAC=javac
JAVA=java
JAR=jar
BUILDDIR=../../../build
PARDIR=../


DRAFT_TEST_DIRS="../annex_c" "../chapter_11" "../chapter_15" "../chapter_16" \
                "../chapter_4" "../chapter_5" "../chapter_6" "../chapter_7" \
                "../chapter_8" "../chapter_9" "../chapter_10" "../chapter_12" \
                "../chapter_2"
F90_TEST_DIRS="../misc" "../program-unit" 

all: runRuleTests

runTests: $(CLASS_FILES)
	$(JAVA) FortranMain ../test_files/*.f03 ../test_files/*.f90

runDraftTests: $(CLASS_FILES)
	@for dir in $(DRAFT_TEST_DIRS) ; do \
           $(JAVA) FortranMain $$dir/*.f03; \
        done;

runRuleTests:
	for file in *.f03 ; do \
           echo "Running test $$file"; \
           $(JAVA) parser.java.FortranMain --dump $$file > $$file.tmp.out; \
           diff -w $$file.tmp.out output/$$file.out > $$file.diff.out; \
           if test $$? != 0 ; then \
             echo $$file "		FAILED!"; \
           else \
             echo $$file "		Passed"; \
             rm $$file.tmp.out $$file.diff.out; \
           fi; \
        done;

clean:
	rm -f R*.diff.out
	rm -f R*.tmp.out
