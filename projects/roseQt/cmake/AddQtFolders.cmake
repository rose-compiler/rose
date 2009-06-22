# Adds all files from a subfolder for compilation
# can handle ui,moc, and rc files correctly
# Usage: AddQtFolders( OUTPUT_VAR dir1 dir2 dir3 )
#        add_executable (myexec  $OUTPUT_VAR )

macro ( add_qt_folders OUT_FILES  )

	foreach( DIRECTORY ${ARGN} )
		set( ALL_HEADER )
		set( MOC_HEADER )	
		set( CPP )
		set( UI )
		set( RES )
		set( MOC_OUTPUT_FILES )
		set( UI_OUTPUT_FILES )
		set( RES_OUTPUT_FILES )
	
		file( GLOB ALL_HEADER . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.h )
		file( GLOB CPP        . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.cpp )
		file( GLOB UI         . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.ui )
		file( GLOB RES        . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.qrc )

		# Find files which need moc processing 
		# (are recognized on Q_OBJECT macro inside file)
		foreach( _current_HEADER ${ALL_HEADER} )
			
			GET_FILENAME_COMPONENT(_abs_HEADER ${_current_HEADER} ABSOLUTE)
			FILE( READ ${_abs_HEADER} _contents)
			STRING( REGEX MATCHALL "Q_OBJECT" _match  "${_contents}" )

			IF( _match)	
				LIST( APPEND MOC_HEADER ${_current_HEADER} )
				
			   #GET_FILENAME_COMPONENT(_rel_HEADER ${_current_HEADER} NAME)	
				#MESSAGE ( "Moc Header in ${DIRECTORY} : ${_rel_HEADER}" )			
			ENDIF (_match)
		endforeach( _current_HEADER )
	

	
		QT4_WRAP_CPP(MOC_OUTPUT_FILES ${MOC_HEADER})
		QT4_WRAP_UI(UI_OUTPUT_FILES ${UI} )
		QT4_ADD_RESOURCES(RES_OUTPUT_FILES ${RES})
	
		LIST( APPEND ${OUT_FILES} ${CPP} ${UI_OUTPUT_FILES} ${MOC_OUTPUT_FILES} ${RES_OUTPUT_FILES} )
	
	endforeach( DIRECTORY)		

endmacro (add_qt_folders)


# Scans the current directory and returns a list of subdirectories.
# Third parameter is 1 if you want relative paths returned.
# Usage: list_subdirectories
macro(list_subdirectories  RETURN_VAL curdir return_relative EXCLUDE)

  FILE ( GLOB _subdirs RELATIVE ${curdir} "${curdir}/*" )
#MESSAGE ( "Glob result : ${_subdirs}" )
  
  foreach(dir ${_subdirs})
    if(IS_DIRECTORY ${curdir}/${dir})
          if ( ${return_relative} )
	     LIST( APPEND ${RETURN_VAL} ${dir} ) 
          else()
             LIST( APPEND ${RETURN_VAL} ${curdir}/${dir} ) 
          endif() 
    endif()
  endforeach(dir)
 

  LIST (APPEND ARGN .svn .git CMakeFiles)
  foreach( arg ${ARGN} )
	 LIST (REMOVE_ITEM ${RETURN_VAL} ${arg} )
  endforeach( arg )
   
#foreach( arg ${${RETURN_VAL}} )
#    set(filename  "${curdir}/${arg}/CMakeLists.txt")
# 	 if( EXISTS "${filename}" )
#	   MESSAGE("Removing ${arg} because CMakeLists.txt exists in it" ) 
#	 	LIST (REMOVE_ITEM ${RETURN_VAL} ${arg} )
#	 endif()	
#  endforeach( arg ) 


endmacro(list_subdirectories)


macro( remove_files_from_list theList )

    foreach( arg ${ARGN} )
        GET_FILENAME_COMPONENT(_abs_arg ${arg} ABSOLUTE)
	LIST (REMOVE_ITEM ${theList} ${_abs_arg} )
   endforeach(arg)
endmacro(remove_files_from_list)

