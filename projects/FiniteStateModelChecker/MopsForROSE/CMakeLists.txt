noinst_LIBRARIES = libMopsForROSE.a

GCJFLAGS += -I$(srcdir)
libMopsForROSE_a_SOURCES = mops/Ast.java mops/CfgFunction.java mops/Cfg.java mops/Edge.java mops/Fsa.java mops/FsaTransition.java mops/FsaTransitionTrace.java mops/LineReader.java mops/MetaFsa.java mops/Node.java mops/Pda.java mops/PdaTransition.java mops/StateLabel.java mops/TransitionBridge.java mops/Util.java

all: libMopsForROSE.a $(libMopsForROSE_a_SOURCES:.java=.h)


# We need to compile the source file again to produce suitable input for gcjh

foreach(java_file ${java_files})
  add_custom_command(
    OUTPUT ${java_file}.class
    COMMAND ${GCJ} ${GCJFLAGS} -C -d . ${java_file}.java
  )

  add_custom_command(
    OUTPUT ${java_file}.h
    DEPENDS ${java_file}.class
    COMMAND ${GCJH} -o ${java_file}.h mops.${javafile}

endforeach()

