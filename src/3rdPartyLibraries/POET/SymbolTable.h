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

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <map>
#include <vector>
#include <poet_config.h>
#include <assert.h>

extern "C" void yyerror(const char* msg);
class POETCode;
class LocalVar;

template <class TableEntry>
class SymbolTable {
 protected:
  std:: map<POETCode*, TableEntry, std:: less<POETCode*> > codemap;
 public:
  typedef typename std:: map<POETCode*,TableEntry,std::less<POETCode*> >::iterator iterator;
  iterator begin() { return codemap.begin(); }
  iterator end() { return codemap.end(); }
  iterator find (POETCode* from) { return codemap.find(from); }
  iterator insert(POETCode* from)
    { 
      return codemap.insert(std::pair<POETCode*,TableEntry>(from,TableEntry())).first; 
    }
  int size() const { return codemap.size(); }
};

class LvarSymbolTable {
 public:
  struct TableEntry {
    POETCode *code, *restr;
    LocalVar *var;
    LocalVarType t;
    TableEntry* next;
    TableEntry() : code(0), restr(0), var(0), t(LVAR_REG), next(0) {}
    TableEntry(POETCode* c, LocalVarType _t, TableEntry* n, LocalVar* v) 
        : code(c), restr(0), var(v), t(_t), next(n) { }
    void reset() {
        if (t == LVAR_ATTR || t == LVAR_OUTPUT) return;
        if (t != LVAR_TUNE && t != LVAR_TUNE_GROUP)
           code = 0; 
    }
    void push(bool copycurrent = false) {
        if (t == LVAR_OUTPUT) return;
        next = new TableEntry(code,t, next,var);
        switch (t)
        {
         case LVAR_REG:
         case LVAR_CODEPAR:
         case LVAR_XFORMPAR:
         case LVAR_TRACE:
         case LVAR_TRACE_OUTDATE:
            if (!copycurrent) code = 0;  break;
         case LVAR_ATTR:
            if (!copycurrent) code = restr;
         default: ; /* do nothing */
        }
    }
    void pop() {
        TableEntry* d = next;
        if (d != 0) {
           t = d->t;
           code = d->code;
           next = d->next;
        }
        else if (t == LVAR_REG) {
           code = 0; 
        }
        if (d != 0) delete d;
    }
  };
 private:
  SymbolTable<TableEntry> impl;
 public:
  LvarSymbolTable() {}
  class Entry {
    SymbolTable<TableEntry>::iterator impl;
    void set_var(LocalVar* v) { (*impl).second.var = v; }
   public:
    Entry(const SymbolTable<TableEntry>::iterator& p, LocalVarType _t) 
       : impl(p) { (*p).second.t = _t; }
    Entry(const SymbolTable<TableEntry>::iterator& p) : impl(p) {}
    POETCode* get_name() const { return (*impl).first; }  
    POETCode* get_code() const { return (*impl).second.code; }
    void set_code(POETCode* c) { (*impl).second.code = c; }
    POETCode* get_restr() const { return (*impl).second.restr; }
    void set_restr(POETCode* c) { (*impl).second.restr = c; }
    LocalVar* get_var() const { return (*impl).second.var; }
    LocalVarType get_entry_type() const { return (*impl).second.t; }
    void set_entry_type(LocalVarType t) { (*impl).second.t = t; }
    Entry operator ++() { return ++impl; }
    Entry operator ++(int) { return impl++; }
    bool operator == (const Entry& that) const { return impl == that.impl; }
    bool operator != (const Entry& that) const { return impl != that.impl; }
    
    void push(bool copycurrent = false) {
        (*impl).second.push(copycurrent);
    }
    void pop() { 
       (*impl).second.pop(); 
    }
    friend class LvarSymbolTable;
  };
  Entry begin() { return impl.begin(); }
  Entry end() { return impl.end(); }
  LocalVar* find (POETCode* var) { 
       Entry r = impl.find(var); 
       if (r.impl == impl.end()) return 0;
       return r.get_var(); 
  }
  LocalVar* insert(POETCode* var, LocalVarType _t = LVAR_UNKNOWN); 
  int size() const { return impl.size(); }
  void push_table(bool copycurrent) {
     for (Entry p = begin(); p != end(); ++p) {
        TableEntry& e = (*p.impl).second;
        e.push(copycurrent);
     }
  }
  void reset_table() {
     for (Entry p = begin(); p != end(); ++p) {
        TableEntry& e = (*p.impl).second;
        e.reset();
     }
  }
  void pop_table() {
     for (Entry p = begin(); p != end(); ++p) {
        TableEntry& e = (*p.impl).second;
        e.pop();
     }
  }

};

class XvarSymbolTable {
 public:
  struct TableEntry {
    POETCode *code;
    POETCode *param;
    LvarSymbolTable* symTable;
    TableEntry() : code(0), param(0), symTable(0) {}
  };
 private:
  SymbolTable<TableEntry> impl;
 public:
  XvarSymbolTable() {}
  class Entry {
    SymbolTable<TableEntry>::iterator impl;
   public:
    Entry(const SymbolTable<TableEntry>::iterator& p) : impl(p) {}
    Entry() {}
    POETCode* get_name() const { return (*impl).first; }  
    POETCode* get_code() const { return (*impl).second.code; }
    void set_codeDef(POETCode* c) { (*impl).second.code = c; }
    LvarSymbolTable* get_symTable() const { return (*impl).second.symTable;}
    void set_symTable(LvarSymbolTable* t) const 
       { assert((*impl).second.symTable == 0); (*impl).second.symTable=t;}
    POETCode* get_param() { return (*impl).second.param; }
    void set_param(POETCode* c) { (*impl).second.param = c; }
    Entry operator ++() { return ++impl; }
    Entry operator ++(int) { return impl++; }
    bool operator == (const Entry& that) const { return impl == that.impl; }
    bool operator != (const Entry& that) const { return impl != that.impl; }
    friend class XvarSymbolTable;
  };
  Entry begin() { return impl.begin(); }
  Entry end() { return impl.end(); }
  Entry find (POETCode* var) { return impl.find(var); }
  Entry insert(POETCode* var) { return impl.insert(var); }
};

class CvarSymbolTable {
 public:
  struct TableEntry {
    POETCode *code;
    POETCode *param;
    LocalVar* inherit;
    std::vector<LocalVar*> attr;
    LvarSymbolTable* symTable;
    POETCode *parse;
    int lookahead;
    TableEntry() : code(0), param(0),inherit(0),symTable(0),parse(0),lookahead(1) {}
  };
 private:
  SymbolTable<TableEntry> impl;
 public:
  CvarSymbolTable() {}
  class Entry {
    SymbolTable<TableEntry>::iterator impl;
   public:
    Entry(const SymbolTable<TableEntry>::iterator& p) : impl(p) {}
    Entry() {}
    POETCode* get_name() const { return (*impl).first; }  
    POETCode* get_code() const { return (*impl).second.code; }
    LocalVar* get_attr(int i) const { return (*impl).second.attr[i]; }
    void append_attr(LocalVar* val) { (*impl).second.attr.push_back(val); }
    unsigned attr_size() const { return (*impl).second.attr.size(); }
    void set_codeDef(POETCode* c) { (*impl).second.code = c; }
    LvarSymbolTable* get_symTable() const { return (*impl).second.symTable;}
    void set_symTable(LvarSymbolTable* t) const 
          { assert((*impl).second.symTable == 0); (*impl).second.symTable=t;}
    POETCode* get_param() { return (*impl).second.param; }
    void set_param(POETCode* c) { (*impl).second.param = c; }
    LocalVar* get_inherit_var() { return (*impl).second.inherit; }
    void set_inherit_var(LocalVar* v) { (*impl).second.inherit = v; }
    POETCode* get_parse() const { return (*impl).second.parse; }
    void set_parse(POETCode* c) { (*impl).second.parse = c; }
    int get_lookahead() const { return  (*impl).second.lookahead; }
    void set_lookahead(int i) { (*impl).second.lookahead=i; }
    
    Entry operator ++() { return ++impl; }
    Entry operator ++(int) { return impl++; }
    bool operator == (const Entry& that) const { return impl == that.impl; }
    bool operator != (const Entry& that) const { return impl != that.impl; }
    friend class CvarSymbolTable;
  };
  Entry begin() { return impl.begin(); }
  Entry end() { return impl.end(); }
  Entry find (POETCode* var) { return impl.find(var); }
  Entry insert(POETCode* var) { return impl.insert(var); }
};
#endif
