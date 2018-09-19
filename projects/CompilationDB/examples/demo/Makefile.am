
check-local: $(builddir)/demo.ast

$(builddir)/../../src/CompDBtoAstIO: $(srcdir)/../../src/CompDBtoAstIO.cxx
	make -C $(builddir)/../../src CompDBtoAstIO

$(builddir)/normalized_compile_commands.json: $(builddir)/compile_commands.json $(srcdir)/../../scripts/normalize.py
	python $(srcdir)/../../scripts/normalize.py $(builddir)/compile_commands.json $(builddir)/normalized_compile_commands.json

$(builddir)/demo.ast: $(builddir)/../../src/CompDBtoAstIO $(builddir)/normalized_compile_commands.json $(srcdir)/demo-0.c $(srcdir)/demo-1.c $(srcdir)/demo.h
	$(builddir)/../../src/CompDBtoAstIO $(builddir)/normalized_compile_commands.json $(builddir)/demo.ast

