
#ifndef ASTPROCESSING_TOPDOWN_IMPL_H
#define ASTPROCESSING_TOPDOWN_IMPL_H

namespace AstProcessing {
   template< typename Evaluator,
   typename InheritedAttribute >
   void TopDown<Evaluator, InheritedAttribute>::
      traverse          ( SgNode *astNode, const InheritedAttribute& inheritedValue )
   {
      //atTraversalStart();
      performTraverse( astNode, inheritedValue );
      //atTraversalEnd();
   }

   template< typename Evaluator,
             typename InheritedAttribute >
   void TopDown<Evaluator, InheritedAttribute>::
      traverseWithinFile( SgNode *astNode, const InheritedAttribute& inheritedValue )
   {
      traversalConstraint = true;
      fileToVisit = isSgFile( astNode );
      ROSE_ASSERT( fileToVisit );

      ROSE_ASSERT( inFileToTraverse( astNode, traversalConstraint, fileToVisit ) );

      traverse( astNode, inheritedValue );
      traversalConstraint = false;
   }

   template< typename Evaluator,
             typename InheritedAttribute >
   void TopDown<Evaluator, InheritedAttribute>::
      traverseInputFiles( SgProject *project, const InheritedAttribute& inheritedValue )
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
         traverseWithinFile( *fl_iter, inheritedValue );
      }
   }

   // implementation of PreOrder Traversal
   template< typename Evaluator,
             typename InheritedAttribute >
   void TopDown<Evaluator, InheritedAttribute>::
      performTraverse( SgNode *astNode, InheritedAttribute inheritedValue )
   {
      // check for null pointer
      if( astNode && inFileToTraverse( astNode, traversalConstraint, fileToVisit ) )
      {
         const size_t numSuccessors( astNode->get_numberOfTraversalSuccessors() );
         // Pre evaluation ( aka inherited Attribute )
         inheritedValue = evaluateInheritedAttribute( astNode, inheritedValue );

         // loop through children
         for( size_t index = 0; index < numSuccessors; ++index )
         {
            SgNode *child = NULL;
            child = astNode->get_traversalSuccessorByIndex( index );

            //process child
            performTraverse( child, inheritedValue );
         }
      } // end if( astNode && inFileToTraverse( astNode, traversalConstraint, fileToVisit ) )
   }
} // end namespace AstProcessing

#endif
