
TARGET= \
  dlx mfb mdcg \
  klt klt-rtl \
  tilek-basic tilek-rtl-basic \
  tilek-threads tilek-rtl-threads \
  tilek-opencl tilek-rtl-opencl \
  tilek-cuda tilek-rtl-cuda

html: $(TARGET)

%: %.doxy
	doxygen $<

clean:
	rm -rf $(TARGET)

