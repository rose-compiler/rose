
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

add_library(rosetollvm
	RoseToLLVMModule.cpp
	CodeAttributesVisitor.cpp
	CodeEmitter.cpp
	CodeGeneratorVisitor.cpp
	LLVMAstAttributes.cpp
	Option.cpp
	Optimize.cpp
	ObjectGenerator.cpp
	Control.cpp
	StringSet.cpp
)
target_link_libraries(rosetollvm ROSE_DLL ${Boost_LIBRARIES})

