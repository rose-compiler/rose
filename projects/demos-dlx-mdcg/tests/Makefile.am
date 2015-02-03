
$(builddir)/../src/logger:
	make -C $(builddir)/../src logger

check-local: $(builddir)/../src/logger $(srcdir)/test_001.c
	$(builddir)/../src/logger -c $(srcdir)/test_001.c

clean-local:
	rm -f rose_* *.o *.c

