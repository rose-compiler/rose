add_library(experimentalRoseLibadalang OBJECT
  ada_support.C
  AdaMaker.C
  Libadalang_to_ROSE.C
  LibadalangExpression.C
  LibadalangStatement.C
  LibadalangType.C
)

add_dependencies(experimentalRoseLibadalang rosetta_generated)

target_include_directories(experimentalRoseLibadalang PRIVATE
  ${ROSE_EXPERIMENTAL_LIBADALANG_ROSE_CONNECTION_GNAT_HOME}/include
)

target_link_directories(experimentalRoseLibadalang PUBLIC
  ${ROSE_EXPERIMENTAL_LIBADALANG_ROSE_CONNECTION_GNAT_HOME}/lib
  /opt/rosedev/GNAT/2021/lib/gcc/x86_64-pc-linux-gnu/10.3.1/adalib
)

target_link_libraries(experimentalRoseLibadalang PRIVATE adalang)

install(FILES AdaMaker.h DESTINATION include/rose)
