#include "rose.h"

#include "sageDoxygen.h"

// #include <iostream>
// #include <stdio.h>
// #include <string>
// #include <functional>
// #include <limits.h>

using namespace std;

std::map<const PreprocessingInfo*, DoxygenComment* > commentMap;






//AS(090707) The template PreprocessingInfoToDoxygenComment<> will take two PreprocessingInfo* and
//call a predicate with the corresponding DoxygenComment pointers. The only purpose of this class is to
//act as an intermediary between the PreprocessingInfo* and the DoxygenComment*. If no DoxygenComment*
//correspond to the PreprocessingInfo* the DoxygenComment* will be NULL.

class PreprocessingInfoToDoxygenComment : public std::binary_function<PreprocessingInfo*,PreprocessingInfo*,bool>
   {
     public:

	  PreprocessingInfoToDoxygenComment(std::map<const PreprocessingInfo*, DoxygenComment* >* cmap,
			 bool (*p)(DoxygenComment*,DoxygenComment*)) : commentMap(cmap), pred(p) {}

	  bool operator()(const PreprocessingInfo* a,const PreprocessingInfo* b) const
	     { 
	       std::map<const PreprocessingInfo*, DoxygenComment* >::iterator firstArg  = commentMap->find(a);
	       std::map<const PreprocessingInfo*, DoxygenComment* >::iterator secondArg = commentMap->find(b);


	       DoxygenComment* a2;
	       if( firstArg == commentMap->end() )
		    a2 = NULL;
	       else 
		    a2 = firstArg->second;

	       DoxygenComment* b2;
	       if( secondArg == commentMap->end() )
		    b2 = NULL;
	       else 
		    b2 = secondArg->second;

	       return pred(a2, b2); 
	     
	     }

     private:
          std::map<const PreprocessingInfo*, DoxygenComment* >* commentMap; 
	  bool (*pred)(DoxygenComment*, DoxygenComment*);
   };


//AS(090707) The IsDoxygenCommentNotGroup class will determine id a DoxygenComment* is not
//a group.

class IsDoxygenCommentNotGroup : public std::unary_function<PreprocessingInfo*, bool>
   {
     public:

	  IsDoxygenCommentNotGroup(std::map<const PreprocessingInfo*, DoxygenComment* >* cmap) 
		  : commentMap(cmap) {}

	  bool operator()(const PreprocessingInfo* a) const
	     { 
	       std::map<const PreprocessingInfo*, DoxygenComment* >::iterator firstArg  = commentMap->find(a);


	       // DoxygenComment* a2;
	       if( firstArg == commentMap->end() )
		    return false;
	       else if( firstArg->second->entry.type() != DoxygenEntry::None )
		    return true;
	       else
		    return false;

	     
	     }

     private:
          std::map<const PreprocessingInfo*, DoxygenComment* >* commentMap; 
   };




void
DoxygenFile::sortComments( bool (*pred)(DoxygenComment*,DoxygenComment*) )
   {

     ROSE_ASSERT(isSgDeclarationStatement(commentParent) != NULL );
     //Create map between PreprocessingInfo objects and DoxygenComments
     // AttachedPreprocessingInfoType* comments = commentParent->getAttachedPreprocessingInfo();
     PreprocessingInfoToDoxygenComment convert(&commentMap, pred  );

     IsDoxygenCommentNotGroup isDox(&commentMap) ;

	 for( std::map<std::string, DoxygenGroup *>::iterator gItr = groups.begin();
		 gItr != groups.end(); ++ gItr ){
	   DoxygenGroup* doxGroup = gItr->second;

	   //Find the iterator to the beginning and end of this group in AttachedPreprocessingInfoType
	   //of the original AST node.
	   AttachedPreprocessingInfoType* currentComments
		 = doxGroup->comment->originalNode->getAttachedPreprocessingInfo();

	   AttachedPreprocessingInfoType::iterator beg_group = 
		 std::find(currentComments->begin(), currentComments->end(), doxGroup->groupStart);
	   ROSE_ASSERT( beg_group != currentComments->end() );
	   AttachedPreprocessingInfoType::iterator end_group = 
		 std::find(beg_group, currentComments->end(), doxGroup->groupEnd);
	   ROSE_ASSERT( end_group != currentComments->end() );

	   //Find the beginning and end of the DoxygenComments that is not the comment for the group start '//@{'
	   //or group end '//@}'
       std::vector<PreprocessingInfo*>::iterator itBeg = find_if( beg_group, end_group,isDox );
	   ROSE_ASSERT( itBeg != end_group );
	   std::vector<PreprocessingInfo*>::iterator itEnd = ( --find_if( itBeg, end_group,not1<IsDoxygenCommentNotGroup>(isDox) ));
       ROSE_ASSERT( itEnd != end_group );
	   sort(itBeg,itEnd,convert);

	 }

   }

string
Doxygen::getDeclStmtName(SgDeclarationStatement *st)
   {
     SgFunctionDeclaration *fn = isSgFunctionDeclaration(st);
     SgVariableDeclaration *vn = isSgVariableDeclaration(st);
     SgClassDeclaration *cn = isSgClassDeclaration(st);
     if (fn)
        {
          return fn->get_name().str();
        }
     else if (vn)
        {
       /* XXX The following is a bit dodgy since single
        * SgVariableDeclaration may declare multiple
        * variables.  But doxygen doesn't handle this case
        * properly.  A useful transform may split up the
        * SgVariableDeclaration for doxygen's sake */
          return (*(vn->get_variables().begin()))->get_name().str();
        }
     else if (cn)
        {
          return cn->get_name().str();
        }
     else
        {
          fprintf(stderr, "Cannot handle this case: %s\n", st->sage_class_name());
          abort();
        }
   }

string
Doxygen::getProtoName(SgDeclarationStatement *st)
   {
     SgScopeStatement *scope;
     if (SgVariableDeclaration* varDeclSt = isSgVariableDeclaration(st))
        {
       // TODO: uncomment SgVariableDeclaration::get_scope removing need for this code
          scope = varDeclSt->get_variables().front()->get_scope();
        }
     else
        {
          scope = st->get_scope();
        }
     if (isSgGlobal(scope))
        {
          return getDeclStmtName(st);
        }
     else
        {
          SgUnparse_Info info;
          info.set_SkipSemiColon();
          info.set_SkipFunctionDefinition();
          info.set_forceQualifiedNames();
          info.set_skipCheckAccess();
          info.set_SkipInitializer();
          info.set_SkipClassSpecifier();
	  //AS(091507) In the new version of ROSE global qualifiers are paret of the qualified name of
	  //a scope statement. For the documentation work we do not want that and we therefore use string::substr()
	  //to trim of  "::" from the front of the qualified name.

	  if( scope->get_qualified_name().getString().length() > 2 )
	     {
                  return scope->get_qualified_name().getString().substr(2)+("::"+getDeclStmtName(st));
          }else{
                   return getDeclStmtName(st);

	  }

	  //return scope->get_qualified_name().str()+("::"+getDeclStmtName(st));
        }
   }


string
Doxygen::getQualifiedPrototype(SgNode *node)
   {
     SgClassDeclaration *cd = dynamic_cast<SgClassDeclaration *>(node);
     if (cd) return getProtoName(cd);
     SgUnparse_Info info;
     info.set_SkipSemiColon();
     info.set_SkipFunctionDefinition();
     info.set_forceQualifiedNames();
     info.set_skipCheckAccess();
     info.set_SkipClassSpecifier();
     info.set_SkipInitializer();
     info.set_current_scope(TransformationSupport::getGlobalScope(node));

     string proto = node->unparseToString(&info);
     while (proto[proto.size()-1] == '\n') { /* sometimes the prototype will be unparsed with a newline at the end */
          proto.resize(proto.size()-1);
     }
     return proto;
   }

bool
Doxygen::isRecognizedDeclaration(SgDeclarationStatement *n)
   {
     if (!n)
        {
          return false;
        }
     if (n->get_file_info()->isCompilerGenerated())
        {
          return false;
        }
     if (SgClassDefinition *cdef = isSgClassDefinition(n->get_scope()))
        {
          if (!isRecognizedDeclaration(cdef->get_declaration()))
             {
               return false;
             }
        }

     if (isSgTemplateInstantiationDecl(n)
      || isSgTemplateInstantiationFunctionDecl(n)
      || isSgTemplateInstantiationMemberFunctionDecl(n))
        {
          return false;
        }

     if (SgFunctionDeclaration *fd = isSgFunctionDeclaration(n))
        {
          if (fd->get_specialization() != SgDeclarationStatement::e_no_specialization)
             {
               return false;
             }
          return true;
        }
     if (SgVariableDeclaration *vd = isSgVariableDeclaration(n))
        {
       // skip any function-local variable declarations
          SgScopeStatement *scope = vd->get_variables().front()->get_scope();
          if (isSgBasicBlock(scope) || isSgFunctionDefinition(scope)
           || isSgForStatement(scope) || isSgIfStmt(scope) || isSgWhileStmt(scope))
             {
               return false;
             }

          if (vd->get_specialization() != SgDeclarationStatement::e_no_specialization)
             {
               return false;
             }


          SgInitializedNamePtrList &v = vd->get_variables();
          for (SgInitializedNamePtrList::iterator i = v.begin(); i != v.end(); ++i)
             {
			   //AS(091207) Added that unnamed variables should not be documented
              if ( ((*i)->get_name().getString().compare(0, 19, "SAGE_Doxygen_Dummy_") == 0)
				    || ((*i)->get_name().getString() == "" )  
				 )
                 {
                   return false;
                 }
             }
          return true;
        }
     if (SgClassDeclaration *cd = isSgClassDeclaration(n))
        {
          return cd->get_specialization() == SgDeclarationStatement::e_no_specialization;
        }
     return false;
#if 0
     return n && (n->get_specialization() == SgDeclarationStatement::e_no_specialization)
              && (n->variantT() == V_SgFunctionDeclaration ||
                  n->variantT() == V_SgMemberFunctionDeclaration ||
                     (n->variantT() == V_SgVariableDeclaration &&
                      isSgVariableDeclaration(n)->get_variables().front()->get_name().getString().
                      compare(0, 19, "SAGE_Doxygen_Dummy_") != 0) ||
                     n->variantT() == V_SgClassDeclaration) &&
             !(isSgLocatedNode(n) && isSgLocatedNode(n)->get_file_info()->isCompilerGenerated());
#endif
   }

DoxygenComment::DoxygenComment(SgLocatedNode *n, PreprocessingInfo *comm) : entry(comm->getString()), originalComment(comm), originalNode(n), originalFile(0)
   {
     commentMap[originalComment] = this;

   }

DoxygenComment::DoxygenComment(SgLocatedNode *n) : entry(DoxygenEntry::None),
                                                   originalComment(new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, "/*!*/", n->get_file_info()->get_filenameString(), 0, 0, 0, PreprocessingInfo::before)), originalNode(n), originalFile(0)
   {
     n->addToAttachedPreprocessingInfo(originalComment, PreprocessingInfo::after);
     commentMap[originalComment] = this;

   }

DoxygenComment::~DoxygenComment()
   {
   }

bool
DoxygenComment::isDoxygenComment(PreprocessingInfo *comm)
   {
     return ((comm->getTypeOfDirective() == PreprocessingInfo::CplusplusStyleComment /* a misnomer */
           || comm->getTypeOfDirective() == PreprocessingInfo::C_StyleComment)  &&
                     DoxygenEntry::isDoxygenEntry(comm->getString()));
   }

void
DoxygenComment::attach(SgLocatedNode *newParent)
   {
     if (!newParent->getAttachedPreprocessingInfo())
        {
          newParent->set_attachedPreprocessingInfoPtr(new AttachedPreprocessingInfoType);
        }
     attach(newParent, newParent->getAttachedPreprocessingInfo()->end());
     originalFile = 0;
   }

void
DoxygenComment::attach(SgLocatedNode *newParent, AttachedPreprocessingInfoType::iterator pos)
   {
     if (!newParent->getAttachedPreprocessingInfo())
        {
          newParent->set_attachedPreprocessingInfoPtr(new AttachedPreprocessingInfoType);
        }


     PreprocessingInfo* posInfo = NULL;
     if( pos != newParent->getAttachedPreprocessingInfo()->end()  )
          posInfo = *pos;

  // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
  // originalNode->getAttachedPreprocessingInfo()->remove(originalComment);
     originalNode->getAttachedPreprocessingInfo()->erase( find(originalNode->getAttachedPreprocessingInfo()->begin(),originalNode->getAttachedPreprocessingInfo()->end(),originalComment) );

  // Find the position again
     if ( originalNode != newParent  )
        {
       // The position iterator should be valid as it is
        }
       else
        {
          if( posInfo == NULL )
               pos = newParent->getAttachedPreprocessingInfo()->end();
            else
               pos = find( newParent->getAttachedPreprocessingInfo()->begin(),newParent->getAttachedPreprocessingInfo()->end(), posInfo);
        }

  // DQ (9/27/2007): Added assertion which catches case of iterator invalidation!
  // ROSE_ASSERT(originalNode != newParent);

     newParent->getAttachedPreprocessingInfo()->insert(pos, originalComment);
     originalNode = newParent;
   }

void
DoxygenComment::attach(DoxygenFile *newParent)
   {
     attach(newParent->commentParent);
     originalFile = newParent;
   }

void
DoxygenComment::attach(DoxygenFile *newParent, DoxygenGroup *newGroup)
   {
	 AttachedPreprocessingInfoType* attachedComments = 
	   newGroup->comment->originalNode->getAttachedPreprocessingInfo();

	 AttachedPreprocessingInfoType::iterator groupEnd = 
	   std::find( attachedComments->begin(), attachedComments->end(), newGroup->groupEnd );
	 ROSE_ASSERT( groupEnd != attachedComments->end() );

     attach(newParent->commentParent, groupEnd  );
     originalFile = newParent;
   }

void
DoxygenComment::updateComment()
   {
     string s = entry.unparse();
     originalComment->setString(s);
   }

DoxygenFile::DoxygenFile(SgLocatedNode *_commentParent) : commentParent(_commentParent)
   {
     printf("commentParent = %p\n", commentParent);
   }

DoxygenFile::DoxygenFile(SgProject *prj, string filename)
   {
     Sg_File_Info *info = new Sg_File_Info(filename, 0, 0);

     SgInitializedName *iname = new SgInitializedName;
     stringstream sname;
     sname << "SAGE_Doxygen_Dummy_" << rand();
     iname->set_name(sname.str());
     iname->set_type(new SgTypeInt);
     iname->set_file_info(info);
     iname->get_storageModifier().setExtern();

     SgVariableDeclaration *decl = new SgVariableDeclaration;
     decl->get_variables().push_back(iname);
     decl->set_startOfConstruct(info);
     decl->set_endOfConstruct(info);
     decl->get_declarationModifier().get_storageModifier().setExtern();
     iname->set_parent(decl);
     iname->set_prev_decl_item(iname);

  // SgGlobal *glob = prj->get_file(0).get_globalScope();
     SgSourceFile* sourceFile = isSgSourceFile(prj->get_fileList()[0]);
     ROSE_ASSERT(sourceFile != NULL);
     SgGlobal *glob = sourceFile->get_globalScope();

  // glob->insertStatementInScope(decl, true);
     glob->get_declarations().insert(glob->get_declarations().begin(),decl);
     decl->set_parent(glob);
     SgVariableSymbol* variableSymbol = new SgVariableSymbol(iname);
     glob->insert_symbol(sname.str(),variableSymbol);


     decl->set_parent(glob);
     std::cout << "Before complete string." << std::endl;
     //glob->append_declaration(decl);
     iname->set_scope(glob);

     decl->unparseToCompleteString();
     
     std::cout << "After complete string." << std::endl;

     commentParent = decl;
     printf("commentParent = %p\n", commentParent);
   }

string
removeWhitespace(string s)
   {
     string ret;
     for (unsigned int i = 0; i < s.size(); i++)
        {
          char c = s[i];
          if (c != ' ' && c != '\t' && c != '\n') ret.push_back(c);
        }
     return ret;
   }

#if 0
SgDeclarationStatement *
Doxygen::getDoxygenAttachedDeclaration(SgDeclarationStatement *ds)
   {
     if (ds->get_definingDeclaration()) return ds->get_definingDeclaration();
     ROSE_ASSERT(ds->get_firstNondefiningDeclaration() != NULL);
     return ds->get_firstNondefiningDeclaration();
   }
#else
SgDeclarationStatement *
Doxygen::getDoxygenAttachedDeclaration(SgDeclarationStatement *ds)
   {
     SgDeclarationStatement *retDs;
     if (ds->get_firstNondefiningDeclaration())
        {
          retDs = ds->get_firstNondefiningDeclaration();
        }
     else
        {
          ROSE_ASSERT(ds->get_definingDeclaration() != NULL);
          retDs = ds->get_definingDeclaration();
        }
     if (SgVariableDeclaration *retVarDecl = isSgVariableDeclaration(retDs))
        {
          SgInitializedName *retInitName = retVarDecl->get_variables().front();
          while (SgInitializedName *retInitNamePrev = retInitName->get_prev_decl_item())
             {
               retInitName = retInitNamePrev;
             }
          retDs = retInitName->get_declaration();
        }
     return retDs;
   }
#endif

list<DoxygenComment *> *
Doxygen::getCommentList(SgDeclarationStatement *ds)
   {
     SgDeclarationStatement *dxDs = Doxygen::getDoxygenAttachedDeclaration(ds);
     DoxygenCommentListAttribute *attr = dynamic_cast<DoxygenCommentListAttribute *>
             (dxDs->getAttribute("DoxygenCommentList"));
     ROSE_ASSERT(attr);
     return &(attr->commentList);
   }

bool
Doxygen::isGroupStart(PreprocessingInfo *p)
   {
     return (p->getString() == "//@{\n" || p->getString() == "/*@{*/");
   }

bool
Doxygen::isGroupEnd(PreprocessingInfo *p)
   {
     return (p->getString() == "//@}\n" || p->getString() == "/*@}*/");
   }

class PureDocumentationFileListAttribute : public AstAttribute
   {

     public:
          map<string, DoxygenFile *> docsList;

   };

map<string, DoxygenFile *> *
Doxygen::getFileList(SgProject *prj)
   {
     PureDocumentationFileListAttribute *attr = dynamic_cast<PureDocumentationFileListAttribute *>
             (prj->getAttribute("PureDocumentationFileList"));
     ROSE_ASSERT(attr);
     return &(attr->docsList);
   }

// Levenshtein Distance Algorithm from http://www.merriampark.com/ldcpp.htm
static int
lDistance(const std::string source, const std::string target)
   {

  // Step 1

     const int n = source.length();
     const int m = target.length();
     if (n == 0)
        {
          return m;
        }
     if (m == 0)
        {
          return n;
        }

  // Good form to declare a TYPEDEF

     typedef std::vector< std::vector<int> > Tmatrix; 

     Tmatrix matrix(n+1);

  // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
  // allow for allocation on declaration of 2.nd dimension of vec of vec

     for (int i = 0; i <= n; i++)
        {
          matrix[i].resize(m+1);
        }

  // Step 2

     for (int i = 0; i <= n; i++)
        {
          matrix[i][0]=i;
        }

     for (int j = 0; j <= m; j++)
        {
          matrix[0][j]=j;
        }

  // Step 3

     for (int i = 1; i <= n; i++)
        {

          const char s_i = source[i-1];

       // Step 4

          for (int j = 1; j <= m; j++)
             {

               const char t_j = target[j-1];

            // Step 5

               int cost;
               if (s_i == t_j)
                  {
                    cost = 0;
                  }
               else
                  {
                    cost = 1;
                  }

            // Step 6

               const int above = matrix[i-1][j];
               const int left = matrix[i][j-1];
               const int diag = matrix[i-1][j-1];
               int cell = min( above + 1, min(left + 1, diag + cost));

            // Step 6A: Cover transposition, in addition to deletion,
            // insertion and substitution. This step is taken from:
            // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
            // Enhanced Dynamic Programming ASM Algorithm"
            // (http://www.acm.org/~hlb/publications/asm/asm.html)

               if (i>2 && j>2)
                  {
                    int trans=matrix[i-2][j-2]+1;
                    if (source[i-2]!=t_j) trans++;
                    if (s_i!=target[j-2]) trans++;
                    if (cell>trans) cell=trans;
                  }

               matrix[i][j]=cell;
             }
        }

  // Step 7

     return matrix[n][m];
   }

void
Doxygen::annotate(SgProject *n)
   {
     typedef map<string, map<SgDeclarationStatement *, list<DoxygenComment *> *> > SymTab;
  /* The first pass finds declarations in the AST, adds them to a
   * rudimentary symbol table and attaches any doxygen comments situated next
   * to the declaration.  The advantage of building our own symbol table
   * is that we do not need to know the exact type of overloaded functions.
   * This comes in handy when doing inexact matches (3rd pass).
   */
     SymTab symTab;
     list<DoxygenComment *> commentList;
     PureDocumentationFileListAttribute *attr = new PureDocumentationFileListAttribute();

     //Find declarations in the AST with doxygen comments attached. This will not find the
     //detached comments in a separate .docs file.
     class CollectDeclarations : public AstSimpleProcessing
     {

       public:
            SymTab *symTab;

            virtual void visit(SgNode *n)
               {
                 SgDeclarationStatement *st = isSgDeclarationStatement(n);
                 if (!st)
                    {
                      return;
                    }
                 if (!isRecognizedDeclaration(st))
                    {
                      return;
                    }

                 string proto = getProtoName(st);
                 SgDeclarationStatement *dxSt = getDoxygenAttachedDeclaration(st);

                 if ((*symTab)[proto].count(dxSt) == 0)
                    {
                      DoxygenCommentListAttribute *attr = new DoxygenCommentListAttribute();
					  // King84 (2010.08.03) : This seems to be called with the same node multiple times.
					  // From what I can tell, this is because a single function has been documented multiple times.
					  // At the moment this function is AssemblerX86::AssemblerX86() from src/frontend/Disassemblers/AssemblerX86.h
					  // For now, we will print out a warning and use the new documentation (instead of addNewAttribute we use setAttribute).
					  if (dxSt->attributeExists("DoxygenCommentList"))
					  {
						  std::cerr << "Warning: Multiple Doxygen comments found for function " << dxSt->get_mangled_name().getString() << " at file " << dxSt->get_file_info()->get_filenameString() << ":" << dxSt->get_file_info()->get_line() << "," << dxSt->get_file_info()->get_col() << ".  Picking the last." << std::endl;
					  }
                      dxSt->setAttribute("DoxygenCommentList", attr);
//                      dxSt->addNewAttribute("DoxygenCommentList", attr);
                      (*symTab)[proto][dxSt] = &(attr->commentList);
                    }
                 list<DoxygenComment *> *commentList = (*symTab)[proto][dxSt];

                 AttachedPreprocessingInfoType *info = st->getAttachedPreprocessingInfo();
                 if (info)
                    {
                      for (AttachedPreprocessingInfoType::iterator i = info->begin(); i != info->end(); ++i)
                         {
                           PreprocessingInfo *pi = *i;
                           if (pi->getRelativePosition() == PreprocessingInfo::before && DoxygenComment::isDoxygenComment(pi))
                              {
                                commentList->push_back(new DoxygenComment(st, pi));
                              }
                         }
                    }
               }
     };

     //Find the detached comments in .docs files
     class FindDetachedComments : public AstSimpleProcessing
     {

       public:
            list<DoxygenComment *> *commentList;
            map<string, DoxygenFile *> *docsList;

            virtual void
            visit(SgNode *n)
               {
                 SgVariableDeclaration *vd = isSgVariableDeclaration(n);
                 if (!vd)
                    {
                      return;
                    }
                 if (isRecognizedDeclaration(vd))
                    {
                      return;
                    }

                 AttachedPreprocessingInfoType *info = vd->getAttachedPreprocessingInfo();
                 if (info)
                    {
                      DoxygenFile *f = new DoxygenFile(vd);
                      DoxygenGroup *currentGroup = 0;
                      DoxygenComment *groupComment = 0;
                      (*docsList)[vd->get_file_info()->get_filenameString()] = f;
                      for (AttachedPreprocessingInfoType::iterator i = info->begin(); i != info->end(); ++i)
                         {
                           PreprocessingInfo *pi = *i;
                        //printf("pass %d: processing comment %s\n", inexact ? 3 : 2, pi->getString().c_str());
                        //printf("processing comment %s\n", pi->getString().c_str());
                           if (DoxygenComment::isDoxygenComment(pi))
                              {
                                DoxygenComment *comm = new DoxygenComment(vd, pi);
                                comm->originalFile = f;
                                if (comm->entry.type() == DoxygenEntry::None)
                                   {
                                     if (comm->entry.hasName())
                                        {
                                       // this is a group
                                       //printf("name = '%s'\n", comm->entry.name().c_str());
                                          if (currentGroup)
                                             {
                                               currentGroup->comment = comm;
                                             }
                                          else
                                             {
                                               groupComment = comm;
                                             }
                                        }
                                     else
                                        {
                                          delete comm;
                                          continue;
                                        }
                                   }
                                commentList->push_back(comm);
#if 0
                                pair<SymTab::iterator, SymTab::iterator> bounds = symTab->equal_range(DoxygenClass::getProtoName(comm->entry.prototype));
                                for (SymTab::iterator i = bounds.first; i != bounds.second; )
                                   {
                                     if (inexact || comm->entry.prototype == getQualifiedPrototype((*i).second))
                                        {
                                          DoxygenCommentListAttribute *attr = dynamic_cast<DoxygenCommentListAttribute *>((*i).second->attribute()["DoxygenCommentList"]);
                                          ROSE_ASSERT(attr);

                                          printf("attaching to node %s\n", (*i).second->unparseToString().c_str());
                                          attr->commentList.push_back(comm);
                                          SymTab::iterator ii = i;
                                          ++i;
                                          symTab->erase(ii);
                                        }
                                     else
                                        {
                                          ++i;
                                        }
                                   }
#endif
                              }
                           else if (isGroupStart(pi))
                              {
                                if (currentGroup)
                                   {
                                     puts("Group already open!");
                                   }
                             //puts("opening group");
                                currentGroup = new DoxygenGroup();
                                currentGroup->groupStart = *i;
                                currentGroup->comment = groupComment;
                                groupComment = 0;
                              }
                           else if (isGroupEnd(pi))
                              {
                             //puts("closing group");
                                if (!currentGroup)
                                   {
                                     puts("Group-end encountered without group begin!");
                                   }
                                else
                                   {
                                     currentGroup->groupEnd = *i;
                                     if (currentGroup->comment)
                                        {
                                          f->groups[currentGroup->comment->entry.name()] = currentGroup;
                                        }
                                     else
                                        {
                                          puts("Group wasn't given a name!");
                                        }
                                     currentGroup = 0;
                                   }
                              }
                         }
                    }
               }

     };



     //Attach the PureDocumentationFileListAttribute to the SgProject for later access
     n->addNewAttribute("PureDocumentationFileList", attr);

     //Find doxygen comments attached to the interface documented
     CollectDeclarations cd;
     cd.symTab = &symTab;
     cd.traverse(n, preorder);

     //Find doxygen comments detached from the interface documented. 
     FindDetachedComments fdc;
     fdc.commentList = &commentList;
     fdc.docsList = &(attr->docsList);
     fdc.traverse(n, preorder);

     bool inexact = false;
     bool innerFound = false;




  /* The first pass finds declarations in the AST, adds them to a
   * rudimentary symbol table and attaches any doxygen comments situated next
   * to the declaration.  The advantage of building our own symbol table
   * is that we do not need to know the exact type of overloaded functions.
   * This comes in handy when doing inexact matches (3rd pass).
   */

  /* The following loop executes exactly two times, during which inexact is first false then true. If inexact is true then matches are based on function name only. */
     do
        {
          for (list<DoxygenComment *>::iterator i = commentList.begin(); i != commentList.end(); )
             {
               DoxygenComment *comm = *i;
               if (!comm->entry.hasPrototype())
                  {
                    list<DoxygenComment *>::iterator ii = i;
                    ++i;
                    commentList.erase(ii);
                    continue;
                  }
               //SymTab::mapped_type *protos = &(symTab[DoxygenEntry::getProtoName(comm->entry.prototype())]);
               //AS(09/24/07) Make the lookup in the symbol table explicit
               SymTab::mapped_type *protos;

               SymTab::iterator    symTabLookup = symTab.find(DoxygenEntry::getProtoName(comm->entry.prototype()));

	       if (symTabLookup != symTab.end())
		    protos = &(symTabLookup->second);
	       else{
		    std::cout << "creating new symTab for : " << comm->originalComment->getString()
			    << " \n";
		    protos = new SymTab::mapped_type();
	       }



            //pair<SymTab::iterator, SymTab::iterator> bounds = symTab.equal_range(DoxygenEntry::getProtoName(comm->entry.prototype()));
               int lowestDist = INT_MAX;
               SymTab::mapped_type::iterator lowestCommentList;
               for (SymTab::mapped_type::iterator j = protos->begin(); j != protos->end(); )
                  {
                    string s1 = removeWhitespace(comm->entry.prototype());
                    string s2 = removeWhitespace(getQualifiedPrototype((*j).first));
                 //printf("s1 = \"%s\"\ns2 = \"%s\"\n", s1.c_str(), s2.c_str());
                    if (inexact)
                       {
                         int dist = lDistance(s1, s2);
                         if (dist < lowestDist)
                            {
                              lowestDist = dist;
                              lowestCommentList = j;
                            }
                         ++j;
                       }
                    else if (s1 == s2)
                       {
                         (*j).second->push_back(comm);
                         SymTab::mapped_type::iterator jj = j;
                         ++j;
                         protos->erase(jj);
                         innerFound = true;
                         break;
                       }
                    else
                       {
                         ++j;
                       }
                  }
               if (lowestDist != INT_MAX)
                  {
                    (*lowestCommentList).second->push_back(comm);
                    protos->erase(lowestCommentList);
                    innerFound = true;
                  }
               if (innerFound)
                  {
                    list<DoxygenComment *>::iterator ii = i;
                    ++i;
                    commentList.erase(ii);
                    innerFound = false;
                  }
               else
                  {
                    ++i;
                  }
             }
          inexact = !inexact;
        } while (inexact);

#if 0
     //Print out the current symbol table
     for( SymTab::iterator iItr = symTab.begin(); iItr != symTab.end();
                ++iItr  ){
      std::cout << "first loop" << std::endl;
       SymTab::mapped_type::iterator jItr = iItr->second.begin();
              for(;
                    jItr != iItr->second.end(); ++jItr){
              std::cout << "second loop" << std::endl;

               for(std::list<DoxygenComment*>::iterator kItr = (*jItr).second->begin();
                   kItr != (*jItr).second->end(); ++kItr){
     std::cout << "third loop" << std::endl;

               std::cout << "symTab: " << (*kItr)->originalComment->getString() <<  std::endl;
}
              }
     }
#endif


  /* any remaining comments in the list do not correspond to an existing symbol */
     for (list<DoxygenComment *>::iterator i = commentList.begin(); i != commentList.end(); ++i)
        {
          printf("%s:%d: warning: comment does not match a symbol\n\tprototype is: %s\n", (*i)->originalNode->get_file_info()->get_filenameString().c_str(), (*i)->originalComment->getLineNumber(), (*i)->entry.prototype().c_str());
          delete *i;
        }

   }

void
DoxygenComment::addUndocumentedParams(SgFunctionDeclaration *fn)
   {
     for (SgInitializedNamePtrList::iterator i = fn->get_args().begin(); i != fn->get_args().end(); ++i)
        {
          if (!(*i)->get_name().is_null())
             {
               string s((*i)->get_name().getString());
               if (s.size() > 0 && !entry.hasParameter(s))
                  {
                 // DQ (9/11/2007): This causes too many entries and breaks Doxygen buffers
                 // entry.parameter(s) = "\\todo Document the "+s+" parameter\n";
                  }
             }
        }
   }

void
Doxygen::unparse(SgProject *p, bool (*shouldBeUnparsed)(DoxygenFile*, std::string& fileName))
   {
     map<string, DoxygenFile *> *docsList = getFileList(p);
     for (map<string, DoxygenFile *>::iterator i = docsList->begin(); i != docsList->end(); ++i)
        {
          string outputFile;



          if (options->inplace)
             {
               outputFile = i->first;
             }
          else
             {
               stringstream ss;
               ss << options->outputDirectory << "/" << StringUtility::stripPathFromFileName(i->first);
               outputFile = ss.str();
             }

		  //AS(091207) Do not unparse files which the user does not want to unparse
		  if( (shouldBeUnparsed != NULL) &&
			  ( shouldBeUnparsed(i->second,outputFile) == false ) )
			continue;

		  //AS(091207) Insert warning into the top of the file saying people should not edit it
          i->second->commentParent->getAttachedPreprocessingInfo()->insert(i->second->commentParent->getAttachedPreprocessingInfo()->begin(),new PreprocessingInfo(PreprocessingInfo::C_StyleComment,
				"/* GENERATED FILE - DO NOT MODIFY */",outputFile,1,1,1,PreprocessingInfo::before));

          //std::cout << "Outputfile:" <<  outputFile << std::endl;
          ofstream docs(outputFile.c_str());
          docs << i->second->unparse();
        }
   }

string
DoxygenFile::name()
   {
     return commentParent->get_file_info()->get_filenameString();
   }

string
DoxygenFile::unparse()
   {
     return commentParent->unparseToCompleteString();
   }

bool
DoxygenFile::hasGroup(string name)
   {
     return (groups.count(name) != 0);
   }

DoxygenGroup *
DoxygenFile::group(string name)
   {
     if (!hasGroup(name))
        {
          createGroup(name);
        }
     return groups[name];
   }

void
DoxygenFile::createGroup(string name)
   {
     if (!hasGroup(name))
        {
          PreprocessingInfo *nameComment = new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, "/*! \\name "+name+"*/", commentParent->get_file_info()->get_filenameString(), 0, 0, 0, PreprocessingInfo::before);
          DoxygenGroup *newGroup = new DoxygenGroup();

          PreprocessingInfo *beginComment = new PreprocessingInfo(PreprocessingInfo::C_StyleComment, "//@{\n", commentParent->get_file_info()->get_filenameString(), 0, 0, 0, PreprocessingInfo::before);
          PreprocessingInfo *endComment = new PreprocessingInfo(PreprocessingInfo::C_StyleComment, "//@}\n", commentParent->get_file_info()->get_filenameString(), 0, 0, 0, PreprocessingInfo::before);
          commentParent->addToAttachedPreprocessingInfo(nameComment, PreprocessingInfo::after);
          commentParent->addToAttachedPreprocessingInfo(beginComment, PreprocessingInfo::after);
          commentParent->addToAttachedPreprocessingInfo(endComment, PreprocessingInfo::after);

          DoxygenComment *nameComm = new DoxygenComment(commentParent, nameComment);
          newGroup->comment = nameComm;

          AttachedPreprocessingInfoType::iterator i = commentParent->getAttachedPreprocessingInfo()->end();
          --i;
          newGroup->groupEnd = *i;
          --i;
          newGroup->groupStart = *i;
          groups[name] = newGroup;
        }
   }

DoxygenGroup *
DoxygenFile::findGroup(DoxygenComment *comm)
   {
     AttachedPreprocessingInfoType *info = commentParent->getAttachedPreprocessingInfo();
     AttachedPreprocessingInfoType::iterator groupStart;
     bool groupStartValid = false;
     for (AttachedPreprocessingInfoType::iterator i = info->begin(); i != info->end(); ++i)
        {
          if (Doxygen::isGroupStart(*i))
             {
               groupStart = i;
               groupStartValid = true;
             }
          else if (Doxygen::isGroupEnd(*i))
             {
               groupStartValid = false;
             }
          else if (*i == comm->originalComment)
             {
               if (groupStartValid)
                  {
                    for (map<string, DoxygenGroup *>::iterator j = groups.begin(); j != groups.end(); ++j)
                       {
                         if (j->second->groupStart == *groupStart)
                            {
                              return j->second;
                            }
                       }
                  }
               return 0;
             }
        }
     puts("findGroup called on the wrong file!");
     return 0;
   }


void
Doxygen::correctAllComments(SgProject *prj,  bool (*shouldHaveDocumentation)(SgNode*) )
   {

     class CorrectAllComments : public AstSimpleProcessing
     {

       private:
            SgProject *sageProject;
            bool (*shouldHaveDocs)(SgNode*);

            void
            correctComment(SgDeclarationStatement *ds, DoxygenComment *comm)
               {
                 SgFunctionDeclaration *fn = isSgFunctionDeclaration(ds);
                 comm->entry.type() = fn ? DoxygenEntry::Function : dynamic_cast<SgVariableDeclaration *>(ds) ? DoxygenEntry::Variable : DoxygenEntry::Class;
                 comm->entry.prototype() = Doxygen::getQualifiedPrototype(ds)+"\n";
                 if (!comm->entry.hasDescription())
                    {
                   // DQ (9/11/2007): This causes too many entries and breaks Doxygen buffers
                   // comm->entry.description() = "\\todo Document this\n";
                    }
                 if (fn)
                    {
                      comm->addUndocumentedParams(fn);
                    }
                 comm->updateComment();
               }

       public:

            CorrectAllComments(SgProject *prj, bool (*fptr)(SgNode*) ) : sageProject(prj), shouldHaveDocs(fptr) {}

            virtual void
            visit(SgNode *n )
               {
                 SgDeclarationStatement *ds = isSgDeclarationStatement(n);
                 //SgMemberFunctionDeclaration *fn = isSgMemberFunctionDeclaration(n);
                 if (!Doxygen::isRecognizedDeclaration(ds)) return;

				 //Do not generate or update documentation for nodes that we are
				 //not interested in
				 if ( ( shouldHaveDocs != NULL ) &&
					  ( shouldHaveDocs(n) == false )
                    )
				 {
				   std::cout << "Should not generate documentation for " 
					         << isSgLocatedNode(n)->unparseToString() << std::endl;
				   return;
				 }
				 
                 list<DoxygenComment *> *commentList = Doxygen::getCommentList(ds);
                 if ( commentList->size() == 0 )
                    {
                      DoxygenComment *comm = new DoxygenComment(ds);
                      commentList->push_back(comm);
                      DoxygenFile *f = Doxygen::destinationFile(sageProject, ds);
                      if (f)
                         {
                           comm->attach(f);
                         }
                 }

                 for (list<DoxygenComment *>::iterator i = commentList->begin(); i != commentList->end(); ++i)
                    {
                      correctComment(ds, *i);
                    }
               }
     };

     CorrectAllComments cac(prj,shouldHaveDocumentation);
     cac.traverse(prj, preorder);
   }

/*! Flag a number of problems which may occur with documentation.
    Currently only flags multiple documentation entries.
 */
void
Doxygen::lint(SgNode *n)
   {

     class Lint : public AstSimpleProcessing
     {

       public:
            virtual void
            visit(SgNode *n)
               {
                 SgDeclarationStatement *ds = isSgDeclarationStatement(n);
                 if (!Doxygen::isRecognizedDeclaration(ds)) return;
                 list<DoxygenComment *> *commentList = Doxygen::getCommentList(ds);
                 if (commentList->size() > 1)
                    {
                      for (list<DoxygenComment *>::iterator i = commentList->begin(); i != commentList->end(); ++i)
                         {
                           PreprocessingInfo *pi = (*i)->originalComment;
                           cout << pi->get_file_info()->get_filenameString() << ":" << pi->get_file_info()->get_line() <<
                                   ": warning: multiple documentation entries for " << getQualifiedPrototype(ds) << endl;
                         }
                    }
               }
     };

     Lint l;
     l.traverse(n, preorder);
   }

                 
string
DoxygenGroup::getName()
   {
     return comment->entry.name();
   }

void
Doxygen::parseCommandLine(vector<string>& argvList)
   {
     srand(time(0)); // shouldn't be here
     options = new DoxygenOptions;
     options->createNewFiles = CommandlineProcessing::isOption(argvList, "-doxygen:", "createNewFiles", true);
     options->inplace = CommandlineProcessing::isOption(argvList, "-doxygen:", "inplace", true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-doxygen:", "classNameTemplate", options->classNameTemplate, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-doxygen:", "functionNameTemplate", options->functionNameTemplate, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-doxygen:", "variableNameTemplate", options->variableNameTemplate, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-doxygen:", "updateScript", options->updateScript, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-doxygen:", "outputDirectory", options->outputDirectory, true);
   }

Doxygen::DoxygenOptions *Doxygen::options;

/*! Retrieves, creating if necessary, the destination file for a given 
declaration.  Returns null if the comment should be placed next to the
declaration. */
DoxygenFile *
Doxygen::destinationFile(SgProject *prj, SgDeclarationStatement *decl)
   {
     if (!options->createNewFiles)
        {
          return 0;
        }

     string name = destinationFilename(decl);
     ROSE_ASSERT(name.size() > 0);
     map<string, DoxygenFile *> *fileList = getFileList(prj);
     if (fileList->count(name) == 0)
        {
          cout << "'Created' the file " << name << endl;
          (*fileList)[name] = new DoxygenFile(prj, name);
          if (options->updateScript.size() > 0)
             {
               char updateScriptBuf[1024];
               snprintf(updateScriptBuf, 1024, options->updateScript.c_str(), name.c_str());
               system(updateScriptBuf);
             }
        }
     return (*fileList)[name];
   }

string
Doxygen::destinationFilename(SgDeclarationStatement *decl)
   {
     if (SgClassDefinition *cdef = isSgClassDefinition(decl))
        {
          return destinationFilename(cdef->get_declaration());
        }
     switch (decl->variantT())
        {
          case V_SgClassDeclaration:
          //case V_SgTemplateInstantiationDecl: // temporary until proper support for templates
                {
                  char classNameBuf[1024];
                  snprintf(classNameBuf, 1024, options->classNameTemplate.c_str(), isSgClassDeclaration(decl)->get_name().str());
                  return classNameBuf;
                }
          case V_SgFunctionDeclaration:
          //case V_SgTemplateInstantiationFunctionDecl: // temporary until proper support for templates
                {
                  char functionNameBuf[1024];
                  snprintf(functionNameBuf, 1024, options->functionNameTemplate.c_str(), isSgFunctionDeclaration(decl)->get_name().str());
                  return functionNameBuf;
                }
          case V_SgVariableDeclaration:
                {
                  if (SgClassDefinition *classDef = isSgClassDefinition(isSgVariableDeclaration(decl)->get_variables().front()->get_scope()))
                     {
                       return destinationFilename(classDef->get_declaration());
                     }
                  char variableNameBuf[1024];
                  snprintf(variableNameBuf, 1024, options->variableNameTemplate.c_str(), isSgVariableDeclaration(decl)->get_variables().front()->get_name().str());
                  return variableNameBuf;
                }
          case V_SgMemberFunctionDeclaration:
          //case V_SgTemplateInstantiationMemberFunctionDecl: // temporary until proper support for templates
             return destinationFilename(isSgMemberFunctionDeclaration(decl)->get_class_scope()->get_declaration());
             // above line now redundant
          default:
             cout << "Doxygen::destinationFilename: default case, class =  " << decl->class_name() << endl;
             return "";
        }
   }
