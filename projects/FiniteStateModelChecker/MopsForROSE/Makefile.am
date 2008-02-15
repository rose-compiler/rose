noinst_LIBRARIES = libMopsForROSE.a

GCJFLAGS += -I$(srcdir)
libMopsForROSE_a_SOURCES = mops/Ast.java mops/CfgFunction.java mops/Cfg.java mops/Edge.java mops/Fsa.java mops/FsaTransition.java mops/FsaTransitionTrace.java mops/LineReader.java mops/MetaFsa.java mops/Node.java mops/Pda.java mops/PdaTransition.java mops/StateLabel.java mops/TransitionBridge.java mops/Util.java

all: libMopsForROSE.a $(libMopsForROSE_a_SOURCES:.java=.h)

#libMopsForROSE.a: $(libMopsForROSE_a_OBJECTS)
#	-rm -f $@
#	${CXX_STATIC_LIB_UPDATE} $@ $(libMopsForROSE_a_OBJECTS)
#	test -f $@ || exit 1

mops/%.h: mops/%.class
	$(GCJ)h -o $@ -I. mops.$(notdir $(basename $<))

# We need to compile the source file again to produce suitable input for gcjh
mops/%.class: $(srcdir)/mops/%.java
	$(GCJ) $(GCJFLAGS) -C -d . $<

clean-local:
	rm -f mops/*.h mops/*.class
