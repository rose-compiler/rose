
MAP_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py
TOOL=$(abs_top_builddir)/tests/nonsmoke/functional/testTranslator
COMPDB=$(builddir)/compile_commands.json
NPROCS=4

check-local: $(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py $(COMPDB) $(TOOL) $(srcdir)/demo-0.c $(srcdir)/demo-1.c $(srcdir)/demo.h
	python $(MAP_SCRIPT) $(COMPDB) $(TOOL) $(NPROCS)

clean-local:
	rm -f $(builddir)/rose_demo-0.c $(builddir)/demo-0.o $(builddir)/rose_demo-1.c $(builddir)/demo-1.o

