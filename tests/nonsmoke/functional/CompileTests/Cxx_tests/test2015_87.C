// This demonstrates a bug in the EDG handling on GNU 4.4.7 compiler using Intel v14 as ahte alternative backend compiler.
// This is because of the use of boost 
// identifier "fexcept_t" is undefined fexcept_t m_flags;

// It also demonstrates a different bug in ROSE, using GNU 4.8.3 and no specified alternative backend.
// src/frontend/SageIII/sageInterface/sageBuilder.C:11655: 
// SgClassDeclaration* SageBuilder::buildClassDeclaration_nfi(const SgName&, SgClassDeclaration::class_types, SgScopeStatement*, 
//                                                            SgClassDeclaration*, bool, SgTemplateArgumentPtrList*): 
// Assertion `nondefdecl->get_type()->get_declaration() == nondefdecl' failed.

#include "boost/random.hpp"
