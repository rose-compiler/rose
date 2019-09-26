
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

test-000.cxx.dot: $(srcdir)/test-000.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-000.cxx:4  $(srcdir)/test-000.cxx

test-001.cxx.dot: $(srcdir)/test-001.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-001.cxx:9  $(srcdir)/test-001.cxx

test-002.cxx.dot: $(srcdir)/test-002.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-002.cxx:5  $(srcdir)/test-002.cxx

test-003.cxx.dot: $(srcdir)/test-003.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-003.cxx:5  $(srcdir)/test-003.cxx

test-004.cxx.dot: $(srcdir)/test-004.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-004.cxx:5  $(srcdir)/test-004.cxx

test-005.cxx.dot: $(srcdir)/test-005.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-005.cxx:5  $(srcdir)/test-005.cxx

test-006.cxx.dot: $(srcdir)/test-006.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-006.cxx:4  $(srcdir)/test-006.cxx

test-007.cxx.dot: $(srcdir)/test-007.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-007.cxx:4  $(srcdir)/test-007.cxx

test-008.cxx.dot: $(srcdir)/test-008.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-008.cxx:4  $(srcdir)/test-008.cxx

test-kripke.cxx.dot: $(srcdir)/test-kripke.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph --positions=test-kripke.cxx:66  $(srcdir)/test-kripke.cxx

check-local: test-000.cxx.dot test-001.cxx.dot test-002.cxx.dot test-003.cxx.dot test-004.cxx.dot test-005.cxx.dot test-006.cxx.dot test-007.cxx.dot test-008.cxx.dot test-kripke.cxx.dot

clean-local:
	rm -f *.dot
	rm -f rose_*.cxx


test-00701.cxx.dot: $(srcdir)/test-00701.cxx $(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar
	$(abs_top_builddir)/projects/TemplateAnalysis/src/inspectAutoVar -DSKIP_ROSE_BUILTIN_DECLARATIONS --refgraph --instgraph --scopegraph $(srcdir)/test-00701.cxx

