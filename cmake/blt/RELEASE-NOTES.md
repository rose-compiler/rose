# BLT Software Release Notes

Notes describing significant changes in each BLT release are documented
in this file.

The format of this file is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).

The project release numbers follow [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased] - Release date yyyy-mm-dd

### Added
- Added 'blt_convert_to_system_includes' macro to convert existing interface includes to system interface includes.

## [Version 0.5.1] - Release date 2022-04-22

### Added
- Added support for C++20. Note: XL does not support C++20.
  While PGI has C++20 support, it is currently disabled (A BLT fatal error will occur).
- BLT_CXX_STD now sets CMAKE_HIP_STANDARD, in CMake 3.21+, similar to CMAKE_CUDA_STANDARD.

### Fixed
- Removed hard-coded -std=c++11 from various places related to CUDA flags.  This now honors
  CMAKE_CUDA_STANDARD if set otherwise falls back on BLT_CXX_STD or CMAKE_CXX_STANDARD.
- Removed extra HIP offload flags that were being added as generator expressions as opposed to simple
  flags.
- Skip check for valid `ELEMENTS` parameter in `blt_list_append` macro when not appending

### Removed
- Removed support for deprecated HCC.

## [Version 0.5.0] - Release date 2022-03-07

### Added
- Added support for IntelLLVM compiler family to blt_append_custom_compiler_flag
- Added support for hip targets configured with cmake 3.21 native hip support
- Added `blt_export_tpl_targets` macro to add BLT-provided third-party library
  targets to an export set.

### Changed
- `BLT_C_FILE_EXTS` updated to include `.cuh`
- Fold `BLT_CLANG_HIP_ARCH` into the `CMAKE_HIP_ARCHITECTURES` variable
- When using `ENABLE_ALL_WARNINGS`, append the flag to the beginning of `CMAKE_{C,CXX}_FLAGS` instead
  of the end
- HIP support now uses the `hip-config.cmake` file provided by ROCM. This
  modification requires a change to the BLT-provided HIP target names, and they
  are now available under the `blt` prefix: `blt::hip` and `blt::hip_runtime`.

### Fixed
- Source code filename extension filtering now uses regular expressions to allow
  for more user customization and to improve handling of file names with multiple
  periods, e.g. `1d.cube.order2.c` is considered a `.c` file.

## [Version 0.4.1] - Release date 2021-07-20

### Added
- Added compilation of HIP with clang using ``ENABLE_CLANG_HIP`` and ``BLT_CLANG_HIP_ARCH``

### Changed
- XL: Use compiler flag `-std=c++14` instead of `-std=c++1y` when `BLT_CXX_STD` is set to `c++14`

### Fixed
- Simpified the clang-format version regex that was causing hangs on some version strings.

## [Version 0.4.0] - Release date 2021-04-09

### Added
- Added variable ``BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE`` for filtering
  link directories implicitly added by CMake. See the following example host-config:
  ``host-configs/llnl/blueos_3_ppc64le_ib_p9/clang@upstream_nvcc_c++17.cmake``
- Added support for clang-tidy static analysis check
- Added ability to change the output name of an executable via the OUTPUT_NAME
  parameter of blt_add_executable
- Added user option for enforcing specific versions of autoformatters - the new options are
  ``BLT_REQUIRED_ASTYLE_VERSION``, ``BLT_REQUIRED_CLANGFORMAT_VERSION``, and ``BLT_REQUIRED_UNCRUSTIFY_VERSION``
- Added ``HEADERS`` to ``blt_add_executable``.  This is important for build system dependency tracking
  and IDE folder support.
- Added support for formatting Python code using YAPF.
- Added new ``blt_import_library`` macro that creates a real CMake target for imported libraries,
  intended to be used instead of ``blt_register_library`` whenever possible
- Added new ``blt_patch_target`` macro to simplify modifying properties of an existing CMake target.
  This macro accounts for known differences in compilers, target types, and CMake releases.
- Added support for formatting CMake code using cmake-format.
- Added an EXPORTABLE option to ``blt_import_library`` that allows imported libraries to be
  added to an export set and installed.
- Added FRUIT's MPI parallel unit test reporting to BLT's internal copy of FRUIT
- CUDA device links for object libraries can be enabled with the existing ``CUDA_RESOLVE_DEVICE_SYMBOLS``
  target property.

### Changed
- MPI Support when using CMake 3.13 and newer: MPI linker flags are now passed
  as single string prefixed by ``SHELL:`` to prevent de-duplication of flags
  passed to ``target_link_options``.
- For HIP-dependent builds, only add HCC include directory if it exists.
- HIP CMake utilities updated to AMD's latest version
- Updated ``add_code_coverage_target`` to ``blt_add_code_coverage_target``, which now supports
  user-specified source directories
- Code coverage targets leave LCOV-generated files intact for later use; these files will
  still be removed by ``make clean``

### Fixed
- ClangFormat checks now support multiple Python executable names
- Prefixed blt_register_library() internal variables with ``_`` to avoid collision
  with input parameters.
- Turn off system includes for registered libraries when using the PGI compiler
- Removed unneeded SYSTEM includes added by googletest that was causing problems
  in PGI builds (BLT was adding them already to the register library calls)
- Removed variable ``BLT_CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES_EXCLUDE``, functionality now
  provided for all languages using ``BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE`` variable
- ClangQuery was being auto-enabled in cases where no checker directories where defined.
  This caused a crash. blt_add_clang_query_target learned parameter CHECKER_DIRECTORIES
  and blt_add_code_checks learned parameter CLANGQUERY_CHECKER_DIRECTORIES.  Both still
  respect BLT_CLANGQUERY_CHECKER_DIRECTORIES.
- It is now a fatal error to supply CLANGFORMAT_CFG_FILE plus
  ASTYLE_CFG_FILE or UNCRUSTIFY_CFG_FILE arguments to
  blt_add_code_checks; previously, these combinations were implied to
  be errors in BLT documentation, but BLT would not return an error in
  those cases.
- ``blt_patch_target`` no longer attempts to set system include directories when a target
  has no include directories
- Header-only libraries now can have dependencies via DEPENDS_ON in ``blt_add_library``
- Added a workaround for include directories of imported targets on PGI. CMake was
  erroneously marking them as SYSTEM but this is not supported by PGI.
- Check added to make sure that if HIP is enabled with fortran, the LINKER LANGUAGE
  is not changed back to Fortran.
- Executables that link to libraries that depend on `hip`/`hip_runtime`/`cuda`/`cuda_runtime`
  will automatically be linked with the HIP or CUDA (NVCC) linker
- Patched an issue with the FindHIP macros that added the inclusive scan of specified
  DEFINEs to compile commands
- Re-added previous OpenMP flag patching logic to maintain compatibility with BLT-registered libraries

## [Version 0.3.6] - Release date 2020-07-27

### Changed
- ``CUDA_TOOLKIT_ROOT_DIR`` is now optional. If it is not specified, FindCUDA.cmake will
  attempt to set it.

## [Version 0.3.5] - Release date 2020-07-20

### Added
- Added blt_assert_exists() utility macro.
- Additional link flags for CUDA may now be specified by setting
  ``CMAKE_CUDA_LINK_FLAGS`` when configuring CMake either in a host-config
  or at the command-line.
- Added support for ClangFormat.

### Changed
- ``CUDA_TOOLKIT_ROOT_DIR`` must now be set in order to use CUDA. If it is not
  specified, BLT will produce an error message.

### Fixed
- blt_add_test is no longer trying to extract target properties from non-targets.
- Improved support for HIP 3.5.
- Improved support for CMake 3.13.0+.
- Remove some known spaces that show up in MPI link flags.
- Remove GTest and GBenchmark adding '-Werror' that got inherited.


## [Version 0.3.0] - Release date 2020-01-08

### Added
- Sets CMake policy CMP0074 to NEW, when available.
- Added simpler Clang+XLF+Cuda host-config for LLNL's blueos
- API Docs that are public!
- Added the ability to override blt's custom target names, e.g. for code checks,
  formatting and generating documentation. The new variables are: ``BLT_CODE_CHECK_TARGET_NAME``,
  ``BLT_CODE_STYLE_TARGET_NAME``, ``BLT_DOCS_TARGET_NAME`` and  ``BLT_RUN_BENCHMARKS_TARGET_NAME``.
- Clean up linking flags when ``CUDA_LINK_WITH_NVCC`` is ON. Added logic to automatically convert
  '-Wl,-rpath' linking flag to '-Xlinker -rpath -Xlinker' and removes ``-pthread`` from from
  MPI linking flags returned from FindMPI because it doesn't work
  (see https://gitlab.kitware.com/cmake/cmake/issues/18008).
- In CMake 3.13+, "SHELL:" was added to blt_add_target_link_flags.  This stops CMake from de-duplicating
  needed linker flags.
- Added optional SCOPE to all target property macros, blt_add_target_link_flags, etc.  It defaults to PUBLIC.
- Added support for Cray compilers in blt_append_custom_compiler_flag.
- Added ability to add flags to the cppcheck command line through blt_add_code_checks()
- Added ability for blt_add_test() to set required number of OpenMP threads via new option NUM_OMP_THREADS.
- Added ClangFormat as an option for code styling.  This has some caveats that are noted here:
  https://llnl-blt.readthedocs.io/en/develop/api/code_check.html

### Changed
- Restructured the host-config directory by site and platform.
- Updated gbenchmark to 1.5.0, note that this requires C++11 to build.
- Updated gtest and gmock to Master as of 2020-01-07, note that this requires C++11 to build.

### Fixed
- Fixed some warnings in CMake 3.14+
- Duplication of MPI link flags in CMake 3.14+ when Fortran was enabled.

### Removed
- Removed unused ``HEADERS_OUTPUT_SUBDIR`` argument from blt_add_library().


## [Version 0.2.5] - Release date 2019-06-13

### Added
- Added support for C++17. Note: Neither XL nor CMake's CUDA_STANDARD supports
  C++17 (A BLT fatal error will occur).
- Added ability to override all MPI variables: BLT_MPI_COMPILE_FLAGS,
  BLT_MPI_INCLUDES, BLT_MPI_LIBRARIES, and BLT_MPI_LINK_FLAGS
- blt_list_remove_duplicates(): macro for removing duplicates from a list that
  doesn't error on empty lists.

### Changed
- Handle CMake 3.10+ changing all the FindMPI output variables.
- BLT_CXX_STD is no longer defined to "c++11" by default. If undefined, BLT will
  not try and add any C++ standard flags.
- Handle FindMPI variable MPIEXEC changed to MPIEXEC_EXECUTABLE in CMake 3.10+.
  This now works regardless of which the user defines or what CMake returns.
- Handle CMake target property LINK_FLAGS changed to LINK_OPTIONS in CMake 3.13+.
  blt_add_target_link_flags() handles this under the covers and converts the
  users strings to a list (3.13+) or list to a string (<3.13).  New property supports
  generator expressions so thats a plus.
- Improved how all BLT MPI information is being merged together and reported to users.
- Increased number of ranks in `blt_mpi_smoke` test to catch regression.
- blt_split_source_list_by_language now supports non-BLT object libraries and errors
  out when any other generator expression is given in source list.  This is to avoid
  very bad side effects of not being able to set source properties on anything
  inside the generator expression.  This is because BLT cannot evaluate them.

### Fixed
- Error out with better message when empty file extensions is hit in
  blt_split_source_list_by_language.


## [Version 0.2.0] - Release date 2019-02-15

### Added
- Release notes...
- Explicitly check for CMake 3.8+ for required CMake features (Not fatal error)
- Object library support through blt_add_library(... OBJECT TRUE ...)
- Now reporting BLT version through CMake cache variable BLT_VERSION
- Output CMake version and executable used during CMake step
- Clang-query support now added (Thanks David Poliakoff)

### Removed

### Deprecated

### Changed
- Object libraries no longer call target_link_libraries() but will pass inherited information
  because why would anyone ever want to install/export a bunch of object files.
- Remove duplicates of select target properties at the end of blt_add_library and
  blt_add_executable. (include directories and compile defines)

### Fixed
- Incorrect use of cuda vs cuda_runtime targets
- Improved tutorial documentation
- Incorrect use of Fortran flags with CUDA (Thanks Robert Blake)
- Handle correctly CMake version differences with CUDA host compiler variables
  (Thanks Randy Settgast)
- Handle uncrustify (version 0.68) command line option changes (--no-backup)

### Known Bugs



[Unreleased]:    https://github.com/LLNL/blt/compare/v0.5.1...develop
[Version 0.5.1]: https://github.com/LLNL/blt/compare/v0.5.0...v0.5.1
[Version 0.5.0]: https://github.com/LLNL/blt/compare/v0.4.1...v0.5.0
[Version 0.4.1]: https://github.com/LLNL/blt/compare/v0.4.0...v0.4.1
[Version 0.4.0]: https://github.com/LLNL/blt/compare/v0.3.6...v0.4.0
[Version 0.3.6]: https://github.com/LLNL/blt/compare/v0.3.5...v0.3.6
[Version 0.3.5]: https://github.com/LLNL/blt/compare/v0.3.0...v0.3.5
[Version 0.3.0]: https://github.com/LLNL/blt/compare/v0.2.5...v0.3.0
[Version 0.2.5]: https://github.com/LLNL/blt/compare/v0.2.0...v0.2.5
[Version 0.2.0]: https://github.com/LLNL/blt/compare/v0.1.0...v0.2.0
