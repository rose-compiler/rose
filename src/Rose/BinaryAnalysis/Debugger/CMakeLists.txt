add_library(roseBinaryAnalysisDebugger
	OBJECT
	Base.C
	Linux.C
	ThreadId.C)

add_dependencies(roseBinaryAnalysisDebugger rosetta_generated)

install(FILES
	Base.h
	BasicTypes.h
	Linux.h
	ThreadId.h

	DESTINATION
	${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Debugger)
