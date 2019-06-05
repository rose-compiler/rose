# Configuration rejects for Debian-9

# Boost
if [ "$rmc_boost" = 1.58.0 ]; then
    if [ "$rmc_python" = 3.6.1 ]; then
	# compiler error: 'pyconfig.h' no such file or directory
	reject-configuration "boost-$rmc_boost cannot use python-$rmc_python"
    fi
fi

# Yaml-CPP
if [ "$rmc_yaml" = 0.5.1 ]; then
    if version-ge "$rmc_boost" 1.67.0; then
	# compiler error: 'next' is not a member of 'boost'
	reject-configuration "yaml-$rmc_yaml cannot use boost-$rmc_boost"
    fi
elif [ "$rmc_yaml" = 0.5.3 ]; then
    if version-ge "$rmc_boost" 1.67.0; then
	# compiler error: 'next' is not a member of 'boost'
	reject-configuration "yaml-$rmc_yaml cannot use boost-$rmc_boost"
    fi
fi
