include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
     documentedExamples \
     defaultTranslator  \
     DOTGenerator       \
     PDFGenerator       \
     AstCopyReplTester

# DQ (9/11/2008): We can add these later but these are older QRose examples.
# Current examples are in developersScratchSpace/Dan/QRose_tests.
#    graphicalUserInterfaceExamples


EXTRA_DIST = CMakeLists.txt
clean-local:
	rm -f rose_test_*
