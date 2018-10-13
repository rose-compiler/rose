
MAP_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py
TOOL=$(abs_top_builddir)/tests/nonsmoke/functional/testTranslator

report.json: $(MAP_SCRIPT) $(TOOL) $(builddir)/compile_commands.json $(srcdir)/demo-0.c $(srcdir)/demo-1.c $(srcdir)/demo.h
	python $(MAP_SCRIPT) $(srcdir) $(builddir) $(TOOL)

check-local: report.json

clean-local:
	rm -f $(builddir)/rose_demo-0.c $(builddir)/demo-0.o $(builddir)/rose_demo-1.c $(builddir)/demo-1.o

