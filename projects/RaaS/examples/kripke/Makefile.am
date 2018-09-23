
check-local: $(abs_builddir)/kripke-sequential.rosebin $(abs_builddir)/kripke-openmp.rosebin

$(abs_builddir)/kripke-src:
	git clone --recursive -b release/v1.2.0-CORAL2 https://github.com/LLNL/Kripke.git $(abs_builddir)/kripke-src



$(abs_builddir)/kripke-openmp/compile_commands.json: $(abs_builddir)/kripke-src
	( \
	  mkdir -p $(abs_builddir)/kripke-openmp ; \
	  cd $(abs_builddir)/kripke-openmp ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $(abs_builddir)/kripke-src \
	)

$(abs_builddir)/kripke-openmp/normalized_compile_commands.json: $(abs_builddir)/kripke-openmp/compile_commands.json
	python $(srcdir)/../../scripts/normalize.py $(abs_builddir)/kripke-openmp/compile_commands.json $(abs_builddir)/kripke-openmp/normalized_compile_commands.json

$(abs_builddir)/kripke-openmp.rosebin: $(abs_builddir)/kripke-openmp/normalized_compile_commands.json
	$(abs_builddir)/../../src/CompDBtoAstIO $(abs_builddir)/kripke-openmp/normalized_compile_commands.json $(abs_builddir)/kripke-openmp.rosebin
	$(abs_builddir)/../../src/CompDBtoAstIO $(abs_builddir)/kripke-openmp/normalized_compile_commands.json $(abs_builddir)/kripke-openmp.rosebin unparse compile



$(abs_builddir)/kripke-sequential/compile_commands.json: $(abs_builddir)/kripke-src
	( \
	  mkdir -p $(abs_builddir)/kripke-sequential ; \
	  cd $(abs_builddir)/kripke-sequential ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_OPENMP=False -DKRIPKE_ARCH=Sequential $(abs_builddir)/kripke-src \
	)

$(abs_builddir)/kripke-sequential/normalized_compile_commands.json: $(abs_builddir)/kripke-sequential/compile_commands.json
	python $(srcdir)/../../scripts/normalize.py $(abs_builddir)/kripke-sequential/compile_commands.json $(abs_builddir)/kripke-sequential/normalized_compile_commands.json

$(abs_builddir)/kripke-sequential.rosebin: $(abs_builddir)/kripke-sequential/normalized_compile_commands.json
	$(builddir)/../../src/CompDBtoAstIO $(abs_builddir)/kripke-sequential/normalized_compile_commands.json $(abs_builddir)/kripke-sequential.rosebin
	$(builddir)/../../src/CompDBtoAstIO $(abs_builddir)/kripke-sequential/normalized_compile_commands.json $(abs_builddir)/kripke-sequential.rosebin unparse compile

clean-local:
	rm -f $(abs_builddir)/kripke-sequential.rosebin $(abs_builddir)/kripke-openmp.rosebin
	rm -rf $(abs_builddir)/kripke-src $(abs_builddir)/kripke-sequential $(abs_builddir)/kripke-openmp
