// This file is needed on macOS when building a static library because the tool chain there forbids empty static libraries.
namespace Rose {
void unusedFunctionWhoseOnlyPurposeIsToWorkAroundBrokenBuildSystems() {}
}
