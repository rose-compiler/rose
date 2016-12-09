#ifndef ROSE_AstSerialization_H
#define ROSE_AstSerialization_H

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB

#include <Cxx_GrammarSerialization.h>                   // the compile-time generated support

namespace rose {

/** Save an AST to an archive.
 *
 *  Since each ROSE AST node has a parent pointer that's stored/restored during serialization, all nodes reachable by these
 *  parent pointers are also saved.  In order to help limit the scope of what's saved, this function temporarily clears the
 *  parent pointer of the root of the specified AST (which makes this function not thread-safe).
 *
 *  The archive is usually @c binary_oarchive, @c binary_iarchive, @c text_oarchive, or @c text_iarchive from the @c
 *  boost::archive namespace, but any @c Archive class that follows that interface will work.
 *
 *  In order for a Sage node to be serializable a couple of things need to be declared in the src/ROSETTA/src files where
 *  serializable @ref AstNodeClass are defined: first, the @ref SgNodeClass::isSerialiable property needs to be set, and
 *  second, a @c serialize function template needs to be defined. The @c serialize function always looks the same, namely:
 *
 *  @code
 *  #ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
 *  private:
 *      friend class boost::serialization::access;
 *
 *      template<class Archive>
 *      void serialize<Archive &archive, const unsigned version) {
 *          archive & boost::serialization::base_object<TheBaseClass>(*this);
 *          archive & p_member1 & p_member2 & ... ;
 *      }
 *  #endif
 *  @endcode
 *
 *  where @c TheBaseClass is the Sage node class from which this node class is derived, and @c p_member1 etc. are the data
 *  members that should be saved and restored.
 *
 *  Also, the class must have a default constructor, although that constructor may be protected if it's not suitable for end
 *  users.
 *
 *  The ROSE_HAVE_BOOST_SERIALIZATION_LIB protection is because some serialization templates might reference symbols from the
 *  optional boost_serialization library in a way that causes them to be required even if the template is never
 *  instantiated. This was seen only with Boost version 1.61, so it may have been a boost bug. */
template<class Archive>
void saveAst(Archive &archive, SgNode *ast) {
    struct T1 {                                         // for exception safe clearing and restoring the root's parent
        SgNode *node, *parent;
        T1(SgNode *node): node(node), parent(node ? node->get_parent() : NULL) {
            if (node)
                node->set_parent(NULL);
        }
        ~T1() {
            if (node)
                node->set_parent(parent);
        }
    };

    T1 saveRestoreRootParent(ast);
    roseAstSerializationRegistration(archive);
    archive <<ast;
}

/** Restore AST from an archive.
 *
 *  Since the parent node of the root of the AST is not stored, it will be a null pointer when restored.
 *
 *  See @ref saveAst for details and requirements. */
template<class Archive>
SgNode* restoreAst(Archive &archive) {
    roseAstSerializationRegistration(archive);
    SgNode *ast = NULL;
    archive >>ast;
    return ast;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partial specialization for SgNode until we're able to add SgNode::serialize. Eventually all the node's data members should
// be serialized. These three functions are more complicated here than if we move them to SgNode because they have to deal with
// the fact that the data members they're trying to serialize are protected.
namespace boost {
namespace serialization {

template<class Archive>
inline void save(Archive &archive, const SgNode &node, const unsigned version) {
    SgNode *parent = node.get_parent();
    bool isModified = node.get_isModified();
    bool containsTransformation = node.get_containsTransformation();
    archive <<parent <<isModified <<containsTransformation;
}

template<class Archive>
inline void load(Archive &archive, SgNode &node, const unsigned version) {
    SgNode *parent = NULL;
    bool isModified=false, containsTransformation=false;
    archive >>parent >>isModified >>containsTransformation;
    node.set_parent(parent);
    node.set_isModified(isModified);
    node.set_containsTransformation(containsTransformation);
}

template<class Archive>
inline void serialize(Archive &archive, SgNode &node, const unsigned version) {
    boost::serialization::split_free(archive, node, version);
}

} // namespace
} // namespace

#endif
#endif
