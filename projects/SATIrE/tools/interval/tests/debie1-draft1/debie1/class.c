/*------------------------------------------------------------------------------
 *
 *    Copyright (C) 1998 : Space Systems Finland Ltd.
 *
 * Space Systems Finland Ltd (SSF) allows you to use this version of
 * the DEBIE-I DPU software for the specific purpose and under the
 * specific conditions set forth in the Terms Of Use document enclosed
 * with or attached to this software. In particular, the software
 * remains the property of SSF and you must not distribute the software
 * to third parties without written and signed authorization from SSF.
 *
 *    System Name:   DEBIE DPU SW
 *    Subsystem  :   DAS
 *    Module     :   class.c
 *
 * Event-classification module.
 *
 * Based on the SSF file class.c, rev 1.10, Tue Jun 01 12:34:56 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#include "keyword.h"
#include "class.h"
#include "classtab.h"
#include "su_ctrl.h"
#include "measure.h"

#define MAX_QUALITY 255

unsigned char RoughLogarithm (unsigned int x)
/* An integer approximation (0..16) of the base-2 log of x */
/* computed as the number of the most-significant non-zero bit. */
/* When x = 0, returns zero. */
/* When x > 0, returns floor(log2(x) + 1). */
/* For example, when x =     1, returns  1. */
/*              when x =     2, returns  2. */
/*              when x =     7, returns  3. */
/*              when x = x7FFF, returns 15. */
/*              when x = x8000, returns 16. */
{
   unsigned char INDIRECT_INTERNAL greatest_non_zero_bit;
   unsigned int  INDIRECT_INTERNAL shifted;

   greatest_non_zero_bit = 0;
   shifted = x;

   while (shifted)
   {
      greatest_non_zero_bit++;
      shifted >>= 1;
   }

   return greatest_non_zero_bit;
}


float GetQualityTerm(unsigned char coeff, unsigned int amplitude)
/* Purpose        : Calculates the ampltude term of the quality formula.     */
/* Interface      : inputs      - parameter coeff defines number of the      */
/*                                quality coefficient.                       */
/*                              - parameter amplitude defines the signal     */
/*                                amplitude.                                 */
/*                  outputs     - quality term value is retruned.            */
/*                  subroutines - RoughLogarithm.                            */
/* Preconditions  : quality coefficients have valid values.                  */
/* Postconditions : none.                                                    */
/* Algorithm      : quality term is calculated according to following        */
/*                  formula :                                                */
/*                     coefficient[coeff]                                    */
/*                     * RoughLogaritm(amplitude)                            */
/*                     / AMPLTUDE_DIVIDER                                    */
/*                  where coefficient[coeff] is the amplitude coefficient of */
/*                  the quality term and AMPLTUDE_DIVIDER is a scaling       */
/*                  factor whose purpose is scale the result below 5.0.      */
/*                  However if the result would become larger than that,     */
/*                  5.0 is returned.                                         */

{
   float EXTERNAL quality;

   quality = 
      (float)(telemetry_data.coefficient[coeff]
              * RoughLogarithm(amplitude))
      / AMPLITUDE_DIVIDER;

   if (quality > 5.0)
   {
      quality = 5.0;
   }

   return quality;
}


void CalculateQualityNumber(event_record_t EXTERNAL *new_event)
/* Purpose        : Calculates the quality number of a particle hit event    */
/*                  and stores in the event record.                          */
/* Interface      : inputs      - event record pointed by the parameter.     */
/*                  outputs     - event record pointed by the parameter.     */
/*                  subroutines - GetQualityTerm.                            */
/* Preconditions  : All components of the event record pointed by the        */
/*                  parameter which are used as input have valid values.     */
/* Postconditions : quality_number component of the event record pointed     */
/*                  by the parameter has is calculated.                      */
/* Algorithm      : quality_number is calculated according to the following  */
/*                  formula :                                                */
/*                     25 * Class + Ai*RoughLogarithm(Si) / Divider , where  */
/*                  Class   is the class of the event,                       */
/*                  Ai      is the amplitude coefficient of the quality      */
/*                             formula                                       */
/*                  Si      is a signal amplitude from the Sensor Unit Peak  */
/*                             detector                                      */
/*                  Divider is scaling factor whose value is determined by   */
/*                             the maximum value (5) of the latter terms     */
/*                  and i goes from 1 to 5.                                  */

{
   float INDIRECT_INTERNAL quality;

   quality = 25.0 * new_event -> classification;
   /* First term of the quality formula. */

   quality += GetQualityTerm(0, VALUE_OF (new_event -> plasma_1_plus));
   /* Add amplitude term for i=1 (see function algorithm). */

   quality += GetQualityTerm(1, VALUE_OF (new_event -> plasma_1_minus));
   /* Add amplitude term for i=2 (see function algorithm). */

   quality += GetQualityTerm(2, VALUE_OF (new_event -> piezo_1));
   /* Add amplitude term for i=3 (see function algorithm). */

   quality += GetQualityTerm(3, VALUE_OF (new_event -> piezo_2));
   /* Add amplitude term for i=4 (see function algorithm). */

   quality += GetQualityTerm(4, VALUE_OF (new_event -> plasma_2_plus));
   /* Add amplitude term for i=5 (see function algorithm). */

   new_event -> quality_number = (unsigned char) (quality + 0.5);
   /* Store quality number to the event record */
}


void ClassifyEvent(event_record_t EXTERNAL *new_event)
/* Purpose        : Classifies a particle hit event and stores result        */
/*                  to the event record pointed by the parameter.            */
/* Interface      : inputs      - event record pointed by the parameter.     */
/*                  outputs     - event record pointed by the parameter.     */
/*                  subroutines - CalculateQualityNumber.                    */
/* Preconditions  : All components of the event record pointed by the        */
/*                  parameter which are used as input have valid values.     */
/* Postconditions : classification and quality_number components of the      */
/*                  event record pointed by the parameter are computed.      */
/* Algorithm      : - class index is determined by comparing signal.         */
/*                    amplitudes and time delays to classification           */
/*                    thresholds.                                            */
/*                  - class number is read from a look-up table using the    */
/*                    class index and stored in the event record.            */
/*                  - CalculateQualityNumber is called.                      */

{
   unsigned char INDIRECT_INTERNAL class_index;
   /* Index for the class look-up table. */

   SU_settings_t EXTERNAL * INDIRECT_INTERNAL limits;
   /* Pointer to the struct holding classification thresholds. */

   class_index = 0;
   /* Bits will be set below according to event attributes. */

   switch (new_event -> SU_number)
   /* Select proper classification thresholds. */
   {
      case SU_1:
         limits = &telemetry_data.sensor_unit_1;
         break;

      case SU_2:
         limits = &telemetry_data.sensor_unit_2;
         break;

      case SU_3:
         limits = &telemetry_data.sensor_unit_3;
         break;

      case SU_4:
         limits = &telemetry_data.sensor_unit_4;
         break;
    }

    if (VALUE_OF (new_event -> plasma_1_plus) >=
        ((limits -> plasma_1_plus_classification) * 256))
    {
       class_index |= PLASMA_1_PLUS_CLASS;
       /* Set classification index bit for Plasma1+ peak amplitude. */ 
    }

    if (VALUE_OF (new_event -> plasma_1_minus) >=
        ((limits -> plasma_1_minus_classification) * 256))
    {
       class_index |= PLASMA_1_MINUS_CLASS;
       /* Set classification index bit for Plasma1- peak amplitude. */ 
    }

    if (VALUE_OF (new_event -> piezo_1) >=
        ((limits -> piezo_1_classification) * 256))
    {
       class_index |= PIEZO_1_CLASS;
       /* Set classification index bit for Piezo1 peak amplitude. */ 
    }

    if (VALUE_OF (new_event -> piezo_2) >=
        ((limits -> piezo_2_classification) * 256))
    {
       class_index |= PIEZO_2_CLASS;
       /* Set classification index bit for Piezo2 peak amplitude. */ 
    }

    if (VALUE_OF (new_event -> plasma_2_plus) >=
        ((limits -> plasma_2_plus_classification) * 256))
    {
       class_index |= PLASMA_2_PLUS_CLASS;
       /* Set classification index bit for Plasma2+ peak amplitude. */ 
    }

    if (VALUE_OF (new_event -> delay_2) >=
        ((limits -> plasma_1_plus_to_piezo_min_time) * 16) &&
        VALUE_OF (new_event -> delay_2) <=
        ((limits -> plasma_1_plus_to_piezo_max_time) * 16))
    {
       class_index |= PLASMA_1_PLUS_TO_PIEZO_CLASS;
       /* Set classification index bit for Plasma1+ to Piezo delay. */ 
    }

    if (VALUE_OF (new_event -> delay_3) >=
        ((limits -> plasma_1_minus_to_piezo_min_time) * 16) &&
        VALUE_OF (new_event -> delay_3) <=
        ((limits -> plasma_1_minus_to_piezo_max_time) * 16))
    {
       class_index |= PLASMA_1_MINUS_TO_PIEZO_CLASS;
       /* Set classification index bit for Plasma1- to Piezo delay. */ 
    }                                  

    if (new_event -> delay_1 <=
        limits -> plasma_1_plus_to_minus_max_time)
    {
       class_index |= PLASMA_1_PLUS_TO_MINUS_CLASS;
       /* Set classification index bit for Plasma1+ to Plasma1- delay. */ 
    }

    new_event -> classification = event_class[class_index];
    /* Store classification number to the event record */

    if (SU_state[new_event->SU_number - SU_1] == self_test_e)
    {
       new_event -> quality_number = MAX_QUALITY;
    }

    else
    {
       CalculateQualityNumber(new_event);
    }

}

void InitClassification(void)
/* Purpose        : Initializes classication coefficients and levels.        */
/* Interface      : inputs      - none.                                      */
/*                  outputs     - quality coefficients in telemetry_data.    */
/*                              - classification levels in telemetry_data.   */
/*                              - threshold levels in telemetry_data.        */
/*                              - min time window in telemetry_data          */
/*                              - max time window in telemetry_data          */ 
/*                  subroutines - Init_SU_Settings                           */
/* Preconditions  : none.                                                    */
/* Postconditions : outputs have their default values.                       */
/* Algorithm      : see below                                                */

{
   uint_least8_t EXTERNAL i;
   /* Loop variable. */

   for (i=0; i<NUM_QCOEFF; i++)
   {
      telemetry_data.coefficient[i] = DEFAULT_COEFF;
   }

   Init_SU_Settings (&telemetry_data.sensor_unit_1);
   Init_SU_Settings (&telemetry_data.sensor_unit_2);
   Init_SU_Settings (&telemetry_data.sensor_unit_3);
   Init_SU_Settings (&telemetry_data.sensor_unit_4);
   /* Default values for thresholds, classification levels and min/max times */
   /* related to classification are set here.                                */

}

void Init_SU_Settings (SU_settings_t EXTERNAL *set)
/* Purpose        : Initializes classication parameters                      */
/* Interface      : inputs      - none.                                      */
/*                  outputs     - classification levels in telemetry_data.   */
/*                              - threshold levels in telemetry_data.        */
/*                              - min time window in telemetry_data          */
/*                              - max time window in telemetry_data          */ 
/*                  subroutines - none                                       */
/* Preconditions  : none.                                                    */
/* Postconditions : outputs have their default values.                       */
/* Algorithm      : Sets default values to telemetry_data.                   */
{ 
   set -> plasma_1_plus_threshold       = DEFAULT_THRESHOLD;
   set -> plasma_1_minus_threshold      = DEFAULT_THRESHOLD;
   set -> piezo_threshold               = DEFAULT_THRESHOLD;
   set -> plasma_1_plus_classification  = DEFAULT_CLASSIFICATION_LEVEL;
   set -> plasma_1_minus_classification = DEFAULT_CLASSIFICATION_LEVEL;
   set -> piezo_1_classification        = DEFAULT_CLASSIFICATION_LEVEL;
   set -> piezo_2_classification        = DEFAULT_CLASSIFICATION_LEVEL;
   set -> plasma_2_plus_classification  = DEFAULT_CLASSIFICATION_LEVEL;
   set -> plasma_1_plus_to_minus_max_time  = DEFAULT_MAX_TIME;
   set -> plasma_1_plus_to_piezo_min_time  = DEFAULT_MIN_TIME;
   set -> plasma_1_plus_to_piezo_max_time  = DEFAULT_MAX_TIME;
   set -> plasma_1_minus_to_piezo_min_time = DEFAULT_MIN_TIME;
   set -> plasma_1_minus_to_piezo_max_time = DEFAULT_MAX_TIME;
}
   

