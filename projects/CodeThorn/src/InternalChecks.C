/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "StateRepresentation.h"
#include "Analyzer.h"
#include "LanguageRestrictor.h"
#include "Timer.h"
#include "LTL.h"
#include "LTLChecker.h"
#include <cstdio>
#include <cstring>
#include <boost/program_options.hpp>
#include <map>
#include "InternalChecks.h"

void checkTypes();
void checkLanguageRestrictor(int argc, char *argv[]);

// intentionally global
bool checkresult=true;

bool internalChecks(int argc, char *argv[]) {
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
  cout << "------------------"<<endl;
  if(checkresult)
	cout << "ALL CHECKS PASSED."<<endl;
  else
	cout << "SOME CHECKS FAILED."<<endl;
  cout << "------------------"<<endl;
  return checkresult;
}

namespace po = boost::program_options;

// this function reports the results of checks
// if the passed argument is true the check is PASS, otherwise FAIL.

void check(string checkIdentifier, bool checkResult) {
  const string csi = "\33[";
  const string white = csi+"37m";
  const string green = csi+"32m";
  const string red = csi+"31m";
  const string normal = csi+"0m";

  static int checkNr=1;
  cout << white << "CHECK "<<checkNr<<": "; 
  if(checkResult) {
	cout<<green<<"PASS";
  } else {
	cout<<red<<"FAIL";
	checkresult=false;
  }
  cout << " ";
  cout << white << "["<<checkIdentifier << "]";
  checkNr++;
  cout<<normal<<endl;
}

void checkTypes() {
  VariableIdMapping variableIdMapping;
  State s1;
  cout << "RUNNING CHECKS:"<<endl;
  VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
  {
	cout << "------------------------------------------"<<endl;
	cout << "RUNNING CHECKS FOR BOOLLATTICE TYPE:"<<endl;
	AType::BoolLattice a;
	a=true;
	check("a=true => isTrue",a.isTrue());
	AType::BoolLattice b;
	b=false;
	check("b=false => isFalse",b.isFalse());
	AType::BoolLattice c=a||b;
	check("c=a||b => c isTrue ",c.isTrue());
	AType::Top e;
	AType::BoolLattice d;
	d=e;
	check("Top e; d=e => d isTop",d.isTop());
	c=c||d;
	check("c=c||d: top",c.isTrue());
	AType::BoolLattice f=AType::Bot();
	d=AType::Bot();
	check("d=bot: bot",d.isBot());
	check("f=bot: bot",f.isBot());
	a=d&&f;
	check("a=d&&f => a isBot",a.isBot());
	f=false;
	check("f=false => f isFalse",f.isFalse());
	a=d&&f;
	check("a=d&&f: a isFalse (we define it this way)",a.isFalse());
  }

#if 0
  // MS: TODO: rewrite the following test to new check format
  {
	cout << "RUNNING CHECKS FOR CONSTINT LATTICE TYPE:"<<endl;
	AType::ConstIntLattice a;
	a=true;
	cout << "a: "<<a.toString();
	if(a.isTrue()) cout << "(true)";
	if(a.isFalse()) cout << "(false)";
	cout<<endl;
	AType::ConstIntLattice b;
	b=false;
	cout << "b: "<<b.toString();
	if(b.isTrue()) cout << "(true)";
	if(b.isFalse()) cout << "(false)";
	cout<<endl;

	AType::ConstIntLattice c=a||b;
	cout << "c=a||b: "<<c.toString()<<endl;
	AType::Top e;
	AType::ConstIntLattice d;
	d=e;
	cout << "d: "<<d<<endl;
	c=c||d;
	cout << "c=c||d: "<<c<<endl;
	AType::ConstIntLattice f=AType::Bot();
	d=AType::Bot();
	cout << "d: "<<d<<endl;
	cout << "f: "<<d<<endl;
	a=d&&f;
	cout << "a=d&&f: "<<a<<endl;
	f=false;
	cout << "f: "<<f<<endl;
	a=d&&f;
	cout << "a=d&&f: "<<a<<endl;
	a=5;
	cout << "a=5: "<<a;
	if(a.isTrue()) cout << "(true)";
	if(a.isFalse()) cout << "(false)";
	cout<<endl;
	a=0;
	cout << "a=0: "<<a;
	if(a.isTrue()) cout << "(true)";
	if(a.isFalse()) cout << "(false)";
	cout<<endl;
	//	a=a+1;
	//cout << "a=a+1: "<<a<<endl;
  }
#endif
  {
	cout << "------------------------------------------"<<endl;
	cout << "RUNNING CHECKS FOR CONSTRAINT TYPE:"<<endl;
	VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
	VariableId var_y=variableIdMapping.createUniqueTemporaryVariableId("y");
	VariableId var_z=variableIdMapping.createUniqueTemporaryVariableId("z");

	Constraint c1(Constraint::EQ_VAR_CONST,var_x,1);
	Constraint c2(Constraint::NEQ_VAR_CONST,var_y,2);
	Constraint c3(Constraint::DEQ_VAR_CONST,var_z,1);
	Constraint c4(Constraint::EQ_VAR_CONST,var_y,2);
	ConstraintSet cs;
	cs.insert(c1);
	cs.insert(c2);
	cs.insert(c3);
	check("inserted 3 different constraints, size of constraint set == 3",cs.size()==3);
	check("c1:constraintExists(EQ_VAR_CONST,x,1) == true",cs.constraintExists(Constraint::EQ_VAR_CONST,var_x,1));
	check("c1:constraintExists(NEQ_VAR_CONST,x,1) == false",!cs.constraintExists(Constraint::NEQ_VAR_CONST,var_x,1));
	check("c2:constraintExists(NEQ_VAR_CONST,y,2) == true",cs.constraintExists(Constraint::NEQ_VAR_CONST,var_y,2));
	check("c3:constraintExists(DEQ_VAR_CONST,z,1) == true",cs.constraintExists(Constraint::DEQ_VAR_CONST,var_z,1));
	cs.insert(c4);
	check("insert y==2; => constraintExists(DEQ_VAR_CONST,y,2) == true",cs.constraintExists(Constraint::DEQ_VAR_CONST,var_y,2));
	check("c1!=c2",c1!=c2);
	check("c1!=c3",c1!=c3);
	check("c2!=c3",c2!=c3);
	ConstraintSet cs1;
	cs1.insert(c1);
	ConstraintSet cs2;
	cs2.insert(c2);
	check("cs1!=cs2)",cs1!=cs2);

	{
	  Constraint c1(Constraint::NEQ_VAR_CONST,var_y,1);
	  Constraint c2(Constraint::NEQ_VAR_CONST,var_y,3);
	  Constraint c3(Constraint::NEQ_VAR_CONST,var_y,5);
	  Constraint c4a(Constraint::EQ_VAR_CONST,var_y,6);
	  Constraint c4b(Constraint::EQ_VAR_CONST,var_y,3);
	  ConstraintSet cs1;
	  ConstraintSet cs2;
	  State s;
	  cs1.insert(c1);
	  cs1.insert(c2);
	  cs1.insert(c3);
	  cs2.insert(c1);
	  cs2.insert(c3);
	  check("acs1!=acs2",cs1!=cs2);
	  check("!(acs1==acs2)",!(cs1==cs2));
	  check("!(acs1<acs2)",!(cs1<cs2));
	  check("acs1>acs2",(cs2<cs1));
	  EStateSet es;
	  es.insert(EState(1,&s,cs1));
	  es.insert(EState(1,&s,cs2));
	  check("es.size()==2",es.size()==2);
	  {
	  cout<<cs1.toString()<<endl;
	  Constraint c5(Constraint::EQ_VAR_CONST,var_y,10);
	  cs1.insert(c5);
	  cout<<cs1.toString()<<endl;
	  check("constraintExists(NEQ_VAR_CONST,y,1) == false",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,1)==false);
	  check("constraintExists(NEQ_VAR_CONST,y,3) == false",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,3)==false);
	  check("constraintExists(NEQ_VAR_CONST,y,5) == false",cs1.constraintExists(Constraint::NEQ_VAR_CONST,var_y,5)==false);
	  check("cs1.size()==1",cs1.size()==1);
	  }
	}
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
	es1.label=1;
	VariableId var_x=variableIdMapping.createUniqueTemporaryVariableId("x");
	es1.constraints.insert(Constraint(Constraint::EQ_VAR_CONST,var_x,1));
	es2.label=1;
	es2.constraints.insert(Constraint(Constraint::NEQ_VAR_CONST,var_x,1));
	cout << "empty EState with label and constraint es1: "<<es1.toString()<<endl;
	cout << "empty EState with label and constraint es2: "<<es2.toString()<<endl;
	State s;
	es1.state=&s;
	es2.state=&s;
	cout << "empty EState with label, empty state, and constraint es1: "<<es1.toString()<<endl;
	cout << "empty EState with label, empty state, and constraint es2: "<<es2.toString()<<endl;
	bool testres=(es1==es2);
	if(testres)
	  cout << "es1==es2: "<<testres<< "(not as expected: FAIL)"<<endl;
	else
	  cout << "es1==es2: "<<testres<< "(as expected: PASS)"<<endl;
  }
#endif
  {  
	cout << "------------------------------------------"<<endl;
	cout << "RUNNING CHECKS FOR CPPCAPSULECONSTINTLATTICE:"<<endl;
	AType::CppCapsuleConstIntLattice cap1(1);
	check("cap1 == 1",cap1.getValue().getIntValue()==1);
	AType::CppCapsuleConstIntLattice cap2;
	cap2.setValue(AType::Top());
	AType::CppCapsuleConstIntLattice cap3;
	cap3.setValue(AType::Top());
	check("cap2 isTop",cap2.getValue().isTop());
	check("cap3 isTop",cap3.getValue().isTop());
	check("!(cap1 == cap3)",!(cap1==cap3));
	check("cap2 == cap3",cap2==cap3);
  }
  {
	cout << "------------------------------------------"<<endl;
	cout << "RUNNING CHECKS FOR STATE AND STATESET:"<<endl;
	VariableIdMapping variableIdMapping;
	EState es1;
	EState es2;
	State s0;
	State s1;
	State s2;
	State s3;
	State s5;
	AValue valtop=AType::Top();
	AValue val1=500;
	AValue val2=501;
	StateSet stateSet;
	VariableId x=variableIdMapping.createUniqueTemporaryVariableId("x");
	VariableId y=variableIdMapping.createUniqueTemporaryVariableId("y");
	check("var x not in state1",s1.varExists(x)==false);
	check("var y not in state2",s2.varExists(y)==false);
	s1[x]=val1;
	s2[y]=val2;
	s3[x]=val2;
	s5[x]=valtop;
	s5[y]=valtop;
	check("var x exists in state s1",s1.varExists(x)==true);
	check("var x==500",((s1[x].getValue()==val1)).isTrue()==true);
	check("var y exists in state s2",s2.varExists(y)==true);
	check("var y==501",((s2[y].getValue()==val2)).isTrue()==true);
	check("s0 < s1",(s0<s1)==true);
	check("s0 < s2",(s0<s2)==true);
	check("!(s1 == s2)",(s1==s2)==false);
	check("s1 < s2",(s1<s2));
	check("!(s2 < s1)",!(s2<s1));
	check("var x in state s3",s3.varExists(x)==true);
	check("s3[x]==501",((s3[x].getValue())==val2).isTrue()==true);
	check("!(s1==s2)",(!(s1==s2))==true);
	check("!(s1==s3)",(!(s1==s3))==true);
	check("!(s2==s3)",(!(s2==s3))==true);
	State s4=s1;
	check("s1==s4",(s1==s4)==true);

	s1[x]=val2;
	check("s1.size()==1",s1.size()==1);

	stateSet.insert(s0);
	check("empty state s0 inserted in stateSet => size of stateSet == 1",stateSet.size()==1);
	stateSet.insert(s1);
	check("s1 inserted in stateSet => size of stateSet == 2",stateSet.size()==2);
	stateSet.insert(s1);
	check("s1 reinserted in stateSet => size remains the same",stateSet.size()==2);
	stateSet.insert(s2);
	check("s2 inserted => size of stateSet == 3",stateSet.size()==3);

	const State* stateptr0=stateSet.statePtr(s0); // version 1
	check("obtain pointer to s0 from stateSet and check !=0",stateptr0!=0);
	check("check pointer refers indeed to s0 (operator==)",(*stateptr0)==s0);
	const State* stateptr1=stateSet.statePtr(s1); // version 1
	check("obtain pointer to s1 from stateSet and check !=0",stateptr1!=0);
	const State* stateptr2=stateSet.statePtr(s2); // version 1
	check("obtain pointer to s2 from stateSet and check !=0",stateptr2!=0);
	const State* stateptr5=stateSet.statePtr(s5); // version 1
	check("obtain pointer to non-existent s5 from stateSet and check ==0",stateptr5==0);
	if(stateptr5!=0) cout <<"stateptr5 -> "<<stateptr5->toString()<<endl;
	check("check stateExists(s0)",stateSet.stateExists(s0));
	check("check stateExists(s1)",stateSet.stateExists(s1));
	check("check stateExists(s2)",stateSet.stateExists(s2));
	check("check stateExists(s5) does not exist",!stateSet.stateExists(s5));
	check("constint-strictWeak-equality-1",strictWeakOrderingIsEqual(val1,val2)==false);
	check("constint-strictWeak-smaller-1",strictWeakOrderingIsSmaller(val1,val2)==true);

	s4[x]=valtop;
	check("created s4; inserted x=top; s4[x].getValue.isTop",s4[x].getValue().isTop());	
	stateSet.insert(s4);
	check("inserted s4 => size of stateSet == 4",stateSet.size()==4);	
	const State* stateptr4=stateSet.statePtr(s4); // version 1
	check("obtain pointer to s4 from stateSet and check !=0",stateptr4!=0);

	EStateSet eStateSet;
	EState es3;
	es1.label=1;
	es1.state=stateptr1;
	es1.constraints.insert(Constraint(Constraint::NEQ_VAR_CONST,x,1));
	es2.label=1;
	es2.state=stateptr1;
	es2.constraints.insert(Constraint(Constraint::EQ_VAR_CONST,x,1));
	es3.label=3;
	es3.state=stateptr4;
	es3.constraints.insert(Constraint(Constraint::NEQ_VAR_CONST,x,1));
	check("check es1 does not exist in eStateSet",eStateSet.eStateExists(es1)==0);
	check("check es2 does not exist in eStateSet",eStateSet.eStateExists(es2)==0);
	check("check es3 does not exist in eStateSet",eStateSet.eStateExists(es3)==0);

	check("es1!=es2",es1!=es2);
	check("es2!=es3",es1!=es3);
	check("es1!=es3",es2!=es3);
	check("es2<es1",es2<es1);
	check("!(es1<es2)",!(es1<es2));
	check("!(es1==es2)",!(es1==es2));
	check("es1<es3",es1<es3);
	check("es2<es3",es2<es3);
	check("es2==es2",es2==es2);
	check("=> eStateSet.size() == 0",eStateSet.size() == 0);

	check("es1 does not exist in eStateSet",!eStateSet.eStateExists(es2));
	eStateSet.insert(es1);
	const EState* estateptr1=eStateSet.eStatePtr(es1);
	check("add es1 and obtain pointer to es1 from eStateSet and check !=0",estateptr1!=0);
	check("es1 exists in eStateSet",eStateSet.eStateExists(es1));
	check("=> eStateSet.size() == 1",eStateSet.size() == 1);

	check("es2 does not exist in eStateSet",!eStateSet.eStateExists(es2));
	eStateSet.insert(es2);
	const EState* estateptr2=eStateSet.eStatePtr(es2);
	check("add es2 and obtain pointer to es2 from eStateSet and check !=0",estateptr2!=0);
	check("es2 exists in eStateSet",eStateSet.eStateExists(es2));
	check("=> eStateSet.size() == 2",eStateSet.size() == 2);

	check("es3 does not exist in eStateSet",!eStateSet.eStateExists(es3));
	eStateSet.insert(es3);
	const EState* estateptr3=eStateSet.eStatePtr(es3);
	check("add es3 and obtain pointer to es3 from eStateSet and check !=0",estateptr3!=0);
	check("es3 exists in eStateSet",eStateSet.eStateExists(es3));
	check("=> eStateSet.size() == 3",eStateSet.size() == 3);

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

