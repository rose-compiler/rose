# Plan to Modernize ROSE CMake Build System for External Package Consumption

## Implementation Status Checklist

This checklist tracks the high-level modernization goals. See the detailed "Implementation Plan by Dependencies" section below for granular step-by-step status.

- [x] **CMake Package Configuration** - Create modern `RoseConfig.cmake` and `RoseConfigVersion.cmake` files
- [x] **Namespaced Targets** - Export `Rose::rose` target instead of plain `rose`
- [x] **Property Propagation** - Automatically propagate include directories, compile features, and definitions
- [x] **Dependency Classification** - Properly mark dependencies as PUBLIC/PRIVATE/INTERFACE
- [ ] **Version Checking** - Support version requirements in `find_package(Rose VERSION)`
- [ ] **Automated Dependency Discovery** - Automatically find all ROSE dependencies from config file
- [ ] **pkg-config Support** - Generate `rose.pc` for non-CMake build systems
- [ ] **Feature Detection Variables** - Export `Rose_ENABLE_*` variables for capability detection
- [ ] **Integration Tests** - Create test suite for external project integration
- [ ] **Documentation & Examples** - Provide clear examples for CMake, Autotools, and Makefiles
- [ ] **Backward Compatibility** - Deprecate old methods while keeping them functional

## Current State Analysis

### Strengths
- CMake build system exists and is relatively new
- Library installation works (installs to `lib/` or `lib64/`)
- Headers install to `include/rose/`
- Basic `FindRose.cmake` exists for external projects

### Weaknesses
1. **No CMake Package Config**: No `RoseConfig.cmake` or `rose-config.cmake` for modern CMake package discovery
2. **No Exported Targets**: `install(TARGETS ROSE_DLL ...)` doesn't use `EXPORT` to create importable targets
3. **Manual Dependency Tracking**: External projects must manually track ROSE's dependencies (Boost, Z3, Dwarf, etc.)
4. **No Namespace**: Library named `rose` instead of modern namespaced target like `Rose::rose`
5. **No Config Version File**: No `RoseConfigVersion.cmake` for version compatibility checking
6. **Legacy FindModule Approach**: Current `FindRose.cmake` uses old-style find_library instead of imported targets
7. **rose-config Script**: The `rose-config` tool provides flags but isn't integrated with CMake's package system
8. **Include Paths**: Complex include structure (`include/rose/`, `include/edg/`) not properly propagated

## Proposed Modernization Plan

This section describes high-level improvements needed to modernize ROSE's CMake infrastructure. The goal is to make ROSE behave like a modern CMake package that external projects can consume with minimal effort, while maintaining compatibility with non-CMake build systems.

### 1. Adopt CMake Package Configuration Files

**Current state:** ROSE provides a legacy `FindRose.cmake` module that requires external projects to manually find the library and set up include paths. This approach is outdated and error-prone.

**Proposed change:** Implement modern CMake "Config-file packages" where ROSE exports its own configuration. When a downstream project calls `find_package(Rose)`, CMake would automatically find and load ROSE's configuration, including all its dependencies, include paths, and compile requirements.

**Benefits:**
- External projects no longer need to manually track ROSE's dependencies
- Installation can be relocated without breaking dependent projects
- Follows modern CMake best practices used by major libraries
- Enables automatic discovery of ROSE's capabilities (binary analysis, language support, etc.)

### 2. Create Namespaced Imported Targets

**Current state:** ROSE installs a library named `rose` (the `ROSE_DLL` target), which can conflict with other projects or libraries named "rose" and doesn't clearly indicate it belongs to ROSE.

**Proposed change:** Export ROSE as a namespaced target `Rose::rose`. This naming convention (Package::component) is the modern CMake standard and immediately communicates the library's origin.

**Benefits:**
- Prevents naming conflicts with other libraries
- Makes code more self-documenting (`Rose::rose` is clearly the ROSE library)
- Enables CMake to detect typos at configuration time rather than link time
- Consistent with how other modern libraries present themselves (e.g., `Boost::filesystem`, `Qt5::Core`)

### 3. Export Target Properties with Installation

**Current state:** When ROSE is installed, the CMake system doesn't export information about the library's compile requirements, include directories, or dependencies. External projects must discover these manually.

**Proposed change:** Use CMake's `install(EXPORT)` mechanism to generate target configuration files that capture all the properties needed to use ROSE correctly. This includes compiler features (C++14), preprocessor definitions, and include directory locations.

**Benefits:**
- Downstream projects automatically get correct compiler flags
- Include directories are automatically added when linking against ROSE
- Build tree and install tree usage both work correctly
- Reduces user error when configuring projects that depend on ROSE

### 4. Properly Classify Dependency Visibility

**Current state:** All of ROSE's dependencies are linked without specifying whether they're part of ROSE's public API or just internal implementation details. This over-exposes implementation details and can cause unnecessary transitive dependencies.

**Proposed change:** Audit each dependency and classify it as:
- **PUBLIC** - Required by ROSE's public headers (e.g., Boost types that appear in ROSE APIs)
- **PRIVATE** - Only needed internally (e.g., Z3, DWARF, Capstone)
- **INTERFACE** - Header-only dependencies needed by consumers

**Benefits:**
- Reduces the number of dependencies downstream projects must find
- Improves link times by not propagating unnecessary libraries
- Makes it clearer which dependencies are truly required vs. optional
- Allows ROSE's internal dependencies to change without breaking downstream projects

### 5. Implement Version Compatibility Checking

**Current state:** There's no formal mechanism for external projects to specify required ROSE versions or for CMake to verify compatibility between requested and found versions.

**Proposed change:** Create a `RoseConfigVersion.cmake` file that implements semantic versioning checks. This allows projects to request specific ROSE versions and have CMake automatically verify compatibility.

**Benefits:**
- Projects can specify minimum ROSE versions: `find_package(Rose 0.11 REQUIRED)`
- Prevents accidental use of incompatible ROSE versions
- Supports version ranges for flexible dependency specifications
- Provides clear error messages when version requirements aren't met

### 6. Automate Dependency Discovery

**Current state:** The existing `FindRose.cmake` doesn't find or verify ROSE's dependencies (Boost, Z3, DWARF, etc.). External projects must independently find all of ROSE's dependencies, which is tedious and error-prone.

**Proposed change:** Create a `RoseConfig.cmake` template that automatically finds all of ROSE's required and optional dependencies using CMake's `find_dependency()` mechanism. The file would know which dependencies are needed based on how ROSE was compiled.

**Benefits:**
- Downstream projects get all dependencies automatically with a single `find_package(Rose)`
- Dependency versions are matched to what ROSE was built with
- Optional dependencies (like Z3 or DWARF) are only required if ROSE was built with them
- Eliminates most boilerplate from projects using ROSE

### 7. Support Non-CMake Build Systems via pkg-config

**Current state:** Projects using Autotools, plain Makefiles, or other build systems have no standard way to discover ROSE's compiler flags and linker options.

**Proposed change:** Generate and install a `rose.pc` pkg-config file that provides compilation and linking flags for non-CMake build systems. This is the standard mechanism used by most Unix/Linux libraries.

**Benefits:**
- Autotools projects can use `PKG_CHECK_MODULES([ROSE], [rose >= 0.11])`
- Makefiles can use `pkg-config --cflags rose` and `pkg-config --libs rose`
- Provides a standardized interface that works across different build systems
- Enables ROSE to be used from languages that don't use CMake

### 8. Maintain Backward Compatibility

**Current state:** Some projects may depend on the existing `FindRose.cmake` module or the `rose-config` script.

**Proposed change:** Keep these existing mechanisms working but deprecate them with clear migration guidance. Gradually phase them out over multiple release cycles.

**Benefits:**
- Existing ROSE users aren't forced to immediately update their build systems
- Clear deprecation warnings guide users toward the modern approach
- Provides time for external projects to migrate
- Reduces support burden for ROSE maintainers over time

### 9. Expose Feature Detection Variables

**Current state:** External projects have no standardized way to detect which features ROSE was compiled with (binary analysis, specific language frontends, etc.).

**Proposed change:** Export CMake variables like `Rose_ENABLE_BINARY_ANALYSIS`, `Rose_ENABLE_C`, etc. that downstream projects can query to determine available functionality.

**Benefits:**
- Projects can conditionally enable features based on ROSE's capabilities
- Better error messages when required ROSE features aren't available
- Enables smarter build-time configuration in downstream projects
- Self-documenting: users can see what features are available

### 10. Document Standard Usage Patterns

**Current state:** Documentation for integrating ROSE into external projects is scattered or outdated.

**Proposed change:** Create comprehensive examples showing how to use ROSE from CMake, Autotools, and plain Makefiles, with clear migration guides for existing users.

**Benefits:**
- Reduces friction for new ROSE users
- Provides canonical examples that can be copy-pasted
- Helps existing users migrate from old patterns to new ones
- Serves as a reference for ROSE maintainers

### 9. Update Documentation

Create examples showing:
- CMake `find_package(Rose)` usage
- Autotools integration via pkg-config
- Manual Makefile integration
- Different build system scenarios

## Benefits for External Projects

### For CMake projects:
```cmake
find_package(Rose REQUIRED)
add_executable(my_tool main.cpp)
target_link_libraries(my_tool PRIVATE Rose::rose)
# Dependencies automatically propagated!
```

### For Autotools projects:
```bash
PKG_CHECK_MODULES([ROSE], [rose >= 0.11])
```

### For manual Makefiles:
```make
ROSE_CFLAGS := $(shell pkg-config --cflags rose)
ROSE_LIBS := $(shell pkg-config --libs rose)
```

## Implementation Plan by Dependencies

This section organizes the implementation into independent, testable steps. Each step builds upon the previous ones but can be completed and verified separately.

**Status Indicators:**
- **Status:** Overall progress (❌ Not Started, 🔄 In Progress, ✅ Complete)
- **Implementation:** Code changes (⬜ Not Implemented, 🔶 Partial, ✅ Implemented)
- **Testing:** Verification (⬜ Not Tested, 🔶 Partial, ✅ Tested)

### Step 1: Add Version Parsing (Foundation)
**Status:** ✅ Complete
**Implementation:** ✅ Implemented
**Testing:** ✅ Tested
**Dependencies:** None
**Time estimate:** 30 minutes
**Files modified:** `CMakeLists.txt`
**GitLab Issue:** #785+

Add version component parsing to the top-level CMakeLists.txt after line 187:

```cmake
# Parse version into components for package configuration
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)" _version_match "${ROSE_PACKAGE_VERSION}")
set(ROSE_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(ROSE_VERSION_MINOR "${CMAKE_MATCH_2}")
set(ROSE_VERSION_PATCH "${CMAKE_MATCH_3}")
set(ROSE_VERSION_BUILD "${CMAKE_MATCH_4}")

message(STATUS "ROSE version components: ${ROSE_VERSION_MAJOR}.${ROSE_VERSION_MINOR}.${ROSE_VERSION_PATCH}.${ROSE_VERSION_BUILD}")
```

**Testing:**
```bash
cd _build
rmc cmake .. | grep "ROSE version components"
# Should output: ROSE version components: 0.11.145.338
```

**Success criteria:** Version variables are set correctly and displayed during configuration.

**Implementation notes:** ROSE uses a four-component version number (major.minor.patch.build). The fourth component is named "build" to represent incremental build numbers.

---

### Step 2: Create Namespaced Alias Target (Build Tree)
**Status:** ✅ Complete
**Implementation:** ✅ Implemented
**Testing:** ✅ Tested
**Dependencies:** Step 1
**Time estimate:** 15 minutes
**Files modified:** `src/CMakeLists.txt`
**Gitlab Issue:** #786+

Add after line 721 (after `set_target_properties(ROSE_DLL PROPERTIES OUTPUT_NAME "rose")`):

```cmake
# Create namespaced alias for modern CMake usage
add_library(Rose::rose ALIAS ROSE_DLL)
message(STATUS "Created alias target Rose::rose")
```

**Testing:**
Create a minimal test in `_build/test_alias/`:
```cmake
# test_alias/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(TestAlias)
add_subdirectory(${CMAKE_SOURCE_DIR}/.. rose EXCLUDE_FROM_ALL)
add_executable(test_alias main.cpp)
target_link_libraries(test_alias PRIVATE Rose::rose)
```

```cpp
// test_alias/main.cpp
#include <rose.h>
int main() { return 0; }
```

```bash
cd _build/test_alias
rmc cmake . && rmc cmake --build .
```

**Success criteria:** Test project builds successfully using `Rose::rose` target.

---

### Step 3: Add Proper Include Directory Propagation
**Status:** ✅ Complete
**Implementation:** ✅ Implemented
**Testing:** ✅ Tested
**Dependencies:** Step 2
**Time estimate:** 1 hour
**Files modified:** `src/CMakeLists.txt`
**Gitlab Issue:** #787+

Replace or augment the existing `include_directories()` calls with proper target-based includes. Add after the `add_library(ROSE_DLL ...)` line:

```cmake
# Propagate include directories to consumers
target_include_directories(ROSE_DLL PUBLIC
  # Build tree includes
  $<BUILD_INTERFACE:${ROSE_TOP_SRC_DIR}/src>
  $<BUILD_INTERFACE:${ROSE_TOP_SRC_DIR}/src/frontend/SageIII>
  $<BUILD_INTERFACE:${ROSE_TOP_SRC_DIR}/src/Rose>
  $<BUILD_INTERFACE:${ROSE_TOP_BINARY_DIR}>
  $<BUILD_INTERFACE:${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII>
  # Install tree includes
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/Rose>
)

# Set compile features
target_compile_features(ROSE_DLL PUBLIC cxx_std_14)
```

**Testing:**
Verify includes work without manual `-I` flags:
```bash
# In a test project, this should work without additional include_directories()
find_package(Rose REQUIRED)
target_link_libraries(myapp PRIVATE Rose::rose)
# myapp should compile with #include <rose.h>
```

**Success criteria:** External projects can use ROSE headers without manually specifying include directories.

---

### Step 4: Classify and Fix Dependency Linkage (PUBLIC/PRIVATE)
**Status:** ✅ Complete
**Implementation:** ✅ Implemented
**Testing:** 🔶 Partial
**Dependencies:** Step 3
**Time estimate:** 2-3 hours
**Files modified:** `src/CMakeLists.txt`
**GitLab Issue:** #788+

Review each `target_link_libraries(ROSE_DLL ...)` call and classify as PUBLIC or PRIVATE.

**Analysis guide:**
- **PUBLIC**: Types/symbols from this library appear in ROSE public headers
  - Boost (many ROSE headers use Boost types)
  - Likely: Sawyer utilities
- **PRIVATE**: Only used internally
  - Most other libraries (Z3, Dwarf, Capstone, etc.)

Example changes around line 723:
```cmake
# Before:
target_link_libraries(ROSE_DLL ${link_with_libraries} ${rose_LIB_OBJS})

# After:
target_link_libraries(ROSE_DLL
  PUBLIC
    # Boost is in public API
    Boost::chrono
    Boost::date_time
    Boost::filesystem
    Boost::iostreams
    Boost::program_options
    Boost::random
    Boost::regex
    Boost::system
    Boost::thread
    Boost::wave
    Boost::serialization
  PRIVATE
    ${rose_LIB_OBJS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)

# Link optional dependencies as PRIVATE (lines 756-813)
if(Z3_FOUND_LIB)
  target_link_libraries(ROSE_DLL PRIVATE ${Z3_LIBRARY})
endif()
# ... etc for other dependencies
```

**Testing:**
1. Build ROSE completely: `rmc -C _build cmake --build . --target ROSE_DLL`
2. Check exported symbols don't expose unnecessary dependencies:
```bash
nm -D lib/librose.so | grep -i "boost\|z3\|dwarf" | wc -l
# Boost should appear, others should be minimal
```

**Success criteria:**
- ROSE builds successfully
- Only necessary dependencies are exposed to consumers
- Link time for test projects is reasonable

---

### Step 5: Update Installation with EXPORT
**Status:** ✅ Complete
**Implementation:** ✅ Implemented
**Testing:** ✅ Tested
**Dependencies:** Step 4
**Time estimate:** 30 minutes
**Files modified:** `src/CMakeLists.txt`
**Gitlab Issue:** #789+

Modify the `install(TARGETS ROSE_DLL ...)` command around line 730:

```cmake
# Install the ROSE library with export information
install(TARGETS ROSE_DLL
  EXPORT RoseTargets
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
)

# Install the export set
install(EXPORT RoseTargets
  FILE RoseTargets.cmake
  NAMESPACE Rose::
  DESTINATION ${LIB_INSTALL_DIR}/cmake/Rose
)
```

**Testing:**
```bash
rmc cmake --build . --target install
ls ${CMAKE_INSTALL_PREFIX}/lib/cmake/Rose/
# Should see RoseTargets.cmake and RoseTargets-*.cmake
cat ${CMAKE_INSTALL_PREFIX}/lib/cmake/Rose/RoseTargets.cmake
# Should contain Rose::rose target definition
```

**Success criteria:** `RoseTargets.cmake` is installed and contains proper target definitions.

---

### Step 6: Create RoseConfigVersion.cmake
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Step 1
**Time estimate:** 45 minutes
**Files created:** `cmake/RoseConfigVersion.cmake.in`
**Files modified:** `CMakeLists.txt`

Create the version file template (see detailed template in Section 1.3 above).

Add to `CMakeLists.txt` after the version parsing (Step 1):

```cmake
# Generate version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/RoseConfigVersion.cmake
  VERSION ${ROSE_PACKAGE_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/RoseConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/cmake/Rose
)
```

**Testing:**
```bash
rmc cmake --build . --target install
cat ${CMAKE_INSTALL_PREFIX}/lib/cmake/Rose/RoseConfigVersion.cmake
# Should contain version checking logic

# Test version finding
cd /tmp/test_version
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.15)
project(VersionTest)
find_package(Rose 0.11 REQUIRED)
message(STATUS "Found Rose version: ${Rose_VERSION}")
EOF
rmc cmake . -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
# Should succeed and print version
```

**Success criteria:**
- Version file is installed
- Correct version queries succeed
- Incorrect version queries fail appropriately

---

### Step 7: Create Minimal RoseConfig.cmake (Core Dependencies Only)
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Steps 5, 6
**Time estimate:** 1-2 hours
**Files created:** `cmake/RoseConfig.cmake.in`
**Files modified:** `CMakeLists.txt`

Create a minimal version handling only Boost (the one truly required dependency):

```cmake
# cmake/RoseConfig.cmake.in (minimal version)
@PACKAGE_INIT@

set(Rose_VERSION "@ROSE_PACKAGE_VERSION@")

include(CMakeFindDependencyMacro)

# Find Boost (always required)
set(Boost_USE_STATIC_LIBS @Boost_USE_STATIC_LIBS@)
find_dependency(Boost @Boost_VERSION@ REQUIRED COMPONENTS
  chrono date_time filesystem iostreams program_options
  random regex system thread wave serialization)

# Import targets
if(NOT TARGET Rose::rose)
  include("${CMAKE_CURRENT_LIST_DIR}/RoseTargets.cmake")
endif()

check_required_components(Rose)
```

Add to `CMakeLists.txt`:
```cmake
# Configure package config file
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RoseConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/RoseConfig.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/Rose
  PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/RoseConfig.cmake
  DESTINATION ${LIB_INSTALL_DIR}/cmake/Rose
)
```

**Testing:**
```bash
rmc cmake --build . --target install

# Create test project
cd /tmp/rose_test
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.15)
project(RoseTest)
find_package(Rose REQUIRED)
add_executable(test_rose test.cpp)
target_link_libraries(test_rose PRIVATE Rose::rose)
EOF

cat > test.cpp << 'EOF'
#include <rose.h>
int main() {
    return 0;
}
EOF

rmc cmake . -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
rmc cmake --build .
./test_rose
```

**Success criteria:**
- External CMake project finds ROSE successfully
- Compiles and links without errors
- No manual include or link directories needed

---

### Step 8: Expand RoseConfig.cmake with Optional Dependencies
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Step 7
**Time estimate:** 2 hours
**Files modified:** `cmake/RoseConfig.cmake.in`

Expand the config file to include all optional dependencies (see complete template in Section 1.2).

Add conditional `find_dependency()` calls for each optional library.

**Testing:**
```bash
# Test with various ROSE configurations
rmc cmake .. -DENABLE-BINARY-ANALYSIS=ON  # Should find Dwarf, Elf, etc.
rmc cmake --build . --target install

# Test that dependencies are found correctly
cd /tmp/rose_test
rmc cmake . -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
# Should not fail on missing optional dependencies
```

**Success criteria:**
- Configurations with optional dependencies work correctly
- Missing optional dependencies don't cause failures for consumers
- Appropriate error messages if required dependency is missing

---

### Step 9: Add Feature Detection Variables
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Step 8
**Time estimate:** 30 minutes
**Files modified:** `cmake/RoseConfig.cmake.in`

Add the feature flag exports (lines 64-72 in the template).

**Testing:**
```cmake
# In consumer project
find_package(Rose REQUIRED)
if(Rose_ENABLE_BINARY_ANALYSIS)
  message(STATUS "Can use binary analysis features")
endif()
```

**Success criteria:** Feature flags accurately reflect build configuration.

---

### Step 10: Create pkg-config Support
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Step 7 (can be parallel)
**Time estimate:** 1-2 hours
**Files created:** `cmake/rose.pc.in`
**Files modified:** `CMakeLists.txt`

Create complete pkg-config file:
```
# cmake/rose.pc.in
prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${prefix}/@LIB_INSTALL_DIR@
includedir=${prefix}/@INCLUDE_INSTALL_DIR@

Name: ROSE
Description: ROSE Compiler Infrastructure for Building Source-to-Source Translators
Version: @ROSE_PACKAGE_VERSION@
URL: http://www.rosecompiler.org

Requires: @ROSE_PC_REQUIRES@
Libs: -L${libdir} -lrose @ROSE_PC_LIBS@
Cflags: -I${includedir} @ROSE_PC_CFLAGS@
```

Generate dependency lists in `CMakeLists.txt`:
```cmake
# Build pkg-config Requires and Libs
set(ROSE_PC_REQUIRES "")
set(ROSE_PC_LIBS "")
set(ROSE_PC_CFLAGS "-std=c++14")

# Add Boost components
foreach(comp chrono date_time filesystem iostreams program_options random regex system thread wave serialization)
  list(APPEND ROSE_PC_REQUIRES "boost_${comp}")
endforeach()
string(REPLACE ";" " " ROSE_PC_REQUIRES "${ROSE_PC_REQUIRES}")

# Configure and install
configure_file(cmake/rose.pc.in ${CMAKE_BINARY_DIR}/rose.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/rose.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
```

**Testing:**
```bash
export PKG_CONFIG_PATH=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
pkg-config --modversion rose
pkg-config --cflags rose
pkg-config --libs rose

# Test in Makefile project
cd /tmp/makefile_test
cat > Makefile << 'EOF'
CXXFLAGS := $(shell pkg-config --cflags rose)
LDFLAGS := $(shell pkg-config --libs rose)
test: test.cpp
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@
EOF
make
```

**Success criteria:** pkg-config integration works for non-CMake build systems.

---

### Step 11: Create Integration Tests
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Steps 7, 10
**Time estimate:** 2-3 hours
**Files created:** `tests/cmake-integration/*`

Create test suite:
```
tests/cmake-integration/
├── 01-basic-cmake/          # Basic find_package test
├── 02-custom-prefix/        # Non-standard install location
├── 03-version-check/        # Version compatibility tests
├── 04-pkgconfig/            # pkg-config integration
├── 05-autotools/            # Autotools integration (optional)
└── run-all-tests.sh         # Test runner script
```

**Success criteria:** All integration tests pass.

---

### Step 12: Documentation and Examples
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Steps 7, 10
**Time estimate:** 2-3 hours
**Files created:**
- `docs/cmake-integration.md`
- `examples/external-project-cmake/`
- `examples/external-project-autotools/`
- `examples/external-project-makefile/`

**Success criteria:** Documentation is clear and examples work out of the box.

---

### Step 13: Deprecate Old FindRose.cmake
**Status:** ❌ Not Started
**Implementation:** ⬜ Not Implemented
**Testing:** ⬜ Not Tested
**Dependencies:** Step 11 (after testing confirms new system works)
**Time estimate:** 30 minutes
**Files modified:** `cmake/FindRose.cmake`

Add deprecation warning:
```cmake
macro(find_rose)
  message(DEPRECATION
    "FindRose.cmake is deprecated. Please use find_package(Rose) with "
    "Config mode instead. See docs/cmake-integration.md for migration guide.")
  # ... rest of existing code
endmacro()
```

**Success criteria:** Users are informed but existing code continues to work.

## Testing Strategy

Create test projects in `tests/cmake-integration/`:
- Basic consumption test
- With custom install prefix
- From build directory vs install directory
- Cross-compilation scenarios
- Multiple ROSE versions side-by-side

## Backward Compatibility

- Keep existing `FindRose.cmake` but deprecate it
- Maintain `rose-config` script
- Document migration path for existing users
- Support both old and new methods for 1-2 releases

## Conclusion

This modernization will bring ROSE's CMake infrastructure up to current best practices while maintaining compatibility with non-CMake build systems. The changes will make ROSE significantly easier to integrate into external projects while preserving backward compatibility for existing users.
