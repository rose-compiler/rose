#include "ofp_builder.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#undef DEBUG_PRINT
#undef USE_MATCHED_VARIABLE

namespace OFP {

void printLowerCase(FILE * fp, const char * str)
{
   int c;
   while ( (c = *str++) ) {
      fprintf(fp, "%c", tolower(c));
   }
}

/**
 * Returns the ultimate symbol found after traversing the alias table
 */
ATerm ofp_getUltimateSymbol(ATermTable table, ATerm symbol)
{
   ATerm alias = ATtableGet(table, symbol);
   if (alias != NULL) {
      ATerm aname = ofp_getAliasSymbol(alias);
      return ofp_getUltimateSymbol(table, aname);
   }
   return symbol;
}

/**
 * Returns the symbol, or a modified form of the symbol if it
 * is already present.
 */
ATerm ofp_getUniqueSymbol(ATermTable table, ATerm symbol)
{
   int id = 0;
   ATerm sym = symbol;
   ATerm value = ATtableGet(table, symbol);

   if (value) {
      assert(ATmatch(value, "<int>", &id));
      id += 1;

      int len;
      char * symString, * uniqueString;

      assert(ATmatch(symbol, "<str>", &symString));
      assert(id < 1000);    // ensures that new name fits

      len = strlen(symString) + 3;
      uniqueString = (char*) malloc(len);
      sprintf(uniqueString, "%s%d", symString, id);

      sym = ATmake("<str>", uniqueString);

      free(uniqueString);
   }

   ATtablePut(table, symbol, ATmake("<int>", id));

   return sym;   
}

ATbool ofp_isListType(ATerm term)
{
   if (ATmatch(term, "\"List\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isStringType(ATerm term)
{
   if (ATmatch(term, "\"String\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isIdentType(ATerm term)
{
   if (ATmatch(term, "\"Ident\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isOptionType(ATerm term)
{
   if (ATmatch(term, "\"Option\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isConstType(ATerm term)
{
   if (ATmatch(term, "\"Const\"")) {
      return ATtrue;
   }
   return ATfalse;
}

enum DataType ofp_getDataType(ATerm term)
{
   if (ofp_isStringType(term)) return STRING;
   if (ofp_isIdentType (term)) return IDENT;
   if (ofp_isOptionType(term)) return OPTION;
   if (ofp_isConstType (term)) return CONST;

   return UNKNOWN;
}

ATbool ofp_isPrimitiveType(ATerm term)
{
   switch (ofp_getDataType(term)) {
      case STRING:
      case IDENT:
         return ATtrue;
      default:
         return ATfalse;
   }
   return ATfalse;
}

char * ofp_getChars(ATerm term)
{
   char * cptr;
   assert(ATmatch(term, "<str>", &cptr));
   return cptr;
}

ATerm ofp_getArgName(ATerm term)
{
   ATerm name, type;

   assert(ATmatch(term, "Arg(<term>,<term>)", &name, &type));
   return name;
}

ATerm ofp_getArgType(ATerm term)
{
   ATerm name, type;

   assert(ATmatch(term, "Arg(<term>,<term>)", &name, &type));
   return type;
}

ATerm ofp_getAliasSymbol(ATerm term)
{
   ATerm symbol, alias, type;

   assert(ATmatch(term, "Alias(<term>,<term>,<term>)", &symbol, &alias, &type));
   return symbol;
}

ATerm ofp_getAliasName(ATerm term)
{
   ATerm symbol, alias, type;

   assert(ATmatch(term, "Alias(<term>,<term>,<term>)", &symbol, &alias, &type));
   return alias;
}

ATerm ofp_getAliasType(ATerm term)
{
   ATerm symbol, alias, type;

   assert(ATmatch(term, "Alias(<term>,<term>,<term>)", &symbol, &alias, &type));
   return type;
}

ATerm ofp_getProdArgName(ATerm term)
{
   ATerm name;
   if (ATmatch(term, "ConstType(SortNoArgs(<term>))", &name)) {
      return name;
   }
   else if (ATmatch(term, "ConstType(Sort(\"Option\",[SortNoArgs(<term>)]))", &name)) {
      return name;
   }
   else if (ATmatch(term, "ConstType(Sort(\"List\",[SortNoArgs(<term>)]))", &name)) {
      return name;
   }

   return ATmake("<str>", "UNKNOWN");
}

ATerm ofp_getProdArgType(ATerm term)
{
   ATerm name, type;
   if (ATmatch(term, "ConstType(SortNoArgs(<term>))", &name)) {
      return ATmake("<str>", "Const");
   }
   else if (ATmatch(term, "ConstType(Sort(\"Option\",[SortNoArgs(<term>)]))", &name)) {
      return ATmake("<str>", "Option");
   }
   else if (ATmatch(term, "ConstType(Sort(\"List\",[SortNoArgs(<term>)]))", &name)) {
      return ATmake("<str>", "List");
   }

   return ATmake("<str>", "UNKNOWN");
}

ATermList ofp_getArgList(ATermTable aliasTable, ATermTable symTable, ATermList alist)
{
   ATerm name, type;

   ATermList args = (ATermList) ATmake("[]");

   ATermList tail = (ATermList) ATmake("<term>", alist);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getUltimateSymbol(aliasTable, ofp_getProdArgName(head));
      ATerm type = ofp_getProdArgType(head);
      ATerm uniqueName = ofp_getUniqueSymbol(symTable, name);

      if (ATisEqual(name, uniqueName)) {
         args = ATappend(args, ATmake("Arg(<term>,<term>)", name, type));
      }
      tail = ATgetNext(tail);
   }

   return args;
}

ATermList ofp_getArgListNotUnique(ATermTable aliasTable, ATermTable symTable, ATermList alist)
{
   ATerm name, type;

   ATermList args = (ATermList) ATmake("[]");

   ATermList tail = (ATermList) ATmake("<term>", alist);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getUltimateSymbol(aliasTable, ofp_getProdArgName(head));
      ATerm type = ofp_getProdArgType(head);

      tail = ATgetNext(tail);
      args = ATappend(args, ATmake("Arg(<term>,<term>)", name, type));
   }

   return args;
}

ATermList ofp_getArgListUnique(ATermTable aliasTable, ATermTable symTable, ATermList alist)
{
   ATerm name, type;

   ATermList args = (ATermList) ATmake("[]");

   ATermList tail = (ATermList) ATmake("<term>", alist);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getUltimateSymbol(aliasTable, ofp_getProdArgName(head));
      ATerm type = ofp_getProdArgType(head);

      name = ofp_getUniqueSymbol(symTable, name);

      tail = ATgetNext(tail);
      args = ATappend(args, ATmake("Arg(<term>,<term>)", name, type));
   }

   return args;
}

ATbool ofp_traverse_Signature(ATerm term, pOFP_Traverse Signature)
{
#ifdef DEBUG_PRINT
   printf("\nSignature: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse Constructors_list;
   if (ATmatch(term, "Signature(<term>)", &Constructors_list.term) ) {

      ATermList Constructors_tail = (ATermList) ATmake("<term>", Constructors_list.term);
      while (! ATisEmpty(Constructors_tail)) {
         OFP_Traverse Constructors;
         Constructors.term = ATgetFirst(Constructors_tail);
         Constructors_tail = ATgetNext(Constructors_tail);

         if (ofp_traverse_Constructors(Constructors.term, &Constructors)) {
            // MATCHED Constructors
         } else return ATfalse;
      }
      return ATtrue;
   }

   return ATfalse;
}

ATbool traverse_Module(ATerm term, pOFP_Traverse Module)
{
#ifdef DEBUG_PRINT
   printf("Module: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse Name, Signature_list;
   if (ATmatch(term, "Module(<term>,<term>)", &Name.term, &Signature_list.term) ) {

      char * String;
      if (ATmatch(Name.term, "<str>", &String)) {
         // MATCHED module name
      }

      // Only traverse one signature, the others cause problems
      //
      ATermList Signature_tail = (ATermList) ATmake("<term>", Signature_list.term);
      if (! ATisEmpty(Signature_tail)) {
         OFP_Traverse Signature;
         Signature.term = ATgetFirst(Signature_tail);
         Signature_tail = ATgetNext(Signature_tail);

         if (ofp_traverse_Signature(Signature.term, &Signature)) {
            // MATCHED Signature
         } else return ATfalse;
      }
      return ATtrue;
   }

   return ATfalse;
}

ATermList ofp_coalesceProdTable(ATermList oldTable)
{
   // Assumes:
   //  1. Contains list of terms Symbol(<str>,Prod(<str>,<list>)))
   //      a. first <str> is the symbol name
   //      b. second <str> is a production name
   //      b. <list> is [production symbols] of length 1
   //  2. Portions of table to be coalesced are in order
   //
   ATerm head;

   ATermList table = (ATermList) ATmake("[]");
   ATermList tail  = (ATermList) ATmake("<term>", oldTable);

   head = ATgetFirst(tail);
   tail = ATgetNext(tail);

   while (1) {
      ATerm headName, headProd, next, nextName, nextProd, symbol;
      ATermList plist = (ATermList) ATmake("[]");

      if (ATisEmpty(tail)) next = ATmake("Symbol(None,None)");
      else                 next = ATgetFirst(tail);

      assert( ATmatch(head, "Symbol(<term>,<term>)", &headName, &headProd) );
      assert( ATmatch(next, "Symbol(<term>,<term>)", &nextName, &nextProd) );

      plist = ATappend(plist, headProd);

      // coalesce multiple productions of same name
      while (ATisEqual(headName, nextName)) {
         head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (ATisEmpty(tail)) next = ATmake("Symbol(None,None)");
         else                 next = ATgetFirst(tail);

         assert( ATmatch(head, "Symbol(<term>,<term>)", &headName, &headProd) );
         assert( ATmatch(next, "Symbol(<term>,<term>)", &nextName, &nextProd) );

         plist = ATappend(plist, headProd);
      }

      symbol = ATmake("Symbol(<term>,<term>)", headName, plist);
      table = ATappend(table, symbol);

      if (ATisEmpty(tail)) break;

      head = ATgetFirst(tail);
      tail = ATgetNext(tail);
   }

   return table;
}

/**
 * Build table of productions.  Multiple productions may be bound to a
 * production name.  Thus the table should be coalesced after building it.
 * An entry in the table is Symbol(symName, [Prod(consName, [symType])])
 */
ATermList ofp_build_production_table(ATerm term)
{
   ATerm OpDecl, symName, consName, symType, symbol, prod;
   ATermList ptable, empty, plist;

   ptable = (ATermList) ATmake("[]");
   empty  = (ATermList) ATmake("[]");

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      OpDecl = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (! ATmatch(OpDecl, "OpDecl(<term>,<term>)", &consName, &symType) ) {
         continue;
      }

      if (ATmatch(symType, "ConstType(SortNoArgs(<term>))", &symName) ) {
         prod   = ATmake("Prod(<term>, <term>)", consName, empty);
         symbol = ATmake("Symbol(<term>,<term>)", symName, prod);
      }
      else if (ATmatch(symType, "FunType(<term>, ConstType(SortNoArgs(<term>)))", &plist, &symName) ) {
         prod   = ATmake("Prod(<term>, <term>)", consName, plist);
         symbol = ATmake("Symbol(<term>,<term>)", symName, prod);
      }
      else {
         printf("???????????????????? %s\n", ATwriteToString(symName));
         continue;
      }

      ptable = ATappend(ptable, symbol);
   }

   /* Coalesce the table of productions so there is one production per symbol
    */
   ptable = ofp_coalesceProdTable(ptable);

   return ptable;
}

ATermList ofp_coalesceAliasTable(ATermList oldTable)
{
   // Assumes:
   //  1. Contains list of terms Type(<str>,<list>) or OptType(<str>,<list>)
   //      a. <str> is type name
   //      b. <list> is [type] of length 1
   //  2. Portions of table to be coalesced are in order
   //  3. If OptType must match "(Some(<term>))"
   //
   ATerm head;

   ATermList table = (ATermList) ATmake("[]");
   ATermList tail  = (ATermList) ATmake("<term>", oldTable);

   head = ATgetFirst(tail);
   tail = ATgetNext(tail);

   while (1) {
      ATerm headSymbol, headAlias, headType, next, nextSymbol, nextAlias, nextType;

      if (ATisEmpty(tail)) next = ATmake("Alias(None,None,None)");
      else                 next = ATgetFirst(tail);

      assert( ATmatch(head, "Alias(<term>,<term>,<term>)", &headSymbol, &headAlias, &headType) );
      assert( ATmatch(next, "Alias(<term>,<term>,<term>)", &nextSymbol, &nextAlias, &nextType) );

      // throw away multiple productions of same alias
      if (ATisEqual(headAlias, nextAlias)) {
         while (ATisEqual(headAlias, nextAlias)) {
            // skip these two
            if (ATisEmpty(tail)) return table;

            tail = ATgetNext(tail);
            head = ATgetFirst(tail);
            tail = ATgetNext(tail);

            if (ATisEmpty(tail)) next = ATmake("Alias(None,None,None)");
            else                 next = ATgetFirst(tail);

            assert( ATmatch(head, "Alias(<term>,<term>,<term>)", &headSymbol, &headAlias, &headType) );
            assert( ATmatch(next, "Alias(<term>,<term>,<term>)", &nextSymbol, &nextAlias, &nextType) );
         }
      }
      else if (!ofp_isPrimitiveType(headSymbol)) {
         // skip primitive types
         table = ATappend(table, head);
      }

      if (ATisEmpty(tail)) break;

      head = ATgetFirst(tail);
      tail = ATgetNext(tail);
   }

   return table;
}

ATbool ofp_build_traversal_class_decl(FILE * fp, ATerm name)
{
   char * nameStr;

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   /** write to header file
    */
   fprintf(fp, "   class %s;\n", nameStr);

   return ATtrue;
}

ATbool ofp_build_traversal_class_decls(FILE * fp, ATermList prodTable)
{
   ATerm term, symbol;
   ATermList productions, tail;

   tail = (ATermList) ATmake("<term>", prodTable);
   while (! ATisEmpty(tail)) {
      term = ATgetFirst(tail);
      tail = ATgetNext (tail);
      if (ATmatch(term, "Symbol(<term>,<term>)", &symbol, &productions)) {
         ofp_build_traversal_class_decl(fp, symbol);
      }
   }
   fprintf(fp, "\n");

   return ATtrue;
}

ATbool ofp_build_class_vars_decl(FILE * fp, ATermList vars)
{
   ATermList tail = (ATermList) ATmake("<term>", vars);

   if (ATisEmpty(tail)) {
      /* no vars to declare */
      return ATtrue;
   }

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getArgName(head);
      ATerm type = ofp_getArgType(head);
      tail = ATgetNext(tail);

      if (ofp_isListType(type)) {
         fprintf(fp, "    std::vector<%s*>* p%sList;\n", ofp_getChars(name), ofp_getChars(name));
      }
      else {
         fprintf(fp, "    %s* p%s;\n", ofp_getChars(name), ofp_getChars(name));
      }
   }

   return ATtrue;
}

ATbool ofp_build_class_cons(FILE * fp, ATerm name, ATermList vars)
{
   char * nameStr;
   ATermList tail = (ATermList) ATmake("<term>", vars);

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   fprintf(fp, "    %s()\n", nameStr);
   fprintf(fp, "      {\n");

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getArgName(head);
      ATerm type = ofp_getArgType(head);
      tail = ATgetNext(tail);
      if (ofp_isListType(type)) {
         fprintf(fp, "         p%sList = new std::vector<%s*>();\n", ofp_getChars(name), ofp_getChars(name));
      }
      else {
         fprintf(fp, "         p%s = NULL;\n", ofp_getChars(name));
      }
   }

   fprintf(fp, "      }\n");

   return ATtrue;
}

ATbool ofp_build_class_enum(FILE * fp, ATermList enums)
{
   ATermList tail = (ATermList) ATmake("<term>", enums);

   if (ATisEmpty(tail)) {
      /* no enums to create */
      return ATtrue;
   } else if (ATgetLength(tail) < 2) {
      /* if length is 1 then no OR clauses so enum not needed (maybe check name as well?) */
      return ATtrue;
   }

   fprintf(fp, "   enum OptionType\n");
   fprintf(fp, "     {\n");
   fprintf(fp, "        DEFAULT = 0");

   while (! ATisEmpty(tail)) {
      char * name;
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (! ATmatch(head, "<str>", &name)) {
         return ATfalse;
      }
      fprintf(fp, ",\n        %s", name);
   }
   fprintf(fp, "\n     };\n\n");

   return ATtrue;
}

ATbool ofp_build_class_new(FILE * fp, ATerm name, ATermList vars)
{
   char * nameStr;
   ATermList tail = (ATermList) ATmake("<term>", vars);

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   fprintf(fp, "    %s* new%s()\n", nameStr, nameStr);
   fprintf(fp, "      {\n");
   fprintf(fp, "         %s* node = new %s();\n", nameStr, nameStr);

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getArgName(head);
      ATerm type = ofp_getArgType(head);
      nameStr = ofp_getChars(name);
      tail = ATgetNext(tail);
      if (ofp_isListType(type)) {
         fprintf(fp, "         delete node->p%sList; node->p%sList = p%sList;  p%sList = NULL;\n", nameStr, nameStr, nameStr, nameStr);
      }
      else {
         fprintf(fp, "         node->p%s = p%s;  p%s = NULL;\n", nameStr, nameStr, nameStr);
      }
   }

   fprintf(fp, "         node->setOptionType(optionType);\n");
   fprintf(fp, "         node->inheritPayload(this);\n");
   fprintf(fp, "         return node;\n");
   fprintf(fp, "      }\n\n");

   return ATtrue;
}

ATbool ofp_build_class_get(FILE * fp, ATermList vars)
{
   ATermList tail = (ATermList) ATmake("<term>", vars);

   if (ATisEmpty(tail)) {
      /* no vars to get */
      return ATtrue;
   }

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getArgName(head);
      ATerm type = ofp_getArgType(head);
      char * nameStr = ofp_getChars(name);
      tail = ATgetNext(tail);
      if (ofp_isListType(type)) {
         fprintf(fp, "    std::vector<%s*>* get%sList() {return p%sList;}\n", nameStr, nameStr, nameStr);
      }
      else {
         fprintf(fp, "    %s* get%s() {return p%s;}\n", nameStr, nameStr, nameStr);
      }
   }
   fprintf(fp, "\n");

   return ATtrue;
}

ATbool ofp_build_class_set(FILE * fp, ATermList vars)
{
   ATermList tail = (ATermList) ATmake("<term>", vars);

   if (ATisEmpty(tail)) {
      /* no vars to set */
      return ATtrue;
   }

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getArgName(head);
      ATerm type = ofp_getArgType(head);
      char * nameStr = ofp_getChars(name);
      tail = ATgetNext(tail);
      if (ofp_isListType(type)) {
         fprintf(fp, "    void append%s(%s* ", nameStr, nameStr);
         printLowerCase(fp, nameStr); 
         fprintf(fp, ") {p%sList->push_back(", nameStr);
         printLowerCase(fp, nameStr);
         fprintf(fp, ");}\n");
      }
      else {
         fprintf(fp, "    void set%s(%s* ", nameStr, nameStr);
         printLowerCase(fp, nameStr); 
         fprintf(fp, ") {p%s = ", nameStr);
         printLowerCase(fp, nameStr);
         fprintf(fp, ";}\n");
      }
   }
   fprintf(fp, "\n");

   return ATtrue;
}

ATbool ofp_build_traversal_class_def(FILE * fp, ATerm name, ATermList cons, ATermList vars)
{
   char * nameStr;

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   fprintf(fp, "class %s : public Node\n", nameStr);
   fprintf(fp, "{\n");
   fprintf(fp, " public:\n");

   ofp_build_class_cons(fp, name, vars);

   fprintf(fp, "   ~%s();\n\n", nameStr);

   ofp_build_class_enum(fp, cons);
   ofp_build_class_new(fp, name, vars);
   ofp_build_class_get(fp, vars);
   ofp_build_class_set(fp, vars);

   fprintf(fp, " private:\n");

   ofp_build_class_vars_decl(fp, vars);

   fprintf(fp, "};\n\n");

   return ATtrue;
}

ATbool ofp_build_traversal_class_destructor(FILE * fp, ATerm name, ATermList cons, ATermList vars)
{
   char * nameStr;
   ATermList tail = (ATermList) ATmake("<term>", vars);

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   fprintf(fp, "OFP::%s::~%s()\n", nameStr, nameStr);
   fprintf(fp, "   {\n");

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      ATerm name = ofp_getArgName(head);
      ATerm type = ofp_getArgType(head);
      nameStr = ofp_getChars(name);
      tail = ATgetNext(tail);
      if (ofp_isListType(type)) {
         fprintf(fp, "      if (p%sList) delete p%sList;\n", nameStr, nameStr);
      }
      else {
         fprintf(fp, "      if (p%s) delete p%s;\n", nameStr, nameStr);
      }
   }
   fprintf(fp, "   }\n\n");

   return ATtrue;
}

ATbool ofp_build_traversal_func_header(FILE * fp, ATerm name)
{
   char * nameStr;

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   /** write to header file
    */
   fprintf(fp, "ATbool ofp_traverse_%s(ATerm term, OFP::%s * %s);\n", nameStr, nameStr, nameStr);

   return ATtrue;
}

ATbool ofp_build_traversal_func_begin(FILE * fp, ATerm name)
{
   char * nameStr;
   const char * percs = "%s";

   if (! ATmatch(name, "<str>", &nameStr)) {
      return ATfalse;
   }

   /** write to implementation file
    */
   fprintf(fp, "ATbool ofp_traverse_%s(ATerm term, OFP::%s* %s)\n", nameStr, nameStr, nameStr);
   fprintf(fp, "{\n");

   /** Debugging output
    */
   fprintf(fp, "#ifdef DEBUG_PRINT\n");
   fprintf(fp, "   printf(\"%s: %s\\n\", ATwriteToString(term));\n", nameStr, percs);
   fprintf(fp, "#endif\n\n");

#ifdef USE_MATCHED_VARIABLE
   /** Return condition
    */
   fprintf(fp, " ATbool matched = ATfalse;\n\n");
#endif

   return ATtrue;
}

ATbool ofp_build_traversal_func_end(FILE * fp, ATerm name, ATbool returnTrue)
{
   if (returnTrue) fprintf(fp, " return ATtrue;\n");
   else            fprintf(fp, " return ATfalse;\n");
   fprintf(fp, "}\n\n");
   return ATtrue;
}

ATbool ofp_build_match_terminal(FILE * fp, ATerm terminal)
{
   char * name;
   if (! ATmatch(terminal, "<str>", &name)) {
      return ATfalse;
   }

   /** Match the terminal name
    */
   fprintf(fp, "   if (ATmatch(term, \"%s\")) {\n", name);
   fprintf(fp, "      // MATCHED %s\n", name);
   fprintf(fp, "      return ATtrue;\n");
   fprintf(fp, "   }\n\n");

   return ATtrue;
}

ATbool ofp_build_match_args_decl(FILE * fp, ATermList args, ATermList uniqueArgs)
{
   ATermList tail  = (ATermList) ATmake("<term>", args);
   ATermList utail = (ATermList) ATmake("<term>", uniqueArgs);

   if (ATisEmpty(tail)) {
      /* no args to declare */
      return ATtrue;
   }

   while (! ATisEmpty(tail)) {
      ATerm head  = ATgetFirst(tail);
      ATerm uhead = ATgetFirst(utail);
      ATerm name  = ofp_getArgName(head);
      ATerm uname = ofp_getArgName(uhead);
      tail  = ATgetNext(tail);
      utail = ATgetNext(utail);
      fprintf(fp, " OFP::%s %s;\n", ofp_getChars(name), ofp_getChars(uname));
   }

   return ATtrue;
}

ATbool ofp_build_match_begin(FILE * fp, ATerm symbol, ATerm constructor, ATermList args, ATermList uniqueArgs)
{
   int i, len;
   ATerm cons, prod, arglist;
   const char * comma = "";

   len = ATgetLength(args);
   if (len == 0) {
      fprintf(fp, " if (ATmatch(term, \"%s\")) {\n", ofp_getChars(constructor));
      return ATtrue;
   }

   ofp_build_match_args_decl(fp, args, uniqueArgs);

   fprintf(fp, " if (ATmatch(term, \"%s(", ofp_getChars(constructor));
   for (i = 0; i < len; i++) {
      fprintf(fp, "%s<term>", comma);
      comma = ",";
   }
   fprintf(fp, ")\"");
   for (i = 0; i < len; i++) {
      ATerm name = ofp_getArgName(ATelementAt(args, i));
      fprintf(fp, ", &%s.term", ofp_getChars(name));
   }
   fprintf(fp, ")) {\n");

   return ATtrue;
}

ATbool ofp_build_match_end(FILE * fp, ATerm symbol, ATerm constructor)
{
   if (ATisEqual(symbol, constructor)) {
#ifdef USE_MATCHED_VARIABLE
      fprintf(fp, "\n   if (matched) return ATtrue;\n");
#else
      fprintf(fp, "\n   return ATtrue;\n");
#endif
   }
   else {
      fprintf(fp, "\n   // MATCHED %s\n", ofp_getChars(constructor));
      fprintf(fp, "   %s->setOptionType(OFP::%s::%s);\n", ofp_getChars(symbol), ofp_getChars(symbol), ofp_getChars(constructor));
      fprintf(fp, "\n   return ATtrue;\n");
   }
   fprintf(fp, " }\n\n");

   return ATtrue;
}

ATbool ofp_build_match_all_begin(FILE * fp, ATermTable aliasTable, ATerm symbol, ATerm unique, ATerm type, ATbool aliased)
{
   char * name;

   switch (ofp_getDataType(type)) {
      case OPTION:
         ofp_build_match_sort_option_begin(fp, unique, aliased);
         break;
      case CONST:
         name = ofp_getChars(unique);
         fprintf(fp, "   if (ATmatch(%s.term, \"(<term>)\", &%s.term)) {\n", name, name);
         break;
      default:
         printf("ERROR: ofp_build_match_all_begin: symbol type not handled: %s\n", ATwriteToString(type));
         break;
   }

   /* Recurs if an alias exists */
   ATerm alias = ATtableGet(aliasTable, symbol);
   if (alias != NULL) {
      ATerm aname   = ofp_getAliasName(alias);
      ATerm asymbol = ofp_getAliasSymbol(alias);
      type = ofp_getAliasType(alias);
      printf("------ recursion: %s\n", ATwriteToString(alias));
      printf("------     alias: %s\n", ATwriteToString(asymbol));
      printf("------    unique: %s\n", ATwriteToString(unique));
      printf("------      type: %s\n", ATwriteToString(type));

      ofp_build_match_all_begin(fp, aliasTable, asymbol, unique, type, ATtrue);
   }

   return ATtrue;
}

ATbool ofp_build_match_all_end(FILE * fp, ATermTable aliasTable, ATerm symbol, ATerm unique, ATerm type)
{
   fprintf(fp, "   }\n");

   /* Recurs if an alias exists */
   ATerm alias = ATtableGet(aliasTable, symbol);
   if (alias != NULL) {
      ATerm asymbol = ofp_getAliasSymbol(alias);
      printf("++++++     alias: %s\n", ATwriteToString(asymbol));
      ofp_build_match_all_end(fp, aliasTable, asymbol, unique, type);
   }

   return ATtrue;

}

ATbool ofp_build_match_sort_option_begin(FILE * fp, ATerm symbol, ATbool aliased)
{
   char * name = ofp_getChars(symbol);
   if (aliased) {
      fprintf(fp, "   if (ATmatch(%s.term, \"(Some(<term>))\", &%s.term)) {\n", name, name);
   }
   else {
      fprintf(fp, "   if (ATmatch(%s.term, \"Some(<term>)\", &%s.term)) {\n", name, name);
   }
   return ATtrue;
}

ATbool ofp_build_match_sort_option_end(FILE * fp, ATerm symbol)
{
   fprintf(fp, "   }\n");
   return ATtrue;
}

ATbool ofp_build_match_nonterminal_begin(FILE * fp, ATerm constructor, ATerm symbol)
{
   char * cons_name, * sym_name;
   if (! ATmatch(constructor, "<str>", &cons_name)) return ATfalse;
   if (! ATmatch(symbol,      "<str>", &sym_name )) return ATfalse;

   /** Match into the constructor name traversal struct
    */
   fprintf(fp, "   OFP_Traverse %s;\n", sym_name);
   fprintf(fp, "   if (ATmatch(term, \"%s(<term>)\", &%s.term)) {\n", cons_name, sym_name);

   return ATtrue;
}

ATbool ofp_build_match_nonterminal_end(FILE * fp, ATerm constructor, ATerm symbol)
{
   fprintf(fp, "   }\n\n");
   return ATtrue;
}

ATbool ofp_build_traversal_nonterminal(FILE * fp, ATerm symbol, ATerm prod_symbol, ATerm unique_sym)
{
   char * sym_name, * prod_name, * unique;
   assert(ATmatch(symbol, "<str>", &sym_name));
   assert(ATmatch(prod_symbol, "<str>", &prod_name));
   assert(ATmatch(unique_sym,  "<str>", &unique   ));

   fprintf(fp, "      if (ofp_traverse_%s(%s.term, &%s)) {\n", prod_name, unique, unique);
   fprintf(fp, "         // MATCHED %s\n", prod_name);
   fprintf(fp, "         %s->set%s(%s.new%s());\n", sym_name, prod_name, prod_name, prod_name);
   fprintf(fp, "         %s->inheritPayload(%s->get%s());\n", sym_name, sym_name, prod_name);
#ifdef USE_MATCHED_VARIABLE
   fprintf(fp, "         matched = ATtrue;\n");
#endif
   fprintf(fp, "      } else return ATfalse;\n");

   return ATtrue;
}

ATbool ofp_build_traversal_production(FILE * fp, ATermTable aliasTable,
                                      ATerm symbol, ATerm constructor, ATermList prod_symbols, ATermList args)
{
   int i, len;

   len = ATgetLength(prod_symbols);
   assert(len == ATgetLength(args));

   //   ATermList tail = (ATermList) ATmake("<term>", prod_symbols);
   //   while (! ATisEmpty(tail)) {
   for (i = 0; i < len; i++) {
      //ATerm head, prod_symbol;
      ATerm prod_symbol;

      ATerm head = ATelementAt(prod_symbols, i);
      ATerm unique = ofp_getArgName(ATelementAt(args, i));

      //      head = ATgetFirst(tail);
      //      tail = ATgetNext(tail);

      if (! ATmatch(head, "ConstType(<term>)", &head)) {
         return ATfalse;
      }

      if (ATmatch(head, "Sort(\"Option\", [SortNoArgs(<term>)])", &prod_symbol)) {
         ATerm ultimate = ofp_getUltimateSymbol (aliasTable, prod_symbol);
         ATerm alias    = ATtableGet            (aliasTable, prod_symbol);
         //ATerm unique   = ofp_getUniqueSymbol   (gSymTable,   ultimate);

                      printf(".....     cons  : %s\n", ATwriteToString(constructor));
                      printf(".....     symbol: %s\n", ATwriteToString(symbol));
                      printf(".....sort symbol: %s\n", ATwriteToString(prod_symbol));
                      printf(".....ulti symbol: %s\n", ATwriteToString(ultimate));
                      printf(".....uniq symbol: %s\n", ATwriteToString(unique));
   if (alias != NULL) printf(".....     alias : %s\n", ATwriteToString(alias));

         //ofp_build_match_sort_option_begin(unique);
         fprintf(fp, "\n");  // pretty printing
         ofp_build_match_all_begin(fp, aliasTable, prod_symbol, unique, ATmake("<str>", "Option"), ATfalse);
         ofp_build_traversal_nonterminal(fp, symbol, ultimate, unique);
         //ofp_build_match_sort_option_end(unique);
         ofp_build_match_all_end(fp, aliasTable, prod_symbol, unique, ATmake("<str>", "Option"));
      }
      else if (ATmatch(head, "Sort(\"List\", [SortNoArgs(<term>)])", &prod_symbol)) {
         fprintf(fp, "\n");
         ofp_build_traversal_list(fp, symbol, prod_symbol, unique);
      }
      else if (ATmatch(head, "SortNoArgs(<term>)", &prod_symbol)) {
         fprintf(fp, "\n");  // to make spacing same as sort match
         ofp_build_traversal_nonterminal(fp, symbol, prod_symbol, unique);
      }
      else {
         printf("ofp_build_traversal_production: need to match something else: %s\n",
                 ATwriteToString(head));
         return ATfalse;
      }
   }

   return ATtrue;
}

ATbool ofp_build_traversal_list(FILE * fp, ATerm symbol, ATerm prod_symbol, ATerm unique_symbol)
{
   char * sym_name, * prod_name;
   assert(ATmatch(symbol, "<str>", &sym_name));
   assert(ATmatch(prod_symbol, "<str>", &prod_name));

   /** Traverse the list
    */
   fprintf(fp, "   ATermList %s_tail = (ATermList) ATmake(\"<term>\", %s.term);\n", prod_name, prod_name);
#ifdef USE_MATCHED_VARIABLE
   fprintf(fp, "   if (ATisEmpty(%s_tail)) matched = ATtrue;\n", prod_name);
#endif
   fprintf(fp, "   while (! ATisEmpty(%s_tail)) {\n", prod_name);
   fprintf(fp, "      %s.term = ATgetFirst(%s_tail);\n", prod_name, prod_name);
   fprintf(fp, "      %s_tail = ATgetNext (%s_tail);\n", prod_name, prod_name);
   fprintf(fp, "      if (ofp_traverse_%s(%s.term, &%s)) {\n", prod_name, prod_name, prod_name);
   fprintf(fp, "         // MATCHED %s\n", prod_name);
   fprintf(fp, "         %s->append%s(%s.new%s());\n", sym_name, prod_name, prod_name, prod_name);
#ifdef USE_MATCHED_VARIABLE
   fprintf(fp, "         matched = ATtrue;\n", prod_name);
#endif
   fprintf(fp, "      } else return ATfalse;\n");
   fprintf(fp, "   }\n");

   return ATtrue;
}

} // namespace OFP
