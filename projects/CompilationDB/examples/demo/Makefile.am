
MAP_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py
RENDER_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_render.py
TOOL=$(abs_top_builddir)/tests/nonsmoke/functional/testTranslator

testTranslator.json: $(builddir)/compile_commands.json $(srcdir)/demo-0.c $(srcdir)/demo-1.c $(srcdir)/demo.h $(MAP_SCRIPT) $(TOOL)
	python $(MAP_SCRIPT) $(srcdir) $(builddir) $(TOOL)

testTranslator.html: $(builddir)/testTranslator.json $(RENDER_SCRIPT)
	python $(RENDER_SCRIPT) $(builddir)

check-local: testTranslator.html

clean-local:
	rm -f $(builddir)/rose_demo-0.c $(builddir)/demo-0.o $(builddir)/rose_demo-1.c $(builddir)/demo-1.o

