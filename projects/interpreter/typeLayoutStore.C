#include <rose.h>

struct TypeLayoutAttribute : AstAttribute
   {
     TypeLayoutAttribute(const StructLayoutInfo &info) : info(info) {}
     StructLayoutInfo info;
   };

const StructLayoutInfo &typeLayout(SgType *t)
   {
     t = t->stripTypedefsAndModifiers();
     TypeLayoutAttribute *tla;
     if (t->attributeExists("TypeLayout"))
        {
          tla = static_cast<TypeLayoutAttribute *>(t->getAttribute("TypeLayout"));
        }
     else
        {
          SystemPrimitiveTypeLayoutGenerator gen1(NULL);
          NonpackedTypeLayoutGenerator gen(&gen1);

          tla = new TypeLayoutAttribute(gen.layoutType(t));
          t->addNewAttribute("TypeLayout", tla);
        }
     return tla->info;
   }
