# define various sets of C files as input for other tests
nonsmoke_genericCoverage_c= \
    helloworld.cpp

# PP 17/05- not sure if the tests below work on all - non-gcc - backend compilers
#   longFile.C 
#   test2006_172.C

# PP 17/05 The tests below will not work on older gcc backend compilers.
#    test2014_158.C 
#    test2014_237.C 
#    test2016_90.C


# verify all for now
nonsmoke_genericCoverage_c_verify=$(nonsmoke_genericCoverage_c)
