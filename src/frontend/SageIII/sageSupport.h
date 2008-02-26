// DQ (8/19/2004): Moved from ROSE/src/midend/astRewriteMechanism/rewrite.h
// Added global function for getting the string associated 
// with an enum (which is defined in global scope)
// string getVariantName ( VariantT v );

// DQ (7/7/2005): Temp location while I implement this function
int buildAstMergeCommandFile ( SgProject* project );

int AstMergeSupport ( SgProject* project );

//! Find the path of a ROSE support file.  If the environment variable
//! ROSE_IN_BUILD_TREE is set, the top of the source tree plus
//! sourceTreeLocation is used as the location.  If the variable is not set,
//! the path in installTreeLocation (with no prefix added) is used instead.
std::string
findRoseSupportPathFromSource(const std::string& sourceTreeLocation,
                              const std::string& installTreeLocation);

//! Find the path of a ROSE support file.  If the environment variable
//! ROSE_IN_BUILD_TREE is set, that path plus buildTreeLocation is used as
//! the location.  If the variable is not set, the path in
//! installTreeLocation (with no prefix added) is used instead.
std::string
findRoseSupportPathFromBuild(const std::string& buildTreeLocation,
                             const std::string& installTreeLocation);

