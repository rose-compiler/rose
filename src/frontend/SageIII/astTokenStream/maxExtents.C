#include "sage3basic.h"

using namespace std;

#include "maxExtents.h"

MaxSourceExtents::InheritedAttribute::InheritedAttribute(SgSourceFile* input_sourceFile, int input_start_line, int input_start_column, int input_end_line, int input_end_column, bool processed) 
   : processChildNodes(processed),
     sourceFile(input_sourceFile)
   {
     start_line   = input_start_line;
     start_column = input_start_column;
     end_line     = input_end_line;
     end_column   = input_end_column;

#if 0
     printf ("In InheritedAttribute constructor: start_line = %d start_column %d end_line = %d end_column = %d \n",start_line,start_column,end_line,end_column);
#endif
   }

MaxSourceExtents::InheritedAttribute::InheritedAttribute ( const InheritedAttribute & X ) 
   : processChildNodes(X.processChildNodes),
     sourceFile(X.sourceFile)
   {
     start_line   = X.start_line;
     start_column = X.start_column;
     end_line     = X.end_line;
     end_column   = X.end_column;
   }


MaxSourceExtents::SynthesizedAttribute::SynthesizedAttribute()
   {
#if 0
     printf ("In SynthesizedAttribute(): default constructor \n");
#endif
     node = NULL;
   }

MaxSourceExtents::SynthesizedAttribute::SynthesizedAttribute(SgNode* n, int input_start_line, int input_start_column, int input_end_line, int input_end_column)
   {
     ROSE_ASSERT(n != NULL);

     start_line   = input_start_line;
     start_column = input_start_column;
     end_line     = input_end_line;
     end_column   = input_end_column;

#if 0
     printf ("In SynthesizedAttribute(SgNode* n) constructor: n = %p = %s start_line = %d start_column %d end_line = %d end_column = %d \n",n,n->class_name().c_str(),start_line,start_column,end_line,end_column);
#endif
     node = n;
   }

MaxSourceExtents::SynthesizedAttribute::SynthesizedAttribute(const SynthesizedAttribute & X)
   {
     node = X.node;

     start_line   = X.start_line;
     start_column = X.start_column;
     end_line     = X.end_line;
     end_column   = X.end_column;

#if 0
     printf ("In SynthesizedAttribute(const SynthesizedAttribute & X): copy constructor: n = %p = %s start_line = %d start_column %d end_line = %d end_column = %d \n",X.node,(X.node != NULL) ? X.node->class_name().c_str() : "null",start_line,start_column,end_line,end_column);
#endif
   }


MaxSourceExtents::InheritedAttribute
MaxSourceExtents::SourceExtentsTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute)
   {
#if 0
     printf ("In SourceExtentsTraversal::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     return inheritedAttribute;
   }


MaxSourceExtents::SynthesizedAttribute
MaxSourceExtents::SourceExtentsTraversal::evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes childAttributes )
   {
#define DEBUG_evaluateSynthesizedAttribute 0

#if DEBUG_evaluateSynthesizedAttribute
     printf ("\n\nIn SourceExtentsTraversal::evaluateSynthesizedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     const int max_value = 9999999;

     SgLocatedNode* this_locatedNode = isSgLocatedNode(n);

     ROSE_ASSERT(this_locatedNode != NULL);
     bool isInSameFile = this_locatedNode->get_startOfConstruct()->isSameFile(inheritedAttribute.sourceFile);

#if DEBUG_evaluateSynthesizedAttribute
     printf ("isInSameFile = %s \n",isInSameFile ? "true" : "false");
#endif
#if 0
     if (isInSameFile == false)
        {
          printf ("Warning: SourceExtentsTraversal::evaluateSynthesizedAttribute(): skipping processing: (isInSameFile == false) \n");
        }
#endif

     if (this_locatedNode == NULL)
        {
          return SynthesizedAttribute(n,max_value,max_value,0,0);
        }

     bool isCompilerGenerated = this_locatedNode->isCompilerGenerated();

  // Set these to the default values if this is a compiler generate node or a node from a different file.
     int this_start_line   = (isCompilerGenerated == false && isInSameFile == true) ? this_locatedNode->get_startOfConstruct()->get_line() : max_value;
     int this_start_column = (isCompilerGenerated == false && isInSameFile == true) ? this_locatedNode->get_startOfConstruct()->get_col()  : max_value;
     int this_end_line     = (isCompilerGenerated == false && isInSameFile == true) ? this_locatedNode->get_endOfConstruct()->get_line()   : 0;
     int this_end_column   = (isCompilerGenerated == false && isInSameFile == true) ? this_locatedNode->get_endOfConstruct()->get_col()    : 0;

#if DEBUG_evaluateSynthesizedAttribute
     printf ("   --- isCompilerGenerated = %s this_start_line = %d this_start_column = %d this_end_line = %d this_end_column = %d \n",
          isCompilerGenerated ? "true" : "false",this_start_line,this_start_column,this_end_line,this_end_column);
#endif

     int local_start_line   = (this_start_line > 0) ? this_start_line : max_value;
     int local_start_column = (this_start_column > 0) ? this_start_column : max_value;
     int local_end_line     = (this_end_line > 0) ? this_end_line : 0;
     int local_end_column   = (this_end_column > 0) ? this_end_column : 0;

#if DEBUG_evaluateSynthesizedAttribute
     printf ("   --- (initial) local_start_line = %d local_start_column = %d local_end_line = %d local_end_column = %d \n",local_start_line,local_start_column,local_end_line,local_end_column);
     printf ("   --- Evaluate children (size = %zu): \n",childAttributes.size());
#endif

     for (size_t i = 0; i < childAttributes.size(); i++)
        {
       // ROSE_ASSERT(childAttributes[i].node != NULL);

          string child_name = n->get_traversalSuccessorNamesContainer()[i];

          if (childAttributes[i].node != NULL)
             {
#if DEBUG_evaluateSynthesizedAttribute
               printf ("   ---   --- child = %p = %s = %s \n",childAttributes[i].node,child_name.c_str(),childAttributes[i].node->class_name().c_str());
#endif
               SgLocatedNode* child_locatedNode = isSgLocatedNode(childAttributes[i].node);
               ROSE_ASSERT(child_locatedNode != NULL);

            // bool child_isCompilerGenerated = child_locatedNode->isCompilerGenerated();

            // Get the child values from the child attribute (not the asociated IR node.
               int child_start_line   = childAttributes[i].start_line;
               int child_start_column = childAttributes[i].start_column;
               int child_end_line     = childAttributes[i].end_line;
               int child_end_column   = childAttributes[i].end_column;
#if DEBUG_evaluateSynthesizedAttribute
               printf ("   ---   ---   --- child_start_line = %d child_start_column = %d child_end_line = %d child_end_column = %d \n",child_start_line,child_start_column,child_end_line,child_end_column);
#endif
               if (child_start_line < local_start_line)
                  {
                    local_start_line   = child_start_line;
                    local_start_column = child_start_column;
#if DEBUG_evaluateSynthesizedAttribute
                    printf ("   ---   ---   --- local_start_line   = %d (reset)  \n",local_start_line);
                    printf ("   ---   ---   --- local_start_column = %d (reset)  \n",local_start_column);
#endif
                  }
                 else
                  {
                    if (child_start_line == local_start_line)
                       {
                         if (child_start_column < local_start_column)
                            {
                              local_start_column = child_start_column;
#if DEBUG_evaluateSynthesizedAttribute
                              printf ("   ---   ---   --- local_start_column = %d (reset)  \n",local_start_column);
#endif
                            }
                           else
                            {
#if DEBUG_evaluateSynthesizedAttribute
                              printf ("   ---   ---   --- local_start_column = %d (not reset)  \n",local_start_column);
#endif
                            }
                       }
                      else
                       {
#if DEBUG_evaluateSynthesizedAttribute
                         printf ("   ---   ---   --- local_start_line   = %d (not reset)  \n",local_start_line);
                         printf ("   ---   ---   --- local_start_column = %d (not reset)  \n",local_start_column);
#endif
                       }
                  }

               if (child_end_line > local_end_line)
                  {
                    local_end_line   = child_end_line;
                    local_end_column = child_end_column;
#if DEBUG_evaluateSynthesizedAttribute
                    printf ("   ---   ---   --- local_end_line   = %d (reset)  \n",local_end_line);
                    printf ("   ---   ---   --- local_end_column = %d (reset)  \n",local_end_column);
#endif
                  }
                 else
                  {
                    if (child_end_line == local_end_line)
                       {
                         if (child_end_column > local_end_column)
                            {
                              local_end_column = child_end_column;
#if DEBUG_evaluateSynthesizedAttribute
                              printf ("   ---   ---   --- local_end_column = %d (reset)  \n",local_end_column);
#endif
                            }
                           else
                            {
#if DEBUG_evaluateSynthesizedAttribute
                              printf ("   ---   ---   --- local_end_column = %d (not reset)  \n",local_end_column);
#endif
                            }
                       }
                      else
                       {
#if DEBUG_evaluateSynthesizedAttribute
                         printf ("   ---   ---   --- local_end_line   = %d (not reset)  \n",local_end_line);
                         printf ("   ---   ---   --- local_end_column = %d (not reset)  \n",local_end_column);
#endif
                       }
                  }
             }
            else
             {
#if DEBUG_evaluateSynthesizedAttribute
               printf ("   ---   --- child = NULL = %s \n",child_name.c_str());
#endif
             }
        }

#if DEBUG_evaluateSynthesizedAttribute
     printf ("   --- (final) local_start_line = %d local_start_column = %d local_end_line = %d local_end_column = %d \n",local_start_line,local_start_column,local_end_line,local_end_column);
#endif

     SynthesizedAttribute return_attribute(n,local_start_line,local_start_column,local_end_line,local_end_column);

     return return_attribute;
   }


void MaxSourceExtents::computeMaxSourceExtents(SgSourceFile* sourceFile, SgNode* n, int & start_line, int & start_column, int & end_line, int & end_column)
   {
     ROSE_ASSERT(sourceFile != NULL);

     const int max_value = 9999999;

     start_line   = max_value;
     start_column = max_value;
     end_line     = 0;
     end_column   = 0;

     bool processed = true;

     InheritedAttribute inheritedAttribute(sourceFile,start_line,start_column,end_line,end_column,processed);

     SourceExtentsTraversal traversal;

     SynthesizedAttribute return_attribute = traversal.traverse(n,inheritedAttribute);

     start_line   = return_attribute.start_line;
     start_column = return_attribute.start_column;
     end_line     = return_attribute.end_line;
     end_column   = return_attribute.end_column;

#if 0
     printf ("In computeMaxSourceExtents(): start_line = %d start_column = %d end_line = %d end_column = %d \n",start_line,start_column,end_line,end_column);
#endif
#if 0
     printf ("Exiting as a test in MaxSourceExtents::computeMaxSourceExtents() \n");
     ROSE_ASSERT(false);
#endif
   }

