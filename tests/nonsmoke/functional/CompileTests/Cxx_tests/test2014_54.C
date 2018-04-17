#include <map>

template<class K, class T>
class Map {
public:
    typedef K Key;
    typedef T Value;

    typedef std::map<Key, Value> StlMap;

    class Node {};

    template<class Derived, class Value, class BaseIterator>
    class my_BidirectionalIterator: public std::iterator<std::bidirectional_iterator_tag, Value> {
    public:
        my_BidirectionalIterator();
    };

#if 0
    typedef my_BidirectionalIterator<NodeIterator, Node, typename StlMap::iterator> my_Super;
#endif

#if 1
    class NodeIterator: public my_BidirectionalIterator<NodeIterator, Node, typename StlMap::iterator> {
#if 1
        typedef                my_BidirectionalIterator<NodeIterator, Node, typename StlMap::iterator> Super;
#endif
#if 1
    public:
        NodeIterator();
#endif
    };
#endif
};

