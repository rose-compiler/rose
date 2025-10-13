if(ENABLE_FORTRAN)
  set(tools_checkFortranInterfaces_exe_HEADERS checkFortranInterfaces.h)

  set(tools_checkFortranInterfaces_exe_SOURCES checkFortranInterfaces.C main.C)

  add_executable(
    tools_checkFortranInterfaces_exe
    ${tools_checkFortranInterfaces_exe_HEADERS}
    ${tools_checkFortranInterfaces_exe_SOURCES})

  target_link_libraries(tools_checkFortranInterfaces_exe PRIVATE ROSE_DLL)

  # TODO: When the refactor goes live, link libraries instead of doing this
  # include hack.
  target_include_directories(
    tools_checkFortranInterfaces_exe
    PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_PREFIX_INSTALL}/include/rose ${ROSE_INCLUDES})
  set_target_properties(tools_checkFortranInterfaces_exe
                        PROPERTIES OUTPUT_NAME "checkFortranInterfaces")

  install(TARGETS tools_checkFortranInterfaces_exe DESTINATION bin)

  add_dependencies(tools tools_checkFortranInterfaces_exe)

endif()
