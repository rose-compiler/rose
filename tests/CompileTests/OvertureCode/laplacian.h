/*  -*-Mode: c++; -*-  */
#ifndef LAPLACIAN_H
#define LAPLACIAN_H
// ================================================================================
// inline functions for intermediate results in Laplacian operators; 
// these are class functions so that we don't have to pass values.
// ================================================================================

#undef ForDplus
#define ForDplus(i) for(i=0; i<=1; i++)
#undef ForDminus
#define ForDminus(i) for(i=-1; i<=0; i++)

inline int Delta(int i,int j)   {return ((i==j) ? 1 : 0 );}
inline int Dplus(int i)         {return (Delta(i,1) - Delta(i,0));}
inline int Dminus(int i)        {return (Delta(i,0) - Delta(i,-1));}


// ... average sx to rx locations

inline real RAverage(int i,int j){return ( (real).25*(                          //use ForDplus(j) { ForDminus(i) {
                                               Delta(i, 0)*Delta(j,0) + // x x o
                                               Delta(i,-1)*Delta(j,0) + // x x o
                                               Delta(i, 0)*Delta(j,1) + // o o o
                                               Delta(i,-1)*Delta(j,1))
                                          );}

// ... average rx to sx locations

inline real SAverage(int i,int j){return ( (real).25*(                          //use ForDplus(i) { ForDminus(j) {
                                               Delta(j, 0)*Delta(i,0) + // o o o
                                               Delta(j,-1)*Delta(i,0) + // o x x
                                               Delta(j, 0)*Delta(i,1) + // o x x
                                               Delta(j,-1)*Delta(i,1))
                                          );}

inline real RSAverage(int i, int j, int k){ return (real).25*(                  //use ForDplus(i) { For Dminus(j) {
                                  Delta(i, 0)*Delta(j, 0)*Delta(k, 0)   //R-derivatives in S-direction
                                + Delta(i, 0)*Delta(j,-1)*Delta(k, 0)
                                + Delta(i, 1)*Delta(j, 0)*Delta(k, 0)
                                + Delta(i, 1)*Delta(j,-1)*Delta(k, 0)
                                );}

inline real SRAverage(int i, int j, int k){ return (real).25*(                  //use ForDplus(j) { For Dminus(i) {
                                  Delta(i, 0)*Delta(j, 0)*Delta(k, 0)   //S-derivatives in R-direction
                                + Delta(i,-1)*Delta(j, 0)*Delta(k, 0)
                                + Delta(i, 0)*Delta(j, 1)*Delta(k, 0)
                                + Delta(i,-1)*Delta(j, 1)*Delta(k, 0)
                                );}

inline real RTAverage(int i, int j, int k){ return (real).25*(                  //use ForDplus(i) { ForDminus(k) {
                                  Delta(i, 0)*Delta(j, 0)*Delta(k, 0)   //R-derivatives in T-direction
                                + Delta(i, 0)*Delta(j, 0)*Delta(k,-1)
                                + Delta(i, 1)*Delta(j, 0)*Delta(k, 0)
                                + Delta(i, 1)*Delta(j, 0)*Delta(k,-1)
                                );}

inline real TRAverage(int i, int j, int k){ return (real).25*(                  //use ForDplus(k) { ForDminus(i) {
                                  Delta(i, 0)*Delta(j, 0)*Delta(k, 0)   //T-derivatives in R-direction
                                + Delta(i,-1)*Delta(j, 0)*Delta(k, 0)
                                + Delta(i, 0)*Delta(j, 0)*Delta(k, 1)
                                + Delta(i,-1)*Delta(j, 0)*Delta(k, 1)
                                );}

inline real STAverage(int i, int j, int k){ return (real).25*(
                                  Delta(i, 0)*Delta(j, 0)*Delta(k, 0)
                                + Delta(i, 0)*Delta(j, 0)*Delta(k,-1)
                                + Delta(i, 0)*Delta(j, 1)*Delta(k, 0)
                                + Delta(i, 0)*Delta(j, 1)*Delta(k,-1)
                                );}

inline real TSAverage(int i, int j, int k){ return (real).25*(
                                  Delta(i, 0)*Delta(j, 0)*Delta(k, 0)
                                + Delta(i, 0)*Delta(j,-1)*Delta(k, 0)
                                + Delta(i, 0)*Delta(j, 0)*Delta(k, 1)
                                + Delta(i, 0)*Delta(j,-1)*Delta(k, 1)
                                );}

inline int coeff(int i,int j)           {return (i+1) + 3*(j+1);}  // maps (-1:1,-1:1) to (0:8)
inline int coeff(int i,int j,int k)     {return (i+1) + 3*((j+1) + 3*(k+1)); } //similar to above

#define CF(i,j,k) (i+halfWidth1 + width*((j+halfWidth2) + width*(k+halfWidth3)))
#endif
