#!/bin/bash
# Validate that generated files are the same age or newer then the files they were generated from.

EXIT_CODE=0

# Check lex.yy.C ==================================================================
# Get the UNIX timestamp of the last commit for each file.
FILE1_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/preproc-c.ll)
FILE2_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/lex.yy.C.pre)

# Compare the timestamps and error if file1 is newer than file2.
if [ "$FILE1_TIMESTAMP" -gt "$FILE2_TIMESTAMP" ]; then
  echo "Error: preproc-c.ll has been modified and lex.yy.C.pre has not been regenerated"
  EXIT_CODE=1
fi

# Check omp-lex.yy.C ==============================================================
# Get the UNIX timestamp of the last commit for each file.
FILE1_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/omplexer.ll)
FILE2_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/omp-lex.yy.C.pre)

# Compare the timestamps and error if file1 is newer than file2.
if [ "$FILE1_TIMESTAMP" -gt "$FILE2_TIMESTAMP" ]; then
  echo "Error: omplexer.ll has been modified and omp-lex.yy.C.pre has not been regenerated"
  EXIT_CODE=1
fi

# Check ompparser.C ===============================================================
# Get the UNIX timestamp of the last commit for each file.
FILE1_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/ompparser.yy)
FILE2_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/ompparser.C.pre)

# Compare the timestamps and error if file1 is newer than file2.
if [ "$FILE1_TIMESTAMP" -gt "$FILE2_TIMESTAMP" ]; then
  echo "Error: ompparser.yy has been modified and ompparser.C.pre has not been regenerated"
  EXIT_CODE=1
fi

# Check ompparser.C ===============================================================
# Get the UNIX timestamp of the last commit for each file.
FILE1_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/ompparser.yy)
FILE2_TIMESTAMP=$(git log -1 --format=%ct -- src/frontend/SageIII/ompparser.h.pre)

# Compare the timestamps and error if file1 is newer than file2.
if [ "$FILE1_TIMESTAMP" -gt "$FILE2_TIMESTAMP" ]; then
  echo "Error: ompparser.yy has been modified and ompparser.h.pre has not been regenerated"
  EXIT_CODE=1
fi

# Check LicenseString.h ===========================================================
# Get the UNIX timestamp of the last commit for each file.
FILE1_TIMESTAMP=$(git log -1 --format=%ct -- LicenseInformation/ROSE_BSD_License.txt)
FILE2_TIMESTAMP=$(git log -1 --format=%ct -- src/Rose/CommandLine/LicenseString.pre)

# Compare the timestamps and error if file1 is newer than file2.
if [ "$FILE1_TIMESTAMP" -gt "$FILE2_TIMESTAMP" ]; then
  echo "Error: ROSE_BSD_License.txt has been modified and LicenseString.pre has not been regenerated"
  EXIT_CODE=1
fi

# Check rose_generated_builtin_functions.h ========================================
# Get the UNIX timestamp of the last commit for each file.
FILE1_TIMESTAMP=$(git log -1 --format=%ct -- config/Builtins.def)
FILE2_TIMESTAMP=$(git log -1 --format=%ct -- cmake/rose_generated_builtin_functions.h.pre)

# Compare the timestamps and error if file1 is newer than file2.
if [ "$FILE1_TIMESTAMP" -gt "$FILE2_TIMESTAMP" ]; then
  echo "Error: Builtins.def has been modified and rose_generated_builtin_functions.h.pre has not been regenerated"
  EXIT_CODE=1
fi

exit $EXIT_CODE