
check-local: $(builddir)/demo.rosebin

$(builddir)/../../src/CompDBtoAstIO: $(srcdir)/../../src/CompDBtoAstIO.cxx
	make -C $(builddir)/../../src CompDBtoAstIO

$(builddir)/normalized_compile_commands.json: $(builddir)/compile_commands.json $(srcdir)/../../scripts/normalize.py
	python $(srcdir)/../../scripts/normalize.py $(builddir)/compile_commands.json $(builddir)/normalized_compile_commands.json

$(builddir)/demo.rosebin: $(builddir)/../../src/CompDBtoAstIO $(builddir)/normalized_compile_commands.json $(srcdir)/demo-0.c $(srcdir)/demo-1.c $(srcdir)/demo.h
	$(builddir)/../../src/CompDBtoAstIO $(builddir)/normalized_compile_commands.json $(builddir)/demo.rosebin
	$(builddir)/../../src/CompDBtoAstIO $(builddir)/normalized_compile_commands.json $(builddir)/demo.rosebin unparse compile

clean-local:
	rm -f $(builddir)/demo.rosebin $(builddir)/normalized_compile_commands.json
	rm -f $(builddir)/rose_demo-0.c $(builddir)/demo-0.o $(builddir)/rose_demo-1.c $(builddir)/demo-1.o

