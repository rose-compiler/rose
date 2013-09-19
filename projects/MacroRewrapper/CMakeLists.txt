
set(MacroRewrapper_libs ROSE_DLL RoseSQLite3xDatabase ${Boost_LIBRARIES})

add_executable(macroRewrapper
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C main.C
  helpFunctions.C
)
target_link_libraries(macroRewrapper ${MacroRewrapper_libs})

add_executable(testLinearization
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C
  testLinearization.C helpFunctions.C
)
target_link_libraries(testLinearization ${MacroRewrapper_libs})

add_executable(testMapping
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C
  testMapping.C helpFunctions.C
)
target_link_libraries(testMapping ${MacroRewrapper_libs})

add_executable(testPruning
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C
  testPruning.C helpFunctions.C
)
target_link_libraries(testPruning ${MacroRewrapper_libs})

