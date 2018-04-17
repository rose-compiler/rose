
struct sockaddr{};
struct sockaddr_in{};

#if 0
enum {
    LSA_LEN_SIZE = 0, // offsetof(len_and_sockaddr, u),
    LSA_SIZEOF_SA = sizeof(
        union {
            struct sockaddr sa;
            struct sockaddr_in sin;
#if ENABLE_FEATURE_IPV6
            struct sockaddr_in6 sin6;
#endif
        } NEW_UNION_TYPE_NAME  // give the anonymous union type a name, is this legal as a sizeof operand
    ),
NEW_ENUM_ITEM = sizeof(NEW_UNION_TYPE_NAME)    // If this is legal then this is extra ugly
};
#endif


enum XXX {
    LSA_LEN_SIZE = 0,
    LSA_SIZEOF_SA = sizeof(
        struct NEW_UNION_TYPE_NAME {
            struct sockaddr sa;
            struct sockaddr_in sin;
        }
    ),

   NEW_ENUM_ITEM = sizeof(int)    // If this is legal then this is extra ugly
// NEW_ENUM_ITEM = sizeof(NEW_UNION_TYPE_NAME)    // If this is legal then this is extra ugly
};

// NEW_UNION_TYPE_NAME x;
