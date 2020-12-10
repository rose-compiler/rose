add_subdirectory(ctests)
add_subdirectory(gnu)
add_subdirectory(kandr)
add_subdirectory(std)

set(TESTCODES
  t0001.cc t0002.cc t0003.cc t0004.cc t0005.cc t0008.cc t0009.cc t0010.cc
  t0011.cc t0013.cc t0014a.cc t0015.cc t0016.cc t0017.cc t0018.cc t0019.cc
  t0020.cc t0021.cc t0022.cc t0023.cc t0024.cc t0025.cc t0026.cc t0027.cc
  t0028.cc t0029.cc t0030a.cc t0030b.cc t0034.cc t0035.cc t0037.cc t0038.cc
  t0039.cc t0043.cc t0044.cc t0045.cc t0046.cc t0047.cc t0048.cc t0049.cc
  t0050.cc t0051.cc t0052.cc t0053.cc t0056.cc t0057.cc t0058.cc t0059.cc
  t0060.cc t0064.cc t0065.cc t0066.cc t0067.cc t0070.cc)

set(ROSE_FLAGS --edg:no_warnings -w)

foreach(file_to_test ${TESTCODES})
  compile_test(${file_to_test})
endforeach()
