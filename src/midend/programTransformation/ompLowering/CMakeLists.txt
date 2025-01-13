########### header files to be installed ###############
set(OMPLOWERING_HEADERS OmpSupport.h omp_lowering.h libgomp_g.h libompc.h libxompf.h libxomp.h)

# OmpSupport.h fails on windows, so we remove it in that case 
if(WIN32)
  list(REMOVE_ITEM ${OMPLOWERING_HEADERS} OmpSupport.h)
endif() 
  
install(FILES ${OMPLOWERING_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR})
