# Configuration rejects for Debian-8

# Boost
if [ "$rmc_boost" = "1.57.0" -a "$rmc_python" = "3.6.1" ]; then
    reject-configuration "boost-$rmc_boost cannot use python-$rmc_python"
fi

# Yaml-CPP
if [ "$rmc_yaml" = "0.5.2" -a "$rmc_boost" = "1.68.0" ]; then
    reject-configuration "boost-$rmc_boost and yamlcpp-$rmc_yaml are incompatible"
fi
