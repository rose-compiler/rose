#include "rose.h"

// #include "general_token_defs.h"

#include "frontierDetection.h"

#include "previousAndNextNode.h"

using namespace std;


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute()
   {
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed)
   {
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
   {
     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;
   }




FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     node       = NULL;
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n)
   {
     node       = isSgStatement(n);
     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
     node       = X.node;
     isFrontier = X.isFrontier;

     frontierNodes    = X.frontierNodes;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;
   }


FrontierDetectionForTokenStreamMapping::
FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile)
   {
   }


#if 0
std::vector<AstAttribute::AttributeNodeInfo>
FrontierDetectionForTokenStreamMapping::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     string vulnerabilityName    = " " + vulnerabilityPointer->get_name();
     string vulnerabilityColor   = vulnerabilityPointer->get_color();
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

  // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, vulnerabilityName, vulnerabilityOptions);
     v.push_back(vulnerabilityNode);

     return v;
   }
#endif

FrontierDetectionForTokenStreamMapping_InheritedAttribute
FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute)
   {
     static int random_counter = 0;

#if 0
     printf ("*** In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
#endif

#if 1
  // Ignore IR nodes that are front-end specific (declarations of builtin functions, etc.).
     if (n->get_file_info()->isFrontendSpecific() == false)
        {
       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }
#endif

     FrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute;

     SgStatement* statement = isSgStatement(n);
  // if (statement != NULL && random_counter > 30 && random_counter < 40)
     if (statement != NULL)
        {
#if 0
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
          string name    = "token_frontier";
          string options = "color=\"blue\"";

       // This will later just check if this is a statement marked as a transformation or that it has been modified (checking the SgNode->isModified member flag).
          if (random_counter > 30 && random_counter < 40)
             {
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",random_counter,statement,statement->class_name().c_str());

               options = "color=\"red\"";

               if (returnAttribute.unparseFromTheAST == false)
                  {
                 // Mark this as the frontier only if this is the first time (while decending into the AST) 
                 // that we will unparse this subtree from the AST.
                    returnAttribute.isFrontier = true;
                  }

               returnAttribute.unparseFromTheAST       = true;
               returnAttribute.unparseUsingTokenStream = false;
             }
            else
             {
               returnAttribute.unparseUsingTokenStream = true;
             }

       // Mark the whole subtree as being unparsed from the AST so that synthizized attributes can be more esily computed.
          if (inheritedAttribute.unparseFromTheAST == true)
             {
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
               returnAttribute.unparseFromTheAST = true;
               returnAttribute.unparseUsingTokenStream = false;
             }

       // AstAttribute::AttributeNodeInfo* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
       // AstAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
          FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
          statement->setAttribute(name,attribute);

#if 0
          if (returnAttribute.isFrontier == true)
             {
            // attribute->set_unparseUsingTokenStream(true);
               returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
             }
            else
             {
            // attribute->set_containsNodesToBeUnparsedFromTheAST(true);
             }
#endif
#if 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
#endif
        }
       else
        {
       // Default setting for all non-SgStatements.
       // returnAttribute.unparseUsingTokenStream = true;

          if (inheritedAttribute.unparseFromTheAST == true)
             {
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
               returnAttribute.unparseFromTheAST = true;
               returnAttribute.unparseUsingTokenStream = false;
             }
            else
             {
            // Default setting for all non-SgStatements.
               returnAttribute.unparseUsingTokenStream = true;
             }

#if 0
          printf ("Non-SgStatement node: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
#endif
        }

#if 0
     printf ("*** Leaving FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );

     if (returnAttribute.isFrontier == true)
        {
          ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
        }

     return returnAttribute;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
FrontierDetectionForTokenStreamMapping::FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // The goal of this function is to identify the node ranges in the frontier that are associated with 
  // tokens stream unparsing, and AST node unparsing.  There ranges are saved and concatinated as we
  // proceed in the evaluation of the synthesized attributes up the AST.

  // We want to generate a IR node range in each node which contains children so that we can concatinate the lists
  // across the whole AST and define the frontier in terms of IR nodes which will then be converted into 
  // token ranges to be unparsed and specific IR nodes to be unparsed from the AST directly.

     ROSE_ASSERT(n != NULL);

#if 0
     printf ("\n\n### In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
#endif

     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n);

  // We need to handle SgStatement, plus the SgSourceFile because we need to copy synthesized results from the SgGlobal to the SgSourceFile.
     SgStatement* statement = isSgStatement(n);
     SgSourceFile* sourceFile = isSgSourceFile(n);
     if (statement != NULL || sourceFile != NULL)
        {
#if 0
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): SgStatement or SgSourceFile = %p = %s \n",n,n->class_name().c_str());
#endif

#if 0
          if (inheritedAttribute.isFrontier == true)
             {
#if 0
               printf ("Marking isFrontier = true (based on inheritedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
            // returnAttribute.isFrontier = inheritedAttribute.isFrontier;
               returnAttribute.isFrontier = true;

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          if (inheritedAttribute.unparseUsingTokenStream == true)
             {
#if 1
               printf ("Marking unparseUsingTokenStream = true (based on inheritedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
               returnAttribute.unparseUsingTokenStream = true;
             }

          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if 1
               printf ("Marking unparseFromTheAST = true (based on inheritedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
               returnAttribute.unparseFromTheAST = true;
             }
#else
       // Mark these directly.
          returnAttribute.isFrontier              = inheritedAttribute.isFrontier;
          returnAttribute.unparseUsingTokenStream = inheritedAttribute.unparseUsingTokenStream;
          returnAttribute.unparseFromTheAST       = inheritedAttribute.unparseFromTheAST;

          if (inheritedAttribute.isFrontier == true)
             {
            // The frontier is by definition, to be unparsed from the AST.  The frontier is not 
            // defined in the inherited attribute when it can be unparsed from the token stream.
#if 0
               printf ("For inheritedAttribute.isFrontier == true: set returnAttribute.unparseFromTheAST = true \n");
#endif
               returnAttribute.unparseFromTheAST = true;
             }

          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if 0
               printf ("For inheritedAttribute.unparseFromTheAST == true: set returnAttribute.containsNodesToBeUnparsedFromTheAST = true \n");
#endif
               returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
             }

          if (inheritedAttribute.unparseUsingTokenStream == true)
             {
#if 0
               printf ("For inheritedAttribute.unparseUsingTokenStream == true: set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
               returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
             }

       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false);
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
#endif

#if 0
          printf ("After using info from the inheritedAttribute: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

#if 0
          if (synthesizedAttributeList.size() == 0)
             {
               returnAttribute.unparseUsingTokenStream = true;
             }
#endif

#if 0
       // Check if there is any frontier amongst the child IR nodes.
          bool isFrontier = false;
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
               if (synthesizedAttributeList[i].isFrontier == true)
                  {
                    isFrontier = true;
                  }
             }

          if (isFrontier == true)
             {
            // If there is a frontier amongst the children, then ALL of them are a part of the frontier.
               for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                  {
                    synthesizedAttributeList[i].isFrontier = true;
                  }
             }
#endif

       // bool nodeTouchesFronter = false;
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
            // ROSE_ASSERT(synthesizedAttributeList[i].node != NULL);
#if 0
               printf ("   --- synthesizedAttributeList[i=%zu].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                    i,synthesizedAttributeList[i].node,
                    synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                    synthesizedAttributeList[i].isFrontier                                  ? "true" : "false",
                    synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true" : "false",
                    synthesizedAttributeList[i].unparseFromTheAST                           ? "true" : "false",
                    synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true" : "false",
                    synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
               if (synthesizedAttributeList[i].node == NULL)
                  {
#if 0
                    printf ("Case of synthesizedAttributeList[i].node == NULL: skipping using info from this child synthesized attribute to set the returnAttribute \n");
#endif
                    continue;
                  }
               
            // if (synthesizedAttributeList[i].isFrontier == true)
            // if (synthesizedAttributeList[i].isFrontier == true || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
            // if ( (synthesizedAttributeList[i].isFrontier == true && synthesizedAttributeList[i].unparseFromTheAST == true) || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
            // if (synthesizedAttributeList[i].isFrontier == true || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
            // if (synthesizedAttributeList[i].isFrontier == true)
            // if (synthesizedAttributeList[i].isFrontier == true || synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
               if (synthesizedAttributeList[i].isFrontier == true)
                  {
#if 0
                    printf ("Marking containsNodesToBeUnparsedFromTheAST = true (based on synthesizedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
                 // returnAttribute.isFrontier = true;
                    returnAttribute.containsNodesToBeUnparsedFromTheAST = true;

                    if (returnAttribute.unparseUsingTokenStream == true)
                       {
#if 0
                         printf ("Marking returnAttribute.unparseUsingTokenStream = false (based on synthesizedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
                         returnAttribute.unparseUsingTokenStream = false;
                       }
#if 0
                    if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
                       {
                         returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                       }
#endif
#if 0
                    if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
                         returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                       }
#endif
                  }
                 else
                  {
                    if (synthesizedAttributeList[i].unparseUsingTokenStream == false && synthesizedAttributeList[i].unparseFromTheAST == false)
                       {
#if 0
                         printf ("Case of: synthesizedAttributeList[i].unparseUsingTokenStream == false && synthesizedAttributeList[i].unparseFromTheAST == false \n");
#endif
                         ROSE_ASSERT(synthesizedAttributeList[i].unparseFromTheAST == false);
                         ROSE_ASSERT(synthesizedAttributeList[i].unparseUsingTokenStream == false);

                      // returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
#if 1
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream == true)
                            {
                              returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                            }
#endif
#if 1
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == true)
                            {
                              returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                            }
#endif
#if 1
                      // Lack of AST usage in the subtree implies that there are nodes unparsed from the token stream.
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                            {
#if 0
                              printf ("Set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
                           // returnAttribute.unparseUsingTokenStream = true;
                              returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                            }
#endif
#if 0
                      // Lack of token stream usage in the subtree implies that there are nodes unparsed from the AST.
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream == false)
                            {
                           // returnAttribute.unparseUsingTokenStream = true;
                              returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                            }
#endif
                       }
                      else
                       {
#if 0
                         printf ("Case of: synthesizedAttributeList[i].unparseUsingTokenStream == true OR synthesizedAttributeList[i].unparseFromTheAST == true \n");
#endif
                         ROSE_ASSERT(synthesizedAttributeList[i].unparseUsingTokenStream == true || synthesizedAttributeList[i].unparseFromTheAST == true);

                         if (synthesizedAttributeList[i].unparseUsingTokenStream == true)
                            {
#if 0
                              printf ("Set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
                              returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                            }
                           else
                            {
                              if (synthesizedAttributeList[i].unparseFromTheAST == true)
                                 {
                                   printf ("Set returnAttribute.containsNodesToBeUnparsedFromTheAST = true \n");
                                   returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                                 }
                                else
                                 {
                                   printf ("ERROR: can't reach this case! \n");
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                  }

            // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
            //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
               ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                            (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                            (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );
#if 0
               printf ("At base of loop over synthesized attribute list elements i = %zu \n",i);
               printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
               printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
               printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
               printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
               printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
             }

       // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
       //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

          if (inheritedAttribute.unparseFromTheAST == true)
             {
            // This does not imply that this is a frontier node, since it could be that all of the children were frontier nodes and this is to be unparsed from the AST, but not a frontier???
            // ROSE_ASSERT(returnAttribute.isFrontier == true);
               ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false);
             }

          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
             {
               string name    = "token_frontier";

               string options = "color=\"green\"";

               FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);

               if (statement != NULL)
                  {
                    statement->setAttribute(name,attribute);
                  }
             }
#if 0
          printf ("After loop over all child attributes in synthesizedAttributeList: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
       // If this contains IR nodes to be unparsed from the AST and from the token stream then we have 
       // to report the list explicitly, else the action of unparsing from either the AST or the token 
       // stream can be reported using the single node (and not entered into the frontier list but 
       // progated usign the synthesized attribute.
       // if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
             {
               returnAttribute.unparseFromTheAST       = false;
               returnAttribute.unparseUsingTokenStream = false;
#if 0
               printf ("   --- Saving the list of relevant frontier nodes! \n");
#endif
               for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                  {
                    SgStatement* synthesized_attribute_statement = synthesizedAttributeList[i].node;
#if 0
                    printf ("   --- synthesized_attribute_statement = %p = %s \n",synthesized_attribute_statement,synthesized_attribute_statement != NULL ? synthesized_attribute_statement->class_name().c_str() : "null");
                    printf ("   ---   --- synthesizedAttributeList[i=%zu].isFrontier                                  = %s \n",i,synthesizedAttributeList[i].isFrontier ? "true" : "false");
                    printf ("   ---   --- synthesizedAttributeList[i=%zu].unparseUsingTokenStream                     = %s \n",i,synthesizedAttributeList[i].unparseUsingTokenStream ? "true" : "false");
                    printf ("   ---   --- synthesizedAttributeList[i=%zu].unparseFromTheAST                           = %s \n",i,synthesizedAttributeList[i].unparseFromTheAST ? "true" : "false");
                    printf ("   ---   --- synthesizedAttributeList[i=%zu].containsNodesToBeUnparsedFromTheAST         = %s \n",i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST ? "true" : "false");
                    printf ("   ---   --- synthesizedAttributeList[i=%zu].containsNodesToBeUnparsedFromTheTokenStream = %s \n",i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
                    if (synthesized_attribute_statement != NULL)
                       {
                         FrontierNode* frontierNode = new FrontierNode(synthesized_attribute_statement,synthesizedAttributeList[i].unparseUsingTokenStream,synthesizedAttributeList[i].unparseFromTheAST);
                         if (synthesizedAttributeList[i].unparseUsingTokenStream == true)
                            {
                              returnAttribute.frontierNodes.push_back(frontierNode);
                            }
                           else
                            {
                              if (synthesizedAttributeList[i].unparseFromTheAST == true)
                                 {
                                   returnAttribute.frontierNodes.push_back(frontierNode);
                                 }
                                else
                                 {
#if 0
                                   printf ("Current node = %p = %s not added to frontier node list (add any lists from synthesizedAttributeList[i=%zu].frontierNodes.size() = %zu) \n",n,n->class_name().c_str(),i,synthesizedAttributeList[i].frontierNodes.size());
#endif
                                // We don't record nodes that are just containing subtrees to be unparsed from the AST.
                                // if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                                      {
                                        for (size_t j = 0; j < synthesizedAttributeList[i].frontierNodes.size(); j++)
                                           {
                                             FrontierNode* frontierNode = synthesizedAttributeList[i].frontierNodes[j];
                                             ROSE_ASSERT(frontierNode != NULL);

                                             returnAttribute.frontierNodes.push_back(frontierNode);
#if 0
                                             printf ("Include synthesized_attribute frontierNodes into frontierNodes (%p = %s) \n",frontierNode,frontierNode->display().c_str());
#endif
                                           }
                                      }
                                 }
                            }
                       }
                      else
                       {
#if 0
                         printf ("WARNING: synthesized_attribute_statement == NULL \n");
#endif
                       }
#if 0
                 // ROSE_ASSERT(synthesized_attribute_statement != NULL);
                    if (synthesized_attribute_statement != NULL)
                       {
                         printf ("   ---   --- Include synthesized_attribute_statement into frontierNodes (%p = %s) \n",synthesized_attribute_statement,synthesized_attribute_statement->class_name().c_str());
                         returnAttribute.frontierNodes.push_back(synthesized_attribute_statement);

                         printf ("   ---   --- synthesizedAttributeList[i=%zu].containsNodesToBeUnparsedFromTheAST = %s \n",i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST ? "true" : "false");

                      // We don't record nodes that are just containing subtrees to be unparsed from the AST.
                         if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                            {
                              for (size_t j = 0; j < synthesizedAttributeList[i].frontierNodes.size(); j++)
                                 {
                                   SgStatement* statement = synthesizedAttributeList[i].frontierNodes[j];
                                   ROSE_ASSERT(statement != NULL);

                                   returnAttribute.frontierNodes.push_back(statement);

                                   printf ("Include synthesized_attribute frontierNodes into frontierNodes (%p = %s) \n",statement,statement->class_name().c_str());
                                 }
                            }
                       }
#endif
                  }
             }
            else
             {
#if 0
               printf ("   --- List of frontier nodes not saved, but the current node is saved \n");
#endif
            // SgStatement* statement = synthesizedAttributeList[i].node;
            // ROSE_ASSERT(statement != NULL);
               if (statement != NULL)
                  {
                    FrontierNode* frontierNode = new FrontierNode(statement,returnAttribute.unparseUsingTokenStream,returnAttribute.unparseFromTheAST);
                    returnAttribute.frontierNodes.push_back(frontierNode);

                 // If this is marked as containing nodes to be unparsed from either the token stream or the AST then reset the unparsing for this specific node.
                    if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true || returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
                         returnAttribute.unparseFromTheAST       = false;
                         returnAttribute.unparseUsingTokenStream = false;
                       }
                  }
                 else
                  {
                 // This case happens for test_CplusplusMacro_C.c, which has nothing but CPP directives.
                  }

#if 0
               if (returnAttribute.unparseUsingTokenStream == true)
                  {
                    SgStatement* statement = isSgStatement(n);
                    returnAttribute.frontierNodes.push_back(statement);
                    printf ("Include current AST node into frontierNodes (%p = %s) ",statement,statement->class_name().c_str());
                  }
                 else
                  {
                 // This does not yet identify this how as to be unparsed from the AST.
                 // ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheAST == true);
                 // returnAttribute.frontierNodes.push_back(synthesizedAttributeList[i].node);              
                  }
#endif
             }

#if 0
          printf ("Frontier (n = %p = %s): ",n,n->class_name().c_str());
          for (size_t j = 0; j < returnAttribute.frontierNodes.size(); j++)
             {
            // SgStatement* statement = returnAttribute.frontierNodes[j];
               FrontierNode* frontierNode = returnAttribute.frontierNodes[j];
               ROSE_ASSERT(frontierNode != NULL);
               SgStatement* statement = frontierNode->node;
               ROSE_ASSERT(statement != NULL);

            // printf (" (%p = %s) ",statement,statement->class_name().c_str());
               printf (" (%s) ",frontierNode->display().c_str());
             }
          printf (" Frontier End \n");
#endif
#if 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Default settings.
       // returnAttribute.unparseUsingTokenStream = true;
          returnAttribute.isFrontier                                  = false;
          returnAttribute.unparseUsingTokenStream                     = false;
          returnAttribute.unparseFromTheAST                           = false;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = false;
          returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = false;

          returnAttribute.isFrontier                                  = inheritedAttribute.isFrontier;
          returnAttribute.unparseUsingTokenStream                     = inheritedAttribute.unparseUsingTokenStream;
          returnAttribute.unparseFromTheAST                           = inheritedAttribute.unparseFromTheAST;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = inheritedAttribute.containsNodesToBeUnparsedFromTheAST;
#if 0
          printf ("Case of non-SgStatement and non-SgSourceFile: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
        }

#if 0
     printf ("### Leaving FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): BOTTOM n = %p = %s \n",n,n->class_name().c_str());
#endif

#if 1
  // This can't be applied to non-statements.
  // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
  //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
     ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );
#endif

     return returnAttribute;
   }


void
frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile )
   {
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     FrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): calling traverse() sourceFile = %p \n",sourceFile);
#endif

  // fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseInputFiles(sourceFile,inheritedAttribute);
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);

#if 0
     printf ("   --- topAttribute.isFrontier                                  = %s \n",topAttribute.isFrontier ? "true" : "false");
     printf ("   --- topAttribute.unparseUsingTokenStream                     = %s \n",topAttribute.unparseUsingTokenStream ? "true" : "false");
     printf ("   --- topAttribute.unparseFromTheAST                           = %s \n",topAttribute.unparseFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",topAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",topAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

#if 0
     printf ("Final Frontier (topAttribute.frontierNodes.size() = %zu) (n = %p = %s): ",topAttribute.frontierNodes.size(),sourceFile,sourceFile->class_name().c_str());
#endif
     for (size_t j = 0; j < topAttribute.frontierNodes.size(); j++)
        {
       // SgStatement* statement = topAttribute.frontierNodes[j];
       // ROSE_ASSERT(statement != NULL);
          FrontierNode* frontierNode = topAttribute.frontierNodes[j];
          ROSE_ASSERT(frontierNode != NULL);
          SgStatement* statement = frontierNode->node;
          ROSE_ASSERT(statement != NULL);

#if 0
          printf (" (%p = %s) ",statement,statement->class_name().c_str());
#endif
          string name    = "token_frontier";
       // string options = "fillcolor=\"greenyellow\",style=filled";
          string options;
          if (frontierNode->unparseUsingTokenStream == true)
             {
               options = "fillcolor=\"greenyellow\",style=filled";
             }
            else
             {
               ROSE_ASSERT(frontierNode->unparseFromTheAST == true);
               options = "fillcolor=\"skyblue\",style=filled";
             }
          FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( statement, name, options);

          statement->setAttribute(name,attribute);
        }
#if 0
     printf (" Frontier End \n");
#endif

  // Output an optional graph of the AST (just the tree, so we can identify the frontier)
     SgFileList* fileList = isSgFileList(sourceFile->get_parent());
     ROSE_ASSERT(fileList != NULL);
     SgProject* project = isSgProject(fileList->get_parent());
     ROSE_ASSERT(project != NULL);

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier defined \n");
#endif

     generateDOT ( *project, "_token_unparsing_frontier" );

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier defined \n");
#endif

  // Now traverse the AST and record the linked list of nodes to be unparsed as tokens and from the AST.
  // So that we can query next and last statements and determin if they were unparsed from the token 
  // stream or the AST.  Not clear if the edges of token-stream/AST unparsing should be unparsed from the 
  // token stream leading trailing token information or from the AST using the attached CPP info.

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Calling previousAndNextNodeTraversal() \n");
#endif

     string name_next     = "frontier_next";
     string name_previous = "frontier_previous";
     string options = "color=\"red\"";

  // std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = previousAndNextNodeTraversal(sourceFile);
     std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = computePreviousAndNextNodes(sourceFile->get_globalScope(),topAttribute.frontierNodes);

#if 0
     printf ("Output previousAndNextNodeMap for frontier: \n");
#endif
     for (size_t j = 0; j < topAttribute.frontierNodes.size(); j++)
        {
          ROSE_ASSERT(topAttribute.frontierNodes[j]->node != NULL);
          FrontierNode* frontierNode = topAttribute.frontierNodes[j];
          ROSE_ASSERT(frontierNode != NULL);
          SgStatement* frontier_statement = frontierNode->node;
          ROSE_ASSERT(frontier_statement != NULL);
#if 0
          printf ("frontier_statement = %p = %s \n",frontier_statement,frontier_statement->class_name().c_str());
#endif
          std::map<SgNode*,PreviousAndNextNodeData*>::iterator i = previousAndNextNodeMap.find(frontier_statement);
       // ROSE_ASSERT(i != previousAndNextNodeMap.end());
          if (i != previousAndNextNodeMap.end())
             {
#if 0
               printf ("   --- it for i->first = %p = %s i->second = %p i->second->previous = %p = %s i->second->next = %p = %s \n",
                    i->first,i->first->class_name().c_str(),i->second,
                    i->second->previous,i->second->previous != NULL ? i->second->previous->class_name().c_str() : "null",
                    i->second->next,i->second->next != NULL ? i->second->next->class_name().c_str() : "null");
#endif
               SgStatement* statement = isSgStatement(i->first);
               if (statement != NULL)
                  {
                    PreviousAndNextAttribute* edgeAttribute_next = new PreviousAndNextAttribute(i->first, i->second->next, name_next, options);
                    statement->setAttribute(name_next,edgeAttribute_next);
                    PreviousAndNextAttribute* edgeAttribute_previous = new PreviousAndNextAttribute(i->first, i->second->previous, name_previous, options);
                    statement->setAttribute(name_previous,edgeAttribute_previous);
                  }
             }
            else
             {
               printf ("WARNING: --- could not find associated IR node in previousAndNextNodeMap \n");
             }
        }

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier AND edges defined \n");
#endif

     generateDOT(*project,"_token_unparsing_frontier_with_next_previous_edges");

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier AND edges defined \n");
#endif

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Calling previousAndNextNodeTraversal() \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }




FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(SgNode* n, std::string name, std::string options)
   : node(n), name(name), options(options)
   {
#if 0
     printf ("In FrontierDetectionForTokenStreamMappingAttribute constructor! name = %s options = %s \n",name.c_str(),options.c_str());
#endif
  // ROSE_ASSERT(false);
   }

FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(const FrontierDetectionForTokenStreamMappingAttribute & X)
   {
     printf ("In FrontierDetectionForTokenStreamMappingAttribute copy constructor! \n");
     ROSE_ASSERT(false);
   }

string
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

  // return "red";
  // return "color=\"red\"";
     return options;
   }


vector<AstAttribute::AttributeEdgeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeEdgeInfo>();
   }


vector<AstAttribute::AttributeNodeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeNodeInfo>();
   }

AstAttribute*
FrontierDetectionForTokenStreamMappingAttribute::copy()
   {
  // Support for the coping of AST and associated attributes on each IR node (required for attributes 
  // derived from AstAttribute, else just the base class AstAttribute will be copied).

     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::copy(): not implemented! \n");
     ROSE_ASSERT(false);

     return new FrontierDetectionForTokenStreamMappingAttribute(*this);
   }

#if 0
void
FrontierDetectionForTokenStreamMappingAttribute::set_unparseUsingTokenStream( bool flag )
   {
     unparseUsingTokenStream = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_unparseUsingTokenStream()
   {
     return unparseUsingTokenStream;
   }


void
FrontierDetectionForTokenStreamMappingAttribute::set_unparseFromTheAST( bool flag )
   {
     unparseFromTheAST = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_unparseFromTheAST()
   {
     return unparseFromTheAST;
   }

void
FrontierDetectionForTokenStreamMappingAttribute::set_containsNodesToBeUnparsedFromTheAST( bool flag )
   {
     containsNodesToBeUnparsedFromTheAST = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_containsNodesToBeUnparsedFromTheAST()
   {
     return containsNodesToBeUnparsedFromTheAST;
   }
#endif


FrontierNode::FrontierNode(SgStatement* node, bool unparseUsingTokenStream, bool unparseFromTheAST)
   : node(node), 
     unparseUsingTokenStream(unparseUsingTokenStream), 
     unparseFromTheAST(unparseFromTheAST)
   {
  // Enforce specific constraints.
     ROSE_ASSERT(node != NULL);
  // ROSE_ASSERT( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) );
     ROSE_ASSERT( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) || (unparseUsingTokenStream == false && unparseFromTheAST == false));
   }


std::string 
FrontierNode::display()
   {
     string s;

     s += string("node=") + StringUtility::numberToString(node) + string(",") + node->class_name() + string(":");
     s += string("(TS=")  + (unparseUsingTokenStream == true ? "true" : "false"); 
     s += string(",AST=") + (unparseFromTheAST == true ? "true" : "false") + ")";
     s += string(" ");

     return s;
   }

