
#ifndef ASTPROCESSING_TOPDOWNBOTTOMUP_IMPL_H
#define ASTPROCESSING_TOPDOWNBOTTOMUP_IMPL_H

namespace AstProcessing {
   template< typename Evaluator,
             typename InheritedAttribute,
             typename SynthesizedAttribute >
   SynthesizedAttribute TopDownBottomUp<Evaluator, InheritedAttribute, SynthesizedAttribute>::
      traverse          (       SgNode              *astNode,
                          const InheritedAttribute& inheritedValue)
   {
      synthesizedAttributes->resetStack();
      //atTraversalStart();
      return performTraverse( astNode, inheritedValue );
      //atTraversalEnd();
   }

   template< typename Evaluator,
             typename InheritedAttribute,
             typename SynthesizedAttribute >
   SynthesizedAttribute TopDownBottomUp<Evaluator, InheritedAttribute, SynthesizedAttribute>::
      traverseWithinFile(       SgNode              *astNode,
                          const InheritedAttribute& inheritedValue )
   {
      traversalConstraint = true;
      fileToVisit = isSgFile( astNode );
      ROSE_ASSERT( fileToVisit );

      ROSE_ASSERT( inFileToTraverse( astNode, traversalConstraint, fileToVisit ) );

      SynthesizedAttribute ret = traverse( astNode );
      traversalConstraint = false;

      return ret;
   }
   
   template< typename Evaluator,
             typename InheritedAttribute,
             typename SynthesizedAttribute >
   void TopDownBottomUp<Evaluator, InheritedAttribute, SynthesizedAttribute>::
      traverseInputFiles(       SgProject           *project,
                          const InheritedAttribute& inheritedValue )
   {
      const SgFilePtrList& fList( project->get_fileList() );

      // DQ (9/1/2008): It is observed that this prevents a SgProject from being built on the generated DOT file!
      // We might want a better design to be used here or call the evaluation directly to force the handling of 
      // inherited and synthesized attributes on the SgProject.  This detail effect the handling of multiple
      // files on the command line (something we want to get a global perspective on if possible)
      if ( SgProject::get_verbose() > 0 )
         printf ("Warning: The traverseInputFiles() iteration over the file list prevents the evaluation of inherited and synthesized attributes on the SgProject IR node! \n");

      for (SgFilePtrList::const_iterator fl_iter = fList.begin(); fl_iter != fList.end(); fl_iter++)
      {
         ROSE_ASSERT( *fl_iter != NULL );
         traverseWithinFile( *fl_iter );
      }
   }

   // implementation of the traversal
   template< typename Evaluator,
             typename InheritedAttribute,
             typename SynthesizedAttribute >
   SynthesizedAttribute TopDownBottomUp<Evaluator, InheritedAttribute, SynthesizedAttribute>::
      performTraverse( SgNode *astNode, InheritedAttribute inheritedValue )
   {
      // check for null pointer and if we are in the file
      if( astNode && inFileToTraverse( astNode, traversalConstraint, fileToVisit ) )
      {
         const size_t numSuccessors( astNode->get_numberOfTraversalSuccessors() );
         // Pre evaluation ( aka inherited Attribute )
         inheritedValue = evaluateInheritedAttribute( astNode, inheritedValue );

         // loop through children ...
         for( size_t index = 0; index < numSuccessors; ++index )
         {
            SgNode *child = NULL;
            child = astNode->get_traversalSuccessorByIndex( index );
            // process child
            synthesizedAttributes->push( performTraverse( child, inheritedValue ) );
         }
         synthesizedAttributes->setFrameSize( numSuccessors );
         ROSE_ASSERT( synthesizedAttributes->size() == numSuccessors );
         // Post evaluation ( aka synthesizedAttributes )
         return evaluateSynthesizedAttribute( astNode, inheritedValue, *synthesizedAttributes );
      } // end if( astNode && inFileToTraverse( astNode, traversalConstraint, fileToVisit ) )
      else
      {
         return defaultSynthesizedAttribute( inheritedValue );
      }
   }
} // end namespace AstProcessing

#endif
