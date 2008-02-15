#ifndef MARK_OVERLOADED_TEMPLATE_INSTANTIATIONS_H
#define MARK_OVERLOADED_TEMPLATE_INSTANTIATIONS_H

// DQ (5/22/2005):
/*! \brief Mark any template instatiations that are overloaded (we can't output these since g++ can't resolve them).

    This function marks IR nodes that can't be output because of a bug in the GNU g++ compile.
    Member functions that are overloaded can't be resolved in the template specialization.
    e.g. 
    template <typename T> \n
    class X { \n
         X(T t){}; \n
         template<typename S> X(S s){} \n
    };

    \internal This is a work around for a bug in GNU g++ (version 3.3.x, 3.4.x, and 4.x (as I understand it))
              Note that this bug in g++ prevents the transformation of non-template member functions 
              in template classes that are overloaded with template member function in the same templated class.

 */
void markOverloadedTemplateInstantiations( SgNode* node );

/*! \brief Mark any template instatiations that are overloaded (we can't output these since g++ can't resolve them).
 */
class MarkOverloadedTemplateInstantiations : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for MARK_OVERLOADED_TEMPLATE_INSTANTIATIONS_H
#endif
