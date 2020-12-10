#include <sage3basic.h>

#if ROSE_WITH_LIBHARU
#include "AstPDFGeneration.h"
#endif

#include "astGraph.h"
#include <list>
#include <analysisUtils.h>
#include <shortCircuitingTransformation.h>

#include <defaultFunctionGenerator.h>
#include <destructorCallAnnotator.h>

using namespace std;

//typedef list<SgConstructorInitializer *> in_list;
//typedef list<SgClassDeclaration *> temp_list;
typedef Rose_STL_Container<SgNode *> in_list;
typedef Rose_STL_Container<SgNode *> temp_list;

// Returns true iff the constructor initializer actually calls a constructor
// NOTE: currently returns wrong result for an array of objects with ctor
bool
isGoodCtorInit(SgConstructorInitializer *ci)
   {
     return (ci->get_class_decl() != 0);
   }

void addStmtVarsIfAny(in_list &objs, SgStatement *stmt)
   {
     if (SgVariableDeclaration *ds = isSgVariableDeclaration(stmt))
        {
          Rose_STL_Container<SgInitializedName *> &vars = ds->get_variables();
          for (Rose_STL_Container<SgInitializedName *>::iterator i = vars.begin(); i != vars.end(); ++i)
             {
               if (SgConstructorInitializer *ci = isSgConstructorInitializer(((*i)->get_initializer())))
                  {
                    if (isGoodCtorInit(ci))
                       {
            // objs.push_front(ci);
               objs.insert(objs.begin(),ci);
                       }
                  }
             }
        }
   }

template<class T>
std::ostream &operator<<(std::ostream &os, const Rose_STL_Container<T> &list)
   {
     os << "[";
     for (typename Rose_STL_Container<T>::const_iterator i = list.begin(); i != list.end(); ++i)
        {
          if (i != list.begin())
             {
               os << ",";
             }
          os << *i;
        }

     os << "]";
     return os;
   }

std::ostream &operator<<(std::ostream &os, SgNode *node)
   {
     os << node->class_name();
     if (SgLocatedNode *ln = isSgLocatedNode(node))
        {
          Sg_File_Info *fi = ln->get_file_info();
          os << "(" << fi->get_filename() << ":" << fi->get_line() << ":" << fi->get_col() << ") ";
        }
     os << node->unparseToCompleteString();
     return os;
   }

std::ostream &operator<<(std::ostream &os, SgInitializedName *node)
   {
     os << node->get_name().getString();
     return os;
   }

std::ostream &operator<<(std::ostream &os, SgConstructorInitializer *node)
   {
     if (SgInitializedName *in = isSgInitializedName(node->get_parent()))
        {
          os << in;
        }
     else
        {
          os << (SgNode *)node;
        }
     return os;
   }

class FindTemporaries : public AstSimpleProcessing
   {
     private:

          bool isGood(SgNode *n)
             {
               if (isSgAssignOp(n->get_parent()) || isSgAssignInitializer(n->get_parent()))
                  {
                    return false;
                  }
               if (SgConstructorInitializer *ci = isSgConstructorInitializer(n))
                  {
                    return isGoodCtorInit(ci);
                  }
               return true;
             }

     public:

          temp_list &temps;

          FindTemporaries(temp_list &_temps) : temps(_temps) {}

          void visit(SgNode *node)
             {
               cout << "node is a " << node->class_name() << endl;
               cout << "parent is a " << node->get_parent()->class_name() << endl;
               if (!isGood(node))
                  {
                    cout << "no good" << endl;
                    return;
                  }
               cout << "is good" << endl;
               if (SgConstructorInitializer *ci = isSgConstructorInitializer(node))
                  {
                    SgNode *parent = ci->get_parent();
                    if (!isSgInitializedName(parent) && !isSgNewExp(parent))
                       {
               SgClassDeclaration *cd = isSgClassDeclaration(ci->get_class_decl());
               ROSE_ASSERT(cd);

            // DQ (9/25/2007): Moved from std::list to std:vector uniformly in ROSE.
            // temps.push_front(cd);
               temps.insert(temps.begin(),cd);
                       }
                 // TODO: return values from functions etc
                  }
               else if (SgFunctionCallExp *fc = isSgFunctionCallExp(node))
                  {
                    SgType *retType = fc->get_type();
                    while (SgTypedefType *retTypedef = isSgTypedefType(retType))
                       {
                         retType = retTypedef->get_base_type();
                       }
                    if (SgClassType *retClsType = isSgClassType(retType))
                       {
                         SgClassDeclaration *retClsDecl = isSgClassDeclaration(retClsType->get_declaration());
                         ROSE_ASSERT(retClsDecl);
                         temps.push_back(retClsDecl);
                       }
                  }
             }

   };

in_list
blockObjectsAllocated(SgStatement *block, SgStatement *stop)
   {
     in_list objs;
     ROSE_ASSERT(block);
     switch (block->variantT())
        {
          case V_SgBasicBlock:
                {
                  SgBasicBlock *b = isSgBasicBlock(block);
                  ROSE_ASSERT(b);
                  
                  SgNode *parent = block->get_parent();
                  if (SgForStatement *fs = isSgForStatement(parent))
                     {
                        addStmtVarsIfAny(objs, fs->get_test());
                     }
                  else if (SgWhileStmt *ws = isSgWhileStmt(parent))
                     {
                       addStmtVarsIfAny(objs, ws->get_condition());
                     }
                  else if (SgIfStmt *is = isSgIfStmt(parent))
                     {
                       addStmtVarsIfAny(objs, is->get_conditional());

                       if (IsSCGenerated(is))
                          {
                            FindTemporaries ft(objs);
                            ft.traverse(is->get_conditional(), preorder);

                            if (b->get_statements().size() > 1)
                               {
                              // the first statement is the left conjunct of a ,
                              // expression... grab temporaries from this as well
                                 ft.traverse(b->get_statements().front(), preorder);
                               }
                          }
                     }

                  SgStatementPtrList &stmts = b->get_statements();
                  for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
                     {
                       if (*i == stop) break;
                       addStmtVarsIfAny(objs, *i);
                     }
                }
             break;
          case V_SgForStatement:
                {
                  SgForStatement *fs = isSgForStatement(block);
                  SgStatementPtrList &stmts = fs->get_init_stmt();
                  for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
                     {
                       if (*i == stop)
                          {
                            break;
                          }
                       addStmtVarsIfAny(objs, *i);
                     }
               // addStmtVarsIfAny(objs, fs->get_test());
                }
             break;
          default:
             break;
        }
     return objs;
   }
     
/*! Returns the list of objects allocated at the given statement, not including
  the given statement if a VariableDeclaration.  List in reverse order,
  i.e. order of destruction.  Does not proceed any further than a node satisfying stopCond.
  Itf omitInit true, omit objects allocated at the start of a loop which will not be 
 */
/* implementation technique: startOfCurrent helps ensure items inserted in reverse order for the current block, while blocks, visited from inner to outer are in the correct order  */
template<class StopCond>
in_list
objectsAllocated(SgStatement *statement, StopCond stopCond)
   {

     in_list objs;
     SgStatement *parent;
     do
        {
#if 0
          in_list::iterator startOfCurrent = objs.end();
#endif
          parent = isSgStatement(statement->get_parent());
          //cout << "parent = " << parent << endl;
          ROSE_ASSERT(parent);
          in_list blockObjs = blockObjectsAllocated(parent, statement);
          objs.insert(objs.end(), blockObjs.begin(), blockObjs.end());
#if 0
          switch (parent->variantT())
             {
               case V_SgBasicBlock:
                     {
                       SgBasicBlock *b = isSgBasicBlock(parent);
                       ROSE_ASSERT(b);
                       SgStatementPtrList &stmts = b->get_statements();
                       for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
                          {
                            if (*i == statement) break;
                            addStmtVarsIfAny(objs, startOfCurrent, *i);
                          }
                     }
                  break;
               case V_SgForStatement:
                     {
                       SgForStatement *fs = isSgForStatement(parent);
                       SgStatementPtrList &stmts = fs->get_init_stmt();
                       bool done = false;
                       if (!omitInit)
                          {
                            for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
                               {
                                 if (*i == statement)
                                    {
                                      done = true;
                                      break;
                                    }
                                 addStmtVarsIfAny(objs, startOfCurrent, *i);
                               }
                          }
                       if (!done)
                          {
                            if (fs->get_test() != statement)
                               {
                                 addStmtVarsIfAny(objs, startOfCurrent, fs->get_test());
                               }
                          }
                     }
                  break;
               default:
                  break;
             }
#endif
          statement = parent;
        } while (!stopCond(parent));
     return objs;
   }

/*
 * The list of objects which are allocated and are normally disposed of by the
 * inner block. This is to ensure that in cases such as
 * while (objtype foo = expr) { ... }
 * foo is properly disposed of should it evaluate to 0
 */
in_list
auxObjectsAllocated(SgStatement *stmt)
   {
     in_list objs;
     if (SgForStatement *fs = isSgForStatement(stmt))
        {
          addStmtVarsIfAny(objs, fs->get_test());
        }
     else if (SgWhileStmt *ws = isSgWhileStmt(stmt))
        {
          addStmtVarsIfAny(objs, ws->get_condition());
        }
     return objs;
   }
     
class IsFunctionDef : unary_function<SgNode *, bool>
   {
     public:
          bool operator()(SgNode *n)
             {
               //cout << "IsFunctionDef: looking at a " << n->class_name() << endl;
               bool ret = isSgFunctionDefinition(n);
               //cout << "returning " << ret << endl;
               return ret;
             }
   };

// if a break/continue is issued, does the buck stop here?
class IsBreakPoint : unary_function<SgNode *, bool>
   {
     public:
          bool operator()(SgNode *n)
             {
               return isSgForStatement(n) ||
                      isSgWhileStmt(n) ||
                      isSgDoWhileStmt(n) ||
                      isSgSwitchStatement(n) ||
                      isSgFunctionDefinition(n);
             }
   };

class ParentIsBreakPoint : unary_function<SgNode *, bool>
   {
     public:
          bool operator()(SgNode *n)
             {
               return IsBreakPoint()(n->get_parent());
             }
   };

class ShowObjectsAllocated : public AstSimpleProcessing
   {
     public:

          void visit(SgNode *node)
             {
               if (SgStatement *stmt = isSgStatement(node))
                  {
                    cout << "Statement " << stmt << endl;
                    cout << "objectsAllocated = " << objectsAllocated(stmt, IsFunctionDef()) << endl;
                  }
             }
   };

template<typename T>
struct GenericAttr : AstAttribute
   {
     T attr;
   };

template<typename T>
inline T &GetAttribute(SgNode *n, const std::string& name)
   {
     if (n->attributeExists(name))
        {
          return static_cast<GenericAttr<T> *>(n->getAttribute(name))
                  ->attr;
        }
     else
        {
          GenericAttr<T> *attr = new GenericAttr<T>;
          n->addNewAttribute(name, attr);
          return attr->attr;
        }
   }


struct ObjectsAllocatedAttr : AstAttribute
   {
     in_list objects;
   };

#define ATTR_ACCESSOR(type, name) \
type &name(SgNode *n) \
   { \
     return GetAttribute<type>(n, #name); \
   }

ATTR_ACCESSOR(in_list, ObjectsAllocated)
ATTR_ACCESSOR(in_list, AuxObjectsAllocated)
ATTR_ACCESSOR(temp_list, Temporaries)

#if 0
     if (n->attributeExists("ObjectsAllocated"))
        {
          return static_cast<ObjectsAllocatedAttr *>(n->getAttribute("ObjectsAllocated"))
                  ->objects;
        }
     else
        {
          ObjectsAllocatedAttr *attr = new ObjectsAllocatedAttr;
          n->addNewAttribute("ObjectsAllocated", attr);
          return attr->objects;
        }
   }

in_list &AuxObjectsAllocated(SgNode *n)
   {
     if (n->attributeExists("AuxObjectsAllocated"))
        {
          return static_cast<ObjectsAllocatedAttr *>(n->getAttribute("AuxObjectsAllocated"))
                  ->objects;
        }
     else
        {
          ObjectsAllocatedAttr *attr = new ObjectsAllocatedAttr;
          n->addNewAttribute("AuxObjectsAllocated", attr);
          return attr->objects;
        }
   }
#endif

class Annotator : public AstSimpleProcessing
   {
     public:

          void visit(SgNode *node)
             {
               if (SgScopeStatement *ss = isSgScopeStatement(node))
                  {
                    ObjectsAllocated(ss) = blockObjectsAllocated(ss, NULL);
                    AuxObjectsAllocated(ss) = auxObjectsAllocated(ss);
                  }
               else if (SgBreakStmt *bs = isSgBreakStmt(node))
                  {
                    ObjectsAllocated(node) = objectsAllocated(bs, IsBreakPoint());
                  }
               else if (SgContinueStmt *cs = isSgContinueStmt(node))
                  {
                    ObjectsAllocated(node) = objectsAllocated(cs, ParentIsBreakPoint());
                  }
               else if (SgReturnStmt *rs = isSgReturnStmt(node))
                  {
                    ObjectsAllocated(node) = objectsAllocated(rs, IsFunctionDef());
                  }
               else if (SgGotoStatement *gs = isSgGotoStatement(node))
                  {
                    in_list gotoList = objectsAllocated(gs, IsFunctionDef());
                    in_list labelList = objectsAllocated(gs->get_label(), IsFunctionDef());
                    in_list::iterator gi = gotoList.end(), li = labelList.end();
                    while (li != labelList.begin()) 
                       {
                         if (gi == gotoList.begin() || *--gi != *--li)
                            {
                              cout << "labelList = " << labelList << endl;
                              cout << "gotoList = " << gotoList << endl;
                              cout << "labelList is not a subset of gotoList!" << endl;
                              ROSE_ASSERT(false);
                            }
                       }

                    in_list &gotoDiff = ObjectsAllocated(node);
                    gotoDiff.insert(gotoDiff.begin(), gotoList.begin(), gi);
                  }
               else if (node->variantT() == V_SgInitializedName)
                  {
                    cout << "XXX Not Skipping " << node << endl;
                    FindTemporaries ft(Temporaries(node));
                    ft.traverse(node, preorder);
                  }
               else
                  {
                    return;
                  }
               // cout << "Node " << node << endl;
               // cout << "ObjectsAllocated are " << ObjectsAllocated(node) << endl;
             }
   };

template<typename NodeType>
NodeType *&findReference(NodeType *node)
   {
     static NodeType *nil = 0;
     SgNode *parent = node->get_parent();
     if (SgBasicBlock *bb = isSgBasicBlock(parent))
        {
          SgStatementPtrList &stmts = bb->get_statements();
          for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
             {
               if (*i == node)
                  {
                    return (NodeType *&)(*i);
                  }
             }
        }
     return nil;
   }

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

SgMemberFunctionRefExp *buildDtorRefExpr(SgClassDeclaration *decl)
   {
     SgClassDeclaration *defDecl = isSgClassDeclaration(decl->get_definingDeclaration());
     ROSE_ASSERT(defDecl);
     SgClassDefinition *def = defDecl->get_definition();

     string dtorName = "~";
     if (SgTemplateInstantiationDecl *tmplDecl = isSgTemplateInstantiationDecl(decl))
        {
          dtorName += tmplDecl->get_templateName();
        }
     else
        {
          dtorName += decl->get_name();
        }
          
     SgMemberFunctionSymbol *dtor = isSgMemberFunctionSymbol(def->lookup_function_symbol(dtorName));
     cout << "dtorName = " << dtorName << ", dtor = " << (void *)dtor->get_declaration() << endl;
     //ROSE_ASSERT(dtor);
     if (!dtor)
        {
          cout << "Class has no dtor!" << endl;
#if ROSE_WITH_LIBHARU
          AstPDFGeneration pdf;
          pdf.generate("noDtor", decl);
#else
          cout << "Warning: libharu support is not enabled" << endl;
#endif
          ROSE_ASSERT(false);
        }
     SgMemberFunctionRefExp *ref = new SgMemberFunctionRefExp(SgNULL_FILE, dtor, false, dtor->get_declaration()->get_type(), false);
     return ref;
   }

SgDotExp *buildDtorDotExpr(SgInitializedName *in)
   {
     SgVariableSymbol *var = in->get_scope()->lookup_var_symbol(in->get_name());
     ROSE_ASSERT(var);
     SgVarRefExp *vref = new SgVarRefExp(SgNULL_FILE, var);
     SgMemberFunctionRefExp *dtorRef = buildDtorRefExpr(isSgConstructorInitializer(in->get_initializer())->get_class_decl());
     SgDotExp *dtorDot = new SgDotExp(SgNULL_FILE, vref, dtorRef, SgTypeVoid::createType());
     vref->set_parent(dtorDot);
     dtorRef->set_parent(dtorDot);
     return dtorDot;
   }

SgExpression *buildDestructorExpr(SgNode *n)
   {
     SgExpression *ref;
     if (SgInitializedName *in = isSgInitializedName(n->get_parent()))
        {
          ref = buildDtorDotExpr(in);
        }
     else if (SgConstructorInitializer *ci = isSgConstructorInitializer(n))
        {
          if (SgInitializedName *in = isSgInitializedName(ci->get_parent()))
             {
               ref = buildDtorDotExpr(in);
             }
          else
             {
               ref = buildDtorRefExpr(ci->get_class_decl());
             }
        }
     else if (SgClassDeclaration *cd = isSgClassDeclaration(n))
        {
          ref = buildDtorRefExpr(cd);
        }
     else
        {
          ROSE_ASSERT(false);
        }

     SgExprListExp *dtorParams = new SgExprListExp(SgNULL_FILE);
     SgFunctionCallExp *dtorCall = new SgFunctionCallExp(SgNULL_FILE, ref, dtorParams, SgTypeVoid::createType());
     ref->set_parent(dtorCall);
     dtorParams->set_parent(dtorCall);
     return dtorCall;
   }

SgStatement *buildDestructorStmt(SgNode *n)
   {
     SgExpression *dtorExpr = buildDestructorExpr(n);
     SgExprStatement *dtorExprStmt = new SgExprStatement(SgNULL_FILE, dtorExpr);
     dtorExpr->set_parent(dtorExprStmt);
     return dtorExprStmt;
   }

template <typename NodeType>
void destroyTemporariesInFollowingPositions(NodeType *n, SgNode *tempSrc)
   {
     temp_list &temps = Temporaries(tempSrc);
     stmt_pos_list poss = findFollowingPositions(n);
     if (poss.empty() && !temps.empty()) {
          SgStatement *parentStmt = isSgStatement(tempSrc->get_parent());
          if (parentStmt != NULL)
             {
               if (SgForInitStatement *forInitStmt = isSgForInitStatement(parentStmt->get_parent()))
                  {
                    SgForStatement *forStmt = isSgForStatement(forInitStmt->get_parent());
                    ROSE_ASSERT(forStmt != NULL);
                    stmt_pos forStmtPos = findPosition(forStmt);

                    SgStatementPtrList &forInitStmtList = forInitStmt->get_init_stmt();
       // SgStatementPtrList::iterator parentStmtPos = findIterator(forInitStmtList, parentStmt);

       // DQ (9/25/2007): Moved from std::list to std::vector
       // forInitStmtList.remove(parentStmt);
          forInitStmtList.erase(find(forInitStmtList.begin(),forInitStmtList.end(),parentStmt));

                    forStmtPos.first->insert(forStmtPos.second, parentStmt);
                    parentStmt->set_parent(forStmtPos.stmtParent);

                    poss = findFollowingPositions(n);
                  }
             }
                    
          if (poss.empty())
             {
          cout << "WARNING: temporary created in statement after which it is" << endl
               << "impossible to call destructor, and I could not correct the" << endl
               << "problem.  Some destructors not called." << endl;
             }
     }
     for (stmt_pos_list::iterator i = poss.begin(); i != poss.end(); ++i)
        {
          for (temp_list::iterator j = temps.begin(); j != temps.end(); ++j)
             {
               SgStatement *dtorStmt = buildDestructorStmt(*j);
               i->first->insert(i->second, dtorStmt);
               dtorStmt->set_parent(i->stmtParent);
             }
        }
   }

class Transformer : public AstSimpleProcessing
   {
     public:

          void visit(SgNode *node)
             {
               if (SgBasicBlock *bb = isSgBasicBlock(node))
                  {
                    in_list &ctors = ObjectsAllocated(bb);
                    for (in_list::iterator i = ctors.begin(); i != ctors.end(); ++i)
                       {
                         SgStatement *dtorStmt = buildDestructorStmt(*i);
                         bb->append_statement(dtorStmt);
                         dtorStmt->set_parent(bb);
                       }
                  }
               else if (node->variantT() == V_SgForStatement || node->variantT() == V_SgWhileStmt)
                  {
                    SgStatement *stmt = isSgStatement(node);
                    ROSE_ASSERT(stmt);
                    
                    stmt_pos pos = findPosition(stmt);
                    pos.second++;

                    in_list &auxCtors = AuxObjectsAllocated(stmt);
                    for (in_list::iterator i = auxCtors.begin(); i != auxCtors.end(); ++i)
                       {
                         SgStatement *dtorStmt = buildDestructorStmt(*i);
                         pos.first->insert(pos.second, dtorStmt);
                         dtorStmt->set_parent(stmt->get_parent());
                       }

                    in_list &ctors = ObjectsAllocated(stmt);
                    for (in_list::iterator i = ctors.begin(); i != ctors.end(); ++i)
                       {
                         SgStatement *dtorStmt = buildDestructorStmt(*i);
                         pos.first->insert(pos.second, dtorStmt);
                         dtorStmt->set_parent(stmt->get_parent());
                       }
                  }
               else if (node->variantT() == V_SgBreakStmt || node->variantT() == V_SgContinueStmt || node->variantT() == V_SgGotoStatement)
                  {
                    SgStatement *stmt = isSgStatement(node);
                    in_list &ctors = ObjectsAllocated(stmt);

                    if (ctors.empty())
                       {
                         return;
                       }

                    SgStatement *&stmtRef = findReference(stmt);
                    ROSE_ASSERT(stmtRef);

                    SgBasicBlock *newBlock = new SgBasicBlock(SgNULL_FILE);
                    for (in_list::iterator i = ctors.begin(); i != ctors.end(); ++i)
                       {
                         newBlock->append_statement(buildDestructorStmt(*i));
                       }
                    cout << "before: " << isSgBasicBlock(stmt->get_parent())->get_statements() << endl;
                    newBlock->set_parent(stmt->get_parent());
                    newBlock->append_statement(stmt);
                    stmtRef = newBlock;
                    //cout << "after: " << isSgBasicBlock(stmt->get_parent())->get_statements() << endl;
                  }
               else if (SgReturnStmt *retStmt = isSgReturnStmt(node))
                  {
                    in_list &ctors = ObjectsAllocated(retStmt);
                    SgExpression *retExp = retStmt->get_expression();
                    if (retExp != NULL)
                       {
                         for (in_list::iterator i = ctors.begin(); i != ctors.end(); ++i)
                            {
                              SgExpression *dtorExp = buildDestructorExpr(*i);
                              ROSE_ASSERT(dtorExp);
                              retExp = new SgCommaOpExp(SgNULL_FILE, retExp, dtorExp);
                              dtorExp->set_parent(retExp);
                            }

                         retStmt->set_expression(retExp);
                         retExp->set_parent(retStmt);
                       }
                    else
                       {
                         stmt_pos retPos = findPosition(retStmt);
                         SgBasicBlock *stmtBB = isSgBasicBlock(retPos.stmtParent);
                         ROSE_ASSERT(stmtBB != NULL);

                         for (in_list::iterator i = ctors.begin(); i != ctors.end(); ++i)
                            {
                              SgStatement *dtorStmt = buildDestructorStmt(*i);
                              ROSE_ASSERT(dtorStmt != NULL);
                              stmtBB->get_statements().insert(retPos.second, dtorStmt);
                              dtorStmt->set_parent(stmtBB);
                            }
                       }
                  }
               else if (SgInitializedName *iname = isSgInitializedName(node))
                  {
                    destroyTemporariesInFollowingPositions(iname, iname);
#if 0
                    temp_list &temps = Temporaries(iname);
                    stmt_pos_list poss = findFollowingPositions(iname);
                    if (poss.empty() && !temps.empty()) {
                         cout << "WARNING: temporary created in statement after which it is" << "impossible to call destructor, e.g. initialization of for statement." << endl << "Use -dft:moveForInitializers flag." << endl;
                         // TODO: cheat and add change params into SgCommaOpExp... not exactly right but the best we can do
                    }
                    for (stmt_pos_list::iterator i = poss.begin(); i != poss.end(); ++i)
                       {
                         for (temp_list::iterator j = temps.begin(); j != temps.end(); ++j)
                            {
                              i->first->insert(i->second, buildDestructorStmt(*j));
                            }
                       }
#endif
                  }
             }
   };

// BEGIN STOLEN from wholeGraphAST.C

// This functor is derived from the STL functor mechanism
struct customFilter: public std::unary_function< bool, pair< SgNode*, std::string>& >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     bool operator() ( AST_Graph::NodeType & x );
   };


bool
customFilter::operator()(pair<SgNode*,std::string>& x)
   {
     bool returnValue = true;
#if 1
     if (isSg_File_Info(x.first) != NULL)
          returnValue = false;
#endif
#if 1
     if (isSgSymbol(x.first) != NULL)
          returnValue = false;
#endif
#if 1
     if (isSgTypedefSeq(x.first) != NULL)
          returnValue = false;
#endif
#if 1
     if (isSgStorageModifier(x.first) != NULL)
          returnValue = false;
#endif
#if 0
     if ( x.second == "parent" )
          returnValue = false;
#endif
#if 1
     SgLocatedNode* locatedNode = isSgLocatedNode(x.first);
     if (locatedNode != NULL)
        {
          Sg_File_Info* fileInfo = locatedNode->get_file_info();
          std::string filename(Rose::utility_stripPathFromFileName(fileInfo->get_filename()));

          if (filename.find("rose_edg_macros_and_functions_required_for_gnu.h") != std::string::npos)
             {
               returnValue = false;
             }

          if (fileInfo->isCompilerGenerated()==true)
             {
            // std::cout << "Is compiler generated\n";
               returnValue = false;
             }
        }
#endif

     return returnValue;
   }

// END STOLEN

void moveForInitializers(SgForStatement *forStmt)
   {
     SgBasicBlock *newBlock = new SgBasicBlock(SgNULL_FILE);

     SgStatementPtrList &initStmts = forStmt->get_init_stmt();
     SgStatementPtrList &blockStmts = newBlock->get_statements();

     blockStmts.insert(blockStmts.begin(), initStmts.begin(), initStmts.end());
     blockStmts.push_back(forStmt);

     stmt_pos forPos = findPosition(forStmt);

     newBlock->set_parent(forStmt->get_parent());

     for (SgStatementPtrList::iterator i = initStmts.begin(); i != initStmts.end(); ++i)
        {
          if (SgDeclarationStatement *declStmt = isSgDeclarationStatement(*i))
             {
               declStmt->get_declarationModifier().get_storageModifier().setDefault();
             }
          (*i)->set_parent(newBlock);
        }
     forStmt->set_parent(newBlock);

     *(forPos.second) = newBlock;
     initStmts.clear();
   }

void destructorCallAnnotator(SgProject *prj)
   {
     Rose_STL_Container<SgNode *> forStmts = NodeQuery::querySubTree(prj, V_SgForStatement);

     for (Rose_STL_Container<SgNode *>::iterator i = forStmts.begin(); i != forStmts.end(); ++i)
        {
          SgForStatement *forStmt = isSgForStatement(*i);
          ROSE_ASSERT(forStmt);
          moveForInitializers(forStmt);
        }

     Rose_STL_Container<SgNode *> defs = NodeQuery::querySubTree(prj, V_SgFunctionDefinition);

     for (Rose_STL_Container<SgNode *>::iterator i = defs.begin(); i != defs.end(); ++i)
        {
          Annotator a;
          a.traverse(isSgFunctionDefinition(*i)->get_body(), preorder);

          Transformer t;
          t.traverse(isSgFunctionDefinition(*i)->get_body(), postorder);
        }
   }
     
#if 0
int main(int argc, char **argv)
   {
     SgProject *prj = frontend(argc, argv);

     AstPDFGeneration pdf;
     pdf.generate("annotatorStage0", prj);

     DefaultFunctionGenerator dfg;
     //dfg.traverseInputFiles(prj, preorder);
     dfg.traverse(prj, preorder);

     pdf.generate("annotatorStage1", prj);

     AstPostProcessing(prj);

     pdf.generate("annotatorStage2", prj);

     shortCircuitingTransformation(prj);

     pdf.generate("annotatorStage3", prj);

     if (true)
        {
          list<SgNode *> forStmts = NodeQuery::querySubTree(prj, V_SgForStatement);

          for (list<SgNode *>::iterator i = forStmts.begin(); i != forStmts.end(); ++i)
             {
               SgForStatement *forStmt = isSgForStatement(*i);
               ROSE_ASSERT(forStmt);
               moveForInitializers(forStmt);
             }
        }

     pdf.generate("annotatorStage4", prj);

     list<SgNode *> defs = NodeQuery::querySubTree(prj, V_SgFunctionDefinition);

     for (list<SgNode *>::iterator i = defs.begin(); i != defs.end(); ++i)
        {
#if 0
          ShowObjectsAllocated soa;
          soa.traverse(isSgFunctionDefinition(*i)->get_body(), preorder);
#else
          Annotator a;
          a.traverse(isSgFunctionDefinition(*i)->get_body(), preorder);

          Transformer t;
          t.traverse(isSgFunctionDefinition(*i)->get_body(), postorder);
#endif
        }

     pdf.generate("annotatorStage5", prj);

     //AstPDFGeneration pdf;
     //pdf.generate("foo.pdf", prj);

     //AST_Graph::writeGraphOfAstSubGraphToFile("wholeGraphAST.dot",prj,customFilter());

     prj->get_file(0).set_unparse_includes(true);

     prj->unparse();
   }
#endif
