# The purpose of this file is soley to check if the user's environment has been configured appropriately to enable Clang
# for the Cxx Frontend Support in ROSE 

# Create list of environment variables which typically are set by sourcing the path below 
# /<internal_path>/llvm/18.0.0/gcc/cxx17/12.2.0/setup.sh
set(CLANG_FRONTEND_ENV_VARS 
   "GCC_HOME"
   "MPC_HOME"
   "MPC_VERSION"
   "GMP_HOME"
   "GMP_VERSION"
   "MPFR_HOME"
   "MPFR_VERSION"
) 

# Loop over the environment variables and ensure check that each has been set
foreach(env_var IN LISTS CLANG_FRONTEND_ENV_VARS)
  if(NOT DEFINED ENV{${env_var}})
    message(FATAL_ERROR "Environment variable '${env_var}' is not set and required for Clang Frontend Support.")
  else()
    message(STATUS "Environment variable '${env_var}' is set to: $ENV{${env_var}}")
  endif()
endforeach()

