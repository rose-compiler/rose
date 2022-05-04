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

#ifndef MiniYaml_Yaml_H
#define MiniYaml_Yaml_H

#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

/** YAML parser and unparser. */
namespace Yaml {

class Node;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper classes and functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace impl {

// Helper functionality, converting string to any data type. Strings are left untouched.
template<typename T>
struct StringConverter {
    static T Get(const std::string & data) {
        T type;
        std::stringstream ss(data);
        ss >> type;
        return type;
    }

    static T Get(const std::string & data, const T & defaultValue) {
        T type;
        std::stringstream ss(data);
        ss >> type;

        if (ss.fail()) {
            return defaultValue;
        }

        return type;
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
    Iterator(const Iterator & it);

    /** Assignment operator. */
    Iterator & operator=(const Iterator & it);

    /** Destructor. */
    ~Iterator();

    /** Get node of iterator.
     *
     *  First pair item is the key of map value, empty if type is sequence. */
    std::pair<const std::string &, Node &> operator*();

    /** Post-increment operator. */
    Iterator & operator++(int);

    /** Post-decrement operator. */
    Iterator & operator--(int);

    /** Check if this iterator is equal to another iterator. */
    bool operator==(const Iterator & it);

    /** Check if this iterator is not equal to another iterator. */
    bool operator!=(const Iterator & it);

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
    ConstIterator(const ConstIterator & it);

    /** Assignment operator. */
    ConstIterator & operator=(const ConstIterator & it);

    /** Destructor. */
    ~ConstIterator();

    /** Get node of iterator.
     *
     *  First pair item is the key of map value, empty if type is sequence. */
    std::pair<const std::string &, const Node &> operator*();

    /** Post-increment operator. */
    ConstIterator & operator++(int);

    /** Post-decrement operator. */
    ConstIterator & operator--(int);

    /** Check if this iterator is equal to another iterator. */
    bool operator==(const ConstIterator & it);

    /** Check if this iterator is not equal to another iterator. */
    bool operator!=(const ConstIterator & it);

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
    eType Type() const;
    bool IsNone() const;
    bool IsSequence() const;
    bool IsMap() const;
    bool IsScalar() const;
    /** @} */

    /** Completely clear node. */
    void Clear();

    /** Get node as given template type. */
    template<typename T>
    T As() const {
        return impl::StringConverter<T>::Get(AsString());
    }

    /** Get node as given template type. */
    template<typename T>
    T As(const T & defaultValue) const {
        return impl::StringConverter<T>::Get(AsString(), defaultValue);
    }

    /** Get size of node.
     *
     *        Nodes of type None or Scalar will return 0. */
    size_t Size() const;

    // Sequence operators

    /** Insert sequence item at given index.
     *
     * Converts node to sequence type if needed. Adding new item to end of sequence if index is larger than sequence size. */
    Node & Insert(const size_t index);

    /** Add new sequence index to back.
     *
     *  Converts node to sequence type if needed. */
    Node & PushFront();

    /** Add new sequence index to front.
     *
     *  Converts node to sequence type if needed. */
    Node & PushBack();

    /** Get sequence/map item.
     *
     *  Converts node to sequence/map type if needed.
     *
     * @param index  Sequence index. Returns None type Node if index is unknown.
     * @param key    Map key. Creates a new node if key is unknown.
     *
     * @{ */
    Node & operator[](const size_t index);
    Node & operator[](const std::string & key);
    /** @} */

    /** Erase item.
     *
     * No action if node is not a sequence or map.
     *
     * @{ */
    void Erase(const size_t index);
    void Erase(const std::string & key);
    /** @} */

    /** Assignment operators.
     *
     * @{ */
    Node & operator=(const Node & node);
    Node & operator=(const std::string & value);
    Node & operator=(const char * value);
    /** @} */

    /** Get start iterator.
     *
     * @{ */
    Iterator Begin();
    ConstIterator Begin() const;
    /** @} */

    /** Get end iterator.
     *
     *  @{ */
    Iterator End();
    ConstIterator End() const;
    /** @} */

private:
    // Get as string. If type is scalar, else empty.
    const std::string & AsString() const;

     // Implementation of node class.
    void * m_pImp;
};


/** Parse YAML.
 *
 * Population given root node with deserialized data.
 *
 * @param root       Root node to populate.
 * @param filename   Path of input file.
 * @param stream     Input stream.
 * @param string     String of input data.
 * @param buffer     Char array of input data.
 * @param size       Buffer size.
 *
 * @throw InternalException  An internal error occurred.
 * @throw ParsingException   Invalid input YAML data.
 * @throw OperationException If filename or buffer pointer is invalid.
 *
 * @{ */
void Parse(Node & root, const char * filename);
void Parse(Node & root, std::iostream & stream);
void Parse(Node & root, const std::string & string);
void Parse(Node & root, const char * buffer, const size_t size);
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

#endif
