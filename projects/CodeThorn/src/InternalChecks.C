/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"

#define EXCLUDE_RDANALYSIS

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "Analyzer.h"
#include "LanguageRestrictor.h"
#include <cstdio>
#include <cstring>
#include <map>
#include "SgTypeSizeMapping.h"

#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

#ifndef EXCLUDE_RDANALYSIS
#include "RDLattice.h"
#endif

#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "InternalChecks.h"

using namespace CodeThorn;
using CodeThorn::color;

void checkTypes();
void checkLanguageRestrictor(int argc, char *argv[]);
void checkLargeSets();
void nocheck(string checkIdentifier, bool checkResult);
void check(string checkIdentifier, bool checkResult, bool check);

// intentionally global
extern bool checkresult;

bool CodeThorn::internalChecks(int argc, char *argv[]) {
  try {
    // checkTypes() writes into checkresult
    checkTypes();
    //checkLanguageRestrictor(argc,argv);
  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    checkresult=false;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    checkresult=false;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    checkresult=false;
  }
  cout << "-------------------------"<<endl;
  if(checkresult)
    cout <<color("bold")<<"ALL ACTIVE CHECKS PASSED."<<color("bold-off")<<endl;
  else
    cout <<color("red")<< "SOME CHECKS FAILED."<<endl;
  cout << color("white")<<"-------------------------"<<endl;
  cout << color("default-bg-color");
  cout << color("normal");
  return checkresult;
}

#ifdef USE_SAWYER_COMMANDLINE

#else
namespace po = boost::program_options;
#endif

// this function reports the results of checks
// if the passed argument is true the check is PASS, otherwise FAIL.

#if 0
void nocheck(string checkIdentifier, bool checkResult) {
  check(checkIdentifier,checkResult,false);
}
void check(string checkIdentifier, bool checkResult, bool check=true) {

  static int checkNr=1;
  cout << color("white") << "CHECK "<<checkNr<<": "; 
  if(!check) {
    cout<<color("white")<<"----";
  } else {
    if(checkResult) {
      cout<<color("green")<<"PASS";
    } else {
      cout<<color("red")<<"FAIL";
      checkresult=false;
    }
  }
  cout << " ";
  cout << color("white") << "["<<checkIdentifier << "]";
  checkNr++;
  cout<<color("normal")<<endl;
}
#endif

void checkTypes() {
  VariableIdMappingExtended variableIdMapping;
  AbstractValue::setVariableIdMapping(&variableIdMapping);
  
  PState s1;
  cout << "RUNNING CHECKS:"<<endl;
  {
    // check temporary variables (create and delete)
    VariableId var_tmp=variableIdMapping.createUniqueTemporaryVariableId("tmp");
    variableIdMapping.deleteUniqueTemporaryVariableId(var_tmp);
  }

  {
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR BOOLLATTICE TYPE:"<<endl;
    CodeThorn::BoolLattice a;
    a=true;
    check("a=true => isTrue",a.isTrue());
    CodeThorn::BoolLattice b;
    b=false;
    check("b=false => isFalse",b.isFalse());
    CodeThorn::BoolLattice c=a||b;
    check("c=a||b => c isTrue ",c.isTrue());
    CodeThorn::Top e;
    CodeThorn::BoolLattice d;
    d=e;
    check("Top e; d=e => d isTop",d.isTop());
    c=c||d;
    check("c=c||d: true",c.isTrue());
    CodeThorn::BoolLattice f=CodeThorn::Bot();
    d=CodeThorn::Bot();
    check("d=bot: bot",d.isBot());
    check("f=bot: bot",f.isBot());
    a=d&&f;
    check("a=d&&f => a isBot",a.isBot());
    f=false;
    check("f=false => f isFalse",f.isFalse());
    a=d&&f;
    check("a=d&&f: a isFalse (we define it this way)",a.isFalse());
  }

  {
    cout << "RUNNING CHECKS FOR NUMBER LATTICE TYPE:"<<endl;
    AbstractValue a;
    a=true;
    check("a=true => isTrue",a.isTrue());
    check("a=true => !isFalse",!a.isFalse());
    AbstractValue b;
    b=false;
    check("b=false => isFalse",b.isFalse());
    check("b=false => !isTrue",!b.isTrue());
    AbstractValue c=a.operatorOr(b);
    check("c=a.operatorOr(b): ",c.isTrue());
    CodeThorn::Top e;
    AbstractValue d;
    d=e;
    check("Top e; d=e; d isTop ",d.isTop());
    c=c.operatorOr(d);
    check("c=c.operatorOr(d) => c isTrue ",c.isTrue());
    AbstractValue f=CodeThorn::Bot();
    d=CodeThorn::Bot();
    
    a=d.operatorAnd(f);
    check("d=bot;f=bot;a=d.operatorAnd(f); => a isBot",a.isBot());
    f=false;
    a=d.operatorAnd(f);
    check("f=false; a=d.operatorAnd(f); => a isFalse",a.isFalse());
    a=5;
    check("a=5; a.isTrue()==true",a.isTrue()==true);
    check("a=5; a.isFalse()==false",a.isFalse()==false);

    a=0;
    check("a=0; a.isTrue()==false",a.isTrue()==false);
    check("a=0; a.isFalse())==true",a.isFalse()==true);
  }

  {
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR CONSTRAINT TYPE:"<<endl;
    VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
    VariableId var_y=variableIdMapping.createUniqueTemporaryVariableId("y");
    Constraint c1(Constraint::EQ_VAR_CONST,var_x,1);
    Constraint c2(Constraint::NEQ_VAR_CONST,var_y,2);
    Constraint c3=DISEQUALITYCONSTRAINT;
    Constraint c4=Constraint(Constraint::EQ_VAR_CONST,var_y,2);
    ConstraintSet cs;
    cs.addConstraint(c1);
    cs.addConstraint(c2);
    check("inserted 2 different constraints, size of constraint set == 2",cs.size()==2);
    check("c1:constraintExists(EQ_VAR_CONST,x,1) == true",cs.constraintExists(Constraint::EQ_VAR_CONST,var_x,1));
    check("c1:constraintExists(NEQ_VAR_CONST,x,1) == false",!cs.constraintExists(Constraint::NEQ_VAR_CONST,var_x,1));
    check("c2:constraintExists(NEQ_VAR_CONST,y,2) == true",cs.constraintExists(Constraint::NEQ_VAR_CONST,var_y,2));
    check("c3:isDisequality==false",cs.disequalityExists()==false);
    //cout << "CS3:"<<cs.toString()<<endl;
    cs.addConstraint(c4);
    //cout << "CS4:"<<cs.toString()<<endl;
    //cout << "CS:"<<cs.toString()<<endl;
    check("insert y==2; => disequalityExists() == true",cs.disequalityExists());
    cs.addConstraint(c3);
    check("added disequality => disequalityExists() == true",cs.disequalityExists());
    check("Disequality exists <=> size()==1",cs.size()==1);
    check("c1!=c2",c1!=c2);
    check("c1!=c3",c1!=c3);
    check("c2!=c3",c2!=c3);

    {
      // check for equalities 
      {
        Constraint c1(Constraint::EQ_VAR_CONST,var_x,1);
        Constraint c2=Constraint(Constraint::EQ_VAR_CONST,var_y,2);
        ConstraintSet cs1;
        cs1.addConstraint(c1);
        cs1.addConstraint(c2);
        Constraint c5(Constraint::EQ_VAR_VAR,var_x,var_y);
        cs1.addConstraint(c5);
        check("cs1.disequalityExists()==true",cs1.disequalityExists());
      }
      {
        Constraint c1(Constraint::NEQ_VAR_CONST,var_x,1);
        Constraint c2=Constraint(Constraint::NEQ_VAR_CONST,var_x,2);
        Constraint c3=Constraint(Constraint::NEQ_VAR_CONST,var_y,3);
        ConstraintSet cs1;
        cs1.addConstraint(c1);
        cs1.addConstraint(c2);
        cs1.addConstraint(c3);
        Constraint c5(Constraint::EQ_VAR_VAR,var_x,var_y);
        cs1.addConstraint(c5);
        check("c5:constraintExists(EQ_VAR_VAR,x,y) == true",cs1.constraintExists(Constraint(Constraint::EQ_VAR_VAR,var_x,var_y)));    

        check("c1:constraintExists(EQ_VAR_CONST,x,1) == false",cs1.constraintExists(Constraint::EQ_VAR_CONST,var_x,1)==false);
        check("c1:constraintExists(NEQ_VAR_CONST,x,1) == true",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_x,1)==true);
        check("c1:constraintExists(EQ_VAR_CONST,y,2) == false",cs1.constraintExists(Constraint::EQ_VAR_CONST,var_y,2)==false);
        check("c1:constraintExists(NEQ_VAR_CONST,y,2) == true",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,2)==true);
        cs1.removeAllConstraintsOfVar(var_x);
        cs1.removeAllConstraintsOfVar(var_y);
        cs1.removeAllConstraintsOfVar(var_x);
      }
    }
    ConstraintSet cs1;
    cs1.addConstraint(c1);
    ConstraintSet cs2;
    cs2.addConstraint(c2);
    check("cs1!=cs2)",cs1!=cs2);

    {
      Constraint c1(Constraint::NEQ_VAR_CONST,var_y,1);
      Constraint c2(Constraint::NEQ_VAR_CONST,var_y,3);
      Constraint c3(Constraint::NEQ_VAR_CONST,var_y,5);
      Constraint c4a(Constraint::EQ_VAR_CONST,var_y,6);
      Constraint c4b(Constraint::EQ_VAR_CONST,var_y,3);
      ConstraintSet cs1;
      ConstraintSet cs2;
      PState s;
      cs1.addConstraint(c1);
      cs1.addConstraint(c2);
      cs1.addConstraint(c3);
      cs2.addConstraint(c1);
      cs2.addConstraint(c3);
      check("cs1!=cs2",cs1!=cs2);
      check("!(cs1==cs2)",!(cs1==cs2));
      check("!(cs1<cs2)",!(cs1<cs2));
      check("cs1>cs2",(cs2<cs1));
      EStateSet es;
      EState es1=EState(1,&s,&cs1);
      es.processNewOrExisting(es1);
      EState es2=EState(1,&s,&cs2);
      es.processNewOrExisting(es2);
      check("es.size()==2",es.size()==2);
      {
      Constraint c5(Constraint::EQ_VAR_CONST,var_y,10);
      cs1.addConstraint(c5);
      check("constraintExists(NEQ_VAR_CONST,y,1) == false",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,1)==false);
      check("constraintExists(NEQ_VAR_CONST,y,3) == false",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,3)==false);
      check("constraintExists(NEQ_VAR_CONST,y,5) == false",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,5)==false);
      check("cs1.size()==1",cs1.size()==1);
      }
    }
  }
  {  
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR NUMBER LATTICE:"<<endl;
    AbstractValue cint1(1);
    check("cint1 == 1",cint1.getIntValue()==1);
    AbstractValue cint2=CodeThorn::Top();
    AbstractValue cint3;
    cint3=CodeThorn::Top();
    check("cint2 isTop",cint2.isTop());
    check("cint3 isTop",cint3.isTop());
    check("!(cint1 == cint3)",!(cint1==cint3)); // strictly weak ordering test
    check("cint2 == cint3",cint2==cint3); // strictly weak ordering test
  }
  {
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR PSTATE AND PSTATESET:"<<endl;
    VariableIdMapping variableIdMapping;
    EState es1;
    EState es2;
    PState s0;
    PState s1;
    PState s2;
    PState s3;
    PState s5;
    AbstractValue valtop=CodeThorn::Top();
    AbstractValue val1=500;
    AbstractValue val2=501;
    PStateSet pstateSet;
    VariableId x=variableIdMapping.createUniqueTemporaryVariableId("x");
    VariableId y=variableIdMapping.createUniqueTemporaryVariableId("y");
    check("var x not in pstate1",s1.varExists(x)==false);
    check("var y not in pstate2",s2.varExists(y)==false);
    s1.writeToMemoryLocation(x,val1);
    s2.writeToMemoryLocation(y,val2);
    s3.writeToMemoryLocation(x,val2);
    s5.writeToMemoryLocation(x,valtop);
    s5.writeToMemoryLocation(y,valtop);
    check("var x exists in pstate s1",s1.varExists(x)==true);
    check("var x==500",((s1.readFromMemoryLocation(x).operatorEq(val1)).isTrue())==true);
    check("var y exists in pstate s2",s2.varExists(y)==true);
    check("var y==501",((s2.readFromMemoryLocation(y).operatorEq(val2)).isTrue())==true);
    //check("s0 < s1",(s0<s1)==true);
    //check("s0 < s2",(s0<s2)==true);
    check("!(s1 == s2)",(s1==s2)==false);
    check("s1<s2 xor s2<s1)",(s1<s2)^(s2<s1));
    check("var x in pstate s3",s3.varExists(x)==true);
    check("s3[x]==501",((s3.readFromMemoryLocation(x).operatorEq(val2)).isTrue())==true);
    check("!(s1==s2)",(!(s1==s2))==true);
    check("!(s1==s3)",(!(s1==s3))==true);
    check("!(s2==s3)",(!(s2==s3))==true);
    PState s4=s1;
    check("s1==s4",(s1==s4)==true);

    s1.writeToMemoryLocation(x,val2);
    check("s1.size()==1",s1.stateSize()==1);

    pstateSet.process(s0);
    check("empty pstate s0 inserted in pstateSet => size of pstateSet == 1",pstateSet.size()==1);
    pstateSet.process(s1);
    check("s1 inserted in pstateSet => size of pstateSet == 2",pstateSet.size()==2);
    pstateSet.process(s1);
    check("s1 reinserted in pstateSet => size remains the same",pstateSet.size()==2);
    pstateSet.process(s2);
    check("s2 inserted => size of pstateSet == 3",pstateSet.size()==3);

    const PState* pstateptr0=pstateSet.processNewOrExisting(s0); // version 1
    check("obtain pointer to s0 from pstateSet and check !=0",pstateptr0!=0);
    check("check pointer refers indeed to s0 (operator==)",(*pstateptr0)==s0);
    const PState* pstateptr1=pstateSet.processNewOrExisting(s1); // version 1
    check("obtain pointer to s1 from pstateSet and check !=0",pstateptr1!=0);
    const PState* pstateptr2=pstateSet.processNewOrExisting(s2); // version 1
    check("obtain pointer to s2 from pstateSet and check !=0",pstateptr2!=0);
    check("check pstate.exists(s0)",pstateSet.exists(s0));
    check("check pstate.exists(s1)",pstateSet.exists(s1));
    check("check pstate.exists(s2)",pstateSet.exists(s2));
    check("check !pstate.exists(s5) [s5 does not exist]",!pstateSet.exists(s5));
    check("constint-strictWeak-equality-1",strictWeakOrderingIsEqual(val1,val2)==false);
    check("constint-strictWeak-smaller-1",strictWeakOrderingIsSmaller(val1,val2)==true);

    s4.writeToMemoryLocation(x,valtop);
    check("created s4; inserted x=top; s4.readFromMemoryLocation(x).isTop",s4.readFromMemoryLocation(x).isTop());    
    pstateSet.processNewOrExisting(s4);
    check("inserted s4 => size of pstateSet == 4",pstateSet.size()==4);    
    const PState* pstateptr4=pstateSet.processNewOrExisting(s4); // version 1
    check("obtain pointer to s4 from pstateSet and check !=0",pstateptr4!=0);

#if 1
    EStateSet eStateSet;
    EState es3;
    ConstraintSetMaintainer csm;

    ConstraintSet cs1;
    cs1.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,x,1));
    const ConstraintSet* cs1ptr=csm.processNewOrExisting(cs1);
    es1=EState(1,pstateptr1,cs1ptr);
    
    ConstraintSet cs2;
    cs2.addConstraint(Constraint(Constraint::EQ_VAR_CONST,x,1));
    const ConstraintSet* cs2ptr=csm.processNewOrExisting(cs2);
    es2=EState(1,pstateptr1,cs2ptr);

    ConstraintSet cs3;
    cs3.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,x,1));
    const ConstraintSet* cs3ptr=csm.processNewOrExisting(cs3);
    es3=EState(3,pstateptr4,cs3ptr);

    check("check es1 does not exist in eStateSet",eStateSet.exists(es1)==0);
    check("check es2 does not exist in eStateSet",eStateSet.exists(es2)==0);
    check("check es3 does not exist in eStateSet",eStateSet.exists(es3)==0);

    check("es1!=es2",es1!=es2);
    check("es2!=es3",es1!=es3);
    check("es1!=es3",es2!=es3);
#ifdef ESTATE_MAINTAINER_LIST
    nocheck("es1<es2",es2<es1);
    nocheck("!(es2<es1)",!(es1<es2));
#endif
#ifdef ESTATE_MAINTAINER_SET
    check("es1<es2",es1<es2);
    check("!(es2<es1)",!(es2<es1));
#endif

    check("!(es1==es2)",!(es1==es2));

#ifdef ESTATE_MAINTAINER_LIST
    nocheck("es1<es3",es1<es3);
    nocheck("es2<es3",es2<es3);
#endif
#ifdef ESTATE_MAINTAINER_SET
    check("es1<es3",es1<es3);
    check("es2<es3",es2<es3);
#endif

    check("es2==es2",es2==es2);
    check("=> eStateSet.size() == 0",eStateSet.size() == 0);

    check("es1 does not exist in eStateSet",!eStateSet.exists(es2));
    eStateSet.processNewOrExisting(es1);
    const EState* estateptr1=eStateSet.processNewOrExisting(es1);
    check("add es1 and obtain pointer to es1 from eStateSet and check !=0",estateptr1!=0);
    check("es1 exists in eStateSet",eStateSet.exists(es1));
    check("=> eStateSet.size() == 1",eStateSet.size() == 1);

    check("es2 does not exist in eStateSet",!eStateSet.exists(es2));
    eStateSet.processNewOrExisting(es2);
    const EState* estateptr2=eStateSet.processNewOrExisting(es2);
    check("add es2 and obtain pointer to es2 from eStateSet and check !=0",estateptr2!=0);
    check("es2 exists in eStateSet",eStateSet.exists(es2));
    check("=> eStateSet.size() == 2",eStateSet.size() == 2);

    check("es3 does not exist in eStateSet",!eStateSet.exists(es3));
    eStateSet.processNewOrExisting(es3);
    const EState* estateptr3=eStateSet.processNewOrExisting(es3);
    check("add es3 and obtain pointer to es3 from eStateSet and check !=0",estateptr3!=0);
    check("es3 exists in eStateSet",eStateSet.exists(es3));
    check("=> eStateSet.size() == 3",eStateSet.size() == 3);
    checkLargeSets();
#endif

 }

#if 0
  // MS: TODO: rewrite the following test to new check format
  {
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR COMBINED TYPES:"<<endl;
    EState es1;
    EState es2;
    cout << "EState created. "<<endl;
    cout << "empty EState: "<<es1.toString()<<endl;
    es1.setLabel(1);
    VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
    es1.constraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,var_x,1));
    es2.setLabel(1);
    es2.constraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,var_x,1));
    cout << "empty EState with label and constraint es1: "<<es1.toString()<<endl;
    cout << "empty EState with label and constraint es2: "<<es2.toString()<<endl;
    PState s;
    es1.setPState(&s);
    es2.setPState(&s);
    cout << "empty EState with label, empty pstate, and constraint es1: "<<es1.toString()<<endl;
    cout << "empty EState with label, empty pstate, and constraint es2: "<<es2.toString()<<endl;
    bool testres=(es1==es2);
    if(testres)
      cout << "es1==es2: "<<testres<< "(not as expected: FAIL)"<<endl;
    else
      cout << "es1==es2: "<<testres<< "(as expected: PASS)"<<endl;
  }
#endif

  // check stream operators
  {
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR INPUT/OUTPUT STREAM OPs"<<endl;

    stringstream ss2;
    ss2<<"test1";
    check("Parse: Testing test2 on test1.",!CodeThorn::Parse::checkWord("test2",ss2));
    //cout << "Remaing stream: "<<ss2.str()<<endl;
    stringstream ss3;
    ss3<<"test1";
    check("Parse: Testing test1 on test1.",CodeThorn::Parse::checkWord("test1",ss3));
    //cout << "Remaing stream: "<<ss3.str()<<endl;

    {
      stringstream ss;
      string s="aaabbb";
      ss<<s;
      string parseString="aaa";
      CodeThorn::Parse::parseString(parseString,ss); // throws exception if it fails
      char next;
      ss>>next;
      check(string("Parsing: ")+parseString+" from:"+s+" Next:"+next,true);      
    }
#ifndef EXCLUDE_RDANALYSIS
    {
      RDLattice a;
      VariableId var1;
      var1.setIdCode(1);
      VariableId var2;
      var2.setIdCode(2);
      a.insertPair(1,var1);
      RDLattice b;
      b.insertPair(1,var1);
      b.insertPair(2,var2);
      //a.toStream(cout);cout<<endl;
      //b.toStream(cout);cout<<endl;
      check("a ApproximatedBy b",a.approximatedBy(b)==true);
      check("not (b ApproximatedBy a)",b.approximatedBy(a)==false);
    }
#endif
  } // end of stream operator checks

  {  
    cout << "------------------------------------------"<<endl;
    cout << "RUNNING CHECKS FOR CALL STRINGS:"<<endl;
    CallString s1;
    CallString s2;
    check("callstrings: "+s1.toString()+" == "+s2.toString()+" (true)",s1==s2);
  }

}

void checkLanguageRestrictor(int argc, char *argv[]) {
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);
  LanguageRestrictor lr;
  LanguageRestrictor::VariantSet vs= lr.computeVariantSetOfProvidedAst(sageProject);
  for(LanguageRestrictor::VariantSet::iterator i=vs.begin();i!=vs.end();++i) {
    cout << "VARIANT: "<<lr.variantToString(*i)<<endl;
  }
  cout <<endl;
  lr.allowAstNodesRequiredForEmptyProgram();
  vs=lr.getAllowedAstNodeVariantSet();
  for(LanguageRestrictor::VariantSet::iterator i=vs.begin();i!=vs.end();++i) {
    cout << "VARIANT: "<<lr.variantToString(*i)<<endl;
  }
}

void checkLargeSets() {
  VariableIdMapping variableIdMapping;
  AbstractValue i;
  set<AbstractValue> cilSet;
  cilSet.insert(AbstractValue(Bot()));
  cilSet.insert(AbstractValue(Top()));
  for(int i=-10;i<10;i++) {
    cilSet.insert(AbstractValue(i));
  }
  check("integer set: bot,-10, ... ,+10,top",cilSet.size()==22); // 1+20+1
}
