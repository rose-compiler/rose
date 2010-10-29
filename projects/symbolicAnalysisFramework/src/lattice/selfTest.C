#include "intArithLogical.h"
#include "ConstrGraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

void intArithLogicalTest();
void ConstrGraphTest();
void mergeAnnotVarsTest();

int main(int argc, char** argv)
{
	// intArithLogicalTest();
	//ConstrGraphTest();
	
/*	pCFG_contProcMatchAnalysis::mergePCFGStates
	int numPSets = 10;
	list<int> pSetsToMerge;
	pSetsToMerge.push_back(0);
	pSetsToMerge.push_back(1);
	pSetsToMerge.push_back(2);
	pSetsToMerge.push_back(3);
	pSetsToMerge.push_back(4);
	pSetsToMerge.push_back(5);
	pSetsToMerge.push_back(6);
	pSetsToMerge.push_back(7);
	pSetsToMerge.push_back(8);
	pSetsToMerge.push_back(9);
	
	
	list<int>::const_iterator it=pSetsToMerge.begin();
	it++;
	int freePSet=*it;
	int usedPSet=freePSet;
	while(freePSet<(numPSets - (pSetsToMerge.size()-1)))
	{	
		// Find the next process set id that is still going to be used
		int nextHole;
		do
		{
			usedPSet++;
			it++;
			if(it==pSetsToMerge.end()) nextHole = numPSets;
			else                       nextHole = *it;
		} while(usedPSet==nextHole);
		
		// usedPSet now refers to the next process set id that will be used
		
		// Move the next few used process set ids to the free spots left by merged process sets
		for(; usedPSet<nextHole; usedPSet++, freePSet++)
		{
			cg->rename(usedPSet, freePSet);
			pSetMigrations[usedPSet] = freePSet;
			cout << "migrating "<<usedPSet<<" to "<<freePSet<<"\n";
		}
	}*/
	
	return 0;
}

int recordMaxC(map< pair<int, int>, int>& maxC, int i, int j)
{
	pair<int, int> p(i, j);
	int r=rand()%100; 
	if(maxC.find(p)==maxC.end()) 
	{
		//printf("recordMaxC old %d,%d => none\n", i, j);
		maxC[p]=r;
	}
	else
	{
		//printf("recordMaxC old %d,%d => %d\n", i, j, maxC[p]);
		maxC[p]=max(maxC[p], r);
	}
	//printf("recordMaxC new %d,%d => %d\n", i, j, maxC[p]);
	return r;	
}

void ConstrGraphTest()
{
	srand(time(NULL));
	
	mergeAnnotVarsTest();

/*	vector<varID> vars;
	for(int i=0; i<10; i++)
	{
		ostringstream outs; outs << "var_" << i;
		varID var(outs.str());
		var.addAnnotation("mod2", (void*)(i%2));
		var.addAnnotation("div2", (void*)(i/2));
		vars.push_back(var);
	}
	
	ConstrGraph cg;
	cg.addScalar(zeroVar);
	for(vector<varID>::iterator it=vars.begin(); it!=vars.end(); it++)
	{
		cg.addScalar(*it);
	}*/

	// for the even vars, make var_i = i
	/*for(int i=0; i<10; i+=2)
	{
		ostringstream outs; outs << "var_" << i;
		varID var(outs.str()); var.addAnnotation("annot", (void*)(i%2));
		cg.assertEq(var, zeroVar, 1, 1, i);
	}
	
	// for the odd vars, make var_i > var_i-1
	for(int i=1; i<10; i+=2)
	{
		ostringstream outsi; outsi << "var_" << i;
		varID vari(outsi.str()); vari.addAnnotation("annot", (void*)(i%2));
		ostringstream outsiM1; outsiM1 << "var_" << (i-1);
		varID variM1(outsiM1.str()); variM1.addAnnotation("annot", (void*)(i%2));

		cg.assertCond(variM1, vari, 1, 1, -1);
	}
	
	cg.transitiveClosure();
	cout << cg.str() << "\n";
	
	cout << "ZERO <= \n";
	for(ConstrGraph::leIterator it = cg.leBegin(zeroVar); it!=cg.leEnd(); it++)
	{
		varAffineInequality vai = *it;
		cout << "    vai = "<<vai.str()<<"\n";
	}
	
	varID var9("var_9");
	cout << "<= var9\n";
	for(ConstrGraph::geIterator it = cg.geBegin(var9); it!=cg.geEnd(); it++)
	{
		varAffineInequality vai = *it;
		cout << "    vai = "<<vai.str()<<"\n";
	}
	
	set<pair<string, void*> > noCopyAnnots;
	cg.copyAnnotVars("annot", (void*)0, "greg", (void*)1, noCopyAnnots);
	cout << cg.str() << "\n";* /
	
	ConstrGraph cg2;
	cg2.addScalar(zeroVar);
	
	for(int i=10; i<12; i++)
	{
		ostringstream outs; outs << "var_" << i;
		varID var(outs.str());
		var.addAnnotation("mod2", (void*)0);
		var.addAnnotation("div2", (void*)5);
		vars.push_back(var);
	}
	
	for(int i=0; i<vars.size(); i++)
	{
		cg2.addScalar(vars[i]);
		if(i==0)
			cg2.assertCond(zeroVar, vars[i], 1, 1, 0);
		else
			cg2.assertCond(vars[i-1], vars[i], 1, 1, 0);
	}
	cout << cg2.str() << "\n";
	
	cout << "-----------------\n";
	
	set<pair<string, void*> > noCopyAnnots;
	pair<string, void*> a("div2", (void*)3);
	noCopyAnnots.insert(a);
	cg2.copyAnnotVars("mod2", (void*)0, "greg", (void*)0xcc, noCopyAnnots);
	cout << cg2.str() << "\n";*/
}

void mergeAnnotVarsTest()
{
	int numVars = 10;
	
	/*varID raga("raga");
	raga.addAnnotation("A", (void*)1);
	raga.addAnnotation("B", (void*)2);
	raga.swapAnnotations("A", (void*)1, "B", (void*)10);*/

	ConstrGraph cg;
	cg.addScalar(zeroVar);
	
	vector<varID> varsA, varsB;
	for(int i=0; i<numVars; i++)
	{
		ostringstream outs; outs << "var_" << i;
		varID varA(outs.str());
		varA.addAnnotation("A", (void*)1);
		cg.addScalar(varA);
		varsA.push_back(varA);		
		
		varID varB(outs.str());
		varB.addAnnotation("B", (void*)1);
		cg.addScalar(varB);
		varsB.push_back(varB);
		
		//cg.assertCond(varA, varB, 1, 1, 0);
		//cg.assertCond(varB, varA, 1, 1, 0);
	}
	
	map< pair<int, int>, int> maxC;
	int r;
	for(int i=0; i<numVars-1; i++)
	{
		r = recordMaxC(maxC, i, i+1);	
		cg.assertCond(varsA[i], varsA[i+1], 1, 1, r);
		r = recordMaxC(maxC, i, i+1);
		cg.assertCond(varsB[i], varsB[i+1], 1, 1, r);
		if(i+2<numVars)
		{
			r = recordMaxC(maxC, i, i+2);
			cg.assertCond(varsA[i], varsB[i+2], 1, 1, r);
			r = recordMaxC(maxC, i, i+2);
			cg.assertCond(varsB[i], varsA[i+2], 1, 1, r);
			r = recordMaxC(maxC, i, i+2);
			cg.assertCond(varsA[i], varsA[i+2], 1, 1, r);
			r = recordMaxC(maxC, i, i+2);
			cg.assertCond(varsB[i], varsB[i+2], 1, 1, r);
		}
		
		if(i-3>=0)
		{
			r = recordMaxC(maxC, i-3, i);
			cg.assertCond(varsA[i-3], varsB[i], 1, 1, r);
			r = recordMaxC(maxC, i-3, i);
			cg.assertCond(varsB[i-3], varsA[i], 1, 1, r);
			r = recordMaxC(maxC, i-3, i);
			cg.assertCond(varsA[i-3], varsA[i], 1, 1, r);
			r = recordMaxC(maxC, i-3, i);
			cg.assertCond(varsB[i-3], varsB[i], 1, 1, r);
		}
	}
	
	//cout << "cg="<<cg.str("    ")<<"\n";;
	
	const set<pair<string, void*> > noCopyAnnots;
	set<varID> noCopyVars;
	cg.mergeAnnotVars("A", (void*)1, "B", (void*)1, noCopyAnnots, noCopyVars);
	
	//cout << "cg="<<cg.str("    ")<<"\n";;
	
	for(map< pair<int, int>, int>::iterator it=maxC.begin(); it!=maxC.end(); it++)
	{
		//cout << "("<<varsA[it->first.first].str()<<","<<varsA[it->first.second].str()<<") => "<<it->second<<" vs "<<cg.getVal(varsA[it->first.first], varsA[it->first.second])->getC()<<"\n";
		ROSE_ASSERT(cg.getVal(varsA[it->first.first], varsA[it->first.second])!=NULL);
		ROSE_ASSERT(cg.getVal(varsA[it->first.first], varsB[it->first.second])==NULL);
		ROSE_ASSERT(cg.getVal(varsB[it->first.first], varsA[it->first.second])==NULL);
		ROSE_ASSERT(cg.getVal(varsB[it->first.first], varsB[it->first.second])==NULL);
		ROSE_ASSERT(it->second == cg.getVal(varsA[it->first.first], varsA[it->first.second])->getC());
		
	}	
}

void intArithLogicalTest()
{
	varID x("x");
	varID y("y");
	varID z("z");
	varID w("w");
/*	IntArithLogical ialXY1A(IntArithLogical::le, 5, x, 3, y, 10); // 5x <= 3y + 1
	cout << "ialXY1A = "<<ialXY1A.str("") << "\n";
	
//ialXY1A.isImplied(IntArithLogical::le, 5, x, 3, y, 10);
	
        IntArithLogical ialXY1B(IntArithLogical::le, 5, x, 3, y, 10); // 5x <= 3y + 1
	cout << "ialXY1B = "<<ialXY1B.str("") << "\n";
	IntArithLogical ialXY2(IntArithLogical::le, 6, x, 3, y, 20); // 5x <= 3y + 20
	cout << "ialXY2 = "<<ialXY2.str("") << "\n";
	IntArithLogical ialXY3(IntArithLogical::le, 5, x, 3, y, 1); // 5x <= 3y + 20
	cout << "ialXY3 = "<<ialXY3.str("") << "\n";
	
	IntArithLogical ialXY4(IntArithLogical::le, 5, x, 3, y, 99); // 5x > 3y + 99
	ialXY4.notUpd();
	cout << "ialXY4 = "<<ialXY4.str("") << "\n";
	
	IntArithLogical ialWZ1(IntArithLogical::eq, 5, w, 3, z, 1); // 5x == 3y + 20
	cout << "ialWZ1 = "<<ialWZ1.str("") << "\n";

	IntArithLogical ialWZ2(IntArithLogical::eq, 5, w, 3, z, 1); // 5x 1= 3y + 20
	ialWZ2.notUpd();
	cout << "ialWZ2 = "<<ialWZ2.str("") << "\n";

	printf("XY1A | XY2\n");
	ialXY1A.orUpd(ialXY2);
	cout << ialXY1A.str("") << "\n";
	
	printf("XY1A & XY3\n");
	ialXY1A.andUpd(ialXY3);
	cout << ialXY1A.str("") << "\n";
	
	printf("XY1A & XY4\n");
	ialXY1A.andUpd(ialXY4);
	cout << ialXY1A.str("") << "\n";

	printf("XY1B & XY2\n");
	ialXY1B.andUpd(ialXY2);
	cout << ialXY1B.str("") << "\n";
	
	printf("XY1B | WZ1\n");
	ialXY1B.andUpd(ialWZ1);
	cout << ialXY1B.str("") << "\n";
	
	printf("XY1A | WZ2\n");
	ialXY1A.andUpd(ialWZ2);
	cout << ialXY1A.str("") << "\n";

	IntArithLogical ialXY1C(ialXY1A);

	printf("XY1A & XY1B\n");
	ialXY1A.andUpd(ialXY1B);
	cout << ialXY1A.str("") << "\n";
	
	printf("XY1C | XY1B\n");
	ialXY1C.orUpd(ialXY1B);
	cout << ialXY1C.str("") << "\n";
	
	ialXY1C.isImplied(IntArithLogical::le, 5, x, 3, y, 10);

	IntArithLogical ialXY(IntArithLogical::le, 1, x, 1, y, 0); // x <= y
	IntArithLogical ialYZ(IntArithLogical::le, 1, y, 1, z, 6); // y <= z+6
	IntArithLogical ialXZ(IntArithLogical::le, 1, x, 1, z, 0); // x <= z+5
	
	ialXY.andUpd(ialYZ);
	
	cout << "ialXY = "<<ialXY.str("") << "\n";
	cout << "ialXZ = "<<ialXZ.str("") << "\n";
	
	if(ialXY.isImplied(IntArithLogical::le, 1, x, 1, z, 10)) cout << "Implied\n"; 
	else cout << "Not Implied\n"; 
		
	if(ialXY.isImplied(IntArithLogical::le, 1, x, 1, z, 6)) cout << "Implied\n"; 
	else cout << "Not Implied\n"; 
		
	if(ialXY.isImplied(IntArithLogical::le, 1, x, 1, z, 0)) cout << "Implied\n"; 
	else cout << "Not Implied\n"; */
		
	IntArithLogical ialXY1(IntArithLogical::le, 5, x, 3, y, 10); // 5x <= 3y + 10
	cout << ialXY1.str("") << "\n";
	if(ialXY1.mayTrue()) cout << "Consistent\n"; 
	else cout << "Not Consistent\n";
		
	cout << "===============================\n";
	
	/*IntArithLogical ialXY2(IntArithLogical::le, 5, x, 3, y, 10); // 5x <= 3y + 10
	ialXY2.notUpd();
	cout << ialXY2.str("") << "\n";
	ialXY1.andUpd(ialXY2);
	
	cout << ialXY1.str("") << "\n";
	
	if(ialXY1.mayTrue()) cout << "Consistent\n"; 
	else cout << "Not Consistent\n";
	*/
	cout << "===============================\n";

	IntArithLogical ialXY3(IntArithLogical::le, 5, x, 3, y, 10); // 5x <= 3y + 10		
	IntArithLogical ialYZ1(IntArithLogical::le, 1, y, 1, z, 1); // y <= z + 1
	IntArithLogical ialYW1(IntArithLogical::eq, 1, y, 2, w, 1); // y <= 2*w + 1
	IntArithLogical ialWZ1(IntArithLogical::eq, 1, w, 1, z, 0); // w!=z
	ialWZ1.notUpd();
	
	ialXY3.andUpd(ialYZ1);
	ialXY3.andUpd(ialYW1);
	ialXY3.andUpd(ialWZ1);
	
	cout << ialXY3.str("") << "\n";
	
	ialXY3.removeVar(y);
	
	cout << "Removed y\n";
	cout << ialXY3.str("") << "\n";
	
	IntArithLogical ialXZ1(IntArithLogical::eq, 5, x, 3, z, 19); // 5x <=3z + 19
	cout << ialXZ1.str("") << "\n";
	if(ialXY3.isImplied(IntArithLogical::le, 5, x, 3, z, 19)) cout << "Implied\n"; 
	else cout << "Not Implied\n";
}
