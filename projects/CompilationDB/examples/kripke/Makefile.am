
MAP_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py
TOOL=$(abs_top_builddir)/tests/nonsmoke/functional/testTranslator
COMPDB=$(builddir)/compile_commands.json
NPROCS=32

$(abs_builddir)/kripke-src:
	git clone --recursive -b release/v1.2.0-CORAL2 https://github.com/LLNL/Kripke.git $(abs_builddir)/kripke-src

$(abs_builddir)/kripke-sequential/compile_commands.json: $(abs_builddir)/kripke-src
	( \
	  mkdir -p $(abs_builddir)/kripke-sequential ; \
	  cd $(abs_builddir)/kripke-sequential ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_OPENMP=False -DKRIPKE_ARCH=Sequential $(abs_builddir)/kripke-src \
	)

$(abs_builddir)/kripke-openmp/compile_commands.json: $(abs_builddir)/kripke-src
	( \
	  mkdir -p $(abs_builddir)/kripke-openmp ; \
	  cd $(abs_builddir)/kripke-openmp ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $(abs_builddir)/kripke-src \
	)

$(abs_builddir)/kripke-sequential/report.json: $(abs_builddir)/kripke-sequential/compile_commands.json
	python $(MAP_SCRIPT) $(abs_builddir)/kripke-sequential/compile_commands.json $(TOOL) $(NPROCS)

$(abs_builddir)/kripke-openmp/report.json: $(abs_builddir)/kripke-openmp/compile_commands.json
	python $(MAP_SCRIPT) $(abs_builddir)/kripke-openmp/compile_commands.json $(TOOL) $(NPROCS)

check-local: $(abs_builddir)/kripke-sequential/report.json $(abs_builddir)/kripke-openmp/report.json

clean-local:
	rm -rf $(abs_builddir)/kripke-src $(abs_builddir)/kripke-sequential $(abs_builddir)/kripke-openmp
