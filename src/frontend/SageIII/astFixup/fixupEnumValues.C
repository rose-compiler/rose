// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupEnumValues.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
fixupEnumValues()
   {
  // DQ (3/10/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup reference to enum declarations in enum values:");

     FixupEnumValues t;

  // t.traverseMemoryPool();
     SgEnumVal::traverseMemoryPoolNodes(t);
   }

void
FixupEnumValues::visit(SgNode* node)
   {
#if 0
     printf ("##### FixupEnumValues::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

     SgEnumVal* enumVal = isSgEnumVal(node);
     ROSE_ASSERT(enumVal != NULL);

     if (enumVal->get_declaration() == NULL)
        {
          ROSE_ASSERT(enumVal->get_startOfConstruct() != NULL);
          ROSE_ASSERT(enumVal->get_endOfConstruct() != NULL);

       // printf ("Found an enum value with a NULL declaration, fixup the declaration! \n");
       // enumVal->get_startOfConstruct()->display("Found an enum value with a NULL declaration");

          SgClassDefinition* enclosingClassDefinition = TransformationSupport::getClassDefinition(enumVal);
          ROSE_ASSERT(enclosingClassDefinition != NULL);

       // Now search for the enumVal name in the symbol table
          SgEnumFieldSymbol* enumFieldSymbol = enclosingClassDefinition->lookup_enum_field_symbol(enumVal->get_name());
          ROSE_ASSERT(enumFieldSymbol != NULL);
          if (enumFieldSymbol != NULL)
             {
            // We have now found the enum symbol in the scope of the class where the enum value was used.  
            // It could have been elsewhere and we don't search there, it could be in a namespace that was 
            // included using a using directive, so this could be very complex if it is not in the enclosing 
            // class.  To handle the case of where we can't find it, we ALLOW the declaration in the enumVal 
            // to be NULL, but we try to set it where we can easily figure it out.

                SgInitializedName* enumFieldName = enumFieldSymbol->get_declaration();
                SgNode* parentNode = enumFieldName->get_parent();
                ROSE_ASSERT(parentNode != NULL);
                SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(parentNode);
                ROSE_ASSERT(enumDeclaration != NULL);

             // Set the declaration to the enum declaration found!
             // printf ("Fixup the NULL declaration in enumVal = %p = %s with valid enum declaration = %p = %s \n",enumVal,enumVal->get_name().str(),enumDeclaration,enumDeclaration->get_name().str());
                enumVal->set_declaration(enumDeclaration);
             }


        }
   }

