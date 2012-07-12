/*
  On Chip Memory Optimizer
  reduces global memory accesses with 
  the utilization of registers and shared memory

*/

#include "StencilAnalysis.h"
#include "../OptimizerInterface/CudaOptimizerInterface.h"
#include "ASTtools.hh"
#include "../../midend/mintTools/MintOptions.h"
#include "../../midend/arrayProcessing/MintArrayInterface.h"

#include <string>
#include "OmpAttribute.h"

using namespace OmpSupport; //for replaceVariableRef
using namespace std;

StencilAnalysis::StencilAnalysis()
{


}

StencilAnalysis::~StencilAnalysis()
{


}

//find out how many planes of size [TILE_Y + offset][TILE_X + offset]
//we can store in shared memory
//we use max-offset since we haven't done the array specific analysis
//return 0, if we cannot apply shared memory opt because the tile size is too big
//assumptions is based on 16KB shared memory, configure the types/MinTypes.h 
//if you want to change the amount of shared memory available on the device
int StencilAnalysis::howManyPlanesInSharedMemory(const MintForClauses_t& clauseList, 
						 const SgType* type)
{

  //we need to know exact order, it makes a big difference
  int tile_x = clauseList.tileDim.x ;
  int tile_y = clauseList.tileDim.y ;

  int min_plane_size = (tile_x + 2)* (tile_y + 2) ; //at least one ghost cell 
  int max_plane_size = (clauseList.tileDim.x + 2*MAX_ORDER) * (clauseList.tileDim.y + 2*MAX_ORDER); 

  if(isSgTypeFloat(type) || isSgTypeInt(type)){
    min_plane_size = min_plane_size * sizeof(float) ;
    max_plane_size = max_plane_size * sizeof(float) ;
  }
  else {//to be safe
    min_plane_size = min_plane_size * sizeof(double);
    max_plane_size = max_plane_size * sizeof(double);
  }
  int min_num_planes = SHARED_MEM_SIZE/(min_plane_size);
  //int max_num_planes = SHARED_MEM_SIZE/(max_plane_size);
  //cout << min_num_planes << " planes " << endl;
  //cout << max_num_planes << " max planes " << endl;

  if(min_num_planes > 8 )
    min_num_planes = 8 ;//quick fix, remove this later.

  int num_planes_asked = MintOptions::getNumberOfSharedMemPlanes();

  //return whichever smaller
  return min_num_planes < num_planes_asked ? min_num_planes : num_planes_asked; 
}


void  sortFreq(int* freq, int* index, int N)
{
  //doing insertion sort
  if( N <= 1)
    return;

  if(N == 2)
    {
      int val0 = freq[0];
      int val1 = freq[1];

      if(val0 < val1)
	{
	  freq[0] = val1;
	  freq[1] = val0;
	  index[0] = 1;
	  index[1] = 0;
	}
      return;
    }

  for(int i=1; i < N; i++ )
    {
      int key = freq[i];      
      int j= i - 1;
      while(j >= 0 && freq[j] < key)
	{
	  index[j+1] = index[j];
	  freq[j+1] = freq[j]; 
	  j--;
	}
      index[j+1] = i ;
      freq[j+1] = key; 
    }
}

int StencilAnalysis::getSharingCategory(const std::vector<SgExpression*> subscripts) 
{
  //return 2 if it is up or down
  //return 1 if it is off-center
  //return 0 if it is center
  //return -1 if it is neither

  //check if subsrcipts are _gidx, _gidy or _gidz
  std::vector<SgExpression*>::const_iterator it;
  
  size_t count = 0;
  int indexNo = 0;
  size_t category = 0;

  for(it= subscripts.begin(); it != subscripts.end(); it++)
    {      
      indexNo++;
      SgExpression* index = isSgExpression(*it);

      Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(index, V_SgVarRefExp);
      
      if(varList.size() != 1) //there should be only 1 variable and that should be gidx,y,z
	return -1;
      //check if that varRef is x, y, z

      SgVarRefExp* varExp = isSgVarRefExp(*(varList.begin()));
      ROSE_ASSERT(varExp);

      string index_str = varExp->unparseToString();      
      
      if(indexNo == 1 && index_str != GIDX )
	return -1;
      else if(indexNo == 2 && index_str != GIDY )
	return -1;
      else if(indexNo == 3 && index_str != GIDZ )
	return -1;

      Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(index, V_SgIntVal);

      if(constList.size() > 1 )
	return -1;

      if(constList.size() == 1)
	{
	  category = 1;

	  SgIntVal* constVal = isSgIntVal(*(constList.begin()));
	  ROSE_ASSERT(constVal);

	  if(constVal->get_value() > MAX_ORDER)
	    return -1;

	  //we keep maximum 3 planes in shared memory
	  if(constVal->get_value() > 1 && indexNo == 3)
	    return -1;

	  Rose_STL_Container<SgNode*> binOpList = NodeQuery::querySubTree(index, V_SgBinaryOp);
      
	  if(binOpList.size() != 1)
	    return -1;

	  SgBinaryOp* binOp = isSgBinaryOp(*(binOpList.begin()));

	  //we want either one add or subtract operation in the index expression
	  //no complex indexing is supported for now. 
	  //A[i+2][i-4] is valid but A[i*2] is not valid
	  
	  if( !isSgAddOp(binOp) &&  !isSgSubtractOp(binOp))
	    return -1;

	  if(indexNo == 3) 
	    { 
	      category = 2;
	    }
	}
      count++;
    }

  return (count == subscripts.size()) ? category : -1 ;

}

void StencilAnalysis::selectCandidates(MintArrFreqPairList_t& candidateVarsShared,
				       std::vector<SgInitializedName*> arrayList, 
				       int* planes1, int* planes3 ,
				       int* indexList1, int* indexList3, int num_planes)
{

  int size = arrayList.size();

  std::set<SgInitializedName* > selected;

  int i=0, j=0;
  int ref1 = planes1[i];
  int ref3 = planes3[j];
  int index1 = indexList1[i];
  int index3 = indexList3[j];
       
  bool onePlane = false;

  while(num_planes > 0 && i < size && j < size && (ref1 != 0 || ref3 != 0))
    {
      //candidate variable for 1-plane is the same as the candidate variable for 3-planes
      if((index1 == index3))
	{
	  //compare the references
	  if(ref1 * 2 >= ref3 || num_planes < 3) 
	      onePlane = true; //1-plane gives more savings 

	  else //3-plane gives more savings
	    {
	      SgInitializedName* name = arrayList.at(index3);	  
	      selected.insert(name);

	      MintArrFreqPair_t apair (name, 3);	      
	      candidateVarsShared.push_back(apair);

	      num_planes -= 3;	      
	      j++; i++;

	      if(j < size){
		  ref3 = planes3[j];
		  index3 = indexList3[j];
		}
	      
	      if (i < size){
		  ref1 = planes1[i];
		  index1 = indexList1[i];
	      }
	    }
	}
      //if the candidate variable for 3-planes is already in the selected list as a 1-plane
      else if(selected.find(arrayList.at(index3)) != selected.end() )
	{
	  int ref1_2 = ref1 + ((i+1 < size) ? planes1[i+1] : 0);

	  //compare the references
	  if(ref1_2 >= ref3 || num_planes < 2) 
	      onePlane = true; //1-plane gives more savings 

	  else //3-plane gives more savings
	    {
	      SgInitializedName* name = arrayList.at(index3);

	      for(MintArrFreqPairList_t::iterator it = candidateVarsShared.begin(); it != candidateVarsShared.end(); it++)
		{
		  SgInitializedName* myname = (*it).first;
		  if(name == myname )
		    {
		      (*it).second = 3;
		      break;
		    }
		}
	      num_planes -= 2; //because we already had one plane

	      if(j < size -1)
		{
		  ref3 = planes3[++j];
		  index3 = indexList3[j];
		}
	    }	  
	}
      else 
	{
	  int ref1_2_3 = ref1 + ((i+1 < size) ? planes1[i+1] : 0) + ((i+2 < size) ? planes1[i+2] : 0);
	 
	  //compare the references
	  if(ref1_2_3 >= ref3 || num_planes < 3) 
	      onePlane = true; //1-plane gives more savings 
	  
	  else //3-plane gives more savings
	    {
	      SgInitializedName* name = arrayList.at(index3);

	      MintArrFreqPair_t apair (name, 3);	      
	      candidateVarsShared.push_back(apair);
	      selected.insert(name);

	      num_planes -= 3;
	      if( j < size -1){
		ref3 = planes3[++j];
		index3 = indexList3[j];
	      }
	    }	  
	}

      if(onePlane)
	{
	  onePlane = false;

	  SgInitializedName* name = arrayList.at(index1);

	  MintArrFreqPair_t apair (name, 1);
	  candidateVarsShared.push_back(apair);
	  selected.insert(name);
	  
	  num_planes--; i++;

	  if(i < size ){
	    //update the candidate for 1-plane and its index
	    ref1 = planes1[i];
	    index1 = indexList1[i];
	  }
	}
    }//end of while

  selected.clear();
}

void StencilAnalysis::computeAccessFreq(MintInitNameMapExpList_t& arrRefList, int num_planes, 
					MintArrFreqPairList_t& candidateVarsShared, 
					MintArrFreqPairList_t& candidateVarsReg)
{
  //finds the frequency of references for each array 
  //categorizes them as up, down, center, off-center
  //we use this categorization to pick which arrays need to be placed 
  //in shared memory
  //then sorts the frequences and then finds the candidates 

  //arrRefList contains the (array, expList)                                                                            
  //ex: A -> A[i][j], A[i+1][j], A[i][j-1] ...    
  ///   B -> B[i][j], B[i-1][j] ...

  MintInitNameMapExpList_t::const_iterator it;
  
  int num_arrays = arrRefList.size();

  std::vector<SgInitializedName*> arrayList;

  int* onePlaneList =(int*) malloc(sizeof(int) * num_arrays);
  int* threePlanesList = (int*)malloc(sizeof(int) * num_arrays);
  int* centerList= (int*)malloc(sizeof(int) * num_arrays);

  //for 1-plane
  int* indexList1 = (int*) malloc(sizeof(int) * num_arrays);
  //for 3-planes
  int* indexList2 = (int*) malloc(sizeof(int) * num_arrays);
  //for center
  int* indexList3 = (int*) malloc(sizeof(int) * num_arrays);

  int count=0;
  for(it = arrRefList.begin(); it != arrRefList.end(); it++)
    {
      SgInitializedName* array = it->first;
      ROSE_ASSERT(array);
      
      int dimension = MintArrayInterface::getDimension(array);

      int center = 0;
      int updown = 0;
      int off_center = 0;
      int other = 0; 

      //if it is 1-dim, skip it.
      if(dimension > 1 && dimension <= 3)
	{
	  //list of all the references to this array 
	  std::vector<SgExpression*> expList = it->second;
	  
	  //go through all the references to this array
	  for(std::vector<SgExpression*>::const_iterator e= expList.begin(); e != expList.end(); e++)
	    {
	      SgExpression* arrRefWithIndices = isSgExpression(*e);
	      SgExpression* arrayExp; 
	      vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	      
	      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
		{
		  int category = getSharingCategory(subscripts); 
		  
		  if (category == 0)
		    center++;
		  else if(category == 1)
		    off_center++;
		  else if(category == 2)
		    updown++;
		  else 
		    other++;
		  
		  //cout << arrRefWithIndices->unparseToString() << " "<< category << endl;
		}
	    }//end of this array
	}
      indexList1[count]= count;
      indexList2[count]= count;      
      indexList3[count]= count;      

      //to be eligible, at least there should be one off-center reference, otherwise, we can just use register
      //to store the center point
      onePlaneList[count] = off_center != 0 ? (center + off_center) : 0; 

      centerList[count] = center;

      //to be eligible, at least there should be more than 1 in the up and bottom planes
      threePlanesList[count] = updown > 2 ? updown : 0;

      arrayList.push_back(array);

      count++;
    }

  //sorts frequencies from highest to lowest
  sortFreq(onePlaneList, indexList1, count);
  sortFreq(threePlanesList, indexList2, count);
  sortFreq(centerList, indexList3, count);

  //store the register candidates and their freq in a data structure 
  for(int i=0 ; i < count ; i++)
    {
      int index = indexList3[i];
      int freq  = centerList[i]; //this is its frequency 

      //this is the candidate array
      SgInitializedName* array = arrayList.at(index);
      MintArrFreqPair_t candidate (array, freq);
      candidateVarsReg.push_back(candidate);
    }

  selectCandidates(candidateVarsShared, arrayList, onePlaneList, threePlanesList, 
		   indexList1, indexList2, num_planes);
 
  free(indexList1);
  free(indexList2);
  free(indexList3);

  free(centerList);
  free(onePlaneList);
  free(threePlanesList);

  arrayList.clear();
}

int StencilAnalysis::performHigherOrderAnalysis(SgBasicBlock* basicBlock, 
						const SgInitializedName* array, int num_planes)
{
  //Didem: rewrote in March 01, 2011 to make it more robust 
  //we are interested in only expressions i,j,k + c where c is a constant 
  //and consider only these kinds of expressions because others cannot 
  //benefit from the on-chip memory optimizations

  //this affects the shared memory offsets [BLOCKDIM + order]
  //assumes symmetric 
  //x dim : -/+ order 
  //y dim : -/+ order
  //z dim : -/+ order 

  int maxOrderX = 0;
  int maxOrderY = 0;
  int maxOrderZ = 0;

  size_t dim = MintArrayInterface::getDimension(array);

  string candidateVarName = array->get_name().str();

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(basicBlock, V_SgPntrArrRefExp);

  Rose_STL_Container<SgNode*>::reverse_iterator arrayNode = nodeList.rbegin();
  
  for(; arrayNode != nodeList.rend(); arrayNode++)
    {
      ROSE_ASSERT(*arrayNode);

      SgExpression* arrayExp; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]

      SgExpression* arrRefWithIndices = isSgExpression(*arrayNode);

      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	{
	  SgInitializedName *arrayName = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayExp));
	  ROSE_ASSERT(arrayName);

	  string var_name= arrayName->get_name().str();	

	  //check if array name matches
	  if(var_name == candidateVarName && subscripts.size() == dim){
		
	    int indexNo = 0;

	    //check if subsrcipts are _gidx and _gidy
	    std::vector<SgExpression*>::iterator it;
	
	    for(it= subscripts.begin(); it != subscripts.end(); it++)
	      {
		indexNo++;
		SgExpression* index = *it;

		Rose_STL_Container<SgNode*> binOpList = NodeQuery::querySubTree(index, V_SgBinaryOp);

		if(binOpList.size() == 1)
		  {
		    SgBinaryOp* binOp = isSgBinaryOp(*(binOpList.begin()));
		    ROSE_ASSERT(binOp);

		    if(isSgAddOp(binOp) || isSgSubtractOp(binOp))
		      {
			SgExpression* lhs = binOp->get_lhs_operand();
			SgExpression* rhs = binOp->get_rhs_operand();
			ROSE_ASSERT(lhs);
			ROSE_ASSERT(rhs);
			
			SgExpression* varExp = NULL;
			string varExp_str = "";
			int newVal =0;

			if(isSgIntVal(lhs) && isSgVarRefExp(rhs))
			  {
			    varExp = isSgVarRefExp(rhs);
			    varExp_str = varExp->unparseToString();
			    newVal = isSgIntVal(lhs)-> get_value();
			  }

			else if (isSgIntVal(rhs) && isSgVarRefExp(lhs))
			  {
			    varExp = isSgVarRefExp(lhs);
			    varExp_str = varExp->unparseToString();
			    newVal = isSgIntVal(rhs)-> get_value();
			  }
			   
			if(indexNo == 1 && varExp_str == GIDX)
			  maxOrderX = newVal > maxOrderX ? newVal : maxOrderX;
			
			if(indexNo == 2 && varExp_str == GIDY)
			  maxOrderY = newVal > maxOrderY ? newVal : maxOrderY;
			
			if(indexNo == 3 && varExp_str == GIDZ)
			  maxOrderZ = newVal > maxOrderZ ? newVal : maxOrderZ;
			
		      }//end of addop subtract op

		  } // end of binary op 

	      } //end of for subscripts

	  }//if end of var_name = candidatename

	}//end of is arrayRef

    }// end of ptr arr ref loop

  if(num_planes == 1)
    return (maxOrderX > maxOrderY) ? maxOrderX : maxOrderY;

  if(maxOrderX > maxOrderY )
   return  (maxOrderX > maxOrderZ) ? maxOrderX : maxOrderZ;

  return  (maxOrderY > maxOrderZ) ? maxOrderY : maxOrderZ;


} //end of function 


void StencilAnalysis::performCornerStencilsAnalysis(SgBasicBlock* basicBlock, 
						    const SgInitializedName* array,
						    bool& plane_xy /*false*/, 
						    bool& plane_xz /*false*/, 
						    bool& plane_yz /*false*/ )
{
  //For example 19 and 27-stencils require upper and lower planes in shared memory but 
  //7-point only needs the center plane in the shared memory. 

  bool cornerXY = false; 
  bool cornerYZ = false; 
  bool cornerXZ = false; 

  size_t dim = MintArrayInterface::getDimension(array);

  string candidateVarName = array->get_name().str();

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(basicBlock, V_SgPntrArrRefExp);

  Rose_STL_Container<SgNode*>::reverse_iterator arrayNode = nodeList.rbegin();
  
  for(; arrayNode != nodeList.rend(); arrayNode++)
    {
      ROSE_ASSERT(*arrayNode);

      SgExpression* arrayExp; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]

      SgExpression* arrRefWithIndices = isSgExpression(*arrayNode);

      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	{
	  SgInitializedName *arrayName = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayExp));

	  string var_name= arrayName->get_name().str();	

	  //check if array name matches
	  if(var_name == candidateVarName && subscripts.size() == dim){
		
	    //check if subsrcipts are _gidx and _gidy
	    std::vector<SgExpression*>::iterator it;
	
	    bool cornerX = false; 
	    bool cornerY = false; 
	    bool cornerZ = false; 

	    for(it= subscripts.begin(); it != subscripts.end(); it++)
	      {

		SgExpression* index = *it;
	       
		Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(index, V_SgIntVal);

		Rose_STL_Container<SgNode*> indexVarExp = NodeQuery::querySubTree(index, V_SgVarRefExp);

		if(constList.size() > 0 && indexVarExp.size() == 1 )
		  {
		    string indexVarStr = isSgExpression(*(indexVarExp.begin())) -> unparseToString();
		 
		    //corner 
		    if(indexVarStr == GIDX )
		      cornerX = true;
		    else if (indexVarStr == GIDY)
		      cornerY = true;
		    else if (indexVarStr == GIDZ)
		      cornerZ = true;		    
		  }
	      }

	    cornerXY = (cornerX && cornerY) ? true : cornerXY;
	    cornerXZ = (cornerX && cornerZ) ? true : cornerXZ;
	    cornerYZ = (cornerZ && cornerY) ? true : cornerYZ;	      
	  }
	}
    }
  
  plane_xy = cornerXY;
  plane_xz = cornerXZ;
  plane_yz = cornerYZ;

  if(plane_xy)
    cout << "  INFO:Mint: Corner x y planes are shared" << endl;

  if(plane_xz)
    cout << "  INFO:Mint: Corner x z planes are shared" << endl;

  if(plane_yz)
    cout << "  INFO:Mint: Corner y z planes are shared" << endl;

}


bool StencilAnalysis::isShareableReference(const std::vector<SgExpression*> subscripts, 
					   const bool corner_yz //=FALSE
					   ) 
{
  //March 2 2011, made changes in the function, we have more robust sharing conditions
  //checks if an array reference can be replaced with shared memory reference
  //by looking at the index expressions 
  //assumes the variable is already checked for candidacy

  //check if subsrcipts are _gidx, _gidy or _gidz
  std::vector<SgExpression*>::const_iterator it;
  
  size_t count = 0;
  int indexNo = 0;

  for(it= subscripts.begin(); it != subscripts.end(); it++)
    {      
      indexNo++;
      SgExpression* index = isSgExpression(*it);

      Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(index, V_SgVarRefExp);
      
      if(varList.size() != 1) //there should be only 1 variable and that should be gidx,y,z
	return false;
      //check if that varRef is x, y, z

      SgVarRefExp* varExp = isSgVarRefExp(*(varList.begin()));
      ROSE_ASSERT(varExp);

      string index_str = varExp->unparseToString();      
      
      if(indexNo == 1 && index_str != GIDX )
	return false;
      if(indexNo == 2 && index_str != GIDY )
	return false;
      if(indexNo == 3 && index_str != GIDZ )
	return false;

      Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(index, V_SgIntVal);

      if(constList.size() > 1 )
	return false;

      if(constList.size() == 1)
	{

	  SgIntVal* constVal = isSgIntVal(*(constList.begin()));
	  ROSE_ASSERT(constVal);

	  if(constVal->get_value() > MAX_ORDER)
	    return false;

	  //we keep maximum 3 planes in shared memory
	  if(constVal->get_value() > 1 && indexNo == 3)
	    return false;

	  Rose_STL_Container<SgNode*> binOpList = NodeQuery::querySubTree(index, V_SgBinaryOp);
      
	  if(binOpList.size() != 1)
	    return false;

	  SgBinaryOp* binOp = isSgBinaryOp(*(binOpList.begin()));

	  //we want either one add or subtract operation in the index expression
	  //no complex indexing is supported for now. 
	  //A[i+2][i-4] is valid but A[i*2] is not valid
	  
	  if( !isSgAddOp(binOp) &&  !isSgSubtractOp(binOp))
	    return false;

	  if(indexNo == 3 && !corner_yz) 
	    { //corner of yz is only shareable when corner_yz is true
	      return false;
	    }

	}
      count++;
    }

  return (count == subscripts.size()) ? true : false ;

}
