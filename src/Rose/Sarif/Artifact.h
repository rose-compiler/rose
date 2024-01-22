#ifndef ROSE_Sarif_Artifact_H
#define ROSE_Sarif_Artifact_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

namespace Rose {
namespace Sarif {

/** Represents an analysis artifact such as a file.
 *
 *  An artifact is referred to by a [URI](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier), and has additional optional
 *  properties such as @ref description, @ref mimeType "MIME type", @ref sourceLanguage "source language", and @ref hash.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C artifact_example */
class Artifact: public Node {
public:
    /** Shared ownership pointer to a @ref Artifact object.
     *
     *  @{ */
    using Ptr = ArtifactPtr;
    using ConstPtr = ArtifactConstPtr;
    /** @} */

private:
    std::string uri_;                                   // location of this artifact
    std::string description_;                           // optional multi-line description
    std::string mimeType_;                              // see RFC2045 https://www.rfc-editor.org/info/rfc2045
    std::string sourceLanguage_;                        // optional source language if artifact is source code
    std::pair<std::string, std::string> hash_;          // hash algorithm name and value for artifact contents

public:
    ~Artifact();
protected:
    explicit Artifact(const std::string &uri);
public:
    /** Allocating constructor. */
    static Ptr instance(const std::string &uri);

    /** Property: URI.
     *
     *  The URI is set when the artifact is constructed. */
    const std::string& uri() const;

    /** Property: MIME type.
     *
     *  The [MIME type](https://www.rfc-editor.org/info/rfc2045) for this property.
     *
     *  @{ */
    const std::string& mimeType() const;
    void mimeType(const std::string&);
    /** @} */

    /** Property: Source language.
     *
     *  If this artifact is source code, then this property should indicate the language. The source language should be lower
     *  case, special symbols should be spelled out, and abbreviations expanded. A language variant can be specified after a
     *  slash character. Examples "c" (C), "cplusplus" (C++), "csharp" (C#), "cplusplus/17" (C++17), "visualbasic" (vb).
     *
     * @{ */
    const std::string& sourceLanguage() const;
    void sourceLanguage(const std::string&);
    /** @} */

    /** Property: Hash.
     *
     *  Specifies a hash of the contents. The hash consists of a hash name such as "sha256" and a string hash value.
     *
     *  @{ */
    const std::pair<std::string /*name*/, std::string /*value*/>& hash() const;
    void hash(const std::pair<std::string /*name*/, std::string /*value*/>&);
    void hash(const std::string &name, const std::string &value);
    /** @} */

    /** Property: Description.
     *
     *  A multi-line description of the artifact.
     *
     * @{ */
    const std::string& description() const;
    void description(const std::string&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string&) override;
    std::string emissionPrefix() override;

private:
    void emitUri(std::ostream&, const std::string&);
    void emitMimeType(std::ostream&, const std::string&);
    void emitSourceLanguage(std::ostream&, const std::string&);
    void emitHash(std::ostream&, const std::string&);
    void emitDescription(std::ostream&, const std::string&);
};

} // namespace
} // namespace

#endif
#endif
