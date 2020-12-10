# Configuration rejects for CentOS-7

# Yaml-CPP
if [ "$rmc_yaml" = none ]; then
    : okay
elif version-ge "$rmc_yaml" 0.6; then
    if [ "$rmc_cmake" = "system" ]; then
	# Cmake is too old
	reject-configuration "yaml-$rmc_yaml cannot use cmake-$rmc_cmake"
    fi
fi
