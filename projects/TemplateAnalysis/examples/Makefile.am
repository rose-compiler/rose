
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

RAT_REL_PATH=projects/TemplateAnalysis
RAT_ORG_PATH=ROSE/Analysis/Template
RAT_PATH=$(top_builddir)/$(RAT_REL_PATH)/src

check-local: if_and_branches/templinstflow.dot count_pointers/templinstflow.dot meta_objects/templinstflow.dot

$(RAT_PATH)/mfbEnv:
	make -C $(RAT_PATH) mfbEnv

if_and_branches/templinstflow.dot: $(srcdir)/codes/if_and_branches.cxx $(srcdir)/queries/if_and_branches.json $(RAT_PATH)/mfbEnv
	mkdir -p $(abs_builddir)/if_and_branches
	$(RAT_PATH)/mfbEnv --cfgfile=$(abs_srcdir)/queries/if_and_branches.json --basedir=$(abs_srcdir)/codes --resdir=$(abs_builddir)/if_and_branches

count_pointers/templinstflow.dot: $(srcdir)/codes/count_pointers.cxx $(srcdir)/queries/count_pointers.json $(RAT_PATH)/mfbEnv
	mkdir -p $(abs_builddir)/count_pointers
	$(RAT_PATH)/mfbEnv --cfgfile=$(abs_srcdir)/queries/count_pointers.json --basedir=$(abs_srcdir)/codes --resdir=$(abs_builddir)/count_pointers

meta_objects/templinstflow.dot: $(srcdir)/codes/meta_objects.cxx $(srcdir)/queries/meta_objects.json $(RAT_PATH)/mfbEnv
	mkdir -p $(abs_builddir)/meta_objects
	$(RAT_PATH)/mfbEnv --cfgfile=$(abs_srcdir)/queries/meta_objects.json --basedir=$(abs_srcdir)/codes --resdir=$(abs_builddir)/meta_objects

clean-local:
	rm -rf $(abs_builddir)/if_and_branches $(abs_builddir)/count_pointers $(abs_builddir)/meta_objects

