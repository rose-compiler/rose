#!/bin/bash

# Looks at the current ROSE build environment and spits out information needed by ROSE's
# dependency matrix test results.  The output should be key=value pairs, one per line
# with no space between the key and equal sign.  The keys are those which are recognized by
# the matrixTestResult command.


# List of key=value pairs.
config_vector=()

# Add a key=value pair to the config_vector. We need to be careful of a couple of things:
#  1. If an environment variable is unset then whoever is configuring ROSE probably doesn't know about
#     that dependency and we should report it as "unknown" since there's no way for us to determine how
#     ROSE is configured in that regard.
#  2. If an environment variable is set to the empty string (not unset) then assume that ROSE is being
#     configured without that dependency (e.g., "--without-foo" or "--with-foo=no").
#  3. If an environment variable is the word "ambivalent" then ROSE was configured without specifying
#     anything about that dependency (neither "--with-foo" nor "--without-foo") and that should be made
#     known to the database by also sending it the string 'ambivalent'.
#
# Most of the time the value comes directly from the variable, but sometimes we want to provide our own value
# and use the variable name argument only to decide whether the dependency is known to the configuration system,
# using the specified value only if the variable is set (possibly empty string, as opposed to unset).
append_rmc_record() {
    local key="$1"; shift
    local var="$1"; shift
    # additional args override the variable's value if the variable is set

    local x=$(eval 'echo "${'$var'+is_set}"')
    [ -z "$x" ] && return 0

    local val
    if [ "$#" -gt 0 ]; then
	val="$1-$2-$3"
	val="$(echo "$val" |sed 's/^-*//;s/---*/-/g;s/-*$//')"
    else
	val=$(eval 'echo "$'$var'"')
    fi
    [ "$val" = "system" ] && val=unknown
    [ "$val" = "" ] && val=none

    config_vector=("${config_vector[@]}" "$key=$val")
}

if [ "$RMC_RMC_VERSION" != "" ]; then
    # User is using the ROSE Meta Config system, so look for RMC_* environment variables
    append_rmc_record assertions RMC_ASSERTIONS
    append_rmc_record boost      RMC_BOOST_VERSION
    append_rmc_record build      RMC_BUILD_SYSTEM
    append_rmc_record compiler   RMC_CXX_VERSION        "$RMC_CXX_VENDOR" "$RMC_CXX_VERSION" "$RMC_CXX_LANGUAGE"
    append_rmc_record debug      RMC_DEBUG
    append_rmc_record dlib       RMC_DLIB_VERSION
    append_rmc_record doxygen    RMC_DOXYGEN_VERSION
    append_rmc_record edg        RMC_EDG_VERSION
    append_rmc_record java       RMC_JAVA_VERSION
    append_rmc_record languages  RMC_LANGUAGES
    append_rmc_record magic      RMC_MAGIC_VERSION
    append_rmc_record optimize   RMC_OPTIM
    append_rmc_record os         RMC_OS_NAME_SHORT
    append_rmc_record python     RMC_PYTHON_VERSION
    append_rmc_record qt         RMC_QT_VERSION
    append_rmc_record readline   RMC_READLINE_VERSION
    append_rmc_record sqlite     RMC_SQLITE_VERSION
    append_rmc_record warnings   RMC_WARNINGS
    append_rmc_record wt         RMC_WT_VERSION
    append_rmc_record yaml       RMC_YAML_VERSION
    append_rmc_record yices      RMC_YICES_VERSION
else
    # User is using some other meta config system...
    : not handled yet, Justin
fi    

# Print results
for kv in "${config_vector[@]}"; do
    echo "'$kv'"
done
