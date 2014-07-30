
$(builddir)/../src/tooling:
	make -C $(builddir)/../src tooling

check-local: $(builddir)/../src/tooling $(srcdir)/test_001.c
	$(builddir)/../src/tooling -c $(srcdir)/test_001.c

clean-local:
	rm -f rose_* *.o *.c

