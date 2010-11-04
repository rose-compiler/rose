#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>

#include "common.h"
#include "variables.h"
#include "varBitVector.h"
using namespace std;
//using namespace VarBitVector;

int main(int argc, char** argv)
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	int ret=0;
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for(Rose_STL_Container<SgNode*>::const_iterator f = functions.begin();
	    f != functions.end(); ++f) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*f);
		ROSE_ASSERT(curFunc);
				
		//SgBasicBlock *funcBody = curFunc->get_body();
		varIDSet allVars = getVarRefsInSubtree(curFunc);
			
		{
			ROSE_ASSERT(allVars.size()>=8);
			
			bool modified;
			int i;
			VarBitVector vbv0_mod3(allVars);
			VarBitVector vbv1_mod3(allVars);
			VarBitVector vbv2_mod3(allVars);
			VarBitVector vbvEven(allVars);
			VarBitVector vbvOdd(allVars);
			VarBitVector vbvAll1(allVars);
			VarBitVector bot(allVars);
			VarBitVector top(allVars);
			
			{
				int mod2=0;
				int mod3=0;
				m_quad2str::iterator it;
				// map all variables to 0
				for(varIDSet::iterator it = allVars.begin(); it != allVars.end(); it++)
				{
					if(mod3==0)
					{
						vbv0_mod3.setVar(*it, true);
						vbv1_mod3.setVar(*it, false);
						vbv2_mod3.setVar(*it, false);
					}
					else if(mod3==1)
					{
						vbv1_mod3[*it] = true;
						vbv0_mod3.setVar(*it, false);
						vbv2_mod3.setVar(*it, false);
					}
					else if(mod3==2)
					{
						vbv2_mod3.setVar(*it, true);
						vbv0_mod3.setVar(*it, false);
						vbv1_mod3.setVar(*it, false);
					}
					
					if(mod2==0)
					{
						vbvEven.setVar(*it, true);
						vbvOdd.setVar(*it, false);
					}
					else
					{
						vbvOdd.setVar(*it, true);
						vbvEven.setVar(*it, false);
					}
					vbvAll1[*it] = true;
		
					mod3=(mod3+1)%3;
					mod2=(mod2+1)%2;
				}
			}
			bot.setToBot();
			top.setToTop();
			
			// setToTop and setTopBot modification functionality
			modified = bot.setToBot();
			if(modified) {printf("ERROR: changing bottom to bottom modifies it !\n"); ret++;}
			modified = vbvAll1.setToTop();
			if(modified) {printf("ERROR: changing top to top modifies it!\n"); ret++;}
			modified = bot.setToTop();
			if(!modified) {printf("ERROR: changing bottom to top does not modify it!\n"); ret++;}
			modified = bot.setToBot();
			if(!modified) {printf("ERROR: changing top to bottom does not modify it!\n"); ret++;}
			VarBitVector tmp(vbvEven);
			modified = tmp.setToBot();
			if(!modified) {printf("ERROR: changing vbvEven to bottom does not modify it!\n"); ret++;}
			
			
			VarBitVector topA(top), topB(top);
			VarBitVector botA(bot), botB(bot);
			// set setVal and its modification bit functionality
			i=0;
			for(varIDSet::iterator it = allVars.begin(); it != allVars.end(); it++, i++)
			{
				if(i%2==0)
				{
					topA[*it]=false;
					if(!topB.setVar(*it, false)) {printf("ERROR: setting index %d of topB to false doesn't change it!\n", i); ret++;}
					botA[*it]=true;
					if(!botB.setVar(*it, true)) {printf("ERROR: setting index %d of botB to true doesn't change it!\n", i); ret++;}
				}
				else
				{
					if(topB.setVar(*it, true)) {printf("ERROR: setting index %d of topB to true changes it!\n", i); ret++;}
					if(botB.setVar(*it, false)) {printf("ERROR: setting index %d of botB to false changes it!\n", i); ret++;}	
				}
			}
			if(botA != vbvEven) {printf("ERROR: botA != vbvEven!\n"); ret++;}
			if(botB != vbvEven) {printf("ERROR: botB != vbvEven!\n"); ret++;}
			if(topA != vbvOdd) {printf("ERROR: topA != vbvOdd!\n"); ret++;}
			if(topB != vbvOdd) {printf("ERROR: topB != vbvOdd!\n"); ret++;}

			VarBitVector* topUbotA = topA.unionVec(botB);
			VarBitVector* topUbotB = topB.unionVec(botA);
			if((*topUbotA) != top) {printf("ERROR: (*topUbotA) != top!\n"); ret++;}
			if((*topUbotB) != top) {printf("ERROR: (*topUbotB) != top!\n"); ret++;}
			VarBitVector* topIbotA = topA.intersectVec(botB);
			VarBitVector* topIbotB = topB.intersectVec(botA);
			if((*topIbotA) != bot) {printf("ERROR: (*topIbotA) != bot!\n"); ret++;}
			if((*topIbotB) != bot) {printf("ERROR: (*topIbotB) != bot!\n"); ret++;}
					
			if(bot == top) {printf("ERROR: bot==top!\n"); ret++;}
			if(vbv0_mod3 == vbv1_mod3) {printf("ERROR: vbv0_mod3==vbv1_mod3!\n"); ret++;}
			if(vbv0_mod3 == vbv2_mod3) {printf("ERROR: vbv0_mod3==vbv2_mod3!\n"); ret++;}
			if(!(vbv2_mod3 != vbv1_mod3)) {printf("ERROR: !(vbv2_mod3 != vbv1_mod3)!\n"); ret++;}
			if(top!=vbvAll1) {printf("ERROR: top!=vbvAll1\n"); ret++;}
			
			VarBitVector* vbv01_mod3 = vbv0_mod3.unionVec(vbv1_mod3);
			VarBitVector* vbv12_mod3 = vbv1_mod3.unionVec(vbv2_mod3);
			VarBitVector* vbv012_mod3 = vbv01_mod3->unionVec(vbv2_mod3);
			
			if(*vbv01_mod3 == *vbv12_mod3) {printf("ERROR: vbv01_mod3 == vbv12_mod3!\n"); ret++;}	
			if(!(*vbv01_mod3 != *vbv012_mod3)) {printf("ERROR: !(vbv01_mod3 != vbv012_mod3)!\n"); ret++;}	
			if(*vbv12_mod3 == *vbv012_mod3) {printf("ERROR: vbv12_mod3 == vbv012_mod3!\n"); ret++;}	
			if(top != *vbv012_mod3) {printf("ERROR: top != vbv012_mod3!\n"); ret++;}
				
			VarBitVector* vbv01_mod3_B = VarBitVector::unionVec(vbv0_mod3, vbv1_mod3);
			VarBitVector* vbv12_mod3_B = VarBitVector::unionVec(vbv1_mod3, vbv2_mod3);
			VarBitVector* vbv012_mod3_B = VarBitVector::unionVec(*vbv12_mod3, vbv0_mod3);
		
			if(!(*vbv01_mod3  == *vbv01_mod3_B))  {printf("ERROR: !(vbv01_mod3 == vbv01_mod3_B)!\n"); ret++;}
			if(*vbv12_mod3  != *vbv12_mod3_B)  {printf("ERROR: vbv12_mod3 != vbv12_mod3_B!\n"); ret++;}
			if(*vbv012_mod3 != *vbv012_mod3_B) {printf("ERROR: vbv012_mod3 != vbv012_mod3_B!\n"); ret++;}
		
			VarBitVector* vbv0I1_mod3 = vbv0_mod3.intersectVec(vbv1_mod3);
			VarBitVector* vbv1I2_mod3 = vbv1_mod3.intersectVec(vbv2_mod3);
			VarBitVector* vbv2I0_mod3 = vbv2_mod3.intersectVec(vbv0_mod3);
			
			if(*vbv0I1_mod3 != bot) {printf("ERROR: vbv0I1_mod3 != bot!\n"); ret++; }	
			if(*vbv1I2_mod3 != bot) {printf("ERROR: vbv1I2_mod3 != bot!\n"); ret++;}	
			if(!(*vbv2I0_mod3 == bot)) {printf("ERROR: !(vbv2I0_mod3 == bot)!\n"); ret++;}	
				
			modified = vbv0_mod3.intersectUpdate(top);
			if(modified){ printf("Error: intersecting vbv0_mod3 with top causes a modification!\n"); ret++; }
			modified = vbv012_mod3_B->intersectUpdate(top);
			if(modified){ printf("Error: intersecting vbv012_mod3_B with top causes a modification!\n"); ret++; }
			modified = vbv0_mod3.unionUpdate(bot);
			if(modified){ printf("Error: unioning vbv0_mod3 with bot causes a modification!\n"); ret++; }
			modified = vbv0_mod3.unionUpdate(vbv1_mod3);
			if(!modified){ printf("Error: unioning vbv0_mod3 with vbv1_mod3 does not cause a modification!\n"); ret++; }
			tmp.copy(vbv0_mod3);
			modified = tmp.intersectUpdate(vbv1_mod3);
			if(!modified){ printf("Error: intersection vbv0_mod3 with vbv1_mod3 does not cause a modification!\n"); ret++; }
			VarBitVector* tmp2 = VarBitVector::intersectVec(vbv1_mod3, vbv0_mod3);
			if(tmp!=(*tmp2)){ printf("Error: two ways of computing intersection of vbv0_mod3 and vbv1_mod3 are not equal!\n"); ret++; }
			
			tmp.copy(bot);
			modified = tmp.unionUpdate(vbvAll1);                                                                  
			if(!modified){ printf("Error! unioning bottom with top does not cause bottom to be modified!\n"); ret++; }
			
			modified = top.unionUpdate(vbvAll1);                                                                  
			if(modified){ printf("Error! unioning top with top causes bottom to be modified!\n"); ret++; }
			
			tmp.copy(vbv1_mod3);
			if(tmp != vbv1_mod3) { printf("Error! A copy of vbv1_mod3 is not equal to vbv1_mod3!\n"); ret++; }
			
			tmp.copy(bot);
			VarBitVector* vbv0U1 = tmp.unionVec(vbv1_mod3);
			VarBitVector* vbv0U1_copy = new VarBitVector(vbv0U1);
			if((*vbv0U1) != (*vbv0U1_copy)){ printf("Error! Two (vbv0 U vbv1_mod3) vectors are not equal!\n"); ret++; }
			
			VarBitVector* vbvTop = new VarBitVector(allVars);
			vbvTop->setToTop();
			if(!vbvTop->isTop()){ printf("Error! The top vector is not top!\n"); ret++;}
			if(!vbvAll1.isTop()){ printf("Error! The vbvAll1 vector is not top!\n"); ret++;}
			if(!vbv012_mod3->isTop()){ printf("Error! The vbv012_mod3 vector is not top!\n"); ret++;}
			
			modified = vbvTop->copy(vbvAll1);
			if(modified){ printf("Error! Top was modified when copied from top!\n"); ret++; }
			
			vbv2I0_mod3->setToBot();
			if(!vbv2I0_mod3->isBot()) {printf("Error! The bottom vector is not bottom!\n"); ret++; }
			if(!vbv0I1_mod3->isBot()) {printf("ERROR: vbv0I1_mod3 is not bottom!\n"); ret++;}	
			if(!vbv1I2_mod3->isBot()) {printf("ERROR: vbv1I2_mod3 is not bottom!\n"); ret++;}	
			if(!vbv2I0_mod3->isBot()) {printf("ERROR: vbv2I0_mod3 is not bottom!\n"); ret++;}	
			
			tmp.copy(bot);
			modified = tmp.copy(vbvAll1);
			if(!modified){ printf("Error! Bottom was not modified when copied from top.\n"); ret++; }
			
			VarBitVector* vbv1_3Ieven = VarBitVector::intersectVec(vbv1_mod3, vbvEven);
			// verify that the intersection maps to true all the variables with indexes = 4 (mod 6) and to false otherwise
			i=0;
			for(varIDSet::iterator it = allVars.begin(); it != allVars.end(); it++, i++)
				if(i%6==4)
				{
					if(((*vbv1_3Ieven)[*it])==VarBitVector::False){ printf("ERROR: The %d-th variable in vbv1_3Ieven is false!\n", i); ret++; }
				}
				else if(((*vbv1_3Ieven)[*it])==VarBitVector::True){ printf("ERROR: The %d-th variable in vbv1_3Ieven is true!\n", i); ret++; }
			
			VarBitVector* vbv0_3Ueven = VarBitVector::unionVec(vbv0_mod3, vbvEven);
			VarBitVector* vbv01_3Ueven = VarBitVector::unionVec(vbv1_mod3, *vbv0_3Ueven);			
			// verify that the union maps to false all the variables with indexes = 5 (mod 6) and to true otherwise
			i=0;
			for(varIDSet::iterator it = allVars.begin(); it != allVars.end(); it++, i++)
				if(i%6==5)
				{
					if((*vbv01_3Ueven)[*it]==VarBitVector::True){ printf("ERROR: The %d-th variable in vbv01_3Ueven is true!\n", i); ret++; }
				}
				else if((*vbv01_3Ueven)[*it]==VarBitVector::False){ printf("ERROR: The %d-th variable in vbv01_3Ueven is false!\n", i); ret++; }
			
			// Tests of initialization behavior
			VarBitVector* vbvUnini0 = new VarBitVector(vbvTop, false);
			VarBitVector* vbvUnini1 = new VarBitVector(*vbvTop, false);
			VarBitVector* vbvUnini2 = new VarBitVector(vbvTop, false);
			VarBitVector* vbvUnini3 = new VarBitVector(*vbvTop, false);
			VarBitVector* vbvUnini4 = new VarBitVector(vbvTop, false);
			VarBitVector* vbvUnini5 = new VarBitVector(*vbvTop, false);
			VarBitVector* vbvUnini6 = new VarBitVector(vbvTop, false);
			VarBitVector* vbvUnini7 = new VarBitVector(vbvTop, false);
			VarBitVector* vbvUnini8 = new VarBitVector(bot, false);
			if(vbvUnini0->isTop() || vbvUnini0->isBot() || (*vbvUnini0) == (*vbvTop))
			{ printf("Error! Uninitialized bitvector equal to another bitvector!"); ret++; }
			
			vbvUnini2->setVar(*(allVars.begin()), true);
			vbvUnini3->setToTop();
			vbvUnini4->setToBot();
			vbvUnini5->unionUpdate(*vbvTop);
			vbvUnini6->intersectUpdate(*vbvUnini4);
			if((*vbvUnini1) == (*vbvTop)) { printf("Error! Uninitialized bitvector has not been correctly initialized: (*vbvUnini1) != (*vbvTop)!\n"); ret++; }
			if((*vbvUnini2) != (*vbvTop)) { printf("Error! Uninitialized bitvector has not been correctly initialized: (*vbvUnini2) != (*vbvTop)!\n"); ret++; }
			if((*vbvUnini3) != (*vbvTop)) { printf("Error! Uninitialized bitvector has not been correctly initialized: (*vbvUnini3) != (*vbvTop)!\n"); ret++; }
			if(!vbvUnini4->isBot()) { printf("Error! Uninitialized bitvector has not been correctly initialized: !vbvUnini4->isBot()!\n"); ret++; }
			if((*vbvUnini5) != (*vbvTop)) { printf("Error! Uninitialized bitvector has not been correctly initialized: (*vbvUnini5) != (*vbvTop)!\n"); ret++; }
			if(bot != (*vbvUnini6)) { printf("Error! Uninitialized bitvector has not been correctly initialized: bot != (*vbvUnini6)!\n"); ret++; }
			
			if(*(vbvUnini7->unionVec(bot)) != bot) { printf("Error! Uninitialized bitvector U bot != bot!\n"); ret++; }
			if(*(vbvUnini8->intersectVec(*vbvTop)) != *vbvTop) { printf("Error! Uninitialized bitvector I top != top!\n"); ret++; }
			if(*(vbvUnini7->unionVec(*vbvUnini8)) == bot) { printf("Error! Uninitialized bitvector(top) U Unitialized bitvector(bot) is initialized!\n"); ret++; }
			if(*(vbvUnini7->unionVec(*vbvUnini8)) == *vbvTop) { printf("Error! Uninitialized bitvector(top) U Unitialized bitvector(bot) is initialized!\n"); ret++; }
			if(*(vbvUnini7->intersectVec(*vbvUnini8)) == bot) { printf("Error! Uninitialized bitvector(top) U Unitialized bitvector(bot) is initialized!\n"); ret++; }
			if(*(vbvUnini7->intersectVec(*vbvUnini8)) == *vbvTop) { printf("Error! Uninitialized bitvector(top) U Unitialized bitvector(bot) is initialized!\n"); ret++; }
				
			vbvUnini7->unionUpdate(bot);
			if(*(vbvUnini7) != bot) { printf("Error! Uninitialized bitvector U-update bot != bot!\n"); ret++; }
			vbvUnini8->intersectUpdate(*vbvTop);
			if(*(vbvUnini8) != *vbvTop) { printf("Error! Uninitialized bitvector I-update top != top!\n"); ret++; }

			/***************************
			 **** TEST UNSET VALUES ****
			 ***************************/
			{
				VarBitVector vbvUnset(allVars);
				VarBitVector vbvUnsetEven(allVars);
				VarBitVector vbvUnsetEvenFalse(allVars);
				VarBitVector vbvUnsetOdd(allVars);
				VarBitVector vbvUnsetOddFalse(allVars);
				VarBitVector bot(allVars);
				VarBitVector top(allVars);
				VarBitVector vbvEven(allVars);
				VarBitVector vbvOdd(allVars);
					
				bot.setToBot();
				top.setToTop();
				
				i=0;
				for(varIDSet::iterator it = allVars.begin(); it != allVars.end(); it++, i++)
				{
					if(i%2==0)
					{
						vbvUnsetEven[*it] = true;
						vbvUnsetEvenFalse[*it] = false;
						vbvEven[*it] = true;
						vbvOdd[*it] = false;
					}
					else
					{
						vbvUnsetOdd[*it] = true;
						vbvUnsetOddFalse[*it] = false;
						vbvEven[*it] = false;
						vbvOdd[*it] = true;
					}
				}
				
				if(vbvUnsetEven == vbvEven) { printf("Error! (vbvUnsetEven == vbvEven)\n"); ret++; }
				if(vbvUnsetEven == vbvOdd) { printf("Error! (vbvUnsetEven == vbvEven)\n"); ret++; }
				if(vbvUnsetEven == vbvUnsetOdd) { printf("Error! (vbvUnsetEven == vbvEven)\n"); ret++; }
				if(vbvUnsetOdd == vbvEven) { printf("Error! (vbvUnsetEven == vbvEven)\n"); ret++; }
				if(vbvUnsetOdd == vbvOdd) { printf("Error! (vbvUnsetEven == vbvEven)\n"); ret++; }
				if(vbvUnsetOdd == vbvUnsetEven) { printf("Error! (vbvUnsetEven == vbvEven)\n"); ret++; }
				
				VarBitVector* uOIuE = vbvUnsetOdd.intersectVec(vbvUnsetEven);
				if(*uOIuE != vbvUnset) { printf("Error! (*uOIuE != vbvUnset)\n"); ret++; }

				VarBitVector* uOUuE = vbvUnsetOdd.unionVec(vbvUnsetEven);
				if(*uOUuE != top) { printf("Error! (*uOUuE != top)\n"); ret++; }

				VarBitVector* uOUbot = vbvUnsetOdd.unionVec(bot);
				if(*uOUbot != vbvOdd) { printf("Error! (*uOUbot != vbvOdd)\n"); ret++; }

				VarBitVector* uEUbot = vbvUnsetEven.unionVec(bot);
				if(*uEUbot != vbvEven) { printf("Error! (*uEUbot != vbvEven)\n"); ret++; }

				VarBitVector* uOIbot = vbvUnsetOdd.intersectVec(bot);
				if(*uOIbot != vbvUnsetOddFalse) { printf("Error! (*uOIbot != vbvUnsetOdd)\n"); ret++; }

				VarBitVector* uEIbot = vbvUnsetEven.intersectVec(bot);
				if(*uEIbot != vbvUnsetEvenFalse) { printf("Error! (*uEIbot != vbvUnsetEven)\n"); ret++; }

				VarBitVector* uOItop = vbvUnsetOdd.intersectVec(top);
				if(*uOItop != vbvUnsetOdd) { printf("Error! (*uOItop != vbvUnsetOdd)\n"); ret++; }

				VarBitVector* uEItop = vbvUnsetEven.intersectVec(top);
				if(*uEItop != vbvUnsetEven) { printf("Error! (*uEItop != vbvUnsetEven)\n"); ret++; }

				VarBitVector* uOUtop = vbvUnsetOdd.unionVec(top);
				if(*uOUtop != top) { printf("Error! (*uOUtop != top)\n"); ret++; }

				VarBitVector* uEUtop = vbvUnsetEven.unionVec(top);
				if(*uEUtop != top) { printf("Error! (*uEUtop != top)\n"); ret++; }
					
				
				VarBitVector vbvAllUnset1(allVars);
				VarBitVector vbvAllUnset2(allVars, VarBitVector::Unset);
				VarBitVector vbvAllFalse(allVars, VarBitVector::False);
				VarBitVector vbvAllTrue(allVars, VarBitVector::True);
				for(varIDSet::iterator it = allVars.begin(); it != allVars.end(); it++)
				{
					if(vbvAllUnset1[*it]!=VarBitVector::Unset) { printf("Error! vbvAllUnset1[%s]!=Unset\n", (*it).str().c_str()); ret++; }
					if(vbvAllUnset2[*it]!=VarBitVector::Unset) { printf("Error! vbvAllUnset2[%s]!=Unset\n", (*it).str().c_str()); ret++; }
					if(vbvAllFalse[*it]!=VarBitVector::False) { printf("Error! vbvAllFalse[%s]!=False\n", (*it).str().c_str()); ret++; }
					if(vbvAllTrue[*it]!=VarBitVector::True) { printf("Error! vbvAllTrue[%s]!=True\n", (*it).str().c_str()); ret++; }
				}
			}
		}
	}
	
	if(ret==0)
		printf("union_find: PASSED!\n");
	else
		printf("union_find: FAILED!\n");
	
	return ret;
}
