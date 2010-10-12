#include "variables.h"
#include "ConstrGraph.h"
#include "procSet.h"
#include "mesgExpr.h"
#include "mesgBuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void procSetTest();
void mesgExprTest();

int main(int argc, char** argv)
{
	//procSetTest();
	
	mesgExprTest();
	
	cout << "PASS!\n";
	
	return 0;
}

void mesgExprTest()
{
	varIDSet scalars, arrays;
	vector<varID> vars;
	/*for(int i=0; i<10; i++)
	{ 
		ostringstream outs;
		outs << "V" << i;
		varID var(outs.str());
		scalars.insert(var);
		vars.push_back(var);
	}*/
	
	/*varID rank("rank");
	scalars.insert(rank);
	vars.push_back(rank);*/
	
	scalars.insert(zeroVar);
	
	varID nprocs("nprocs");
	scalars.insert(nprocs);
	vars.push_back(nprocs);
	
	varID i("i");
	scalars.insert(i);
	vars.push_back(i);

	//{
		ConstrGraph cg(scalars, arrays);
		// 1 <= nprocs
		cg.assertCond(zeroVar, nprocs, 1, 1, -1);
		// 1 == i < nprocs
		cg.assertCond(zeroVar, i, 1, 1, -1);
		cg.assertCond(i, zeroVar, 1, 1, 1);
		cg.assertCond(i, nprocs, 1, 1, -1);

		// [i, i]
		contRangeProcSet iRange(i, i, &cg);
		OneDmesgExpr send(iRange);
		cout << "send = "<<send.str()<<"\n";
		cg.transitiveClosure();
		cout << "cg = "<<cg.str()<<"\n";
		cout << "=======================================================\n=======================================================\n=======================================================\n";
	//}
	
	//{
		ConstrGraph cgB(cg);
		OneDmesgExpr sendB(send);
		sendB.mRange.setConstr(&cgB);

		// i = i+1
		cgB.assign(i, i, 1, 1, 1);
		contRangeProcSet iRangeB(i, i, &cgB);
		OneDmesgExpr nextSend(iRangeB);
		cout << "nextSend = "<<nextSend.str()<<"\n";
		cout << "sendB.mustMergeable(nextSend) = "<<sendB.mustMergeable(nextSend)<<"\n";
		cout << "sendB = "<<sendB.str()<<"\n";
		//cout << "cgB = "<<cgB.str()<<"\n";
		sendB.mergeUpd(nextSend);
		
		cout << "sendB = "<<sendB.str()<<"\n";
		cgB.transitiveClosure();
		cout << "cgB = "<<cgB.str()<<"\n";
		cout << "=======================================================\n=======================================================\n=======================================================\n";
	//}
	
	cg.widenUpdate(&cgB);
	cout << "cg = "<<cg.str()<<"\n";
	cout << "=======================================================\n=======================================================\n=======================================================\n";	
	
	//{
		ConstrGraph cgC(cg);
		OneDmesgExpr sendC(send);
		// [0, 0)
		contRangeProcSet recvRange(zeroVar, 1, 1, 0, 
		                           zeroVar, 1, 1, 0, &cgC);
		OneDmesgExpr recvC(recvRange);
		sendC.mRange.setConstr(&cgC);
		
		// i >=nprocs
		cgC.setVal(nprocs, i, 1, 1, 0);
		cgC.transitiveClosure();
		cout << "sendC = "<<sendC.str()<<"\n";
		cout << "recvC = "<<recvC.str()<<"\n";
		cout << "cgC = "<<cgC.str()<<"\n";
		cout << "=======================================================\n=======================================================\n=======================================================\n";
	//}
	
	//{
		mesgBuf mb;
		mb.addMesgExpr(sendC);
		// 0 <= rank < nprocs
		contRangeProcSet rank(zeroVar, 1, 1, 0,
		                      nprocs, 1, 1, -1, &cgC);
		// 0 == sRank 
		contRangeProcSet sRank(zeroVar, 1, 1, 0,
		                       zeroVar, 1, 1, 0, &cgC);
		// 1 <= rRank < nprocs
		contRangeProcSet rRank(zeroVar, 1, 1, 1,
		                       nprocs, 1, 1, -1, &cgC);
		
		procSet *senders, *nonSenders, *receivers, *nonReceivers;
		mesgBuf *nonSendMesgs;
		cout << "mb before = "<<mb.str()<<"\n";
		bool matchSucc = mb.match(sRank, rRank, recvC,
	                             &senders, &nonSenders, &nonSendMesgs,
	                             &receivers, &nonReceivers);
	   cout << "sRank = "<<sRank.str()<<"\n";
	   cout << "rRank = "<<rRank.str()<<"\n";
	   cout << "recvC = "<<recvC.str()<<"\n";
	   cout << "senders = "<<senders->str()<<"\n";
	   cout << "nonSenders = "<<nonSenders->str()<<"\n";
	   if(!nonSenders->emptySet())
	   	cout << "nonSendMesgs = "<<nonSendMesgs->str()<<"\n";
	   cout << "receivers = "<<receivers->str()<<"\n";
	   cout << "nonReceivers = "<<nonReceivers->str()<<"\n";
	   cout << "mb after = "<<mb.str()<<"\n";
	   cout << "cgC = "<<cgC.str()<<"\n";
		cout << "=======================================================\n=======================================================\n=======================================================\n";
	//}
}

void procSetTest()
{
	varIDSet scalars, arrays;
	vector<varID> vars;
	for(int i=0; i<10; i++)
	{ 
		ostringstream outs;
		outs << "V" << i;
		varID var(outs.str());
		scalars.insert(var);
		vars.push_back(var);
	}
	
	ConstrGraph cg(scalars, arrays);
	//cg.assertCond(vars[0], vars[1], 1, 1, 0);
	contRangeProcSet range1(vars[0], vars[1], &cg);
	//cg.assertCond(vars[2], vars[3], 1, 1, 0);
	contRangeProcSet range2(vars[2], vars[3], &cg);
	
//cout << "cg = "<<cg.str()<<"\n";
	ROSE_ASSERT(range1.getConstr() == &cg);
	ROSE_ASSERT(range1.getConstr()->eqVars(range1.getLB(), vars[0]));
	ROSE_ASSERT(range1.getConstr()->eqVars(range1.getUB(), vars[1]));
	ROSE_ASSERT(range2.getConstr() == &cg);
	ROSE_ASSERT(range2.getConstr()->eqVars(range2.getLB(), vars[2]));
	ROSE_ASSERT(range2.getConstr()->eqVars(range2.getUB(), vars[3]));
	
	// ------------------------------------------
	ConstrGraph cg2(scalars, arrays);
	cg2.assertCond(vars[8], vars[9], 1, 1, 0);
	contRangeProcSet range1B(vars[8], vars[9], &cg2);
	range1B = range1;
	ROSE_ASSERT(range1 == range1B);
	
	range1B.emptify();
	ROSE_ASSERT(range1B.emptySet());
	range1B.invalidate();
	ROSE_ASSERT(!range1B.validSet());
	
	range1B.copy(range1);
	ROSE_ASSERT(range1 == range1B);
	range1B.emptify();
	ROSE_ASSERT(range1B.emptySet());
	
	range1B.makeNonEmpty();
	range1B.setConstr(&cg);
	range1B.setLB(range1.getLB());
	range1B.setUB(range1.getUB());
	//cout << "range1 = "<<range1.str()<<"\n";
	//cout << "range1B = "<<range1B.str()<<"\n";
	ROSE_ASSERT(range1 == range1B);
	
	// ------------------------------------------
	range1B.setConstr(&cg2);
	range1B.copyVar(&cg, range1.getLB());
	range1B.copyVar(&cg, range1.getUB());
	range1B.copyVar(&cg, range2.getLB());
	range1B.copyVar(&cg, range2.getUB());
	
	//cout << "cg = "<<cg.str()<<"\n";
	//cout << "cg2 = "<<cg2.str()<<"\n";
	ROSE_ASSERT(cg2.lteVars(range1.getLB(), range1.getUB()));
	ROSE_ASSERT(cg2.lteVars(range2.getLB(), range2.getUB()));
	ROSE_ASSERT(cg2.lteVars(range1B.getLB(), range1B.getUB()));
	
	// ------------------------------------------
	//cout << "range1 = "<<range1.str()<<"\n";
	//cout << "range2 = "<<range2.str()<<"\n";
	
	// [V0 == V2,  V1 == V3]
	{
		ConstrGraph cgB(cg);
		contRangeProcSet range1(vars[0], vars[1], &cgB);
		contRangeProcSet range2(vars[2], vars[3], &cgB);
	
		cgB.assertCond(range1.getLB(), range2.getLB(), 1, 1, 0);
		cgB.assertCond(range2.getLB(), range1.getLB(), 1, 1, 0);
		cgB.assertCond(range1.getUB(), range2.getUB(), 1, 1, 0);
		cgB.assertCond(range2.getUB(), range1.getUB(), 1, 1, 0);
		cgB.transitiveClosure();
		
		contRangeProcSet& range1I2 = dynamic_cast<contRangeProcSet&>(range1.intersect(range2, true));
		/*cout << "range1 = "<<range1.str()<<"\n";
		cout << "range2 = "<<range2.str()<<"\n";
		cout << "range1 I range2 = "<<range1I2.str()<<"\n";
		cout << "cgB = "<<cgB.str()<<"\n";*/
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getLB(), range1.getLB()));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), range1.getUB()));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getLB(), range2.getLB()));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), range2.getUB()));
		ROSE_ASSERT(range1I2.getConstr() == &cgB);
		
		contRangeProcSet& range1R2 = dynamic_cast<contRangeProcSet&>(range1.rem(range2, true));
		//cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		ROSE_ASSERT(range1R2.emptySet());
		
		contRangeProcSet& range2R1 = dynamic_cast<contRangeProcSet&>(range2.rem(range1, true));
		//cout << "range1 - range2 = "<<range2R1.str()<<"\n";
		ROSE_ASSERT(range2R1.emptySet());
		
		/*cg.setToTop();
		cg.assertCond(vars[0], vars[1], 1, 1, 0);
		cg.assertCond(vars[2], vars[3], 1, 1, 0);*/
	}
	
	// [V0 <= V2,  V1 == V3]
	{
		ConstrGraph cgB(cg);
		contRangeProcSet range1(vars[0], vars[1], &cgB);
		contRangeProcSet range2(vars[2], vars[3], &cgB);
		
		cgB.assertCond(range1.getLB(), range2.getLB(), 1, 1, -10);
		cgB.assertCond(range1.getUB(), range2.getUB(), 1, 1, 0);
		cgB.assertCond(range2.getUB(), range1.getUB(), 1, 1, 0);
		cgB.transitiveClosure();
		
		contRangeProcSet& range1I2 = dynamic_cast<contRangeProcSet&>(range1.intersect(range2, true));
		//cout << "range1 I range2 = "<<range1I2.str()<<"\n";
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getLB(), vars[2]));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), vars[1]));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), vars[3]));
		ROSE_ASSERT(range1I2.getConstr() == &cgB);
		
		contRangeProcSet& range1R2 = dynamic_cast<contRangeProcSet&>(range1.rem(range2, true));
		/*cout << "range1 = "<<range1.str()<<"\n";
		cout << "range2 = "<<range2.str()<<"\n";
		cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		cout << "cgB = "<<cgB.str()<<"\n";*/
		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getLB(), vars[0]));
		ROSE_ASSERT(range1R2.getConstr()->ltVars(range1R2.getUB(), vars[2]));
		ROSE_ASSERT(range1R2.getConstr()->ltVars(range1R2.getUB(), range2.getLB()));
		ROSE_ASSERT(range1R2.getConstr() == &cgB);
		
		contRangeProcSet& range2R1 = dynamic_cast<contRangeProcSet&>(range2.rem(range1, true));
		//cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		ROSE_ASSERT(range2R1.emptySet());
		
		/*cg.setToTop();
		cg.assertCond(vars[0], vars[1], 1, 1, 0);
		cg.assertCond(vars[2], vars[3], 1, 1, 0);*/
	}
	
	// [V0 == V2, V1 <= V3]
	{
		ConstrGraph cgB(cg);
		contRangeProcSet range1(vars[0], vars[1], &cgB);
		contRangeProcSet range2(vars[2], vars[3], &cgB);
		
		cgB.assertCond(range1.getLB(), range2.getLB(), 1, 1, 0);
		cgB.assertCond(range2.getLB(), range1.getLB(), 1, 1, 0);
		cgB.assertCond(range1.getUB(), range2.getUB(), 1, 1, 0);
		cgB.transitiveClosure();
		
		contRangeProcSet& range1I2 = dynamic_cast<contRangeProcSet&>(range1.intersect(range2, true));
		//cout << "range1 I range2 = "<<range1I2.str()<<"\n";
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getLB(), range1.getLB()));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), range1.getUB()));
		ROSE_ASSERT(range1I2.getConstr() == &cgB);
		
		contRangeProcSet& range1R2 = dynamic_cast<contRangeProcSet&>(range1.rem(range2, true));
		//cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		ROSE_ASSERT(range1R2.emptySet());
		
		contRangeProcSet& range2R1 = dynamic_cast<contRangeProcSet&>(range2.rem(range1, true));
		ROSE_ASSERT(range2R1.getConstr()->ltVars(range1.getUB(), range2R1.getLB()));
		ROSE_ASSERT(range2R1.getConstr()->eqVars(range2R1.getUB(), vars[3]));
		ROSE_ASSERT(range2R1.getConstr()->eqVars(range2R1.getUB(), range2.getUB()));
		ROSE_ASSERT(range1R2.getConstr() == &cgB);
		
		/*cg.setToTop();
		cg.assertCond(vars[0], vars[1], 1, 1, 0);
		cg.assertCond(vars[2], vars[3], 1, 1, 0);*/
	}
	
	// [V0, V1] < [V2, V3]
	{
		ConstrGraph cgB(cg);
		contRangeProcSet range1(vars[0], vars[1], &cgB);
		contRangeProcSet range2(vars[2], vars[3], &cgB);
		
		cgB.assertCond(range1.getUB(), range2.getLB(), 1, 1, -1);
		cgB.transitiveClosure();
		
		contRangeProcSet& range1I2 = dynamic_cast<contRangeProcSet&>(range1.intersect(range2, true));
		//cout << "range1 I range2 = "<<range1I2.str()<<"\n";
		ROSE_ASSERT(range1I2.emptySet());
		
		contRangeProcSet& range1R2 = dynamic_cast<contRangeProcSet&>(range1.rem(range2, true));
		/*cout << "range1 = "<<range1.str()<<"\n";
		cout << "range2 = "<<range2.str()<<"\n";
		cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		cout << "cgB = "<<cgB.str()<<"\n";*/
 		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getLB(), vars[0]));
		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getUB(), vars[1]));
		ROSE_ASSERT(range1R2.getConstr() == &cgB);
		/*cg.setToTop();
		cg.assertCond(vars[0], vars[1], 1, 1, 0);
		cg.assertCond(vars[2], vars[3], 1, 1, 0);*/
	}
	
	// V2 <= V0 <= V3 <= V1
	{
		ConstrGraph cgB(cg);
		contRangeProcSet range1(vars[0], vars[1], &cgB);
		contRangeProcSet range2(vars[2], vars[3], &cgB);
		
		cgB.assertCond(range2.getLB(), range1.getLB(), 1, 1, -1);
		cgB.assertCond(range1.getLB(), range2.getUB(), 1, 1, 0);
		cgB.assertCond(range2.getUB(), range1.getUB(), 1, 1, -20);
		cgB.transitiveClosure();
		
		contRangeProcSet& range1I2 = dynamic_cast<contRangeProcSet&>(range1.intersect(range2, true));
		//cout << "range1 I range2 = "<<range1I2.str()<<"\n";
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getLB(), vars[0]));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), vars[3]));
		ROSE_ASSERT(range1I2.getConstr() == &cgB);
		
		contRangeProcSet& range1R2 = dynamic_cast<contRangeProcSet&>(range1.rem(range2, true));
		//cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		ROSE_ASSERT(range1R2.getConstr()->ltVars(vars[0], range1R2.getLB()));
		ROSE_ASSERT(range1R2.getConstr()->ltVars(range1.getLB(), range1R2.getLB()));
		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getUB(), vars[1]));
		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getUB(), range1.getUB()));
		ROSE_ASSERT(range1R2.getConstr() == &cgB);
		
		contRangeProcSet& range2R1 = dynamic_cast<contRangeProcSet&>(range2.rem(range1, true));
		//cout << "range1 - range2 = "<<range2R1.str()<<"\n";
		ROSE_ASSERT(range2R1.getConstr()->ltVars(range2R1.getUB(), vars[3]));
		ROSE_ASSERT(range2R1.getConstr()->ltVars(range2R1.getUB(), range2.getUB()));
		ROSE_ASSERT(range2R1.getConstr()->eqVars(range2R1.getLB(), vars[2]));
		ROSE_ASSERT(range2R1.getConstr()->eqVars(range2R1.getLB(), range2.getLB()));
		ROSE_ASSERT(range2R1.getConstr() == &cgB);
		
		
		/*cg.setToTop();
		cg.assertCond(vars[0], vars[1], 1, 1, 0);
		cg.assertCond(vars[2], vars[3], 1, 1, 0);*/
	}
	
	// V0 <= V2 <= V1 <= V3
	{
		ConstrGraph cgB(cg);
		contRangeProcSet range1(vars[0], vars[1], &cgB);
		contRangeProcSet range2(vars[2], vars[3], &cgB);
		
		cgB.assertCond(range1.getLB(), range2.getLB(), 1, 1, -1);
		cgB.assertCond(range2.getLB(), range1.getUB(), 1, 1, 0);
		cgB.assertCond(range1.getUB(), range2.getUB(), 1, 1, -20);
		cgB.transitiveClosure();
		
		contRangeProcSet& range1I2 = dynamic_cast<contRangeProcSet&>(range1.intersect(range2, true));
		//cout << "range1 I range2 = "<<range1I2.str()<<"\n";
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getLB(), vars[2]));
		ROSE_ASSERT(range1I2.getConstr()->eqVars(range1I2.getUB(), vars[1]));
		ROSE_ASSERT(range1I2.getConstr() == &cgB);
		
		contRangeProcSet& range1R2 = dynamic_cast<contRangeProcSet&>(range1.rem(range2, true));
		//cout << "range1 - range2 = "<<range1R2.str()<<"\n";
		ROSE_ASSERT(range1R2.getConstr()->ltVars(range1R2.getUB(), vars[2]));
		ROSE_ASSERT(range1R2.getConstr()->ltVars(range1R2.getUB(), range2.getLB()));
		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getLB(), vars[0]));
		ROSE_ASSERT(range1R2.getConstr()->eqVars(range1R2.getLB(), range1.getLB()));
		ROSE_ASSERT(range1R2.getConstr() == &cgB);
		
		contRangeProcSet& range2R1 = dynamic_cast<contRangeProcSet&>(range2.rem(range1, true));
		//cout << "range1 - range2 = "<<range2R1.str()<<"\n";
		ROSE_ASSERT(range2R1.getConstr()->ltVars(vars[1], range2R1.getLB()));
		ROSE_ASSERT(range2R1.getConstr()->ltVars(range1.getUB(), range2R1.getLB()));
		ROSE_ASSERT(range2R1.getConstr()->eqVars(range2R1.getUB(), vars[3]));
		ROSE_ASSERT(range2R1.getConstr()->eqVars(range2R1.getUB(), range2.getUB()));
		ROSE_ASSERT(range2R1.getConstr() == &cgB);
		
		/*cg.setToTop();
		cg.assertCond(vars[0], vars[1], 1, 1, 0);
		cg.assertCond(vars[2], vars[3], 1, 1, 0);*/
	}
}
