#include "rose.h"

// #include "general_token_defs.h"

#include "frontierDetection.h"

using namespace std;


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute()
   {
     containsFrontier = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed)
   {
     containsFrontier = false;
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
   {
     containsFrontier = X.containsFrontier;
   }




FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     containsFrontier = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n)
   {
     containsFrontier = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
     containsFrontier = X.containsFrontier;
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

#if 1
  // Ignore IR nodes that are front-end specific (declarations of builtin functions, etc.).
     if (n->get_file_info()->isFrontendSpecific() == false)
        {
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());

       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }
#endif

     FrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute;

     SgStatement* statement = isSgStatement(n);
  // if (statement != NULL && random_counter > 30 && random_counter < 40)
     if (statement != NULL)
        {
          string name    = "token_frontier";
          string options = "color=\"blue\"";

          if (random_counter > 30 && random_counter < 40)
             {
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",random_counter,statement,statement->class_name().c_str());

               options = "color=\"red\"";

               returnAttribute.containsFrontier = true;
             }

       // AstAttribute::AttributeNodeInfo* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);
          AstAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( (SgNode*) n, name, options);

          statement->setAttribute(name,attribute);
        }

  // return FrontierDetectionForTokenStreamMapping_InheritedAttribute();
     return returnAttribute;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
FrontierDetectionForTokenStreamMapping::FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute;


     if (inheritedAttribute.containsFrontier == true)
        {
#if 1
          printf ("Marking containsFrontier = true (based on inheritedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
          returnAttribute.containsFrontier = inheritedAttribute.containsFrontier;
        }

     for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
        {
          if (synthesizedAttributeList[i].containsFrontier == true)
             {
#if 1
               printf ("Marking containsFrontier = true (based on synthesizedAttribute) at node = %p = %s \n",n,n->class_name().c_str());
#endif
               returnAttribute.containsFrontier = true;
             }
        }

  // return FrontierDetectionForTokenStreamMapping_SynthesizedAttribute();
     return returnAttribute;
   }


void
frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile )
   {
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     FrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

#if 1
     printf ("In frontierDetectionForTokenStreamMapping(): calling traverse() sourceFile = %p \n",sourceFile);
#endif

     fdTraversal.traverse(sourceFile,inheritedAttribute);

  // Output an optional graph of the AST (just the tree, so we can identify the frontier)
     SgFileList* fileList = isSgFileList(sourceFile->get_parent());
     ROSE_ASSERT(fileList != NULL);
     SgProject* project = isSgProject(fileList->get_parent());
     ROSE_ASSERT(project != NULL);

#if 1
     printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier defined \n");
#endif

     generateDOT ( *project, "_token_unparsing_frontier" );

#if 1
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier defined \n");
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
     printf ("In FrontierDetectionForTokenStreamMappingAttribute constructor! name = %s options = %s \n",name.c_str(),options.c_str());
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
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions(): not implemented! \n");
  // ROSE_ASSERT(false);

  // return "red";
  // return "color=\"red\"";
     return options;
   }


vector<AstAttribute::AttributeEdgeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo()
   {
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo(): not implemented! \n");
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeEdgeInfo>();
   }


vector<AstAttribute::AttributeNodeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo()
   {
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo(): not implemented! \n");
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

