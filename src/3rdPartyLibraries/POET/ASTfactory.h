/*
   POET : Parameterized Optimizations for Empirical Tuning
   Copyright (c)  2008,  Qing Yi.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of UTSA nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef AST_FACTORY_H
#define AST_FACTORY_H

#include <error_config.h>
#include <poetAST.h>
#include <iostream>

extern int user_debug;

class MultiFactory 
{
  typedef std:: map<POETCode*, POETCode*> LastMap;
  LastMap lastMap;
  typedef std:: map<POETCode*, LastMap*> FirstMap;
  FirstMap firstMap;
  virtual POETCode* new_code(POETCode* first, POETCode* rest) = 0;
 public:
  POETCode* new_item(POETCode* first, POETCode* rest) {
     LastMap* m = 0;
     FirstMap::const_iterator p = firstMap.find(first);
     if (p == firstMap.end()) {
        firstMap[first] = m = new LastMap();
     }
     else
       m = (*p).second;
     LastMap::const_iterator p2 = m->find(rest);
     if (p2 == m->end()) {
        return (*m)[rest] = new_code(first, rest);
     }
     else
        return (*p2).second;
   }
};

class CvarTable : public MultiFactory {
    CvarSymbolTable::Entry entry; 
     virtual POETCode* new_code(POETCode* first, POETCode* rest) 
        { 
          CodeVar* res = new CodeVar(entry, first,rest); 
          return res;
        }
   public:
     CvarTable(const CvarSymbolTable::Entry& e) : entry(e) {}
};

class TupleTable : public MultiFactory {
     virtual POETCode* new_code(POETCode* first, POETCode* last) 
     { 
        POETTuple* r1= dynamic_cast<POETTuple*>(first);
        assert(r1 != 0); 
        int r1size = r1->size();
        POETTuple* result = new POETTuple(r1size + 1);
        for (int i = 0; i < r1size; ++i) {
            result->impl[i] =r1->get_entry(i);
        } 
        result->impl[r1size]=last; 
        return result;
     }
};

class TupleFactory 
  {
    std::map<POETCode*, POETTuple*> singles;
    TupleTable vectors;
    POETTuple emptyVec;
   public:
    POETTuple* new_empty() { return &emptyVec; }
    POETTuple* new_single(POETCode* item)
    {
      assert(item != 0);
        std::map<POETCode*,POETTuple*>::const_iterator p = singles.find(item);
        if (p != singles.end()) 
           return (*p).second;
        POETTuple*  result =  new POETTuple(1);
        result->impl[0] = item;
        singles[item] = result;
        return result;
    }
    POETTuple* new_tuple(POETTuple* prev, POETCode* item)
    {
     assert(prev != 0 && item != 0);
     return static_cast<POETTuple*>(vectors.new_item(prev, item));
    }
};

class ListFactory  : public MultiFactory
  {
    virtual POETCode* new_code(POETCode* first, POETCode* rest) 
     { 
        POETList* restlist = dynamic_cast<POETList*>(rest);
        if (rest != 0 && restlist==0)
             restlist = new_item(rest,0);
        if (first == 0) return restlist;
        return  new POETList(first,restlist); 
      }
   public:
    POETList* new_item(POETCode* first, POETCode* rest) 
     { 
       return static_cast<POETList*>(MultiFactory::new_item(first,rest)); 
     }
};

template <POETOperatorType op> 
class BopFactory  : public MultiFactory
  {
    virtual POETCode* new_code(POETCode* first, POETCode* second) 
     { 
        POETBop * res=  new POETBop(op, first,second); 
        return res; 
      }
   public:
    POETBop* new_item(POETCode* first, POETCode* second) 
     { return static_cast<POETBop*>(MultiFactory::new_item(first,second)); }
};
class ASTFactory {
  typedef std:: map<int, POETIconst*> IconstMap;
  typedef std:: map<POETCode*, CvarTable*> CvarNameMap;
  typedef std:: map<POETCode*, XformVar*> XvarMap;
  typedef std:: map<std::string, POETString*> StringMap;
  typedef std:: map<POETOperatorType, POETOperator*> OperatorMap;
  typedef std:: map<POETCode*, POETTypeNot*> TypeNotMap;
  typedef std:: map<POETCode*, POETTypeUop*> TypeUopMap;
  XvarSymbolTable xformDef;
  CvarSymbolTable codeDef;
  IconstMap iconstMap;
  CvarNameMap cvarMap;
  XvarMap xvarMap;
  StringMap stringConstMap;
  OperatorMap opMap;
  ListFactory listMaker;
  BopFactory<POET_OP_MAP> MAPMaker;
  TypeUopMap listMap,list1Map;
  TypeNotMap notMap;
  TupleFactory tupleMaker;

  static POETString* emptyString;
  static POETType* intType, *stringType, *lvarAny, *idType;

  static ASTFactory* _inst;

   CvarTable* get_cvarTable(const CvarSymbolTable::Entry& e)
   {
     CvarTable* m = 0;
     POETCode* name = e.get_name();
     CvarNameMap::const_iterator p = cvarMap.find(name);
     if (p == cvarMap.end()) {
        cvarMap[name] = m = new CvarTable(e);
     }
     else m = (*p).second;
     return m;
   }
 public:
  static ASTFactory* inst() {
     if (_inst == 0)
        _inst = new ASTFactory();
     return _inst;
  }
  static POETString* new_empty() { return emptyString; }
  static POETType* make_any()  { return lvarAny; }
  static POETType* new_type(POETTypeEnum t) 
      { switch(t) 
        { case TYPE_INT: return intType;  
          case TYPE_ANY: return lvarAny;
          case TYPE_STRING: return stringType;
          case TYPE_ID: return idType;
          default: std::cerr << "unrecognized type: " << t << "\n"; assert(0);
        }
      } 
  POETTypeNot* new_typeNot(POETCode* arg)
     { 
        TypeNotMap::const_iterator p = notMap.find(arg);
        if (p == notMap.end()) 
                 return notMap[arg] = new POETTypeNot(arg);
        return (*p).second;
     }
  POETTypeUop* new_typeUop(POETOperatorType t, POETCode* arg)
      { 
       TypeUopMap* addr = 0;
       switch(t) {
          case TYPE_LIST: addr = &listMap; break;
          case TYPE_LIST1: addr = &list1Map; break;
          default: std::cerr << "unrecognized type: " << t << "\n"; assert(0);
        }
        TypeUopMap::const_iterator p = addr->find(arg);
        if (p == addr->end()) 
                 return (*addr)[arg] = new POETTypeUop(t,arg);
        return (*p).second;
      } 
     
  POETString* new_string(const std::string& entry) {
     if (entry == "") return new_empty();
     StringMap::const_iterator p = stringConstMap.find(entry);
     if (p == stringConstMap.end()) {
       return stringConstMap[entry] = new POETString(entry);
     }
     return (*p).second;
   }

  POETList* new_list(POETCode* first, POETCode* rest) 
   { 
      return listMaker.new_item(first,rest); 
   }

  POETOperator* new_op(POETOperatorType value) {
     OperatorMap::const_iterator p = opMap.find(value);
     if (p == opMap.end()) {
       return opMap[value] = new POETOperator(value);
     }
     return (*p).second;
  }
  POETIconst* new_iconst(int value) {
     IconstMap::const_iterator p = iconstMap.find(value);
     if (p == iconstMap.end()) {
       return iconstMap[value] = new POETIconst(value);
     }
     return (*p).second;
   }
  bool find_codeDef(POETCode* name, CvarSymbolTable::Entry& ge)
  {
    ge = codeDef.find(name);
    return ge != codeDef.end(); 
  }
  CvarSymbolTable::Entry insert_codeDef(POETCode* name) 
      { 
        CvarSymbolTable::Entry r = codeDef.find(name);
        if (r == codeDef.end())
           return codeDef.insert(name); 
         return r;
      }
   /* QY: do not evaluate attributes of code ref */
   CodeVar* new_codeRef(const CvarSymbolTable::Entry e, POETCode* args)
     {
       CvarTable* m = get_cvarTable(e);
       return static_cast<CodeVar*>(m->new_item(args, 0));
     }
   /* QY: evaluate attributes of code ref */
   CodeVar* build_codeRef(const CvarSymbolTable::Entry e, POETCode* args);
   /* QY: copy attributes of r1Code */
   CodeVar* build_codeRef(CodeVar* r1Code, POETCode* r2, bool overwrite)
        {
         POETCode* arg = r1Code->get_args();
         if (overwrite || arg == 0) {
            CvarSymbolTable::Entry e = r1Code->get_entry();
            CvarTable* m = get_cvarTable(e);
            return static_cast<CodeVar*>(m->new_item(r2, r1Code->get_attr()));
         }
         else 
            CVAR_MISMATCH(r1Code, r1Code->get_entry().get_param(), r2);
       }


  XformVar* find_xformDef(POETCode* name, bool insertIfNotFound)
  {
      XformVar* res = 0;
      XvarMap::const_iterator pXvar = xvarMap.find(name);
      if (pXvar == xvarMap.end()) {
         if (insertIfNotFound) {
            XvarSymbolTable::Entry r = xformDef.find(name);
            if (r == xformDef.end())
               r = xformDef.insert(name); 
            res = new XformVar(r);
            xvarMap.insert(std::pair<POETCode*, XformVar*>(name,res));
         }
      }
      else res = (*pXvar).second;
      return res;
    }
  LocalVar* make_attrAccess(POETCode* scope, POETCode* name2)
  {
     LvarSymbolTable* tab = 0;
     switch (scope->get_enum()) {
     case SRC_XVAR: 
           tab = dynamic_cast<XformVar*>(scope)->get_entry().get_symTable();
           break;
     case SRC_CVAR: 
           tab = dynamic_cast<CodeVar*>(scope)->get_entry().get_symTable();
           break;
     default:
            INCORRECT_XFORM_CODE(scope->get_className()+":"+scope->toString());
     } 
     if (tab == 0)
        SYM_UNDEFINED(scope->toString(OUTPUT_NO_DEBUG)+"."+name2->toString(OUTPUT_NO_DEBUG));
     LocalVar* res = tab->find(name2);
     if (res == 0) {
        SYM_UNDEFINED(scope->toString(OUTPUT_NO_DEBUG)+"."+name2->toString(OUTPUT_NO_DEBUG));
     }
     return res;
  }
  POETCode* SetTrace(POETCode* var, POETCode* val) {
      if (var != val && var != 0 && var->get_enum() == SRC_LVAR && 
          static_cast<LocalVar*>(var)->get_entry().get_entry_type() == LVAR_TRACE) 
         static_cast<LocalVar*>(var)->get_entry().set_code(val);
      return var;
   }
  XformVar* make_xvar( XformVar* v, POETCode* config, POETCode* args) {
       if (args == v->get_args()) return v;
       XformVarInvoke* tmp = new XformVarInvoke(v->get_entry(), config, args);
       return tmp;
   }

    POETTuple* append_tuple(POETCode* v1, POETCode* v2) 
    { 
      assert (v2 != 0);
      if (v1 == 0) { return tupleMaker.new_single(v2); }
      if (v1->get_enum() == SRC_TUPLE) 
          return tupleMaker.new_tuple(static_cast<POETTuple*>(v1), v2); 
      return tupleMaker.new_tuple(tupleMaker.new_single(v1),v2);
    }
    POETTuple* new_pair(POETCode* v1, POETCode* v2)
     { return append_tuple(tupleMaker.new_single(v1), v2); }

    POETBop* new_MAP(POETCode* arg1, POETCode* arg2)
     { return MAPMaker.new_item(arg1,arg2); }
};
#define EMPTY ASTFactory::new_empty()

#endif
