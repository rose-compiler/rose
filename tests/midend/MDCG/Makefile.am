include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = build-model

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

$(top_builddir)/src/backend/MDCG/lib/libMDCG.la:
	$(MAKE) -C $(top_builddir)/src/backend/MDCG/lib libMDCG.la

build_model_SOURCES = build-model.cpp
build_model_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
build_model_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-model: build-model
	echo "one bar $(srcdir) h" > test.dat
	./build-model test.dat model_test.dot
	echo "one kernel $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >  acc_internal.dat
	echo "one loop $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one region $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one compiler $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one data-env $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one init $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one mem-manager $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one opencl-debug $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one opencl-init $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	echo "one runtime $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> acc_internal.dat
	./build-model acc_internal.dat model_acc_internal.dot -I$(top_srcdir)/../libopenacc/include/ -I/export/tmp.vanderbrugge1/scratch/
	echo "one openacc $(top_srcdir)/../libopenacc/include/OpenACC/ h" >  openacc.dat
	echo "one data-env $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one debug $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one init $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one kernel $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one loop $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one region $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one runtime $(top_srcdir)/../libopenacc/include/OpenACC/private h" >> openacc.dat
	echo "one kernel $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >>  openacc.dat
	echo "one loop $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one region $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one compiler $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one data-env $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one init $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one mem-manager $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one opencl-debug $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one opencl-init $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	echo "one runtime $(top_srcdir)/../libopenacc/include/OpenACC/internal h" >> openacc.dat
	./build-model openacc.dat model_openacc.dot -I$(top_srcdir)/../libopenacc/include/ -I/export/tmp.vanderbrugge1/scratch/

check-local: build-model
	$(MAKE) check-model

clean-local:
	rm -f *.hpp *.cpp *.dat *.png *.dot

