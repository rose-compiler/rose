// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_HashMap_H
#define Sawyer_HashMap_H

#include <Sawyer/Optional.h>
#include <Sawyer/Sawyer.h>

#include <boost/range/iterator_range.hpp>
#include <boost/unordered_map.hpp>
#include <stdexcept>

namespace Sawyer {
namespace Container {

/** %Container associating values with keys.
 *
 *  This container is similar to TL's @c unordered_map in that it stores a value for each key using a hash-based mechanism,
 *  although it works for C++ versions before C++11.  The naming scheme is similar to other %Sawyer containers.  If you're used
 *  to the STL, the main differences are described in the documentation for the @ref Sawyer::Container name space.
 *
 *  See also, @ref Map. */
template<class K, class T, class H = boost::hash<K>, class C = std::equal_to<K>,
         class A = std::allocator<std::pair<const K, T> > >
class HashMap {
public:
    typedef K Key;                                      /**< Type of keys. */
    typedef T Value;                                    /**< Type of values. */
    typedef H Hasher;                                   /**< Functor for hashing keys. */
    typedef C Comparator;                               /**< Functor for comparing keys. */
    typedef A Allocator;                                /**< Functor for allocating node memory. */

private:
    typedef boost::unordered_map<Key, Value, Hasher, Comparator, Allocator> ImplMap;
    ImplMap map_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    // Apparently no serialization functions for boost::unordered_map, so do it the hard way.
    template<class S>
    void save(S &s, const unsigned /*verion*/) const {
        size_t nElmts = map_.size();
        s & BOOST_SERIALIZATION_NVP(nElmts);
        for (typename ImplMap::const_iterator iter = map_.begin(); iter != map_.end(); ++iter) {
            const Key &key = iter->first;
            s & BOOST_SERIALIZATION_NVP(key);
            const Value &value = iter->second;
            s & BOOST_SERIALIZATION_NVP(value);
        }
    }

    template<class S>
    void load(S & s, const unsigned /*version*/) {
        size_t nElmts;
        s & BOOST_SERIALIZATION_NVP(nElmts);
        for (size_t i=0; i<nElmts; ++i) {
            Key key;
            s & BOOST_SERIALIZATION_NVP(key);
            Value value;
            s & BOOST_SERIALIZATION_NVP(value);
            map_.insert(std::make_pair(key, value));
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    // Apparently no serialization functions for boost::unordered_map, so do it the hard way.
    template<class Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        size_t nElmts = map_.size();
        archive(CEREAL_NVP(nElmts));
        for (typename ImplMap::const_iterator iter = map_.begin(); iter != map_.end(); ++iter) {
            const Key &key = iter->first;
            archive(CEREAL_NVP(key));
            const Value &value = iter->second;
            archive(CEREAL_NVP(value));
        }
    }

    template<class Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
        size_t nElmts;
        archive(CEREAL_NVP(nElmts));
        for (size_t i = 0; i < nElmts; ++i) {
            Key key;
            archive(CEREAL_NVP(key));
            Value value;
            archive(CEREAL_NVP(key));
            map_.insert(std::make_pair(key, value));
        }
    }
#endif


public:
    /** Type for stored nodes.
     *
     *  A storage node contains an immutable key and its associated value. */
    class Node: private ImplMap::value_type {
        // This class MUST be binary compatible with ImplMap::value_type
    public:
        explicit Node(const std::pair<const Key, Value> &pair)
            : std::pair<const Key, Value>(pair) {}

        Node(const Key &key, Value &value)
            : std::pair<const Key, Value>(key, value) {}

        /** Key part of key/value node.
         *
         *  Returns the key part of a key/value node. Keys are not mutable when they are part of a map. */
        const Key& key() const { return this->first; }

        /** Value part of key/value node.
         *
         *  Returns a reference to the value part of a key/value node.
         *
         * @{ */
        Value& value() { return this->second; }
        const Value& value() const { return this->second; }
        /** @} */
    };
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    template<class Derived, class Value, class BaseIterator>
    class ForwardIterator {
    public:
        // Five standard iterator types
        using iterator_category = std::forward_iterator_tag;
        using value_type = Value;
        using difference_type = std::ptrdiff_t;
        using pointer = Value*;
        using reference = Value&;

    protected:
        BaseIterator base_;
        ForwardIterator() {}
        ForwardIterator(const BaseIterator &base): base_(base) {}
    public:
        Derived& operator=(const Derived &other) { base_ = other.base_; return *derived(); }
        Derived& operator++() { ++base_; return *derived(); }
        Derived operator++(int) { Derived old = *derived(); ++*this; return old; }
        template<class OtherIter> bool operator==(const OtherIter &other) const { return base_ == other.base(); }
        template<class OtherIter> bool operator!=(const OtherIter &other) const { return base_ != other.base(); }
        const BaseIterator& base() const { return base_; }
    protected:
        Derived* derived() { return static_cast<Derived*>(this); }
        const Derived* derived() const { return static_cast<const Derived*>(this); }
    };

public:
    /** Forward iterator over key/value nodes.
     *
     *  Dereferencing this iterator will return a Node from which both the key and the value can be obtained. Node iterators
     *  are implicitly convertible to both key and value iterators. */
    class NodeIterator: public ForwardIterator<NodeIterator, Node, typename ImplMap::iterator> {
        typedef                ForwardIterator<NodeIterator, Node, typename ImplMap::iterator> Super;
    public:
        NodeIterator() {}
        NodeIterator(const NodeIterator &other): Super(other) {}
        Node& operator*() const { return *(Node*)&*this->base_; } // boost iter's value cast to our own node type
        Node* operator->() const { return (Node*)&*this->base_; }
    private:
        friend class HashMap;
        NodeIterator(const typename ImplMap::iterator &base): Super(base) {}
    };

    /** Forward iterator over key/value nodes.
     *
     *  Dereferencing this iterator will return a Node from which both the key and the value can be obtained. Node iterators
     *  are implicitly convertible to both key and value iterators. */
    class ConstNodeIterator: public ForwardIterator<ConstNodeIterator, const Node, typename ImplMap::const_iterator> {
        typedef                     ForwardIterator<ConstNodeIterator, const Node, typename ImplMap::const_iterator> Super;
    public:
        ConstNodeIterator() {}
        ConstNodeIterator(const ConstNodeIterator &other): Super(other) {}
        ConstNodeIterator(const NodeIterator &other): Super(typename ImplMap::const_iterator(other.base())) {}
        const Node& operator*() const { return *(const Node*)&*this->base_; }
        const Node* operator->() const { return (const Node*)&*this->base_; }
    private:
        friend class HashMap;
        ConstNodeIterator(const typename ImplMap::const_iterator &base): Super(base) {}
        ConstNodeIterator(const typename ImplMap::iterator &base): Super(typename ImplMap::const_iterator(base)) {}
    };

    /** Forward iterator over keys.
     *
     *  Dereferencing this iterator will return a reference to a const key.  Keys cannot be altered while they are a member of
     *  this container. */
    class ConstKeyIterator: public ForwardIterator<ConstKeyIterator, const Key, typename ImplMap::const_iterator> {
        typedef                    ForwardIterator<ConstKeyIterator, const Key, typename ImplMap::const_iterator> Super;
    public:
        ConstKeyIterator() {}
        ConstKeyIterator(const ConstKeyIterator &other): Super(other) {}
        ConstKeyIterator(const NodeIterator &other): Super(typename ImplMap::const_iterator(other.base())) {}
        ConstKeyIterator(const ConstNodeIterator &other): Super(other.base()) {}
        const Key& operator*() const { return this->base()->first; }
        const Key* operator->() const { return &this->base()->first; }
    };

    /** Forward iterator over values.
     *
     *  Dereferencing this iterator will return a reference to the user-defined value of the node.  Values may be altered
     *  in-place while they are members of a container. */
    class ValueIterator: public ForwardIterator<ValueIterator, Value, typename ImplMap::iterator> {
        typedef                 ForwardIterator<ValueIterator, Value, typename ImplMap::iterator> Super;
    public:
        ValueIterator() {}
        ValueIterator(const ValueIterator &other): Super(other) {}
        ValueIterator(const NodeIterator &other): Super(other.base()) {}
        Value& operator*() const { return this->base()->second; }
        Value* operator->() const { return &this->base()->second; }
    };

    /** Forward iterator over values.
     *
     *  Dereferencing this iterator will return a reference to the user-defined value of the node.  Values may be altered
     *  in-place while they are members of a container. */
    class ConstValueIterator: public ForwardIterator<ConstValueIterator, const Value, typename ImplMap::const_iterator> {
        typedef ForwardIterator<ConstValueIterator, const Value, typename ImplMap::const_iterator> Super;
    public:
        ConstValueIterator() {}
        ConstValueIterator(const ConstValueIterator &other): Super(other) {}
        ConstValueIterator(const ValueIterator &other): Super(typename ImplMap::const_iterator(other.base())) {}
        ConstValueIterator(const ConstNodeIterator &other): Super(other.base()) {}
        ConstValueIterator(const NodeIterator &other): Super(typename ImplMap::const_iterator(other.base())) {}
        const Value& operator*() const { return this->base()->second; }
        const Value* operator->() const { return &this->base()->second; }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Default constructor.
     *
     *  Creates an empty map. */
    HashMap() {}

    /** Constructs a hash map with at least @p n buckets. */
    HashMap(size_t n, const Hasher &hasher = Hasher(), const Comparator &cmp = Comparator(), const Allocator& alloc = Allocator())
        : map_(n, hasher, cmp, alloc) {}

    /** Copy constructor. */
    HashMap(const HashMap& other)
        : map_(other.map_) {}

    /**  Copy constructor.
     *
     *   Initializes the new map with copies of the nodes of the @p other map. The keys and values must be convertible from the
     *   ohter map to this map. */
    template<class K2, class T2, class H2, class C2, class A2>
    HashMap(const HashMap<K2, T2, H2, C2, A2> &other) {
        typedef typename HashMap<K2, T2, H2, C2, A2>::ConstNodeIterator OtherIterator;
        boost::iterator_range<OtherIterator> otherNodes = other.nodes();
        for (OtherIterator otherIter = otherNodes.begin(); otherIter != otherNodes.end(); ++otherIter)
            map_.insert(std::make_pair(Key(otherIter->key()), Value(otherIter->value())));
    }
    
    /** Assignment operator. */
    template<class K2, class T2, class H2, class C2, class A2>
    HashMap& operator=(const HashMap<K2, T2, H2, C2, A2> &other) {
        typedef typename HashMap<K2, T2, H2, C2, A2>::ConstNodeIterator OtherIterator;
        clear();
        boost::iterator_range<OtherIterator> otherNodes = other.nodes();
        for (OtherIterator otherIter = otherNodes.begin(); otherIter != otherNodes.end(); ++otherIter)
            map_.insert(std::make_pair(Key(otherIter->key()), Value(otherIter->value())));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Iteration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Iterators for container nodes.
     *
     *  This returns a range of node-iterators that will traverse all nodes (key/value pairs) of this container.
     *
     * @{ */
    boost::iterator_range<NodeIterator> nodes() {
        return boost::iterator_range<NodeIterator>(NodeIterator(map_.begin()), NodeIterator(map_.end()));
    }
    boost::iterator_range<ConstNodeIterator> nodes() const {
        return boost::iterator_range<ConstNodeIterator>(ConstNodeIterator(map_.begin()), ConstNodeIterator(map_.end()));
    }
    /** @} */

    /** Iterators for container keys.
     *
     *  Returns a range of key-iterators that will traverse the keys of this container.
     *
     * @{ */
    boost::iterator_range<ConstKeyIterator> keys() {
        return boost::iterator_range<ConstKeyIterator>(NodeIterator(map_.begin()), NodeIterator(map_.end()));
    }
    boost::iterator_range<ConstKeyIterator> keys() const {
        return boost::iterator_range<ConstKeyIterator>(ConstNodeIterator(map_.begin()), ConstNodeIterator(map_.end()));
    }
    /** @} */

    /** Iterators for container values.
     *
     *  Returns a range of iterators that will traverse the user-defined values of this container.  The values are iterated in
     *  key order, although the keys are not directly available via these iterators.
     *
     * @{ */
    boost::iterator_range<ValueIterator> values() {
        return boost::iterator_range<ValueIterator>(NodeIterator(map_.begin()), NodeIterator(map_.end()));
    }
    boost::iterator_range<ConstValueIterator> values() const {
        return boost::iterator_range<ConstValueIterator>(ConstNodeIterator(map_.begin()), ConstNodeIterator(map_.end()));
    }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Size and capacity
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Determine whether this container is empty.
     *
     *  Returns true if the container is empty, and false if it has at least one node. This method executes in constant time. */
    bool isEmpty() const {
        return map_.empty();
    }

    /** Number of nodes, keys, or values in this container.
     *
     *  Returns the number of nodes currently stored in this container. A node is a key + value pair. This method executes in
     *  constant time. */
    size_t size() const {
        return map_.size();
    }

    /** Number of buckets. */
    size_t nBuckets() const {
        return map_.bucket_count();
    }

    /** Average number of nodes per bucket. */
    double loadFactor() const {
        return map_.load_factor();
    }

    /** Property: Maximum allowed load faster before automatic rehash.
     *
     * @{ */
    double maxLoadFactor() const {
        return map_.max_load_factor();
    }
    void maxLoadFactor(double mlf) {
        map_.max_load_factor(mlf);
    }
    /** @} */

    /** Change number of buckets. */
    void rehash(size_t nBuckets) {
        map_.rehash(nBuckets);
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Searching
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Find a node by key.
     *
     *  Looks for a node whose key is equal to the specified @p key and returns an iterator to that node, or the end iterator
     *  if no such node exists.
     *
     * @{ */
    NodeIterator find(const Key &key) {
        return map_.find(key);
    }
    ConstNodeIterator find(const Key &key) const {
        return map_.find(key);
    }
    /** @} */

    /** Determine if a key exists.
     *
     *  Looks for a node whose key is equal to the specified @p key and returns true if found, or false if no such node
     *  exists. */
    bool exists(const Key &key) const {
        return map_.find(key) != map_.end();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Accessors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Return a reference to an existing value.
     *
     *  Returns a reference to the value at the node with the specified @p key. Unlike <code>std::map</code>, this container
     *  does not instantiate a new key/value pair if the @p key is not in the map's domain.  In other words, the array operator
     *  for this class is more like an array operator on arrays or vectors--such objects are not automatically extended if
     *  dereferenced with an operand that is outside the domain.
     *
     *  If the @p key is not part of this map's domain then an <code>std::domain_error</code> is thrown.
     *
     *  @sa insert insertDefault
     *
     * @{ */
    Value& operator[](const Key &key) {
        return get(key);
    }
    const Value& operator[](const Key &key) const {
        return get(key);
    }
    /** @} */

    /** Lookup and retun an existing value.
     *
     *  Returns a reference to the value at the node with the specified @p key, which must exist. If the @p key is not part of
     *  this map's domain, then an <code>std::domain_error</code> is thrown.
     *
     *  @sa insert insertDefault
     *
     * @{ */
    Value& get(const Key &key) {
        typename ImplMap::iterator found = map_.find(key);
        if (found == map_.end())
            throw std::domain_error("key lookup failure; key is not in map domain");
        return found->second;
    }
    const Value& get(const Key &key) const {
        typename ImplMap::const_iterator found = map_.find(key);
        if (found == map_.end())
            throw std::domain_error("key lookup failure; key is not in map domain");
        return found->second;
    }
    /** @} */

    /** Lookup and return a value or nothing.
     *
     *  Looks up the node with the specified key and returns either a copy of its value, or nothing.
     *
     *  Here's an example of one convenient way to use this:
     *
     * @code
     *  HashMap<std::string, FileInfo> files;
     *  ...
     *  if (Optional<FileInfo> fileInfo = files.getOptional(fileName))
     *      std::cout <<"file info for \"" <<fileName <<"\" is " <<*fileInfo <<"\n";
     * @endcode
     *
     *  The equivalent STL approach is:
     *
     * @code
     *  std::map<std::string, FileInfo> files;
     *  ...
     *  std::map<std::string, FileInfo>::const_iterator filesIter = files.find(fileName);
     *  if (fileIter != files.end())
     *      std::cout <<"file info for \"" <<fileName <<"\" is " <<filesIter->second <<"\n";
     * @endcode */
    Optional<Value> getOptional(const Key &key) const {
        typename ImplMap::const_iterator found = map_.find(key);
        return found == map_.end() ? Optional<Value>() : Optional<Value>(found->second);
    }

    /** Lookup and return a value or something else.
     *
     *  This is similar to the @ref get method, except a default can be provided.  If a node with the specified @p key is
     *  present in this container, then a reference to that node's value is returned, otherwise the (reference to) supplied
     *  default is returned.
     *
     * @{ */
    Value& getOrElse(const Key &key, Value &dflt) {
        typename ImplMap::iterator found = map_.find(key);
        return found == map_.end() ? dflt : found->second;
    }
    const Value& getOrElse(const Key &key, const Value &dflt) const {
        typename ImplMap::const_iterator found = map_.find(key);
        return found == map_.end() ? dflt : found->second;
    }
    /** @} */

    /** Lookup and return a value or a default.
     *
     *  This is similar to the @ref getOrElse method except when the key is not present in the map, a reference to a const,
     *  default-constructed value is returned. */
    const Value& getOrDefault(const Key &key) const {
        static const Value dflt;
        typename ImplMap::const_iterator found = map_.find(key);
        return found==map_.end() ? dflt : found->second;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mutators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Insert or update a key/value pair.
     *
     *  Inserts the key/value pair into the container. If a previous node already had the same key then it is replaced by the
     *  new node.
     *
     *  @sa insertDefault insertMaybe insertMaybeDefault insertMultiple insertMaybeMultiple */
    HashMap& insert(const Key &key, const Value &value) {
        std::pair<typename ImplMap::iterator, bool> inserted = map_.insert(std::make_pair(key, value));
        if (!inserted.second)
            inserted.first->second = value;
        return *this;
    }

    /** Insert or update a key with a default value.
     *
     *  The value associated with @p key in the map is replaced with a default-constructed value.  If the key does not exist
     *  then it is inserted with a default value.  This operation is similar to the array operator of <code>std::map</code>.
     *
     *  @sa insert insertMaybe insertMaybeDefault insertMultiple insertMaybeMultiple */
    HashMap& insertDefault(const Key &key) {
        return insert(key, Value());
    }

    /** Insert multiple values.
     *
     *  Inserts copies of the nodes in the specified node iterator range. The iterators must iterate over objects that have
     *  <code>key</code> and <code>value</code> methods that return keys and values that are convertible to the types used by
     *  this container.
     *
     *  The normal way to insert the contents of one map into another is:
     *
     * @code
     *  HashMap<...> source = ...;
     *  HashMap<...> destination = ...;
     *  destination.insertMultiple(source.nodes());
     * @endcode
     *
     * @sa insert insertDefault insertMaybe insertMaybeDefault insertMaybeMultiple
     *
     * @{ */
    template<class OtherNodeIterator>
    HashMap& insertMultiple(const OtherNodeIterator &begin, const OtherNodeIterator &end) {
        for (OtherNodeIterator otherIter = begin; otherIter != end; ++otherIter)
            insert(Key(otherIter->key()), Value(otherIter->value()));
        return *this;
    }
    template<class OtherNodeIterator>
    HashMap& insertMultiple(const boost::iterator_range<OtherNodeIterator> &range) {
        return insertMultiple(range.begin(), range.end());
    }
    /** @} */

    /** Conditionally insert a new key/value pair.
     *
     *  Inserts the key/value pair into the container if the container does not yet have a node with the same key.  The return
     *  value is a reference to the value that is in the container, either the value that previously existed or a copy of the
     *  specified @p value.
     *
     *  @sa insert insertDefault insertMaybeDefault insertMultiple insertMaybeMultiple */
    Value& insertMaybe(const Key &key, const Value &value) {
        return map_.insert(std::make_pair(key, value)).first->second;
    }

    /** Conditionally insert a new key with default value.
     *
     *  Inserts a key/value pair into the container if the container does not yet have a node with the same key. The value is
     *  default-constructed. The return value is a reference to the value that is in the container, either the value that
     *  previously existed or the new default-constructed value.
     *
     *  @sa insert insertDefault insertMultiple insertMaybeMultiple */
    Value& insertMaybeDefault(const Key &key) {
        return insertMaybe(key, Value());
    }

    /** Conditionally insert multiple key/value pairs.
     *
     *  Inserts each of the specified key/value pairs into this container where this container does not already contain a value
     *  for the key.  The return value is a reference to the container itself so that this method can be chained with others.
     *
     *  @sa insert insertDefault insertMaybe insertMaybeDefault insertMultiple */
    template<class OtherNodeIterator>
    HashMap& insertMaybeMultiple(const boost::iterator_range<OtherNodeIterator> &range) {
        for (OtherNodeIterator otherIter = range.begin(); otherIter != range.end(); ++otherIter)
            insertMaybe(Key(otherIter->key()), Value(otherIter->value()));
        return *this;
    }

    /** Remove all nodes.
     *
     *  All nodes are removed from this container. This method executes in linear time in the number of nodes in this
     *  container. */
    HashMap& clear() {
        map_.clear();
        return *this;
    }

    /** Remove a node with specified key.
     *
     *  Removes the node whose key is equal to the specified key, or does nothing if no such node exists.  Two keys are
     *  considered equal if this container's @ref Comparator object returns false reflexively.
     *
     *  @sa eraseMultiple eraseAt eraseAtMultiple */
    HashMap& erase(const Key &key) {
        map_.erase(key);
        return *this;
    }

    /** Remove keys stored in another HashMap.
     *
     *  All nodes of this container whose keys are equal to any key in the @p other container are removed from this container.
     *  The keys of the other container must be convertible to the types used by this container, and two keys are considered
     *  equal if this container's @ref Comparator object returns false reflexively.
     *
     *  @sa erase eraseAt eraseAtMultiple */
    template<class OtherKeyIterator>
    HashMap& eraseMultiple(const boost::iterator_range<OtherKeyIterator> &range) {
        for (OtherKeyIterator otherIter = range.begin(); otherIter != range.end(); ++otherIter)
            map_.erase(Key(*otherIter));
        return *this;
    }

    /** Remove a node by iterator.
     *
     *  Removes the node referenced by @p iter. The iterator must reference a valid node in this container.
     *
     *  @sa erase eraseMultiple eraseAtMultiple
     *
     *  @{ */
    HashMap& eraseAt(const NodeIterator &iter) {
        map_.erase(iter.base());
        return *this;
    }
    HashMap& eraseAt(const ConstKeyIterator &iter) {
        map_.erase(iter.base());
        return *this;
    }
    HashMap& eraseAt(const ValueIterator &iter) {
        map_.erase(iter.base());
        return *this;
    }
    /** @} */

    /** Remove multiple nodes by iterator range.
     *
     *  The iterator range must contain iterators that point into this container.
     *
     *  @sa erase eraseAt eraseMultiple
     *
     * @{ */
    template<class Iter>
    HashMap& eraseAtMultiple(const Iter &begin, const Iter &end) {
        map_.erase(begin.base(), end.base());
        return *this;
    }
    template<class Iter>
    HashMap& eraseAtMultiple(const boost::iterator_range<Iter> &range) {
        map_.erase(range.begin().base(), range.end().base());
        return *this;
    }
    /** @} */
};

} // namespace
} // namespace

#endif
