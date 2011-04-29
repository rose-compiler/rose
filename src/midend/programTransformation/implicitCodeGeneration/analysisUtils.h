
#include <list>
#include <utility>

//typedef std::pair<SgStatementPtrList *, SgStatementPtrList::iterator> stmt_pos;
struct stmt_pos {
     stmt_pos(SgStatementPtrList *const &first, const SgStatementPtrList::iterator &second,
              SgStatement *const &stmtParent)
             : first(first), second(second), stmtParent(stmtParent) { }
     SgStatementPtrList *first; // these names are because this object used to be a pair.. FIXME
     SgStatementPtrList::iterator second;
     SgStatement *stmtParent;
};

typedef std::list<stmt_pos> stmt_pos_list;
stmt_pos findPosition(SgNode *n);
stmt_pos_list findFollowingPositions(SgInitializedName *in);
stmt_pos_list findFollowingPositions(SgStatement *stmt);
stmt_pos_list findPrecedingPositions(SgStatement *stmt);

template <typename T, T *(isT)(SgNode *)>
inline std::pair<T *, SgNode *> findNearestParentOfType(SgNode *node)
   {
     while (true)
        {
          ROSE_ASSERT(node != NULL);
          SgNode *nodeParent = node->get_parent();

          T *typedNode = isT(nodeParent);
          if (typedNode != NULL)
             {
               return std::make_pair(typedNode, node);
             }
          node = nodeParent;
        }
   }

template<typename ElemType, typename ListType>
inline typename ListType::iterator findIterator(ListType &list, ElemType &elem)
   {
     for (typename ListType::iterator i = list.begin(); i != list.end(); ++i)
        {
          if (*i == elem)
             {
               return i;
             }
        }
     return list.end();
   }

SgStatement *findStatementForExpression(SgExpression *expr);
std::pair<SgBasicBlock *, SgNode *> findBasicBlockForStmt(SgStatement *stmt);
SgStatement *findIfStmtForSC(SgExpression *expr);
