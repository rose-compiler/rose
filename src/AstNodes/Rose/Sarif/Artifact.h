#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <string>

namespace Rose {
namespace Sarif {

/** Represents an analysis artifact such as a file.
 *
 *  An artifact is referred to by a [URI](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier), and has additional optional
 *  properties such as @ref description, @ref mimeType "MIME type", @ref sourceLanguage "source language", and @ref hash.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_artifact */
class Artifact: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: URI.
     *
     *  The location of this artifact. The URI is set when the artifact is constructed. */
    [[Rosebud::ctor_arg]]
    std::string uri;

    /** Property: Description.
     *
     *  Optional multi-line description of the artifact. */
    [[Rosebud::property]]
    std::string description;

    /** Property: MIME type.
     *
     *  The [MIME type](https://www.rfc-editor.org/info/rfc2045) for this property. */
    [[Rosebud::property]]
    std::string mimeType;

    /** Property: Source language.
     *
     *  If this artifact is source code, then this property should indicate the language. The source language should be lower
     *  case, special symbols should be spelled out, and abbreviations expanded. A language variant can be specified after a
     *  slash character. Examples "c" (C), "cplusplus" (C++), "csharp" (C#), "cplusplus/17" (C++17), "visualbasic" (vb). */
    [[Rosebud::property]]
    std::string sourceLanguage;

    /** Property: Hash.
     *
     *  Specifies a hash of the contents. The hash consists of a hash name such as "sha256" and a string hash value.
     *
     * @{ */
    [[Rosebud::large]]
    std::pair<std::string, std::string> hash;

    void hash(const std::string &name, const std::string &value);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
