######################################################
# These are C+ testcodes targeting aspects of both C #
# and C++ that can be compiled using a C++ compiler  #
#                                                    #
# This Makefile stub defines the macro:              #
#                                                    #
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS               #
#     A list of all possible example test codes that #
#     should pass.                                   #
#                                                    #
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT         #
#     A shorter list for faster testing.             #
#                                                    #
######################################################

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
     test07.C test09.C  test10.C  test11.C \
     test12.C test13.C  test14.C  \
     test16.C test19.C  test20.C  test21.C \
     test42.C test43.C  test44.C  test45.C \
     test47.C test48.C  test50.C  test53.C \
     test56.C test58.C  \
     test60.C

# DQ (6/22/2011): Moved this to the the Cxx_tests directory (for separate testing).
# test57.C

EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT = \
     test07.C test09.C  test10.C  test11.C \
     test12.C test13.C  test14.C 

# DQ (5/22/2007): This test code from Robert demonstrated an error in namespace aliasing
# test22.C

# DQ (1/3/2008): These test codes fail, and it might be recent.
# test15.C 
# test59.C 
