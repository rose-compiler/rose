
EXTRA_DIST = funcs.annot  interp1.C  redBlack1.C rose_redBlack1.C.save  rose_redBlack1_split.C.save \
             TestDriver floatArray.annot  Index.annot  rose_interp1.C.save  simpleA++.h

ROSE_FLAGS=-I$(srcdir)
#-fs2 means multi-level loop fusion for more reuses for loop translation
rose_interp1.C: ../ArrayProcessor
	../ArrayProcessor $(ROSE_FLAGS) -c -fs2 -annot $(srcdir)/floatArray.annot -annot $(srcdir)/funcs.annot -annot $(srcdir)/Index.annot $(srcdir)/interp1.C

rose_redBlack1.C: ../ArrayProcessor
	../ArrayProcessor $(ROSE_FLAGS) -c -fs2 -annot $(srcdir)/floatArray.annot -annot $(srcdir)/funcs.annot -annot $(srcdir)/Index.annot $(srcdir)/redBlack1.C

../ArrayProcessor:
	cd ..&&$(MAKE) 

# to deactivate tests comment out check-local.
check-local:
	$(MAKE) FORCE_TEST_CODES_TO_RUN

FORCE_TEST_CODES_TO_RUN:../ArrayProcessor 
	$(srcdir)/TestDriver $(srcdir) $(top_builddir)/projects/arrayOptimization

clean-local:
	rm -f *.o rose_*.[cC] 
