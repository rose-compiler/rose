#!/bin/bash
# Check that "identityTranslator --version" runs

enclosing_directory=`dirname $0`
export ROSE_SCRIPT_DIR=`(cd ${enclosing_directory}; pwd)`
source ${ROSE_SCRIPT_DIR}/declare_install_functions.sh

use_latest_intel_19_0_4_rose

print_rose_vars
${ROSE_HOME}/bin/identityTranslator --version

