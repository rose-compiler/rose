#ifndef ROSE_MAPPING
#define ROSE_MAPPING 1

// minimal header for mapping rose onto the class hierarchy analysis

#include <sage3basic.h>
#include <VariableIdMapping.h>

#include <string>
#include <functional>

namespace CodeThorn
{

struct RoseCompatibilityBridge
{
    explicit
    RoseCompatibilityBridge(CodeThorn::VariableIdMapping& vIdMap)
    : vIdMapping(vIdMap)
    {}

    ~RoseCompatibilityBridge() = default;

    CodeThorn::VariableIdMapping& vmap() { return vIdMapping; }

  private:
    RoseCompatibilityBridge()                                    = delete;
    RoseCompatibilityBridge(RoseCompatibilityBridge&&)                 = delete;
    RoseCompatibilityBridge& operator=(RoseCompatibilityBridge&&)      = delete;
    RoseCompatibilityBridge(const RoseCompatibilityBridge&)            = delete;
    RoseCompatibilityBridge& operator=(const RoseCompatibilityBridge&) = delete;

    CodeThorn::VariableIdMapping& vIdMapping;
};


using ClassKeyType = const SgClassDefinition*;
using TypeKeyType  = const SgType*;
using CastKeyType  = const SgCastExp*;
using ProjectType  = SgProject*;

struct ClassAnalysis;
struct CastAnalysis;

struct FileInfo
{
  FileInfo(CastKeyType n)
  : node(n)
  {}

  CastKeyType node;
};

std::ostream& operator<<(std::ostream& os, const FileInfo& fi);

std::vector<CodeThorn::VariableId>
getDataMembers(RoseCompatibilityBridge&, const ClassKeyType);

std::string typeNameOf(ClassKeyType key);

void inheritanceEdges( ClassKeyType,
                       std::function<void(ClassKeyType, ClassKeyType, bool, bool)>
                     );

void extractFromProject(ClassAnalysis&, CastAnalysis&, ProjectType);

bool classHasVirtualMethods(ClassKeyType);

std::pair<ClassKeyType, TypeKeyType> getClassCastInfo(TypeKeyType);

std::string unparseToString(CastKeyType);
int typeOf(CastKeyType);

}
#endif /* ROSE_MAPPING */
