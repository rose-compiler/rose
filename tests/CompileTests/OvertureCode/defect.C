#include "Ogmg.h"

//\begin{>>OgmgInclude.tex}{\subsection{defect(level)}}
void Ogmg::
defect(const int & level)
//---------------------------------------------------------------------------------------------
// /Description:
//    Defect computation 
//
//   Fill in  defectMG[level] with f-Lu
//\end{OgmgInclude.tex} 
//---------------------------------------------------------------------------------------------
{
  if( Ogmg::debug & 16 )
    printf("%*.1s Ogmg:defect, level = %i, ",level*2,"  ",level);
  for( int grid=0; grid<mgcg.multigridLevel[level].numberOfComponentGrids(); grid++ )
    defect(level,grid);

  defectMG.multigridLevel[level].periodicUpdate(); // **** is this needed ?? *****

}

#undef C
#undef M123
#define M123(m1,m2,m3) (m1+halfWidth1+width1*(m2+halfWidth2+width2*(m3+halfWidth3)))
// define C(m1,m2,m3,I1,I2,I3) c(I1,I2,I3,M123(m1,m2,m3))
#define C(m1,m2,m3,I1,I2,I3) c(M123(m1,m2,m3),I1,I2,I3)
// define C(m1,m2,m3,I1,I2,I3) c(M123(m1,m2,m3),I1,I2,I3)


//\begin{>>OgmgInclude.tex}{\subsection{defect(level,grid)}}
void Ogmg::
defect(const int & level, const int & grid)
//---------------------------------------------------------------------------------------------
// /Description:
//    Defect computation on a component grid
//
//    Compute defectMG.multigridLevel[level][grid]
//\end{OgmgInclude.tex} 
//---------------------------------------------------------------------------------------------
{
  real time=getCPU();

  realArray & u = uMG.multigridLevel[level][grid];
  realArray & f = fMG.multigridLevel[level][grid];     // *** use f at level 1!
  realMappedGridFunction & c =  level==0 ? cMG[grid] : cMG.multigridLevel[level][grid];
  MappedGrid & mg = mgcg.multigridLevel[level][grid];  
  realArray & defect = defectMG.multigridLevel[level][grid];

  Index I1,I2,I3;
  getIndex(mg.extendedIndexRange(),I1,I2,I3);

  defect=0.;                   // ********************* fix this ***********
  getDefect(level,grid,f,u,I1,I2,I3,defect);

  if( Ogmg::debug & 16 )
  {
    real maximumDefect=max(fabs(defect(I1,I2,I3)));
    printf("maximumDefect = %e \n",maximumDefect);
  }
  
  if( Ogmg::debug & 64 )
  {
    u.display("defect: Here is u");
    defect.display("defect: Here is the defect");
  }
  
  tm(timeForDefect)+=getCPU()-time;
}


//\begin{>>OgmgInclude.tex}{\subsection{getDefect}}
void Ogmg::
getDefect(const int & level, 
          const int & grid, 
          realArray & f,     
          realArray & u, 
          const Index & I1,
          const Index & I2,
          const Index & I3,
          realArray & defect,
          const int lineSmoothOption)
//==================================================================================
// /Description:
//    Defect computation on a component grid
//
//   Determine the defect = f - C*u
//
//   This routine knows how to efficiently compute the defect for rectangular
//   and non-rectangular grids. It also knows how to compute the defect for
//   line smoothers
// Input -
//   level,grid
//   f,u
//   I1,I2,I3
//   lineSmoothOption = -1 :
//                    = 0,1,2 : compute defect for line solve in direction 0,1,2
// Output -
//   defect
//
//\end{OgmgInclude.tex} 
//==================================================================================
{
  realArray & c =   level==0 ? cMG[grid] : cMG.multigridLevel[level][grid];
  MappedGrid & mg = mgcg.multigridLevel[level][grid];  

  u.reshape(1,u.dimension(0),u.dimension(1),u.dimension(2));
  defect.reshape(1,defect.dimension(0),defect.dimension(1),defect.dimension(2));
  f.reshape(1,f.dimension(0),f.dimension(1),f.dimension(2));
  
  evaluateTheDefectFormula(level,grid,c,u,f,defect,mg,I1,I2,I3,I1,I2,I3,lineSmoothOption);

  u.reshape(u.dimension(1),u.dimension(2),u.dimension(3));
  defect.reshape(defect.dimension(1),defect.dimension(2),defect.dimension(3));
  f.reshape(f.dimension(1),f.dimension(2),f.dimension(3));
  
  // @PD realArray3[defect] Range[I1,I2,I3]


  // ------------ zero out unused points -----------------
  where( mg.mask()(I1,I2,I3)<=0 )   // ****************************************************** is this needed ????
  {
    defect(I1,I2,I3)=0.;  //  @PAW
  }
}

// @PD realArray4[defect,f,c,u] Range[I1,I2,I3,I1u,I2u,I3u] 


void Ogmg::
evaluateTheDefectFormula(const int & level, 
                         const int & grid, 
                         const realArray & c,
                         const realArray & u,  
                         const realArray & f, 
                         realArray & defect, 
                         MappedGrid & mg,
                         const Index & I1,
                         const Index & I2,
                         const Index & I3,
                         const Index & I1u,
                         const Index & I2u,
                         const Index & I3u,
                         const int lineSmoothOption)
// ==================================================================================================
//   /Description:
//     This routine actually evaluates the defect for various cases, lineSmooth or not. It can be used
//     to evaluate the defect on neumann BC's using I1u,I2u,I3u.
//
// /I1,I2,I3 (input) : evaluate defect, f and c at these Index's
// /I1u,I2u,I3u (input) : evaluate u here
// ==================================================================================================
{
          
  // ****************** Defect is wrong for extrapolation equations on ghost lines *****************
  realMappedGridFunction & cmg = level==0 ? cMG[grid] : cMG.multigridLevel[level][grid];

  // The matrix generated on extra levels by operator averaging will have a full stencil
  // and thus will not count as "rectangular"
  const int rectangular=(*cmg.getOperators()).isRectangular() &&
                        ( level < mgcg.numberOfMultigridLevels()-numberOfExtraLevels ); 
  // const int rectangular=FALSE;
  if( Ogmg::debug & 8 )
    fprintf(debugFile,"%*.1s evaluateTheDefectFormula:level=%i, grid=%i, rectangular=%i \n",level*4," ",
           level,grid,rectangular);
  if( Ogmg::debug & 64 )
  {
    if( rectangular )
      display(isConstantCoefficients,"isConstantCoefficients",debugFile);
    if( rectangular && isConstantCoefficients(grid) )
    {
      
      display(constantCoefficients,sPrintF(buff,"constantCoefficients for level=%i ",level),debugFile);
    }
  }
  


  if( lineSmoothOption<-1 || lineSmoothOption >= mg.numberOfDimensions() )
  {
    cout << "Ogmg::getDefect:ERROR invalid value for lineSmoothOption=" << lineSmoothOption << endl;
    exit(1);
  }

  if( mg.numberOfDimensions()==2 )
  {
    if( rectangular )
    {
      if( isConstantCoefficients(grid) )
      {
        if( lineSmoothOption==-1 )
        { // general defect
          defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
            constantCoefficients(M123( 0, 0,0),grid)*u(0,I1u  ,I2u  ,I3u)+   // **** could be wrong, assumes 5 pt
            constantCoefficients(M123( 1, 0,0),grid)*u(0,I1u+1,I2u  ,I3u)+
            constantCoefficients(M123( 0, 1,0),grid)*u(0,I1u  ,I2u+1,I3u)+
            constantCoefficients(M123(-1, 0,0),grid)*u(0,I1u-1,I2u  ,I3u)+
            constantCoefficients(M123( 0,-1,0),grid)*u(0,I1u  ,I2u-1,I3u)
            );
        }
        else if( lineSmoothOption==0 )
        { // defect for line smooth in direction 0
          defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
            c(M123( 0, 1,0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u)+
            c(M123( 0,-1,0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u)
            );
        }
        else if( lineSmoothOption==1 )
        { // defect for line smooth in direction 1
          defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
            c(M123( 1, 0,0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u)+
            c(M123(-1, 0,0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u)
            );
        }
        else
        {
          throw "error";
        }
        // **** do any boundaries ****
        Index Ib1,Ib2,Ib3;
        int side,axis;
        for( axis=0; axis<mg.numberOfDimensions(); axis++ )
        {
          for( side=0; side<=1; side++ )
          {
            if( boundaryCondition(side,axis)>0 && boundaryCondition(side,axis)==extrapolation )
            {
              getBoundaryIndex(mg.gridIndexRange(),side,axis,Ib1,Ib2,Ib3);
              defect(0,Ib1,Ib2,Ib3)=f(0,Ib1,Ib2,Ib3)-(
                c(M123( 0, 0,0),Ib1,Ib2,Ib3)*u(0,Ib1  ,Ib2  ,Ib3)+
                c(M123( 1, 0,0),Ib1,Ib2,Ib3)*u(0,Ib1+1,Ib2  ,Ib3)+
                c(M123( 0, 1,0),Ib1,Ib2,Ib3)*u(0,Ib1  ,Ib2+1,Ib3)+
                c(M123(-1, 0,0),Ib1,Ib2,Ib3)*u(0,Ib1-1,Ib2  ,Ib3)+
                c(M123( 0,-1,0),Ib1,Ib2,Ib3)*u(0,Ib1  ,Ib2-1,Ib3)
                );
            }
          }
        }
      }
      else
      {
        if( level>=2 && debug & 16 )
        {
          display(u,sPrintF(buff," evaluate defect level=%i : u",level),debugFile,"%9.1e");
          display(f,sPrintF(buff," evaluate defect level=%i : f",level),debugFile,"%9.1e");
          display(c,sPrintF(buff," evaluate defect level=%i : c",level),debugFile,"%9.1e");
        }
        

        if( lineSmoothOption==-1 )
        { // general defect
          defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(                     // @PA
            c(M123( 0, 0,0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u)+
            c(M123( 1, 0,0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u)+
            c(M123( 0, 1,0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u)+
            c(M123(-1, 0,0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u)+
            c(M123( 0,-1,0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u)
            );
/* ----
            const int i1Bound=I1.getBound();
            const int i2Bound=I2.getBound();
            const int i3Bound=I3.getBound();
            const int 
            for( int i3=I3.getBase(); i3<i3Bound; i3+=stride3 )
            for( int i2=I2.getBase(); i2<i2Bound; i2+=stride2 )
            for( int i1=I1.getBase(); i1<i1Bound; i1+=stride1 )
            {
              defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
                c(M123( 0, 0,0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u)+
                c(M123( 1, 0,0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u)+
                c(M123( 0, 1,0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u)+
                c(M123(-1, 0,0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u)+
                c(M123( 0,-1,0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u)
                );
            }
----- */    
        }
        else if( lineSmoothOption==0 )
        { // defect for line smooth in direction 0
          defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
             c(M123( 0, 1,0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u)
            +c(M123( 0,-1,0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u)
            );
        }
        else if( lineSmoothOption==1 )
        { // defect for line smooth in direction 1
          defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
             c(M123( 1, 0,0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u)
            +c(M123(-1, 0,0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u)
            );
        }
        else
        {
          throw "error";
        }
      }
    }
    else
    {
      if( lineSmoothOption==-1 )
      { // general 3x3 defect
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(                  // @PA
           c(M123( 0, 0,0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u)
          +c(M123( 1, 0,0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u)
          +c(M123( 0, 1,0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u)
          +c(M123(-1, 0,0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u)
          +c(M123( 0,-1,0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u)
          +c(M123( 1, 1,0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u)
          +c(M123( 1,-1,0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u)
          +c(M123(-1, 1,0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u)
          +c(M123(-1,-1,0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u)
          );
      }
      else if( lineSmoothOption==0 )
      { // defect for line smooth in direction 0
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
           c(M123( 0, 1,0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u)
          +c(M123( 0,-1,0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u)
          +c(M123( 1, 1,0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u)
          +c(M123( 1,-1,0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u)
          +c(M123(-1, 1,0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u)
          +c(M123(-1,-1,0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u)
          );
      }
      else if( lineSmoothOption==1 )
      { // defect for line smooth in direction 1
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
           c(M123( 1, 0,0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u)
          +c(M123(-1, 0,0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u)
          +c(M123( 1, 1,0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u)
          +c(M123( 1,-1,0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u)
          +c(M123(-1, 1,0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u)
          +c(M123(-1,-1,0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u)
          );
      }
      else
      {
        cout << "Ogmg:evaluateTheDefectFormula:ERROR: invalid lineSmoothOption \n";
        throw "error";
      }
      
    }
  }
  else // ---- 3D -----
  {
    if( rectangular )
    {
      if( lineSmoothOption==-1 )
      { // general defect
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(                    // @PA
           c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
          +c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
          +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          );
      }
      else if( lineSmoothOption==0 )
      { // defect for line smooth in direction 0
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
          // c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
          // +c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
           c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          // +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
          +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          );
      }
      else if( lineSmoothOption==1 )
      { // defect for line smooth in direction 1
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
          // c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
           c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          // +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
          // +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          );
      }
      else if( lineSmoothOption==2 )
      { // defect for line smooth in direction 2
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
          // c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
           c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
          +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          // +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          // +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          );
      }
      else
      {
        cout << "Ogmg:evaluateTheDefectFormula:ERROR: invalid lineSmoothOption \n";
        throw "error";
      }
    }
    else
    {
      if( lineSmoothOption==-1 )
      { // general defect
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(                    // @PA
           c(M123(-1,-1,-1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u-1)
          +c(M123( 0,-1,-1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u-1)
          +c(M123( 1,-1,-1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u-1)
          +c(M123(-1, 0,-1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u-1)
          +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          +c(M123( 1, 0,-1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u-1)
          +c(M123(-1, 1,-1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u-1)
          +c(M123( 0, 1,-1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u-1)
          +c(M123( 1, 1,-1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u-1)

          +c(M123(-1,-1, 0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u  )
          +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 1,-1, 0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u  )
          +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
          +c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
          +c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          +c(M123(-1, 1, 0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u  )
          +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123( 1, 1, 0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u  )

          +c(M123(-1,-1, 1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u+1)
          +c(M123( 0,-1, 1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u+1)
          +c(M123( 1,-1, 1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u+1)
          +c(M123(-1, 0, 1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u+1)
          +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 1, 0, 1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u+1)
          +c(M123(-1, 1, 1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u+1)
          +c(M123( 0, 1, 1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u+1)
          +c(M123( 1, 1, 1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u+1)
          );
      }
      else if( lineSmoothOption==0 )
      { // defect for line smooth in direction 0
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
           c(M123(-1,-1,-1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u-1)
          +c(M123( 0,-1,-1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u-1)
          +c(M123( 1,-1,-1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u-1)
          +c(M123(-1, 0,-1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u-1)
          +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          +c(M123( 1, 0,-1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u-1)
          +c(M123(-1, 1,-1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u-1)
          +c(M123( 0, 1,-1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u-1)
          +c(M123( 1, 1,-1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u-1)

          +c(M123(-1,-1, 0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u  )
          +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 1,-1, 0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u  )
           // +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
           // +c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
           // +c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          +c(M123(-1, 1, 0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u  )
          +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123( 1, 1, 0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u  )

          +c(M123(-1,-1, 1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u+1)
          +c(M123( 0,-1, 1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u+1)
          +c(M123( 1,-1, 1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u+1)
          +c(M123(-1, 0, 1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u+1)
          +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 1, 0, 1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u+1)
          +c(M123(-1, 1, 1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u+1)
          +c(M123( 0, 1, 1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u+1)
          +c(M123( 1, 1, 1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u+1)
          );
      }
      else if( lineSmoothOption==1 )
      { // defect for line smooth in direction 1
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
           c(M123(-1,-1,-1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u-1)
          +c(M123( 0,-1,-1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u-1)
          +c(M123( 1,-1,-1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u-1)
          +c(M123(-1, 0,-1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u-1)
          +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          +c(M123( 1, 0,-1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u-1)
          +c(M123(-1, 1,-1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u-1)
          +c(M123( 0, 1,-1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u-1)
          +c(M123( 1, 1,-1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u-1)

          +c(M123(-1,-1, 0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u  )
           // +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 1,-1, 0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u  )
          +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
           // +c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
          +c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          +c(M123(-1, 1, 0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u  )
           // +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123( 1, 1, 0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u  )

          +c(M123(-1,-1, 1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u+1)
          +c(M123( 0,-1, 1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u+1)
          +c(M123( 1,-1, 1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u+1)
          +c(M123(-1, 0, 1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u+1)
          +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 1, 0, 1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u+1)
          +c(M123(-1, 1, 1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u+1)
          +c(M123( 0, 1, 1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u+1)
          +c(M123( 1, 1, 1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u+1)
          );
      }
      else if( lineSmoothOption==2 )
      { // defect for line smooth in direction 1
        defect(0,I1,I2,I3)=f(0,I1,I2,I3)-(
           c(M123(-1,-1,-1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u-1)
          +c(M123( 0,-1,-1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u-1)
          +c(M123( 1,-1,-1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u-1)
          +c(M123(-1, 0,-1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u-1)
           // +c(M123( 0, 0,-1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u-1)
          +c(M123( 1, 0,-1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u-1)
          +c(M123(-1, 1,-1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u-1)
          +c(M123( 0, 1,-1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u-1)
          +c(M123( 1, 1,-1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u-1)

          +c(M123(-1,-1, 0),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u  )
          +c(M123( 0,-1, 0),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u  )
          +c(M123( 1,-1, 0),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u  )
          +c(M123(-1, 0, 0),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u  )
           // +c(M123( 0, 0, 0),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u  )
          +c(M123( 1, 0, 0),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u  )
          +c(M123(-1, 1, 0),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u  )
          +c(M123( 0, 1, 0),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u  )
          +c(M123( 1, 1, 0),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u  )

          +c(M123(-1,-1, 1),I1,I2,I3)*u(0,I1u-1,I2u-1,I3u+1)
          +c(M123( 0,-1, 1),I1,I2,I3)*u(0,I1u  ,I2u-1,I3u+1)
          +c(M123( 1,-1, 1),I1,I2,I3)*u(0,I1u+1,I2u-1,I3u+1)
          +c(M123(-1, 0, 1),I1,I2,I3)*u(0,I1u-1,I2u  ,I3u+1)
           // +c(M123( 0, 0, 1),I1,I2,I3)*u(0,I1u  ,I2u  ,I3u+1)
          +c(M123( 1, 0, 1),I1,I2,I3)*u(0,I1u+1,I2u  ,I3u+1)
          +c(M123(-1, 1, 1),I1,I2,I3)*u(0,I1u-1,I2u+1,I3u+1)
          +c(M123( 0, 1, 1),I1,I2,I3)*u(0,I1u  ,I2u+1,I3u+1)
          +c(M123( 1, 1, 1),I1,I2,I3)*u(0,I1u+1,I2u+1,I3u+1)
          );
      }
      else
      {
        cout << "Ogmg:evaluateTheDefectFormula:ERROR: invalid lineSmoothOption \n";
        throw "error";
      }

    }
  }
}

#undef C
#undef M123

