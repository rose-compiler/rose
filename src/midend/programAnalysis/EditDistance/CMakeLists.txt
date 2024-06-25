# These files install in the same location as the Autoconf system
# i.e., /<rose-install-dir>/include/rose 
# previously this was /<rose-install-dir>/include/rose/EditDistance 
# rose-install-dir == ${CMAKE_INSTALL_PREFIX}
install(FILES       DamerauLevenshtein.h
                    EditDistance.h
                    Levenshtein.h
                    TreeEditDistance.h
                    LinearEditDistance.h
        DESTINATION ${INCLUDE_INSTALL_DIR})

