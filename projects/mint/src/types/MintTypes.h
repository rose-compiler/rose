
#ifndef __MINTTYPES_H

#define __MINTTYPES_H
//#include "rose.h"

#include <string>
#include <map>
#include <vector>

#define VERBOSE
//#define VERBOSE_2


using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

typedef std::map <SgInitializedName*, std::vector < SgExpression*> > MintInitNameMapExpList_t;
typedef std::map <SgVariableSymbol*, SgInitializedName* > MintHostSymToDevInitMap_t;

typedef std::map <SgVariableSymbol*, std::vector< string > > MintSymSizesMap_t;
typedef std::map <SgInitializedName*, SgExpression*> MintInitNameExpMap_t;
typedef std::map <SgExpression*, SgExpression*> MintExp2ExpList_t; 

typedef pair <SgInitializedName*, int> MintArrFreqPair_t;
typedef std::vector < MintArrFreqPair_t > MintArrFreqPairList_t;

//device reserved variables
static const std::string THREADIDXX ("threadIdx.x");
static const std::string THREADIDXY ("threadIdx.y");
static const std::string THREADIDXZ ("threadIdx.z");

static const std::string BLOCKIDXX ("blockIdx.x");
static const std::string BLOCKIDXY ("blockIdx.y");
static const std::string BLOCKIDXZ ("blockIdx.z");

static const std::string BLOCKDIM_X ("blockDim.x");
static const std::string BLOCKDIM_Y ("blockDim.y");
static const std::string BLOCKDIM_Z ("blockDim.z");

static const std::string TILE_X ("TILE_X");
static const std::string TILE_Y ("TILE_Y");
static const std::string TILE_Z ("TILE_Z");

static const std::string CHUNKSIZE_X ("CHUNKSIZE_X");
static const std::string CHUNKSIZE_Y ("CHUNKSIZE_Y");
static const std::string CHUNKSIZE_Z ("CHUNKSIZE_Z");

//global and local indices
static const std::string IDX ("_idx");
static const std::string IDY ("_idy");
static const std::string IDZ ("_idz");
static const std::string GIDX ("_gidx");
static const std::string GIDY ("_gidy");
static const std::string GIDZ ("_gidz");
static const std::string UPPERBOUND_X ("_upperb_x");
static const std::string UPPERBOUND_Y ("_upperb_y");

//intermediate variables for optimization
static const std::string REG_SUFFIX("_r");
static const std::string SHARED_BLOCK("_sh_block_");
static const std::string BORDER_GLOBAL_INDEX_DIFF("_borderGlobalIndexDiff");

//Mint Pragmas 
static const std::string MINT_FOR_PRAGMA ("for");
static const std::string MINT_PRAGMA ("mint");
static const std::string MINT_PARALLEL_PRAGMA ("parallel");
static const std::string MINT_SINGLE_PRAGMA ("single");
static const std::string MINT_COPY_PRAGMA ("copy");
static const std::string MINT_DATA_TRF_TO_DEV_PRAGMA ("toDevice");
static const std::string MINT_DATA_TRF_FROM_DEV_PRAGMA ("fromDevice");
static const std::string MINT_NESTED_PRAGMA ("nest");
static const std::string MINT_ALL_PRAGMA ("all");
static const std::string MINT_TILE_PRAGMA ("tile");
static const std::string MINT_CHUNKSIZE_PRAGMA ("chunksize");

//structs 
typedef struct {
  int x;
  int y;
  int z;
}Dim3_t;
 
typedef struct {
  int nested;           
  Dim3_t tileDim;   //number of data points in a tile (computed by a thread block)
  Dim3_t chunksize;  // chunksize determines how many output data points a thread computes                     
} MintForClauses_t ;

typedef struct { 
  string dst;
  string src;
  string trfType;
  std::vector<string> sizes;
}MintTrfParams_t;

//device related constant 
static const int SHARED_MEM_SIZE = (16386); //in bytes
static const int MAX_NUM_THREADS_PER_BLOCK = 512;
static const int DEFAULT_BLOCKDIM = 128;
static const int DEF_BLOCKDIM_X = 16;
static const int DEF_BLOCKDIM_Y = 16;
static const int DEF_BLOCKDIM_Z = 32;

// compilation related constant
static const int SHORT_LOOP_UNROLL_FACTOR =5;
static const int MAX_ORDER = 3;
#endif 
//__MINTTYPES_H
