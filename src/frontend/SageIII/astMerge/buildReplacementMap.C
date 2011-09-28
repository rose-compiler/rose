// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "buildMangledNameMap.h"
#include "buildReplacementMap.h"

#include "fixupTraversal.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
using namespace std;
using namespace SageInterface; // Liao, 2/8/2009, for  generateUniqueName()

ReplacementMapTraversal::ReplacementMapTraversal( MangledNameMapTraversal::MangledNameMapType & inputMangledNameMap, 
                                                  ReplacementMapTraversal::ReplacementMapType & inputReplacementMap,
                                                  ReplacementMapTraversal::ListToDeleteType   & inputDeleteList )
   : mangledNameMap(inputMangledNameMap),replacementMap(inputReplacementMap),deleteList(inputDeleteList)
   {
     numberOfNodes         = 0;
     numberOfNodesTested   = 0;
     numberOfNodesMatching = 0;
   }

set<SgNode*>
ReplacementMapTraversal::buildListOfODRviolations ( ReplacementMapTraversal::ODR_ViolationType & ODR_Violations )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Build the STL list of ODR violations in the unmerged AST:");

  // Convert the list< pair<SgNode*,SgNode*> > into a list<SgNode*> (note that these specify subtrees)
     set<SgNode*> returnList;
     for (ODR_ViolationType::iterator i = ODR_Violations.begin(); i != ODR_Violations.end(); i++)
        {
          returnList.insert(i->first);
          returnList.insert(i->second);
        }

     if (SgProject::get_verbose() > 0)
          printf ("ODR_Violations.size() = %zu  returnList.size() = %zu \n",ODR_Violations.size(),returnList.size());

     return returnList;
   }

bool
ReplacementMapTraversal::verifyODR( SgNode* node, SgNode* duplicateNodeFromOriginalAST )
   {
     bool passesODR = false;
  // printf ("Verify that node = %p is equivalent to duplicateNodeFromOriginalAST = %p = %s \n",node,duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str());

  // Verify that these strings match
     ROSE_ASSERT (duplicateNodeFromOriginalAST->variantT() == node->variantT());
     ROSE_ASSERT (duplicateNodeFromOriginalAST->class_name() == node->class_name());
  // ROSE_ASSERT (generateUniqueName(duplicateNodeFromOriginalAST) == generateUniqueName(node));

     string nodeString;
     string duplicateNodeFromOriginalASTstring;

#if 0
  // DQ (2/3/2007): This is a test to debug the ODR checking.
  // I think that the unparser has some state specific to the output of access protections.
  // if the unparsing changes the state then the access permission (public, protected, private) 
  // is output and this cause a falue trigger to the ODR string match.  This is a temp fix to
  // absorbe any change of state, but we need a mechanism to clear the state in the unparser.
     string absorbeUnparserStateChange_A = node->unparseToString();
     string absorbeUnparserStateChange_B = duplicateNodeFromOriginalAST->unparseToString();
#endif
#if 0
  // DQ (2/3/2007): Make sure that there are close to being related. It appears that if these are
     if (node->get_parent()->variantT() == duplicateNodeFromOriginalAST->get_parent()->variantT())
        {
          nodeString                         = node->unparseToString();
          duplicateNodeFromOriginalASTstring = duplicateNodeFromOriginalAST->unparseToString();
        }
#endif

     bool skip_ODR_test = false;

     bool nodeIsCompilerGenerated = 
          (node->get_file_info() != NULL) ? node->get_file_info()->isCompilerGenerated() : false;
     bool duplicateNodeFromOriginalASTIsCompilerGenerated = 
          (duplicateNodeFromOriginalAST->get_file_info() != NULL) ? duplicateNodeFromOriginalAST->get_file_info()->isCompilerGenerated() : false;

     bool nodeIsFrontendSpecific = 
          (node->get_file_info() != NULL) ? node->get_file_info()->isFrontendSpecific() : false;
     bool duplicateNodeFromOriginalASTIsFrontendSpecific = 
          (duplicateNodeFromOriginalAST->get_file_info() != NULL) ? duplicateNodeFromOriginalAST->get_file_info()->isFrontendSpecific() : false;

  // If this is a template declaration for a function then it might have been a part of another template declaration 
  // for the template class and thus might not exist explicitly in the AST (and thus not have enough information from 
  // which to generate a meaningful mangled name).  Skip ODR testing of these cases.
     bool isTemplateMemberFunctionInTemplatedClass = false;
     SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
     if (templateDeclaration != NULL)
        {
          SgTemplateDeclaration* dup_templateDeclaration = isSgTemplateDeclaration(duplicateNodeFromOriginalAST);
          ROSE_ASSERT(dup_templateDeclaration != NULL);
          if ( templateDeclaration->get_string().is_null() && dup_templateDeclaration->get_string().is_null() )
             {
               isTemplateMemberFunctionInTemplatedClass = true;
             }
        }

     if (isTemplateMemberFunctionInTemplatedClass == true)
        {
          printf ("ODR not tested isTemplateMemberFunctionInTemplatedClass == true. \n");
          skip_ODR_test = true;
        }

     if (nodeIsFrontendSpecific == true || duplicateNodeFromOriginalASTIsFrontendSpecific == true || nodeIsCompilerGenerated == true || duplicateNodeFromOriginalASTIsCompilerGenerated == true)
        {
       // printf ("ODR not tested for frontend specific compiler generated code. \n");
          skip_ODR_test = true;
        }

  // DQ (1/20/2007): The unparse will not generate a string if the code is frontend specific or compiler generated (I forget which).
  // if (nodeIsFrontendSpecific == true || duplicateNodeFromOriginalASTIsFrontendSpecific == true)
     if (skip_ODR_test == true)
        {
       // printf ("ODR not tested for frontend specific compiler generated code. \n");
          passesODR = true;
        }
       else
        {
          SgUnparse_Info info_a;
          SgUnparse_Info info_b;

       // DQ (2/6/2007): Force qualified names to be used uniformally (note that info.set_requiresGlobalNameQualification() 
       // causes an error) info.set_requiresGlobalNameQualification();
          info_a.set_forceQualifiedNames();
          info_b.set_forceQualifiedNames();

          nodeString                         = node->unparseToString(&info_a);

       // DQ (2/6/2007): The SgUnparse_Info object carries state which controls the use of access qualification and the 
       // first call to unparseToString might have set the access (e.g. to "public") and the second call would drop the 
       // access qualification.  We unset the access qualification state in the SgUnparse_Info object so that both will 
       // be unparsed the same (we could have alternatively used two separate SgUnparse_Info objects.
       // info.set_isUnsetAccess();

          duplicateNodeFromOriginalASTstring = duplicateNodeFromOriginalAST->unparseToString(&info_b);

          passesODR = (nodeString == duplicateNodeFromOriginalASTstring);
        }

  // Don't count the cases where the unparse fails to to invalid parent in redundant SgClassDeclaration (fix these later)
     if (passesODR == false && nodeString.empty() == false && duplicateNodeFromOriginalASTstring.empty() == false)
        {
#if 1
          if (SgProject::get_verbose() > 0)
               printf ("##### In ReplacementMapTraversal::verifyODR() is false: node = %p = %s duplicateNodeFromOriginalAST = %p = %s \n",
                    node,node->class_name().c_str(),duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str());

       // printf ("##### passesODR = %s \n",passesODR ? "true" : "false");
       // printf ("duplicateNodeFromOriginalASTstring = \n---> %s\n",duplicateNodeFromOriginalASTstring.c_str());
       // printf ("nodeString                         = \n---> %s\n",nodeString.c_str());
          if (node->get_file_info() != NULL && duplicateNodeFromOriginalAST->get_file_info() != NULL)
             {
               if (SgProject::get_verbose() > 0)
                  {
                    SgNode* parent_node = node->get_parent();

                 // DQ (9/13/2011): Reported as possible NULL value in static analysis of ROSE code.
                    ROSE_ASSERT(parent_node != NULL);

                    printf ("parent_node = %p = %s = %s \n",parent_node,parent_node->class_name().c_str(),SageInterface::get_name(parent_node).c_str());
                    SgNode* parent_dup = duplicateNodeFromOriginalAST->get_parent();

                 // DQ (9/13/2011): Reported as possible NULL value in static analysis of ROSE code.
                    ROSE_ASSERT(parent_dup != NULL);

                    printf ("parent_dup = %p = %s = %s \n",parent_dup,parent_dup->class_name().c_str(),SageInterface::get_name(parent_dup).c_str());

                    printf ("\nPosition of error: \n");
                    node->get_file_info()->display("In ReplacementMapTraversal::verifyODR(node) is false: debug");
                    duplicateNodeFromOriginalAST->get_file_info()->display("In ReplacementMapTraversal::verifyODR(duplicateNodeFromOriginalAST) is false: debug");
                    printf ("\nPosition of error: \n");

                    printf ("\nPosition of error (parent IR node): \n");
                    parent_node->get_file_info()->display("In ReplacementMapTraversal::verifyODR(parent_node) is false: debug");
                    parent_dup->get_file_info()->display("In ReplacementMapTraversal::verifyODR(parent_dup) is false: debug");
                    printf ("\nPosition of error (parent IR node): \n");
                  }
             }
            else
             {
               SgClassType* classType = isSgClassType(node);
               SgClassType* duplicateNodeFromOriginalAST_classType = isSgClassType(duplicateNodeFromOriginalAST);
               if (classType != NULL)
                  {
                    if (SgProject::get_verbose() > 0)
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                         ROSE_ASSERT(classDeclaration != NULL);
                         classDeclaration->get_file_info()->display("In ReplacementMapTraversal::verifyODR(node) is false (classType)");
                         printf ("classDeclaration = %p definingDeclaration = %p nondefiningDeclaration = %p \n",
                              classDeclaration,
                              classDeclaration->get_definingDeclaration(),
                              classDeclaration->get_firstNondefiningDeclaration());

                         ROSE_ASSERT(duplicateNodeFromOriginalAST_classType != NULL);
                         SgClassDeclaration* duplicateNodeFromOriginalAST_classDeclaration = isSgClassDeclaration(duplicateNodeFromOriginalAST_classType->get_declaration());
                         ROSE_ASSERT(duplicateNodeFromOriginalAST_classDeclaration != NULL);
                         duplicateNodeFromOriginalAST_classDeclaration->get_file_info()->display("In ReplacementMapTraversal::verifyODR(node) is false (duplicateNodeFromOriginalAST_classType)");
                         printf ("duplicateNodeFromOriginalAST_classDeclaration = %p definingDeclaration = %p nondefiningDeclaration = %p \n",
                              duplicateNodeFromOriginalAST_classDeclaration,
                              duplicateNodeFromOriginalAST_classDeclaration->get_definingDeclaration(),
                              duplicateNodeFromOriginalAST_classDeclaration->get_firstNondefiningDeclaration());
                       }
                  }
             }
#endif
          odrViolations.push_back(pair<SgNode*,SgNode*>(node,duplicateNodeFromOriginalAST));
        }
#if 0
     printf ("duplicateNodeFromOriginalASTstring = %p = %s \n---> %s\n",
          duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str(),duplicateNodeFromOriginalASTstring.c_str());
     printf ("nodeString                         = %p = %s \n---> %s\n",
          node,node->class_name().c_str(),nodeString.c_str());
#endif
#if 0
     SgClassType* original = isSgClassType(duplicateNodeFromOriginalAST);
     SgClassType* target   = isSgClassType(node);
     if (original != NULL && target != NULL)
        {
          printf ("original declaration = %p \n",original->get_declaration());
          printf ("target declaration   = %p \n",target->get_declaration());
        }
#endif
#if 1
     if (passesODR == false)
        {
          if (SgProject::get_verbose() > 0)
             {
               string node_generatedName         = SageInterface::generateUniqueName(node,false);
               string originalNode_generatedName = SageInterface::generateUniqueName(duplicateNodeFromOriginalAST,false);

               printf ("ODR Violation Source code: nodeString                         = \n%s\n \n",nodeString.c_str());
               printf ("ODR Violation Source code: duplicateNodeFromOriginalASTstring = \n%s\n \n",duplicateNodeFromOriginalASTstring.c_str());
               printf ("nodeString = %s \n",nodeString.c_str());
               printf ("node_generatedName         = %s \n",node_generatedName.c_str());
               printf ("originalNode_generatedName = %s \n",originalNode_generatedName.c_str());

               printf ("node                         = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               printf ("duplicateNodeFromOriginalAST = %p = %s = %s \n",duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str(),SageInterface::get_name(duplicateNodeFromOriginalAST).c_str());

               printf ("node (unique string)                         = %s \n",generateUniqueName(node,true).c_str());
               printf ("duplicateNodeFromOriginalAST (unique string) = %s \n",generateUniqueName(duplicateNodeFromOriginalAST,true).c_str());

               SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
               if (declarationStatement != NULL)
                  {
                    printf ("declarationStatement->get_definingDeclaration()         = %p \n",declarationStatement->get_definingDeclaration());
                    printf ("declarationStatement->get_firstNondefiningDeclaration() = %p \n",declarationStatement->get_firstNondefiningDeclaration());
                  }

               SgDeclarationStatement* declarationStatement2 = isSgDeclarationStatement(duplicateNodeFromOriginalAST);
               if (declarationStatement2 != NULL)
                  {
                    printf ("declarationStatement2->get_definingDeclaration()         = %p \n",declarationStatement2->get_definingDeclaration());
                    printf ("declarationStatement2->get_firstNondefiningDeclaration() = %p \n",declarationStatement2->get_firstNondefiningDeclaration());
                  }

               printf ("Source code positions of ORD violation: \n");
               node->get_file_info()->display("In ReplacementMapTraversal::verifyODR(node) is false: debug");
               duplicateNodeFromOriginalAST->get_file_info()->display("In ReplacementMapTraversal::verifyODR(duplicateNodeFromOriginalAST) is false: debug");
             }
        }
#endif
  // ROSE_ASSERT(nodeString == duplicateNodeFromOriginalASTstring);
     ROSE_ASSERT(passesODR == true);

     return passesODR;
   }

#if 0
SgNode* ReplacementMapTraversal::getOriginalNode (const string & key) const
   {
     SgNode* duplicateNode = NULL;
#if 0
     printf ("ReplacementMapTraversal::getOriginalNode: key = %s \n",key.c_str());
     printf ("mangledNameMap.size() = %ld \n",mangledNameMap.size());
#endif
#if 0
     for (MangledNameMapTraversal::MangledNameMapType::iterator i = mangledNameMap.begin(); i != mangledNameMap.end(); i++)
        {
          printf ("i->first  = %s \n",i->first.c_str());
          printf ("i->second = %p \n",i->second);
        }     
#endif
     if (mangledNameMap.find(key) != mangledNameMap.end())
        {
       // This key already exists in the mangledNameMap
       // duplicateNode = mangledNameMap[key];
       // return mangledNameMap[key];
          duplicateNode = mangledNameMap[key];
        }
#if 0
       else
        {
       // This key does not exist in the mangledNameMap
          printf ("Warning: This key does not exist in the mangledNameMap, ignoring this case! key = %s \n",key.c_str());
        }
#endif
#if 0
     printf ("ReplacementMapTraversal::getOriginalNode() returning duplicateNode = %p = %s \n",
          duplicateNode,(duplicateNode != NULL) ? duplicateNode->class_name().c_str() : "NULL");
#endif
     return duplicateNode;
   }
#endif


void ReplacementMapTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
  // printf ("ReplacementMapTraversal::visit: node = %p = %s \n",node,node->class_name().c_str());

  // Keep a count of the number of IR nodes visited
     numberOfNodes++;

  // By computing what is sharable we can eliminate many tests (testing mostly just declarations in global or namespace scope)
     bool sharable = MangledNameMapTraversal::shareableIRnode(node);
     if (sharable == true)
        {
       // printf ("ReplacementMapTraversal::visit() sharable node = %p = %s \n",node,node->class_name().c_str());

       // Keep a count of the number of IR nodes tests (shared)
          numberOfNodesTested++;

       // This is a relatively expensive operation, but required to do the reverse lookup 
       // into the mangled name map to build entries for the replacement map.
       // This could be made much faster by separating out the different kinds of IR nodes
       // and building many different maps instead of just one using a SgNode pointer.
          const string & key = SageInterface::generateUniqueName(node,false);
       // printf ("ReplacementMapTraversal::visit(): node = %p = %s generated name (key) = %s \n",node,node->class_name().c_str(),key.c_str());

       // All cases (above) should generate a valid name, however SgSymbolTable, SgCtorInitializerList, 
       // SgReturnStmt, and SgBasicBlock don't generate names (should this be fixed?).
          if (key.empty() == true)
             {
            // printf ("Warning: empty key generated for node = %p = %s \n",node,node->class_name().c_str());
             }
       // ROSE_ASSERT(key.empty() == false);

          SgNode* duplicateNodeFromOriginalAST = NULL;

       // Skip declarations where we would generate empty keys (mangled names are empty)
          if (key.empty() == false)
             {
            // We need to protect the mangledNameMap from having a new key added!
            // Is there a better way to do this?
            // duplicateNodeFromOriginalAST = getOriginalNode(key);

            // DQ (2/19/2007): This is more efficient since it looks up the element from the map only once.
               MangledNameMapTraversal::MangledNameMapType::iterator mangledMap_it = mangledNameMap.find(key);
               if (mangledMap_it != mangledNameMap.end())
                  {
                 // duplicateNodeFromOriginalAST = mangledNameMap[key];
                    duplicateNodeFromOriginalAST = mangledMap_it->second;
                  }
             }

#if 0
          printf ("(after being set: duplicateNodeFromOriginalAST = %p \n",duplicateNodeFromOriginalAST);
          if (duplicateNodeFromOriginalAST != NULL)
             {
               printf ("duplicateNodeFromOriginalAST = %p = %s \n",duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str());
             }
#endif

       // printf ("     duplicateNodeFromOriginalAST = %p \n",duplicateNodeFromOriginalAST);
          if (duplicateNodeFromOriginalAST != NULL && node != duplicateNodeFromOriginalAST)
             {
            // These should be at least the same type of IR node (I think, else what kind of uniqueness do we have in mangled names!)
               if (node->variantT() != duplicateNodeFromOriginalAST->variantT())
                  {
                    printf ("Error (IR nodes should not have been matched): node = %p = %s = %s while duplicateNodeFromOriginalAST = %p = %s = %s \n",
                         node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),
                         duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str(),SageInterface::get_name(duplicateNodeFromOriginalAST).c_str());
                    printf ("generateUniqueName(node,false)                         = %s \n",generateUniqueName(node,false).c_str());
                    printf ("generateUniqueName(duplicateNodeFromOriginalAST,false) = %s \n",generateUniqueName(duplicateNodeFromOriginalAST,false).c_str());
                    node->get_file_info()->display("node: IR nodes should not have been matched: debug");
                    duplicateNodeFromOriginalAST->get_file_info()->display("duplicateNodeFromOriginalAST: IR nodes should not have been matched: debug");
                  }
               ROSE_ASSERT(node->variantT() == duplicateNodeFromOriginalAST->variantT());

            // Keep a count of the number of IR nodes added to the replacement map (requiring fixup later)
               numberOfNodesMatching++;

#if 0
               printf ("Before calling verifyODR() node = %p = %s duplicateNodeFromOriginalAST = %p = %s \n",
                    node,node->class_name().c_str(),duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str());
#endif

            // Check to make sure that they are the same under ODR
               bool passesODR = true;
#if 0
            // Skip ODR test to see if this make a significant performance improvement. Since we alsways are 
            // required to pass this test, it is just a test and not required except for security analysis.
               passesODR = verifyODR(node,duplicateNodeFromOriginalAST);
#endif

            // Insert a map into the multimap using a pair built from the current node and the original AST node that will replace it (later)
               if (passesODR == true)
                  {
#if 0
                    printf ("At node = %p = %s duplicateNodeFromOriginalAST = %p = %s \n",
                         node,node->class_name().c_str(),
                         duplicateNodeFromOriginalAST,duplicateNodeFromOriginalAST->class_name().c_str());
#endif
                 // Build up the replacementMap
                    replacementMap.insert(pair<SgNode*,SgNode*>(node,duplicateNodeFromOriginalAST));

                 // Save the key (subtree of the redundant tree now not referenced) so that we can delete it later.
                    deleteList.insert(node);

                 // DQ (5/21/2006): Note that get_file_info() is a virtual function that
                 // works where there is not startOfConstruct (e.g. SgInitializedName).
                 // ROSE_ASSERT(duplicateNodeFromOriginalAST->get_file_info() != NULL);
                    if (duplicateNodeFromOriginalAST->get_file_info() != NULL)
                       {
                      // duplicateNodeFromOriginalAST->get_file_info()->setShared();
                         duplicateNodeFromOriginalAST->get_startOfConstruct()->setShared();
                         if (duplicateNodeFromOriginalAST->get_endOfConstruct() != NULL)
                              duplicateNodeFromOriginalAST->get_endOfConstruct()->setShared();
                       }
                  }
             }
        }
#if 0
       else
        {
          printf ("Not a sharable IR node, but might contain some IR node pointers to be reset! node = %p = %s \n",node,node->class_name().c_str());

       // We could just wait until we see a SgSymbol that needs to be replaced and extract it from the symbol table directly.
          SgSymbolTable* symbolTable = isSgSymbolTable(node);
          if (symbolTable != NULL)
             {
            // There are pointers to SgSymbols that share the name of SgSymbols that we want to use alternatively.
            // We can just reset their pointers if the name would not change.
             }
        }
#endif
   }


// ReplacementMapTraversal::ReplacementMapType
void
replacementMapTraversal ( 
   MangledNameMapTraversal::MangledNameMapType & mangledNameMap,
   ReplacementMapTraversal::ReplacementMapType & replacementMap,
   ReplacementMapTraversal::ODR_ViolationType  & violations,
   ReplacementMapTraversal::ListToDeleteType   & deleteList )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Build the STL map of shared IR nodes and replacement sites in the AST:");

     if (SgProject::get_verbose() > 0)
          printf ("In replacementMapTraversal(): mangledNameMap.size() = %zu \n",mangledNameMap.size());

     ReplacementMapTraversal traversal(mangledNameMap,replacementMap,deleteList);
     traversal.traverseMemoryPool();

     violations = traversal.odrViolations;

     if (SgProject::get_verbose() > 0)
        {
          printf ("ReplacementMapTraversal statistics: \n");
          printf ("     violations.size() = %zu \n",violations.size());
          printf ("     numberOfNodes         = %d \n",traversal.numberOfNodes);
          printf ("     numberOfNodesTested   = %d \n",traversal.numberOfNodesTested);
          printf ("     numberOfNodesMatching = %d \n",traversal.numberOfNodesMatching);
        }

  // return traversal.replacementMap;
   }

void
ReplacementMapTraversal::displayReplacementMap ( const ReplacementMapTraversal::ReplacementMapType & m )
   {
  // typedef multimap<SgNode*,SgNode*> ReplacementMapType;

     printf ("ReplacementMapTraversal::displayReplacementMap(): size() = %zu \n",m.size());

     ReplacementMapTraversal::ReplacementMapType::const_iterator i = m.begin();
     while (i != m.end())
        {
          ROSE_ASSERT(i->first != NULL);
          ROSE_ASSERT(i->second != NULL);

          Sg_File_Info* first_fileInfo  = i->first->get_file_info();
          Sg_File_Info* second_fileInfo = i->second->get_file_info();
          printf ("ReplacementMapTraversal::displayReplacementMap(): i->first  = %p = %s from: %s \n",
               i->first,i->first->class_name().c_str()  ,
               (first_fileInfo != NULL)  ? first_fileInfo->get_raw_filename().c_str()  : "NO SOURCE POSITION" );
          printf ("ReplacementMapTraversal::displayReplacementMap(): i->second = %p = %s from: %s \n",
               i->second,i->second->class_name().c_str(),
               (second_fileInfo != NULL) ? second_fileInfo->get_raw_filename().c_str() : "NO SOURCE POSITION");

          i++;
        }
   }

