//  Automatically generated code for the schema defined by:
//
//  https://docs.oasis-open.org/sarif/sarif/v2.1.0/cos02/schemas/sarif-schema-2.1.0.json
//
//  QuickType.io was used for the code generation
//
//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Coordinate data = nlohmann::json::parse(jsonString);

#pragma once

#include "nlohmann/json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>
#include <unordered_map>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, const std::shared_ptr<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json & j) {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace SARIF {
    using nlohmann::json;

    const std::string VERSION = "2.1.0";
    const std::string SCHEMA = "https://docs.oasis-open.org/sarif/sarif/v2.1.0/cos02/schemas/sarif-schema-2.1.0.json";

    class ClassMemberConstraints {
        private:
        boost::optional<int> min_value;
        boost::optional<int> max_value;
        boost::optional<size_t> min_length;
        boost::optional<size_t> max_length;
        boost::optional<std::string> pattern;

        public:
        ClassMemberConstraints(
            boost::optional<int> min_value,
            boost::optional<int> max_value,
            boost::optional<size_t> min_length,
            boost::optional<size_t> max_length,
            boost::optional<std::string> pattern
        ) : min_value(min_value), max_value(max_value), min_length(min_length), max_length(max_length), pattern(pattern) {}
        ClassMemberConstraints() = default;
        virtual ~ClassMemberConstraints() = default;

        void set_min_value(int min_value) { this->min_value = min_value; }
        auto get_min_value() const { return min_value; }

        void set_max_value(int max_value) { this->max_value = max_value; }
        auto get_max_value() const { return max_value; }

        void set_min_length(size_t min_length) { this->min_length = min_length; }
        auto get_min_length() const { return min_length; }

        void set_max_length(size_t max_length) { this->max_length = max_length; }
        auto get_max_length() const { return max_length; }

        void set_pattern(const std::string &  pattern) { this->pattern = pattern; }
        auto get_pattern() const { return pattern; }
    };

    class ClassMemberConstraintException : public std::runtime_error {
        public:
        ClassMemberConstraintException(const std::string &  msg) : std::runtime_error(msg) {}
    };

    class ValueTooLowException : public ClassMemberConstraintException {
        public:
        ValueTooLowException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooHighException : public ClassMemberConstraintException {
        public:
        ValueTooHighException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooShortException : public ClassMemberConstraintException {
        public:
        ValueTooShortException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooLongException : public ClassMemberConstraintException {
        public:
        ValueTooLongException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class InvalidPatternException : public ClassMemberConstraintException {
        public:
        InvalidPatternException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, int64_t value) {
        if (c.get_min_value() != boost::none && value < *c.get_min_value()) {
            throw ValueTooLowException ("Value too low for " + name + " (" + std::to_string(value) + "<" + std::to_string(*c.get_min_value()) + ")");
        }

        if (c.get_max_value() != boost::none && value > *c.get_max_value()) {
            throw ValueTooHighException ("Value too high for " + name + " (" + std::to_string(value) + ">" + std::to_string(*c.get_max_value()) + ")");
        }
    }

    void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, const std::string &  value) {
        if (c.get_min_length() != boost::none && value.length() < *c.get_min_length()) {
            throw ValueTooShortException ("Value too short for " + name + " (" + std::to_string(value.length()) + "<" + std::to_string(*c.get_min_length()) + ")");
        }

        if (c.get_max_length() != boost::none && value.length() > *c.get_max_length()) {
            throw ValueTooLongException ("Value too long for " + name + " (" + std::to_string(value.length()) + ">" + std::to_string(*c.get_max_length()) + ")");
        }

        if (c.get_pattern() != boost::none) {
            std::smatch result;
            std::regex_search(value, result, std::regex( *c.get_pattern() ));
            if (result.empty()) {
                throw InvalidPatternException ("Value doesn't match pattern for " + name + " (" + value +" != " + *c.get_pattern() + ")");
            }
        }
    }

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json & j, std::string property) {
        return get_optional<T>(j, property.data());
    }

    /**
     * Key/value pairs that provide additional information about the address.
     *
     * Key/value pairs that provide additional information about the object.
     *
     * Key/value pairs that provide additional information about the artifact content.
     *
     * Key/value pairs that provide additional information about the message.
     *
     * Key/value pairs that provide additional information about the artifact location.
     *
     * Key/value pairs that provide additional information about the artifact.
     *
     * Contains configuration information specific to a report.
     *
     * Key/value pairs that provide additional information about the reporting configuration.
     *
     * Key/value pairs that provide additional information about the reporting descriptor
     * reference.
     *
     * Key/value pairs that provide additional information about the toolComponentReference.
     *
     * Key/value pairs that provide additional information about the configuration override.
     *
     * Key/value pairs that provide additional information about the invocation.
     *
     * Key/value pairs that provide additional information about the exception.
     *
     * Key/value pairs that provide additional information about the region.
     *
     * Key/value pairs that provide additional information about the logical location.
     *
     * Key/value pairs that provide additional information about the physical location.
     *
     * Key/value pairs that provide additional information about the location.
     *
     * Key/value pairs that provide additional information about the location relationship.
     *
     * Key/value pairs that provide additional information about the stack frame.
     *
     * Key/value pairs that provide additional information about the stack.
     *
     * Key/value pairs that provide additional information about the notification.
     *
     * Key/value pairs that provide additional information about the conversion.
     *
     * Key/value pairs that provide additional information about the report.
     *
     * Key/value pairs that provide additional information about the tool component.
     *
     * Key/value pairs that provide additional information about the translation metadata.
     *
     * Key/value pairs that provide additional information about the tool.
     *
     * Key/value pairs that provide additional information that will be merged with a separate
     * run.
     *
     * Key/value pairs that provide additional information about the edge.
     *
     * Key/value pairs that provide additional information about the node.
     *
     * Key/value pairs that provide additional information about the graph.
     *
     * Key/value pairs that provide additional information about the external properties.
     *
     * Key/value pairs that provide additional information about the attachment.
     *
     * Key/value pairs that provide additional information about the rectangle.
     *
     * Key/value pairs that provide additional information about the code flow.
     *
     * Key/value pairs that provide additional information about the threadflow location.
     *
     * Key/value pairs that provide additional information about the request.
     *
     * Key/value pairs that provide additional information about the response.
     *
     * Key/value pairs that provide additional information about the thread flow.
     *
     * Key/value pairs that provide additional information about the change.
     *
     * Key/value pairs that provide additional information about the replacement.
     *
     * Key/value pairs that provide additional information about the fix.
     *
     * Key/value pairs that provide additional information about the edge traversal.
     *
     * Key/value pairs that provide additional information about the graph traversal.
     *
     * Key/value pairs that provide additional information about the result.
     *
     * Key/value pairs that provide additional information about the suppression.
     *
     * Key/value pairs that provide additional information about the log file.
     *
     * Key/value pairs that provide additional information about the run automation details.
     *
     * Key/value pairs that provide additional information about the external property file.
     *
     * Key/value pairs that provide additional information about the external property files.
     *
     * Key/value pairs that provide additional information about the run.
     *
     * Key/value pairs that provide additional information about the special locations.
     *
     * Key/value pairs that provide additional information about the version control details.
     */
    class PropertyBag {
        public:
        PropertyBag() = default;
        virtual ~PropertyBag() = default;

        private:
        std::shared_ptr<std::vector<std::string>> tags;

        public:
        /**
         * A set of distinct strings that provide additional information.
         */
        std::shared_ptr<std::vector<std::string>> get_tags() const { return tags; }
        void set_tags(std::shared_ptr<std::vector<std::string>> value) { this->tags = value; }
    };

    /**
     * A physical or virtual address, or a range of addresses, in an 'addressable region'
     * (memory or a binary file).
     *
     * The address of the location.
     */
    class Address {
        public:
        Address() :
            absolute_address_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            parent_index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~Address() = default;

        private:
        std::shared_ptr<int64_t> absolute_address;
        ClassMemberConstraints absolute_address_constraint;
        std::shared_ptr<std::string> fully_qualified_name;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<std::string> kind;
        std::shared_ptr<int64_t> length;
        std::shared_ptr<std::string> name;
        std::shared_ptr<int64_t> offset_from_parent;
        std::shared_ptr<int64_t> parent_index;
        ClassMemberConstraints parent_index_constraint;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<int64_t> relative_address;

        public:
        /**
         * The address expressed as a byte offset from the start of the addressable region.
         */
        std::shared_ptr<int64_t> get_absolute_address() const { return absolute_address; }
        void set_absolute_address(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("absolute_address", absolute_address_constraint, *value); this->absolute_address = value; }

        /**
         * A human-readable fully qualified name that is associated with the address.
         */
        std::shared_ptr<std::string> get_fully_qualified_name() const { return fully_qualified_name; }
        void set_fully_qualified_name(std::shared_ptr<std::string> value) { this->fully_qualified_name = value; }

        /**
         * The index within run.addresses of the cached object for this address.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * An open-ended string that identifies the address kind. 'data', 'function',
         * 'header','instruction', 'module', 'page', 'section', 'segment', 'stack', 'stackFrame',
         * 'table' are well-known values.
         */
        std::shared_ptr<std::string> get_kind() const { return kind; }
        void set_kind(std::shared_ptr<std::string> value) { this->kind = value; }

        /**
         * The number of bytes in this range of addresses.
         */
        std::shared_ptr<int64_t> get_length() const { return length; }
        void set_length(std::shared_ptr<int64_t> value) { this->length = value; }

        /**
         * A name that is associated with the address, e.g., '.text'.
         */
        std::shared_ptr<std::string> get_name() const { return name; }
        void set_name(std::shared_ptr<std::string> value) { this->name = value; }

        /**
         * The byte offset of this address from the absolute or relative address of the parent
         * object.
         */
        std::shared_ptr<int64_t> get_offset_from_parent() const { return offset_from_parent; }
        void set_offset_from_parent(std::shared_ptr<int64_t> value) { this->offset_from_parent = value; }

        /**
         * The index within run.addresses of the parent object.
         */
        std::shared_ptr<int64_t> get_parent_index() const { return parent_index; }
        void set_parent_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("parent_index", parent_index_constraint, *value); this->parent_index = value; }

        /**
         * Key/value pairs that provide additional information about the address.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The address expressed as a byte offset from the absolute address of the top-most parent
         * object.
         */
        std::shared_ptr<int64_t> get_relative_address() const { return relative_address; }
        void set_relative_address(std::shared_ptr<int64_t> value) { this->relative_address = value; }
    };

    /**
     * An alternate rendered representation of the artifact (e.g., a decompiled representation
     * of a binary region).
     *
     * A message string or message format string rendered in multiple formats.
     *
     * A comprehensive description of the tool component.
     *
     * A description of the report. Should, as far as possible, provide details sufficient to
     * enable resolution of any problem indicated by the result.
     *
     * Provides the primary documentation for the report, useful when there is no online
     * documentation.
     *
     * A concise description of the report. Should be a single sentence that is understandable
     * when visible space is limited to a single line of text.
     *
     * A brief description of the tool component.
     *
     * A comprehensive description of the translation metadata.
     *
     * A brief description of the translation metadata.
     */
    class MultiformatMessageString {
        public:
        MultiformatMessageString() = default;
        virtual ~MultiformatMessageString() = default;

        private:
        std::shared_ptr<std::string> markdown;
        std::shared_ptr<PropertyBag> properties;
        std::string text;

        public:
        /**
         * A Markdown message string or format string.
         */
        std::shared_ptr<std::string> get_markdown() const { return markdown; }
        void set_markdown(std::shared_ptr<std::string> value) { this->markdown = value; }

        /**
         * Key/value pairs that provide additional information about the message.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A plain text message string or format string.
         */
        const std::string & get_text() const { return text; }
        std::string & get_mutable_text() { return text; }
        void set_text(const std::string & value) { this->text = value; }
    };

    /**
     * The contents of the artifact.
     *
     * Represents the contents of an artifact.
     *
     * The portion of the artifact contents within the specified region.
     *
     * The body of the request.
     *
     * The body of the response.
     *
     * The content to insert at the location specified by the 'deletedRegion' property.
     */
    class ArtifactContent {
        public:
        ArtifactContent() = default;
        virtual ~ArtifactContent() = default;

        private:
        std::shared_ptr<std::string> binary;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<MultiformatMessageString> rendered;
        std::shared_ptr<std::string> text;

        public:
        /**
         * MIME Base64-encoded content from a binary artifact, or from a text artifact in its
         * original encoding.
         */
        std::shared_ptr<std::string> get_binary() const { return binary; }
        void set_binary(std::shared_ptr<std::string> value) { this->binary = value; }

        /**
         * Key/value pairs that provide additional information about the artifact content.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An alternate rendered representation of the artifact (e.g., a decompiled representation
         * of a binary region).
         */
        std::shared_ptr<MultiformatMessageString> get_rendered() const { return rendered; }
        void set_rendered(std::shared_ptr<MultiformatMessageString> value) { this->rendered = value; }

        /**
         * UTF-8-encoded content from a text artifact.
         */
        std::shared_ptr<std::string> get_text() const { return text; }
        void set_text(std::shared_ptr<std::string> value) { this->text = value; }
    };

    /**
     * A short description of the artifact.
     *
     * A short description of the artifact location.
     *
     * A message relevant to the region.
     *
     * A message relevant to the location.
     *
     * A description of the location relationship.
     *
     * A message relevant to this call stack.
     *
     * A message that describes the condition that was encountered.
     *
     * A description of the reporting descriptor relationship.
     *
     * A description of the graph.
     *
     * A short description of the edge.
     *
     * A short description of the node.
     *
     * A message describing the role played by the attachment.
     *
     * A message relevant to the rectangle.
     *
     * A message relevant to the code flow.
     *
     * A message relevant to the thread flow.
     *
     * A message that describes the proposed fix, enabling viewers to present the proposed
     * change to an end user.
     *
     * A description of this graph traversal.
     *
     * A message to display to the user as the edge is traversed.
     *
     * A message that describes the result. The first sentence of the message only will be
     * displayed when visible space is limited.
     *
     * A description of the identity and role played within the engineering system by this
     * object's containing run object.
     *
     * Encapsulates a message intended to be read by the end user.
     */
    class Message {
        public:
        Message() = default;
        virtual ~Message() = default;

        private:
        std::shared_ptr<std::vector<std::string>> arguments;
        std::shared_ptr<std::string> id;
        std::shared_ptr<std::string> markdown;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::string> text;

        public:
        /**
         * An array of strings to substitute into the message string.
         */
        std::shared_ptr<std::vector<std::string>> get_arguments() const { return arguments; }
        void set_arguments(std::shared_ptr<std::vector<std::string>> value) { this->arguments = value; }

        /**
         * The identifier for this message.
         */
        std::shared_ptr<std::string> get_id() const { return id; }
        void set_id(std::shared_ptr<std::string> value) { this->id = value; }

        /**
         * A Markdown message string.
         */
        std::shared_ptr<std::string> get_markdown() const { return markdown; }
        void set_markdown(std::shared_ptr<std::string> value) { this->markdown = value; }

        /**
         * Key/value pairs that provide additional information about the message.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A plain text message string.
         */
        std::shared_ptr<std::string> get_text() const { return text; }
        void set_text(std::shared_ptr<std::string> value) { this->text = value; }
    };

    /**
     * The location of the artifact.
     *
     * Specifies the location of an artifact.
     *
     * An absolute URI specifying the location of the executable that was invoked.
     *
     * A file containing the standard error stream from the process that was invoked.
     *
     * A file containing the standard input stream to the process that was invoked.
     *
     * A file containing the standard output stream from the process that was invoked.
     *
     * A file containing the interleaved standard output and standard error stream from the
     * process that was invoked.
     *
     * The working directory for the invocation.
     *
     * Identifies the artifact that the analysis tool was instructed to scan. This need not be
     * the same as the artifact where the result actually occurred.
     *
     * The location of the attachment.
     *
     * The location of the artifact to change.
     *
     * The location of the external property file.
     *
     * Provides a suggestion to SARIF consumers to display file paths relative to the specified
     * location.
     *
     * The location in the local file system to which the root of the repository was mapped at
     * the time of the analysis.
     */
    class ArtifactLocation {
        public:
        ArtifactLocation() :
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~ArtifactLocation() = default;

        private:
        std::shared_ptr<Message> description;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::string> uri;
        std::shared_ptr<std::string> uri_base_id;

        public:
        /**
         * A short description of the artifact location.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * The index within the run artifacts array of the artifact object associated with the
         * artifact location.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * Key/value pairs that provide additional information about the artifact location.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A string containing a valid relative or absolute URI.
         */
        std::shared_ptr<std::string> get_uri() const { return uri; }
        void set_uri(std::shared_ptr<std::string> value) { this->uri = value; }

        /**
         * A string which indirectly specifies the absolute URI with respect to which a relative URI
         * in the "uri" property is interpreted.
         */
        std::shared_ptr<std::string> get_uri_base_id() const { return uri_base_id; }
        void set_uri_base_id(std::shared_ptr<std::string> value) { this->uri_base_id = value; }
    };

    enum class Role : int { ADDED, ANALYSIS_TARGET, ATTACHMENT, DEBUG_OUTPUT_FILE, DELETED, DIRECTORY, DRIVER, EXTENSION, MEMORY_CONTENTS, MODIFIED, POLICY, REFERENCED_ON_COMMAND_LINE, RENAMED, RESPONSE_FILE, RESULT_FILE, STANDARD_STREAM, TAXONOMY, TOOL_SPECIFIED_CONFIGURATION, TRACED_FILE, TRANSLATION, UNCONTROLLED, UNMODIFIED, USER_SPECIFIED_CONFIGURATION };

    /**
     * A single artifact. In some cases, this artifact might be nested within another artifact.
     */
    class Artifact {
        public:
        Artifact() :
            length_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            mime_type_constraint(boost::none, boost::none, boost::none, boost::none, std::string("[^/]+/.+")),
            offset_constraint(0, boost::none, boost::none, boost::none, boost::none),
            parent_index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~Artifact() = default;

        private:
        std::shared_ptr<ArtifactContent> contents;
        std::shared_ptr<Message> description;
        std::shared_ptr<std::string> encoding;
        std::shared_ptr<std::map<std::string, std::string>> hashes;
        std::shared_ptr<std::string> last_modified_time_utc;
        std::shared_ptr<int64_t> length;
        ClassMemberConstraints length_constraint;
        std::shared_ptr<ArtifactLocation> location;
        std::shared_ptr<std::string> mime_type;
        ClassMemberConstraints mime_type_constraint;
        std::shared_ptr<int64_t> offset;
        ClassMemberConstraints offset_constraint;
        std::shared_ptr<int64_t> parent_index;
        ClassMemberConstraints parent_index_constraint;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<Role>> roles;
        std::shared_ptr<std::string> source_language;

        public:
        /**
         * The contents of the artifact.
         */
        std::shared_ptr<ArtifactContent> get_contents() const { return contents; }
        void set_contents(std::shared_ptr<ArtifactContent> value) { this->contents = value; }

        /**
         * A short description of the artifact.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * Specifies the encoding for an artifact object that refers to a text file.
         */
        std::shared_ptr<std::string> get_encoding() const { return encoding; }
        void set_encoding(std::shared_ptr<std::string> value) { this->encoding = value; }

        /**
         * A dictionary, each of whose keys is the name of a hash function and each of whose values
         * is the hashed value of the artifact produced by the specified hash function.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_hashes() const { return hashes; }
        void set_hashes(std::shared_ptr<std::map<std::string, std::string>> value) { this->hashes = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which the artifact was most
         * recently modified. See "Date/time properties" in the SARIF spec for the required format.
         */
        std::shared_ptr<std::string> get_last_modified_time_utc() const { return last_modified_time_utc; }
        void set_last_modified_time_utc(std::shared_ptr<std::string> value) { this->last_modified_time_utc = value; }

        /**
         * The length of the artifact in bytes.
         */
        std::shared_ptr<int64_t> get_length() const { return length; }
        void set_length(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("length", length_constraint, *value); this->length = value; }

        /**
         * The location of the artifact.
         */
        std::shared_ptr<ArtifactLocation> get_location() const { return location; }
        void set_location(std::shared_ptr<ArtifactLocation> value) { this->location = value; }

        /**
         * The MIME type (RFC 2045) of the artifact.
         */
        std::shared_ptr<std::string> get_mime_type() const { return mime_type; }
        void set_mime_type(std::shared_ptr<std::string> value) { if (value) CheckConstraint("mime_type", mime_type_constraint, *value); this->mime_type = value; }

        /**
         * The offset in bytes of the artifact within its containing artifact.
         */
        std::shared_ptr<int64_t> get_offset() const { return offset; }
        void set_offset(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("offset", offset_constraint, *value); this->offset = value; }

        /**
         * Identifies the index of the immediate parent of the artifact, if this artifact is nested.
         */
        std::shared_ptr<int64_t> get_parent_index() const { return parent_index; }
        void set_parent_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("parent_index", parent_index_constraint, *value); this->parent_index = value; }

        /**
         * Key/value pairs that provide additional information about the artifact.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The role or roles played by the artifact in the analysis.
         */
        std::shared_ptr<std::vector<Role>> get_roles() const { return roles; }
        void set_roles(std::shared_ptr<std::vector<Role>> value) { this->roles = value; }

        /**
         * Specifies the source language for any artifact object that refers to a text file that
         * contains source code.
         */
        std::shared_ptr<std::string> get_source_language() const { return source_language; }
        void set_source_language(std::shared_ptr<std::string> value) { this->source_language = value; }
    };

    /**
     * Specifies the failure level for the report.
     *
     * A value specifying the severity level of the notification.
     *
     * A value specifying the severity level of the result.
     */
    enum class Level : int { ERROR, NONE, NOTE, WARNING };

    /**
     * Specifies how the rule or notification was configured during the scan.
     *
     * Information about a rule or notification that can be configured at runtime.
     *
     * Default reporting configuration information.
     */
    class ReportingConfiguration {
        public:
        ReportingConfiguration() :
            rank_constraint(-1, 100, boost::none, boost::none, boost::none)
        {}
        virtual ~ReportingConfiguration() = default;

        private:
        std::shared_ptr<bool> enabled;
        std::shared_ptr<Level> level;
        std::shared_ptr<PropertyBag> parameters;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<double> rank;
        ClassMemberConstraints rank_constraint;

        public:
        /**
         * Specifies whether the report may be produced during the scan.
         */
        std::shared_ptr<bool> get_enabled() const { return enabled; }
        void set_enabled(std::shared_ptr<bool> value) { this->enabled = value; }

        /**
         * Specifies the failure level for the report.
         */
        std::shared_ptr<Level> get_level() const { return level; }
        void set_level(std::shared_ptr<Level> value) { this->level = value; }

        /**
         * Contains configuration information specific to a report.
         */
        std::shared_ptr<PropertyBag> get_parameters() const { return parameters; }
        void set_parameters(std::shared_ptr<PropertyBag> value) { this->parameters = value; }

        /**
         * Key/value pairs that provide additional information about the reporting configuration.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * Specifies the relative priority of the report. Used for analysis output only.
         */
        std::shared_ptr<double> get_rank() const { return rank; }
        void set_rank(std::shared_ptr<double> value) { if (value) CheckConstraint("rank", rank_constraint, *value); this->rank = value; }
    };

    /**
     * A reference used to locate the toolComponent associated with the descriptor.
     *
     * Identifies a particular toolComponent object, either the driver or an extension.
     *
     * The component which is strongly associated with this component. For a translation, this
     * refers to the component which has been translated. For an extension, this is the driver
     * that provides the extension's plugin model.
     */
    class ToolComponentReference {
        public:
        ToolComponentReference() :
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~ToolComponentReference() = default;

        private:
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<std::string> name;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * The 'guid' property of the referenced toolComponent.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * An index into the referenced toolComponent in tool.extensions.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * The 'name' property of the referenced toolComponent.
         */
        std::shared_ptr<std::string> get_name() const { return name; }
        void set_name(std::shared_ptr<std::string> value) { this->name = value; }

        /**
         * Key/value pairs that provide additional information about the toolComponentReference.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A reference used to locate the descriptor whose configuration was overridden.
     *
     * A reference used to locate the rule descriptor associated with this notification.
     *
     * A reference used to locate the descriptor relevant to this notification.
     *
     * A reference to the related reporting descriptor.
     *
     * A reference used to locate the rule descriptor relevant to this result.
     *
     * Information about how to locate a relevant reporting descriptor.
     */
    class ReportingDescriptorReference {
        public:
        ReportingDescriptorReference() :
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~ReportingDescriptorReference() = default;

        private:
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<std::string> id;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<ToolComponentReference> tool_component;

        public:
        /**
         * A guid that uniquely identifies the descriptor.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * The id of the descriptor.
         */
        std::shared_ptr<std::string> get_id() const { return id; }
        void set_id(std::shared_ptr<std::string> value) { this->id = value; }

        /**
         * The index into an array of descriptors in toolComponent.ruleDescriptors,
         * toolComponent.notificationDescriptors, or toolComponent.taxonomyDescriptors, depending on
         * context.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * Key/value pairs that provide additional information about the reporting descriptor
         * reference.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A reference used to locate the toolComponent associated with the descriptor.
         */
        std::shared_ptr<ToolComponentReference> get_tool_component() const { return tool_component; }
        void set_tool_component(std::shared_ptr<ToolComponentReference> value) { this->tool_component = value; }
    };

    /**
     * Information about how a specific rule or notification was reconfigured at runtime.
     */
    class ConfigurationOverride {
        public:
        ConfigurationOverride() = default;
        virtual ~ConfigurationOverride() = default;

        private:
        ReportingConfiguration configuration;
        ReportingDescriptorReference descriptor;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * Specifies how the rule or notification was configured during the scan.
         */
        const ReportingConfiguration & get_configuration() const { return configuration; }
        ReportingConfiguration & get_mutable_configuration() { return configuration; }
        void set_configuration(const ReportingConfiguration & value) { this->configuration = value; }

        /**
         * A reference used to locate the descriptor whose configuration was overridden.
         */
        const ReportingDescriptorReference & get_descriptor() const { return descriptor; }
        ReportingDescriptorReference & get_mutable_descriptor() { return descriptor; }
        void set_descriptor(const ReportingDescriptorReference & value) { this->descriptor = value; }

        /**
         * Key/value pairs that provide additional information about the configuration override.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A region within an artifact where a result was detected.
     *
     * Specifies a portion of the artifact that encloses the region. Allows a viewer to display
     * additional context around the region.
     *
     * Specifies a portion of the artifact.
     *
     * The region of the artifact to delete.
     */
    class Region {
        public:
        Region() :
            byte_length_constraint(0, boost::none, boost::none, boost::none, boost::none),
            byte_offset_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            char_length_constraint(0, boost::none, boost::none, boost::none, boost::none),
            char_offset_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            end_column_constraint(1, boost::none, boost::none, boost::none, boost::none),
            end_line_constraint(1, boost::none, boost::none, boost::none, boost::none),
            start_column_constraint(1, boost::none, boost::none, boost::none, boost::none),
            start_line_constraint(1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~Region() = default;

        private:
        std::shared_ptr<int64_t> byte_length;
        ClassMemberConstraints byte_length_constraint;
        std::shared_ptr<int64_t> byte_offset;
        ClassMemberConstraints byte_offset_constraint;
        std::shared_ptr<int64_t> char_length;
        ClassMemberConstraints char_length_constraint;
        std::shared_ptr<int64_t> char_offset;
        ClassMemberConstraints char_offset_constraint;
        std::shared_ptr<int64_t> end_column;
        ClassMemberConstraints end_column_constraint;
        std::shared_ptr<int64_t> end_line;
        ClassMemberConstraints end_line_constraint;
        std::shared_ptr<Message> message;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<ArtifactContent> snippet;
        std::shared_ptr<std::string> source_language;
        std::shared_ptr<int64_t> start_column;
        ClassMemberConstraints start_column_constraint;
        std::shared_ptr<int64_t> start_line;
        ClassMemberConstraints start_line_constraint;

        public:
        /**
         * The length of the region in bytes.
         */
        std::shared_ptr<int64_t> get_byte_length() const { return byte_length; }
        void set_byte_length(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("byte_length", byte_length_constraint, *value); this->byte_length = value; }

        /**
         * The zero-based offset from the beginning of the artifact of the first byte in the region.
         */
        std::shared_ptr<int64_t> get_byte_offset() const { return byte_offset; }
        void set_byte_offset(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("byte_offset", byte_offset_constraint, *value); this->byte_offset = value; }

        /**
         * The length of the region in characters.
         */
        std::shared_ptr<int64_t> get_char_length() const { return char_length; }
        void set_char_length(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("char_length", char_length_constraint, *value); this->char_length = value; }

        /**
         * The zero-based offset from the beginning of the artifact of the first character in the
         * region.
         */
        std::shared_ptr<int64_t> get_char_offset() const { return char_offset; }
        void set_char_offset(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("char_offset", char_offset_constraint, *value); this->char_offset = value; }

        /**
         * The column number of the character following the end of the region.
         */
        std::shared_ptr<int64_t> get_end_column() const { return end_column; }
        void set_end_column(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("end_column", end_column_constraint, *value); this->end_column = value; }

        /**
         * The line number of the last character in the region.
         */
        std::shared_ptr<int64_t> get_end_line() const { return end_line; }
        void set_end_line(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("end_line", end_line_constraint, *value); this->end_line = value; }

        /**
         * A message relevant to the region.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the region.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The portion of the artifact contents within the specified region.
         */
        std::shared_ptr<ArtifactContent> get_snippet() const { return snippet; }
        void set_snippet(std::shared_ptr<ArtifactContent> value) { this->snippet = value; }

        /**
         * Specifies the source language, if any, of the portion of the artifact specified by the
         * region object.
         */
        std::shared_ptr<std::string> get_source_language() const { return source_language; }
        void set_source_language(std::shared_ptr<std::string> value) { this->source_language = value; }

        /**
         * The column number of the first character in the region.
         */
        std::shared_ptr<int64_t> get_start_column() const { return start_column; }
        void set_start_column(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("start_column", start_column_constraint, *value); this->start_column = value; }

        /**
         * The line number of the first character in the region.
         */
        std::shared_ptr<int64_t> get_start_line() const { return start_line; }
        void set_start_line(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("start_line", start_line_constraint, *value); this->start_line = value; }
    };

    /**
     * A logical location of a construct that produced a result.
     */
    class LogicalLocation {
        public:
        LogicalLocation() :
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            parent_index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~LogicalLocation() = default;

        private:
        std::shared_ptr<std::string> decorated_name;
        std::shared_ptr<std::string> fully_qualified_name;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<std::string> kind;
        std::shared_ptr<std::string> name;
        std::shared_ptr<int64_t> parent_index;
        ClassMemberConstraints parent_index_constraint;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * The machine-readable name for the logical location, such as a mangled function name
         * provided by a C++ compiler that encodes calling convention, return type and other details
         * along with the function name.
         */
        std::shared_ptr<std::string> get_decorated_name() const { return decorated_name; }
        void set_decorated_name(std::shared_ptr<std::string> value) { this->decorated_name = value; }

        /**
         * The human-readable fully qualified name of the logical location.
         */
        std::shared_ptr<std::string> get_fully_qualified_name() const { return fully_qualified_name; }
        void set_fully_qualified_name(std::shared_ptr<std::string> value) { this->fully_qualified_name = value; }

        /**
         * The index within the logical locations array.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * The type of construct this logical location component refers to. Should be one of
         * 'function', 'member', 'module', 'namespace', 'parameter', 'resource', 'returnType',
         * 'type', 'variable', 'object', 'array', 'property', 'value', 'element', 'text',
         * 'attribute', 'comment', 'declaration', 'dtd' or 'processingInstruction', if any of those
         * accurately describe the construct.
         */
        std::shared_ptr<std::string> get_kind() const { return kind; }
        void set_kind(std::shared_ptr<std::string> value) { this->kind = value; }

        /**
         * Identifies the construct in which the result occurred. For example, this property might
         * contain the name of a class or a method.
         */
        std::shared_ptr<std::string> get_name() const { return name; }
        void set_name(std::shared_ptr<std::string> value) { this->name = value; }

        /**
         * Identifies the index of the immediate parent of the construct in which the result was
         * detected. For example, this property might point to a logical location that represents
         * the namespace that holds a type.
         */
        std::shared_ptr<int64_t> get_parent_index() const { return parent_index; }
        void set_parent_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("parent_index", parent_index_constraint, *value); this->parent_index = value; }

        /**
         * Key/value pairs that provide additional information about the logical location.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * Identifies the artifact and region.
     *
     * A physical location relevant to a result. Specifies a reference to a programming artifact
     * together with a range of bytes or characters within that artifact.
     */
    class PhysicalLocation {
        public:
        PhysicalLocation() = default;
        virtual ~PhysicalLocation() = default;

        private:
        std::shared_ptr<Address> address;
        std::shared_ptr<ArtifactLocation> artifact_location;
        std::shared_ptr<Region> context_region;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<Region> region;

        public:
        /**
         * The address of the location.
         */
        std::shared_ptr<Address> get_address() const { return address; }
        void set_address(std::shared_ptr<Address> value) { this->address = value; }

        /**
         * The location of the artifact.
         */
        std::shared_ptr<ArtifactLocation> get_artifact_location() const { return artifact_location; }
        void set_artifact_location(std::shared_ptr<ArtifactLocation> value) { this->artifact_location = value; }

        /**
         * Specifies a portion of the artifact that encloses the region. Allows a viewer to display
         * additional context around the region.
         */
        std::shared_ptr<Region> get_context_region() const { return context_region; }
        void set_context_region(std::shared_ptr<Region> value) { this->context_region = value; }

        /**
         * Key/value pairs that provide additional information about the physical location.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * Specifies a portion of the artifact.
         */
        std::shared_ptr<Region> get_region() const { return region; }
        void set_region(std::shared_ptr<Region> value) { this->region = value; }
    };

    /**
     * Information about the relation of one location to another.
     */
    class LocationRelationship {
        public:
        LocationRelationship() :
            target_constraint(0, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~LocationRelationship() = default;

        private:
        std::shared_ptr<Message> description;
        std::shared_ptr<std::vector<std::string>> kinds;
        std::shared_ptr<PropertyBag> properties;
        int64_t target;
        ClassMemberConstraints target_constraint;

        public:
        /**
         * A description of the location relationship.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * A set of distinct strings that categorize the relationship. Well-known kinds include
         * 'includes', 'isIncludedBy' and 'relevant'.
         */
        std::shared_ptr<std::vector<std::string>> get_kinds() const { return kinds; }
        void set_kinds(std::shared_ptr<std::vector<std::string>> value) { this->kinds = value; }

        /**
         * Key/value pairs that provide additional information about the location relationship.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A reference to the related location.
         */
        const int64_t & get_target() const { return target; }
        int64_t & get_mutable_target() { return target; }
        void set_target(const int64_t & value) { CheckConstraint("target", target_constraint, value); this->target = value; }
    };

    /**
     * The location to which this stack frame refers.
     *
     * A location within a programming artifact.
     *
     * A code location associated with the node.
     *
     * The code location.
     *
     * Identifies the location associated with the suppression.
     */
    class Location {
        public:
        Location() :
            id_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~Location() = default;

        private:
        std::shared_ptr<std::vector<Region>> annotations;
        std::shared_ptr<int64_t> id;
        ClassMemberConstraints id_constraint;
        std::shared_ptr<std::vector<LogicalLocation>> logical_locations;
        std::shared_ptr<Message> message;
        std::shared_ptr<PhysicalLocation> physical_location;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<LocationRelationship>> relationships;

        public:
        /**
         * A set of regions relevant to the location.
         */
        std::shared_ptr<std::vector<Region>> get_annotations() const { return annotations; }
        void set_annotations(std::shared_ptr<std::vector<Region>> value) { this->annotations = value; }

        /**
         * Value that distinguishes this location from all other locations within a single result
         * object.
         */
        std::shared_ptr<int64_t> get_id() const { return id; }
        void set_id(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("id", id_constraint, *value); this->id = value; }

        /**
         * The logical locations associated with the result.
         */
        std::shared_ptr<std::vector<LogicalLocation>> get_logical_locations() const { return logical_locations; }
        void set_logical_locations(std::shared_ptr<std::vector<LogicalLocation>> value) { this->logical_locations = value; }

        /**
         * A message relevant to the location.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Identifies the artifact and region.
         */
        std::shared_ptr<PhysicalLocation> get_physical_location() const { return physical_location; }
        void set_physical_location(std::shared_ptr<PhysicalLocation> value) { this->physical_location = value; }

        /**
         * Key/value pairs that provide additional information about the location.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of objects that describe relationships between this location and others.
         */
        std::shared_ptr<std::vector<LocationRelationship>> get_relationships() const { return relationships; }
        void set_relationships(std::shared_ptr<std::vector<LocationRelationship>> value) { this->relationships = value; }
    };

    /**
     * A function call within a stack trace.
     */
    class StackFrame {
        public:
        StackFrame() = default;
        virtual ~StackFrame() = default;

        private:
        std::shared_ptr<Location> location;
        std::shared_ptr<std::string> stack_frame_module;
        std::shared_ptr<std::vector<std::string>> parameters;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<int64_t> thread_id;

        public:
        /**
         * The location to which this stack frame refers.
         */
        std::shared_ptr<Location> get_location() const { return location; }
        void set_location(std::shared_ptr<Location> value) { this->location = value; }

        /**
         * The name of the module that contains the code of this stack frame.
         */
        std::shared_ptr<std::string> get_stack_frame_module() const { return stack_frame_module; }
        void set_stack_frame_module(std::shared_ptr<std::string> value) { this->stack_frame_module = value; }

        /**
         * The parameters of the call that is executing.
         */
        std::shared_ptr<std::vector<std::string>> get_parameters() const { return parameters; }
        void set_parameters(std::shared_ptr<std::vector<std::string>> value) { this->parameters = value; }

        /**
         * Key/value pairs that provide additional information about the stack frame.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The thread identifier of the stack frame.
         */
        std::shared_ptr<int64_t> get_thread_id() const { return thread_id; }
        void set_thread_id(std::shared_ptr<int64_t> value) { this->thread_id = value; }
    };

    /**
     * The sequence of function calls leading to the exception.
     *
     * A call stack that is relevant to a result.
     *
     * The call stack leading to this location.
     */
    class Stack {
        public:
        Stack() = default;
        virtual ~Stack() = default;

        private:
        std::vector<StackFrame> frames;
        std::shared_ptr<Message> message;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * An array of stack frames that represents a sequence of calls, rendered in reverse
         * chronological order, that comprise the call stack.
         */
        const std::vector<StackFrame> & get_frames() const { return frames; }
        std::vector<StackFrame> & get_mutable_frames() { return frames; }
        void set_frames(const std::vector<StackFrame> & value) { this->frames = value; }

        /**
         * A message relevant to this call stack.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the stack.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * The runtime exception, if any, relevant to this notification.
     *
     * Describes a runtime exception encountered during the execution of an analysis tool.
     */
    class Exception {
        public:
        Exception() = default;
        virtual ~Exception() = default;

        private:
        std::shared_ptr<std::vector<Exception>> inner_exceptions;
        std::shared_ptr<std::string> kind;
        std::shared_ptr<std::string> message;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<Stack> stack;

        public:
        /**
         * An array of exception objects each of which is considered a cause of this exception.
         */
        std::shared_ptr<std::vector<Exception>> get_inner_exceptions() const { return inner_exceptions; }
        void set_inner_exceptions(std::shared_ptr<std::vector<Exception>> value) { this->inner_exceptions = value; }

        /**
         * A string that identifies the kind of exception, for example, the fully qualified type
         * name of an object that was thrown, or the symbolic name of a signal.
         */
        std::shared_ptr<std::string> get_kind() const { return kind; }
        void set_kind(std::shared_ptr<std::string> value) { this->kind = value; }

        /**
         * A message that describes the exception.
         */
        std::shared_ptr<std::string> get_message() const { return message; }
        void set_message(std::shared_ptr<std::string> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the exception.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The sequence of function calls leading to the exception.
         */
        std::shared_ptr<Stack> get_stack() const { return stack; }
        void set_stack(std::shared_ptr<Stack> value) { this->stack = value; }
    };

    /**
     * Describes a condition relevant to the tool itself, as opposed to being relevant to a
     * target being analyzed by the tool.
     */
    class Notification {
        public:
        Notification() = default;
        virtual ~Notification() = default;

        private:
        std::shared_ptr<ReportingDescriptorReference> associated_rule;
        std::shared_ptr<ReportingDescriptorReference> descriptor;
        std::shared_ptr<Exception> exception;
        std::shared_ptr<Level> level;
        std::shared_ptr<std::vector<Location>> locations;
        Message message;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<int64_t> thread_id;
        std::shared_ptr<std::string> time_utc;

        public:
        /**
         * A reference used to locate the rule descriptor associated with this notification.
         */
        std::shared_ptr<ReportingDescriptorReference> get_associated_rule() const { return associated_rule; }
        void set_associated_rule(std::shared_ptr<ReportingDescriptorReference> value) { this->associated_rule = value; }

        /**
         * A reference used to locate the descriptor relevant to this notification.
         */
        std::shared_ptr<ReportingDescriptorReference> get_descriptor() const { return descriptor; }
        void set_descriptor(std::shared_ptr<ReportingDescriptorReference> value) { this->descriptor = value; }

        /**
         * The runtime exception, if any, relevant to this notification.
         */
        std::shared_ptr<Exception> get_exception() const { return exception; }
        void set_exception(std::shared_ptr<Exception> value) { this->exception = value; }

        /**
         * A value specifying the severity level of the notification.
         */
        std::shared_ptr<Level> get_level() const { return level; }
        void set_level(std::shared_ptr<Level> value) { this->level = value; }

        /**
         * The locations relevant to this notification.
         */
        std::shared_ptr<std::vector<Location>> get_locations() const { return locations; }
        void set_locations(std::shared_ptr<std::vector<Location>> value) { this->locations = value; }

        /**
         * A message that describes the condition that was encountered.
         */
        const Message & get_message() const { return message; }
        Message & get_mutable_message() { return message; }
        void set_message(const Message & value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the notification.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The thread identifier of the code that generated the notification.
         */
        std::shared_ptr<int64_t> get_thread_id() const { return thread_id; }
        void set_thread_id(std::shared_ptr<int64_t> value) { this->thread_id = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which the analysis tool generated
         * the notification.
         */
        std::shared_ptr<std::string> get_time_utc() const { return time_utc; }
        void set_time_utc(std::shared_ptr<std::string> value) { this->time_utc = value; }
    };

    /**
     * An invocation object that describes the invocation of the converter.
     *
     * The runtime environment of the analysis tool run.
     */
    class Invocation {
        public:
        Invocation() = default;
        virtual ~Invocation() = default;

        private:
        std::shared_ptr<std::string> account;
        std::shared_ptr<std::vector<std::string>> arguments;
        std::shared_ptr<std::string> command_line;
        std::shared_ptr<std::string> end_time_utc;
        std::shared_ptr<std::map<std::string, std::string>> environment_variables;
        std::shared_ptr<ArtifactLocation> executable_location;
        bool execution_successful;
        std::shared_ptr<int64_t> exit_code;
        std::shared_ptr<std::string> exit_code_description;
        std::shared_ptr<std::string> exit_signal_name;
        std::shared_ptr<int64_t> exit_signal_number;
        std::shared_ptr<std::string> machine;
        std::shared_ptr<std::vector<ConfigurationOverride>> notification_configuration_overrides;
        std::shared_ptr<int64_t> process_id;
        std::shared_ptr<std::string> process_start_failure_message;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<ArtifactLocation>> response_files;
        std::shared_ptr<std::vector<ConfigurationOverride>> rule_configuration_overrides;
        std::shared_ptr<std::string> start_time_utc;
        std::shared_ptr<ArtifactLocation> stderr;
        std::shared_ptr<ArtifactLocation> stdin;
        std::shared_ptr<ArtifactLocation> stdout;
        std::shared_ptr<ArtifactLocation> stdout_stderr;
        std::shared_ptr<std::vector<Notification>> tool_configuration_notifications;
        std::shared_ptr<std::vector<Notification>> tool_execution_notifications;
        std::shared_ptr<ArtifactLocation> working_directory;

        public:
        /**
         * The account under which the invocation occurred.
         */
        std::shared_ptr<std::string> get_account() const { return account; }
        void set_account(std::shared_ptr<std::string> value) { this->account = value; }

        /**
         * An array of strings, containing in order the command line arguments passed to the tool
         * from the operating system.
         */
        std::shared_ptr<std::vector<std::string>> get_arguments() const { return arguments; }
        void set_arguments(std::shared_ptr<std::vector<std::string>> value) { this->arguments = value; }

        /**
         * The command line used to invoke the tool.
         */
        std::shared_ptr<std::string> get_command_line() const { return command_line; }
        void set_command_line(std::shared_ptr<std::string> value) { this->command_line = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which the invocation ended. See
         * "Date/time properties" in the SARIF spec for the required format.
         */
        std::shared_ptr<std::string> get_end_time_utc() const { return end_time_utc; }
        void set_end_time_utc(std::shared_ptr<std::string> value) { this->end_time_utc = value; }

        /**
         * The environment variables associated with the analysis tool process, expressed as
         * key/value pairs.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_environment_variables() const { return environment_variables; }
        void set_environment_variables(std::shared_ptr<std::map<std::string, std::string>> value) { this->environment_variables = value; }

        /**
         * An absolute URI specifying the location of the executable that was invoked.
         */
        std::shared_ptr<ArtifactLocation> get_executable_location() const { return executable_location; }
        void set_executable_location(std::shared_ptr<ArtifactLocation> value) { this->executable_location = value; }

        /**
         * Specifies whether the tool's execution completed successfully.
         */
        const bool & get_execution_successful() const { return execution_successful; }
        bool & get_mutable_execution_successful() { return execution_successful; }
        void set_execution_successful(const bool & value) { this->execution_successful = value; }

        /**
         * The process exit code.
         */
        std::shared_ptr<int64_t> get_exit_code() const { return exit_code; }
        void set_exit_code(std::shared_ptr<int64_t> value) { this->exit_code = value; }

        /**
         * The reason for the process exit.
         */
        std::shared_ptr<std::string> get_exit_code_description() const { return exit_code_description; }
        void set_exit_code_description(std::shared_ptr<std::string> value) { this->exit_code_description = value; }

        /**
         * The name of the signal that caused the process to exit.
         */
        std::shared_ptr<std::string> get_exit_signal_name() const { return exit_signal_name; }
        void set_exit_signal_name(std::shared_ptr<std::string> value) { this->exit_signal_name = value; }

        /**
         * The numeric value of the signal that caused the process to exit.
         */
        std::shared_ptr<int64_t> get_exit_signal_number() const { return exit_signal_number; }
        void set_exit_signal_number(std::shared_ptr<int64_t> value) { this->exit_signal_number = value; }

        /**
         * The machine on which the invocation occurred.
         */
        std::shared_ptr<std::string> get_machine() const { return machine; }
        void set_machine(std::shared_ptr<std::string> value) { this->machine = value; }

        /**
         * An array of configurationOverride objects that describe notifications related runtime
         * overrides.
         */
        std::shared_ptr<std::vector<ConfigurationOverride>> get_notification_configuration_overrides() const { return notification_configuration_overrides; }
        void set_notification_configuration_overrides(std::shared_ptr<std::vector<ConfigurationOverride>> value) { this->notification_configuration_overrides = value; }

        /**
         * The id of the process in which the invocation occurred.
         */
        std::shared_ptr<int64_t> get_process_id() const { return process_id; }
        void set_process_id(std::shared_ptr<int64_t> value) { this->process_id = value; }

        /**
         * The reason given by the operating system that the process failed to start.
         */
        std::shared_ptr<std::string> get_process_start_failure_message() const { return process_start_failure_message; }
        void set_process_start_failure_message(std::shared_ptr<std::string> value) { this->process_start_failure_message = value; }

        /**
         * Key/value pairs that provide additional information about the invocation.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The locations of any response files specified on the tool's command line.
         */
        std::shared_ptr<std::vector<ArtifactLocation>> get_response_files() const { return response_files; }
        void set_response_files(std::shared_ptr<std::vector<ArtifactLocation>> value) { this->response_files = value; }

        /**
         * An array of configurationOverride objects that describe rules related runtime overrides.
         */
        std::shared_ptr<std::vector<ConfigurationOverride>> get_rule_configuration_overrides() const { return rule_configuration_overrides; }
        void set_rule_configuration_overrides(std::shared_ptr<std::vector<ConfigurationOverride>> value) { this->rule_configuration_overrides = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which the invocation started. See
         * "Date/time properties" in the SARIF spec for the required format.
         */
        std::shared_ptr<std::string> get_start_time_utc() const { return start_time_utc; }
        void set_start_time_utc(std::shared_ptr<std::string> value) { this->start_time_utc = value; }

        /**
         * A file containing the standard error stream from the process that was invoked.
         */
        std::shared_ptr<ArtifactLocation> get_stderr() const { return stderr; }
        void set_stderr(std::shared_ptr<ArtifactLocation> value) { this->stderr = value; }

        /**
         * A file containing the standard input stream to the process that was invoked.
         */
        std::shared_ptr<ArtifactLocation> get_stdin() const { return stdin; }
        void set_stdin(std::shared_ptr<ArtifactLocation> value) { this->stdin = value; }

        /**
         * A file containing the standard output stream from the process that was invoked.
         */
        std::shared_ptr<ArtifactLocation> get_stdout() const { return stdout; }
        void set_stdout(std::shared_ptr<ArtifactLocation> value) { this->stdout = value; }

        /**
         * A file containing the interleaved standard output and standard error stream from the
         * process that was invoked.
         */
        std::shared_ptr<ArtifactLocation> get_stdout_stderr() const { return stdout_stderr; }
        void set_stdout_stderr(std::shared_ptr<ArtifactLocation> value) { this->stdout_stderr = value; }

        /**
         * A list of conditions detected by the tool that are relevant to the tool's configuration.
         */
        std::shared_ptr<std::vector<Notification>> get_tool_configuration_notifications() const { return tool_configuration_notifications; }
        void set_tool_configuration_notifications(std::shared_ptr<std::vector<Notification>> value) { this->tool_configuration_notifications = value; }

        /**
         * A list of runtime conditions detected by the tool during the analysis.
         */
        std::shared_ptr<std::vector<Notification>> get_tool_execution_notifications() const { return tool_execution_notifications; }
        void set_tool_execution_notifications(std::shared_ptr<std::vector<Notification>> value) { this->tool_execution_notifications = value; }

        /**
         * The working directory for the invocation.
         */
        std::shared_ptr<ArtifactLocation> get_working_directory() const { return working_directory; }
        void set_working_directory(std::shared_ptr<ArtifactLocation> value) { this->working_directory = value; }
    };

    enum class Content : int { LOCALIZED_DATA, NON_LOCALIZED_DATA };

    /**
     * Information about the relation of one reporting descriptor to another.
     */
    class ReportingDescriptorRelationship {
        public:
        ReportingDescriptorRelationship() = default;
        virtual ~ReportingDescriptorRelationship() = default;

        private:
        std::shared_ptr<Message> description;
        std::shared_ptr<std::vector<std::string>> kinds;
        std::shared_ptr<PropertyBag> properties;
        ReportingDescriptorReference target;

        public:
        /**
         * A description of the reporting descriptor relationship.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * A set of distinct strings that categorize the relationship. Well-known kinds include
         * 'canPrecede', 'canFollow', 'willPrecede', 'willFollow', 'superset', 'subset', 'equal',
         * 'disjoint', 'relevant', and 'incomparable'.
         */
        std::shared_ptr<std::vector<std::string>> get_kinds() const { return kinds; }
        void set_kinds(std::shared_ptr<std::vector<std::string>> value) { this->kinds = value; }

        /**
         * Key/value pairs that provide additional information about the reporting descriptor
         * reference.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A reference to the related reporting descriptor.
         */
        const ReportingDescriptorReference & get_target() const { return target; }
        ReportingDescriptorReference & get_mutable_target() { return target; }
        void set_target(const ReportingDescriptorReference & value) { this->target = value; }
    };

    /**
     * Metadata that describes a specific report produced by the tool, as part of the analysis
     * it provides or its runtime reporting.
     */
    class ReportingDescriptor {
        public:
        ReportingDescriptor() :
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$"))
        {}
        virtual ~ReportingDescriptor() = default;

        private:
        std::shared_ptr<ReportingConfiguration> default_configuration;
        std::shared_ptr<std::vector<std::string>> deprecated_guids;
        std::shared_ptr<std::vector<std::string>> deprecated_ids;
        std::shared_ptr<std::vector<std::string>> deprecated_names;
        std::shared_ptr<MultiformatMessageString> full_description;
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<MultiformatMessageString> help;
        std::shared_ptr<std::string> help_uri;
        std::string id;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> message_strings;
        std::shared_ptr<std::string> name;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<ReportingDescriptorRelationship>> relationships;
        std::shared_ptr<MultiformatMessageString> short_description;

        public:
        /**
         * Default reporting configuration information.
         */
        std::shared_ptr<ReportingConfiguration> get_default_configuration() const { return default_configuration; }
        void set_default_configuration(std::shared_ptr<ReportingConfiguration> value) { this->default_configuration = value; }

        /**
         * An array of unique identifies in the form of a GUID by which this report was known in
         * some previous version of the analysis tool.
         */
        std::shared_ptr<std::vector<std::string>> get_deprecated_guids() const { return deprecated_guids; }
        void set_deprecated_guids(std::shared_ptr<std::vector<std::string>> value) { this->deprecated_guids = value; }

        /**
         * An array of stable, opaque identifiers by which this report was known in some previous
         * version of the analysis tool.
         */
        std::shared_ptr<std::vector<std::string>> get_deprecated_ids() const { return deprecated_ids; }
        void set_deprecated_ids(std::shared_ptr<std::vector<std::string>> value) { this->deprecated_ids = value; }

        /**
         * An array of readable identifiers by which this report was known in some previous version
         * of the analysis tool.
         */
        std::shared_ptr<std::vector<std::string>> get_deprecated_names() const { return deprecated_names; }
        void set_deprecated_names(std::shared_ptr<std::vector<std::string>> value) { this->deprecated_names = value; }

        /**
         * A description of the report. Should, as far as possible, provide details sufficient to
         * enable resolution of any problem indicated by the result.
         */
        std::shared_ptr<MultiformatMessageString> get_full_description() const { return full_description; }
        void set_full_description(std::shared_ptr<MultiformatMessageString> value) { this->full_description = value; }

        /**
         * A unique identifer for the reporting descriptor in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * Provides the primary documentation for the report, useful when there is no online
         * documentation.
         */
        std::shared_ptr<MultiformatMessageString> get_help() const { return help; }
        void set_help(std::shared_ptr<MultiformatMessageString> value) { this->help = value; }

        /**
         * A URI where the primary documentation for the report can be found.
         */
        std::shared_ptr<std::string> get_help_uri() const { return help_uri; }
        void set_help_uri(std::shared_ptr<std::string> value) { this->help_uri = value; }

        /**
         * A stable, opaque identifier for the report.
         */
        const std::string & get_id() const { return id; }
        std::string & get_mutable_id() { return id; }
        void set_id(const std::string & value) { this->id = value; }

        /**
         * A set of name/value pairs with arbitrary names. Each value is a multiformatMessageString
         * object, which holds message strings in plain text and (optionally) Markdown format. The
         * strings can include placeholders, which can be used to construct a message in combination
         * with an arbitrary number of additional string arguments.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_message_strings() const { return message_strings; }
        void set_message_strings(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->message_strings = value; }

        /**
         * A report identifier that is understandable to an end user.
         */
        std::shared_ptr<std::string> get_name() const { return name; }
        void set_name(std::shared_ptr<std::string> value) { this->name = value; }

        /**
         * Key/value pairs that provide additional information about the report.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of objects that describe relationships between this reporting descriptor and
         * others.
         */
        std::shared_ptr<std::vector<ReportingDescriptorRelationship>> get_relationships() const { return relationships; }
        void set_relationships(std::shared_ptr<std::vector<ReportingDescriptorRelationship>> value) { this->relationships = value; }

        /**
         * A concise description of the report. Should be a single sentence that is understandable
         * when visible space is limited to a single line of text.
         */
        std::shared_ptr<MultiformatMessageString> get_short_description() const { return short_description; }
        void set_short_description(std::shared_ptr<MultiformatMessageString> value) { this->short_description = value; }
    };

    /**
     * Translation metadata, required for a translation, not populated by other component
     * types.
     *
     * Provides additional metadata related to translation.
     */
    class TranslationMetadata {
        public:
        TranslationMetadata() = default;
        virtual ~TranslationMetadata() = default;

        private:
        std::shared_ptr<std::string> download_uri;
        std::shared_ptr<MultiformatMessageString> full_description;
        std::shared_ptr<std::string> full_name;
        std::shared_ptr<std::string> information_uri;
        std::string name;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<MultiformatMessageString> short_description;

        public:
        /**
         * The absolute URI from which the translation metadata can be downloaded.
         */
        std::shared_ptr<std::string> get_download_uri() const { return download_uri; }
        void set_download_uri(std::shared_ptr<std::string> value) { this->download_uri = value; }

        /**
         * A comprehensive description of the translation metadata.
         */
        std::shared_ptr<MultiformatMessageString> get_full_description() const { return full_description; }
        void set_full_description(std::shared_ptr<MultiformatMessageString> value) { this->full_description = value; }

        /**
         * The full name associated with the translation metadata.
         */
        std::shared_ptr<std::string> get_full_name() const { return full_name; }
        void set_full_name(std::shared_ptr<std::string> value) { this->full_name = value; }

        /**
         * The absolute URI from which information related to the translation metadata can be
         * downloaded.
         */
        std::shared_ptr<std::string> get_information_uri() const { return information_uri; }
        void set_information_uri(std::shared_ptr<std::string> value) { this->information_uri = value; }

        /**
         * The name associated with the translation metadata.
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * Key/value pairs that provide additional information about the translation metadata.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A brief description of the translation metadata.
         */
        std::shared_ptr<MultiformatMessageString> get_short_description() const { return short_description; }
        void set_short_description(std::shared_ptr<MultiformatMessageString> value) { this->short_description = value; }
    };

    /**
     * The analysis tool that was run.
     *
     * A component, such as a plug-in or the driver, of the analysis tool that was run.
     *
     * The analysis tool object that will be merged with a separate run.
     */
    class ToolComponent {
        public:
        ToolComponent() :
            dotted_quad_file_version_constraint(boost::none, boost::none, boost::none, boost::none, std::string("[0-9]+(\\.[0-9]+){3}")),
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            language_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-zA-Z]{2}|^[a-zA-Z]{2}-[a-zA-Z]{2}]?$"))
        {}
        virtual ~ToolComponent() = default;

        private:
        std::shared_ptr<ToolComponentReference> associated_component;
        std::shared_ptr<std::vector<Content>> contents;
        std::shared_ptr<std::string> dotted_quad_file_version;
        ClassMemberConstraints dotted_quad_file_version_constraint;
        std::shared_ptr<std::string> download_uri;
        std::shared_ptr<MultiformatMessageString> full_description;
        std::shared_ptr<std::string> full_name;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> global_message_strings;
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<std::string> information_uri;
        std::shared_ptr<bool> is_comprehensive;
        std::shared_ptr<std::string> language;
        ClassMemberConstraints language_constraint;
        std::shared_ptr<std::string> localized_data_semantic_version;
        std::shared_ptr<std::vector<ArtifactLocation>> locations;
        std::shared_ptr<std::string> minimum_required_localized_data_semantic_version;
        std::string name;
        std::shared_ptr<std::vector<ReportingDescriptor>> notifications;
        std::shared_ptr<std::string> organization;
        std::shared_ptr<std::string> product;
        std::shared_ptr<std::string> product_suite;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::string> release_date_utc;
        std::shared_ptr<std::vector<ReportingDescriptor>> rules;
        std::shared_ptr<std::string> semantic_version;
        std::shared_ptr<MultiformatMessageString> short_description;
        std::shared_ptr<std::vector<ToolComponentReference>> supported_taxonomies;
        std::shared_ptr<std::vector<ReportingDescriptor>> taxa;
        std::shared_ptr<TranslationMetadata> translation_metadata;
        std::shared_ptr<std::string> version;

        public:
        /**
         * The component which is strongly associated with this component. For a translation, this
         * refers to the component which has been translated. For an extension, this is the driver
         * that provides the extension's plugin model.
         */
        std::shared_ptr<ToolComponentReference> get_associated_component() const { return associated_component; }
        void set_associated_component(std::shared_ptr<ToolComponentReference> value) { this->associated_component = value; }

        /**
         * The kinds of data contained in this object.
         */
        std::shared_ptr<std::vector<Content>> get_contents() const { return contents; }
        void set_contents(std::shared_ptr<std::vector<Content>> value) { this->contents = value; }

        /**
         * The binary version of the tool component's primary executable file expressed as four
         * non-negative integers separated by a period (for operating systems that express file
         * versions in this way).
         */
        std::shared_ptr<std::string> get_dotted_quad_file_version() const { return dotted_quad_file_version; }
        void set_dotted_quad_file_version(std::shared_ptr<std::string> value) { if (value) CheckConstraint("dotted_quad_file_version", dotted_quad_file_version_constraint, *value); this->dotted_quad_file_version = value; }

        /**
         * The absolute URI from which the tool component can be downloaded.
         */
        std::shared_ptr<std::string> get_download_uri() const { return download_uri; }
        void set_download_uri(std::shared_ptr<std::string> value) { this->download_uri = value; }

        /**
         * A comprehensive description of the tool component.
         */
        std::shared_ptr<MultiformatMessageString> get_full_description() const { return full_description; }
        void set_full_description(std::shared_ptr<MultiformatMessageString> value) { this->full_description = value; }

        /**
         * The name of the tool component along with its version and any other useful identifying
         * information, such as its locale.
         */
        std::shared_ptr<std::string> get_full_name() const { return full_name; }
        void set_full_name(std::shared_ptr<std::string> value) { this->full_name = value; }

        /**
         * A dictionary, each of whose keys is a resource identifier and each of whose values is a
         * multiformatMessageString object, which holds message strings in plain text and
         * (optionally) Markdown format. The strings can include placeholders, which can be used to
         * construct a message in combination with an arbitrary number of additional string
         * arguments.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_global_message_strings() const { return global_message_strings; }
        void set_global_message_strings(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->global_message_strings = value; }

        /**
         * A unique identifer for the tool component in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * The absolute URI at which information about this version of the tool component can be
         * found.
         */
        std::shared_ptr<std::string> get_information_uri() const { return information_uri; }
        void set_information_uri(std::shared_ptr<std::string> value) { this->information_uri = value; }

        /**
         * Specifies whether this object contains a complete definition of the localizable and/or
         * non-localizable data for this component, as opposed to including only data that is
         * relevant to the results persisted to this log file.
         */
        std::shared_ptr<bool> get_is_comprehensive() const { return is_comprehensive; }
        void set_is_comprehensive(std::shared_ptr<bool> value) { this->is_comprehensive = value; }

        /**
         * The language of the messages emitted into the log file during this run (expressed as an
         * ISO 639-1 two-letter lowercase language code) and an optional region (expressed as an ISO
         * 3166-1 two-letter uppercase subculture code associated with a country or region). The
         * casing is recommended but not required (in order for this data to conform to RFC5646).
         */
        std::shared_ptr<std::string> get_language() const { return language; }
        void set_language(std::shared_ptr<std::string> value) { if (value) CheckConstraint("language", language_constraint, *value); this->language = value; }

        /**
         * The semantic version of the localized strings defined in this component; maintained by
         * components that provide translations.
         */
        std::shared_ptr<std::string> get_localized_data_semantic_version() const { return localized_data_semantic_version; }
        void set_localized_data_semantic_version(std::shared_ptr<std::string> value) { this->localized_data_semantic_version = value; }

        /**
         * An array of the artifactLocation objects associated with the tool component.
         */
        std::shared_ptr<std::vector<ArtifactLocation>> get_locations() const { return locations; }
        void set_locations(std::shared_ptr<std::vector<ArtifactLocation>> value) { this->locations = value; }

        /**
         * The minimum value of localizedDataSemanticVersion required in translations consumed by
         * this component; used by components that consume translations.
         */
        std::shared_ptr<std::string> get_minimum_required_localized_data_semantic_version() const { return minimum_required_localized_data_semantic_version; }
        void set_minimum_required_localized_data_semantic_version(std::shared_ptr<std::string> value) { this->minimum_required_localized_data_semantic_version = value; }

        /**
         * The name of the tool component.
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * An array of reportingDescriptor objects relevant to the notifications related to the
         * configuration and runtime execution of the tool component.
         */
        std::shared_ptr<std::vector<ReportingDescriptor>> get_notifications() const { return notifications; }
        void set_notifications(std::shared_ptr<std::vector<ReportingDescriptor>> value) { this->notifications = value; }

        /**
         * The organization or company that produced the tool component.
         */
        std::shared_ptr<std::string> get_organization() const { return organization; }
        void set_organization(std::shared_ptr<std::string> value) { this->organization = value; }

        /**
         * A product suite to which the tool component belongs.
         */
        std::shared_ptr<std::string> get_product() const { return product; }
        void set_product(std::shared_ptr<std::string> value) { this->product = value; }

        /**
         * A localizable string containing the name of the suite of products to which the tool
         * component belongs.
         */
        std::shared_ptr<std::string> get_product_suite() const { return product_suite; }
        void set_product_suite(std::shared_ptr<std::string> value) { this->product_suite = value; }

        /**
         * Key/value pairs that provide additional information about the tool component.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A string specifying the UTC date (and optionally, the time) of the component's release.
         */
        std::shared_ptr<std::string> get_release_date_utc() const { return release_date_utc; }
        void set_release_date_utc(std::shared_ptr<std::string> value) { this->release_date_utc = value; }

        /**
         * An array of reportingDescriptor objects relevant to the analysis performed by the tool
         * component.
         */
        std::shared_ptr<std::vector<ReportingDescriptor>> get_rules() const { return rules; }
        void set_rules(std::shared_ptr<std::vector<ReportingDescriptor>> value) { this->rules = value; }

        /**
         * The tool component version in the format specified by Semantic Versioning 2.0.
         */
        std::shared_ptr<std::string> get_semantic_version() const { return semantic_version; }
        void set_semantic_version(std::shared_ptr<std::string> value) { this->semantic_version = value; }

        /**
         * A brief description of the tool component.
         */
        std::shared_ptr<MultiformatMessageString> get_short_description() const { return short_description; }
        void set_short_description(std::shared_ptr<MultiformatMessageString> value) { this->short_description = value; }

        /**
         * An array of toolComponentReference objects to declare the taxonomies supported by the
         * tool component.
         */
        std::shared_ptr<std::vector<ToolComponentReference>> get_supported_taxonomies() const { return supported_taxonomies; }
        void set_supported_taxonomies(std::shared_ptr<std::vector<ToolComponentReference>> value) { this->supported_taxonomies = value; }

        /**
         * An array of reportingDescriptor objects relevant to the definitions of both standalone
         * and tool-defined taxonomies.
         */
        std::shared_ptr<std::vector<ReportingDescriptor>> get_taxa() const { return taxa; }
        void set_taxa(std::shared_ptr<std::vector<ReportingDescriptor>> value) { this->taxa = value; }

        /**
         * Translation metadata, required for a translation, not populated by other component types.
         */
        std::shared_ptr<TranslationMetadata> get_translation_metadata() const { return translation_metadata; }
        void set_translation_metadata(std::shared_ptr<TranslationMetadata> value) { this->translation_metadata = value; }

        /**
         * The tool component version, in whatever format the component natively provides.
         */
        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    /**
     * A tool object that describes the converter.
     *
     * The analysis tool that was run.
     *
     * Information about the tool or tool pipeline that generated the results in this run. A run
     * can only contain results produced by a single tool or tool pipeline. A run can aggregate
     * results from multiple log files, as long as context around the tool run (tool
     * command-line arguments and the like) is identical for all aggregated files.
     */
    class Tool {
        public:
        Tool() = default;
        virtual ~Tool() = default;

        private:
        ToolComponent driver;
        std::shared_ptr<std::vector<ToolComponent>> extensions;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * The analysis tool that was run.
         */
        const ToolComponent & get_driver() const { return driver; }
        ToolComponent & get_mutable_driver() { return driver; }
        void set_driver(const ToolComponent & value) { this->driver = value; }

        /**
         * Tool extensions that contributed to or reconfigured the analysis tool that was run.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_extensions() const { return extensions; }
        void set_extensions(std::shared_ptr<std::vector<ToolComponent>> value) { this->extensions = value; }

        /**
         * Key/value pairs that provide additional information about the tool.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A conversion object that will be merged with a separate run.
     *
     * Describes how a converter transformed the output of a static analysis tool from the
     * analysis tool's native output format into the SARIF format.
     *
     * A conversion object that describes how a converter transformed an analysis tool's native
     * reporting format into the SARIF format.
     */
    class Conversion {
        public:
        Conversion() = default;
        virtual ~Conversion() = default;

        private:
        std::shared_ptr<std::vector<ArtifactLocation>> analysis_tool_log_files;
        std::shared_ptr<Invocation> invocation;
        std::shared_ptr<PropertyBag> properties;
        Tool tool;

        public:
        /**
         * The locations of the analysis tool's per-run log files.
         */
        std::shared_ptr<std::vector<ArtifactLocation>> get_analysis_tool_log_files() const { return analysis_tool_log_files; }
        void set_analysis_tool_log_files(std::shared_ptr<std::vector<ArtifactLocation>> value) { this->analysis_tool_log_files = value; }

        /**
         * An invocation object that describes the invocation of the converter.
         */
        std::shared_ptr<Invocation> get_invocation() const { return invocation; }
        void set_invocation(std::shared_ptr<Invocation> value) { this->invocation = value; }

        /**
         * Key/value pairs that provide additional information about the conversion.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A tool object that describes the converter.
         */
        const Tool & get_tool() const { return tool; }
        Tool & get_mutable_tool() { return tool; }
        void set_tool(const Tool & value) { this->tool = value; }
    };

    /**
     * Represents a directed edge in a graph.
     */
    class Edge {
        public:
        Edge() = default;
        virtual ~Edge() = default;

        private:
        std::string id;
        std::shared_ptr<Message> label;
        std::shared_ptr<PropertyBag> properties;
        std::string source_node_id;
        std::string target_node_id;

        public:
        /**
         * A string that uniquely identifies the edge within its graph.
         */
        const std::string & get_id() const { return id; }
        std::string & get_mutable_id() { return id; }
        void set_id(const std::string & value) { this->id = value; }

        /**
         * A short description of the edge.
         */
        std::shared_ptr<Message> get_label() const { return label; }
        void set_label(std::shared_ptr<Message> value) { this->label = value; }

        /**
         * Key/value pairs that provide additional information about the edge.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * Identifies the source node (the node at which the edge starts).
         */
        const std::string & get_source_node_id() const { return source_node_id; }
        std::string & get_mutable_source_node_id() { return source_node_id; }
        void set_source_node_id(const std::string & value) { this->source_node_id = value; }

        /**
         * Identifies the target node (the node at which the edge ends).
         */
        const std::string & get_target_node_id() const { return target_node_id; }
        std::string & get_mutable_target_node_id() { return target_node_id; }
        void set_target_node_id(const std::string & value) { this->target_node_id = value; }
    };

    /**
     * Represents a node in a graph.
     */
    class Node {
        public:
        Node() = default;
        virtual ~Node() = default;

        private:
        std::shared_ptr<std::vector<Node>> children;
        std::string id;
        std::shared_ptr<Message> label;
        std::shared_ptr<Location> location;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * Array of child nodes.
         */
        std::shared_ptr<std::vector<Node>> get_children() const { return children; }
        void set_children(std::shared_ptr<std::vector<Node>> value) { this->children = value; }

        /**
         * A string that uniquely identifies the node within its graph.
         */
        const std::string & get_id() const { return id; }
        std::string & get_mutable_id() { return id; }
        void set_id(const std::string & value) { this->id = value; }

        /**
         * A short description of the node.
         */
        std::shared_ptr<Message> get_label() const { return label; }
        void set_label(std::shared_ptr<Message> value) { this->label = value; }

        /**
         * A code location associated with the node.
         */
        std::shared_ptr<Location> get_location() const { return location; }
        void set_location(std::shared_ptr<Location> value) { this->location = value; }

        /**
         * Key/value pairs that provide additional information about the node.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A network of nodes and directed edges that describes some aspect of the structure of the
     * code (for example, a call graph).
     */
    class Graph {
        public:
        Graph() = default;
        virtual ~Graph() = default;

        private:
        std::shared_ptr<Message> description;
        std::shared_ptr<std::vector<Edge>> edges;
        std::shared_ptr<std::vector<Node>> nodes;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * A description of the graph.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * An array of edge objects representing the edges of the graph.
         */
        std::shared_ptr<std::vector<Edge>> get_edges() const { return edges; }
        void set_edges(std::shared_ptr<std::vector<Edge>> value) { this->edges = value; }

        /**
         * An array of node objects representing the nodes of the graph.
         */
        std::shared_ptr<std::vector<Node>> get_nodes() const { return nodes; }
        void set_nodes(std::shared_ptr<std::vector<Node>> value) { this->nodes = value; }

        /**
         * Key/value pairs that provide additional information about the graph.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * An area within an image.
     */
    class Rectangle {
        public:
        Rectangle() = default;
        virtual ~Rectangle() = default;

        private:
        std::shared_ptr<double> bottom;
        std::shared_ptr<double> left;
        std::shared_ptr<Message> message;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<double> right;
        std::shared_ptr<double> top;

        public:
        /**
         * The Y coordinate of the bottom edge of the rectangle, measured in the image's natural
         * units.
         */
        std::shared_ptr<double> get_bottom() const { return bottom; }
        void set_bottom(std::shared_ptr<double> value) { this->bottom = value; }

        /**
         * The X coordinate of the left edge of the rectangle, measured in the image's natural units.
         */
        std::shared_ptr<double> get_left() const { return left; }
        void set_left(std::shared_ptr<double> value) { this->left = value; }

        /**
         * A message relevant to the rectangle.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the rectangle.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The X coordinate of the right edge of the rectangle, measured in the image's natural
         * units.
         */
        std::shared_ptr<double> get_right() const { return right; }
        void set_right(std::shared_ptr<double> value) { this->right = value; }

        /**
         * The Y coordinate of the top edge of the rectangle, measured in the image's natural units.
         */
        std::shared_ptr<double> get_top() const { return top; }
        void set_top(std::shared_ptr<double> value) { this->top = value; }
    };

    /**
     * An artifact relevant to a result.
     */
    class Attachment {
        public:
        Attachment() = default;
        virtual ~Attachment() = default;

        private:
        ArtifactLocation artifact_location;
        std::shared_ptr<Message> description;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<Rectangle>> rectangles;
        std::shared_ptr<std::vector<Region>> regions;

        public:
        /**
         * The location of the attachment.
         */
        const ArtifactLocation & get_artifact_location() const { return artifact_location; }
        ArtifactLocation & get_mutable_artifact_location() { return artifact_location; }
        void set_artifact_location(const ArtifactLocation & value) { this->artifact_location = value; }

        /**
         * A message describing the role played by the attachment.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * Key/value pairs that provide additional information about the attachment.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of rectangles specifying areas of interest within the image.
         */
        std::shared_ptr<std::vector<Rectangle>> get_rectangles() const { return rectangles; }
        void set_rectangles(std::shared_ptr<std::vector<Rectangle>> value) { this->rectangles = value; }

        /**
         * An array of regions of interest within the attachment.
         */
        std::shared_ptr<std::vector<Region>> get_regions() const { return regions; }
        void set_regions(std::shared_ptr<std::vector<Region>> value) { this->regions = value; }
    };

    /**
     * The state of a result relative to a baseline of a previous run.
     */
    enum class BaselineState : int { ABSENT, NEW, UNCHANGED, UPDATED };

    /**
     * Specifies the importance of this location in understanding the code flow in which it
     * occurs. The order from most to least important is "essential", "important",
     * "unimportant". Default: "important".
     */
    enum class Importance : int { ESSENTIAL, IMPORTANT, UNIMPORTANT };

    /**
     * A web request associated with this thread flow location.
     *
     * Describes an HTTP request.
     *
     * A web request associated with this result.
     */
    class WebRequest {
        public:
        WebRequest() :
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~WebRequest() = default;

        private:
        std::shared_ptr<ArtifactContent> body;
        std::shared_ptr<std::map<std::string, std::string>> headers;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<std::string> method;
        std::shared_ptr<std::map<std::string, std::string>> parameters;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::string> protocol;
        std::shared_ptr<std::string> target;
        std::shared_ptr<std::string> version;

        public:
        /**
         * The body of the request.
         */
        std::shared_ptr<ArtifactContent> get_body() const { return body; }
        void set_body(std::shared_ptr<ArtifactContent> value) { this->body = value; }

        /**
         * The request headers.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_headers() const { return headers; }
        void set_headers(std::shared_ptr<std::map<std::string, std::string>> value) { this->headers = value; }

        /**
         * The index within the run.webRequests array of the request object associated with this
         * result.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * The HTTP method. Well-known values are 'GET', 'PUT', 'POST', 'DELETE', 'PATCH', 'HEAD',
         * 'OPTIONS', 'TRACE', 'CONNECT'.
         */
        std::shared_ptr<std::string> get_method() const { return method; }
        void set_method(std::shared_ptr<std::string> value) { this->method = value; }

        /**
         * The request parameters.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_parameters() const { return parameters; }
        void set_parameters(std::shared_ptr<std::map<std::string, std::string>> value) { this->parameters = value; }

        /**
         * Key/value pairs that provide additional information about the request.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The request protocol. Example: 'http'.
         */
        std::shared_ptr<std::string> get_protocol() const { return protocol; }
        void set_protocol(std::shared_ptr<std::string> value) { this->protocol = value; }

        /**
         * The target of the request.
         */
        std::shared_ptr<std::string> get_target() const { return target; }
        void set_target(std::shared_ptr<std::string> value) { this->target = value; }

        /**
         * The request version. Example: '1.1'.
         */
        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    /**
     * A web response associated with this thread flow location.
     *
     * Describes the response to an HTTP request.
     *
     * A web response associated with this result.
     */
    class WebResponse {
        public:
        WebResponse() :
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~WebResponse() = default;

        private:
        std::shared_ptr<ArtifactContent> body;
        std::shared_ptr<std::map<std::string, std::string>> headers;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<bool> no_response_received;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::string> protocol;
        std::shared_ptr<std::string> reason_phrase;
        std::shared_ptr<int64_t> status_code;
        std::shared_ptr<std::string> version;

        public:
        /**
         * The body of the response.
         */
        std::shared_ptr<ArtifactContent> get_body() const { return body; }
        void set_body(std::shared_ptr<ArtifactContent> value) { this->body = value; }

        /**
         * The response headers.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_headers() const { return headers; }
        void set_headers(std::shared_ptr<std::map<std::string, std::string>> value) { this->headers = value; }

        /**
         * The index within the run.webResponses array of the response object associated with this
         * result.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * Specifies whether a response was received from the server.
         */
        std::shared_ptr<bool> get_no_response_received() const { return no_response_received; }
        void set_no_response_received(std::shared_ptr<bool> value) { this->no_response_received = value; }

        /**
         * Key/value pairs that provide additional information about the response.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The response protocol. Example: 'http'.
         */
        std::shared_ptr<std::string> get_protocol() const { return protocol; }
        void set_protocol(std::shared_ptr<std::string> value) { this->protocol = value; }

        /**
         * The response reason. Example: 'Not found'.
         */
        std::shared_ptr<std::string> get_reason_phrase() const { return reason_phrase; }
        void set_reason_phrase(std::shared_ptr<std::string> value) { this->reason_phrase = value; }

        /**
         * The response status code. Example: 451.
         */
        std::shared_ptr<int64_t> get_status_code() const { return status_code; }
        void set_status_code(std::shared_ptr<int64_t> value) { this->status_code = value; }

        /**
         * The response version. Example: '1.1'.
         */
        std::shared_ptr<std::string> get_version() const { return version; }
        void set_version(std::shared_ptr<std::string> value) { this->version = value; }
    };

    /**
     * A location visited by an analysis tool while simulating or monitoring the execution of a
     * program.
     */
    class ThreadFlowLocation {
        public:
        ThreadFlowLocation() :
            execution_order_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            index_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            nesting_level_constraint(0, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~ThreadFlowLocation() = default;

        private:
        std::shared_ptr<int64_t> execution_order;
        ClassMemberConstraints execution_order_constraint;
        std::shared_ptr<std::string> execution_time_utc;
        std::shared_ptr<Importance> importance;
        std::shared_ptr<int64_t> index;
        ClassMemberConstraints index_constraint;
        std::shared_ptr<std::vector<std::string>> kinds;
        std::shared_ptr<Location> location;
        std::shared_ptr<std::string> thread_flow_location_module;
        std::shared_ptr<int64_t> nesting_level;
        ClassMemberConstraints nesting_level_constraint;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<Stack> stack;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> state;
        std::shared_ptr<std::vector<ReportingDescriptorReference>> taxa;
        std::shared_ptr<WebRequest> web_request;
        std::shared_ptr<WebResponse> web_response;

        public:
        /**
         * An integer representing the temporal order in which execution reached this location.
         */
        std::shared_ptr<int64_t> get_execution_order() const { return execution_order; }
        void set_execution_order(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("execution_order", execution_order_constraint, *value); this->execution_order = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which this location was executed.
         */
        std::shared_ptr<std::string> get_execution_time_utc() const { return execution_time_utc; }
        void set_execution_time_utc(std::shared_ptr<std::string> value) { this->execution_time_utc = value; }

        /**
         * Specifies the importance of this location in understanding the code flow in which it
         * occurs. The order from most to least important is "essential", "important",
         * "unimportant". Default: "important".
         */
        std::shared_ptr<Importance> get_importance() const { return importance; }
        void set_importance(std::shared_ptr<Importance> value) { this->importance = value; }

        /**
         * The index within the run threadFlowLocations array.
         */
        std::shared_ptr<int64_t> get_index() const { return index; }
        void set_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("index", index_constraint, *value); this->index = value; }

        /**
         * A set of distinct strings that categorize the thread flow location. Well-known kinds
         * include 'acquire', 'release', 'enter', 'exit', 'call', 'return', 'branch', 'implicit',
         * 'false', 'true', 'caution', 'danger', 'unknown', 'unreachable', 'taint', 'function',
         * 'handler', 'lock', 'memory', 'resource', 'scope' and 'value'.
         */
        std::shared_ptr<std::vector<std::string>> get_kinds() const { return kinds; }
        void set_kinds(std::shared_ptr<std::vector<std::string>> value) { this->kinds = value; }

        /**
         * The code location.
         */
        std::shared_ptr<Location> get_location() const { return location; }
        void set_location(std::shared_ptr<Location> value) { this->location = value; }

        /**
         * The name of the module that contains the code that is executing.
         */
        std::shared_ptr<std::string> get_thread_flow_location_module() const { return thread_flow_location_module; }
        void set_thread_flow_location_module(std::shared_ptr<std::string> value) { this->thread_flow_location_module = value; }

        /**
         * An integer representing a containment hierarchy within the thread flow.
         */
        std::shared_ptr<int64_t> get_nesting_level() const { return nesting_level; }
        void set_nesting_level(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("nesting_level", nesting_level_constraint, *value); this->nesting_level = value; }

        /**
         * Key/value pairs that provide additional information about the threadflow location.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The call stack leading to this location.
         */
        std::shared_ptr<Stack> get_stack() const { return stack; }
        void set_stack(std::shared_ptr<Stack> value) { this->stack = value; }

        /**
         * A dictionary, each of whose keys specifies a variable or expression, the associated value
         * of which represents the variable or expression value. For an annotation of kind
         * 'continuation', for example, this dictionary might hold the current assumed values of a
         * set of global variables.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_state() const { return state; }
        void set_state(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->state = value; }

        /**
         * An array of references to rule or taxonomy reporting descriptors that are applicable to
         * the thread flow location.
         */
        std::shared_ptr<std::vector<ReportingDescriptorReference>> get_taxa() const { return taxa; }
        void set_taxa(std::shared_ptr<std::vector<ReportingDescriptorReference>> value) { this->taxa = value; }

        /**
         * A web request associated with this thread flow location.
         */
        std::shared_ptr<WebRequest> get_web_request() const { return web_request; }
        void set_web_request(std::shared_ptr<WebRequest> value) { this->web_request = value; }

        /**
         * A web response associated with this thread flow location.
         */
        std::shared_ptr<WebResponse> get_web_response() const { return web_response; }
        void set_web_response(std::shared_ptr<WebResponse> value) { this->web_response = value; }
    };

    /**
     * Describes a sequence of code locations that specify a path through a single thread of
     * execution such as an operating system or fiber.
     */
    class ThreadFlow {
        public:
        ThreadFlow() = default;
        virtual ~ThreadFlow() = default;

        private:
        std::shared_ptr<std::string> id;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> immutable_state;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> initial_state;
        std::vector<ThreadFlowLocation> locations;
        std::shared_ptr<Message> message;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * An string that uniquely identifies the threadFlow within the codeFlow in which it occurs.
         */
        std::shared_ptr<std::string> get_id() const { return id; }
        void set_id(std::shared_ptr<std::string> value) { this->id = value; }

        /**
         * Values of relevant expressions at the start of the thread flow that remain constant.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_immutable_state() const { return immutable_state; }
        void set_immutable_state(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->immutable_state = value; }

        /**
         * Values of relevant expressions at the start of the thread flow that may change during
         * thread flow execution.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_initial_state() const { return initial_state; }
        void set_initial_state(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->initial_state = value; }

        /**
         * A temporally ordered array of 'threadFlowLocation' objects, each of which describes a
         * location visited by the tool while producing the result.
         */
        const std::vector<ThreadFlowLocation> & get_locations() const { return locations; }
        std::vector<ThreadFlowLocation> & get_mutable_locations() { return locations; }
        void set_locations(const std::vector<ThreadFlowLocation> & value) { this->locations = value; }

        /**
         * A message relevant to the thread flow.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the thread flow.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A set of threadFlows which together describe a pattern of code execution relevant to
     * detecting a result.
     */
    class CodeFlow {
        public:
        CodeFlow() = default;
        virtual ~CodeFlow() = default;

        private:
        std::shared_ptr<Message> message;
        std::shared_ptr<PropertyBag> properties;
        std::vector<ThreadFlow> thread_flows;

        public:
        /**
         * A message relevant to the code flow.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the code flow.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of one or more unique threadFlow objects, each of which describes the progress
         * of a program through a thread of execution.
         */
        const std::vector<ThreadFlow> & get_thread_flows() const { return thread_flows; }
        std::vector<ThreadFlow> & get_mutable_thread_flows() { return thread_flows; }
        void set_thread_flows(const std::vector<ThreadFlow> & value) { this->thread_flows = value; }
    };

    /**
     * The replacement of a single region of an artifact.
     */
    class Replacement {
        public:
        Replacement() = default;
        virtual ~Replacement() = default;

        private:
        Region deleted_region;
        std::shared_ptr<ArtifactContent> inserted_content;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * The region of the artifact to delete.
         */
        const Region & get_deleted_region() const { return deleted_region; }
        Region & get_mutable_deleted_region() { return deleted_region; }
        void set_deleted_region(const Region & value) { this->deleted_region = value; }

        /**
         * The content to insert at the location specified by the 'deletedRegion' property.
         */
        std::shared_ptr<ArtifactContent> get_inserted_content() const { return inserted_content; }
        void set_inserted_content(std::shared_ptr<ArtifactContent> value) { this->inserted_content = value; }

        /**
         * Key/value pairs that provide additional information about the replacement.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A change to a single artifact.
     */
    class ArtifactChange {
        public:
        ArtifactChange() = default;
        virtual ~ArtifactChange() = default;

        private:
        ArtifactLocation artifact_location;
        std::shared_ptr<PropertyBag> properties;
        std::vector<Replacement> replacements;

        public:
        /**
         * The location of the artifact to change.
         */
        const ArtifactLocation & get_artifact_location() const { return artifact_location; }
        ArtifactLocation & get_mutable_artifact_location() { return artifact_location; }
        void set_artifact_location(const ArtifactLocation & value) { this->artifact_location = value; }

        /**
         * Key/value pairs that provide additional information about the change.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of replacement objects, each of which represents the replacement of a single
         * region in a single artifact specified by 'artifactLocation'.
         */
        const std::vector<Replacement> & get_replacements() const { return replacements; }
        std::vector<Replacement> & get_mutable_replacements() { return replacements; }
        void set_replacements(const std::vector<Replacement> & value) { this->replacements = value; }
    };

    /**
     * A proposed fix for the problem represented by a result object. A fix specifies a set of
     * artifacts to modify. For each artifact, it specifies a set of bytes to remove, and
     * provides a set of new bytes to replace them.
     */
    class Fix {
        public:
        Fix() = default;
        virtual ~Fix() = default;

        private:
        std::vector<ArtifactChange> artifact_changes;
        std::shared_ptr<Message> description;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * One or more artifact changes that comprise a fix for a result.
         */
        const std::vector<ArtifactChange> & get_artifact_changes() const { return artifact_changes; }
        std::vector<ArtifactChange> & get_mutable_artifact_changes() { return artifact_changes; }
        void set_artifact_changes(const std::vector<ArtifactChange> & value) { this->artifact_changes = value; }

        /**
         * A message that describes the proposed fix, enabling viewers to present the proposed
         * change to an end user.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * Key/value pairs that provide additional information about the fix.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * Represents the traversal of a single edge during a graph traversal.
     */
    class EdgeTraversal {
        public:
        EdgeTraversal() :
            step_over_edge_count_constraint(0, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~EdgeTraversal() = default;

        private:
        std::string edge_id;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> final_state;
        std::shared_ptr<Message> message;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<int64_t> step_over_edge_count;
        ClassMemberConstraints step_over_edge_count_constraint;

        public:
        /**
         * Identifies the edge being traversed.
         */
        const std::string & get_edge_id() const { return edge_id; }
        std::string & get_mutable_edge_id() { return edge_id; }
        void set_edge_id(const std::string & value) { this->edge_id = value; }

        /**
         * The values of relevant expressions after the edge has been traversed.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_final_state() const { return final_state; }
        void set_final_state(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->final_state = value; }

        /**
         * A message to display to the user as the edge is traversed.
         */
        std::shared_ptr<Message> get_message() const { return message; }
        void set_message(std::shared_ptr<Message> value) { this->message = value; }

        /**
         * Key/value pairs that provide additional information about the edge traversal.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The number of edge traversals necessary to return from a nested graph.
         */
        std::shared_ptr<int64_t> get_step_over_edge_count() const { return step_over_edge_count; }
        void set_step_over_edge_count(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("step_over_edge_count", step_over_edge_count_constraint, *value); this->step_over_edge_count = value; }
    };

    /**
     * Represents a path through a graph.
     */
    class GraphTraversal {
        public:
        GraphTraversal() :
            result_graph_index_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            run_graph_index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~GraphTraversal() = default;

        private:
        std::shared_ptr<Message> description;
        std::shared_ptr<std::vector<EdgeTraversal>> edge_traversals;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> immutable_state;
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> initial_state;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<int64_t> result_graph_index;
        ClassMemberConstraints result_graph_index_constraint;
        std::shared_ptr<int64_t> run_graph_index;
        ClassMemberConstraints run_graph_index_constraint;

        public:
        /**
         * A description of this graph traversal.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * The sequences of edges traversed by this graph traversal.
         */
        std::shared_ptr<std::vector<EdgeTraversal>> get_edge_traversals() const { return edge_traversals; }
        void set_edge_traversals(std::shared_ptr<std::vector<EdgeTraversal>> value) { this->edge_traversals = value; }

        /**
         * Values of relevant expressions at the start of the graph traversal that remain constant
         * for the graph traversal.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_immutable_state() const { return immutable_state; }
        void set_immutable_state(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->immutable_state = value; }

        /**
         * Values of relevant expressions at the start of the graph traversal that may change during
         * graph traversal.
         */
        std::shared_ptr<std::map<std::string, MultiformatMessageString>> get_initial_state() const { return initial_state; }
        void set_initial_state(std::shared_ptr<std::map<std::string, MultiformatMessageString>> value) { this->initial_state = value; }

        /**
         * Key/value pairs that provide additional information about the graph traversal.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The index within the result.graphs to be associated with the result.
         */
        std::shared_ptr<int64_t> get_result_graph_index() const { return result_graph_index; }
        void set_result_graph_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("result_graph_index", result_graph_index_constraint, *value); this->result_graph_index = value; }

        /**
         * The index within the run.graphs to be associated with the result.
         */
        std::shared_ptr<int64_t> get_run_graph_index() const { return run_graph_index; }
        void set_run_graph_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("run_graph_index", run_graph_index_constraint, *value); this->run_graph_index = value; }
    };

    /**
     * A value that categorizes results by evaluation state.
     */
    enum class ResultKind : int { FAIL, INFORMATIONAL, NOT_APPLICABLE, OPEN, PASS, REVIEW };

    /**
     * Information about how and when the result was detected.
     *
     * Contains information about how and when a result was detected.
     */
    class ResultProvenance {
        public:
        ResultProvenance() :
            first_detection_run_guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            invocation_index_constraint(-1, boost::none, boost::none, boost::none, boost::none),
            last_detection_run_guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$"))
        {}
        virtual ~ResultProvenance() = default;

        private:
        std::shared_ptr<std::vector<PhysicalLocation>> conversion_sources;
        std::shared_ptr<std::string> first_detection_run_guid;
        ClassMemberConstraints first_detection_run_guid_constraint;
        std::shared_ptr<std::string> first_detection_time_utc;
        std::shared_ptr<int64_t> invocation_index;
        ClassMemberConstraints invocation_index_constraint;
        std::shared_ptr<std::string> last_detection_run_guid;
        ClassMemberConstraints last_detection_run_guid_constraint;
        std::shared_ptr<std::string> last_detection_time_utc;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * An array of physicalLocation objects which specify the portions of an analysis tool's
         * output that a converter transformed into the result.
         */
        std::shared_ptr<std::vector<PhysicalLocation>> get_conversion_sources() const { return conversion_sources; }
        void set_conversion_sources(std::shared_ptr<std::vector<PhysicalLocation>> value) { this->conversion_sources = value; }

        /**
         * A GUID-valued string equal to the automationDetails.guid property of the run in which the
         * result was first detected.
         */
        std::shared_ptr<std::string> get_first_detection_run_guid() const { return first_detection_run_guid; }
        void set_first_detection_run_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("first_detection_run_guid", first_detection_run_guid_constraint, *value); this->first_detection_run_guid = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which the result was first
         * detected. See "Date/time properties" in the SARIF spec for the required format.
         */
        std::shared_ptr<std::string> get_first_detection_time_utc() const { return first_detection_time_utc; }
        void set_first_detection_time_utc(std::shared_ptr<std::string> value) { this->first_detection_time_utc = value; }

        /**
         * The index within the run.invocations array of the invocation object which describes the
         * tool invocation that detected the result.
         */
        std::shared_ptr<int64_t> get_invocation_index() const { return invocation_index; }
        void set_invocation_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("invocation_index", invocation_index_constraint, *value); this->invocation_index = value; }

        /**
         * A GUID-valued string equal to the automationDetails.guid property of the run in which the
         * result was most recently detected.
         */
        std::shared_ptr<std::string> get_last_detection_run_guid() const { return last_detection_run_guid; }
        void set_last_detection_run_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("last_detection_run_guid", last_detection_run_guid_constraint, *value); this->last_detection_run_guid = value; }

        /**
         * The Coordinated Universal Time (UTC) date and time at which the result was most recently
         * detected. See "Date/time properties" in the SARIF spec for the required format.
         */
        std::shared_ptr<std::string> get_last_detection_time_utc() const { return last_detection_time_utc; }
        void set_last_detection_time_utc(std::shared_ptr<std::string> value) { this->last_detection_time_utc = value; }

        /**
         * Key/value pairs that provide additional information about the result.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * A string that indicates where the suppression is persisted.
     */
    enum class SuppressionKind : int { EXTERNAL, IN_SOURCE };

    /**
     * A string that indicates the review status of the suppression.
     */
    enum class Status : int { ACCEPTED, REJECTED, UNDER_REVIEW };

    /**
     * A suppression that is relevant to a result.
     */
    class Suppression {
        public:
        Suppression() :
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$"))
        {}
        virtual ~Suppression() = default;

        private:
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<std::string> justification;
        SuppressionKind kind;
        std::shared_ptr<Location> location;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<Status> status;

        public:
        /**
         * A stable, unique identifer for the suprression in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * A string representing the justification for the suppression.
         */
        std::shared_ptr<std::string> get_justification() const { return justification; }
        void set_justification(std::shared_ptr<std::string> value) { this->justification = value; }

        /**
         * A string that indicates where the suppression is persisted.
         */
        const SuppressionKind & get_kind() const { return kind; }
        SuppressionKind & get_mutable_kind() { return kind; }
        void set_kind(const SuppressionKind & value) { this->kind = value; }

        /**
         * Identifies the location associated with the suppression.
         */
        std::shared_ptr<Location> get_location() const { return location; }
        void set_location(std::shared_ptr<Location> value) { this->location = value; }

        /**
         * Key/value pairs that provide additional information about the suppression.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * A string that indicates the review status of the suppression.
         */
        std::shared_ptr<Status> get_status() const { return status; }
        void set_status(std::shared_ptr<Status> value) { this->status = value; }
    };

    /**
     * A result produced by an analysis tool.
     */
    class Result {
        public:
        Result() :
            correlation_guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            occurrence_count_constraint(1, boost::none, boost::none, boost::none, boost::none),
            rank_constraint(-1, 100, boost::none, boost::none, boost::none),
            rule_index_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~Result() = default;

        private:
        std::shared_ptr<ArtifactLocation> analysis_target;
        std::shared_ptr<std::vector<Attachment>> attachments;
        std::shared_ptr<BaselineState> baseline_state;
        std::shared_ptr<std::vector<CodeFlow>> code_flows;
        std::shared_ptr<std::string> correlation_guid;
        ClassMemberConstraints correlation_guid_constraint;
        std::shared_ptr<std::map<std::string, std::string>> fingerprints;
        std::shared_ptr<std::vector<Fix>> fixes;
        std::shared_ptr<std::vector<Graph>> graphs;
        std::shared_ptr<std::vector<GraphTraversal>> graph_traversals;
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<std::string> hosted_viewer_uri;
        std::shared_ptr<ResultKind> kind;
        std::shared_ptr<Level> level;
        std::shared_ptr<std::vector<Location>> locations;
        Message message;
        std::shared_ptr<int64_t> occurrence_count;
        ClassMemberConstraints occurrence_count_constraint;
        std::shared_ptr<std::map<std::string, std::string>> partial_fingerprints;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<ResultProvenance> provenance;
        std::shared_ptr<double> rank;
        ClassMemberConstraints rank_constraint;
        std::shared_ptr<std::vector<Location>> related_locations;
        std::shared_ptr<ReportingDescriptorReference> rule;
        std::shared_ptr<std::string> rule_id;
        std::shared_ptr<int64_t> rule_index;
        ClassMemberConstraints rule_index_constraint;
        std::shared_ptr<std::vector<Stack>> stacks;
        std::shared_ptr<std::vector<Suppression>> suppressions;
        std::shared_ptr<std::vector<ReportingDescriptorReference>> taxa;
        std::shared_ptr<WebRequest> web_request;
        std::shared_ptr<WebResponse> web_response;
        std::shared_ptr<std::vector<std::string>> work_item_uris;

        public:
        /**
         * Identifies the artifact that the analysis tool was instructed to scan. This need not be
         * the same as the artifact where the result actually occurred.
         */
        std::shared_ptr<ArtifactLocation> get_analysis_target() const { return analysis_target; }
        void set_analysis_target(std::shared_ptr<ArtifactLocation> value) { this->analysis_target = value; }

        /**
         * A set of artifacts relevant to the result.
         */
        std::shared_ptr<std::vector<Attachment>> get_attachments() const { return attachments; }
        void set_attachments(std::shared_ptr<std::vector<Attachment>> value) { this->attachments = value; }

        /**
         * The state of a result relative to a baseline of a previous run.
         */
        std::shared_ptr<BaselineState> get_baseline_state() const { return baseline_state; }
        void set_baseline_state(std::shared_ptr<BaselineState> value) { this->baseline_state = value; }

        /**
         * An array of 'codeFlow' objects relevant to the result.
         */
        std::shared_ptr<std::vector<CodeFlow>> get_code_flows() const { return code_flows; }
        void set_code_flows(std::shared_ptr<std::vector<CodeFlow>> value) { this->code_flows = value; }

        /**
         * A stable, unique identifier for the equivalence class of logically identical results to
         * which this result belongs, in the form of a GUID.
         */
        std::shared_ptr<std::string> get_correlation_guid() const { return correlation_guid; }
        void set_correlation_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("correlation_guid", correlation_guid_constraint, *value); this->correlation_guid = value; }

        /**
         * A set of strings each of which individually defines a stable, unique identity for the
         * result.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_fingerprints() const { return fingerprints; }
        void set_fingerprints(std::shared_ptr<std::map<std::string, std::string>> value) { this->fingerprints = value; }

        /**
         * An array of 'fix' objects, each of which represents a proposed fix to the problem
         * indicated by the result.
         */
        std::shared_ptr<std::vector<Fix>> get_fixes() const { return fixes; }
        void set_fixes(std::shared_ptr<std::vector<Fix>> value) { this->fixes = value; }

        /**
         * An array of zero or more unique graph objects associated with the result.
         */
        std::shared_ptr<std::vector<Graph>> get_graphs() const { return graphs; }
        void set_graphs(std::shared_ptr<std::vector<Graph>> value) { this->graphs = value; }

        /**
         * An array of one or more unique 'graphTraversal' objects.
         */
        std::shared_ptr<std::vector<GraphTraversal>> get_graph_traversals() const { return graph_traversals; }
        void set_graph_traversals(std::shared_ptr<std::vector<GraphTraversal>> value) { this->graph_traversals = value; }

        /**
         * A stable, unique identifer for the result in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * An absolute URI at which the result can be viewed.
         */
        std::shared_ptr<std::string> get_hosted_viewer_uri() const { return hosted_viewer_uri; }
        void set_hosted_viewer_uri(std::shared_ptr<std::string> value) { this->hosted_viewer_uri = value; }

        /**
         * A value that categorizes results by evaluation state.
         */
        std::shared_ptr<ResultKind> get_kind() const { return kind; }
        void set_kind(std::shared_ptr<ResultKind> value) { this->kind = value; }

        /**
         * A value specifying the severity level of the result.
         */
        std::shared_ptr<Level> get_level() const { return level; }
        void set_level(std::shared_ptr<Level> value) { this->level = value; }

        /**
         * The set of locations where the result was detected. Specify only one location unless the
         * problem indicated by the result can only be corrected by making a change at every
         * specified location.
         */
        std::shared_ptr<std::vector<Location>> get_locations() const { return locations; }
        void set_locations(std::shared_ptr<std::vector<Location>> value) { this->locations = value; }

        /**
         * A message that describes the result. The first sentence of the message only will be
         * displayed when visible space is limited.
         */
        const Message & get_message() const { return message; }
        Message & get_mutable_message() { return message; }
        void set_message(const Message & value) { this->message = value; }

        /**
         * A positive integer specifying the number of times this logically unique result was
         * observed in this run.
         */
        std::shared_ptr<int64_t> get_occurrence_count() const { return occurrence_count; }
        void set_occurrence_count(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("occurrence_count", occurrence_count_constraint, *value); this->occurrence_count = value; }

        /**
         * A set of strings that contribute to the stable, unique identity of the result.
         */
        std::shared_ptr<std::map<std::string, std::string>> get_partial_fingerprints() const { return partial_fingerprints; }
        void set_partial_fingerprints(std::shared_ptr<std::map<std::string, std::string>> value) { this->partial_fingerprints = value; }

        /**
         * Key/value pairs that provide additional information about the result.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * Information about how and when the result was detected.
         */
        std::shared_ptr<ResultProvenance> get_provenance() const { return provenance; }
        void set_provenance(std::shared_ptr<ResultProvenance> value) { this->provenance = value; }

        /**
         * A number representing the priority or importance of the result.
         */
        std::shared_ptr<double> get_rank() const { return rank; }
        void set_rank(std::shared_ptr<double> value) { if (value) CheckConstraint("rank", rank_constraint, *value); this->rank = value; }

        /**
         * A set of locations relevant to this result.
         */
        std::shared_ptr<std::vector<Location>> get_related_locations() const { return related_locations; }
        void set_related_locations(std::shared_ptr<std::vector<Location>> value) { this->related_locations = value; }

        /**
         * A reference used to locate the rule descriptor relevant to this result.
         */
        std::shared_ptr<ReportingDescriptorReference> get_rule() const { return rule; }
        void set_rule(std::shared_ptr<ReportingDescriptorReference> value) { this->rule = value; }

        /**
         * The stable, unique identifier of the rule, if any, to which this result is relevant.
         */
        std::shared_ptr<std::string> get_rule_id() const { return rule_id; }
        void set_rule_id(std::shared_ptr<std::string> value) { this->rule_id = value; }

        /**
         * The index within the tool component rules array of the rule object associated with this
         * result.
         */
        std::shared_ptr<int64_t> get_rule_index() const { return rule_index; }
        void set_rule_index(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("rule_index", rule_index_constraint, *value); this->rule_index = value; }

        /**
         * An array of 'stack' objects relevant to the result.
         */
        std::shared_ptr<std::vector<Stack>> get_stacks() const { return stacks; }
        void set_stacks(std::shared_ptr<std::vector<Stack>> value) { this->stacks = value; }

        /**
         * A set of suppressions relevant to this result.
         */
        std::shared_ptr<std::vector<Suppression>> get_suppressions() const { return suppressions; }
        void set_suppressions(std::shared_ptr<std::vector<Suppression>> value) { this->suppressions = value; }

        /**
         * An array of references to taxonomy reporting descriptors that are applicable to the
         * result.
         */
        std::shared_ptr<std::vector<ReportingDescriptorReference>> get_taxa() const { return taxa; }
        void set_taxa(std::shared_ptr<std::vector<ReportingDescriptorReference>> value) { this->taxa = value; }

        /**
         * A web request associated with this result.
         */
        std::shared_ptr<WebRequest> get_web_request() const { return web_request; }
        void set_web_request(std::shared_ptr<WebRequest> value) { this->web_request = value; }

        /**
         * A web response associated with this result.
         */
        std::shared_ptr<WebResponse> get_web_response() const { return web_response; }
        void set_web_response(std::shared_ptr<WebResponse> value) { this->web_response = value; }

        /**
         * The URIs of the work items associated with this result.
         */
        std::shared_ptr<std::vector<std::string>> get_work_item_uris() const { return work_item_uris; }
        void set_work_item_uris(std::shared_ptr<std::vector<std::string>> value) { this->work_item_uris = value; }
    };

    /**
     * The SARIF format version of this external properties object.
     *
     * The SARIF format version of this log file.
     */
    enum class Version : int { THE_210 };

    /**
     * The top-level element of an external property file.
     */
    class ExternalProperties {
        public:
        ExternalProperties() :
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            run_guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$"))
        {}
        virtual ~ExternalProperties() = default;

        private:
        std::shared_ptr<std::vector<Address>> addresses;
        std::shared_ptr<std::vector<Artifact>> artifacts;
        std::shared_ptr<Conversion> conversion;
        std::shared_ptr<ToolComponent> driver;
        std::shared_ptr<std::vector<ToolComponent>> extensions;
        std::shared_ptr<PropertyBag> externalized_properties;
        std::shared_ptr<std::vector<Graph>> graphs;
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<std::vector<Invocation>> invocations;
        std::shared_ptr<std::vector<LogicalLocation>> logical_locations;
        std::shared_ptr<std::vector<ToolComponent>> policies;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<Result>> results;
        std::shared_ptr<std::string> run_guid;
        ClassMemberConstraints run_guid_constraint;
        std::shared_ptr<std::string> schema;
        std::shared_ptr<std::vector<ToolComponent>> taxonomies;
        std::shared_ptr<std::vector<ThreadFlowLocation>> thread_flow_locations;
        std::shared_ptr<std::vector<ToolComponent>> translations;
        std::shared_ptr<Version> version;
        std::shared_ptr<std::vector<WebRequest>> web_requests;
        std::shared_ptr<std::vector<WebResponse>> web_responses;

        public:
        /**
         * Addresses that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<Address>> get_addresses() const { return addresses; }
        void set_addresses(std::shared_ptr<std::vector<Address>> value) { this->addresses = value; }

        /**
         * An array of artifact objects that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<Artifact>> get_artifacts() const { return artifacts; }
        void set_artifacts(std::shared_ptr<std::vector<Artifact>> value) { this->artifacts = value; }

        /**
         * A conversion object that will be merged with a separate run.
         */
        std::shared_ptr<Conversion> get_conversion() const { return conversion; }
        void set_conversion(std::shared_ptr<Conversion> value) { this->conversion = value; }

        /**
         * The analysis tool object that will be merged with a separate run.
         */
        std::shared_ptr<ToolComponent> get_driver() const { return driver; }
        void set_driver(std::shared_ptr<ToolComponent> value) { this->driver = value; }

        /**
         * Tool extensions that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_extensions() const { return extensions; }
        void set_extensions(std::shared_ptr<std::vector<ToolComponent>> value) { this->extensions = value; }

        /**
         * Key/value pairs that provide additional information that will be merged with a separate
         * run.
         */
        std::shared_ptr<PropertyBag> get_externalized_properties() const { return externalized_properties; }
        void set_externalized_properties(std::shared_ptr<PropertyBag> value) { this->externalized_properties = value; }

        /**
         * An array of graph objects that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<Graph>> get_graphs() const { return graphs; }
        void set_graphs(std::shared_ptr<std::vector<Graph>> value) { this->graphs = value; }

        /**
         * A stable, unique identifer for this external properties object, in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * Describes the invocation of the analysis tool that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<Invocation>> get_invocations() const { return invocations; }
        void set_invocations(std::shared_ptr<std::vector<Invocation>> value) { this->invocations = value; }

        /**
         * An array of logical locations such as namespaces, types or functions that will be merged
         * with a separate run.
         */
        std::shared_ptr<std::vector<LogicalLocation>> get_logical_locations() const { return logical_locations; }
        void set_logical_locations(std::shared_ptr<std::vector<LogicalLocation>> value) { this->logical_locations = value; }

        /**
         * Tool policies that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_policies() const { return policies; }
        void set_policies(std::shared_ptr<std::vector<ToolComponent>> value) { this->policies = value; }

        /**
         * Key/value pairs that provide additional information about the external properties.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of result objects that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<Result>> get_results() const { return results; }
        void set_results(std::shared_ptr<std::vector<Result>> value) { this->results = value; }

        /**
         * A stable, unique identifer for the run associated with this external properties object,
         * in the form of a GUID.
         */
        std::shared_ptr<std::string> get_run_guid() const { return run_guid; }
        void set_run_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("run_guid", run_guid_constraint, *value); this->run_guid = value; }

        /**
         * The URI of the JSON schema corresponding to the version of the external property file
         * format.
         */
        std::shared_ptr<std::string> get_schema() const { return schema; }
        void set_schema(std::shared_ptr<std::string> value) { this->schema = value; }

        /**
         * Tool taxonomies that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_taxonomies() const { return taxonomies; }
        void set_taxonomies(std::shared_ptr<std::vector<ToolComponent>> value) { this->taxonomies = value; }

        /**
         * An array of threadFlowLocation objects that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<ThreadFlowLocation>> get_thread_flow_locations() const { return thread_flow_locations; }
        void set_thread_flow_locations(std::shared_ptr<std::vector<ThreadFlowLocation>> value) { this->thread_flow_locations = value; }

        /**
         * Tool translations that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_translations() const { return translations; }
        void set_translations(std::shared_ptr<std::vector<ToolComponent>> value) { this->translations = value; }

        /**
         * The SARIF format version of this external properties object.
         */
        std::shared_ptr<Version> get_version() const { return version; }
        void set_version(std::shared_ptr<Version> value) { this->version = value; }

        /**
         * Requests that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<WebRequest>> get_web_requests() const { return web_requests; }
        void set_web_requests(std::shared_ptr<std::vector<WebRequest>> value) { this->web_requests = value; }

        /**
         * Responses that will be merged with a separate run.
         */
        std::shared_ptr<std::vector<WebResponse>> get_web_responses() const { return web_responses; }
        void set_web_responses(std::shared_ptr<std::vector<WebResponse>> value) { this->web_responses = value; }
    };

    /**
     * Automation details that describe this run.
     *
     * Information that describes a run's identity and role within an engineering system process.
     */
    class RunAutomationDetails {
        public:
        RunAutomationDetails() :
            correlation_guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$"))
        {}
        virtual ~RunAutomationDetails() = default;

        private:
        std::shared_ptr<std::string> correlation_guid;
        ClassMemberConstraints correlation_guid_constraint;
        std::shared_ptr<Message> description;
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<std::string> id;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * A stable, unique identifier for the equivalence class of runs to which this object's
         * containing run object belongs in the form of a GUID.
         */
        std::shared_ptr<std::string> get_correlation_guid() const { return correlation_guid; }
        void set_correlation_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("correlation_guid", correlation_guid_constraint, *value); this->correlation_guid = value; }

        /**
         * A description of the identity and role played within the engineering system by this
         * object's containing run object.
         */
        std::shared_ptr<Message> get_description() const { return description; }
        void set_description(std::shared_ptr<Message> value) { this->description = value; }

        /**
         * A stable, unique identifer for this object's containing run object in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * A hierarchical string that uniquely identifies this object's containing run object.
         */
        std::shared_ptr<std::string> get_id() const { return id; }
        void set_id(std::shared_ptr<std::string> value) { this->id = value; }

        /**
         * Key/value pairs that provide additional information about the run automation details.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * Specifies the unit in which the tool measures columns.
     */
    enum class ColumnKind : int { UNICODE_CODE_POINTS, UTF16_CODE_UNITS };

    /**
     * An external property file containing a run.conversion object to be merged with the root
     * log file.
     *
     * An external property file containing a run.driver object to be merged with the root log
     * file.
     *
     * An external property file containing a run.properties object to be merged with the root
     * log file.
     *
     * Contains information that enables a SARIF consumer to locate the external property file
     * that contains the value of an externalized property associated with the run.
     */
    class ExternalPropertyFileReference {
        public:
        ExternalPropertyFileReference() :
            guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            item_count_constraint(-1, boost::none, boost::none, boost::none, boost::none)
        {}
        virtual ~ExternalPropertyFileReference() = default;

        private:
        std::shared_ptr<std::string> guid;
        ClassMemberConstraints guid_constraint;
        std::shared_ptr<int64_t> item_count;
        ClassMemberConstraints item_count_constraint;
        std::shared_ptr<ArtifactLocation> location;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * A stable, unique identifer for the external property file in the form of a GUID.
         */
        std::shared_ptr<std::string> get_guid() const { return guid; }
        void set_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("guid", guid_constraint, *value); this->guid = value; }

        /**
         * A non-negative integer specifying the number of items contained in the external property
         * file.
         */
        std::shared_ptr<int64_t> get_item_count() const { return item_count; }
        void set_item_count(std::shared_ptr<int64_t> value) { if (value) CheckConstraint("item_count", item_count_constraint, *value); this->item_count = value; }

        /**
         * The location of the external property file.
         */
        std::shared_ptr<ArtifactLocation> get_location() const { return location; }
        void set_location(std::shared_ptr<ArtifactLocation> value) { this->location = value; }

        /**
         * Key/value pairs that provide additional information about the external property file.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * References to external property files that should be inlined with the content of a root
     * log file.
     */
    class ExternalPropertyFileReferences {
        public:
        ExternalPropertyFileReferences() = default;
        virtual ~ExternalPropertyFileReferences() = default;

        private:
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> addresses;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> artifacts;
        std::shared_ptr<ExternalPropertyFileReference> conversion;
        std::shared_ptr<ExternalPropertyFileReference> driver;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> extensions;
        std::shared_ptr<ExternalPropertyFileReference> externalized_properties;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> graphs;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> invocations;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> logical_locations;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> policies;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> results;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> taxonomies;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> thread_flow_locations;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> translations;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> web_requests;
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> web_responses;

        public:
        /**
         * An array of external property files containing run.addresses arrays to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_addresses() const { return addresses; }
        void set_addresses(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->addresses = value; }

        /**
         * An array of external property files containing run.artifacts arrays to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_artifacts() const { return artifacts; }
        void set_artifacts(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->artifacts = value; }

        /**
         * An external property file containing a run.conversion object to be merged with the root
         * log file.
         */
        std::shared_ptr<ExternalPropertyFileReference> get_conversion() const { return conversion; }
        void set_conversion(std::shared_ptr<ExternalPropertyFileReference> value) { this->conversion = value; }

        /**
         * An external property file containing a run.driver object to be merged with the root log
         * file.
         */
        std::shared_ptr<ExternalPropertyFileReference> get_driver() const { return driver; }
        void set_driver(std::shared_ptr<ExternalPropertyFileReference> value) { this->driver = value; }

        /**
         * An array of external property files containing run.extensions arrays to be merged with
         * the root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_extensions() const { return extensions; }
        void set_extensions(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->extensions = value; }

        /**
         * An external property file containing a run.properties object to be merged with the root
         * log file.
         */
        std::shared_ptr<ExternalPropertyFileReference> get_externalized_properties() const { return externalized_properties; }
        void set_externalized_properties(std::shared_ptr<ExternalPropertyFileReference> value) { this->externalized_properties = value; }

        /**
         * An array of external property files containing a run.graphs object to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_graphs() const { return graphs; }
        void set_graphs(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->graphs = value; }

        /**
         * An array of external property files containing run.invocations arrays to be merged with
         * the root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_invocations() const { return invocations; }
        void set_invocations(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->invocations = value; }

        /**
         * An array of external property files containing run.logicalLocations arrays to be merged
         * with the root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_logical_locations() const { return logical_locations; }
        void set_logical_locations(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->logical_locations = value; }

        /**
         * An array of external property files containing run.policies arrays to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_policies() const { return policies; }
        void set_policies(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->policies = value; }

        /**
         * Key/value pairs that provide additional information about the external property files.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of external property files containing run.results arrays to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_results() const { return results; }
        void set_results(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->results = value; }

        /**
         * An array of external property files containing run.taxonomies arrays to be merged with
         * the root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_taxonomies() const { return taxonomies; }
        void set_taxonomies(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->taxonomies = value; }

        /**
         * An array of external property files containing run.threadFlowLocations arrays to be
         * merged with the root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_thread_flow_locations() const { return thread_flow_locations; }
        void set_thread_flow_locations(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->thread_flow_locations = value; }

        /**
         * An array of external property files containing run.translations arrays to be merged with
         * the root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_translations() const { return translations; }
        void set_translations(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->translations = value; }

        /**
         * An array of external property files containing run.requests arrays to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_web_requests() const { return web_requests; }
        void set_web_requests(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->web_requests = value; }

        /**
         * An array of external property files containing run.responses arrays to be merged with the
         * root log file.
         */
        std::shared_ptr<std::vector<ExternalPropertyFileReference>> get_web_responses() const { return web_responses; }
        void set_web_responses(std::shared_ptr<std::vector<ExternalPropertyFileReference>> value) { this->web_responses = value; }
    };

    /**
     * A specialLocations object that defines locations of special significance to SARIF
     * consumers.
     *
     * Defines locations of special significance to SARIF consumers.
     */
    class SpecialLocations {
        public:
        SpecialLocations() = default;
        virtual ~SpecialLocations() = default;

        private:
        std::shared_ptr<ArtifactLocation> display_base;
        std::shared_ptr<PropertyBag> properties;

        public:
        /**
         * Provides a suggestion to SARIF consumers to display file paths relative to the specified
         * location.
         */
        std::shared_ptr<ArtifactLocation> get_display_base() const { return display_base; }
        void set_display_base(std::shared_ptr<ArtifactLocation> value) { this->display_base = value; }

        /**
         * Key/value pairs that provide additional information about the special locations.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }
    };

    /**
     * Specifies the information necessary to retrieve a desired revision from a version control
     * system.
     */
    class VersionControlDetails {
        public:
        VersionControlDetails() = default;
        virtual ~VersionControlDetails() = default;

        private:
        std::shared_ptr<std::string> as_of_time_utc;
        std::shared_ptr<std::string> branch;
        std::shared_ptr<ArtifactLocation> mapped_to;
        std::shared_ptr<PropertyBag> properties;
        std::string repository_uri;
        std::shared_ptr<std::string> revision_id;
        std::shared_ptr<std::string> revision_tag;

        public:
        /**
         * A Coordinated Universal Time (UTC) date and time that can be used to synchronize an
         * enlistment to the state of the repository at that time.
         */
        std::shared_ptr<std::string> get_as_of_time_utc() const { return as_of_time_utc; }
        void set_as_of_time_utc(std::shared_ptr<std::string> value) { this->as_of_time_utc = value; }

        /**
         * The name of a branch containing the revision.
         */
        std::shared_ptr<std::string> get_branch() const { return branch; }
        void set_branch(std::shared_ptr<std::string> value) { this->branch = value; }

        /**
         * The location in the local file system to which the root of the repository was mapped at
         * the time of the analysis.
         */
        std::shared_ptr<ArtifactLocation> get_mapped_to() const { return mapped_to; }
        void set_mapped_to(std::shared_ptr<ArtifactLocation> value) { this->mapped_to = value; }

        /**
         * Key/value pairs that provide additional information about the version control details.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The absolute URI of the repository.
         */
        const std::string & get_repository_uri() const { return repository_uri; }
        std::string & get_mutable_repository_uri() { return repository_uri; }
        void set_repository_uri(const std::string & value) { this->repository_uri = value; }

        /**
         * A string that uniquely and permanently identifies the revision within the repository.
         */
        std::shared_ptr<std::string> get_revision_id() const { return revision_id; }
        void set_revision_id(std::shared_ptr<std::string> value) { this->revision_id = value; }

        /**
         * A tag that has been applied to the revision.
         */
        std::shared_ptr<std::string> get_revision_tag() const { return revision_tag; }
        void set_revision_tag(std::shared_ptr<std::string> value) { this->revision_tag = value; }
    };

    /**
     * Describes a single run of an analysis tool, and contains the reported output of that run.
     */
    class Run {
        public:
        Run() :
            baseline_guid_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$")),
            language_constraint(boost::none, boost::none, boost::none, boost::none, std::string("^[a-zA-Z]{2}|^[a-zA-Z]{2}-[a-zA-Z]{2}]?$"))
        {}
        virtual ~Run() = default;

        private:
        std::shared_ptr<std::vector<Address>> addresses;
        std::shared_ptr<std::vector<Artifact>> artifacts;
        std::shared_ptr<RunAutomationDetails> automation_details;
        std::shared_ptr<std::string> baseline_guid;
        ClassMemberConstraints baseline_guid_constraint;
        std::shared_ptr<ColumnKind> column_kind;
        std::shared_ptr<Conversion> conversion;
        std::shared_ptr<std::string> default_encoding;
        std::shared_ptr<std::string> default_source_language;
        std::shared_ptr<ExternalPropertyFileReferences> external_property_file_references;
        std::shared_ptr<std::vector<Graph>> graphs;
        std::shared_ptr<std::vector<Invocation>> invocations;
        std::shared_ptr<std::string> language;
        ClassMemberConstraints language_constraint;
        std::shared_ptr<std::vector<LogicalLocation>> logical_locations;
        std::shared_ptr<std::vector<std::string>> newline_sequences;
        std::shared_ptr<std::map<std::string, ArtifactLocation>> original_uri_base_ids;
        std::shared_ptr<std::vector<ToolComponent>> policies;
        std::shared_ptr<PropertyBag> properties;
        std::shared_ptr<std::vector<std::string>> redaction_tokens;
        std::shared_ptr<std::vector<Result>> results;
        std::shared_ptr<std::vector<RunAutomationDetails>> run_aggregates;
        std::shared_ptr<SpecialLocations> special_locations;
        std::shared_ptr<std::vector<ToolComponent>> taxonomies;
        std::shared_ptr<std::vector<ThreadFlowLocation>> thread_flow_locations;
        Tool tool;
        std::shared_ptr<std::vector<ToolComponent>> translations;
        std::shared_ptr<std::vector<VersionControlDetails>> version_control_provenance;
        std::shared_ptr<std::vector<WebRequest>> web_requests;
        std::shared_ptr<std::vector<WebResponse>> web_responses;

        public:
        /**
         * Addresses associated with this run instance, if any.
         */
        std::shared_ptr<std::vector<Address>> get_addresses() const { return addresses; }
        void set_addresses(std::shared_ptr<std::vector<Address>> value) { this->addresses = value; }

        /**
         * An array of artifact objects relevant to the run.
         */
        std::shared_ptr<std::vector<Artifact>> get_artifacts() const { return artifacts; }
        void set_artifacts(std::shared_ptr<std::vector<Artifact>> value) { this->artifacts = value; }

        /**
         * Automation details that describe this run.
         */
        std::shared_ptr<RunAutomationDetails> get_automation_details() const { return automation_details; }
        void set_automation_details(std::shared_ptr<RunAutomationDetails> value) { this->automation_details = value; }

        /**
         * The 'guid' property of a previous SARIF 'run' that comprises the baseline that was used
         * to compute result 'baselineState' properties for the run.
         */
        std::shared_ptr<std::string> get_baseline_guid() const { return baseline_guid; }
        void set_baseline_guid(std::shared_ptr<std::string> value) { if (value) CheckConstraint("baseline_guid", baseline_guid_constraint, *value); this->baseline_guid = value; }

        /**
         * Specifies the unit in which the tool measures columns.
         */
        std::shared_ptr<ColumnKind> get_column_kind() const { return column_kind; }
        void set_column_kind(std::shared_ptr<ColumnKind> value) { this->column_kind = value; }

        /**
         * A conversion object that describes how a converter transformed an analysis tool's native
         * reporting format into the SARIF format.
         */
        std::shared_ptr<Conversion> get_conversion() const { return conversion; }
        void set_conversion(std::shared_ptr<Conversion> value) { this->conversion = value; }

        /**
         * Specifies the default encoding for any artifact object that refers to a text file.
         */
        std::shared_ptr<std::string> get_default_encoding() const { return default_encoding; }
        void set_default_encoding(std::shared_ptr<std::string> value) { this->default_encoding = value; }

        /**
         * Specifies the default source language for any artifact object that refers to a text file
         * that contains source code.
         */
        std::shared_ptr<std::string> get_default_source_language() const { return default_source_language; }
        void set_default_source_language(std::shared_ptr<std::string> value) { this->default_source_language = value; }

        /**
         * References to external property files that should be inlined with the content of a root
         * log file.
         */
        std::shared_ptr<ExternalPropertyFileReferences> get_external_property_file_references() const { return external_property_file_references; }
        void set_external_property_file_references(std::shared_ptr<ExternalPropertyFileReferences> value) { this->external_property_file_references = value; }

        /**
         * An array of zero or more unique graph objects associated with the run.
         */
        std::shared_ptr<std::vector<Graph>> get_graphs() const { return graphs; }
        void set_graphs(std::shared_ptr<std::vector<Graph>> value) { this->graphs = value; }

        /**
         * Describes the invocation of the analysis tool.
         */
        std::shared_ptr<std::vector<Invocation>> get_invocations() const { return invocations; }
        void set_invocations(std::shared_ptr<std::vector<Invocation>> value) { this->invocations = value; }

        /**
         * The language of the messages emitted into the log file during this run (expressed as an
         * ISO 639-1 two-letter lowercase culture code) and an optional region (expressed as an ISO
         * 3166-1 two-letter uppercase subculture code associated with a country or region). The
         * casing is recommended but not required (in order for this data to conform to RFC5646).
         */
        std::shared_ptr<std::string> get_language() const { return language; }
        void set_language(std::shared_ptr<std::string> value) { if (value) CheckConstraint("language", language_constraint, *value); this->language = value; }

        /**
         * An array of logical locations such as namespaces, types or functions.
         */
        std::shared_ptr<std::vector<LogicalLocation>> get_logical_locations() const { return logical_locations; }
        void set_logical_locations(std::shared_ptr<std::vector<LogicalLocation>> value) { this->logical_locations = value; }

        /**
         * An ordered list of character sequences that were treated as line breaks when computing
         * region information for the run.
         */
        std::shared_ptr<std::vector<std::string>> get_newline_sequences() const { return newline_sequences; }
        void set_newline_sequences(std::shared_ptr<std::vector<std::string>> value) { this->newline_sequences = value; }

        /**
         * The artifact location specified by each uriBaseId symbol on the machine where the tool
         * originally ran.
         */
        std::shared_ptr<std::map<std::string, ArtifactLocation>> get_original_uri_base_ids() const { return original_uri_base_ids; }
        void set_original_uri_base_ids(std::shared_ptr<std::map<std::string, ArtifactLocation>> value) { this->original_uri_base_ids = value; }

        /**
         * Contains configurations that may potentially override both
         * reportingDescriptor.defaultConfiguration (the tool's default severities) and
         * invocation.configurationOverrides (severities established at run-time from the command
         * line).
         */
        std::shared_ptr<std::vector<ToolComponent>> get_policies() const { return policies; }
        void set_policies(std::shared_ptr<std::vector<ToolComponent>> value) { this->policies = value; }

        /**
         * Key/value pairs that provide additional information about the run.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * An array of strings used to replace sensitive information in a redaction-aware property.
         */
        std::shared_ptr<std::vector<std::string>> get_redaction_tokens() const { return redaction_tokens; }
        void set_redaction_tokens(std::shared_ptr<std::vector<std::string>> value) { this->redaction_tokens = value; }

        /**
         * The set of results contained in an SARIF log. The results array can be omitted when a run
         * is solely exporting rules metadata. It must be present (but may be empty) if a log file
         * represents an actual scan.
         */
        std::shared_ptr<std::vector<Result>> get_results() const { return results; }
        void set_results(std::shared_ptr<std::vector<Result>> value) { this->results = value; }

        /**
         * Automation details that describe the aggregate of runs to which this run belongs.
         */
        std::shared_ptr<std::vector<RunAutomationDetails>> get_run_aggregates() const { return run_aggregates; }
        void set_run_aggregates(std::shared_ptr<std::vector<RunAutomationDetails>> value) { this->run_aggregates = value; }

        /**
         * A specialLocations object that defines locations of special significance to SARIF
         * consumers.
         */
        std::shared_ptr<SpecialLocations> get_special_locations() const { return special_locations; }
        void set_special_locations(std::shared_ptr<SpecialLocations> value) { this->special_locations = value; }

        /**
         * An array of toolComponent objects relevant to a taxonomy in which results are categorized.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_taxonomies() const { return taxonomies; }
        void set_taxonomies(std::shared_ptr<std::vector<ToolComponent>> value) { this->taxonomies = value; }

        /**
         * An array of threadFlowLocation objects cached at run level.
         */
        std::shared_ptr<std::vector<ThreadFlowLocation>> get_thread_flow_locations() const { return thread_flow_locations; }
        void set_thread_flow_locations(std::shared_ptr<std::vector<ThreadFlowLocation>> value) { this->thread_flow_locations = value; }

        /**
         * Information about the tool or tool pipeline that generated the results in this run. A run
         * can only contain results produced by a single tool or tool pipeline. A run can aggregate
         * results from multiple log files, as long as context around the tool run (tool
         * command-line arguments and the like) is identical for all aggregated files.
         */
        const Tool & get_tool() const { return tool; }
        Tool & get_mutable_tool() { return tool; }
        void set_tool(const Tool & value) { this->tool = value; }

        /**
         * The set of available translations of the localized data provided by the tool.
         */
        std::shared_ptr<std::vector<ToolComponent>> get_translations() const { return translations; }
        void set_translations(std::shared_ptr<std::vector<ToolComponent>> value) { this->translations = value; }

        /**
         * Specifies the revision in version control of the artifacts that were scanned.
         */
        std::shared_ptr<std::vector<VersionControlDetails>> get_version_control_provenance() const { return version_control_provenance; }
        void set_version_control_provenance(std::shared_ptr<std::vector<VersionControlDetails>> value) { this->version_control_provenance = value; }

        /**
         * An array of request objects cached at run level.
         */
        std::shared_ptr<std::vector<WebRequest>> get_web_requests() const { return web_requests; }
        void set_web_requests(std::shared_ptr<std::vector<WebRequest>> value) { this->web_requests = value; }

        /**
         * An array of response objects cached at run level.
         */
        std::shared_ptr<std::vector<WebResponse>> get_web_responses() const { return web_responses; }
        void set_web_responses(std::shared_ptr<std::vector<WebResponse>> value) { this->web_responses = value; }
    };

    /**
     * Static Analysis Results Format (SARIF) Version 2.1.0 JSON Schema: a standard format for
     * the output of static analysis tools.
     */
    class Coordinate {
        public:
        Coordinate() = default;
        virtual ~Coordinate() = default;

        private:
        std::shared_ptr<std::string> schema;
        std::shared_ptr<std::vector<ExternalProperties>> inline_external_properties;
        std::shared_ptr<PropertyBag> properties;
        std::vector<Run> runs;
        Version version;

        public:
        /**
         * The URI of the JSON schema corresponding to the version.
         */
        std::shared_ptr<std::string> get_schema() const { return schema; }
        void set_schema(std::shared_ptr<std::string> value) { this->schema = value; }

        /**
         * References to external property files that share data between runs.
         */
        std::shared_ptr<std::vector<ExternalProperties>> get_inline_external_properties() const { return inline_external_properties; }
        void set_inline_external_properties(std::shared_ptr<std::vector<ExternalProperties>> value) { this->inline_external_properties = value; }

        /**
         * Key/value pairs that provide additional information about the log file.
         */
        std::shared_ptr<PropertyBag> get_properties() const { return properties; }
        void set_properties(std::shared_ptr<PropertyBag> value) { this->properties = value; }

        /**
         * The set of runs contained in this log file.
         */
        const std::vector<Run> & get_runs() const { return runs; }
        std::vector<Run> & get_mutable_runs() { return runs; }
        void set_runs(const std::vector<Run> & value) { this->runs = value; }

        /**
         * The SARIF format version of this log file.
         */
        const Version & get_version() const { return version; }
        Version & get_mutable_version() { return version; }
        void set_version(const Version & value) { this->version = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, SARIF::PropertyBag & x);
    void to_json(json & j, const SARIF::PropertyBag & x);

    void from_json(const json & j, SARIF::Address & x);
    void to_json(json & j, const SARIF::Address & x);

    void from_json(const json & j, SARIF::MultiformatMessageString & x);
    void to_json(json & j, const SARIF::MultiformatMessageString & x);

    void from_json(const json & j, SARIF::ArtifactContent & x);
    void to_json(json & j, const SARIF::ArtifactContent & x);

    void from_json(const json & j, SARIF::Message & x);
    void to_json(json & j, const SARIF::Message & x);

    void from_json(const json & j, SARIF::ArtifactLocation & x);
    void to_json(json & j, const SARIF::ArtifactLocation & x);

    void from_json(const json & j, SARIF::Artifact & x);
    void to_json(json & j, const SARIF::Artifact & x);

    void from_json(const json & j, SARIF::ReportingConfiguration & x);
    void to_json(json & j, const SARIF::ReportingConfiguration & x);

    void from_json(const json & j, SARIF::ToolComponentReference & x);
    void to_json(json & j, const SARIF::ToolComponentReference & x);

    void from_json(const json & j, SARIF::ReportingDescriptorReference & x);
    void to_json(json & j, const SARIF::ReportingDescriptorReference & x);

    void from_json(const json & j, SARIF::ConfigurationOverride & x);
    void to_json(json & j, const SARIF::ConfigurationOverride & x);

    void from_json(const json & j, SARIF::Region & x);
    void to_json(json & j, const SARIF::Region & x);

    void from_json(const json & j, SARIF::LogicalLocation & x);
    void to_json(json & j, const SARIF::LogicalLocation & x);

    void from_json(const json & j, SARIF::PhysicalLocation & x);
    void to_json(json & j, const SARIF::PhysicalLocation & x);

    void from_json(const json & j, SARIF::LocationRelationship & x);
    void to_json(json & j, const SARIF::LocationRelationship & x);

    void from_json(const json & j, SARIF::Location & x);
    void to_json(json & j, const SARIF::Location & x);

    void from_json(const json & j, SARIF::StackFrame & x);
    void to_json(json & j, const SARIF::StackFrame & x);

    void from_json(const json & j, SARIF::Stack & x);
    void to_json(json & j, const SARIF::Stack & x);

    void from_json(const json & j, SARIF::Exception & x);
    void to_json(json & j, const SARIF::Exception & x);

    void from_json(const json & j, SARIF::Notification & x);
    void to_json(json & j, const SARIF::Notification & x);

    void from_json(const json & j, SARIF::Invocation & x);
    void to_json(json & j, const SARIF::Invocation & x);

    void from_json(const json & j, SARIF::ReportingDescriptorRelationship & x);
    void to_json(json & j, const SARIF::ReportingDescriptorRelationship & x);

    void from_json(const json & j, SARIF::ReportingDescriptor & x);
    void to_json(json & j, const SARIF::ReportingDescriptor & x);

    void from_json(const json & j, SARIF::TranslationMetadata & x);
    void to_json(json & j, const SARIF::TranslationMetadata & x);

    void from_json(const json & j, SARIF::ToolComponent & x);
    void to_json(json & j, const SARIF::ToolComponent & x);

    void from_json(const json & j, SARIF::Tool & x);
    void to_json(json & j, const SARIF::Tool & x);

    void from_json(const json & j, SARIF::Conversion & x);
    void to_json(json & j, const SARIF::Conversion & x);

    void from_json(const json & j, SARIF::Edge & x);
    void to_json(json & j, const SARIF::Edge & x);

    void from_json(const json & j, SARIF::Node & x);
    void to_json(json & j, const SARIF::Node & x);

    void from_json(const json & j, SARIF::Graph & x);
    void to_json(json & j, const SARIF::Graph & x);

    void from_json(const json & j, SARIF::Rectangle & x);
    void to_json(json & j, const SARIF::Rectangle & x);

    void from_json(const json & j, SARIF::Attachment & x);
    void to_json(json & j, const SARIF::Attachment & x);

    void from_json(const json & j, SARIF::WebRequest & x);
    void to_json(json & j, const SARIF::WebRequest & x);

    void from_json(const json & j, SARIF::WebResponse & x);
    void to_json(json & j, const SARIF::WebResponse & x);

    void from_json(const json & j, SARIF::ThreadFlowLocation & x);
    void to_json(json & j, const SARIF::ThreadFlowLocation & x);

    void from_json(const json & j, SARIF::ThreadFlow & x);
    void to_json(json & j, const SARIF::ThreadFlow & x);

    void from_json(const json & j, SARIF::CodeFlow & x);
    void to_json(json & j, const SARIF::CodeFlow & x);

    void from_json(const json & j, SARIF::Replacement & x);
    void to_json(json & j, const SARIF::Replacement & x);

    void from_json(const json & j, SARIF::ArtifactChange & x);
    void to_json(json & j, const SARIF::ArtifactChange & x);

    void from_json(const json & j, SARIF::Fix & x);
    void to_json(json & j, const SARIF::Fix & x);

    void from_json(const json & j, SARIF::EdgeTraversal & x);
    void to_json(json & j, const SARIF::EdgeTraversal & x);

    void from_json(const json & j, SARIF::GraphTraversal & x);
    void to_json(json & j, const SARIF::GraphTraversal & x);

    void from_json(const json & j, SARIF::ResultProvenance & x);
    void to_json(json & j, const SARIF::ResultProvenance & x);

    void from_json(const json & j, SARIF::Suppression & x);
    void to_json(json & j, const SARIF::Suppression & x);

    void from_json(const json & j, SARIF::Result & x);
    void to_json(json & j, const SARIF::Result & x);

    void from_json(const json & j, SARIF::ExternalProperties & x);
    void to_json(json & j, const SARIF::ExternalProperties & x);

    void from_json(const json & j, SARIF::RunAutomationDetails & x);
    void to_json(json & j, const SARIF::RunAutomationDetails & x);

    void from_json(const json & j, SARIF::ExternalPropertyFileReference & x);
    void to_json(json & j, const SARIF::ExternalPropertyFileReference & x);

    void from_json(const json & j, SARIF::ExternalPropertyFileReferences & x);
    void to_json(json & j, const SARIF::ExternalPropertyFileReferences & x);

    void from_json(const json & j, SARIF::SpecialLocations & x);
    void to_json(json & j, const SARIF::SpecialLocations & x);

    void from_json(const json & j, SARIF::VersionControlDetails & x);
    void to_json(json & j, const SARIF::VersionControlDetails & x);

    void from_json(const json & j, SARIF::Run & x);
    void to_json(json & j, const SARIF::Run & x);

    void from_json(const json & j, SARIF::Coordinate & x);
    void to_json(json & j, const SARIF::Coordinate & x);

    void from_json(const json & j, SARIF::Role & x);
    void to_json(json & j, const SARIF::Role & x);

    void from_json(const json & j, SARIF::Level & x);
    void to_json(json & j, const SARIF::Level & x);

    void from_json(const json & j, SARIF::Content & x);
    void to_json(json & j, const SARIF::Content & x);

    void from_json(const json & j, SARIF::BaselineState & x);
    void to_json(json & j, const SARIF::BaselineState & x);

    void from_json(const json & j, SARIF::Importance & x);
    void to_json(json & j, const SARIF::Importance & x);

    void from_json(const json & j, SARIF::ResultKind & x);
    void to_json(json & j, const SARIF::ResultKind & x);

    void from_json(const json & j, SARIF::SuppressionKind & x);
    void to_json(json & j, const SARIF::SuppressionKind & x);

    void from_json(const json & j, SARIF::Status & x);
    void to_json(json & j, const SARIF::Status & x);

    void from_json(const json & j, SARIF::Version & x);
    void to_json(json & j, const SARIF::Version & x);

    void from_json(const json & j, SARIF::ColumnKind & x);
    void to_json(json & j, const SARIF::ColumnKind & x);

    inline void from_json(const json & j, SARIF::PropertyBag& x) {
        x.set_tags(SARIF::get_optional<std::vector<std::string>>(j, "tags"));
    }

    inline void to_json(json & j, const SARIF::PropertyBag & x) {
        j = json::object();
        j["tags"] = x.get_tags();
    }

    inline void from_json(const json & j, SARIF::Address& x) {
        x.set_absolute_address(SARIF::get_optional<int64_t>(j, "absoluteAddress"));
        x.set_fully_qualified_name(SARIF::get_optional<std::string>(j, "fullyQualifiedName"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_kind(SARIF::get_optional<std::string>(j, "kind"));
        x.set_length(SARIF::get_optional<int64_t>(j, "length"));
        x.set_name(SARIF::get_optional<std::string>(j, "name"));
        x.set_offset_from_parent(SARIF::get_optional<int64_t>(j, "offsetFromParent"));
        x.set_parent_index(SARIF::get_optional<int64_t>(j, "parentIndex"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_relative_address(SARIF::get_optional<int64_t>(j, "relativeAddress"));
    }

    inline void to_json(json & j, const SARIF::Address & x) {
        j = json::object();
        j["absoluteAddress"] = x.get_absolute_address();
        j["fullyQualifiedName"] = x.get_fully_qualified_name();
        j["index"] = x.get_index();
        j["kind"] = x.get_kind();
        j["length"] = x.get_length();
        j["name"] = x.get_name();
        j["offsetFromParent"] = x.get_offset_from_parent();
        j["parentIndex"] = x.get_parent_index();
        j["properties"] = x.get_properties();
        j["relativeAddress"] = x.get_relative_address();
    }

    inline void from_json(const json & j, SARIF::MultiformatMessageString& x) {
        x.set_markdown(SARIF::get_optional<std::string>(j, "markdown"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_text(j.at("text").get<std::string>());
    }

    inline void to_json(json & j, const SARIF::MultiformatMessageString & x) {
        j = json::object();
        j["markdown"] = x.get_markdown();
        j["properties"] = x.get_properties();
        j["text"] = x.get_text();
    }

    inline void from_json(const json & j, SARIF::ArtifactContent& x) {
        x.set_binary(SARIF::get_optional<std::string>(j, "binary"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_rendered(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "rendered"));
        x.set_text(SARIF::get_optional<std::string>(j, "text"));
    }

    inline void to_json(json & j, const SARIF::ArtifactContent & x) {
        j = json::object();
        j["binary"] = x.get_binary();
        j["properties"] = x.get_properties();
        j["rendered"] = x.get_rendered();
        j["text"] = x.get_text();
    }

    inline void from_json(const json & j, SARIF::Message& x) {
        x.set_arguments(SARIF::get_optional<std::vector<std::string>>(j, "arguments"));
        x.set_id(SARIF::get_optional<std::string>(j, "id"));
        x.set_markdown(SARIF::get_optional<std::string>(j, "markdown"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_text(SARIF::get_optional<std::string>(j, "text"));
    }

    inline void to_json(json & j, const SARIF::Message & x) {
        j = json::object();
        j["arguments"] = x.get_arguments();
        j["id"] = x.get_id();
        j["markdown"] = x.get_markdown();
        j["properties"] = x.get_properties();
        j["text"] = x.get_text();
    }

    inline void from_json(const json & j, SARIF::ArtifactLocation& x) {
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_uri(SARIF::get_optional<std::string>(j, "uri"));
        x.set_uri_base_id(SARIF::get_optional<std::string>(j, "uriBaseId"));
    }

    inline void to_json(json & j, const SARIF::ArtifactLocation & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["index"] = x.get_index();
        j["properties"] = x.get_properties();
        j["uri"] = x.get_uri();
        j["uriBaseId"] = x.get_uri_base_id();
    }

    inline void from_json(const json & j, SARIF::Artifact& x) {
        x.set_contents(SARIF::get_optional<SARIF::ArtifactContent>(j, "contents"));
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_encoding(SARIF::get_optional<std::string>(j, "encoding"));
        x.set_hashes(SARIF::get_optional<std::map<std::string, std::string>>(j, "hashes"));
        x.set_last_modified_time_utc(SARIF::get_optional<std::string>(j, "lastModifiedTimeUtc"));
        x.set_length(SARIF::get_optional<int64_t>(j, "length"));
        x.set_location(SARIF::get_optional<SARIF::ArtifactLocation>(j, "location"));
        x.set_mime_type(SARIF::get_optional<std::string>(j, "mimeType"));
        x.set_offset(SARIF::get_optional<int64_t>(j, "offset"));
        x.set_parent_index(SARIF::get_optional<int64_t>(j, "parentIndex"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_roles(SARIF::get_optional<std::vector<SARIF::Role>>(j, "roles"));
        x.set_source_language(SARIF::get_optional<std::string>(j, "sourceLanguage"));
    }

    inline void to_json(json & j, const SARIF::Artifact & x) {
        j = json::object();
        j["contents"] = x.get_contents();
        j["description"] = x.get_description();
        j["encoding"] = x.get_encoding();
        j["hashes"] = x.get_hashes();
        j["lastModifiedTimeUtc"] = x.get_last_modified_time_utc();
        j["length"] = x.get_length();
        j["location"] = x.get_location();
        j["mimeType"] = x.get_mime_type();
        j["offset"] = x.get_offset();
        j["parentIndex"] = x.get_parent_index();
        j["properties"] = x.get_properties();
        j["roles"] = x.get_roles();
        j["sourceLanguage"] = x.get_source_language();
    }

    inline void from_json(const json & j, SARIF::ReportingConfiguration& x) {
        x.set_enabled(SARIF::get_optional<bool>(j, "enabled"));
        x.set_level(SARIF::get_optional<SARIF::Level>(j, "level"));
        x.set_parameters(SARIF::get_optional<SARIF::PropertyBag>(j, "parameters"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_rank(SARIF::get_optional<double>(j, "rank"));
    }

    inline void to_json(json & j, const SARIF::ReportingConfiguration & x) {
        j = json::object();
        j["enabled"] = x.get_enabled();
        j["level"] = x.get_level();
        j["parameters"] = x.get_parameters();
        j["properties"] = x.get_properties();
        j["rank"] = x.get_rank();
    }

    inline void from_json(const json & j, SARIF::ToolComponentReference& x) {
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_name(SARIF::get_optional<std::string>(j, "name"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::ToolComponentReference & x) {
        j = json::object();
        j["guid"] = x.get_guid();
        j["index"] = x.get_index();
        j["name"] = x.get_name();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::ReportingDescriptorReference& x) {
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_id(SARIF::get_optional<std::string>(j, "id"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_tool_component(SARIF::get_optional<SARIF::ToolComponentReference>(j, "toolComponent"));
    }

    inline void to_json(json & j, const SARIF::ReportingDescriptorReference & x) {
        j = json::object();
        j["guid"] = x.get_guid();
        j["id"] = x.get_id();
        j["index"] = x.get_index();
        j["properties"] = x.get_properties();
        j["toolComponent"] = x.get_tool_component();
    }

    inline void from_json(const json & j, SARIF::ConfigurationOverride& x) {
        x.set_configuration(j.at("configuration").get<SARIF::ReportingConfiguration>());
        x.set_descriptor(j.at("descriptor").get<SARIF::ReportingDescriptorReference>());
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::ConfigurationOverride & x) {
        j = json::object();
        j["configuration"] = x.get_configuration();
        j["descriptor"] = x.get_descriptor();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::Region& x) {
        x.set_byte_length(SARIF::get_optional<int64_t>(j, "byteLength"));
        x.set_byte_offset(SARIF::get_optional<int64_t>(j, "byteOffset"));
        x.set_char_length(SARIF::get_optional<int64_t>(j, "charLength"));
        x.set_char_offset(SARIF::get_optional<int64_t>(j, "charOffset"));
        x.set_end_column(SARIF::get_optional<int64_t>(j, "endColumn"));
        x.set_end_line(SARIF::get_optional<int64_t>(j, "endLine"));
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_snippet(SARIF::get_optional<SARIF::ArtifactContent>(j, "snippet"));
        x.set_source_language(SARIF::get_optional<std::string>(j, "sourceLanguage"));
        x.set_start_column(SARIF::get_optional<int64_t>(j, "startColumn"));
        x.set_start_line(SARIF::get_optional<int64_t>(j, "startLine"));
    }

    inline void to_json(json & j, const SARIF::Region & x) {
        j = json::object();
        j["byteLength"] = x.get_byte_length();
        j["byteOffset"] = x.get_byte_offset();
        j["charLength"] = x.get_char_length();
        j["charOffset"] = x.get_char_offset();
        j["endColumn"] = x.get_end_column();
        j["endLine"] = x.get_end_line();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
        j["snippet"] = x.get_snippet();
        j["sourceLanguage"] = x.get_source_language();
        j["startColumn"] = x.get_start_column();
        j["startLine"] = x.get_start_line();
    }

    inline void from_json(const json & j, SARIF::LogicalLocation& x) {
        x.set_decorated_name(SARIF::get_optional<std::string>(j, "decoratedName"));
        x.set_fully_qualified_name(SARIF::get_optional<std::string>(j, "fullyQualifiedName"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_kind(SARIF::get_optional<std::string>(j, "kind"));
        x.set_name(SARIF::get_optional<std::string>(j, "name"));
        x.set_parent_index(SARIF::get_optional<int64_t>(j, "parentIndex"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::LogicalLocation & x) {
        j = json::object();
        j["decoratedName"] = x.get_decorated_name();
        j["fullyQualifiedName"] = x.get_fully_qualified_name();
        j["index"] = x.get_index();
        j["kind"] = x.get_kind();
        j["name"] = x.get_name();
        j["parentIndex"] = x.get_parent_index();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::PhysicalLocation& x) {
        x.set_address(SARIF::get_optional<SARIF::Address>(j, "address"));
        x.set_artifact_location(SARIF::get_optional<SARIF::ArtifactLocation>(j, "artifactLocation"));
        x.set_context_region(SARIF::get_optional<SARIF::Region>(j, "contextRegion"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_region(SARIF::get_optional<SARIF::Region>(j, "region"));
    }

    inline void to_json(json & j, const SARIF::PhysicalLocation & x) {
        j = json::object();
        j["address"] = x.get_address();
        j["artifactLocation"] = x.get_artifact_location();
        j["contextRegion"] = x.get_context_region();
        j["properties"] = x.get_properties();
        j["region"] = x.get_region();
    }

    inline void from_json(const json & j, SARIF::LocationRelationship& x) {
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_kinds(SARIF::get_optional<std::vector<std::string>>(j, "kinds"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_target(j.at("target").get<int64_t>());
    }

    inline void to_json(json & j, const SARIF::LocationRelationship & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["kinds"] = x.get_kinds();
        j["properties"] = x.get_properties();
        j["target"] = x.get_target();
    }

    inline void from_json(const json & j, SARIF::Location& x) {
        x.set_annotations(SARIF::get_optional<std::vector<SARIF::Region>>(j, "annotations"));
        x.set_id(SARIF::get_optional<int64_t>(j, "id"));
        x.set_logical_locations(SARIF::get_optional<std::vector<SARIF::LogicalLocation>>(j, "logicalLocations"));
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_physical_location(SARIF::get_optional<SARIF::PhysicalLocation>(j, "physicalLocation"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_relationships(SARIF::get_optional<std::vector<SARIF::LocationRelationship>>(j, "relationships"));
    }

    inline void to_json(json & j, const SARIF::Location & x) {
        j = json::object();
        j["annotations"] = x.get_annotations();
        j["id"] = x.get_id();
        j["logicalLocations"] = x.get_logical_locations();
        j["message"] = x.get_message();
        j["physicalLocation"] = x.get_physical_location();
        j["properties"] = x.get_properties();
        j["relationships"] = x.get_relationships();
    }

    inline void from_json(const json & j, SARIF::StackFrame& x) {
        x.set_location(SARIF::get_optional<SARIF::Location>(j, "location"));
        x.set_stack_frame_module(SARIF::get_optional<std::string>(j, "module"));
        x.set_parameters(SARIF::get_optional<std::vector<std::string>>(j, "parameters"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_thread_id(SARIF::get_optional<int64_t>(j, "threadId"));
    }

    inline void to_json(json & j, const SARIF::StackFrame & x) {
        j = json::object();
        j["location"] = x.get_location();
        j["module"] = x.get_stack_frame_module();
        j["parameters"] = x.get_parameters();
        j["properties"] = x.get_properties();
        j["threadId"] = x.get_thread_id();
    }

    inline void from_json(const json & j, SARIF::Stack& x) {
        x.set_frames(j.at("frames").get<std::vector<SARIF::StackFrame>>());
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::Stack & x) {
        j = json::object();
        j["frames"] = x.get_frames();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::Exception& x) {
        x.set_inner_exceptions(SARIF::get_optional<std::vector<SARIF::Exception>>(j, "innerExceptions"));
        x.set_kind(SARIF::get_optional<std::string>(j, "kind"));
        x.set_message(SARIF::get_optional<std::string>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_stack(SARIF::get_optional<SARIF::Stack>(j, "stack"));
    }

    inline void to_json(json & j, const SARIF::Exception & x) {
        j = json::object();
        j["innerExceptions"] = x.get_inner_exceptions();
        j["kind"] = x.get_kind();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
        j["stack"] = x.get_stack();
    }

    inline void from_json(const json & j, SARIF::Notification& x) {
        x.set_associated_rule(SARIF::get_optional<SARIF::ReportingDescriptorReference>(j, "associatedRule"));
        x.set_descriptor(SARIF::get_optional<SARIF::ReportingDescriptorReference>(j, "descriptor"));
        x.set_exception(SARIF::get_optional<SARIF::Exception>(j, "exception"));
        x.set_level(SARIF::get_optional<SARIF::Level>(j, "level"));
        x.set_locations(SARIF::get_optional<std::vector<SARIF::Location>>(j, "locations"));
        x.set_message(j.at("message").get<SARIF::Message>());
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_thread_id(SARIF::get_optional<int64_t>(j, "threadId"));
        x.set_time_utc(SARIF::get_optional<std::string>(j, "timeUtc"));
    }

    inline void to_json(json & j, const SARIF::Notification & x) {
        j = json::object();
        j["associatedRule"] = x.get_associated_rule();
        j["descriptor"] = x.get_descriptor();
        j["exception"] = x.get_exception();
        j["level"] = x.get_level();
        j["locations"] = x.get_locations();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
        j["threadId"] = x.get_thread_id();
        j["timeUtc"] = x.get_time_utc();
    }

    inline void from_json(const json & j, SARIF::Invocation& x) {
        x.set_account(SARIF::get_optional<std::string>(j, "account"));
        x.set_arguments(SARIF::get_optional<std::vector<std::string>>(j, "arguments"));
        x.set_command_line(SARIF::get_optional<std::string>(j, "commandLine"));
        x.set_end_time_utc(SARIF::get_optional<std::string>(j, "endTimeUtc"));
        x.set_environment_variables(SARIF::get_optional<std::map<std::string, std::string>>(j, "environmentVariables"));
        x.set_executable_location(SARIF::get_optional<SARIF::ArtifactLocation>(j, "executableLocation"));
        x.set_execution_successful(j.at("executionSuccessful").get<bool>());
        x.set_exit_code(SARIF::get_optional<int64_t>(j, "exitCode"));
        x.set_exit_code_description(SARIF::get_optional<std::string>(j, "exitCodeDescription"));
        x.set_exit_signal_name(SARIF::get_optional<std::string>(j, "exitSignalName"));
        x.set_exit_signal_number(SARIF::get_optional<int64_t>(j, "exitSignalNumber"));
        x.set_machine(SARIF::get_optional<std::string>(j, "machine"));
        x.set_notification_configuration_overrides(SARIF::get_optional<std::vector<SARIF::ConfigurationOverride>>(j, "notificationConfigurationOverrides"));
        x.set_process_id(SARIF::get_optional<int64_t>(j, "processId"));
        x.set_process_start_failure_message(SARIF::get_optional<std::string>(j, "processStartFailureMessage"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_response_files(SARIF::get_optional<std::vector<SARIF::ArtifactLocation>>(j, "responseFiles"));
        x.set_rule_configuration_overrides(SARIF::get_optional<std::vector<SARIF::ConfigurationOverride>>(j, "ruleConfigurationOverrides"));
        x.set_start_time_utc(SARIF::get_optional<std::string>(j, "startTimeUtc"));
        x.set_stderr(SARIF::get_optional<SARIF::ArtifactLocation>(j, "stderr"));
        x.set_stdin(SARIF::get_optional<SARIF::ArtifactLocation>(j, "stdin"));
        x.set_stdout(SARIF::get_optional<SARIF::ArtifactLocation>(j, "stdout"));
        x.set_stdout_stderr(SARIF::get_optional<SARIF::ArtifactLocation>(j, "stdoutStderr"));
        x.set_tool_configuration_notifications(SARIF::get_optional<std::vector<SARIF::Notification>>(j, "toolConfigurationNotifications"));
        x.set_tool_execution_notifications(SARIF::get_optional<std::vector<SARIF::Notification>>(j, "toolExecutionNotifications"));
        x.set_working_directory(SARIF::get_optional<SARIF::ArtifactLocation>(j, "workingDirectory"));
    }

    inline void to_json(json & j, const SARIF::Invocation & x) {
        j = json::object();
        j["account"] = x.get_account();
        j["arguments"] = x.get_arguments();
        j["commandLine"] = x.get_command_line();
        j["endTimeUtc"] = x.get_end_time_utc();
        j["environmentVariables"] = x.get_environment_variables();
        j["executableLocation"] = x.get_executable_location();
        j["executionSuccessful"] = x.get_execution_successful();
        j["exitCode"] = x.get_exit_code();
        j["exitCodeDescription"] = x.get_exit_code_description();
        j["exitSignalName"] = x.get_exit_signal_name();
        j["exitSignalNumber"] = x.get_exit_signal_number();
        j["machine"] = x.get_machine();
        j["notificationConfigurationOverrides"] = x.get_notification_configuration_overrides();
        j["processId"] = x.get_process_id();
        j["processStartFailureMessage"] = x.get_process_start_failure_message();
        j["properties"] = x.get_properties();
        j["responseFiles"] = x.get_response_files();
        j["ruleConfigurationOverrides"] = x.get_rule_configuration_overrides();
        j["startTimeUtc"] = x.get_start_time_utc();
        j["stderr"] = x.get_stderr();
        j["stdin"] = x.get_stdin();
        j["stdout"] = x.get_stdout();
        j["stdoutStderr"] = x.get_stdout_stderr();
        j["toolConfigurationNotifications"] = x.get_tool_configuration_notifications();
        j["toolExecutionNotifications"] = x.get_tool_execution_notifications();
        j["workingDirectory"] = x.get_working_directory();
    }

    inline void from_json(const json & j, SARIF::ReportingDescriptorRelationship& x) {
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_kinds(SARIF::get_optional<std::vector<std::string>>(j, "kinds"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_target(j.at("target").get<SARIF::ReportingDescriptorReference>());
    }

    inline void to_json(json & j, const SARIF::ReportingDescriptorRelationship & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["kinds"] = x.get_kinds();
        j["properties"] = x.get_properties();
        j["target"] = x.get_target();
    }

    inline void from_json(const json & j, SARIF::ReportingDescriptor& x) {
        x.set_default_configuration(SARIF::get_optional<SARIF::ReportingConfiguration>(j, "defaultConfiguration"));
        x.set_deprecated_guids(SARIF::get_optional<std::vector<std::string>>(j, "deprecatedGuids"));
        x.set_deprecated_ids(SARIF::get_optional<std::vector<std::string>>(j, "deprecatedIds"));
        x.set_deprecated_names(SARIF::get_optional<std::vector<std::string>>(j, "deprecatedNames"));
        x.set_full_description(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "fullDescription"));
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_help(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "help"));
        x.set_help_uri(SARIF::get_optional<std::string>(j, "helpUri"));
        x.set_id(j.at("id").get<std::string>());
        x.set_message_strings(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "messageStrings"));
        x.set_name(SARIF::get_optional<std::string>(j, "name"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_relationships(SARIF::get_optional<std::vector<SARIF::ReportingDescriptorRelationship>>(j, "relationships"));
        x.set_short_description(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "shortDescription"));
    }

    inline void to_json(json & j, const SARIF::ReportingDescriptor & x) {
        j = json::object();
        j["defaultConfiguration"] = x.get_default_configuration();
        j["deprecatedGuids"] = x.get_deprecated_guids();
        j["deprecatedIds"] = x.get_deprecated_ids();
        j["deprecatedNames"] = x.get_deprecated_names();
        j["fullDescription"] = x.get_full_description();
        j["guid"] = x.get_guid();
        j["help"] = x.get_help();
        j["helpUri"] = x.get_help_uri();
        j["id"] = x.get_id();
        j["messageStrings"] = x.get_message_strings();
        j["name"] = x.get_name();
        j["properties"] = x.get_properties();
        j["relationships"] = x.get_relationships();
        j["shortDescription"] = x.get_short_description();
    }

    inline void from_json(const json & j, SARIF::TranslationMetadata& x) {
        x.set_download_uri(SARIF::get_optional<std::string>(j, "downloadUri"));
        x.set_full_description(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "fullDescription"));
        x.set_full_name(SARIF::get_optional<std::string>(j, "fullName"));
        x.set_information_uri(SARIF::get_optional<std::string>(j, "informationUri"));
        x.set_name(j.at("name").get<std::string>());
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_short_description(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "shortDescription"));
    }

    inline void to_json(json & j, const SARIF::TranslationMetadata & x) {
        j = json::object();
        j["downloadUri"] = x.get_download_uri();
        j["fullDescription"] = x.get_full_description();
        j["fullName"] = x.get_full_name();
        j["informationUri"] = x.get_information_uri();
        j["name"] = x.get_name();
        j["properties"] = x.get_properties();
        j["shortDescription"] = x.get_short_description();
    }

    inline void from_json(const json & j, SARIF::ToolComponent& x) {
        x.set_associated_component(SARIF::get_optional<SARIF::ToolComponentReference>(j, "associatedComponent"));
        x.set_contents(SARIF::get_optional<std::vector<SARIF::Content>>(j, "contents"));
        x.set_dotted_quad_file_version(SARIF::get_optional<std::string>(j, "dottedQuadFileVersion"));
        x.set_download_uri(SARIF::get_optional<std::string>(j, "downloadUri"));
        x.set_full_description(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "fullDescription"));
        x.set_full_name(SARIF::get_optional<std::string>(j, "fullName"));
        x.set_global_message_strings(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "globalMessageStrings"));
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_information_uri(SARIF::get_optional<std::string>(j, "informationUri"));
        x.set_is_comprehensive(SARIF::get_optional<bool>(j, "isComprehensive"));
        x.set_language(SARIF::get_optional<std::string>(j, "language"));
        x.set_localized_data_semantic_version(SARIF::get_optional<std::string>(j, "localizedDataSemanticVersion"));
        x.set_locations(SARIF::get_optional<std::vector<SARIF::ArtifactLocation>>(j, "locations"));
        x.set_minimum_required_localized_data_semantic_version(SARIF::get_optional<std::string>(j, "minimumRequiredLocalizedDataSemanticVersion"));
        x.set_name(j.at("name").get<std::string>());
        x.set_notifications(SARIF::get_optional<std::vector<SARIF::ReportingDescriptor>>(j, "notifications"));
        x.set_organization(SARIF::get_optional<std::string>(j, "organization"));
        x.set_product(SARIF::get_optional<std::string>(j, "product"));
        x.set_product_suite(SARIF::get_optional<std::string>(j, "productSuite"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_release_date_utc(SARIF::get_optional<std::string>(j, "releaseDateUtc"));
        x.set_rules(SARIF::get_optional<std::vector<SARIF::ReportingDescriptor>>(j, "rules"));
        x.set_semantic_version(SARIF::get_optional<std::string>(j, "semanticVersion"));
        x.set_short_description(SARIF::get_optional<SARIF::MultiformatMessageString>(j, "shortDescription"));
        x.set_supported_taxonomies(SARIF::get_optional<std::vector<SARIF::ToolComponentReference>>(j, "supportedTaxonomies"));
        x.set_taxa(SARIF::get_optional<std::vector<SARIF::ReportingDescriptor>>(j, "taxa"));
        x.set_translation_metadata(SARIF::get_optional<SARIF::TranslationMetadata>(j, "translationMetadata"));
        x.set_version(SARIF::get_optional<std::string>(j, "version"));
    }

    inline void to_json(json & j, const SARIF::ToolComponent & x) {
        j = json::object();
        j["associatedComponent"] = x.get_associated_component();
        j["contents"] = x.get_contents();
        j["dottedQuadFileVersion"] = x.get_dotted_quad_file_version();
        j["downloadUri"] = x.get_download_uri();
        j["fullDescription"] = x.get_full_description();
        j["fullName"] = x.get_full_name();
        j["globalMessageStrings"] = x.get_global_message_strings();
        j["guid"] = x.get_guid();
        j["informationUri"] = x.get_information_uri();
        j["isComprehensive"] = x.get_is_comprehensive();
        j["language"] = x.get_language();
        j["localizedDataSemanticVersion"] = x.get_localized_data_semantic_version();
        j["locations"] = x.get_locations();
        j["minimumRequiredLocalizedDataSemanticVersion"] = x.get_minimum_required_localized_data_semantic_version();
        j["name"] = x.get_name();
        j["notifications"] = x.get_notifications();
        j["organization"] = x.get_organization();
        j["product"] = x.get_product();
        j["productSuite"] = x.get_product_suite();
        j["properties"] = x.get_properties();
        j["releaseDateUtc"] = x.get_release_date_utc();
        j["rules"] = x.get_rules();
        j["semanticVersion"] = x.get_semantic_version();
        j["shortDescription"] = x.get_short_description();
        j["supportedTaxonomies"] = x.get_supported_taxonomies();
        j["taxa"] = x.get_taxa();
        j["translationMetadata"] = x.get_translation_metadata();
        j["version"] = x.get_version();
    }

    inline void from_json(const json & j, SARIF::Tool& x) {
        x.set_driver(j.at("driver").get<SARIF::ToolComponent>());
        x.set_extensions(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "extensions"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::Tool & x) {
        j = json::object();
        j["driver"] = x.get_driver();
        j["extensions"] = x.get_extensions();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::Conversion& x) {
        x.set_analysis_tool_log_files(SARIF::get_optional<std::vector<SARIF::ArtifactLocation>>(j, "analysisToolLogFiles"));
        x.set_invocation(SARIF::get_optional<SARIF::Invocation>(j, "invocation"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_tool(j.at("tool").get<SARIF::Tool>());
    }

    inline void to_json(json & j, const SARIF::Conversion & x) {
        j = json::object();
        j["analysisToolLogFiles"] = x.get_analysis_tool_log_files();
        j["invocation"] = x.get_invocation();
        j["properties"] = x.get_properties();
        j["tool"] = x.get_tool();
    }

    inline void from_json(const json & j, SARIF::Edge& x) {
        x.set_id(j.at("id").get<std::string>());
        x.set_label(SARIF::get_optional<SARIF::Message>(j, "label"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_source_node_id(j.at("sourceNodeId").get<std::string>());
        x.set_target_node_id(j.at("targetNodeId").get<std::string>());
    }

    inline void to_json(json & j, const SARIF::Edge & x) {
        j = json::object();
        j["id"] = x.get_id();
        j["label"] = x.get_label();
        j["properties"] = x.get_properties();
        j["sourceNodeId"] = x.get_source_node_id();
        j["targetNodeId"] = x.get_target_node_id();
    }

    inline void from_json(const json & j, SARIF::Node& x) {
        x.set_children(SARIF::get_optional<std::vector<SARIF::Node>>(j, "children"));
        x.set_id(j.at("id").get<std::string>());
        x.set_label(SARIF::get_optional<SARIF::Message>(j, "label"));
        x.set_location(SARIF::get_optional<SARIF::Location>(j, "location"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::Node & x) {
        j = json::object();
        j["children"] = x.get_children();
        j["id"] = x.get_id();
        j["label"] = x.get_label();
        j["location"] = x.get_location();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::Graph& x) {
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_edges(SARIF::get_optional<std::vector<SARIF::Edge>>(j, "edges"));
        x.set_nodes(SARIF::get_optional<std::vector<SARIF::Node>>(j, "nodes"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::Graph & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["edges"] = x.get_edges();
        j["nodes"] = x.get_nodes();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::Rectangle& x) {
        x.set_bottom(SARIF::get_optional<double>(j, "bottom"));
        x.set_left(SARIF::get_optional<double>(j, "left"));
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_right(SARIF::get_optional<double>(j, "right"));
        x.set_top(SARIF::get_optional<double>(j, "top"));
    }

    inline void to_json(json & j, const SARIF::Rectangle & x) {
        j = json::object();
        j["bottom"] = x.get_bottom();
        j["left"] = x.get_left();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
        j["right"] = x.get_right();
        j["top"] = x.get_top();
    }

    inline void from_json(const json & j, SARIF::Attachment& x) {
        x.set_artifact_location(j.at("artifactLocation").get<SARIF::ArtifactLocation>());
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_rectangles(SARIF::get_optional<std::vector<SARIF::Rectangle>>(j, "rectangles"));
        x.set_regions(SARIF::get_optional<std::vector<SARIF::Region>>(j, "regions"));
    }

    inline void to_json(json & j, const SARIF::Attachment & x) {
        j = json::object();
        j["artifactLocation"] = x.get_artifact_location();
        j["description"] = x.get_description();
        j["properties"] = x.get_properties();
        j["rectangles"] = x.get_rectangles();
        j["regions"] = x.get_regions();
    }

    inline void from_json(const json & j, SARIF::WebRequest& x) {
        x.set_body(SARIF::get_optional<SARIF::ArtifactContent>(j, "body"));
        x.set_headers(SARIF::get_optional<std::map<std::string, std::string>>(j, "headers"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_method(SARIF::get_optional<std::string>(j, "method"));
        x.set_parameters(SARIF::get_optional<std::map<std::string, std::string>>(j, "parameters"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_protocol(SARIF::get_optional<std::string>(j, "protocol"));
        x.set_target(SARIF::get_optional<std::string>(j, "target"));
        x.set_version(SARIF::get_optional<std::string>(j, "version"));
    }

    inline void to_json(json & j, const SARIF::WebRequest & x) {
        j = json::object();
        j["body"] = x.get_body();
        j["headers"] = x.get_headers();
        j["index"] = x.get_index();
        j["method"] = x.get_method();
        j["parameters"] = x.get_parameters();
        j["properties"] = x.get_properties();
        j["protocol"] = x.get_protocol();
        j["target"] = x.get_target();
        j["version"] = x.get_version();
    }

    inline void from_json(const json & j, SARIF::WebResponse& x) {
        x.set_body(SARIF::get_optional<SARIF::ArtifactContent>(j, "body"));
        x.set_headers(SARIF::get_optional<std::map<std::string, std::string>>(j, "headers"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_no_response_received(SARIF::get_optional<bool>(j, "noResponseReceived"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_protocol(SARIF::get_optional<std::string>(j, "protocol"));
        x.set_reason_phrase(SARIF::get_optional<std::string>(j, "reasonPhrase"));
        x.set_status_code(SARIF::get_optional<int64_t>(j, "statusCode"));
        x.set_version(SARIF::get_optional<std::string>(j, "version"));
    }

    inline void to_json(json & j, const SARIF::WebResponse & x) {
        j = json::object();
        j["body"] = x.get_body();
        j["headers"] = x.get_headers();
        j["index"] = x.get_index();
        j["noResponseReceived"] = x.get_no_response_received();
        j["properties"] = x.get_properties();
        j["protocol"] = x.get_protocol();
        j["reasonPhrase"] = x.get_reason_phrase();
        j["statusCode"] = x.get_status_code();
        j["version"] = x.get_version();
    }

    inline void from_json(const json & j, SARIF::ThreadFlowLocation& x) {
        x.set_execution_order(SARIF::get_optional<int64_t>(j, "executionOrder"));
        x.set_execution_time_utc(SARIF::get_optional<std::string>(j, "executionTimeUtc"));
        x.set_importance(SARIF::get_optional<SARIF::Importance>(j, "importance"));
        x.set_index(SARIF::get_optional<int64_t>(j, "index"));
        x.set_kinds(SARIF::get_optional<std::vector<std::string>>(j, "kinds"));
        x.set_location(SARIF::get_optional<SARIF::Location>(j, "location"));
        x.set_thread_flow_location_module(SARIF::get_optional<std::string>(j, "module"));
        x.set_nesting_level(SARIF::get_optional<int64_t>(j, "nestingLevel"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_stack(SARIF::get_optional<SARIF::Stack>(j, "stack"));
        x.set_state(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "state"));
        x.set_taxa(SARIF::get_optional<std::vector<SARIF::ReportingDescriptorReference>>(j, "taxa"));
        x.set_web_request(SARIF::get_optional<SARIF::WebRequest>(j, "webRequest"));
        x.set_web_response(SARIF::get_optional<SARIF::WebResponse>(j, "webResponse"));
    }

    inline void to_json(json & j, const SARIF::ThreadFlowLocation & x) {
        j = json::object();
        j["executionOrder"] = x.get_execution_order();
        j["executionTimeUtc"] = x.get_execution_time_utc();
        j["importance"] = x.get_importance();
        j["index"] = x.get_index();
        j["kinds"] = x.get_kinds();
        j["location"] = x.get_location();
        j["module"] = x.get_thread_flow_location_module();
        j["nestingLevel"] = x.get_nesting_level();
        j["properties"] = x.get_properties();
        j["stack"] = x.get_stack();
        j["state"] = x.get_state();
        j["taxa"] = x.get_taxa();
        j["webRequest"] = x.get_web_request();
        j["webResponse"] = x.get_web_response();
    }

    inline void from_json(const json & j, SARIF::ThreadFlow& x) {
        x.set_id(SARIF::get_optional<std::string>(j, "id"));
        x.set_immutable_state(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "immutableState"));
        x.set_initial_state(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "initialState"));
        x.set_locations(j.at("locations").get<std::vector<SARIF::ThreadFlowLocation>>());
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::ThreadFlow & x) {
        j = json::object();
        j["id"] = x.get_id();
        j["immutableState"] = x.get_immutable_state();
        j["initialState"] = x.get_initial_state();
        j["locations"] = x.get_locations();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::CodeFlow& x) {
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_thread_flows(j.at("threadFlows").get<std::vector<SARIF::ThreadFlow>>());
    }

    inline void to_json(json & j, const SARIF::CodeFlow & x) {
        j = json::object();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
        j["threadFlows"] = x.get_thread_flows();
    }

    inline void from_json(const json & j, SARIF::Replacement& x) {
        x.set_deleted_region(j.at("deletedRegion").get<SARIF::Region>());
        x.set_inserted_content(SARIF::get_optional<SARIF::ArtifactContent>(j, "insertedContent"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::Replacement & x) {
        j = json::object();
        j["deletedRegion"] = x.get_deleted_region();
        j["insertedContent"] = x.get_inserted_content();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::ArtifactChange& x) {
        x.set_artifact_location(j.at("artifactLocation").get<SARIF::ArtifactLocation>());
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_replacements(j.at("replacements").get<std::vector<SARIF::Replacement>>());
    }

    inline void to_json(json & j, const SARIF::ArtifactChange & x) {
        j = json::object();
        j["artifactLocation"] = x.get_artifact_location();
        j["properties"] = x.get_properties();
        j["replacements"] = x.get_replacements();
    }

    inline void from_json(const json & j, SARIF::Fix& x) {
        x.set_artifact_changes(j.at("artifactChanges").get<std::vector<SARIF::ArtifactChange>>());
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::Fix & x) {
        j = json::object();
        j["artifactChanges"] = x.get_artifact_changes();
        j["description"] = x.get_description();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::EdgeTraversal& x) {
        x.set_edge_id(j.at("edgeId").get<std::string>());
        x.set_final_state(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "finalState"));
        x.set_message(SARIF::get_optional<SARIF::Message>(j, "message"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_step_over_edge_count(SARIF::get_optional<int64_t>(j, "stepOverEdgeCount"));
    }

    inline void to_json(json & j, const SARIF::EdgeTraversal & x) {
        j = json::object();
        j["edgeId"] = x.get_edge_id();
        j["finalState"] = x.get_final_state();
        j["message"] = x.get_message();
        j["properties"] = x.get_properties();
        j["stepOverEdgeCount"] = x.get_step_over_edge_count();
    }

    inline void from_json(const json & j, SARIF::GraphTraversal& x) {
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_edge_traversals(SARIF::get_optional<std::vector<SARIF::EdgeTraversal>>(j, "edgeTraversals"));
        x.set_immutable_state(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "immutableState"));
        x.set_initial_state(SARIF::get_optional<std::map<std::string, SARIF::MultiformatMessageString>>(j, "initialState"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_result_graph_index(SARIF::get_optional<int64_t>(j, "resultGraphIndex"));
        x.set_run_graph_index(SARIF::get_optional<int64_t>(j, "runGraphIndex"));
    }

    inline void to_json(json & j, const SARIF::GraphTraversal & x) {
        j = json::object();
        j["description"] = x.get_description();
        j["edgeTraversals"] = x.get_edge_traversals();
        j["immutableState"] = x.get_immutable_state();
        j["initialState"] = x.get_initial_state();
        j["properties"] = x.get_properties();
        j["resultGraphIndex"] = x.get_result_graph_index();
        j["runGraphIndex"] = x.get_run_graph_index();
    }

    inline void from_json(const json & j, SARIF::ResultProvenance& x) {
        x.set_conversion_sources(SARIF::get_optional<std::vector<SARIF::PhysicalLocation>>(j, "conversionSources"));
        x.set_first_detection_run_guid(SARIF::get_optional<std::string>(j, "firstDetectionRunGuid"));
        x.set_first_detection_time_utc(SARIF::get_optional<std::string>(j, "firstDetectionTimeUtc"));
        x.set_invocation_index(SARIF::get_optional<int64_t>(j, "invocationIndex"));
        x.set_last_detection_run_guid(SARIF::get_optional<std::string>(j, "lastDetectionRunGuid"));
        x.set_last_detection_time_utc(SARIF::get_optional<std::string>(j, "lastDetectionTimeUtc"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::ResultProvenance & x) {
        j = json::object();
        j["conversionSources"] = x.get_conversion_sources();
        j["firstDetectionRunGuid"] = x.get_first_detection_run_guid();
        j["firstDetectionTimeUtc"] = x.get_first_detection_time_utc();
        j["invocationIndex"] = x.get_invocation_index();
        j["lastDetectionRunGuid"] = x.get_last_detection_run_guid();
        j["lastDetectionTimeUtc"] = x.get_last_detection_time_utc();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::Suppression& x) {
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_justification(SARIF::get_optional<std::string>(j, "justification"));
        x.set_kind(j.at("kind").get<SARIF::SuppressionKind>());
        x.set_location(SARIF::get_optional<SARIF::Location>(j, "location"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_status(SARIF::get_optional<SARIF::Status>(j, "status"));
    }

    inline void to_json(json & j, const SARIF::Suppression & x) {
        j = json::object();
        j["guid"] = x.get_guid();
        j["justification"] = x.get_justification();
        j["kind"] = x.get_kind();
        j["location"] = x.get_location();
        j["properties"] = x.get_properties();
        j["status"] = x.get_status();
    }

    inline void from_json(const json & j, SARIF::Result& x) {
        x.set_analysis_target(SARIF::get_optional<SARIF::ArtifactLocation>(j, "analysisTarget"));
        x.set_attachments(SARIF::get_optional<std::vector<SARIF::Attachment>>(j, "attachments"));
        x.set_baseline_state(SARIF::get_optional<SARIF::BaselineState>(j, "baselineState"));
        x.set_code_flows(SARIF::get_optional<std::vector<SARIF::CodeFlow>>(j, "codeFlows"));
        x.set_correlation_guid(SARIF::get_optional<std::string>(j, "correlationGuid"));
        x.set_fingerprints(SARIF::get_optional<std::map<std::string, std::string>>(j, "fingerprints"));
        x.set_fixes(SARIF::get_optional<std::vector<SARIF::Fix>>(j, "fixes"));
        x.set_graphs(SARIF::get_optional<std::vector<SARIF::Graph>>(j, "graphs"));
        x.set_graph_traversals(SARIF::get_optional<std::vector<SARIF::GraphTraversal>>(j, "graphTraversals"));
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_hosted_viewer_uri(SARIF::get_optional<std::string>(j, "hostedViewerUri"));
        x.set_kind(SARIF::get_optional<SARIF::ResultKind>(j, "kind"));
        x.set_level(SARIF::get_optional<SARIF::Level>(j, "level"));
        x.set_locations(SARIF::get_optional<std::vector<SARIF::Location>>(j, "locations"));
        x.set_message(j.at("message").get<SARIF::Message>());
        x.set_occurrence_count(SARIF::get_optional<int64_t>(j, "occurrenceCount"));
        x.set_partial_fingerprints(SARIF::get_optional<std::map<std::string, std::string>>(j, "partialFingerprints"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_provenance(SARIF::get_optional<SARIF::ResultProvenance>(j, "provenance"));
        x.set_rank(SARIF::get_optional<double>(j, "rank"));
        x.set_related_locations(SARIF::get_optional<std::vector<SARIF::Location>>(j, "relatedLocations"));
        x.set_rule(SARIF::get_optional<SARIF::ReportingDescriptorReference>(j, "rule"));
        x.set_rule_id(SARIF::get_optional<std::string>(j, "ruleId"));
        x.set_rule_index(SARIF::get_optional<int64_t>(j, "ruleIndex"));
        x.set_stacks(SARIF::get_optional<std::vector<SARIF::Stack>>(j, "stacks"));
        x.set_suppressions(SARIF::get_optional<std::vector<SARIF::Suppression>>(j, "suppressions"));
        x.set_taxa(SARIF::get_optional<std::vector<SARIF::ReportingDescriptorReference>>(j, "taxa"));
        x.set_web_request(SARIF::get_optional<SARIF::WebRequest>(j, "webRequest"));
        x.set_web_response(SARIF::get_optional<SARIF::WebResponse>(j, "webResponse"));
        x.set_work_item_uris(SARIF::get_optional<std::vector<std::string>>(j, "workItemUris"));
    }

    inline void to_json(json & j, const SARIF::Result & x) {
        j = json::object();
        j["analysisTarget"] = x.get_analysis_target();
        j["attachments"] = x.get_attachments();
        j["baselineState"] = x.get_baseline_state();
        j["codeFlows"] = x.get_code_flows();
        j["correlationGuid"] = x.get_correlation_guid();
        j["fingerprints"] = x.get_fingerprints();
        j["fixes"] = x.get_fixes();
        j["graphs"] = x.get_graphs();
        j["graphTraversals"] = x.get_graph_traversals();
        j["guid"] = x.get_guid();
        j["hostedViewerUri"] = x.get_hosted_viewer_uri();
        j["kind"] = x.get_kind();
        j["level"] = x.get_level();
        j["locations"] = x.get_locations();
        j["message"] = x.get_message();
        j["occurrenceCount"] = x.get_occurrence_count();
        j["partialFingerprints"] = x.get_partial_fingerprints();
        j["properties"] = x.get_properties();
        j["provenance"] = x.get_provenance();
        j["rank"] = x.get_rank();
        j["relatedLocations"] = x.get_related_locations();
        j["rule"] = x.get_rule();
        j["ruleId"] = x.get_rule_id();
        j["ruleIndex"] = x.get_rule_index();
        j["stacks"] = x.get_stacks();
        j["suppressions"] = x.get_suppressions();
        j["taxa"] = x.get_taxa();
        j["webRequest"] = x.get_web_request();
        j["webResponse"] = x.get_web_response();
        j["workItemUris"] = x.get_work_item_uris();
    }

    inline void from_json(const json & j, SARIF::ExternalProperties& x) {
        x.set_addresses(SARIF::get_optional<std::vector<SARIF::Address>>(j, "addresses"));
        x.set_artifacts(SARIF::get_optional<std::vector<SARIF::Artifact>>(j, "artifacts"));
        x.set_conversion(SARIF::get_optional<SARIF::Conversion>(j, "conversion"));
        x.set_driver(SARIF::get_optional<SARIF::ToolComponent>(j, "driver"));
        x.set_extensions(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "extensions"));
        x.set_externalized_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "externalizedProperties"));
        x.set_graphs(SARIF::get_optional<std::vector<SARIF::Graph>>(j, "graphs"));
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_invocations(SARIF::get_optional<std::vector<SARIF::Invocation>>(j, "invocations"));
        x.set_logical_locations(SARIF::get_optional<std::vector<SARIF::LogicalLocation>>(j, "logicalLocations"));
        x.set_policies(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "policies"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_results(SARIF::get_optional<std::vector<SARIF::Result>>(j, "results"));
        x.set_run_guid(SARIF::get_optional<std::string>(j, "runGuid"));
        x.set_schema(SARIF::get_optional<std::string>(j, "schema"));
        x.set_taxonomies(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "taxonomies"));
        x.set_thread_flow_locations(SARIF::get_optional<std::vector<SARIF::ThreadFlowLocation>>(j, "threadFlowLocations"));
        x.set_translations(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "translations"));
        x.set_version(SARIF::get_optional<SARIF::Version>(j, "version"));
        x.set_web_requests(SARIF::get_optional<std::vector<SARIF::WebRequest>>(j, "webRequests"));
        x.set_web_responses(SARIF::get_optional<std::vector<SARIF::WebResponse>>(j, "webResponses"));
    }

    inline void to_json(json & j, const SARIF::ExternalProperties & x) {
        j = json::object();
        j["addresses"] = x.get_addresses();
        j["artifacts"] = x.get_artifacts();
        j["conversion"] = x.get_conversion();
        j["driver"] = x.get_driver();
        j["extensions"] = x.get_extensions();
        j["externalizedProperties"] = x.get_externalized_properties();
        j["graphs"] = x.get_graphs();
        j["guid"] = x.get_guid();
        j["invocations"] = x.get_invocations();
        j["logicalLocations"] = x.get_logical_locations();
        j["policies"] = x.get_policies();
        j["properties"] = x.get_properties();
        j["results"] = x.get_results();
        j["runGuid"] = x.get_run_guid();
        j["schema"] = x.get_schema();
        j["taxonomies"] = x.get_taxonomies();
        j["threadFlowLocations"] = x.get_thread_flow_locations();
        j["translations"] = x.get_translations();
        j["version"] = x.get_version();
        j["webRequests"] = x.get_web_requests();
        j["webResponses"] = x.get_web_responses();
    }

    inline void from_json(const json & j, SARIF::RunAutomationDetails& x) {
        x.set_correlation_guid(SARIF::get_optional<std::string>(j, "correlationGuid"));
        x.set_description(SARIF::get_optional<SARIF::Message>(j, "description"));
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_id(SARIF::get_optional<std::string>(j, "id"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::RunAutomationDetails & x) {
        j = json::object();
        j["correlationGuid"] = x.get_correlation_guid();
        j["description"] = x.get_description();
        j["guid"] = x.get_guid();
        j["id"] = x.get_id();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::ExternalPropertyFileReference& x) {
        x.set_guid(SARIF::get_optional<std::string>(j, "guid"));
        x.set_item_count(SARIF::get_optional<int64_t>(j, "itemCount"));
        x.set_location(SARIF::get_optional<SARIF::ArtifactLocation>(j, "location"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::ExternalPropertyFileReference & x) {
        j = json::object();
        j["guid"] = x.get_guid();
        j["itemCount"] = x.get_item_count();
        j["location"] = x.get_location();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::ExternalPropertyFileReferences& x) {
        x.set_addresses(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "addresses"));
        x.set_artifacts(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "artifacts"));
        x.set_conversion(SARIF::get_optional<SARIF::ExternalPropertyFileReference>(j, "conversion"));
        x.set_driver(SARIF::get_optional<SARIF::ExternalPropertyFileReference>(j, "driver"));
        x.set_extensions(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "extensions"));
        x.set_externalized_properties(SARIF::get_optional<SARIF::ExternalPropertyFileReference>(j, "externalizedProperties"));
        x.set_graphs(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "graphs"));
        x.set_invocations(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "invocations"));
        x.set_logical_locations(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "logicalLocations"));
        x.set_policies(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "policies"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_results(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "results"));
        x.set_taxonomies(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "taxonomies"));
        x.set_thread_flow_locations(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "threadFlowLocations"));
        x.set_translations(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "translations"));
        x.set_web_requests(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "webRequests"));
        x.set_web_responses(SARIF::get_optional<std::vector<SARIF::ExternalPropertyFileReference>>(j, "webResponses"));
    }

    inline void to_json(json & j, const SARIF::ExternalPropertyFileReferences & x) {
        j = json::object();
        j["addresses"] = x.get_addresses();
        j["artifacts"] = x.get_artifacts();
        j["conversion"] = x.get_conversion();
        j["driver"] = x.get_driver();
        j["extensions"] = x.get_extensions();
        j["externalizedProperties"] = x.get_externalized_properties();
        j["graphs"] = x.get_graphs();
        j["invocations"] = x.get_invocations();
        j["logicalLocations"] = x.get_logical_locations();
        j["policies"] = x.get_policies();
        j["properties"] = x.get_properties();
        j["results"] = x.get_results();
        j["taxonomies"] = x.get_taxonomies();
        j["threadFlowLocations"] = x.get_thread_flow_locations();
        j["translations"] = x.get_translations();
        j["webRequests"] = x.get_web_requests();
        j["webResponses"] = x.get_web_responses();
    }

    inline void from_json(const json & j, SARIF::SpecialLocations& x) {
        x.set_display_base(SARIF::get_optional<SARIF::ArtifactLocation>(j, "displayBase"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
    }

    inline void to_json(json & j, const SARIF::SpecialLocations & x) {
        j = json::object();
        j["displayBase"] = x.get_display_base();
        j["properties"] = x.get_properties();
    }

    inline void from_json(const json & j, SARIF::VersionControlDetails& x) {
        x.set_as_of_time_utc(SARIF::get_optional<std::string>(j, "asOfTimeUtc"));
        x.set_branch(SARIF::get_optional<std::string>(j, "branch"));
        x.set_mapped_to(SARIF::get_optional<SARIF::ArtifactLocation>(j, "mappedTo"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_repository_uri(j.at("repositoryUri").get<std::string>());
        x.set_revision_id(SARIF::get_optional<std::string>(j, "revisionId"));
        x.set_revision_tag(SARIF::get_optional<std::string>(j, "revisionTag"));
    }

    inline void to_json(json & j, const SARIF::VersionControlDetails & x) {
        j = json::object();
        j["asOfTimeUtc"] = x.get_as_of_time_utc();
        j["branch"] = x.get_branch();
        j["mappedTo"] = x.get_mapped_to();
        j["properties"] = x.get_properties();
        j["repositoryUri"] = x.get_repository_uri();
        j["revisionId"] = x.get_revision_id();
        j["revisionTag"] = x.get_revision_tag();
    }

    inline void from_json(const json & j, SARIF::Run& x) {
        x.set_addresses(SARIF::get_optional<std::vector<SARIF::Address>>(j, "addresses"));
        x.set_artifacts(SARIF::get_optional<std::vector<SARIF::Artifact>>(j, "artifacts"));
        x.set_automation_details(SARIF::get_optional<SARIF::RunAutomationDetails>(j, "automationDetails"));
        x.set_baseline_guid(SARIF::get_optional<std::string>(j, "baselineGuid"));
        x.set_column_kind(SARIF::get_optional<SARIF::ColumnKind>(j, "columnKind"));
        x.set_conversion(SARIF::get_optional<SARIF::Conversion>(j, "conversion"));
        x.set_default_encoding(SARIF::get_optional<std::string>(j, "defaultEncoding"));
        x.set_default_source_language(SARIF::get_optional<std::string>(j, "defaultSourceLanguage"));
        x.set_external_property_file_references(SARIF::get_optional<SARIF::ExternalPropertyFileReferences>(j, "externalPropertyFileReferences"));
        x.set_graphs(SARIF::get_optional<std::vector<SARIF::Graph>>(j, "graphs"));
        x.set_invocations(SARIF::get_optional<std::vector<SARIF::Invocation>>(j, "invocations"));
        x.set_language(SARIF::get_optional<std::string>(j, "language"));
        x.set_logical_locations(SARIF::get_optional<std::vector<SARIF::LogicalLocation>>(j, "logicalLocations"));
        x.set_newline_sequences(SARIF::get_optional<std::vector<std::string>>(j, "newlineSequences"));
        x.set_original_uri_base_ids(SARIF::get_optional<std::map<std::string, SARIF::ArtifactLocation>>(j, "originalUriBaseIds"));
        x.set_policies(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "policies"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_redaction_tokens(SARIF::get_optional<std::vector<std::string>>(j, "redactionTokens"));
        x.set_results(SARIF::get_optional<std::vector<SARIF::Result>>(j, "results"));
        x.set_run_aggregates(SARIF::get_optional<std::vector<SARIF::RunAutomationDetails>>(j, "runAggregates"));
        x.set_special_locations(SARIF::get_optional<SARIF::SpecialLocations>(j, "specialLocations"));
        x.set_taxonomies(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "taxonomies"));
        x.set_thread_flow_locations(SARIF::get_optional<std::vector<SARIF::ThreadFlowLocation>>(j, "threadFlowLocations"));
        x.set_tool(j.at("tool").get<SARIF::Tool>());
        x.set_translations(SARIF::get_optional<std::vector<SARIF::ToolComponent>>(j, "translations"));
        x.set_version_control_provenance(SARIF::get_optional<std::vector<SARIF::VersionControlDetails>>(j, "versionControlProvenance"));
        x.set_web_requests(SARIF::get_optional<std::vector<SARIF::WebRequest>>(j, "webRequests"));
        x.set_web_responses(SARIF::get_optional<std::vector<SARIF::WebResponse>>(j, "webResponses"));
    }

    inline void to_json(json & j, const SARIF::Run & x) {
        j = json::object();
        j["addresses"] = x.get_addresses();
        j["artifacts"] = x.get_artifacts();
        j["automationDetails"] = x.get_automation_details();
        j["baselineGuid"] = x.get_baseline_guid();
        j["columnKind"] = x.get_column_kind();
        j["conversion"] = x.get_conversion();
        j["defaultEncoding"] = x.get_default_encoding();
        j["defaultSourceLanguage"] = x.get_default_source_language();
        j["externalPropertyFileReferences"] = x.get_external_property_file_references();
        j["graphs"] = x.get_graphs();
        j["invocations"] = x.get_invocations();
        j["language"] = x.get_language();
        j["logicalLocations"] = x.get_logical_locations();
        j["newlineSequences"] = x.get_newline_sequences();
        j["originalUriBaseIds"] = x.get_original_uri_base_ids();
        j["policies"] = x.get_policies();
        j["properties"] = x.get_properties();
        j["redactionTokens"] = x.get_redaction_tokens();
        j["results"] = x.get_results();
        j["runAggregates"] = x.get_run_aggregates();
        j["specialLocations"] = x.get_special_locations();
        j["taxonomies"] = x.get_taxonomies();
        j["threadFlowLocations"] = x.get_thread_flow_locations();
        j["tool"] = x.get_tool();
        j["translations"] = x.get_translations();
        j["versionControlProvenance"] = x.get_version_control_provenance();
        j["webRequests"] = x.get_web_requests();
        j["webResponses"] = x.get_web_responses();
    }

    inline void from_json(const json & j, SARIF::Coordinate& x) {
        x.set_schema(SARIF::get_optional<std::string>(j, "$schema"));
        x.set_inline_external_properties(SARIF::get_optional<std::vector<SARIF::ExternalProperties>>(j, "inlineExternalProperties"));
        x.set_properties(SARIF::get_optional<SARIF::PropertyBag>(j, "properties"));
        x.set_runs(j.at("runs").get<std::vector<SARIF::Run>>());
        x.set_version(j.at("version").get<SARIF::Version>());
    }

    inline void to_json(json & j, const SARIF::Coordinate & x) {
        j = json::object();
        j["$schema"] = x.get_schema();
        j["inlineExternalProperties"] = x.get_inline_external_properties();
        j["properties"] = x.get_properties();
        j["runs"] = x.get_runs();
        j["version"] = x.get_version();
    }

    inline void from_json(const json & j, SARIF::Role & x) {
        static std::unordered_map<std::string, SARIF::Role> enumValues {
            {"added", SARIF::Role::ADDED},
            {"analysisTarget", SARIF::Role::ANALYSIS_TARGET},
            {"attachment", SARIF::Role::ATTACHMENT},
            {"debugOutputFile", SARIF::Role::DEBUG_OUTPUT_FILE},
            {"deleted", SARIF::Role::DELETED},
            {"directory", SARIF::Role::DIRECTORY},
            {"driver", SARIF::Role::DRIVER},
            {"extension", SARIF::Role::EXTENSION},
            {"memoryContents", SARIF::Role::MEMORY_CONTENTS},
            {"modified", SARIF::Role::MODIFIED},
            {"policy", SARIF::Role::POLICY},
            {"referencedOnCommandLine", SARIF::Role::REFERENCED_ON_COMMAND_LINE},
            {"renamed", SARIF::Role::RENAMED},
            {"responseFile", SARIF::Role::RESPONSE_FILE},
            {"resultFile", SARIF::Role::RESULT_FILE},
            {"standardStream", SARIF::Role::STANDARD_STREAM},
            {"taxonomy", SARIF::Role::TAXONOMY},
            {"toolSpecifiedConfiguration", SARIF::Role::TOOL_SPECIFIED_CONFIGURATION},
            {"tracedFile", SARIF::Role::TRACED_FILE},
            {"translation", SARIF::Role::TRANSLATION},
            {"uncontrolled", SARIF::Role::UNCONTROLLED},
            {"unmodified", SARIF::Role::UNMODIFIED},
            {"userSpecifiedConfiguration", SARIF::Role::USER_SPECIFIED_CONFIGURATION},
        };
        auto iter = enumValues.find(j);
        if (iter != enumValues.end()) {
            x = iter->second;
        }
    }

    inline void to_json(json & j, const SARIF::Role & x) {
        switch (x) {
            case SARIF::Role::ADDED: j = "added"; break;
            case SARIF::Role::ANALYSIS_TARGET: j = "analysisTarget"; break;
            case SARIF::Role::ATTACHMENT: j = "attachment"; break;
            case SARIF::Role::DEBUG_OUTPUT_FILE: j = "debugOutputFile"; break;
            case SARIF::Role::DELETED: j = "deleted"; break;
            case SARIF::Role::DIRECTORY: j = "directory"; break;
            case SARIF::Role::DRIVER: j = "driver"; break;
            case SARIF::Role::EXTENSION: j = "extension"; break;
            case SARIF::Role::MEMORY_CONTENTS: j = "memoryContents"; break;
            case SARIF::Role::MODIFIED: j = "modified"; break;
            case SARIF::Role::POLICY: j = "policy"; break;
            case SARIF::Role::REFERENCED_ON_COMMAND_LINE: j = "referencedOnCommandLine"; break;
            case SARIF::Role::RENAMED: j = "renamed"; break;
            case SARIF::Role::RESPONSE_FILE: j = "responseFile"; break;
            case SARIF::Role::RESULT_FILE: j = "resultFile"; break;
            case SARIF::Role::STANDARD_STREAM: j = "standardStream"; break;
            case SARIF::Role::TAXONOMY: j = "taxonomy"; break;
            case SARIF::Role::TOOL_SPECIFIED_CONFIGURATION: j = "toolSpecifiedConfiguration"; break;
            case SARIF::Role::TRACED_FILE: j = "tracedFile"; break;
            case SARIF::Role::TRANSLATION: j = "translation"; break;
            case SARIF::Role::UNCONTROLLED: j = "uncontrolled"; break;
            case SARIF::Role::UNMODIFIED: j = "unmodified"; break;
            case SARIF::Role::USER_SPECIFIED_CONFIGURATION: j = "userSpecifiedConfiguration"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::Level & x) {
        if (j == "error") x = SARIF::Level::ERROR;
        else if (j == "none") x = SARIF::Level::NONE;
        else if (j == "note") x = SARIF::Level::NOTE;
        else if (j == "warning") x = SARIF::Level::WARNING;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::Level & x) {
        switch (x) {
            case SARIF::Level::ERROR: j = "error"; break;
            case SARIF::Level::NONE: j = "none"; break;
            case SARIF::Level::NOTE: j = "note"; break;
            case SARIF::Level::WARNING: j = "warning"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::Content & x) {
        if (j == "localizedData") x = SARIF::Content::LOCALIZED_DATA;
        else if (j == "nonLocalizedData") x = SARIF::Content::NON_LOCALIZED_DATA;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::Content & x) {
        switch (x) {
            case SARIF::Content::LOCALIZED_DATA: j = "localizedData"; break;
            case SARIF::Content::NON_LOCALIZED_DATA: j = "nonLocalizedData"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::BaselineState & x) {
        if (j == "absent") x = SARIF::BaselineState::ABSENT;
        else if (j == "new") x = SARIF::BaselineState::NEW;
        else if (j == "unchanged") x = SARIF::BaselineState::UNCHANGED;
        else if (j == "updated") x = SARIF::BaselineState::UPDATED;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::BaselineState & x) {
        switch (x) {
            case SARIF::BaselineState::ABSENT: j = "absent"; break;
            case SARIF::BaselineState::NEW: j = "new"; break;
            case SARIF::BaselineState::UNCHANGED: j = "unchanged"; break;
            case SARIF::BaselineState::UPDATED: j = "updated"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::Importance & x) {
        if (j == "essential") x = SARIF::Importance::ESSENTIAL;
        else if (j == "important") x = SARIF::Importance::IMPORTANT;
        else if (j == "unimportant") x = SARIF::Importance::UNIMPORTANT;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::Importance & x) {
        switch (x) {
            case SARIF::Importance::ESSENTIAL: j = "essential"; break;
            case SARIF::Importance::IMPORTANT: j = "important"; break;
            case SARIF::Importance::UNIMPORTANT: j = "unimportant"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::ResultKind & x) {
        if (j == "fail") x = SARIF::ResultKind::FAIL;
        else if (j == "informational") x = SARIF::ResultKind::INFORMATIONAL;
        else if (j == "notApplicable") x = SARIF::ResultKind::NOT_APPLICABLE;
        else if (j == "open") x = SARIF::ResultKind::OPEN;
        else if (j == "pass") x = SARIF::ResultKind::PASS;
        else if (j == "review") x = SARIF::ResultKind::REVIEW;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::ResultKind & x) {
        switch (x) {
            case SARIF::ResultKind::FAIL: j = "fail"; break;
            case SARIF::ResultKind::INFORMATIONAL: j = "informational"; break;
            case SARIF::ResultKind::NOT_APPLICABLE: j = "notApplicable"; break;
            case SARIF::ResultKind::OPEN: j = "open"; break;
            case SARIF::ResultKind::PASS: j = "pass"; break;
            case SARIF::ResultKind::REVIEW: j = "review"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::SuppressionKind & x) {
        if (j == "external") x = SARIF::SuppressionKind::EXTERNAL;
        else if (j == "inSource") x = SARIF::SuppressionKind::IN_SOURCE;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::SuppressionKind & x) {
        switch (x) {
            case SARIF::SuppressionKind::EXTERNAL: j = "external"; break;
            case SARIF::SuppressionKind::IN_SOURCE: j = "inSource"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::Status & x) {
        if (j == "accepted") x = SARIF::Status::ACCEPTED;
        else if (j == "rejected") x = SARIF::Status::REJECTED;
        else if (j == "underReview") x = SARIF::Status::UNDER_REVIEW;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::Status & x) {
        switch (x) {
            case SARIF::Status::ACCEPTED: j = "accepted"; break;
            case SARIF::Status::REJECTED: j = "rejected"; break;
            case SARIF::Status::UNDER_REVIEW: j = "underReview"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::Version & x) {
        if (j == "2.1.0") x = SARIF::Version::THE_210;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::Version & x) {
        switch (x) {
            case SARIF::Version::THE_210: j = "2.1.0"; break;
            default: throw "This should not happen";
        }
    }

    inline void from_json(const json & j, SARIF::ColumnKind & x) {
        if (j == "unicodeCodePoints") x = SARIF::ColumnKind::UNICODE_CODE_POINTS;
        else if (j == "utf16CodeUnits") x = SARIF::ColumnKind::UTF16_CODE_UNITS;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json & j, const SARIF::ColumnKind & x) {
        switch (x) {
            case SARIF::ColumnKind::UNICODE_CODE_POINTS: j = "unicodeCodePoints"; break;
            case SARIF::ColumnKind::UTF16_CODE_UNITS: j = "utf16CodeUnits"; break;
            default: throw "This should not happen";
        }
    }
}
