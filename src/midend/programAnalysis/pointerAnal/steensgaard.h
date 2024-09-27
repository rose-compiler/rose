#ifndef STEENSGAARD_H
#define STEENSGAARD_H

/******Author: Qing Yi, Andrew Long 2007 ********/

#include <union_find.h>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>

#define BOT NULL

class ECR;
struct Lambda {
   std::list<ECR *> inParams, outParams;
   std::list<ECR*>& get_inParams() { return inParams; }
   std::list<ECR*>& get_outParams() { return outParams; }
}; ;

class ECR : public UF_elem
{
   ECR* type;
   Lambda* lambda;
   std::list<ECR *> pending;
   ECR * find_group()
    {  return static_cast<ECR*>(UF_elem::find_group()); }


public:
   ECR() : UF_elem(), type(BOT), lambda(BOT) {}
   ~ECR() {}

   ECR * union_with(ECR *that) {
                UF_elem::union_with(that);
                return static_cast<ECR*>(UF_elem::find_group());
   }

   ECR* get_ecr()  { return find_group(); }
   ECR* get_type() { 
        ECR* g = find_group();
        return (g->type)? g->type->find_group() : g->type;
    }
   void set_type(ECR *that) { 
        ECR* g = find_group();
        g->type = that;
   }
   std::list<ECR*>& get_pending() {return find_group()->pending;}
   Lambda* get_lambda() { return lambda; }
   void set_lambda(Lambda* l) { lambda = l; } 
};

class ECRmap {
 typedef std::string Variable;
 public:
   class VariableAlreadyDefined { 
       public: 
         Variable var; 
         VariableAlreadyDefined(const Variable& _var) : var(_var) {}
   }; 

   // x = y
   void x_eq_y(Variable x, Variable y) {
      ECR* t1 = get_ECR(x)->get_type();
      ECR* t2 = get_ECR(y)->get_type();
      if (t1 != t2)
         cjoin(t1, t2);
   }
   // x = & y
   void x_eq_addr_y(Variable x, Variable y) {
      ECR * t1 = get_ECR(x)->get_type();
      ECR * t2 = get_ECR(y);
      if (t1 != t2) {
         join(t1, t2);
      }
   }
   // x = *y
   void x_eq_deref_y(Variable x, Variable y) {
      ECR* t1 = get_ECR(x)->get_type();
      ECR* t2 = get_ECR(y)->get_type();
      if (t2->get_type() == BOT) {
         set_type(t2, t1);
      }
      else {
         ECR* t3 = t2->get_type();
         if (t1 != t3)  {
             cjoin(t1, t3);
         }
      }
   }   
   // x = op(y1,...yn)
   void x_eq_op_y(Variable x, const std::list<Variable>& y) {
      ECR* t1 = get_ECR(x)->get_type();
      for (std::list<Variable>::const_iterator yp = y.begin();
           yp != y.end(); ++yp) {
         ECR* t2 = get_ECR(*yp)->get_type();
         if (t1 != t2) cjoin(t1, t2);
      }
   }
  // allocate(x)
  void allocate(Variable x) {
      ECR* t = get_ECR(x)->get_type();
      if (t->get_type() == BOT) {
          ECR* res = new_ECR();
          set_type(t,res);
      }
  }
  // *x = y
  void deref_x_eq_y(Variable x, Variable y) {
      ECR* t1 = get_ECR(x)->get_type();
      ECR* t2 = get_ECR(y)->get_type();
      if (t1->get_type() == BOT) {
         set_type(t1, t2);
      }
      else {
         ECR* t3 = t1->get_type();
         if (t2 != t3) 
             cjoin(t3, t2);
      }
   }   
  // outParams = x (inparams)
  void function_def_x(Variable x, const std::list<Variable>& inParams, const std::list<Variable>& outParams) 
   {
     ECR* t = get_ECR(x)->get_type();
     Lambda* l = t->get_lambda();
     if (l == BOT) {
        l = new_Lambda();
        set_lambda(l,inParams, outParams);
        t->set_lambda(l);
     }
     else {
       std::list<ECR *>::const_iterator p1=l->get_inParams().begin();
       std::list<Variable>::const_iterator p2=inParams.begin();
        for ( ; p1 != l->get_inParams().end(); ++p1,++p2) {
           assert(p2 != inParams.end());
           join(*p1, get_ECR(*p2)->get_type());
        }
        assert(p2 == inParams.end());
        p1=l->get_outParams().begin();
        p2=outParams.begin();
        for ( ; p1 != l->get_outParams().end(); ++p1,++p2) {
           assert(p2 != outParams.end());
           join(*p1, get_ECR(*p2)->get_type());
        }
        assert(p2 == outParams.end());
     } 
   }
  // x = p (y)
  void function_call_p(Variable p, const std::list<Variable>& x, const std::list<Variable>& y)
  {
     ECR* t = get_ECR(p)->get_type();
     Lambda* l = t->get_lambda();
     if (l == BOT) {
        l = new_Lambda();
        set_lambda(l,y,x);
        t->set_lambda(l);
     }
     else {
       std::list<ECR *>::const_iterator p1=l->get_inParams().begin();
       std::list<Variable>::const_iterator p2=y.begin();
        for ( ; p1 != l->get_inParams().end(); ++p1,++p2) {
           assert(p2 != y.end());
          ECR* cur = *p1;
          assert(cur != 0);
          Variable v2 = *p2;
          if (v2 != "")
             join(cur->get_ecr(), get_ECR(v2)->get_type());
        }
        assert(p2 == y.end());
        p1=l->get_outParams().begin();
        p2=x.begin();
        for ( ; p1 != l->get_outParams().end(); ++p1,++p2) {
           assert(p2 != x.end());
           ECR* cur = *p1;
          assert(cur != 0);
          Variable v2 = *p2;
           if ( v2 != "")
           join(get_ECR(v2)->get_type(), cur->get_ecr());
        }
        assert(p2 == x.end());
     } 
  }

  virtual void dump() { output(std::cerr); }
  int find_LOC(std::ostream& out, std::map<ECR*, int>& locmap, int& loc, ECR* p)
    {
              int cur = -1;
              std::map<ECR*, int>::const_iterator p1 = locmap.find(p);
              if (p1 == locmap.end()) {
                  locmap[p] = ++loc;
                  cur = loc;
              } 
              else
                 cur = p1->second;
      return cur;
    }
  void outputLOC(std::ostream& out, std::map<ECR*, int>& locmap, int& loc, ECR* p) {
      int max = 0;
      out << " LOC" << find_LOC(out,locmap,loc,p);
      for (;;) {
        p = p->get_type();
        if (p == 0) break;
        int cur = find_LOC(out,locmap,loc,p);
        if (max < 0) break;
        else if (cur <= max) max = -1;
        else max = cur;
        out << "=>" << "LOC" << cur << " ";
        if (p ->get_pending().size() != 0) {
           out << "(pending ";
           for (std::list<ECR*>::const_iterator pp=p->get_pending().begin(); 
                pp != p->get_pending().end(); ++pp) 
               outputLOC(out,locmap, loc, (*pp)->get_ecr());
           out << ") ";
        }
        Lambda* t = p->get_lambda();
        if (t != 0) {
           out << "(inparams: ";
           for (std::list<ECR*>::const_iterator pp=t->get_inParams().begin(); 
                pp != t->get_inParams().end(); ++pp) 
              outputLOC(out,locmap,loc,(*pp)->get_ecr());
           out << ") ";
           out << "->(outparams: ";
           for (std::list<ECR*>::const_iterator pp=t->get_outParams().begin(); 
                pp != t->get_outParams().end(); ++pp)  
              outputLOC(out,locmap,loc,(*pp)->get_ecr());
           out << ") ";
       }
    }
  }

  void output(std::ostream& out) {
      std::map<ECR*, int> locmap;
      int loc = 0;
      for (std::map<Variable, ECR*>::iterator 
           itMap = table.begin(); itMap != table.end(); itMap++) {
           ECR* p = itMap->second->get_ecr();
           out << itMap->first ;
           outputLOC(out,locmap,loc,p);
           out << "\n";
      }
   }

   bool mayAlias(Variable x, Variable y) {
      if (table.find(x) == table.end() || table.find(y) == table.end()) 
         return false;     
      
      if (table[x]->get_type() == table[y]->get_type())
         return true;
      else
         return false;
   }
   virtual ~ECRmap() {
      for (std::list<ECR*>::const_iterator p = ecrList.begin();
           p != ecrList.end(); ++p) {
          delete (*p);
      }
   }

 private:
  std::map<Variable, ECR*> table;
  std::list<ECR*> ecrList;
  std::list<Lambda> lambdaList;
  ECR* get_ECR(Variable x) {
     assert(x != "");
     std::map<Variable, ECR*>::const_iterator p = table.find(x);
     ECR* res = 0;
     if (p == table.end()) {
        res = new_ECR();
        table[x] = res;
     }
     else
        res = (*p).second;
     if (res->get_type() == 0) 
         res->set_type(new_ECR());
     return res;
  }
  ECR* new_ECR() {
     ecrList.push_back(new ECR());
     return ecrList.back();
  }
  Lambda* new_Lambda() {
     lambdaList.push_back(Lambda());
     return &lambdaList.back();
  }
  void set_lambda(Lambda* l,const std::list<Variable>& inParams, const std::list<Variable>& outParams) {
     for (std::list<Variable>::const_iterator p = inParams.begin();
          p != inParams.end(); ++p) {
        Variable cur = *p;
        if (cur != "")
           l->get_inParams().push_back(get_ECR(cur)->get_type());
        else l->get_inParams().push_back(0);
     }
     for (std::list<Variable>::const_iterator p2 = outParams.begin();
          p2 != outParams.end(); ++p2) {
        Variable cur = *p2;
        if (cur != "")
           l->get_outParams().push_back(get_ECR(cur)->get_type());
        else
           l->get_outParams().push_back(new_ECR());
     }
  }
  void set_type(ECR * e, ECR * t) {
      e->set_type(t);
     assert(t != BOT && e->get_type() == t);
      std::list<ECR*> pending = e->get_pending();
      if (pending.size()) {
         for (std::list<ECR*>::const_iterator p=pending.begin(); 
              p != pending.end(); ++p) 
            join(t, *p);
         e->get_pending().clear();
      }
   }
   
  void cjoin(ECR* e1, ECR* e2) {
      if (e2->get_type() == BOT) {
         e2->get_pending().push_back(e1);
       }
      else
         join(e1, e2);
   }

  void unify_lambda(Lambda* l1, Lambda* l2)
  {
        std::list<ECR *>::const_iterator p1=l1->get_inParams().begin();
        std::list<ECR *>::const_iterator p2=l2->get_inParams().begin();
        for ( ; p1 != l1->get_inParams().end(); ++p1,++p2) {
           assert(p2 != l2->get_inParams().end());
           join(*p1, *p2);
        }
        assert(p2 == l2->get_inParams().end());
        p1=l1->get_outParams().begin();
        p2=l2->get_outParams().begin();
        for ( ; p1 != l1->get_outParams().end(); ++p1,++p2) {
           assert(p2 != l2->get_outParams().end());
           join(*p1, *p2);
        }
        assert(p2 == l2->get_outParams().end());
   }
  void unify(ECR * t1, ECR * t2) {
     assert(t1 != 0 && t2 != 0);
     Lambda* l1 = t1->get_lambda();
     Lambda* l2 = t2->get_lambda();
     if (l1 && l2) {
        unify_lambda(l1,l2);
     }
     join(t1,t2);
  }

  void join(ECR * e1, ECR * e2) {
      e1 = e1->get_ecr();
      e2 = e2->get_ecr();
      if (e1 == e2) return;
      ECR* t1 = e1->get_type();
      ECR* t2 = e2->get_type();
      Lambda* l1 = e1->get_lambda();
      Lambda* l2 = e2->get_lambda();
      std::list<ECR*> *pending1 = &e1->get_pending(), *pending2 = &e2->get_pending();
      ECR* e = e1->union_with(e2);
      if (l1 == BOT) {
         if (l2 != BOT) 
           e->set_lambda(l2);
      }
      else {
         e->set_lambda(l1);
         if (l2 != BOT)
            unify_lambda(l1,l2); 
      }

      std::list<ECR*> *pending = &e->get_pending();
 
      if (t1 == BOT) {
         e->set_type(t2);
         if (t2 == BOT) {
            if (e == e2) {
               assert(pending != pending1);
               pending->insert(pending->end(), pending1->begin(), pending1->end());
            }
            else if (e == e1) {
              assert(pending != pending2);
               pending->insert(pending->end(), pending2->begin(), pending2->end());
            }
            else {
              assert(pending != pending2 && pending != pending1);
               *pending = *pending1;
               pending->insert(pending->end(), pending2->begin(), pending2->end());
              }
         }
         else {
           if (pending1->size()) {
              for (std::list<ECR*>::const_iterator p=pending1->begin();
                   p != pending1->end(); ++p) 
                 join(e, *p);
            }
            pending->clear();
        }
      }
      else {
         e->set_type(t1);
         if (t2 == BOT) {
             if (pending2->size()) {
               for (std::list<ECR*>::const_iterator p=pending2->begin();
                    p != pending2->end(); ++p) 
                  join(e, *p);
             }
         }
         else
            unify(t1, t2);
         pending->clear();
      }
   }
};

#endif
