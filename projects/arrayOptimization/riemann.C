// define BOUNDS_CHECK

//#include <A++.h>
#include "simpleA++.h"
#include <time.h>

#if 0
#ifdef __GNUC__
#define USE_ELSEWHERE_MASK_MACRO
#endif
#ifdef CRAY
#define USE_ELSEWHERE_MASK_MACRO
#endif
#endif


// A++/P++ translation of FORTRAN function for Dinshaw's code!

void One_Dimensional_Goduov_Solver ( int nriemann,
                double prsfac ,
                double rhofloor ,
                double prsfloor ,
                double gamma ,
                int pprieman ,
                int rieiter ,
     doubleArray & rhoremi, doubleArray & rhorepl,
     doubleArray & prsremi, doubleArray & prsrepl,
     doubleArray & vlxremi, doubleArray & vlxrepl,
     doubleArray & vlyremi, doubleArray & vlyrepl,
     doubleArray & vlzremi, doubleArray & vlzrepl,
     doubleArray & rhoflux, doubleArray & engflux,
     doubleArray & vlxflux, doubleArray & vlyflux, 
     doubleArray & vlzflux,
     doubleArray & rho0, doubleArray & prs0, doubleArray & vlx0,
     doubleArray & vly0, doubleArray & vlz0, doubleArray & sdsp0 )
   {
     doubleArray lsdremi  (nriemann+2);
     doubleArray lsdrepl  (nriemann+2);
     doubleArray spdbarmi (nriemann+2);
     doubleArray spdbarpl (nriemann+2);
     doubleArray prsbar   (nriemann+2);
     doubleArray oprsbar  (nriemann+2);
     doubleArray rhobarmi (nriemann+2);
     doubleArray rhobarpl (nriemann+2);
     doubleArray sdbarmi  (nriemann+2);
     doubleArray sdbarpl  (nriemann+2);
     doubleArray vlxbar   (nriemann+2);

     doubleArray wavspd1  (nriemann+2);
     doubleArray wavspd2  (nriemann+2);
     doubleArray wavspd3  (nriemann+2);
     doubleArray wavspd4  (nriemann+2);
     doubleArray wavspd5  (nriemann+2);

     intArray inraref  (nriemann+2);
     intArray indxriem (nriemann+2);
     intArray indxsave (nriemann+2);
     intArray flagriem (nriemann+2);
     intArray nprsbar  (nriemann+2);

     lsdremi  = 0;
     lsdrepl  = 0;
     spdbarmi = 0;
     spdbarpl = 0;
     prsbar   = 0;
     oprsbar  = 0;
     rhobarmi = 0;
     rhobarpl = 0;
     sdbarmi  = 0;
     sdbarpl  = 0;
     vlxbar   = 0;
     wavspd1  = 0;
     wavspd2  = 0;
     wavspd3  = 0;
     wavspd4  = 0;
     wavspd5  = 0;

     int i       = 0;
     int ii      = 0;
     int numconv = 0;
     int ncnew   = 0;

  // Formerly scalar temporaries, now they become arrays, due to the array language.
  // Later we want to look at where they are used so they can be partitioned in an 
  // associated way with the data they will be used with as temporaries!
     doubleArray tempaa   (nriemann);
     doubleArray tempab   (nriemann);
     doubleArray tempac   (nriemann);
     doubleArray tempzmi  (nriemann);
     doubleArray tempzpl  (nriemann);
     doubleArray tempumi  (nriemann);
     doubleArray tempupl  (nriemann);
     doubleArray omprsfac (nriemann);

// ********************************************************************

     omprsfac.fill ( 1.0 - prsfac );

  // Note that "prsbar()" is derived from a purely mechanical condition
  // and doesn't depend on equation of state. The sound speeds of course
  // do depend on an eos.

  // Fix bottom on density and pressure variables.
     Index Generic_Index ( 1 , nriemann, 1 );  // The name is special request from Dinshaw!

     rhoremi (Generic_Index).replace ( rhoremi (Generic_Index) <= rhofloor , rhofloor );

     rhorepl (Generic_Index).replace ( rhorepl (Generic_Index) <= rhofloor , rhofloor );
     prsremi (Generic_Index).replace ( prsremi (Generic_Index) <= prsfloor , prsfloor );
     prsrepl (Generic_Index).replace ( prsrepl (Generic_Index) <= prsfloor , prsfloor );
     lsdremi (Generic_Index) = sqrt ( gamma * prsremi (Generic_Index) * rhoremi (Generic_Index));

     lsdrepl (Generic_Index) = sqrt ( gamma * prsrepl (Generic_Index) * rhorepl (Generic_Index));

     spdbarmi (Generic_Index) = lsdremi (Generic_Index);
     spdbarpl (Generic_Index) = lsdrepl (Generic_Index);

     prsbar (Generic_Index) = ( spdbarpl (Generic_Index) * prsremi  (Generic_Index) + 
                                 spdbarmi (Generic_Index) * prsrepl (Generic_Index) - 
                                 spdbarmi (Generic_Index) * spdbarpl (Generic_Index) * 
                                 ( vlxrepl  (Generic_Index) - vlxremi  (Generic_Index) )) /
                                 ( spdbarmi (Generic_Index) + spdbarpl (Generic_Index) );

  // set a bottom value for it.
     prsbar (Generic_Index).replace ( prsbar (Generic_Index) <= prsfloor , prsfloor );

// ***************************************************************
// *
// * Beginning of Riemann iteration with Colella's (1982)
// * two shock scheme.
// *
  // IF ( pprieman .EQ. 1) THEN
     if ( pprieman == 1)
        {
       // STEP I a)

       // DO 30010 ii = 1, rieiter
          for (int ii = 1; ii <= rieiter; ii++)
             {
               tempaa.fill ( (gamma + 1.0) / (2.0 * gamma) );

               spdbarpl ( Generic_Index) = lsdrepl ( Generic_Index) * 
                    sqrt ( 1.0 + tempaa * ( prsbar ( Generic_Index) / prsrepl ( Generic_Index) - 1.0) );

            // Now evaluate left going shock speed (31gamma) of vanLeer (1979).
               spdbarmi (Generic_Index) = lsdremi (Generic_Index) * 
                    sqrt ( 1.0 + tempaa * ( prsbar ( Generic_Index) / prsremi ( Generic_Index) - 1.0) );

            // Evaluate slopes of the pressure-velocity Hugoniot i.e.
            // (A2gamma) (top) of VanLeer (1979)
               tempzpl = 2.0 * pow (spdbarpl ( Generic_Index),3.0) / 
                         ( pow (lsdrepl (Generic_Index),2.0) + pow (spdbarpl (Generic_Index),2.0) );

               tempzmi = 2.0 * pow (spdbarmi (Generic_Index),3.0) / 
                         ( pow (lsdremi (Generic_Index),2.0) + pow (spdbarmi (Generic_Index),2.0) );

            // Now evaluate intermediate velocities using (25) of VanLeer with +, -
            // signs resp'ly for right and left states.
               tempupl = vlxrepl (Generic_Index) + ( prsbar (Generic_Index) - 
                         prsrepl (Generic_Index)) / spdbarpl (Generic_Index);

               tempumi = vlxremi (Generic_Index) - ( prsbar (Generic_Index) - 
                         prsremi (Generic_Index)) / spdbarmi (Generic_Index);

            // Evaluate "prsbar" using (A1) of vanLeer(1979).
               prsbar (Generic_Index) = prsbar (Generic_Index) - tempzmi * tempzpl * 
                                      ( tempupl - tempumi ) / ( tempzmi + tempzpl );

            // And set a bottom value for it.
               prsbar ( Generic_Index).replace ( prsbar ( Generic_Index) <= prsfloor , prsfloor );

            // 30010 CONTINUE
             }

       // Lastly find converged wave speeds in either direction.

          tempaa.fill ( (gamma + 1.0) / (2.0 * gamma) );

       // first evaluate right going shock speed (31gamma) of vanLeer(1979).
          spdbarpl (Generic_Index) = lsdrepl (Generic_Index) * 
               sqrt ( 1.0 + tempaa * ( prsbar (Generic_Index) / prsrepl (Generic_Index) - 1.0) ) ;

       // now evaluate left going shock speed (31gamma) of vanLeer (1979).
          spdbarmi (Generic_Index) = lsdremi (Generic_Index) * 
               sqrt ( 1.0 + tempaa * ( prsbar (Generic_Index) / prsremi (Generic_Index) - 1.0) );

       // STEPS I b,c,d,e)
       // step b)

          vlxbar (Generic_Index) = ( spdbarmi (Generic_Index) * vlxremi (Generic_Index)
                   + spdbarpl (Generic_Index) * vlxrepl  (Generic_Index)
                 - ( prsrepl  (Generic_Index) - prsremi  (Generic_Index) ) )
                 / ( spdbarmi (Generic_Index) + spdbarpl (Generic_Index) );

       // step c)
          rhobarpl (Generic_Index) = 1.0 / rhorepl (Generic_Index) - 
               ( vlxbar (Generic_Index) - vlxrepl (Generic_Index) ) / spdbarpl (Generic_Index);

       // IF ( rhobarpl (i) .LE. 0.0) rhobarpl (i) = 1.0 / rhofloor
          rhobarpl (Generic_Index).replace ( rhobarpl (Generic_Index) <= 0.0 , 1.0 / rhofloor );

       // step d)
          rhobarmi (Generic_Index) = 1.0 / rhoremi (Generic_Index) + 
               ( vlxbar (Generic_Index) - vlxremi (Generic_Index) ) / spdbarmi (Generic_Index);

       // IF ( rhobarmi ( i) .LE. 0.0) rhobarmi ( i) = 1.0 / rhofloor
          rhobarmi (Generic_Index).replace ( rhobarmi (Generic_Index) <= 0.0 , 1.0 / rhofloor );

          rhobarpl (Generic_Index) = 1.0 / rhobarpl (Generic_Index);
          rhobarmi (Generic_Index) = 1.0 / rhobarmi (Generic_Index);

          rhobarpl (Generic_Index).replace ( rhobarpl (Generic_Index) <= rhofloor , rhofloor );
          rhobarmi (Generic_Index).replace ( rhobarmi (Generic_Index) <= rhofloor , rhofloor );

       // step e)
          sdbarpl (Generic_Index) = sqrt ( gamma * prsbar (Generic_Index) / rhobarpl (Generic_Index));
          sdbarmi (Generic_Index) = sqrt ( gamma * prsbar (Generic_Index) / rhobarmi (Generic_Index));

       // And at this point STEP III is complete
       // END IF
        }

  // End of Riemann iteration of Colella's (1982) two shock scheme

  // ***************************************************************

  // STEP II
  // Find the structure of the waves with respect to the zone boundary.

  // IF ( prsbar (i) .GE. prsremi ( i) ) THEN
  // Shock wave.
     where ( prsbar (Generic_Index) >= prsremi (Generic_Index) );
        {
          wavspd1 (Generic_Index) = vlxremi (Generic_Index) - spdbarmi (Generic_Index) / rhoremi (Generic_Index);
          wavspd2 (Generic_Index) = wavspd1 (Generic_Index);
        }
  // ELSE
     otherwise()
        {
       // Rarefaction
          wavspd1 (Generic_Index) = vlxremi (Generic_Index) - lsdremi (Generic_Index) / rhoremi (Generic_Index);
          wavspd2 (Generic_Index) = vlxbar (Generic_Index) - sdbarmi (Generic_Index);
        }
  // END IF

     wavspd3 (Generic_Index) = vlxbar (Generic_Index);

  // IF ( prsbar (i) .GE. prsrepl (i) ) THEN
  // Shock wave.

     where ( prsbar (Generic_Index) >= prsrepl (Generic_Index) )
        {
          wavspd4 (Generic_Index) = vlxrepl (Generic_Index) + spdbarpl (Generic_Index) / rhorepl (Generic_Index);
          wavspd5 (Generic_Index) = wavspd4 (Generic_Index);
        }
  // ELSE
     otherwise()
        {
       // Rarefaction
          wavspd4 (Generic_Index) = vlxbar  (Generic_Index) + sdbarpl (Generic_Index);
          wavspd5 (Generic_Index) = vlxrepl (Generic_Index) + lsdrepl (Generic_Index) / rhorepl (Generic_Index);
        }
  // END IF


// ***********************************************************************
// ***********************************************************************

  // STEP III
  // Now find the the resolved state that lies along the zone boundary.
  // Note that detecting and keeping record of whether one is in a rarefaction
  // fan is mandatory to getting good results. Also, for all other regions
  // ( i.e. other than 2 and 5) the construction guarantees that the density
  // and pressure are bounded so the bounds aren't explicitly set.

     tempaa.fill ( (gamma - 1.0) / (gamma + 1.0) );

  // This could easily remain a scalar since it is only used in the pow function 
  // where it is easier to treat it as a scalar!
     tempab.fill ( 2.0 / (gamma - 1.0) );

  // IF ( wavspd1 (i) .GE. 0.0) THEN
     where ( wavspd1 (Generic_Index) >= 0.0 )
        {
       // Region 1 to be picked out.

          rho0    (Generic_Index) = rhoremi (Generic_Index);
          prs0    (Generic_Index) = prsremi (Generic_Index);
          sdsp0   (Generic_Index) = lsdremi (Generic_Index) / rhoremi (Generic_Index);
          vlx0    (Generic_Index) = vlxremi (Generic_Index);
          vly0    (Generic_Index) = vlyremi (Generic_Index);
          vlz0    (Generic_Index) = vlzremi (Generic_Index);
          inraref (Generic_Index) = 0;
        }
#ifdef USE_ELSEWHERE_MASK_MACRO
       elsewhere_mask ( wavspd2 (Generic_Index) >= 0.0 )
#else
       elsewhere ( wavspd2 (Generic_Index) >= 0.0 )
#endif
        {
       // ELSE IF ( wavspd2 ( i) .GT. 0.0) THEN
       // We need to accumulate the history of the mask values to
       // support the if - then - else if - else if -else if FORTRAN 
       // construction.
       // intArray Temp_Mask_History = Temp_Mask;
       // printf ("STEP III Test a1! \n");

       // Region 2 to be picked up. Note that : a) Being inside a shock is
       // is automatically excluded b) Being on either edge of a W- fan is also
       // excluded, which is good.

          tempac = tempaa * ( ( vlxremi (Generic_Index) * rhoremi (Generic_Index) / 
                        lsdremi (Generic_Index)) + tempab);
          tempac.replace ( tempac >= 0.0 , tempac );

       // Example of pow function taking an exponent array!
       // rho0    (Generic_Index) = rhoremi (Generic_Index) * tempac**tempab;
       // rho0    (Generic_Index) = rhoremi (Generic_Index) * pow ( tempab , tempac );
       // rho0    (Generic_Index) = rhoremi (Generic_Index) * pow ( 2.0 / ( gamma - 1.0) , tempac );
       // rho0    (Generic_Index).replace ( Temp_Mask , rhoremi (Generic_Index) * pow ( 2.0/(gamma-1.0) , tempac ) );

          rho0    (Generic_Index) = rhoremi (Generic_Index) * pow ( tempac , tempab );
          rho0    (Generic_Index).replace ( (rho0 (Generic_Index) <= rhofloor) , rhofloor );
          sdsp0   (Generic_Index) = ( lsdremi (Generic_Index)/rhoremi (Generic_Index) ) * tempac;
          prs0    (Generic_Index) = rho0 (Generic_Index) * pow ( sdsp0 (Generic_Index),2.0 ) / gamma;
          prs0    (Generic_Index).replace ( (prs0 (Generic_Index) <= prsfloor) , prsfloor );
          sdsp0   (Generic_Index) = prs0 (Generic_Index) / rho0 (Generic_Index);

          vlx0    (Generic_Index) = sdsp0 (Generic_Index);
          vly0    (Generic_Index) = vlyremi (Generic_Index);
          vlz0    (Generic_Index) = vlzremi (Generic_Index);

          inraref (Generic_Index) = 1;
        }
#ifdef USE_ELSEWHERE_MASK_MACRO
       elsewhere_mask ( wavspd3 (Generic_Index) >= 0.0  )
#else
       elsewhere ( wavspd3 (Generic_Index) >= 0.0  )
#endif
        {
       // ELSE IF ( wavspd3 ( i) .GT. 0.0) THEN
       // Region 3 to be picked up.

          rho0    (Generic_Index) = rhobarmi (Generic_Index);
          prs0    (Generic_Index) = prsbar   (Generic_Index);
          sdsp0   (Generic_Index) = sdbarmi  (Generic_Index);
          vlx0    (Generic_Index) = vlxbar   (Generic_Index);
          vly0    (Generic_Index) = vlyremi  (Generic_Index);
          vlz0    (Generic_Index) = vlzremi  (Generic_Index);
          inraref (Generic_Index) = 0;
        }
#ifdef USE_ELSEWHERE_MASK_MACRO
       elsewhere_mask ( wavspd4 (Generic_Index) >= 0.0 )
#else
       elsewhere ( wavspd4 (Generic_Index) >= 0.0 )
#endif
        {
       // ELSE IF ( wavspd4 ( i) .GE. 0.0) THEN
       // Region 4 to be picked up.

          rho0    (Generic_Index) = rhobarpl (Generic_Index);
          prs0    (Generic_Index) = prsbar   (Generic_Index);
          sdsp0   (Generic_Index) = sdbarpl  (Generic_Index);
          vlx0    (Generic_Index) = vlxbar   (Generic_Index);
          vly0    (Generic_Index) = vlyrepl  (Generic_Index);
          vlz0    (Generic_Index) = vlzrepl  (Generic_Index);
          inraref (Generic_Index) = 0;
        }
#ifdef USE_ELSEWHERE_MASK_MACRO
       elsewhere_mask ( wavspd5 (Generic_Index) >= 0.0 )
#else
       elsewhere ( wavspd5 (Generic_Index) >= 0.0 )
#endif
        {
       // ELSE IF ( wavspd5 ( i) .GT. 0.0) THEN
       // Region 5 to be picked up. Notice as before that we will never be inside
       // a shock or be flush on either side of the W+ rarefaction fan.

          tempac = tempaa * ( - ( vlxrepl (Generic_Index) * rhorepl (Generic_Index) / lsdrepl (Generic_Index)) + tempab);

       // What does this do!
          tempac.replace ( tempac >= 0.0 , tempac );
       // The following is better!
       // tempac = max ( 0.0 , tempac );

       // printf ("Test 15! \n");
          rho0    (Generic_Index) = rhorepl (Generic_Index) * pow ( tempac , tempab );
          rho0    (Generic_Index).replace ( (rho0 (Generic_Index) <= rhofloor) , rhofloor );
       // The following is better!
       // rho0    (Generic_Index) = max ( rho0 (Generic_Index) , rhofloor );
          sdsp0   (Generic_Index) = ( lsdrepl (Generic_Index) / rhorepl (Generic_Index) ) * tempac;
          prs0    (Generic_Index) = rho0 (Generic_Index) * pow (sdsp0 (Generic_Index),2.0) / gamma;
          prs0    (Generic_Index).replace ( (prs0 (Generic_Index) <= prsfloor), prsfloor );
       // The following is better!
       // prs0    (Generic_Index) = max ( prs0 (Generic_Index) <= prsfloor , prsfloor );
          sdsp0   (Generic_Index) = sqrt ( gamma * prs0 (Generic_Index) / rho0 (Generic_Index) );
          vlx0    (Generic_Index) = - sdsp0 (Generic_Index);
          vly0    (Generic_Index) = vlyrepl (Generic_Index);
          vlz0    (Generic_Index) = vlzrepl (Generic_Index);
          inraref (Generic_Index) = 2;
        }
       otherwise()
        {
       // ELSE
       // Region 6 to be picked up.

          rho0    (Generic_Index) = rhorepl (Generic_Index);
          prs0    (Generic_Index) = prsrepl (Generic_Index);
          sdsp0   (Generic_Index) = lsdrepl (Generic_Index) / rhorepl (Generic_Index);
          vlx0    (Generic_Index) = vlxrepl (Generic_Index);
          vly0    (Generic_Index) = vlyrepl (Generic_Index);
          vlz0    (Generic_Index) = vlzrepl (Generic_Index);
          inraref (Generic_Index) = 0;
        }

  // END IF

  // *****************************
  // STEP IV
  // Make the fluxes from the boundary states.
      tempab.fill ( gamma / (gamma - 1.0) );
      tempaa = rho0 (Generic_Index) * vlx0 (Generic_Index);
      rhoflux (Generic_Index) = tempaa;
      vlxflux (Generic_Index) = tempaa * vlx0 (Generic_Index) + prs0 (Generic_Index);
      vlyflux (Generic_Index) = tempaa * vly0 (Generic_Index);
      vlzflux (Generic_Index) = tempaa * vlz0 (Generic_Index);
      engflux (Generic_Index) = tempaa * ( tempab * prs0 (Generic_Index) / rho0 (Generic_Index)
                                 + 0.5 * ( pow (vlx0 (Generic_Index),2.0)
                                 + pow (vly0 (Generic_Index),2.0)
                                 + pow (vlz0 (Generic_Index),2.0) ) );


  // printf ("Leaving One_Dimensional_Goduov_Solver! \n");
  // ***************************************************************
  // *****************  END  OF  PROCEDURE  ************************
  // ***************************************************************
   }


int main (int argc, char** argv)
   {
     ios::sync_with_stdio();     // Sync the C and C++ io systems!
     Index::setBoundsCheck (ON); // Turn on Index bounds checking!

     int Number_Of_Processors = 1;
     Optimization_Manager::Initialize_Virtual_Machine (
          "",Number_Of_Processors,argc,argv);

     printf ("Run P++ code (Number_Of_Processors = %d) \n",Number_Of_Processors);

  // extern void abort();
  // if ( ieee_handler ("set","all",abort) != 0 )
  //    {
  //      printf ("Setup ieee_handler! \n");
  //    }

     printf ("Running ...\n");

  // APP_DEBUG = 5;

     int SIZE = 17;

  // Input  variables (passed by reference)!
     int nriemann    = SIZE - 2;
     double prsfac   = 1.0;
     double rhofloor = 1.0e-8;
     double prsfloor = 1.0e-16;
     double gamma    = 1.4;
     int pprieman    = 1;
     int rieiter     = 5;
     doubleArray rhoremi (SIZE);
     doubleArray rhorepl (SIZE);
     doubleArray prsremi (SIZE); 
     doubleArray prsrepl (SIZE);
     doubleArray vlxremi (SIZE); 
     doubleArray vlxrepl (SIZE);
     doubleArray vlyremi (SIZE); 
     doubleArray vlyrepl (SIZE);
     doubleArray vlzremi (SIZE); 
     doubleArray vlzrepl (SIZE);

  // Return variables (passed by reference)!
     doubleArray rhoflux (SIZE); 
     doubleArray engflux (SIZE);
     doubleArray vlxflux (SIZE); 
     doubleArray vlyflux (SIZE); 
     doubleArray vlzflux (SIZE);
     doubleArray rho0    (SIZE); 
     doubleArray prs0    (SIZE); 
     doubleArray vlx0    (SIZE);
     doubleArray vly0    (SIZE); 
     doubleArray vlz0    (SIZE); 
     doubleArray sdsp0   (SIZE);

     sdsp0 = 0;

  // Initialize Input!
     rhoremi.fill (1.0);
     rhorepl.fill (1.0);
     prsremi.fill (1.0); 
     prsrepl.fill (1.0);
     vlxremi.fill (12.0); 
     vlxrepl.fill (-12.0);
     vlyremi.fill (0.0); 
     vlyrepl.fill (0.0);
     vlzremi.fill (0.0);
     vlzrepl.fill (0.0);

     printf ("Calling One_Dimensional_Goduov_Solver! \n");

     long Start_Clock = clock();

     for (int i=0; i < 1; i++)
          One_Dimensional_Goduov_Solver ( nriemann,
                prsfac , rhofloor , prsfloor , gamma , pprieman , rieiter ,
                rhoremi, rhorepl, prsremi, prsrepl, vlxremi, vlxrepl, vlyremi, vlyrepl,
                vlzremi, vlzrepl, rhoflux, engflux, vlxflux, vlyflux, vlzflux,
                rho0, prs0, vlx0, vly0, vlz0, sdsp0 );

     long End_Clock = clock();

     printf ("Time for solve %ld \n", (End_Clock-Start_Clock) / 1000 );

  // sdsp0.display("sdsp0");
     if ( fabs(sdsp0(sdsp0.getLocalBase(0)+1) - 6.4949) > 0.05)
        {
          printf ("ERROR: A++ test code not computing correct result! \n");
          APP_ABORT();
        }

     printf ("PROGRAM TERMINATED NORMALLY! \n");

  // printf ("Optimization_Manager::Number_Of_Assignments_Optimized_Out = %d \n",
  //      Optimization_Manager::Number_Of_Assignments_Optimized_Out);

     Optimization_Manager::Exit_Virtual_Machine ();
     printf ("Virtual Machine exited! \n");
   }


