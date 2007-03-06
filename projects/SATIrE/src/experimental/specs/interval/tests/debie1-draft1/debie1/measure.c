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
 *    Module     :   measure.c
 *
 * Event Measurement module.
 *
 * Based on the SSF file measure.c, rev 1.51, Wed Oct 13 19:48:50 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#include "keyword.h"
#include "kernobj.h"
#include "tm_data.h"
#include "isr_ctrl.h"
#include "msg_ctrl.h"
#include "tc_hand.h"
#include "telem.h"
#include "ttc_ctrl.h"
#include "su_ctrl.h"
#include "health.h"
#include "ad_conv.h"
#include "measure.h"
#include "taskctrl.h"
#include "class.h"

#define HIT_ADC_ERROR          0x80
#define HIT_SELF_TEST_RESET    0x40
#define SU_NUMBER_MASK         0x07

sensor_number_t EXTERNAL self_test_SU_number = NO_SU;
/* By default this variable indicates that no SU self test   */
/* sequence is running.                                      */
/* Number of SU being self tested (SU_1, SU_2, SU_3 or SU_4) */
/* or NO_SU if no SU is being self tested.                   */

unsigned char EXTERNAL test_channel;
/* Channel being tested in SU Self Test. Valid only if triggering SU */
/* (indicated by self_test_SU) is in Self Test state.                */

SU_test_level_t EXTERNAL test_level;
/* Test level being used in SU Self Test. */

SU_state_t EXTERNAL SU_state[NUM_SU] = {off_e, off_e, off_e, off_e};

/*This array contains information about the state of a given Sensor Unit.    */
/*Default value is 'off'.                                                    */

EXTERNAL unsigned short int ADC_result[NUM_CH];
/*Used to temporarily store AD conversion results.                           */

EXTERNAL unsigned char confirm_hit_result;
/*This variable indicates a hit with a high value.                           */

uint_least8_t EXTERNAL hit_budget       = HIT_BUDGET_DEFAULT;
uint_least8_t EXTERNAL hit_budget_left  = HIT_BUDGET_DEFAULT;

#ifdef REG52_DEFINED
   #pragma REGISTERBANK(1)
#endif

void InitHitTriggerTask (void)

/* Purpose        : Initialize the global state of Hit Trigger handling      */
/* Interface      : inputs  - none                                           */
/*                  outputs - none                                           */
/* Preconditions  : none                                                     */
/* Postconditions : Calling task attached as Hit Trigger ISR.                */
/*                  Hit Trigger interrupt enabled                            */
/* Algorithm      : - attach current task as Hit Trigger ISR                 */
/*                  - enable Hit Trigger interrupt                           */

{
   AttachInterrupt(HIT_TRIGGER_ISR_SOURCE);
   /*Now 'HitTriggerTask()' will listen for Hit trigger interrupt.           */

   ENABLE_HIT_TRIGGER;
}

void HandleHitTrigger (void)

/* Purpose        : Wait for and handle one Hit Trigger interrupt            */
/* Interface      : inputs  - Five analog outputs from Peak Detectors        */
/*                  outputs - Acquisition task mailbox                       */
/*                          - Sampled ADC_result                             */
/* Preconditions  : none                                                     */
/* Postconditions : Message holding the number of triggering Sensor Unit is  */
/*                  sent to Aqcuisition task.                                */
/* Algorithm      : - wait for Hit Trigger interrupt                         */
/*                  - Read Peak Detector outputs from hardware registers.    */
/*                  - Sample and store these into a buffer.                  */
/*                  - Send number of triggering Sensor Unit to Aqcuisition   */
/*                    task mailbox.                                          */

{
   unsigned char EXTERNAL initial_delay;
   /* Delay before the first AD channel is selected in */
   /* ShortDelay() units.                              */

   unsigned char EXTERNAL delay_limit;
   /* Delay between channel selection and start of conversion in */
   /* ShortDelay() units.                                        */

   INDIRECT_INTERNAL sensor_number_t trigger;
   /*Used to store Sensor Unit number, which has beem hit.                   */

   INDIRECT_INTERNAL channel_t CH_base;
   /* First ADC channel number for the relevant Sensor Unit.                 */

   DIRECT_INTERNAL uint_least8_t i;
   /* Used in a for -loop, which reads the peak sensor outputs.              */

   DIRECT_INTERNAL unsigned char lsb, msb;
   /*These variables are used to combine two bytes into one word.            */

   DIRECT_INTERNAL uint_least8_t conversion_try_count;
   /*This variable stores the number of failed conversion starts.            */

   initial_delay = (uint_least8_t) (DELAY_LIMIT(100));
   /* Initial delay before converting first channel. */

   delay_limit = (uint_least8_t) (DELAY_LIMIT(100));
   /* The signal settling delay is 100 microseconds. */

   WaitInterrupt (HIT_TRIGGER_ISR_SOURCE, 255);
   /* Interrupt arrival is awaited.    */
   /* Execution result is not handled. */

   CLEAR_HIT_TRIGGER_ISR_FLAG;
   /* Acknowledge the interrupt.            */
   /* This bit must be cleared by software. */

   if (hit_budget_left == 0)
   {
      /* Too many hit triggers during one Health Monitoring period. */

      if (telemetry_data.hit_budget_exceedings < 255)
      {
         telemetry_data.hit_budget_exceedings++;
      }

      DISABLE_HIT_TRIGGER;
      /* No more hit triggers will be handled before next Health */
      /* Monitoring period starts (or DEBIE is reset).           */
   }
   else
   {
      /* Some hit budget left; this hit will be handled. */

      hit_budget_left--;

      confirm_hit_result = 1;
      /*This variable indicates a hit with a high value.                  */

      ADC_channel_register &= BP_DOWN;
      UPDATE_ADC_CHANNEL_REG;   
      /*AD converter is set to unipolar mode                              */

      START_CONVERSION;
      /*Dummy cycle to set unipolar mode.                                 */

      conversion_try_count = 0;

      while (   conversion_try_count < ADC_MAX_TRIES
             && END_OF_ADC != CONVERSION_ACTIVE )
      {
         conversion_try_count++;
         /*Conversion try counter is increased. If this counter exeeds the*/
         /*maximum number of conversion start tries the conversion will be*/
         /*dropped.                                                       */
      }       

      if (self_test_SU_number != NO_SU)
      {
         /* Some Sensor Unit is being Self Tested. */
         trigger = self_test_SU_number;

         if (SU_state[self_test_SU_number - SU1] == self_test_e)
         {
            /* Some Sensor Unit is being Self Tested but this is */
            /* not the right self test pulse.                    */

            trigger |= HIT_SELF_TEST_RESET	;
            /* Self test pulse is incorrect and an indication     */
            /* of this is stored in to 'trigger' variable.        */
            /* The AcquisitionTask will adjust its operation      */
            /* based on this indication result.                   */
         }

         else if (SU_state[self_test_SU_number - SU1] == self_test_trigger_e)
         {
            /* Some Sensor Unit is being Self Tested  and this is the correct. */
            /* self test pulse.                                                */

            SU_state[self_test_SU_number - SU1] = self_test_e;
            /* Indication of a succesfully received self test pulse */
         }
      }

      else
      {
         /* There is no Sensor Unit Self Test in progress. */

         trigger = ((int)TRIGGER_SOURCE_0 
                      + 2 
                      * (int)TRIGGER_SOURCE_1) 
                      + SU1;
         /* Sensor Unit which caused the hit trigger is resolved. */
      }   

      CH_base = 
         ((int)(trigger - SU_1)&2) * 12 + ((int)(trigger - SU_1)&1) * 8; 
      /* First channel address for the given SU is calculated. */

      ShortDelay(initial_delay);
      /* Delay before converting first channel. */

      ADC_channel_register =
         (ADC_channel_register & 0xC0) | CH_base;
      UPDATE_ADC_CHANNEL_REG;
      /* First channel is selected. */

      ShortDelay(delay_limit);
      /* Delay of 100 microseconds (+ function call overhead). */


      for (i = 0; i < NUM_CH; i++)
      {

         ShortDelay(delay_limit);
         /* Delay of 100 microseconds (+ function call overhead). */

         START_CONVERSION;
         /* AD conversion for the selected channel is started. */

         ADC_channel_register =
            (ADC_channel_register & 0xC0) | (CH_base + i + 1);
         UPDATE_ADC_CHANNEL_REG;
         /* Next channel is selected. */

         conversion_try_count = 0;

         while (   conversion_try_count < ADC_MAX_TRIES
                && END_OF_ADC != CONVERSION_ACTIVE )
         {
            conversion_try_count++;
            /*Conversion try counter is increased. If this counter exeeds */
            /*the maximum number of conversion start tries the conversion */
            /*will be dropped.                                            */
         }       

         if (conversion_try_count < ADC_MAX_TRIES)
         {
            msb = GET_RESULT;
            /*Most significant byte is read from ADC result address.      */

            lsb = GET_RESULT;
            /*Least significant byte is read from ADC result address.     */

            ADC_result[i] = 
               ((unsigned int)msb << 8) | (unsigned int)lsb;
            /*Msb and lsb are combined into one word.                     */
         } 

         else
         {
            trigger |= HIT_ADC_ERROR;
            /*Conversion has failed and an indication of this is stored in*/
            /*to 'trigger' variable by setting the Most Significant Bit   */
            /*(MSB) high. The AcquisitionTask will adjust its operation   */
            /*based on this indication result.                            */

            ADC_result[i] = 0;
         }     

      }

      SendTaskMail(ACQUISITION_MAILBOX,trigger, 0);
      /*The number of the Sensor unit that has caused the hit trigger     */
      /*interrupt is sent to a mailbox for the acquisition task.          */

   }  /* end if (hit budget left) */

}

void HitTriggerTask(void) TASK(HIT_TRIGGER_ISR_TASK) PRIORITY(HIT_TRIGGER_PR)
                           
/* Purpose        : Handles the Hit Trigger interrupts                       */
/* Interface      : inputs  - Five analog outputs from Peak Detectors        */
/*                  outputs - Acquisition task mailbox                       */
/*                          - Sampled ADC_result                             */
/* Preconditions  : Aqcuisition enabled                                      */
/* Postconditions : Message holding the number of triggering Sensor Unit is  */
/*                  sent to Aqcuisition task.                                */
/* Algorithm      : - InitHitTriggerTask                                     */
/*                  - loop forever:                                          */
/*                  -    HandleHitTrigger                                    */

{
   InitHitTriggerTask ();

   while(1)
   {
      HandleHitTrigger ();
   }
}                          


#ifdef REG52_DEFINED
   #pragma REGISTERBANK(0)
#endif


static EXTERNAL incoming_mail_t ACQ_mail;
/* Holds parameters for the mail waiting function.                        */
/* Must be in xdata memory because parameter of subroutine is pointer     */
/* to xdata area.                                                         */

static EXTERNAL uint16_t trigger_unit;
/* Number of the triggering Sensor Unit.                                  */


void InitAcquisitionTask (void)
/* Purpose        : Initialize the global state of the Acquisition task.     */
/* Interface      : inputs      - none                                       */
/*                  outputs     - ACQ_mail static fields.                    */
/* Preconditions  : none                                                     */
/* Postconditions : AcqusitionTask is operational.                           */
/* Algorithm      : - initialize task variables                              */
{
   /* ACQ_mail struct fields are set.                                        */
   ACQ_mail.mailbox_number = ACQUISITION_MAILBOX;
   ACQ_mail.message        = &trigger_unit;
   ACQ_mail.timeout        = 0;
}


void HandleAcquisition (void)

/* Purpose        : Acquires the data for one hit event.                     */
/* Interface      : inputs      - Acquisition task mailbox                   */
/*                              - Mail from Hit Trigger interrupt service    */
/*                              - Buffer with sampled Peak detector outputs  */
/*                              - Housekeeping Telemetry registers           */
/*                  outputs     - Science data                               */
/* Preconditions  : none                                                     */
/* Postconditions : one message processed from Acquisition task mailbox      */
/* Algorithm      : - wait for mail to Acquisition task mailbox              */
/*                  - if mail is "SU_NUMBER"                                 */
/*                    - get Peak Detector Outputs sampled by the interrupt   */
/*                      service                                              */
/*                    - measure Pulse Rise Time                              */
/*                    - measure delays between trigger signals               */
/*                    - get measurement time                                 */
/*                    - get sensor unit temperatures from Housekeeping       */
/*                      Telemetry registers                                  */
/*                    - calculate time difference between Plasma1- and       */
/*                      Plasma1+ trigger signals                             */
/*                    - calculate quality number                             */
/*                    - call RecordEvent()                                   */

{
   EXTERNAL unsigned char * EXTERNAL checksum_pointer;
   unsigned char EXTERNAL event_checksum;
   uint_least8_t EXTERNAL i;
   /* These variables are used when checksum is computed for a given event   */
   /* before storing it to Science data area.                                */
 
   event_record_t EXTERNAL *event;
   /* Pointer to the new event record.                                       */

   EXTERNAL delays_t delay_counters;
   /*This is a struct which stores the Delay Counter time data.              */

   DIRECT_INTERNAL signed int time_delay;
   /*This variable is used to store the delay from plasma 1+ to plasma 1-.   */
  
   SU_state_t EXTERNAL state = off_e;   
   /* Used to store sensor unit state. */                                     

   WaitMail(&ACQ_mail);

   if (trigger_unit & HIT_ADC_ERROR)
   {
      /* There has been an  error in AD conversion */
      /* in Hit trigger processing.                */
      SetModeStatusError(ADC_ERROR);
   }

   if(trigger_unit == SU_1 || trigger_unit == SU_2 || 
      trigger_unit == SU_3 || trigger_unit == SU_4)

   {
      state = SU_state[trigger_unit - SU_1];

      if ((state == self_test_e || state == acquisition_e) 
           && (EVENT_FLAG == ACCEPT_EVENT))
      {
         event = GetFreeRecord();
         /* Get pointer to the new event record. */


         /*Number of the Sensor Unit, which has been hit, is stored into  */
         /*Event Record.                                                  */
         event -> SU_number = (unsigned char)(trigger_unit & 0xFF);

         /*Contents of a temporary buffer is stored into Event Record.    */
         COPY (event -> plasma_1_plus ,ADC_result[0]);
         COPY (event -> plasma_1_minus,ADC_result[1]);
         COPY (event -> piezo_1       ,ADC_result[2]);    
         COPY (event -> piezo_2       ,ADC_result[3]);
         COPY (event -> plasma_2_plus ,ADC_result[4]);

         /*Rise time counter is read in to Event Record.                  */
         event -> rise_time = ReadRiseTimeCounter();

         /*Delay counters are read in to a struct.                        */
         ReadDelayCounters(&delay_counters);  

         /*Delay from plasma 1+ to PZT 1/2 is stored into Event Record.   */
         COPY (event -> delay_2,delay_counters.FromPlasma1Plus);                

         /*Delay from plasma 1- to PZT 1/2 is stored into Event Record.   */
         COPY (event -> delay_3,delay_counters.FromPlasma1Minus);           

         /*Delay from plasma 1+ to plasma 1- is calculated and stored into*/
         /*Event Record.                                                  */

         time_delay = delay_counters.FromPlasma1Plus  
                   - delay_counters.FromPlasma1Minus; 
         if(time_delay > 127)
         {
            event -> delay_1 = 127;
            /*If the delay from plasma 1+ to plasma 1- is positive and    */
            /*doesn't fit into signed char 'event_record.delay_1', then   */
            /*the largest value for the signed char is stored instead.    */
         }

         else if(time_delay < -128)
         {
            event -> delay_1 = -128;
            /*If the delay from plasma 1+ to plasma 1- is negative and    */
            /*doesn't fit into signed char 'event_record.delay_1', then   */
            /*the smallest value for the signed char is stored instead.   */
         }

         else
         {
            event -> delay_1 = time_delay;
            /*Delay from plasma 1+ to plasma 1- is calculated and stored  */
            /*into Event Record.                                          */
         }

         /*Measurement time is stored into Event Record.                  */
         COPY (event -> hit_time, internal_time);

         /*Unit temperatures are stored into Event Record.                */

         event -> SU_temperature_1 = 
            telemetry_data.SU_temperature[trigger_unit - SU1][0];

         event -> SU_temperature_2 = 
            telemetry_data.SU_temperature[trigger_unit - SU1][1];

         ClassifyEvent(event);
         /* New event is classified. */

         checksum_pointer = (EXTERNAL unsigned char *)event;
         event_checksum = 0;

         for (i = 1; i < sizeof(event_record_t); i++)
         {
            event_checksum ^= *checksum_pointer;
            checksum_pointer++;
         }

         event -> checksum = event_checksum;

         /* After the event record is filled up, it is stored into science*/
         /* data.                                                         */
         RecordEvent();
      }
   }

   else
   {
      /*The received mail contained an invalid Sensor unit number.        */
   }

   trigger_unit &= SU_NUMBER_MASK;
   /* Delete possible error bits. */

   WaitTimeout(PEAK_RESET_MIN_DELAY);

   ResetPeakDetector(trigger_unit);
   /*Peak detector for this Sensor Unit is resetted. */       

   WaitTimeout(PEAK_RESET_MIN_DELAY);

   ResetPeakDetector(trigger_unit);
   /*Peak detector for this Sensor Unit is resetted again. */       

   WaitTimeout(COUNTER_RESET_MIN_DELAY);

   ResetDelayCounters();
   /*The Delay Counters are resetted. */
}       


void AcquisitionTask(void) TASK(ACQUISITION_TASK) PRIORITY(ACQUISITION_PR)
/* Purpose        : Implements the Acquisition task.                         */
/* Interface      : inputs      - Acquisition task mailbox                   */
/*                              - Mail from Hit Trigger interrupt service    */
/*                              - Buffer with sampled Peak detector outputs  */
/*                              - Housekeeping Telemetry registers           */
/*                  outputs     - Science data                               */
/* Preconditions  : none                                                     */
/* Postconditions : This function does not return.                           */
/* Algorithm      : - InitAcquisitionTask                                    */
/*                  - loop forever:                                          */
/*                    -   HandleAcquisition                                  */
{
   InitAcquisitionTask ();

   while(1)
   {
      HandleAcquisition ();
   }
}       


/*Assign pointers to tasks*/
void (* EXTERNAL hit_task)(void) = HitTriggerTask;
void (* EXTERNAL acq_task)(void) = AcquisitionTask;
   
void Switch_SU_State(sensor_unit_t EXTERNAL *SU_setting) 
        COMPACT_DATA REENTRANT_FUNC
/* Purpose        : Used when only the SU_state variable must be modified.   */
/* Interface      : inputs      - SU_state                                   */
/*                              - An Address of 'sensor_unit_t' type of a    */
/*                                struct.                                    */
/*                  outputs     - SU_state                                   */
/*                              - SU_setting.execution_result                */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : SU_state variable is conditionally modified.             */
/* Algorithm      :                                                          */
/*                  - If the expected SU_state variable value related to the */
/*                    given SU_index number is not valid, variable value is  */
/*                    not changed. Error indication is recorded instead.     */
/*                  - Else state variable value is changed and an indication */
/*                    of this is recorded.                                   */
{
   if (SU_state[(SU_setting -> SU_number) - SU_1] != 
                 SU_setting -> expected_source_state)
   {
      /* The original SU state is wrong. */

      SU_setting -> execution_result = SU_STATE_TRANSITION_FAILED;
   }

   else if (SU_setting -> SU_state == self_test_mon_e &&
            self_test_SU_number    != NO_SU)
   {
      /* There is a self test sequence running already */

      SU_setting -> execution_result = SU_STATE_TRANSITION_FAILED;
   }

   
   else
   {
      /* The original SU state is correct. */

      if (SU_setting -> SU_state == self_test_mon_e)
      {
         self_test_SU_number = SU_setting -> SU_number;
         /* Number of the SU under self test is recorded. */
      }

      else if (SU_setting -> SU_number == self_test_SU_number)
      {
         self_test_SU_number = NO_SU;
         /* Reset self test state i.e. no self test is running. */
      }

      SU_state[(SU_setting -> SU_number) - SU_1] = SU_setting -> SU_state;
      SU_setting->execution_result  = SU_STATE_TRANSITION_OK;
   }
}

void Start_SU_SwitchingOn(
      sensor_index_t SU,
      unsigned char EXTERNAL *exec_result) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : Transition to SU state on.                               */
/* Interface      : inputs      - Sensor_index number                        */
/*                              - An Address of 'exec_result' variable       */
/*                              - SU_state                                   */
/*                  outputs     - SU_state                                   */
/*                              - 'exec_result'                              */
/*                  subroutines - Switch_SU_On                               */
/* Preconditions  : none                                                     */
/* Postconditions : Under valid conditions transition to 'on' state is       */
/*                  completed.                                               */
/* Algorithm      :                                                          */
/*                  - If the original SU_state variable value related to the */
/*                    given SU_index number is not valid, variable value is  */
/*                    not changed. Error indication is recorded instead.     */
/*                  - Else                                                   */
/*                    - Disable interrups                                    */
/*                    - 'Switch_SU_On' function is called and an             */
/*                      indication of this transition is recorded.           */
/*                    - Enable interrupts                                    */
{
   *exec_result  = SU_STATE_TRANSITION_OK;
   /* Default value, may be changed below. */ 

   if (SU_state[SU] != off_e)
   {
      /* The original SU state is wrong. */

      *exec_result = SU_STATE_TRANSITION_FAILED;
   }

   else
   {
      /* The original SU state is correct. */

      DISABLE_INTERRUPT_MASTER;

      /* SU state is still off_e, because there is only one task  */
      /* which can switch SU state from off_e to any other state. */

      Switch_SU_On(
         SU + SU_1, 
         exec_result);

      if (*exec_result == SU + SU_1)
      {
         /* Transition succeeds. */

         SU_state[SU] = start_switching_e;
      }

      else
      {
         /* Transition fails. */

         *exec_result = SU_STATE_TRANSITION_FAILED;
      }

      ENABLE_INTERRUPT_MASTER;
   }   
}


void SetSensorUnitOff(
         sensor_index_t SU,
         unsigned char EXTERNAL *exec_result) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : Transition to SU state off.                              */
/* Interface      : inputs      - Sensor_index number                        */
/*                              - An Address of 'exec_result' variable       */
/*                              - SU_state                                   */
/*                  outputs     - SU_state                                   */
/*                              - 'exec_result'                              */
/*                  subroutines - Switch_SU_Off                              */
/* Preconditions  : none                                                     */
/* Postconditions : Under valid conditions transition to 'off' state is      */
/*                  completed.                                               */
/* Algorithm      :                                                          */
/*                  - Disable interrups                                      */
/*                  - 'Switch_SU_Off' function is called.                    */
/*                  - If transition succeeds,                                */
/*                    - 'Off' state is recorded to 'SU_state' variable.      */
/*                    - Indication of transition is recorded to              */
/*                      'exec_result'.                                       */
/*                  - Else if transition fails,                              */
/*                    - Indication of this is recorded to 'exec_result'.     */
/*                  - Enable interrupts                                      */
{
   sensor_unit_t EXTERNAL SU_setting;
   /* Holds parameters for "Switch_SU_State" operation                  */
   /* Must be in external memory, because the parameter to the function */
   /* is pointer to external memory                                     */

   DISABLE_INTERRUPT_MASTER;

   Switch_SU_Off(
      SU + SU_1, 
      exec_result);

   if (*exec_result == SU + SU_1)
   {
      /* Transition succeeds. */
  
      SU_setting.SU_number = SU + SU_1;
      SU_setting.expected_source_state = SU_state[SU]; 
      SU_setting.SU_state = off_e;
      Switch_SU_State (&SU_setting);
      *exec_result = SU_STATE_TRANSITION_OK;
   }

   else
   {
      /* Transition fails. */

      *exec_result = SU_STATE_TRANSITION_FAILED;
   }

   ENABLE_INTERRUPT_MASTER;
}

SU_state_t  ReadSensorUnit(unsigned char SU_number) COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  To find out whether given Sensor Unit is switched on or */
/*                   off.                                                    */
/* Interface      :                                                          */
/* Preconditions  :  SU_Number should be 1,2,3 or 4.                         */
/* Postconditions :  Value of state variable is returned.                    */
/* Algorithm      :  Value of state variable (on_e or off_e) is returned.    */
{
   return SU_state[SU_number - 1];      
}       


void Update_SU_State(sensor_index_t SU_index) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : Sensor unit state is updated.                            */
/* Interface      : inputs      - SU_state                                   */
/*                              - SU_index number                            */
/*                  outputs     - SU_state                                   */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : SU_state variable is modified.                           */
/* Algorithm      : - Disable interrups                                      */
/*                  - Change SU_state variable value related to the given    */
/*                    SU_index number. Selection of the new state depends on */
/*                    the present one.                                       */
/*                  - Enable interrups                                       */
{
   DISABLE_INTERRUPT_MASTER;

   if (SU_state[SU_index] == start_switching_e)
   {
      SU_state[SU_index] = switching_e;
   }

   else if (SU_state[SU_index] == switching_e)
   {
      ResetPeakDetector(SU_index + SU_1);
      /*Peak detector for this Sensor Unit is resetted. */       
 
      WaitTimeout(PEAK_RESET_MIN_DELAY);
 
      ResetPeakDetector(SU_index + SU_1);
      /*Peak detector for this Sensor Unit is resetted again. */   

      SU_state[SU_index] = on_e;
   }

   ENABLE_INTERRUPT_MASTER;

}




              
