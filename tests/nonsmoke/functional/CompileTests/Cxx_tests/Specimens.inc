# define various sets of C files as input for other tests
nonsmoke_genericCoverage_c= \
    longFile.C \
    test2006_172.C \
    test2014_158.C \
    test2014_237.C \
    test2016_90.C

#   helloworld.cpp

# verify all for now
nonsmoke_genericCoverage_c_verify=$(nonsmoke_genericCoverage_c)
