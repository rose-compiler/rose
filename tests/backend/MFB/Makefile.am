include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = test

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

test_SOURCES = test.cpp
test_CXXFLAGS = \
  -I$(top_srcdir)/src/backend/MFB/include \
  -g \
  $(ROSE_INCLUDES)
test_LDADD= \
  $(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local: test
	./test
	@echo "============="
	@echo "============="
	@echo "*** A.hpp ***"
	@echo
	@cat A.hpp
	@echo
	@echo "============="
	@echo "*** A.cpp ***"
	@echo
	@cat A.cpp
	@echo
	@echo "============="
	@echo "============="
	@echo "*** B.hpp ***"
	@echo
	@cat B.hpp
	@echo
	@echo "============="
	@echo "*** B.cpp ***"
	@echo
	@cat B.cpp
	@echo
	@echo "============="
	@echo "============="
	@echo "*** main.cpp ***"
	@echo
	@cat main.cpp
	

clean-local:
	rm -f A.hpp A.cpp B.hpp B.cpp main.cpp
