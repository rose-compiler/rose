include_directories( ${LIBXML2_INCLUDE_DIR} include )

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(docs)


add_library( roseExtensions OBJECT
  src/profir2sage/attach.cc
  src/xml2profir/query.cc
  src/xml2profir/convert.cc
  src/xml2profir/factory.cc
  src/xml2profir/process.cc
  src/gprof/gprof_info.cc
  src/sage/depth.cc
  src/sage/metric.cc
  src/sage/collect.cc
  src/sage/propagate.cc
  src/util/general.cc
  src/rosehpct.cc
  src/profir/profir.cc
  src/xml/util.cc
  src/xml/handler.cc
  src/xml/elem.cc
  src/xml/parser.cc
  )
add_dependencies(roseExtensions rosetta_generated)
