#------------------------------------------------------------------------------
# add a target to generate documentation with sphinx
#------------------------------------------------------------------------------

if(SPHINX_FOUND)
    blt_add_sphinx_target( calc_pi_sphinx )
endif()
