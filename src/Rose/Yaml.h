////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is mini-yaml version 22d3dcf5684a11f9c0508c1ad8b3282a1d888319 subsequently modified by the ROSE team.
//
// Do not upgrade to a new version without consulting Dan Quinlan.  Any new version needs to be code reviewed before being
// added to ROSE.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

/*
YAML documentation:
http://yaml.org/spec/1.0/index.html
https://www.codeproject.com/Articles/28720/YAML-Parser-in-C
*/

#ifndef ROSE_Yaml_H
#define ROSE_Yaml_H

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <rose_strtoull.h>
#include <sstream>
#include <string>
#include <type_traits>

namespace Rose {

/** YAML parser and unparser. */
namespace Yaml {

class Node;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper classes and functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace impl {

// Helper functionality, converting string to any data type. Strings are left untouched.
template<typename T, class Enable = void>
struct StringConverter {
    static T Get(const std::string &data) {
        return boost::lexical_cast<T>(data);
    }

    static T Get(const std::string & data, const T & defaultValue) {
        T result{};
        return boost::conversion::try_lexical_convert(data, result) ? result : defaultValue;
    }
};

// Converter from string to integral types handles hexadecimal, octal, and decimal.
template<typename T>
struct StringConverter<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    static T Get(const std::string &data) {
        const char *s = data.c_str();
        char *rest = nullptr;
        static_assert(sizeof(T) <= sizeof(uint64_t), "sizeof(T) is too big for implementation");
        if (strlen(s) != data.size() || data.empty() || isspace(data[0]))
            throw std::runtime_error("string is not a recognized integral literal");
        errno = 0;
        uint64_t big = rose_strtoull(s, &rest, 0);
        if (*rest) {
            throw std::runtime_error("string is not a recognized integral literal");
        } else if (ULLONG_MAX == big && ERANGE == errno) {
            throw std::runtime_error("integral literal out of range");
        } else {
            return boost::numeric_cast<T>(big);
        }
    }

    static T Get(const std::string & data, const T & defaultValue) {
        try {
            return Get(data);
        } catch (...) {
            return defaultValue;
        }
    }
};

template<>
struct StringConverter<std::string> {
    static std::string Get(const std::string & data) {
        return data;
    }

    static std::string Get(const std::string & data, const std::string & defaultValue) {
        if (data.size() == 0) {
            return defaultValue;
        }
        return data;
    }
};

template<>
struct StringConverter<bool> {
    static bool Get(const std::string & data) {
        std::string tmpData = data;
        std::transform(tmpData.begin(), tmpData.end(), tmpData.begin(), ::tolower);
        if (tmpData == "true" || tmpData == "yes" || tmpData == "1") {
            return true;
        }

        return false;
    }

    static bool Get(const std::string & data, const bool & defaultValue) {
        if (data.size() == 0) {
            return defaultValue;
        }

        return Get(data);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for YAML exceptions. */
class Exception : public std::runtime_error {
public:
    /** Exception types. */
    enum eType {
        InternalError,                                  /**< Internal error. */
        ParsingError,                                   /**< Invalid parsing data. */
        OperationError                                  /**< User operation error. */
    };

    /** Constructor.
    *
    * @param message    Exception message.
    * @param type       Type of exception. */
    Exception(const std::string & message, const eType type);

    /** Get type of exception. */
    eType Type() const;

    /** Get message of exception. */
    const char * Message() const;

private:
    eType m_Type;                                       // Type of exception.
};


/** Internal exception.
 *
 * @see Exception */
class InternalException : public Exception {
public:
    /** Constructor.
    *
    * @param message Exception message. */
    InternalException(const std::string & message);
};


/** Parsing exception.
 *
 * @see Exception */
class ParsingException : public Exception {
public:
    /** Constructor.
     *
     * @param message Exception message. */
    ParsingException(const std::string & message);
};


/** Operation exception.
 *
 * @see Exception */
class OperationException : public Exception {
public:
    /** Constructor.
     *
     * @param message Exception message. */
    OperationException(const std::string & message);
};


/** Iterator over YAML nodes. */
class Iterator {
public:
    friend class Node;

    /** Default constructor. */
    Iterator();

    /** Copy constructor. */
    Iterator(const Iterator&);

    /** Assignment operator. */
    Iterator & operator=(const Iterator&);

    /** Destructor. */
    ~Iterator();

    /** Get node of iterator.
     *
     *  First pair item is the key of map value, empty if type is sequence. */
    std::pair<const std::string&, Node&> operator*();

    /** Dereference. */
    std::unique_ptr<std::pair<const std::string&, Node&>> operator->();

    /** Increment operator.
     *
     * @{ */
    Iterator& operator++();
    Iterator operator++(int);
    /** @} */

    /** Decrement operator.
     *
     * @{ */
    Iterator& operator--();
    Iterator operator--(int);
    /** @} */

    /** Check if this iterator is equal to another iterator. */
    bool operator==(const Iterator&);

    /** Check if this iterator is not equal to another iterator. */
    bool operator!=(const Iterator&);

private:
    enum eType {
        None,
        SequenceType,
        MapType
    };

    eType   m_Type;                                     // Type of iterator.
    void *  m_pImp;                                     // Implementation of iterator class.
};


/** Iterator over constant YAML nodes. */
class ConstIterator {
public:
    friend class Node;

    /** Default constructor. */
    ConstIterator();

    /** Copy constructor. */
    ConstIterator(const ConstIterator&);

    /** Assignment operator. */
    ConstIterator & operator=(const ConstIterator&);

    /** Destructor. */
    ~ConstIterator();

    /** Get node of iterator.
     *
     *  First pair item is the key of map value, empty if type is sequence. */
    std::pair<const std::string&, const Node&> operator*();

    /** Dereference. */
    std::unique_ptr<std::pair<const std::string&, const Node&>> operator->();

    /** Increment operator.
     *
     * @{ */
    ConstIterator& operator++();
    ConstIterator operator++(int);
    /** @} */

    /** Decrement operator.
     *
     * @{ */
    ConstIterator& operator--();
    ConstIterator operator--(int);
    /** @} */

    /** Check if this iterator is equal to another iterator. */
    bool operator==(const ConstIterator&);

    /** Check if this iterator is not equal to another iterator. */
    bool operator!=(const ConstIterator&);

private:
    enum eType {
        None,
        SequenceType,
        MapType
    };

    eType   m_Type;                                     // Type of iterator.
    void *  m_pImp;                                     // Implementation of constant iterator class.
};


/** YAML node. */
class Node {
public:
    friend class Iterator;

    /** Enumeration of node types. */
    enum eType {
        None,
        SequenceType,
        MapType,
        ScalarType
    };

    /** Default constructor. */
    Node();

    /** Copy constructor. */
    Node(const Node & node);

    /** Assignment constructors.
     *
     *  Converts node to scalar type if needed.
     *
     *  @{ */
    Node(const std::string & value);
    Node(const char * value);
    /** @} */

    /** Destructor. */
    ~Node();

    /** Functions for checking type of node.
     *
     * @{ */
    eType type() const { return Type(); }
    bool isNone() const { return IsNone(); }
    bool isSequence() const { return IsSequence(); }
    bool isMap() const { return IsMap(); }
    bool isScalar() const { return IsScalar(); }
    /** @} */

    /** Completely clear node. */
    void clear() { Clear(); }

    /** Get node as given template type.
     *
     * @{ */
    template<typename T>
    T as() const {
        return impl::StringConverter<T>::Get(AsString());
    }

    template<typename T>
    T as(const T & defaultValue) const {
        return impl::StringConverter<T>::Get(AsString(), defaultValue);
    }
    /** @} */

    /** Get size of node.
     *
     *        Nodes of type None or Scalar will return 0. */
    size_t size() const { return Size(); }

    // Sequence operators

    /** Insert sequence item at given index.
     *
     * Converts node to sequence type if needed. Adding new item to end of sequence if index is larger than sequence size. */
    Node& insert(const size_t index) { return Insert(index); }

    /** Add new sequence index to back.
     *
     *  Converts node to sequence type if needed. */
    Node& pushFront() { return PushFront(); }

    /** Add new sequence index to front.
     *
     *  Converts node to sequence type if needed. */
    Node& pushBack() { return PushBack(); }

    /** Get sequence/map item.
     *
     *  Converts node to sequence/map type if needed.
     *
     * @param index  Sequence index. Returns None type Node if index is unknown.
     * @param key    Map key. Creates a new node if key is unknown.
     *
     * @{ */
    Node& operator[](const size_t index);
    Node& operator[](const std::string& key);
    /** @} */

    /** Erase item.
     *
     * No action if node is not a sequence or map.
     *
     * @{ */
    void erase(const size_t index) { Erase(index); }
    void erase(const std::string& key) { Erase(key); }
    /** @} */

    /** Assignment operators.
     *
     * @{ */
    Node& operator=(const Node& node);
    Node& operator=(const std::string& value);
    Node& operator=(const char* value);
    /** @} */

    /** Get start iterator.
     *
     * @{ */
    Iterator begin() { return Begin(); }
    ConstIterator begin() const { return Begin(); }
    /** @} */

    /** Get end iterator.
     *
     *  @{ */
    Iterator end() { return End(); }
    ConstIterator end() const { return End(); }
    /** @} */

    // Original function names with unusual capitalization.
    eType Type() const;
    bool IsNone() const;
    bool IsSequence() const;
    bool IsMap() const;
    bool IsScalar() const;
    void Clear();
    size_t Size() const;
    Node & Insert(const size_t index);
    Node & PushFront();
    Node & PushBack();
    void Erase(const size_t index);
    void Erase(const std::string & key);
    Iterator Begin();
    ConstIterator Begin() const;
    Iterator End();
    ConstIterator End() const;

    template<typename T>
    T As() const {
        return impl::StringConverter<T>::Get(AsString());
    }

    template<typename T>
    T As(const T & defaultValue) const {
        return impl::StringConverter<T>::Get(AsString(), defaultValue);
    }


private:
    // Get as string. If type is scalar, else empty.
    const std::string & AsString() const;

     // Implementation of node class.
    void * m_pImp;
};


// Original oddly capitalized functions
void Parse(Node &root, const boost::filesystem::path&);
void Parse(Node &root, std::iostream&);
void Parse(Node &root, const std::string &data);
void Parse(Node &root, const char *data, const size_t size);

/** Parse YAML from file into node. */
void parse(Node &root, const boost::filesystem::path&);

/** Parse YAML from stream into node. */
void parse(Node &root, std::iostream&);

/** Parse YAML from data into node.
 *
 * @{ */
void parse(Node &root, const std::string &data);
void parse(Node &root, const char *data);
/** @} */

/** Serialization configuration structure, describing output behavior. */
struct SerializeConfig {
    /** Constructor.
     *
     * @param spaceIndentation       Number of spaces per indentation.
     * @param scalarMaxLength        Maximum length of scalars. Serialized as folder scalars if exceeded.
     *                               Ignored if equal to 0.
     * @param sequenceMapNewline     Put maps on a new line if parent node is a sequence.
     * @param mapScalarNewline       Put scalars on a new line if parent node is a map. */
    SerializeConfig(const size_t spaceIndentation = 2,
                    const size_t scalarMaxLength = 64,
                    const bool sequenceMapNewline = false,
                    const bool mapScalarNewline = false);

    size_t SpaceIndentation;                            /**< Number of spaces per indentation. */
    size_t ScalarMaxLength;                             /**< Maximum length of scalars. Serialized as folder scalars if exceeded. */
    bool SequenceMapNewline;                            /**< Put maps on a new line if parent node is a sequence. */
    bool MapScalarNewline;                              /**< Put scalars on a new line if parent node is a map. */
};


/** Serialize YAML.
*
* @param root       Root node to serialize.
* @param filename   Path of output file.
* @param stream     Output stream.
* @param string     String of output data.
* @param config     Serialization configurations.
*
* @throw InternalException  An internal error occurred.
* @throw OperationException If filename or buffer pointer is invalid.
*                           If config is invalid.
*
* @{ */
void Serialize(const Node & root, const char * filename, const SerializeConfig & config = {2, 64, false, false});
void Serialize(const Node & root, std::iostream & stream, const SerializeConfig & config = {2, 64, false, false});
void Serialize(const Node & root, std::string & string, const SerializeConfig & config = {2, 64, false, false});
/** @} */

} // namespace
} // namespace

#endif
