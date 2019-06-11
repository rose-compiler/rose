# Configuration rejects that apply to any operating system.

# Build
if [ "$rmc_build" = "tup" ]; then
    if [ "${rmc_languages/fortran/}" != "$rmc_languages" ]; then
        reject-configuration "tup builds do not yet support fortran"
    elif [ "${rmc_languages/all/}" != "$rmc_languages" ]; then
        reject-configuration "tup builds do not yet support fortran"
    fi
fi

# EDG compile.
if [ "$rmc_edg_compile" = 'yes' -a \
     -d "$SOURCE_CODE/src/frontend/CxxFrontend" -a \
     ! -d "$SOURCE_CODE/src/frontend/CxxFrontend/EDG/EDG_5.0/." ]; then
    reject-configuration "EDG compilation requested but no EDG source code present"
fi

# Boost
if  version-le "$rmc_boost" 1.58.0; then
    if [ "$rmc_python" = 3.6.1 ]; then
        # compile error: pyconfig.h: no such file or directory
        reject-configuration "boost-$rmc_boost cannot use python-$rmc_python"
    fi
fi

# Yaml-CPP
if [ "$rmc_yaml" = 0.5.1 ]; then
    if version-ge "$rmc_boost" 1.67.0; then
        # compile error: 'next' is not a member of 'boost'
        reject-configuration "yaml-$rmc_yaml cannot use boost-$rmc_boost"
    fi
elif [ "$rmc_yaml" = 0.5.2 ]; then
    if version-ge "$rmc_boost" 1.67.0; then
        # compile error: 'next' is not a member of 'boost'
        reject-configuration "yaml-$rmc_yaml cannot use boost-$rmc_boost"
    fi
elif [ "$rmc_yaml" = 0.5.3 ]; then
    if version-ge "$rmc_boost" 1.67.0; then
        # compile error: 'next' is not a member of 'boost'
        reject-configuration "yaml-$rmc_yaml cannot use boost-$rmc_boost"
    fi
fi

# Wt
if [ "$rmc_wt" = 3.3.4 ]; then
    if version-ge "$rmc_boost" 1.66.0; then
        # compile error: 'class boost::asio::basic_socket_acceptor<boost::asio::ip::tcp>' has no member named 'native'
        reject-configuration "wt-$rmc_wt cannot use boost-$rmc_boost"
    fi
elif [ "$rmc_wt" = 3.3.5 ]; then
    if version-ge "$rmc_boost" 1.67.0; then
        # compile error: 'strand_' was not declared in this scope
        reject-configuration "wt-$rmc_wt cannot use boost-$rmc_boost"
    fi
elif [ "$rmc_wt" = 3.3.6 ]; then
    if [ "$rmc_boost" = 1.63.0 ]; then
        # compile error: initializing argument 5 of 'static boost::phoenix::expr_ext<...>::type boost::phoenix::expr_ext<...>::make(...)
        reject-configuration "$wt-$rmc_wt cannot use boost-$rmc_boost"
    elif [ "$rmc_boost" = 1.65.0 ]; then
        # compile error: 'make_array' is not a member of 'boost::serialization'
        reject-configuration "$wt-$rmc_wt cannot use boost-$rmc_boost"
    elif version-ge "$rmc_boost" 1.65.1; then
        # compile error
        reject-configuration "wt-$rmc_wt cannot use boost-$rmc_boost"
    fi
fi
