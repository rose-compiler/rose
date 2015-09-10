#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REAL float
//#define ntotal  16
#define nghost 1
#define ncube 2 // 2 + 2 * nghost
#define nxL0 (3) // 1 + 2*nghost
#define nyL0 (3) // 1 + 2*nghost
#define nxL1 (4) // 2 + 2*nghost
#define nyL1 (4) // 2 + 2*nghost
#define nxL2 (6) // 4 + 2*nghost
#define nyL2 (6) // 4 + 2*nghost
#define nxL3 (10) // 8 + 2*nghost
#define nyL3 (10) // 8 + 2*nghost
#define nxL4 (18) // 16 + 2*nghost
#define nyL4 (18) // 16 + 2*nghost
#define nxL5 (34) // 32 + 2*nghost
#define nyL5 (34) // 32 + 2*nghost
#define nxL6 (66) // 64 + 2*nghost
#define nyL6 (66) // 64 + 2*nghost
#define nxL7 (130) // 130 + 2*nghost
#define nyL7 (130) // 130 + 2*nghost
#define nxL8 (258) // 256 + 2*nghost
#define nyL8 (258) // 256 + 2*nghost
#define nxL9 (514) // 512 + 2*nghost
#define nyL9 (514) // 512 + 2*nghost
#define nxL10 (1026) // 1024 + 2*nghost
#define nyL10 (1026) // 1024 + 2*nghost


/*
L0 data:  data are stored in the column-major format
   x x x x x x
   x x x x x x
   x x o o x x
   x x o o x x
   x x x x x x
   x x x x x x
*/
struct L0struct{
#pragma 0
REAL (*dptr)[nxL0][nyL0][ncube][ncube];
};

struct L1struct{
#pragma 0
REAL (*dptr)[nxL1][nyL1][ncube][ncube];
};

struct L2struct{
#pragma 0
REAL (*dptr)[nxL2][nyL2][ncube][ncube];
};

struct L3struct{
#pragma 0
REAL (*dptr)[nxL3][nyL3][ncube][ncube];
};

struct L4struct{
#pragma 0
REAL (*dptr)[nxL4][nyL4][ncube][ncube];
};

struct L5struct{
#pragma 1
REAL (*dptr)[nxL5][nyL5][ncube][ncube];
};

struct L6struct{
#pragma 1
REAL (*dptr)[nxL6][nyL6][ncube][ncube];
};

struct L7struct{
#pragma 1
REAL (*dptr)[nxL7][nyL7][ncube][ncube];
};

struct L8struct{
#pragma 1
REAL (*dptr)[nxL8][nyL8][ncube][ncube];
};

struct L9struct{
#pragma 1
REAL (*dptr)[nxL9][nyL9][ncube][ncube];
};

struct L10struct{
#pragma 1
REAL (*dptr)[nxL10][nyL10][ncube][ncube];
};
/*
void initData(int X, int Y, REAL (*dptr)[X][Y][ncube][ncube])
{
  int cubex, cubey;
  int ix,iy;
  for(cubex=0; cubex < X; ++cubex)
  for(cubey=0; cubey < Y; ++cubey)
  for(ix=0; ix < ncube; ++ix)
  for(iy=0; iy < ncube; ++iy)
   (*dptr)[cubex][cubey][ix][iy] = (cubex*ncube+ix) + (cubey*ncube+iy)*(ncube*X); 
}


void printData(int X, int Y, REAL (*dptr)[X][Y][ncube][ncube], char* st)
{
  int row,col;
  printf("%s\n",st);
  int cubex, cubey;
  int ix,iy;
  for(cubey=0; cubey < Y; ++cubey)
  for(iy=0; iy < ncube; ++iy)
  {
  for(cubex=0; cubex < X; ++cubex)
  for(ix=0; ix < ncube; ++ix)
  {
   printf("%3.2f\t",(*dptr)[cubex][cubey][ix][iy]);
  }
  printf("\n");
  }
}
*/
void computeL0(REAL (*dptr)[nxL0][nyL0][ncube][ncube], int iold, int inew)
{
  int ix,iy;
  for(ix=0; ix < ncube; ++ix)
  for(iy=0; iy < ncube; ++iy)
   dptr[inew][1][1][ix][iy] = (dptr[iold][1][1][ix][iy] +  \
                               ((ix == 0) ? dptr[iold][0][1][ncube-1][iy]:dptr[iold][1][1][ix-1][iy]) +  \
                               ((ix == ncube-1) ? dptr[iold][2][1][0][iy]:dptr[iold][1][1][ix+1][iy]) +  \
                               ((iy == 0) ? dptr[iold][1][0][ix][ncube-1]:dptr[iold][1][1][ix][iy-1]) +  \
                               ((iy == ncube-1) ? dptr[iold][1][2][ix][0]:dptr[iold][1][1][ix][iy+1]))/4;
}

int main()
{
  // double-buffer (ld, st)
  struct L10struct L10data;
  L10data.dptr = (REAL (*)[nxL10][nyL10][ncube][ncube]) malloc(sizeof(REAL)*nxL10*nyL10*ncube*ncube*2);

  int i_l10_load = 0;
  int i_l10_comp = 1;
  // initailize data with linearalized values
  //initData(nxL10, nyL10, &L10data.dptr[i_l10_load]);
  //printData(nxL10,nxL10,&L10data.dptr[i_l10_load], "before");

/******   starting Level-10    ******/
  struct L9struct L9data;
  L9data.dptr = (REAL (*)[nxL9][nyL9][ncube][ncube]) malloc(sizeof(REAL)*nxL9*nyL9*ncube*ncube*2);
  int i_l9_load = 0;
  int i_l9_comp = 1;
  //prefetch first chunk
  int iL10copy;
  for(iL10copy = 0; iL10copy < nxL9; ++iL10copy)
    memcpy(&L9data.dptr[i_l9_load][iL10copy], &L10data.dptr[i_l10_load][iL10copy], sizeof(REAL)*ncube*ncube*nyL9);
  int idx_l10, idtofetch_l10;
  int i10offset = (nxL10-2) / (ncube);
  for(idx_l10 = 0; idx_l10 < 4; idx_l10++)
  {
    idtofetch_l10 = idx_l10 + 1; 
    int idx_fetch_l10 = idtofetch_l10 / 2;
    int idy_fetch_l10 = idtofetch_l10 % 2;;
    int idx_store_l10 = idx_l10 / 2;
    int idy_store_l10 = idx_l10 % 2;;
    // flip double buffer index
    i_l9_load ^= 1;
    i_l9_comp ^= 1;
    // fetch for next chunk
    if(idx_l10 != 3)
    {
      //printf("copying L10 %d %d %d\n",idx_fetch_l10, idy_fetch_l10,i10offset);
      for(iL10copy = 0; iL10copy < nxL9; ++iL10copy)
        memcpy(&L9data.dptr[i_l9_load][iL10copy], &L10data.dptr[i_l10_load][iL10copy+idx_fetch_l10*i10offset][idy_fetch_l10*i10offset], sizeof(REAL)*ncube*ncube*nyL9);
    }
    // working on the computation for next level

  {
/******   starting Level-9    ******/
  struct L8struct L8data;
  L8data.dptr = (REAL (*)[nxL8][nyL8][ncube][ncube]) malloc(sizeof(REAL)*nxL8*nyL8*ncube*ncube*2);
  int i_l8_load = 0;
  int i_l8_comp = 1;
  //prefetch first chunk
  int iL9copy;
  for(iL9copy = 0; iL9copy < nxL8; ++iL9copy)
    memcpy(&L8data.dptr[i_l8_load][iL9copy], &L9data.dptr[i_l9_load][iL9copy], sizeof(REAL)*ncube*ncube*nyL8);
  int idx_l9, idtofetch_l9;
  int i9offset = (nxL9-2) / (ncube);
  for(idx_l9 = 0; idx_l9 < 4; idx_l9++)
  {
    idtofetch_l9 = idx_l9 + 1; 
    int idx_fetch_l9 = idtofetch_l9 / 2;
    int idy_fetch_l9 = idtofetch_l9 % 2;;
    int idx_store_l9 = idx_l9 / 2;
    int idy_store_l9 = idx_l9 % 2;;
    // flip double buffer index
    i_l8_load ^= 1;
    i_l8_comp ^= 1;
    // fetch for next chunk
    if(idx_l9 != 3)
    {
      //printf("copying L9 %d %d %d\n",idx_fetch_l9, idy_fetch_l9,i9offset);
      for(iL9copy = 0; iL9copy < nxL8; ++iL9copy)
        memcpy(&L8data.dptr[i_l8_load][iL9copy], &L9data.dptr[i_l9_load][iL9copy+idx_fetch_l9*i9offset][idy_fetch_l9*i9offset], sizeof(REAL)*ncube*ncube*nyL8);
    }
    // working on the computation for next level
  {
/******   starting Level-8    ******/
  struct L7struct L7data;
  L7data.dptr = (REAL (*)[nxL7][nyL7][ncube][ncube]) malloc(sizeof(REAL)*nxL7*nyL7*ncube*ncube*2);
  int i_l7_load = 0;
  int i_l7_comp = 1;
  //prefetch first chunk
  int iL8copy;
  for(iL8copy = 0; iL8copy < nxL7; ++iL8copy)
    memcpy(&L7data.dptr[i_l7_load][iL8copy], &L8data.dptr[i_l8_load][iL8copy], sizeof(REAL)*ncube*ncube*nyL7);
  int idx_l8, idtofetch_l8;
  int i8offset = (nxL8-2) / (ncube);
  for(idx_l8 = 0; idx_l8 < 4; idx_l8++)
  {
    idtofetch_l8 = idx_l8 + 1; 
    int idx_fetch_l8 = idtofetch_l8 / 2;
    int idy_fetch_l8 = idtofetch_l8 % 2;;
    int idx_store_l8 = idx_l8 / 2;
    int idy_store_l8 = idx_l8 % 2;;
    // flip double buffer index
    i_l7_load ^= 1;
    i_l7_comp ^= 1;
    // fetch for next chunk
    if(idx_l8 != 3)
    {
      //printf("copying L8 %d %d %d\n",idx_fetch_l8, idy_fetch_l8,i8offset);
      for(iL8copy = 0; iL8copy < nxL7; ++iL8copy)
        memcpy(&L7data.dptr[i_l7_load][iL8copy], &L8data.dptr[i_l8_load][iL8copy+idx_fetch_l8*i8offset][idy_fetch_l8*i8offset], sizeof(REAL)*ncube*ncube*nyL7);
    }
    // working on the computation for next level
  {
/******   starting Level-7    ******/
  struct L6struct L6data;
  L6data.dptr = (REAL (*)[nxL6][nyL6][ncube][ncube]) malloc(sizeof(REAL)*nxL6*nyL6*ncube*ncube*2);
  int i_l6_load = 0;
  int i_l6_comp = 1;
  //prefetch first chunk
  int iL7copy;
  for(iL7copy = 0; iL7copy < nxL6; ++iL7copy)
    memcpy(&L6data.dptr[i_l6_load][iL7copy], &L7data.dptr[i_l7_load][iL7copy], sizeof(REAL)*ncube*ncube*nyL6);
  int idx_l7, idtofetch_l7;
  int i7offset = (nxL7-2) / (ncube);
  for(idx_l7 = 0; idx_l7 < 4; idx_l7++)
  {
    idtofetch_l7 = idx_l7 + 1; 
    int idx_fetch_l7 = idtofetch_l7 / 2;
    int idy_fetch_l7 = idtofetch_l7 % 2;;
    int idx_store_l7 = idx_l7 / 2;
    int idy_store_l7 = idx_l7 % 2;;
    // flip double buffer index
    i_l6_load ^= 1;
    i_l6_comp ^= 1;
    // fetch for next chunk
    if(idx_l7 != 3)
    {
      //printf("copying L7 %d %d %d\n",idx_fetch_l7, idy_fetch_l7,i7offset);
      for(iL7copy = 0; iL7copy < nxL6; ++iL7copy)
        memcpy(&L6data.dptr[i_l6_load][iL7copy], &L7data.dptr[i_l7_load][iL7copy+idx_fetch_l7*i7offset][idy_fetch_l7*i7offset], sizeof(REAL)*ncube*ncube*nyL6);
    }
    // working on the computation for next level
  {
/******   starting Level-6    ******/
  struct L5struct L5data;
  L5data.dptr = (REAL (*)[nxL5][nyL5][ncube][ncube]) malloc(sizeof(REAL)*nxL5*nyL5*ncube*ncube*2);
  int i_l5_load = 0;
  int i_l5_comp = 1;
  //prefetch first chunk
  int iL6copy;
  for(iL6copy = 0; iL6copy < nxL5; ++iL6copy)
    memcpy(&L5data.dptr[i_l5_load][iL6copy], &L6data.dptr[i_l6_load][iL6copy], sizeof(REAL)*ncube*ncube*nyL5);
  int idx_l6, idtofetch_l6;
  int i6offset = (nxL6-2) / (ncube);
  for(idx_l6 = 0; idx_l6 < 4; idx_l6++)
  {
    idtofetch_l6 = idx_l6 + 1; 
    int idx_fetch_l6 = idtofetch_l6 / 2;
    int idy_fetch_l6 = idtofetch_l6 % 2;;
    int idx_store_l6 = idx_l6 / 2;
    int idy_store_l6 = idx_l6 % 2;;
    // flip double buffer index
    i_l5_load ^= 1;
    i_l5_comp ^= 1;
    // fetch for next chunk
    if(idx_l6 != 3)
    {
      //printf("copying L6 %d %d %d\n",idx_fetch_l6, idy_fetch_l6,i6offset);
      for(iL6copy = 0; iL6copy < nxL5; ++iL6copy)
        memcpy(&L5data.dptr[i_l5_load][iL6copy], &L6data.dptr[i_l6_load][iL6copy+idx_fetch_l6*i6offset][idy_fetch_l6*i6offset], sizeof(REAL)*ncube*ncube*nyL5);
    }
    // working on the computation for next level
  {
/******   starting Level-5    ******/
  struct L4struct L4data;
  L4data.dptr = (REAL (*)[nxL4][nyL4][ncube][ncube]) malloc(sizeof(REAL)*nxL4*nyL4*ncube*ncube*2);
  int i_l4_load = 0;
  int i_l4_comp = 1;
  //prefetch first chunk
  int iL5copy;
  for(iL5copy = 0; iL5copy < nxL4; ++iL5copy)
    memcpy(&L4data.dptr[i_l4_load][iL5copy], &L5data.dptr[i_l5_load][iL5copy], sizeof(REAL)*ncube*ncube*nyL4);
  int idx_l5, idtofetch_l5;
  int i5offset = (nxL5-2) / (ncube);
  for(idx_l5 = 0; idx_l5 < 4; idx_l5++)
  {
    idtofetch_l5 = idx_l5 + 1; 
    int idx_fetch_l5 = idtofetch_l5 / 2;
    int idy_fetch_l5 = idtofetch_l5 % 2;;
    int idx_store_l5 = idx_l5 / 2;
    int idy_store_l5 = idx_l5 % 2;;
    // flip double buffer index
    i_l4_load ^= 1;
    i_l4_comp ^= 1;
    // fetch for next chunk
    if(idx_l5 != 3)
    {
      //printf("copying L5 %d %d %d\n",idx_fetch_l5, idy_fetch_l5,i5offset);
      for(iL5copy = 0; iL5copy < nxL4; ++iL5copy)
        memcpy(&L4data.dptr[i_l4_load][iL5copy], &L5data.dptr[i_l5_load][iL5copy+idx_fetch_l5*i5offset][idy_fetch_l5*i5offset], sizeof(REAL)*ncube*ncube*nyL4);
    }
    // working on the computation for next level
  {
/******   starting Level-4    ******/
  struct L3struct L3data;
  L3data.dptr = (REAL (*)[nxL3][nyL3][ncube][ncube]) malloc(sizeof(REAL)*nxL3*nyL3*ncube*ncube*2);
  int i_l3_load = 0;
  int i_l3_comp = 1;
  //prefetch first chunk
  int iL4copy;
  for(iL4copy = 0; iL4copy < nxL3; ++iL4copy)
    memcpy(&L3data.dptr[i_l3_load][iL4copy], &L4data.dptr[i_l4_load][iL4copy], sizeof(REAL)*ncube*ncube*nyL3);
  int idx_l4, idtofetch_l4;
  int i4offset = (nxL4-2) / (ncube);
  for(idx_l4 = 0; idx_l4 < 4; idx_l4++)
  {
    idtofetch_l4 = idx_l4 + 1; 
    int idx_fetch_l4 = idtofetch_l4 / 2;
    int idy_fetch_l4 = idtofetch_l4 % 2;;
    int idx_store_l4 = idx_l4 / 2;
    int idy_store_l4 = idx_l4 % 2;;
    // flip double buffer index
    i_l3_load ^= 1;
    i_l3_comp ^= 1;
    // fetch for next chunk
    if(idx_l4 != 3)
    {
      //printf("copying L4 %d %d %d\n",idx_fetch_l4, idy_fetch_l4,i4offset);
      for(iL4copy = 0; iL4copy < nxL3; ++iL4copy)
        memcpy(&L3data.dptr[i_l3_load][iL4copy], &L4data.dptr[i_l4_load][iL4copy+idx_fetch_l4*i4offset][idy_fetch_l4*i4offset], sizeof(REAL)*ncube*ncube*nyL3);
    }
    // working on the computation for next level
  {
/******   starting Level-3    ******/
  struct L2struct L2data;
  L2data.dptr = (REAL (*)[nxL2][nyL2][ncube][ncube]) malloc(sizeof(REAL)*nxL2*nyL2*ncube*ncube*2);
  int i_l2_load = 0;
  int i_l2_comp = 1;
  //prefetch first chunk
  int iL3copy;
  for(iL3copy = 0; iL3copy < nxL2; ++iL3copy)
    memcpy(&L2data.dptr[i_l2_load][iL3copy], &L3data.dptr[i_l3_load][iL3copy], sizeof(REAL)*ncube*ncube*nyL2);
  int idx_l3, idtofetch_l3;
  int i3offset = (nxL3-2) / (ncube);
  for(idx_l3 = 0; idx_l3 < 4; idx_l3++)
  {
    idtofetch_l3 = idx_l3 + 1; 
    int idx_fetch_l3 = idtofetch_l3 / 2;
    int idy_fetch_l3 = idtofetch_l3 % 2;;
    int idx_store_l3 = idx_l3 / 2;
    int idy_store_l3 = idx_l3 % 2;;
    // flip double buffer index
    i_l2_load ^= 1;
    i_l2_comp ^= 1;
    // fetch for next chunk
    if(idx_l3 != 3)
    {
      //printf("copying L3 %d %d %d\n",idx_fetch_l3, idy_fetch_l3,i3offset);
      for(iL3copy = 0; iL3copy < nxL2; ++iL3copy)
        memcpy(&L2data.dptr[i_l2_load][iL3copy], &L3data.dptr[i_l3_load][iL3copy+idx_fetch_l3*i3offset][idy_fetch_l3*i3offset], sizeof(REAL)*ncube*ncube*nyL2);
    }
    // working on the computation for next level
  {
/******   starting Level-2    ******/
  struct L1struct L1data;
  L1data.dptr = (REAL (*)[nxL1][nyL1][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL1*nyL1*2);
  int i_l1_load = 0;
  int i_l1_comp = 1;
  //prefetch first chunk
  int iL2copy;
  for(iL2copy = 0; iL2copy < nxL1; ++iL2copy)
    memcpy(&L1data.dptr[i_l1_load][iL2copy], &L2data.dptr[i_l2_load][iL2copy], sizeof(REAL)*ncube*ncube*nyL1);
  int idx_l2, idtofetch_l2;
  int i2offset = (nxL2-2) / (ncube);
  for(idx_l2 = 0; idx_l2 < 4; idx_l2++)
  {
    idtofetch_l2 = idx_l2 + 1; 
    int idx_fetch_l2 = idtofetch_l2 / 2;
    int idy_fetch_l2 = idtofetch_l2 % 2;;
    int idx_store_l2 = idx_l2 / 2;
    int idy_store_l2 = idx_l2 % 2;;
    // flip double buffer index
    i_l1_load ^= 1;
    i_l1_comp ^= 1;
    // fetch for next chunk
    if(idx_l2 != 3)
    {
      //printf("copying L2 %d %d %d\n",idx_fetch_l2, idy_fetch_l2,i2offset);
      for(iL2copy = 0; iL2copy < nxL1; ++iL2copy)
        memcpy(&L1data.dptr[i_l1_load][iL2copy], &L2data.dptr[i_l2_load][iL2copy+idx_fetch_l2*i2offset][idy_fetch_l2*i2offset], sizeof(REAL)*ncube*ncube*nyL1);
    }
    // working on the computation for next level
  {
/******   starting Level-1    ******/
  struct L0struct L0data;
  L0data.dptr = (REAL (*)[nxL0][nyL0][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL0*nyL0*3);
  int i_l0_load = 0;
  int i_l0_comp = 1;
  int i_l0_store = 2;
  //prefetch first chunk
  int iL1copy;
  for(iL1copy = 0; iL1copy < nxL0; ++iL1copy)
    memcpy(&L0data.dptr[i_l0_load][iL1copy], &L1data.dptr[i_l1_load][iL1copy], sizeof(REAL)*ncube*ncube*nyL0);
  int idx_l1, idtofetch_l1;
  int i1offset = (nxL1-2) / (ncube);
  for(idx_l1 = 0; idx_l1 < 4; idx_l1++)
  {
    idtofetch_l1 = idx_l1 + 1; 
    int idx_fetch_l1 = idtofetch_l1 / 2;
    int idy_fetch_l1 = idtofetch_l1 % 2;;
    int idx_store_l1 = idx_l1 / 2;
    int idy_store_l1 = idx_l1 % 2;;
    // rotate triple buffer index only for L0
    i_l0_load = (i_l0_load == 0) ? 2:(i_l0_load-1);
    i_l0_comp = (i_l0_comp == 0) ? 2:(i_l0_comp-1);
    i_l0_store = (i_l0_store == 0) ? 2:(i_l0_store-1);
    // fetch for next chunk
    if(idx_l1 != 3)
    {
      //printf("copying L1 %d %d %d\n",idx_fetch_l1, idy_fetch_l1,i1offset);
      for(iL1copy = 0; iL1copy < nxL0; ++iL1copy)
        memcpy(&L0data.dptr[i_l0_load][iL1copy], &L1data.dptr[i_l1_load][iL1copy+idx_fetch_l1*i1offset][idy_fetch_l1*i1offset], sizeof(REAL)*ncube*ncube*nyL0);
    }
    // working on the computation for final level
    computeL0(L0data.dptr, i_l0_comp, i_l0_store);
 
    // storing back the computed result
    printf("storing back to L1 %d %d\n",idx_store_l1,idy_store_l1);
    for(iL1copy = 0; iL1copy < nxL0-1; ++iL1copy)
      memcpy(&L1data.dptr[i_l1_comp][iL1copy+idx_store_l1*i1offset+1][idy_store_l1*i1offset+1], &L0data.dptr[i_l0_store][iL1copy+1][1], sizeof(REAL)*ncube*ncube*(nyL0-1));
  }
  free(L0data.dptr);
/******   end Level-1    ******/
  } 
 
    printf("storing back to L2 %d %d\n",idx_store_l2,idy_store_l2);
    for(iL2copy = 0; iL2copy < nxL1-2; ++iL2copy)
      memcpy(&L2data.dptr[i_l2_comp][iL2copy+idx_store_l2*i2offset+1][idy_store_l2*i2offset+1], &L1data.dptr[i_l1_comp][iL2copy+1][1], sizeof(REAL)*ncube*ncube*(nyL1-2));
  }
  free(L1data.dptr);
/******   end Level-2    ******/
  } 
 
    printf("storing back to L3 %d %d\n",idx_store_l3,idy_store_l3);
    for(iL3copy = 0; iL3copy < nxL2-2; ++iL3copy)
      memcpy(&L3data.dptr[i_l3_comp][iL3copy+idx_store_l3*i3offset+1][idy_store_l3*i3offset+1], &L2data.dptr[i_l2_comp][iL3copy+1][1], sizeof(REAL)*ncube*ncube*(nyL2-2));
  }
  free(L2data.dptr);
/******   end Level-3    ******/
  } 
 
    printf("storing back to L4 %d %d\n",idx_store_l4,idy_store_l4);
    for(iL4copy = 0; iL4copy < nxL3-2; ++iL4copy)
      memcpy(&L4data.dptr[i_l4_comp][iL4copy+idx_store_l4*i4offset+1][idy_store_l4*i4offset+1], &L3data.dptr[i_l3_comp][iL4copy+1][1], sizeof(REAL)*ncube*ncube*(nyL3-2));
  }
  free(L3data.dptr);
/******   end Level-4    ******/
  } 
 
    printf("storing back to L5 %d %d\n",idx_store_l5,idy_store_l5);
    for(iL5copy = 0; iL5copy < nxL4-2; ++iL5copy)
      memcpy(&L5data.dptr[i_l5_comp][iL5copy+idx_store_l5*i5offset+1][idy_store_l5*i5offset+1], &L4data.dptr[i_l4_comp][iL5copy+1][1], sizeof(REAL)*ncube*ncube*(nyL4-2));
  }
  free(L4data.dptr);
/******   end Level-5    ******/
  } 
 
    printf("storing back to L6 %d %d\n",idx_store_l6,idy_store_l6);
    for(iL6copy = 0; iL6copy < nxL5-2; ++iL6copy)
      memcpy(&L6data.dptr[i_l6_comp][iL6copy+idx_store_l6*i6offset+1][idy_store_l6*i6offset+1], &L5data.dptr[i_l5_comp][iL6copy+1][1], sizeof(REAL)*ncube*ncube*(nyL5-2));
  }
  free(L5data.dptr);
/******   end Level-6    ******/
  } 
 
    printf("storing back to L7 %d %d\n",idx_store_l7,idy_store_l7);
    for(iL7copy = 0; iL7copy < nxL6-2; ++iL7copy)
      memcpy(&L7data.dptr[i_l7_comp][iL7copy+idx_store_l7*i7offset+1][idy_store_l7*i7offset+1], &L6data.dptr[i_l6_comp][iL7copy+1][1], sizeof(REAL)*ncube*ncube*(nyL6-2));
  }
  free(L6data.dptr);
/******   end Level-7    ******/
  } 
 
    printf("storing back to L8 %d %d\n",idx_store_l8,idy_store_l8);
    for(iL8copy = 0; iL8copy < nxL7-2; ++iL8copy)
      memcpy(&L8data.dptr[i_l8_comp][iL8copy+idx_store_l8*i8offset+1][idy_store_l8*i8offset+1], &L7data.dptr[i_l7_comp][iL8copy+1][1], sizeof(REAL)*ncube*ncube*(nyL7-2));
  }
  free(L7data.dptr);
/******   end Level-8    ******/
  } 
    printf("storing back to L9 %d %d\n",idx_store_l9,idy_store_l9);
    for(iL9copy = 0; iL9copy < nxL8-2; ++iL9copy)
      memcpy(&L9data.dptr[i_l9_comp][iL9copy+idx_store_l9*i9offset+1][idy_store_l9*i9offset+1], &L8data.dptr[i_l8_comp][iL9copy+1][1], sizeof(REAL)*ncube*ncube*(nyL8-2));
  }
  free(L8data.dptr);
/******   end Level-9    ******/
  } 
    printf("storing back to L10 %d %d\n",idx_store_l10,idy_store_l10);
    for(iL10copy = 0; iL10copy < nxL9-2; ++iL10copy)
      memcpy(&L10data.dptr[i_l10_comp][iL10copy+idx_store_l10*i10offset+1][idy_store_l10*i10offset+1], &L9data.dptr[i_l9_comp][iL10copy+1][1], sizeof(REAL)*ncube*ncube*(nyL9-2));
  }
  free(L9data.dptr);
/******   end Level-10    ******/
  free(L10data.dptr);
  return 0;
}
