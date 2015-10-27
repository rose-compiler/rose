include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = \
	$(ROSE_INCLUDES)

bin_PROGRAMS = RTC

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) #-ldl $(LIBLTDL)

#.PHONY: plainrun instrun

RTC_SOURCES = RTC.C \
			CheckBase.C \
			ArithCheck.C \
			QSTraversal.C \
			BinarySimpleTraversal.C \
			CheckBase.h \
			ArithCheck.h \
			QSTraversal.h \
			BinarySimpleTraversal.h \
			RTC.h

RTED_TESTS = pointer_example \
		pointer_example2 \
		pointer_example3 \
		pointer_example4 \
		pointer_example5 \
		pointer_example6 \
		pointer_example7 \
		pointer_example8 \
		pointer_example9


#check: plainrun instrun 
#	@echo "Executing commands for check"

#plainrun: RTC
#	for test in $(RTED_TESTS); do \
#		echo "$$test"; \
#		echo "./RTC -plain -rose:o $$test.out.c $$test.cpp;"; \
#		./RTC -plain -rose:o $$test.out.c $$test.cpp; \
#		./a.out;\
#	done

#instrun: RTC
#	echo "Instrumented Run"; \
#	for test in $(RTED_TESTS); do \
#		./RTC -rose:o $$test.out.c $$test.cpp ptr_lib.o; \
#		g++ $$test.out.c ptr_lib.o; \
#		./a.out >& instrun.txt; \
#	done

EXTRA_DIST = RTC.h
	
CLEANFILES = *.o

clean:
	rm -f *.o RTC

#runclean:
#	rm -f instrun.txt plainrun.txt

check-local:
	@echo "********************************************************************************"
	@echo "*** ROSE/projects/RTC: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************"
