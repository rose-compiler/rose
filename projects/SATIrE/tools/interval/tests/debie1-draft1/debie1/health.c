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
 *    Module     :   health.c
 *
 * Monitoring DEBIE DPU and system health.
 *
 * Based on the SSF file health.c, rev 1.74, Fri Oct 15 20:18:02 1999.
 *
 *- * -----------------------------------------------------------------------
*/

#include "ad_conv.h"
#include "dpu_ctrl.h"     
#include "taskctrl.h"
#include "health.h"
#include "ttc_ctrl.h"
#include "measure.h"
#include "tc_hand.h"
#include "keyword.h"
#include "kernobj.h"
#include "tm_data.h"
#include "telem.h"
#include "isr_ctrl.h"
#include "version.h"
#include "class.h"


#define BOOT_WAIT_INTERVAL 55
/* Boot wait interval: 55 x 10 ms = 550 ms */

#define SYSTEM_INTERVAL 9198
/* Sets system time interval to 10 ms, assuming a clock  */
/* frequency of exactly 11.0592 MHz.                     */           
/* SYSTEM_INTERVAL is in units of the processor cycle,   */
/* and is computed as follows, in principle:             */
/*     10 ms * 11.0592 MHz / 12                          */
/* where 12 is the number of clock cycles per processor  */
/* cycle.                                                */
/* The above calculation gives the value 9216 precisely. */
/* In practice this value makes the DPU time (updated by */
/* the Health Monitoring Task) retard by about 0.2%.     */
/* This is probably due to imprecise handling of timer   */
/* ticks by RTX-51. As a primitive correction, we reduce */
/* the value by 0.2% to 9198. This correction will not   */
/* be exact, especially under heavy interrupt load.      */

#define HM_INTERVAL 100
/* Health Monitoring interval wait: 100 x 10 ms = 1s     */
/* This wait time should be waited 10 times to get       */
/* period of 10s.                                        */
/* Alternatively the execution of the Health Monitoring  */
/* Task can be divided to ten steps which are executed   */
/* 1 second apart from each other and then all actions   */
/* of the task would be executed once in 10 seconds      */

#define ADC_TEMPERATURE_MAX_TRIES         255
/* When temperatures are measured this macro defines the maximum       */
/* amount of tries to be used in reading and handling an AD channel in */
/* 'Read_AD_Channel()'.                                                */ 

#define CONVERSION_TEMPERATURE_MAX_TRIES  255
/* When temperatures are measured this macro defines the maximum       */
/* amount of tries to be used when End Of Conversion indication is     */
/* waited in function 'Convert_AD()'.                                  */ 

#define ADC_VOLTAGE_MAX_TRIES             255
/* When voltages are measured this macro defines the maximum           */
/* amount of tries to be used in reading and handling an AD channel in */
/* 'Read_AD_Channel()'.                                                */ 

#define CONVERSION_VOLTAGE_MAX_TRIES      255
/* When voltages are measured this macro defines the maximum           */
/* amount of tries to be used when End Of Conversion indication is     */
/* waited in function 'Convert_AD()'.                                  */ 

#define DPU_5V_SELECTOR     3
#define SU_1_2_P5V_SELECTOR 0
#define SU_1_2_M5V_SELECTOR 4
/* DPU Self Test voltage measurement channel selectors. */
/* See MeasureVoltage function.                         */

#define SU_P5V_ANA_LOWER_LIMIT    0xBA
#define SU_P5V_ANA_UPPER_LIMIT    0xE4
#define SU_M5V_ANA_LOWER_LIMIT    0x0D
#define SU_M5V_ANA_UPPER_LIMIT    0x22
#define SU_P50V_LOWER_LIMIT       0xA8
#define SU_P50V_UPPER_LIMIT       0xE3
#define SU_M50V_LOWER_LIMIT       0x0E
#define SU_M50V_UPPER_LIMIT       0x2C
#define DPU_P5V_DIG_LOWER_LIMIT   0xBA
#define DPU_P5V_DIG_UPPER_LIMIT   0xE4
/* Upper and lower limits for monitoring */
/* SU and DPU supply voltages.           */

#define SELF_TEST_DONE     0
#define SELF_TEST_RUNNING  1
/* Used in SU self test. */

typedef struct {
   channel_t       ADC_channel;
   uint_least8_t   ADC_max_tries;
   uint_least8_t   conversion_max_tries;
   unsigned int    unsigned_ADC;
   signed   int    signed_ADC;
   unsigned char   AD_execution_result;
   sensor_number_t sensor_unit;
} ADC_parameters_t;
/* This struct is used to hold parameters for Reading AD channels.           */
/*                                                                           */
/* 'ADC_channel'          Stores the number the AD channel to be measured.   */
/*                        Includes BP_UP bit to select unipolar/bipolar mode.*/
/* 'ADC_max_tries'        Gives the function the maximum amount of tries     */
/*                        used in reading and handling an AD channel.        */
/* 'conversion_max_tries' Gives the function the maximum amount of tries     */
/*                        used in reading and handling a single AD           */
/*                        conversion.                                        */
/* 'unsigned_ADC'         These variables are used for storing an ADC        */
/* 'signed_ADC'           result.                                            */
/* 'AD_execution_result'  Indicates whether the AD measurent is a success or */
/*                        indicates what has gone wrong with following       */
/*                        values.                                            */
/*  sensor_unit           See the function DAC_SelfTest.                     */
/*  CONVERSION_ACTIVE 0                                                      */
/*  RESULT_OK         1                                                      */
/*  HIT_OCCURRED      2                                                      */


typedef enum {
        su1_e,su2_e,su3_e,su4_e
} SU_index_t;



uint_least8_t EXTERNAL temp_meas_count = TEMP_COUNT;
/* This variable is used for counting temperature measurement interval       */
/* 1/60 secs.                                                                */
 
uint_least8_t EXTERNAL voltage_meas_count = VOLTAGE_COUNT;
/* This variable is used for counting voltage measurement interval 1/180     */
/* secs.                                                                     */

uint_least8_t EXTERNAL checksum_count = CHECK_COUNT;
/* This variable is used for counting  checksum counter, interval 1/60       */
/* secs.                                                                     */
                                                                             
unsigned char EXTERNAL code_checksum;
/* This variable is used for calculating checksum from the memory.           */

unsigned char EXTERNAL self_test_flag = SELF_TEST_DONE;
/* Used with SU self test initiation.                                  */

/* enumerations */
typedef enum  {
   round_0_e, round_1_e, round_2_e, round_3_e, round_4_e, 
   round_5_e, round_6_e, round_7_e, round_8_e, round_9_e    

} round_t;

typedef enum  {
   channel_0_e, channel_1_e, channel_2_e, channel_3_e, channel_4_e, 
   channel_5_e, channel_6_e    

} AD_channel_t;

unsigned char EXTERNAL ADC_channel_register = 0x80;
/* Holds value of the ADC Channel HW register       */
/* Is used by Hit Trigger ISR task and Health       */
/* Monitoring task.                                 */
/* Updating must be atomic in the Health Monitoring */
/* task, because Hit Trigger can preempt it.        */


dpu_time_t EXTERNAL internal_time;
/* DEBIE internal time counter. */


/*  Function prototypes. */

void DelayAwhile (unsigned short duration);
void Read_AD_Channel (ADC_parameters_t EXTERNAL * ADC_results);
void DAC_SelfTest(unsigned char DAC_output, 
                  ADC_parameters_t EXTERNAL * ADC_test_parameters) ;
void Monitor(uint_least8_t health_mon_round);
void UpdateTime(void);
void MeasureTemperature(sensor_index_t SU_index);
void HighVoltageCurrent(sensor_index_t SU_index);
void LowVoltageCurrent(void);
void MeasureVoltage(uint_least8_t channel_selector);
void InitSystem(void);
void CalculateChecksum(uint_least8_t checksum_count);
void UpdatePeriodCounter(uint_least8_t EXTERNAL * counter, 
                         uint_least8_t full_counter_value); 
void Convert_AD (ADC_parameters_t EXTERNAL * ADC_parameters);
void TemperatureFailure(sensor_index_t SU_index);
void VoltageFailure(channel_t ADC_channel);

void SelfTest_SU(sensor_index_t self_test_SU_index);
void Monitor_DPU_Voltage(void);
void Monitor_SU_Voltage(sensor_index_t self_test_SU_index);
unsigned char ExceedsLimit(
   unsigned char value,
   unsigned char lower_limit,
   unsigned char upper_limit);
void RestoreSettings(sensor_index_t self_test_SU_index);
void SelfTestChannel(sensor_index_t self_test_SU_index);
void ExecuteChannelTest(
   sensor_index_t self_test_SU_index,
   unsigned char  test_channel,
   unsigned char  test_pulse_start_level);


/* Other function prototypes. */

void Set_SU_TriggerLevels (
   sensor_number_t         sensor_unit,
   SU_settings_t EXTERNAL *settings);


/*****************************************************************************/
/*                         Boot and DPU self test                            */
/*****************************************************************************/

void Clear_RTX_Errors(void)
/* Purpose        : Clears RTX error registers in telemetry          */
/* Interface      : input   -                                        */
/*                  output  - telemetry_data, rtx error registers    */
/* Preconditions  : none                                             */
/* Postconditions : RTX error registers are cleared.                 */
/* Algorithm      : See below, self explanatory.                     */ 
{
   telemetry_data.isr_send_message_error    = 0xFF;
   telemetry_data.os_send_message_error     = 0xFF;
   telemetry_data.os_create_task_error      = 0xFF;
   telemetry_data.os_wait_error             = 0xFF;
   telemetry_data.os_attach_interrupt_error = 0xFF;
   telemetry_data.os_enable_isr_error       = 0xFF; 
   telemetry_data.os_disable_isr_error      = 0xFF;
}


void SetSoftwareError(unsigned char error) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : This function will be called always when                */
/*                  bit(s) in the software error status                     */
/*                  register are set.                                       */
/* Interface      : inputs      - software error status register            */
/*                              - measurement_error, which specifies what   */
/*                                bits are set in software error status.    */
/*                                Value is as follows,                      */
/*                                           									    */
/*                                MEASUREMENT_ERROR                         */
/*								                                                    */
/*                  outputs     - software error status register            */
/*                  subroutines - none                                      */
/* Preconditions  : none                                                    */
/* Postconditions : none                                                    */
/* Algorithm      : - Disable interrupts                                    */
/*                  - Write to software error status register               */
/*                  - Enable interrupts                                     */
{
   DISABLE_INTERRUPT_MASTER;

   telemetry_data.software_error |= error;

   ENABLE_INTERRUPT_MASTER;
}
      
void ClearSoftwareError()
/* Purpose        : This function will be called always when all            */
/*                  bits in the software error status                       */
/*                  register are cleared.                                   */
/* Interface      : inputs      - software error status register            */
/*                  outputs     - software error status register            */
/*                  subroutines - none                                      */
/* Preconditions  : none                                                    */
/* Postconditions : none                                                    */
/* Algorithm      :                                							    */
/*                  - Write to SoftwareErrorStatuRegister                   */

{
    telemetry_data.software_error = 0;
}

void SetModeStatusError(unsigned char mode_status_error) 
        COMPACT_DATA REENTRANT_FUNC
/* Purpose        : This function will be called always when               */
/*                  error bit(s) in the mode status register are set.      */
/* Interface      : inputs      - mode status register                     */
/*                              - mode_status_error, which specifies what  */
/*                                bit(s) are to be set in                  */
/*                                mode status register. Value is one of    */
/*                                the following,                           */
/*                                           									   */
/*                                  SUPPLY_ERROR                           */
/*                                  DATA_MEMORY_ERROR                      */
/*                                  PROGRAM_MEMORY_ERROR                   */
/*                                  MEMORY_WRITE_ERROR                     */
/*                                  ADC_ERROR                              */
/*                                           									   */
/*                  outputs     - mode status register                     */
/*                  subroutines - none                                     */
/* Preconditions  : none                                                   */
/* Postconditions : none                                                   */
/* Algorithm      : - Disable interrupts                                   */
/*                  - Write to Mode Status register                        */
/*                  - Enable interrupts                                    */
{
   DISABLE_INTERRUPT_MASTER;

   telemetry_data.mode_status |= (mode_status_error & (~MODE_BITS_MASK));
   /* The mode bits are secured against unintended modification by */
   /* clearing those bits in 'mode_status_error' before "or":ing   */
   /* its value to 'telemetry_data.mode_status'.                   */


   ENABLE_INTERRUPT_MASTER;
}
void ClearModeStatusError(void)
/* Purpose        : This function will be called always when all           */
/*                  error bits in the mode status register are cleared.    */
/* Interface      : inputs      - mode status register                     */
/*        								   */
/*                  outputs     - mode status register                     */
/*                  subroutines - none                                     */
/* Preconditions  : none                                                   */
/* Postconditions : none                                                   */
/* Algorithm      : - Disable interrupts                                   */
/*                  - Write to Mode Status register                        */
/*                  - Enable interrupts                                    */ 
{
   DISABLE_INTERRUPT_MASTER;

   telemetry_data.mode_status &= MODE_BITS_MASK;
   /* Error bits in the mode status register are cleared. */

   ENABLE_INTERRUPT_MASTER;
}

void SetMode(DEBIE_mode_t mode) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : This function will be called always when                */
/*                  mode in the mode status register is set.                */
/* Interface      : inputs      - mode status register                      */
/*                                mode_bits, which specify the mode to be   */
/*                                stored in the mode status register.       */
/*                                Value is on one of the following:         */
/*                                   DPU self test                          */
/*                                   stand by                               */
/*                                   acquisition                            */
/*                                       								             */
/*                  outputs     - mode status register                      */
/*                  subroutines - none                                      */
/* Preconditions  : none                                                    */
/* Postconditions : none                                                    */
/* Algorithm      : - Disable interrupts                                    */
/*                  - Write to Mode Status register                         */
/*                  - Enable interrupts                                     */
                       
{
   DISABLE_INTERRUPT_MASTER;

   telemetry_data.mode_status = (telemetry_data.mode_status & ~MODE_BITS_MASK)
                                   | (mode & MODE_BITS_MASK);     
   /* First mode status bits are cleared, and then the given mode is set. */

   ENABLE_INTERRUPT_MASTER;
}

DEBIE_mode_t GetMode()
/* Purpose        : This function will be called always when                */
/*                  mode in the mode status register is checked.            */
/* Interface      :                                                         */
/*                  inputs      - mode status register                      */
/*									                                                 */
/*                  outputs     - mode status register                      */
/*                              - Mode bits, which specify the mode         */
/*                                stored in the ModeStatus register.        */
/*                                Value is on one of the following:         */
/*                                   DPU self test                          */
/*                                   stand by                               */
/*                                   acquisition                            */
/*   								                                                 */
/*                  subroutines - none                                      */
/* Preconditions  : none                                                    */
/* Postconditions : none                                                    */
/* Algorithm      :                					                            */
/*                  - Read Mode Status register                             */
                  
      
{
   return(telemetry_data.mode_status & MODE_BITS_MASK);
   /* Return the value of the two least significant bits in */
   /* mode status register and return this value.           */    
}


void Clear_SU_Error(void)
/* Purpose        : This function will be called always when all           */
/*                  error bits in the SU# status register are cleared.     */
/* Interface      : inputs      - SU# status register                      */
/*        								                                          */
/*                  outputs     - SU# status register                      */
/*                  subroutines - none                                     */
/* Preconditions  : none                                                   */
/* Postconditions : none                                                   */
/* Algorithm      : - Disable interrupts                                   */
/*                  - Write to Mode Status register                        */
/*                  - Enable interrupts                                    */ 
{
   sensor_index_t EXTERNAL i;
 
   DISABLE_INTERRUPT_MASTER;
   for (i = 0; i < NUM_SU; i++)
   {
      telemetry_data.SU_status[i] &= SUPPLY_VOLTAGE_MASK;
      /* Error bits in the SU# status register are cleared. */
   }

   ENABLE_INTERRUPT_MASTER;
}


void Set_SU_Error(sensor_index_t SU_index, unsigned char SU_error)
/* Purpose        : This function will be called always when               */
/*                  error bit(s) in the SU# status register are set.       */
/* Interface      : inputs      - SU# status register                      */
/*        	                    - 'SU_index' (0-3)                         */
/*                              - 'SU_error' is one of the following:      */
/*                                                                         */
/*                                 LV_SUPPLY_ERROR                         */
/*                                 HV_SUPPLY_ERROR                         */
/*                                 TEMPERATURE_ERROR                       */
/*                                 SELF_TEST_ERROR                         */
/*                                                                         */
/*                  outputs     - SU# status register                      */
/*                  subroutines - none                                     */
/* Preconditions  : none                                                   */
/* Postconditions : none                                                   */
/* Algorithm      : - Disable interrupts                                   */
/*                  - Write to SU# status register                         */
/*                  - Set corresponding SU# error bit in the               */
/*                    error status register.                               */
/*                  - Enable interrupts                                    */ 
{
 
   DISABLE_INTERRUPT_MASTER;
   
      telemetry_data.SU_status[SU_index] |= 
         (SU_error &(~SUPPLY_VOLTAGE_MASK));
      /* Error bits in the SU# status register are cleared. */
      /* The voltage status bits in the SU# status register */
      /* are secured against unintended modification by     */
      /* clearing those bits in 'SU_error' before           */
      /* "or":ing its value to                              */
      /* 'telemetry_data.SU_status'.                        */
 
      SetErrorStatus(ERROR_STATUS_OFFSET << SU_index);      
      /* SU# error is set in the error status register, if      */
      /* anyone of the error bits in the SU# status register    */
      /* is set.                                                */
      /* Because this subroutine enables itself the interrupts, */
      /* the call of it must be the last operation in the       */ 
      /* interrupt blocked area !                               */

   ENABLE_INTERRUPT_MASTER;
}


void Set_SU_TriggerLevels (
   sensor_number_t         sensor_unit,
   SU_settings_t EXTERNAL *settings)
/* Purpose        : Set all trigger-levels of one SU.                   */
/* Interface      : inputs      - SU number in 'sensor_unit'.           */
/*                              - Triggering levels in 'settings'.      */
/*                  outputs     - Hardware trigger levels.              */
/*                  subroutines - SetTriggerLevel                       */
/* Preconditions  : none                                                */
/* Postconditions : none                                                */
/* Algorithm      : - set trigger level for Plasma 1+                   */
/*                  - set trigger level for Plasma 1-                   */
/*                  - set trigger level for Piezos.                     */ 
{   
   trigger_set_t EXTERNAL trigger;
   /* Holds parameters for SetTriggerLevel. */

   trigger.sensor_unit = sensor_unit;
 
   trigger.level   = settings -> plasma_1_plus_threshold;
   trigger.channel = PLASMA_1_PLUS;
   SetTriggerLevel (&trigger);
 
   trigger.level   = settings -> plasma_1_minus_threshold;
   trigger.channel = PLASMA_1_MINUS;
   SetTriggerLevel (&trigger);
 
   trigger.level   = settings -> piezo_threshold;
   trigger.channel = PZT_1_2;
   SetTriggerLevel (&trigger);
}


void SetErrorStatus(unsigned char error_source)
/* Purpose        : This function will be called always when               */
/*                  error bit(s) in the error status register are set.     */
/*                  Exceptionally TC_ERROR will be set with a separate     */
/*                  function as it is used so often.                       */
/* Interface      : inputs      - error status register                    */
/*                              - 'error_source' specifies the error bit   */
/*                                to be set. Its value is one of the       */
/*                                following,                               */
/*                                                                         */
/*                                SU4_ERROR                                */
/*                                SU3_ERROR                                */
/*                                SU2_ERROR                                */
/*                                SU1_ERROR                                */
/*                                CHECKSUM_ERROR                           */
/*                                WATCHDOG_ERROR                           */
/*                                PARITY_ERROR                             */
/*                                                                         */
/*                  outputs     - error status register                    */
/*                  subroutines - none                                     */
/* Preconditions  : none                                                   */
/* Postconditions : none                                                   */
/* Algorithm      : - Disable interrupts                                   */
/*                  - Write to error status register                       */
/*                  - Enable interrupts                                    */ 
{
 
   DISABLE_INTERRUPT_MASTER;
   
      telemetry_data.error_status |= (error_source &(~TC_ERROR));
      /* Error bits in the error status register are set.   */
      /* The TC_ERROR bit in the error status register      */
      /* is secured against unintended modification by      */
      /* clearing that bit in 'error_source' before         */
      /* "or":ing its value to                              */
      /* 'telemetry_data.error_status'.                     */
 

   ENABLE_INTERRUPT_MASTER;
}


void ClearErrorStatus()
/* Purpose        : This function will be called always when               */
/*                  error bits in the error status register are cleared.   */
/* Interface      : inputs      - error status register                    */
/*                                                                         */
/*                  outputs     - error status register                    */
/*                  subroutines - none                                     */
/* Preconditions  : none                                                   */
/* Postconditions : none                                                   */
/* Algorithm      :	                               					         */
/*                  - Write to error status register                       */

{
      telemetry_data.error_status = 0;
      /* Error bits in the error status register are      */
      /* cleared.			                  */
}


void DPU_SelfTest (void)
/* Purpose        : Executes the DPU voltage self test.                      */
/* Interface      : inputs      - none                                       */
/*                                                                           */
/*                  outputs     - none                                       */
/*                  subroutines - Monitor_DPU_Voltage                        */
/* Preconditions  : none                                                     */
/* Postconditions : - Chosen supply voltages are measured.                   */
/* Algorithm      : - Chosen supply voltages are measured and monitored with */
/*                    Monitor_DPU_Voltage                                    */
{
   Monitor_DPU_Voltage();
}


void Boot(void)
/* Purpose        : Executes Boot sequence                                   */
/* Interface      : inputs      - failed_code_address                        */
/*                              - failed_data_address                        */
/*                  outputs     - intialized state of all variables          */
/*                  subroutines - SetSensorUnitOff                           */
/*                                GetResetClass                              */
/*                                SignalMemoryErros                          */
/*                                ResetDelayCounters                         */
/*                                InitClassification                         */
/*                                ClearErrorStatus                           */
/*                                ClearSoftwareError                         */
/*                                Set_SU_TriggerLevels                       */
/* Preconditions  : Init_DPU called earlier, after reset.                    */
/*                  Keil C startup code executed; xdata RAM initialised.     */
/*                  Tasks are not yet running.                               */
/* Postconditions : DAS variables initialised.                               */
/*                  All Sensor Units are in off state                        */
/*                  If boot was caused by power-up reset, TM data registers  */
/*                  and Science Data File are initialized                    */
/*                  If boot was not caused by watchdog-reset, error counters */
/*                  are cleared                                              */
/*                  DEBIE mode is DPU_SELF_TEST                              */
/* Algorithm      : see below.                                               */

{
   EXTERNAL unsigned char  execution_result;
   /* Execution result for SetSensorUnitOff function.  */

   EXTERNAL unsigned char * DIRECT_INTERNAL fill_pointer;
   /* Used for data structure initialization */

   DIRECT_INTERNAL reset_class_t reset_class;
   /* What kind of reset caused this boot ? */

   DIRECT_INTERNAL unsigned int i;
   /* Loop variable */


   SU_ctrl_register |= 0x0F;
   SET_DATA_BYTE(SU_CONTROL,SU_ctrl_register);
   /* Set all Peak detector reset signals to high */

   max_events = MAX_EVENTS;

   ResetDelayCounters();

   SetSensorUnitOff(su1_e, &execution_result);
   /* Set Sensor Unit 1 to Off state */

   SetSensorUnitOff(su2_e, &execution_result);
   /* Set Sensor Unit 2 to Off state */

   SetSensorUnitOff(su3_e, &execution_result);
   /* Set Sensor Unit 3 to Off state */

   SetSensorUnitOff(su4_e, &execution_result);
   /* Set Sensor Unit 4 to Off state */

   ADC_channel_register |= 0x80;
   UPDATE_ADC_CHANNEL_REG;
   /* ADC interleave calibration is not used. */

   reset_class = GetResetClass();
   /* Find out what caused the reset. */

   
   if (reset_class != warm_reset_e)
   {
      /* We are running the PROM code unpatched, either   */
      /* from PROM or from SRAM.                          */
      
      reference_checksum = INITIAL_CHECKSUM_VALUE;
      /* 'reference_checksum' is used as a reference when */
      /* the integrity of the code is checked by          */
      /* HealthMonitoringTask. It is set to  its initial  */
      /* value here, after program code is copied from    */
      /* PROM to RAM.                                     */
   }

   if (reset_class == power_up_reset_e)
   { 
      /* Data RAM was tested and is therefore garbage. */
      /* Init TM data registers and Science Data File. */

      internal_time = 0;
      
      fill_pointer = (EXTERNAL unsigned char * DIRECT_INTERNAL)&telemetry_data;

      for (i=0; i < sizeof(telemetry_data); i++)
      {
         *fill_pointer = 0;
         fill_pointer++;
      }

      ResetEventQueueLength();
      /* Empty event queue. */

      ClearEvents();
      /* Clears the event counters, quality numbers  */
      /* and free_slot_index of the event records in */
      /* the science data memory.                    */

      InitClassification();
      /* Initializes thresholds, classification levels and */
      /* min/max times related to classification.          */

      Clear_RTX_Errors();
      /* RTX error indicating registers are initialized. */

   }

   else if (reset_class == watchdog_reset_e)
   {
      /* Record watchdog failure in telemetry. */

      telemetry_data.error_status |= WATCHDOG_ERROR;

      if (telemetry_data.watchdog_failures < 255)
      {
         telemetry_data.watchdog_failures++;
      }
   }

   else if (reset_class == checksum_reset_e)
   {
      /* Record checksum failure in telemetry. */	
	
      telemetry_data.error_status |= CHECKSUM_ERROR;       
      
      if (telemetry_data.checksum_failures < 255)
      {
         telemetry_data.checksum_failures++;
      }
   } 

   else
   {
      /* Soft or Warm reset. */
      /* Preserve most of telemetry_data; clear some parts. */
 
      ClearErrorStatus();  
      Clear_SU_Error();
      Clear_RTX_Errors();
      ClearSoftwareError();
      telemetry_data.mode_status       &= MODE_BITS_MASK;
      telemetry_data.watchdog_failures  = 0;
      telemetry_data.checksum_failures  = 0;
      telemetry_data.TC_word            = 0;
      /* Clear error status bits, error status counters */
      /* and Command Status register.                   */

      ResetEventQueueLength();
      /* Empty event queue. */

      ClearEvents();
      /* Clears the event counters, quality numbers  */
      /* and free_slot_index of the event records in */
      /* the science data memory.                    */

      InitClassification();
      /* Initializes thresholds, classification levels and */
      /* min/max times related to classification.          */

      self_test_SU_number = NO_SU;
      /* Self test SU number indicating parameter */
      /* is set to its default value.             */                       
   }


   telemetry_data.mode_status = 
      (telemetry_data.mode_status & ~MODE_BITS_MASK) | DPU_SELF_TEST;
   /* Enter DPU self test mode. */

   /* Software version information is stored in the telemetry data. */
   telemetry_data.SW_version = SW_VERSION;

   SignalMemoryErrors();
   /* Copy results of RAM tests to telemetry_data. */

   SetTestPulseLevel(DEFAULT_TEST_PULSE_LEVEL);
   /* Initializes test pulse level. */

   Set_SU_TriggerLevels (SU_1, &telemetry_data.sensor_unit_1);
   Set_SU_TriggerLevels (SU_2, &telemetry_data.sensor_unit_2);
   Set_SU_TriggerLevels (SU_3, &telemetry_data.sensor_unit_3);
   Set_SU_TriggerLevels (SU_4, &telemetry_data.sensor_unit_4);

}


/*****************************************************************************/
/*                         Health Monitoring Task                            */
/*****************************************************************************/


uint_least8_t EXTERNAL health_mon_round = HEALTH_COUNT;
/* This variable is used for counting the ten health monitoring rounds    */
/* which altogether equal 10 secs.                                        */


void InitHealthMonitoring (void)
/* Purpose        : Initialize the health monitoring for DEBIE.              */
/* Interface      : inputs      - none                                       */
/*                                                                           */
/*                  outputs     - telemetry_data                             */
/*                                                                           */
/*                  subroutines - InitSystem()                               */
/*                                DPU_SelfTest()                             */
/*                                SetMode()                                  */
/*                                                                           */
/* Preconditions  : Debie is on                                              */
/* Postconditions : See subroutines                                          */
/* Algorithm      :                                                          */
/*                   - Executes InitSystem()                                 */
/*                   - Executes DPU_SelfTest()                               */
/*                   - Enter stand_by mode                                   */
{
   InitSystem();
   /* Initializes the system.    */ 

   DPU_SelfTest();
   /* Execute the DPU self test. */

   SetMode(STAND_BY);
   /* Switch to Standby mode */
}


void HandleHealthMonitoring (void)
/* Purpose        : One round of health monitoring for DEBIE.                */
/* Interface      : inputs      - telemetry_data                             */
/*                                                                           */
/*                  outputs     - telemetry_data                             */
/*                                                                           */
/*                  subroutines - UpdateTime()                               */
/*                                Monitor()                                  */
/*                                UpdatePeriodCounter()                      */
/*                                WaitInterval()                             */
/*                                                                           */
/* Preconditions  : Debie is on                                              */
/* Postconditions : See subroutines                                          */
/* Algorithm      :                                                          */
/*                  - Updates sensor unit states                             */
/*                  - Executes UpdateTime()                                  */
/*                  - Calls Monitor() function                               */
/*                  - UpdatePeriodCounter() function advances the            */
/*                    health monitoring counter                              */
/*                  - Executes WaitInterval()                                */
{
   Update_SU_State (0);
   Update_SU_State (1);
   Update_SU_State (2);
   Update_SU_State (3);

   UpdateTime();
   /* Update telemetry registers. */

   Monitor(health_mon_round);
   /* Execute current Health Monitoring Round.                            */

   UpdatePeriodCounter(&health_mon_round, HEALTH_COUNT);
   /* Decrease or reset health monitor loop counter depending on its      */
   /* current and limiting values.                                        */

   WaitInterval(HM_INTERVAL);    
   /* Wait for next activation */
}


void HealthMonitoringTask(void) TASK(HEALTH_MONITORING_TASK) 
                                PRIORITY(HEALTH_MONITORING_PR)
/* Purpose        : Takes care of health monitoring for DEBIE.               */
/* Interface      : inputs      - telemetry_data                             */
/*                                                                           */
/*                  outputs     - telemetry_data                             */
/*                                                                           */
/*                  subroutines - UpdateTime()                               */
/*                                Monitor()                                  */
/*                                UpdatePeriodCounter()                      */
/*                                WaitInterval()                             */
/*                                                                           */
/* Preconditions  : Debie is on                                              */
/* Postconditions : See subroutines                                          */
/* Algorithm      :                                                          */
/*                  - InitHealthMonitoring                                   */
/*                  - loop forever:                                          */
/*                  -   HandleHealthMonitoring                               */
{
   InitHealthMonitoring ();

   while(1)
   {
      HandleHealthMonitoring ();
   }
}	


void Monitor(uint_least8_t health_mon_round)
/* Purpose        : Monitors DEBIE's vital signs                             */
/* Interface      : inputs      - Health Monitoring Round count              */
/*                  outputs     - none                                       */
/*                  subroutines - MeasureTemperature()                       */
/*                                CheckCurrent()                             */
/*                                MeasureVoltage()                           */
/*                                CalculateChecksum()                        */
/* Preconditions  : Health monitoring is on.                                 */
/* Postconditions : Health monitoring duties are carried out.                */
/* Algorithm      :                                                          */
/*                     - Executes the given health monitor loop round.       */
/*                     - Starts three loops:                                 */
/*                         - voltage measurement loop      180 secs          */
/*                         - temperature measurement loop  60 secs           */
/*                         - checksum count loop           60 secs           */
/*                     The values of these counters are decreased after each */
/*                     loop cycle.                                           */
/*                                                                           */
/*                     Health monitoring loop which lasts 10 secs and is     */
/*                     divided into 10 individual rounds. On each round      */
/*                     some specific Health Monitoring duties are carried    */
/*                     out. For example the Watchdog counter is resetted.    */
/*                                                                           */
/*                     Temperature measurement loop lasts 60 secs and        */
/*                     consists of 6 Health Monitoring loop cycles each      */
/*                     lasting 10 secs. It is executed partly on each 10 sec */
/*                     Health Monitoring cycle by measuring temperatures of  */
/*                     one SU on each cycle. Measurement starts on the second*/
/*                     Health Monitoring cycle and is completed after six    */
/*                     cycles.                                               */
/*                                                                           */
/*                     Voltage measurement loop lasts 180 secs and consists  */
/*                     of 18 Health Monitoring loop cycles each lasting 10   */
/*                     secs. On each cycle some of its duties are carried    */
/*                     out.                                                  */
/*                                                                           */
/*                     Checksum calculation loop lasts 60 secs and consists  */
/*                     of 6 Health Monitoring loop cycles each lasting 10    */
/*                     secs. It is executed  partly on each Health Monitoring*/
/*                     round.                                                */
/*                                                                           */
/*    Illustration of the process:                                           */
/*                                                                           */
/*              M                                                            */
/*     _________|_________                                                   */
/*    |                   |                                                  */
/*    |R-R-R-R-R-R-R-R-R-R> : Health Monitoring loop: M                      */
/*    <---------------------<   Round: R = 1 sec                             */
/*                              M = 10*R = 10 secs                           */
/*                                                                           */
/*          T                                                                */
/*     _____|_____                                                           */
/*    |           |                                                          */
/*    |M-M-M-M-M-M>   Temperature Measurement loop: T                        */
/*    <-----------<   Health Monitoring loop: M = 10 sec                     */
/*                    T = 6*M = 60 secs                                      */
/*                                                                           */
/*                  C                                                        */
/*     _____________|____________     Checksum count loop: C                 */
/*    |                          |                                           */
/*    |R-R-R-R-R-R-R-R-R-R>                                                  */
/*      >R-R-R-R-R-R-R-R-R-R>                                                */
/*       >R-R-R-R-R-R-R-R-R-R>                                               */
/*        >R-R-R-R-R-R-R-R-R-R>                                              */
/*         >R-R-R-R-R-R-R-R-R-R>                                             */
/*          >R-R-R-R-R-R-R-R-R-R>                                            */
/*                                Health Monitoring loop cycle: R = 1 sec    */
/*                                C = 60*R = 60 secs                         */
/*                                                                           */
/*                      V                                                    */
/*     _________________|__________________                                  */
/*    |                                   |                                  */
/*    |M-M-M-M-M-M-M-M-M-M-M-M-M-M-M-M-M-M>   Voltage Measurement loop: V    */
/*    <-----------------------------------<   Health Monitoring loop: M      */
/*                                            V = 18*M = 180 secs            */
{
   CalculateChecksum(checksum_count);
   /* A 1/60th part of the memory checksum is calculated.                 */

   UpdatePeriodCounter(&checksum_count, CHECK_COUNT);
   /* Decrease or reset checksum counter                                  */
   /* depending on its current and limiting values.                       */


   switch (health_mon_round)
   {
      case round_0_e:
      
         HighVoltageCurrent((sensor_index_t)health_mon_round);
         /* Overcurrent indicating bits related to sensor unit 1 in HV       */
         /* status register are checked.                                     */

         UpdatePeriodCounter(&temp_meas_count, TEMP_COUNT);
         UpdatePeriodCounter(&voltage_meas_count, VOLTAGE_COUNT);
         /* Decrease or reset temperature, checksum and voltage counters     */
         /* depending on their current and limiting values.                  */

        
         hit_budget_left = hit_budget;
         /* Health Monitoring period ends and new hit budget can be started. */

         if (HIT_TRIGGER_FLAG == 0)
         {
            /* Hit budget was exceeded during this ending Health Monitoring */
            /* period.                                                      */

            ResetPeakDetector(SU_1);
            ResetPeakDetector(SU_2);
            ResetPeakDetector(SU_3);
            ResetPeakDetector(SU_4);
            /* Reset all Peak detectors */

            WaitTimeout(COUNTER_RESET_MIN_DELAY);

            ENABLE_HIT_TRIGGER;
            /* Allows a later falling edge on T2EX to cause */
            /* a Hit Trigger interrupt (i.e. to set EXF2).  */

            ResetDelayCounters();
            /* Resets the SU logic that generates Hit Triggers.    */
            /* Brings T2EX to a high level, making a new falling   */
            /* edge possible.                                      */
            /* This statement must come after the above "enable",  */
            /* since T2EX edges are not remembered by the HW from  */
            /* before the "enable", unlike normal interrupt enable */
            /* and disable masking.                                */
         }
  
         break;
	
      case round_1_e:

         HighVoltageCurrent((sensor_index_t)health_mon_round);
         /* Overcurrent indicating bits related to sensor unit 2 in HV       */
         /* status register are checked.                                     */

         break;

      case round_2_e:                                   

         HighVoltageCurrent((sensor_index_t)health_mon_round);
         /* Overcurrent indicating bits related to sensor unit 3 in HV       */
         /* status register are checked.                                     */

         break;

      case round_3_e:

         HighVoltageCurrent((sensor_index_t)health_mon_round);
         /* Overcurrent indicating bits related to sensor unit 4 in HV       */
         /* status register are checked.                                     */

         break;

      case round_4_e:

         LowVoltageCurrent();
         /* 'V_DOWN' indicator bit is checked.                               */


         break;

      case round_5_e:

         if (voltage_meas_count < 7)

         {
            /* Seven Secondary voltage channels are measured starting when   */
            /* 'voltage_meas_count' reaches a value of 6. Last measurement is*/
            /*  executed on voltage_meas_count value 0.                      */
 
            MeasureVoltage(voltage_meas_count);
         } 

         break;

      case round_6_e:
 
         if ((self_test_SU_number               != NO_SU) &&
	     (SU_state[self_test_SU_number - SU1] == self_test_e))
         {
            /* SU self test sequence continues   */

            SelfTestChannel(self_test_SU_number - SU1);
            /* SU channels are monitored in this round. */
 
            self_test_SU_number = NO_SU;
            /* SU self test sequence ends here  */
         }
       
         break;
 
      case round_7_e:
 
         if (self_test_SU_number != NO_SU)
         {
            /* SU self test sequence has started   */
 
            self_test_flag = SELF_TEST_RUNNING;
            /* Indication of a started test. */

            SelfTest_SU(self_test_SU_number - SU1);
            /* Supply voltages and SU temperatures are monitored in this round. */

            if (self_test_SU_number != NO_SU)
            {
               SU_state[self_test_SU_number - SU1] = self_test_e;
            }

         }

         break;

      case round_8_e:

         SET_WD_RESET_HIGH;

         /* The Watch Dog time out signal state is reset HIGH state, as it is*/
         /* falling edge active.                                             */

         break;

      case round_9_e:

         if (temp_meas_count < NUM_SU)

         {
            /* Two channels of one sensor unit are measured when             */
            /* 'temp_meas_count' reaches 3 -> 2 -> 1 -> 0. I.e. measuring    */
            /*  begins after 10 secs and is finished after 50 secs.          */
 
            MeasureTemperature((sensor_index_t)temp_meas_count);
         }

         SET_WD_RESET_LOW;

         /* The Watch Dog timer is reset by setting WD_RESET bit low at I/O  */
         /* port 1. This is done here with 10 sec interval. The watch dog    */
         /* time-out is 12.1 secs.                                           */
         break;   
   }

}

void UpdateTime(void)
/* Purpose        : advances time in the telemetry                           */
/* Interface      : inputs      - telemetry_data.time                        */
/*                  outputs     - telemetry_data.time                        */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : Time updated in telemetry_data.                          */
/* Algorithm      :                                                          */
/*                  Time in the telemetry_data.time is advanced until        */
/*                  maximum value for the variable is reached, after that it */
/*                  is implicitely wrapped-around on overflow.               */
{

   DISABLE_INTERRUPT_MASTER;
   /* Disable all interrupts.                                             */

   internal_time ++;
   /* Increment internal time. */
                                           
   ENABLE_INTERRUPT_MASTER;
   /* Enable all interrupts.                                              */

}


void MeasureTemperature(sensor_index_t SU_index)
/* Purpose        : Measures and monitors SU temperatures                    */
/* Interface      : inputs      - SU_index, sensor unit index  (0 - 3)       */
/*                                telemetry_data                             */
/*                  outputs     - telemetry_data                             */
/*                  subroutines - Read_AD_Channel()                          */
/*                                TemperatureFailure()                       */
/* Preconditions  : none                                                     */
/* Postconditions : - ADC temperature channels are measured.                 */
/*                  - In case of an overheated SU,                           */
/*                    'TemperatureFailure()' function is called.i.e.         */
/*                    all secondary supply voltages to that SU are switched  */
/*                    off, SU related Bit in the Error Status Register is set*/
/*                    and temperature error indicating bit in the SU_Status  */
/*                    register is set.                                       */
/*                  - If a measurement has failed, in addition to overheating*/
/*                    response also measurement error indication bit in mode */
/*                    status register is set                                 */
/* Algorithm      : - Temperatures of a given sensor unit are measured.      */
/*                  - If measured temperature is large enough, it is stored  */
/*                    into telemetry.                                        */
/*                                                                           */
/*                  - Else measured temperature value is too small to be     */
/*                    stored. Zero value is stored into telemetry.           */
/*                                                                           */
/*                  - If temperature of any of the Sensor Units is over      */
/*                    MAX_TEMP, 'TemperatureFailure()' function is           */
/*                    called.                                                */
/*                                                                           */
/*                  - If temperature measurement of a Sensor Unit has failed,*/
/*                    temperature error indicating bit in the SU_Status      */
/*                    register is set and TemperatureFailure()'              */
/*                    function is called.                                    */
{
                                                           
   ADC_parameters_t EXTERNAL AD_temperature_parameters;
   /* This struct is used to hold parameters for Reading AD channels.        */
 
   unsigned char temp_limit_value;

   uint_least8_t EXTERNAL j;  
   /* This variable is used in for-loop.                                     */



   for (j=0; j < NUM_TEMP; j++)

   {
      AD_temperature_parameters.ADC_channel = 
         5 + (SU_index&1)*8 + (SU_index&2)*12 + j;
      /* Select the channel to be measured.                                  */
      
      AD_temperature_parameters.ADC_max_tries = ADC_TEMPERATURE_MAX_TRIES; 
      /* When temperatures are measured this variable defines the maximum    */
      /* amount of tries to be used in reading and handling an AD channel in */
      /* 'Read_AD_Channel()'.                                                */

      AD_temperature_parameters.conversion_max_tries = 
         CONVERSION_TEMPERATURE_MAX_TRIES;
      /* When temperatures are measured this variable defines the maximum    */
      /* amount of tries to be used when End Of Conversion indication is     */
      /* waited in function 'Convert_AD()'.                                  */
                                 
      Read_AD_Channel(&AD_temperature_parameters);
      /* Get ADC temperature measurement result.                             */


					
      if (AD_temperature_parameters.unsigned_ADC & 0x8000)

      {
         /* Temperature is stored in the telemetry.                          */
       	            
         telemetry_data.SU_temperature[SU_index][j] = 
         (unsigned char)((AD_temperature_parameters.unsigned_ADC
         & 0x7FFF) >> 7);
         /* Store bits 7 .. 14 */
      }

      else

      {                  

         telemetry_data.SU_temperature[SU_index][j] = 0;
         /* Temperature too small -> store zero */

      }
  
      temp_limit_value = ( j==0 ? MAX_TEMP_1 : MAX_TEMP_2 );
 
      if (telemetry_data.SU_temperature[SU_index][j] > temp_limit_value)
         
      {
         /* Temperature has exeeded a predefined limit                       */

         TemperatureFailure(SU_index);
         /* Given SU is switched off, error and SU status registers are      */
         /* updated in telemetry.                                            */

      }	

      if (AD_temperature_parameters.AD_execution_result != RESULT_OK) 

      {
         /* An anomaly has occurred during the measurement.                  */

         SetSoftwareError(MEASUREMENT_ERROR);         
         /* Set measurement error indication bit in   */
         /* software error status register.           */   




                 
         TemperatureFailure(SU_index);
         /* Given SU is switched off and error and SU status registers are   */
         /* updated in telemetry.                                            */
                                        
 	}	      

   }
}       


void HighVoltageCurrent(sensor_index_t SU_index)
/* Purpose        : Monitors overcurrent indicating bits in the HV Status    */
/*                  register for a given sensor unit.                        */
/* Interface      : inputs      - SU_index, sensor unit index (0 - 3)        */
/*                                telemetry_data                             */
/*                                HV_status register                         */
/*                  outputs     - telemetry_data                             */
/*                  subroutines - SetErrorStatus()                           */
/*                                Set_SU_Error()                             */
/* Preconditions  : none                                                     */
/* Postconditions : following registers are updated in case of an error,     */
/*                  - Sensor Unit or units are switched off                  */
/*                  - Error Status register updated                          */
/*                  - SU Status register updated                             */
/* Algorithm      :                                                          */
/*                  - if any of the HV_Status bits indicate a short          */
/*                    circuit or overload, the corresponding Error Status    */
/*                    Bits in the Error Status and SU_Status Registers are   */
/*                    set.                                                   */
{
   unsigned char EXTERNAL SU_current_mask[] = {3,12,48,192};
   /* This array holds parameters for checking the HV status register.       */

   unsigned char EXTERNAL valid_value[] = {1,4,16,64};
   /* This array holds comparison parameters for checking the HV status      */
   /* register.                                                              */
 
   if (CHECK_CURRENT(SU_current_mask[SU_index]) != 
           valid_value[SU_index]) 

   {
      /* Overcurrent is detected.                                            */

      SetErrorStatus(ERROR_STATUS_OFFSET << SU_index);
      /* Set high corresponding bit for the SU in Error Status Register.     */

      Set_SU_Error(SU_index, HV_SUPPLY_ERROR);
      /* Set high HV supply error indicating bit in the SU_Status register*/

   }
  
}

void LowVoltageCurrent(void)
/* Purpose        : Monitors low voltage currents in Sensor Units.           */
/* Interface      : inputs      - telemetry_data                             */
/*                                        V_DOWN bit                         */
/*                  outputs     - telemetry_data                             */
/*                  subroutines - SetSensorUnitOff()                         */
/* Preconditions  : none                                                     */
/* Postconditions : Following actions are taken in case of an error,         */
/*                  - Sensor Unit or units are switched off                  */
/*                  - SU Status register updated                             */
/*                  - Error Status register updated                          */
/* Algorithm      :                                                          */
/*                  - If V_DOWN bit in I/O port 1 is LOW indicating that     */
/*                    +-5 V DC/DC converter(s) is(are) limiting the output   */
/*                    current, all SU supply voltages are                    */
/*                    switched off and corresponding bits in the Error and   */
/*                    mode Status Register are set.                          */
{

   sensor_index_t EXTERNAL i;
   /* This variable is used in a for-loop.                                   */

   unsigned char EXTERNAL exec_result;
   /* This variable is used by SetSensorUnitOff() function.                 */

   if (V_DOWN == LOW)

   {
      /*  An error is detected, output current is limited.                   */

      for (i = 0; i < NUM_SU; i++)

      {
         /* Switch off all Sensor Units. */

         SetSensorUnitOff(i,&exec_result);
         /* Switch off given sensor unit.                                 */

         Set_SU_Error(i,LV_SUPPLY_ERROR);
         /* Set high LV supply error indicating bit in the SU_Status      */
         /* register.                                                     */
      }
      
      SetErrorStatus(OVERALL_SU_ERROR);
     /* Set all SU error status bits in 'error status register' at telemetry.*/

   }
}

void MeasureVoltage(uint_least8_t channel_selector)
/* Purpose        : Measure secondary Sensor Unit voltages.                  */
/* Interface      : inputs      - Channel selector, values 0 - 6             */
/*                  outputs     - telemetry_data.mode_status                 */
/*                  subroutines - Read_AD_Channel()                          */
/*                                VoltageFailure()                           */
/* Preconditions  : none                                                     */
/* Postconditions : - Measurement results are written to telemetry.          */
/*                  - If a measurement has failed, measurement error         */
/*                    indication bit in mode status register is set and      */
/*                    'VoltageFailure()' function is called.i.e.             */
/*                    SUs related to failed ADC channel are switched         */
/*                    off, SU related Bit in the Error Status Register is set*/
/*                    and LV error indicating bit in the SU_Status           */
/*                    register is set.                                       */
/* Algorithm      :    - Secondary SU supply voltages are measured from      */
/*                       a given channel.                                    */
/*                     - If measurement has failed,                          */
/*                       measurement error indication bit in mode status     */
/*                       register is set and 'VoltageFailure()'              */
/*                       function is called.                                 */
/*                     - If no errors have occurred, results are stored      */
/*                       in telemetry_data.                                  */
{

   ADC_parameters_t EXTERNAL AD_voltage_parameters;
   /* This struct is used to hold parameters for Reading AD channels.        */

   unsigned char EXTERNAL voltage_channel[] = {
      0x10,0x11,0x12,0x13,0x54,0x55,0x56};
   /* This array holds parameters for setting the ADC channel for the        */
   /* measurement.                                                           */

   AD_voltage_parameters.ADC_channel = voltage_channel[channel_selector];
   /* Select the channel to be measured:                                     */
   /* channel_selector ->  ADC channel                                       */
   /*                0 ->  0x10                                              */
   /*                1 ->  0x11                                              */
   /*                2 ->  0x12                                              */
   /*                3 ->  0x13                                              */
   /*                4 ->  0x54                                              */
   /*                5 ->  0x55                                              */
   /*                6 ->  0x56                                              */

   AD_voltage_parameters.ADC_max_tries = ADC_VOLTAGE_MAX_TRIES; 
   /* When voltages are measured this variable defines the maximum        */
   /* amount of tries to be used in reading and handling an AD channel in */
   /* 'Read_AD_Channel()'.                                                */ 

   AD_voltage_parameters.conversion_max_tries = 
      CONVERSION_VOLTAGE_MAX_TRIES;
   /* When voltages are measured this variable defines the maximum        */
   /* amount of tries to be used when End Of Conversion indication is     */
   /* waited in function 'Convert_AD()'.                                  */ 
                                 
   Read_AD_Channel(&AD_voltage_parameters);
   /* Voltage channel is read.                                            */
 
   if (AD_voltage_parameters.AD_execution_result != RESULT_OK) 
   {
      /* An anomaly has occurred during the measurement. */

      SetSoftwareError(MEASUREMENT_ERROR);         
      /* Set measurement error indication bit in */
      /* software error status register.         */

   }

   else
   {

      switch (channel_selector)
      {
         /* Measurement result bits 8..15 from channels involving positive      */
         /* voltages are written to telemetry.                                  */

         case channel_0_e:

            telemetry_data.sensor_unit_1.plus_5_voltage = 
               AD_voltage_parameters.unsigned_ADC >> 8; 
	   
            telemetry_data.sensor_unit_2.plus_5_voltage =	
               AD_voltage_parameters.unsigned_ADC >> 8;

            break;

         case channel_1_e:

            telemetry_data.sensor_unit_3.plus_5_voltage = 
               AD_voltage_parameters.unsigned_ADC >> 8; 
	   
            telemetry_data.sensor_unit_4.plus_5_voltage =	
               AD_voltage_parameters.unsigned_ADC >> 8;

            break;

         case channel_2_e:
	   
            telemetry_data.SU_plus_50 = AD_voltage_parameters.unsigned_ADC >> 8; 
         
            break;

         case channel_3_e:

            telemetry_data.DPU_plus_5_digital = 
               AD_voltage_parameters.unsigned_ADC >> 8;

            break;


        /* Measurement result bits 8..15 from channels involving negative      */
        /* voltages are written to telemetry.                                  */
        /* Note that even here, the "unsigned" or "raw" conversion result is   */
        /* used; this is a requirement.                                        */
 
         case channel_4_e:

            telemetry_data.sensor_unit_1.minus_5_voltage = 
               AD_voltage_parameters.unsigned_ADC >> 8; 
	   
            telemetry_data.sensor_unit_2.minus_5_voltage =	
               AD_voltage_parameters.unsigned_ADC >> 8;

            break;

         case channel_5_e:
	   
            telemetry_data.sensor_unit_3.minus_5_voltage = 
               AD_voltage_parameters.unsigned_ADC >> 8; 
	   
            telemetry_data.sensor_unit_4.minus_5_voltage =	
               AD_voltage_parameters.unsigned_ADC >> 8;

            break;

         case channel_6_e:

            telemetry_data.SU_minus_50 =
               AD_voltage_parameters.unsigned_ADC >> 8;
      
            break;
      }
   }   
}

void InitSystem(void)
/* Purpose        : Initialize system after RTX system is started.           */
/* Interface      : inputs      - none                                       */
/*                  outputs     - none                                       */
/*                  subroutines - SetTimeSlice()                             */
/*                                WaitInterval()                             */
/*                                CreateTask()                               */
/* Preconditions  : RTX is on.                                               */
/* Postconditions : System initialization duties are carried out i.e. rtx    */
/*                  tasks are activated, system clock interval is set.       */
/* Algorithm      :    - Set system clock interval                           */
/*                     - Wait for automatic A/D converter calibration.       */
/*                     - Activate the Telecommand Execution task.            */
/*                     - Activate the Acquisition task.                      */
/*                     - Activate Hit Trigger Interrupt Service task.        */
{
   task_info_t EXTERNAL new_task;
   /* Parameters for creating new task */

   SetTimeSlice(SYSTEM_INTERVAL);
   /* Set system clock interval */

   WaitInterval(BOOT_WAIT_INTERVAL);
   /* Wait for automatic A/D converter calibration */

   new_task.rtx_task_number    = TC_TM_INTERFACE_TASK;
   new_task.task_main_function = TC_task;
   CreateTask(&new_task);
   /* Activate the Telecommand Execution task */

   new_task.rtx_task_number    = ACQUISITION_TASK;
   new_task.task_main_function = acq_task;
   CreateTask(&new_task);
   /* Activate the Acquisition task */

   new_task.rtx_task_number    = HIT_TRIGGER_ISR_TASK;
   new_task.task_main_function = hit_task;
   CreateTask(&new_task);
   /* Activate Hit Trigger Interrupt Service task */
   
}	

void CalculateChecksum(uint_least8_t checksum_count) 
/* Purpose        : Calculates memory checksum.                              */
/* Interface      : inputs      - 'checksum_count' gives the current         */
/*                                 checksum loop cycle value,                */
/*                                 MIN_CHECKSUM_COUNT <= 'checksum_count'    */
/*                                                    <=  MAX_CHECKSUM_COUNT */
/*                                                                           */
/*                              - 'reference_checksum' variable to verify    */
/*                                correct codememory.                        */
/*                              - global variable 'code_not_patched'         */
/*                  outputs     - 'code_checksum' is modified.               */
/*                              - value of global variable                   */
/*                                'code_not_patched' is conditionally set.   */
/*                  subroutines - Reboot()                                   */
/* Preconditions  : none                                                     */
/* Postconditions : Changes values of variable 'code_checksum' and           */
/*                  on first round variable 'code_not_patched'.              */
/*                                                                           */
/*                  - In case of a checksum error,                           */
/*                    Soft reset is executed, if 'code_not_patched'          */
/*                    indication enables this.                               */
/*                                                                           */
/* Algorithm      : - Health monitoring calculates checksum with XOR for     */
/*                    CHECK_SIZE amount of codememory bytes at a time. At    */
/*                    the end of the check cycle, i.e. 'checksum_count' = 0, */
/*                    the variable 'code_checksum'                           */
/*                    should be equal with 'reference_checksum', if no       */
/*                    errors have occurred and code has not been patched     */
/*                    during the checksum cycle.                             */
/*                                                                           */
/*                  - If an error is detected in the code memory, Reboot()   */
/*                    function is called.                                    */
/*                                                                           */
/*                  - if no anomalies have been encountered there will be no */
/*                    changes and checksum loop starts from the beginning    */
/*                    i.e. on 'checksum_count' = 59, 'code_not_patched'      */
/*                    is set and 'code_checksum' is initialised.             */
{
   code_address_t EXTERNAL i;
   /* This variable is used in a for-loop.                                   */

   code_address_t EXTERNAL check_start;
   /* This variable is used for determining the start address of a given     */
   /* check.                                                                 */

   code_address_t EXTERNAL check_end;
   /* This variable is used for determining the end address of a given check.*/

   check_start = checksum_count * CHECK_SIZE;
 
   if (checksum_count == MAX_CHECKSUM_COUNT)

   {
      /* This piece ends at the top memory address. */
      check_end = CODE_MEMORY_END;
   }

   else  

   {
      check_end = check_start + (CHECK_SIZE - 1);
   }

   if (checksum_count == MAX_CHECKSUM_COUNT)
   {  
      
     code_checksum = 0;
     /* This global variable is used to store the result from each XOR     */
     /* calculation during a code memory checksum round. It is cleared     */
     /* here at the beginning of a new cycle and checked at the end of     */
     /* each cycle against 'reference_checksum'.                           */

     code_not_patched = 1;
     /* This global variable shows whether code is patched during a code   */
     /* memory checksum cycle. It is set here at the beginning of a new    */
     /* cycle and checked at the end of each cycle whether it has been     */
     /* cleared as an indication of an executed code patching.             */
   }

   for (i = check_start; i <= check_end; i++)

   {
     /* It is assumed that 'CODE_MEMORY_END'  < 2^16 - 1 */
     /* Otherwise variable 'i' can never have a value    */
     /* larger than 'check_end' and this loop will never */
     /* stop.                                            */             

      code_checksum ^= GET_CODE_BYTE(i); 
      /* XOR is counted for code memory byte under check. */
   }

   if (    (checksum_count == MIN_CHECKSUM_COUNT)
        && (code_checksum  != reference_checksum)   )
   {
      /* Checksum mismatch due to a memory error or */
      /* code memory patch.                         */
 
      if (code_not_patched)
      {
         /* An anomaly has been detected in the code memory  */
         /* area. Code has not been patched during this code */
         /* memory checksum cycle.                           */

         Reboot (checksum_reset_e);
         /* Soft reset is executed, as global variable       */
         /* 'code_not_patched' enables it. Note that         */
         /* Reboot() does not return here.                   */
      }
   }
   
}   

  
void UpdatePeriodCounter(
   uint_least8_t EXTERNAL * counter, 
   uint_least8_t full_counter_value) 
/* Purpose        : Advances counters                                        */
/* Interface      : inputs      - address to counter variable                */
/*                  outputs     - none                                       */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : counter value is adjusted                                */
/* Algorithm      : - If a given counter is not already zero, its value is   */
/*                    decreased.                                             */
/*                  - Else it set to its initialization value.               */
{
   if (*counter)

   {
      (*counter)--;
      /* Decrease temperature measurement counter. */
   }
      
   else
   {
      *counter = full_counter_value;
      /* Reset temperature measurement counter. */
   }
}
         


void Convert_AD (ADC_parameters_t EXTERNAL * ADC_parameters)
/* Purpose        : Conversion is executed on a selected AD channel          */
/* Interface      : inputs      - Address of a struct for storing the ADC    */
/*                                results.                                   */
/*                  outputs     - ADC results are stored to the given struct */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : ADC results are written to a struct                      */
/* Algorithm      : - AD Conversion is started on the selected channel.      */
/*                  - End of conversion is polled, EOC bit at I/O port 1,    */
/*                    as long as the preset limit is not exceeded.           */
/*                  - If limit has not been exeeded, resulting MSB and LSB of*/
/*                    the conversion are read from the HW registers and      */
/*                    combined into one word.                                */
/*                    Else end of conversion can no longer be waited. No     */
/*                    results are gained, instead a zero is stored with an   */
/*                    indication of this occurred anomaly.                   */
{
   unsigned char DIRECT_INTERNAL conversion_count;

   /* Counts the amount of end of conversion polls.                          */

   unsigned char DIRECT_INTERNAL  msb, lsb;                        
   /* MSB and LSB of the conversion result                                   */

   unsigned int DIRECT_INTERNAL word;
   /*This variable is used to combine MSB and LSB bytes into one word.       */
 
   START_CONVERSION;
 
   conversion_count = 0;
 
   while(conversion_count < ADC_parameters -> conversion_max_tries  
          && (END_OF_ADC != CONVERSION_ACTIVE))
   {
      /* Previous conversion is still active.                                */
      conversion_count++;
   }

   /* There is a slight chance for the following occurrence:                 */
   /* Conversion has ended after max_tries has been reached but before the   */
   /* following condition loop is entered. As a result measurement is failed */
   /* even if conversion has ended in time. The effect is dimished if the    */
   /* max_tries is a large value i.e. end of conversion has been waited long */
   /* enough.                                                                */

   if (conversion_count < ADC_parameters -> conversion_max_tries  )

   {
      /* Conversion has ended. Read AD result.                               */

      msb = GET_RESULT;
      lsb = GET_RESULT;

      word = (unsigned int)msb*256+lsb; 
      /* Combine MSB and LSB as type 'unsigned int' in the given struct.    */

      ADC_parameters -> signed_ADC =  (signed short int)(word^0x8000);
      /* Store result as of type 'signed int'.                            */

      ADC_parameters -> unsigned_ADC = word;
      /* Store result as of type 'unsigned int'.                             */

      ADC_parameters -> AD_execution_result = RESULT_OK;
      /* Store indication of an succesful measurement.                       */

   }

   else

   {
      
      /* Conversion has not ended in time. No results gained, store zero.    */
      ADC_parameters -> unsigned_ADC = 0;
      ADC_parameters -> signed_ADC = 0;

      /* Store indication of an unsuccesful measurement.                     */
      ADC_parameters -> AD_execution_result = CONVERSION_ACTIVE;

   }

}

void VoltageFailure(channel_t ADC_channel)
/* Purpose        : Takes care of resulting actions in case of a failed      */
/*                  measurement.                                             */
/* Interface      : inputs      - 'ADC_channel' related to the failed        */
/*                                measurement.                               */
/*                                                                           */
/*                  outputs     - telemetry_data.error_status register       */
/*                                telemetry_data.SU_status register          */
/*                                telemetry_data.mode_status register        */
/*                  subroutines - none                                       */
/* Preconditions  : There has been an anomaly                                */
/*                  during an ADC channel measurement.                       */
/* Postconditions : Sensor unit(s) related error indication bit(s) are set   */
/*                  in error_status, SU_status and mode_status registers.    */
/* Algorithm      :                                                          */
/*                  'ADC_channel' is the number of the channel which failed  */
/*                  measurement. The following actions are taken.            */
/*                     - Find out which sensor units are related to failed   */
/*                       measurement and store indications of these target   */
/*                       SUs in the 'SU_action[]' array.                     */
/*                     - If channel number indicates that the DPU voltage    */
/*                       measurement has failed, set high DPU +5V measurement*/
/*                       error indicating bit in the mode_status register.   */
/*                     - Set Error status bit of the related SU(s) in error  */
/*                       status register.                                    */
/*                     - If the channel number is for +- 50V, set high the   */
/*                       HV supply error indicating bit in the SU_Status     */
/*                       register for the related SU(s).                     */
/*                     - If the channel number if low voltage, set high the  */
/*                       LV supply error indicating bit in the SU_Status     */
/*                       register for the related SU(s).                     */
{
   unsigned char EXTERNAL i;
   /* This variable is used in a for-loop.                                   */

   unsigned char EXTERNAL SU_action[NUM_SU] = {LOW, LOW, LOW, LOW};
   /* This array stores actions targeted at a SU judging by the given        */
   /* ADC information.                                                       */


   /* Voltage measurement has failed, actions are taken accordingly.      */


   if (ADC_channel == 0x10 || ADC_channel == 0x54)

   {
      /* Measurement of channels related to sensor units 1 and 2 have     */
      /* failed.                                                          */
         
      SU_action[su1_e] = HIGH;
      SU_action[su2_e] = HIGH;

      /* Select sensor units 1 and 2 as targets in the 'SU_action' array. */
         
      SetModeStatusError(SUPPLY_ERROR);
      /* Set Supply Error. */


   }

   else if (ADC_channel == 0x11 || ADC_channel == 0x55)

   {
      /* Measurement of channels related to sensor units 3 and 4 have     */
      /* failed.                                                          */

      SU_action[su3_e] = HIGH;
      SU_action[su4_e] = HIGH;

      /* Select sensor units 3 and 4 as targets in the 'SU_action' array. */
   }

   else if (ADC_channel == 0x12 || ADC_channel == 0x56)

   {
      /* Measurement of channels related to all sensor units have         */
      /* failed.                                                          */

      SU_action[su1_e] = HIGH;
      SU_action[su2_e] = HIGH;
      SU_action[su3_e] = HIGH;
      SU_action[su4_e] = HIGH;

      /* Select all sensor units as targets in the 'SU_action' array.     */
   }
          
   else if (ADC_channel == 0x13)
      
   {
      /* Measurement of a channel related to DPU voltage has failed.      */

      SetModeStatusError(SUPPLY_ERROR);
      /* Set high DPU +5V supply error indicating bit in the         */
      /* mode status register.                                       */

   }

   for (i = 0; i < NUM_SU; i++)

   {
      /* Set error indications related to selected Sensor Units. */

      if (SU_action[i])

      {
         SetErrorStatus (ERROR_STATUS_OFFSET << i);
         /* Set Error status bit of the related SU in error status */
         /* register.                                              */

         if (ADC_channel == 0x12
          || ADC_channel == 0x56)
         {
            Set_SU_Error (i, HV_SUPPLY_ERROR);
            /* Set high HV supply error indicating bit in the */
            /*  SU_Status register.                           */
         }
        
         if (ADC_channel == 0x10
          || ADC_channel == 0x11
          || ADC_channel == 0x54
          || ADC_channel == 0x55)
         {
            Set_SU_Error (i, LV_SUPPLY_ERROR);
            /* Set high LV supply error indicating bit in the */
            /* SU_Status register.                            */
         }

      }
      
   }
}


void TemperatureFailure(sensor_index_t SU_index)
/* Purpose        : Takes care of resulting actions in case of a failed      */
/*                  measurement.                                             */
/* Interface      : inputs      - 'SU_index' which contains sensor unit      */
/*                                index number related to failed measurement */
/*                                or overheated SU.                          */
/*                  outputs     - telemetry_data.error_status register       */
/*                                telemetry_data.SU_status register          */
/*                  subroutines - SetSensorUnitOff()                         */
/* Preconditions  : There has been an anomaly                                */
/*                  during an ADC temperature channel measurement or         */
/*                  measurement has revealed that the given SU is            */
/*                  overheated.                                              */
/* Postconditions : Selected Sensor unit is switched off and error indication*/
/*                  bit is set in error_status register and SU_status        */
/*                  register.                                                */
/* Algorithm      : Following actions are taken,                             */
/*                     - switch off given sensor unit                        */
/*                     - Set Error status bit of the related SU in error     */
/*                       status register.                                    */
/*                     - Set high temperature error indicating bit in the    */
/*                       SU_Status register.                                 */
{

   unsigned char EXTERNAL exec_result;
   /* This variable is used by SetSensorUnitOff() function.        */

   /* Temperature measurement has failed, actions are taken accordingly. */

   SetSensorUnitOff(SU_index,&exec_result);
   /* Switch off given sensor unit. */    

   SetErrorStatus(ERROR_STATUS_OFFSET << SU_index);
   /* Set Error status bit of the related SU in error status register. */

   Set_SU_Error(SU_index, TEMPERATURE_ERROR);
   /* Set high temperature error indicating bit in the SU_Status */
   /* register.                                                  */
}


   




/*****************************************************************************/
/*                         AD conversion routines                            */
/*****************************************************************************/


void DelayAwhile (unsigned short duration)
/* Purpose        : Delay for a (brief) duration.                            */
/* Interface      : inputs      - delay duration, in ShortDelay() units.     */
/*                  outputs     - none.                                      */
/*                  subroutines - ShortDelay()                               */
/* Preconditions  : none.                                                    */
/* Postconditions : at least "duration" time units have passed.              */
/* Algorithm      : Call ShortDelay() as many times as necessary to delay    */
/*                  for at least the desired duration.                       */
{
   while (duration > MAX_SHORT_DELAY)
   {
      ShortDelay (MAX_SHORT_DELAY);
      duration = duration - MAX_SHORT_DELAY;
      /* Since ShortDelay() has a positive constant delay term, the  */
      /* actual total delay will be a little larger than 'duration'. */
   }

   if (duration > 0)
   {
      /* Some delay left after the loop above. */
      ShortDelay ((unsigned char) duration);
   }
}


void Read_AD_Channel (ADC_parameters_t EXTERNAL * ADC_parameters)
/* Purpose        : Reading an ADC channel                                   */
/* Interface      : inputs      - Address of a struct which contains         */
/*                                parameters for this function.              */
/*                  outputs     - Results are stored to the previously       */
/*                                mentioned structure.                       */
/*                  subroutines - Convert()                                  */
/* Preconditions  : Health monitoring is on.                                 */
/*                  ADC_parameters.ADC_max_tries > 0.                        */
/* Postconditions : AD channels are measured and results written to a given  */
/*                  structure.                                               */
/* Algorithm      :                                                          */
/*                  while                                                    */
/*                     - Hit trigger interrupt indicating flag is resetted.  */
/*                     - Handling of the given channel is started in a while */
/*                       loop.                                               */
/*                     - Channel to be converted is selected by setting bits */
/*                       0 - 6 from ADC Channel register to value of channel.*/
/*                       Channel value includes BP_UP bit to select          */
/*                       unipolar/bipolar mode.                              */
/*                     - Convert_AD() function executes the conversion and   */
/*                       stores the results in the given struct mentioned    */
/*                       earlier.                                            */
/*                     - If Hit trigger interrupt flag indicates that no     */
/*                       hit trigger interrupts have occurred during the     */
/*                       channel reading, exit the while loop.               */
/*                       Else continue loop from beginning, if               */
/*                       predefined limit for executing the loop has not been*/
/*                       reached.                                            */
/*                     - If all the conversion tries have been used up and   */
/*                       Hit trigger interrupt has corrupted the results,    */
/*                       only a zero is stored with an indication of this    */
/*                       occurred anomaly.                                   */
/*                  End of loop                                              */
{
   unsigned char EXTERNAL tries_left;
   /* Number of attempts remaining to try conversion without */
   /* interference from a particle hit.                      */

   unsigned short EXTERNAL delay_limit;
   /* Delay between channel selection and start of conversion in */
   /* ShortDelay() units.                                        */

   delay_limit = DELAY_LIMIT(2000);
   /* Set delay limit to 2ms. */

   tries_left = ADC_parameters -> ADC_max_tries;
   /* Limits the number of conversion attempts repeated because */
   /* of particle hit interrupts. Assumed to be at least 1.     */

   while (tries_left > 0)

   {
      confirm_hit_result = 0;
      /* Clear interrupt indicating flag.                                    */

      ADC_channel_register = (ADC_channel_register & 0x80) | 
                                 ADC_parameters -> ADC_channel; 
      UPDATE_ADC_CHANNEL_REG;
      /* AD Channel register is set. */
      
      START_CONVERSION;
      /* Initiate dummy cycle to set AD mode to unipolar or bipolar.         */

      DelayAwhile (delay_limit);
      /* Wait for analog signal and MUX to settle. */

      Convert_AD(ADC_parameters);
      /* Start conversion and measurement.                                   */

      tries_left--;
      /* Repeat while-loop until the max number of tries. */

      if (confirm_hit_result == 0)
      {
         /* Conversion has NOT been interrupted by a hit trigger interrupt.  */
         /* Exit from the while-loop.                                        */

         tries_left = 0;
      }
   }

   if (confirm_hit_result != 0)

   {
      /* Conversion has been interrupted by a hit trigger interrupt. Discard */
      /* corrupted results.                                                  */
              
      ADC_parameters -> unsigned_ADC = 0;
      ADC_parameters -> signed_ADC   = 0;

      ADC_parameters -> AD_execution_result = HIT_OCCURRED;
      /* Store indication of an unsuccessful measurement.                  */

   }   

   else if (ADC_parameters -> AD_execution_result == CONVERSION_ACTIVE 
               && confirm_hit_result == 0)
   {
      SetModeStatusError(ADC_ERROR);
      /* ADC error indication is set because a time-out has          */
      /* occurred during AD conversion and no hit trigger interrupt  */
      /* has occurred.                                               */
   }


   /* Either RESULT_OK or CONVERSION_ACTIVE indications are already */
   /* stored in the 'ADC_parameters -> AD_execution_result' field   */
   /* as a result from conversion in the Convert() function.        */  
}








   
void DAC_SelfTest(unsigned char DAC_output, 
                  ADC_parameters_t EXTERNAL * ADC_test_parameters) 
/* Purpose        : To test the analog signal chain in the circuitry.        */
/* Interface      : inputs      - Test data 'DAC_Output                      */
/*                              - struct 'ADC_test_parameters'               */
/*                  outputs     - none                                       */
/*                  subroutines - Read_AD_Channel()                          */
/* Preconditions  : none                                                     */
/* Postconditions : DAC selftest is carried out and results written to a     */
/*                  given struct.                                            */
/* Algorithm      :                                                          */
/*                  - Test data is written to HW registers.                  */
/*                  - Test channel is AD converted.                          */
/*                  - Results are stored in a given structure.               */
/*                  - If the measurement has failed,                         */
/*                                                                           */
/*                     - Set self test error indication bit in mode status   */
/*                       register                                            */
/*                     - Set measurement error indication bit in mode status */
/*                       register.                                           */
{

   SET_DAC_OUTPUT(DAC_output);
   /*test data is written to DAC output                                     */
   
   Read_AD_Channel(ADC_test_parameters);
   /* Start reading the channel.                                             */

	if (ADC_test_parameters -> AD_execution_result != RESULT_OK) 

      {
         /* An anomaly has occurred during the measurement.                  */

         Set_SU_Error(ADC_test_parameters -> sensor_unit - SU1, 
                             SELF_TEST_ERROR);                    
         /* Self self test error indication bit in mode status register.    */
         /* Offset in numbering is taken into account by transforming       */
         /* 'sensor_unit' (value 1-4) to a sensor unit index (value 0-3)    */

         SetSoftwareError(MEASUREMENT_ERROR);         
         /* Set measurement error indication bit in */
         /* software error status register.         */   

      }	
}


void Monitor_DPU_Voltage(void)
/* Purpose        : Monitors DPU voltages                                    */
/* Interface      : inputs      - telemetry_data, DPU voltages               */
/*                  outputs     - telemetry_data.mode_status                 */
/*                                telemetry_data, supply voltages            */
/*                  subroutines -  SetModeStatusError                        */
/*                                 ExceedsLimit                              */
/*                                 MeasureVoltage                            */
/* Preconditions  : none                                                     */
/* Postconditions : DPU voltages are measured and monitored.                 */
/* Algorithm      :                                                          */
/*                  - Channels are checked one by one and in case of an error*/
/*                    corresponding error bit is set.                        */
{
   MeasureVoltage(DPU_5V_SELECTOR);
   MeasureVoltage(SU_1_2_P5V_SELECTOR);
   MeasureVoltage(SU_1_2_M5V_SELECTOR);

   /* Result of voltage measurement from SU_1/2 +5V is compared against */
   /* limits.          							*/

   if (ExceedsLimit(telemetry_data.sensor_unit_1.plus_5_voltage,
         SU_P5V_ANA_LOWER_LIMIT,
         SU_P5V_ANA_UPPER_LIMIT))
   {
         SetModeStatusError(SUPPLY_ERROR);   
   }

   /* Result of voltage measurement from SU_1/2 -5V is compared against */
   /* limits.          							*/

   if (ExceedsLimit(telemetry_data.sensor_unit_1.minus_5_voltage,
         SU_M5V_ANA_LOWER_LIMIT,
         SU_M5V_ANA_UPPER_LIMIT))
   {
         SetModeStatusError(SUPPLY_ERROR);   
   }

   /* Result of voltage measurement from DIG +5V is compared against    */
   /* limits.          							*/

   if (ExceedsLimit(telemetry_data.DPU_plus_5_digital,
         DPU_P5V_DIG_LOWER_LIMIT,
         DPU_P5V_DIG_UPPER_LIMIT))
   {
         SetModeStatusError(SUPPLY_ERROR);   
   }

}


void Monitor_SU_Voltage(sensor_index_t self_test_SU_index)
/* Purpose        : Monitors SU voltages                                     */
/* Interface      : inputs      - self_test_SU_index                         */
/*                                telemetry_data, sensor voltages            */
/*                  outputs     - telemetry_data.SU_error                    */
/*                  subroutines -  ExceedsLimit                              */
/*                                 Set_SU_Error                              */
/* Preconditions  : SU voltages are measured                                 */
/* Postconditions : SU voltages are monitored.                               */
/* Algorithm      :                                                          */
/*                  - Channels are checked one by one and in case of an error*/
/*                    corresponding error bit is set.                        */
{
   switch (self_test_SU_index)
   {
      case su1_e:
      case su2_e:

         /* Result of voltage measurement from SU_1/2 +5V is compared against */
         /* limits.          						      */

         if (ExceedsLimit(telemetry_data.sensor_unit_1.plus_5_voltage,
               SU_P5V_ANA_LOWER_LIMIT,
               SU_P5V_ANA_UPPER_LIMIT))
         {
            Set_SU_Error(self_test_SU_index, LV_LIMIT_ERROR);   
         }

         /* Result of voltage measurement from SU_1/2 -5V is compared against */
         /* limits.          						      */

         if (ExceedsLimit(telemetry_data.sensor_unit_1.minus_5_voltage,
               SU_M5V_ANA_LOWER_LIMIT,
               SU_M5V_ANA_UPPER_LIMIT))
         {
            Set_SU_Error(self_test_SU_index, LV_LIMIT_ERROR);   
         }
        
         break;


      case su3_e:
      case su4_e:

         /* Result of voltage measurement from SU_3/4 +5V is compared against */
         /* limits.          						      */

         if (ExceedsLimit(telemetry_data.sensor_unit_3.plus_5_voltage,
               SU_P5V_ANA_LOWER_LIMIT,
               SU_P5V_ANA_UPPER_LIMIT))
         {
            Set_SU_Error(self_test_SU_index, LV_LIMIT_ERROR);   
         }

         /* Result of voltage measurement from SU_3/4 -5V is compared against */
         /* limits.          						      */

         if (ExceedsLimit(telemetry_data.sensor_unit_3.minus_5_voltage,
               SU_M5V_ANA_LOWER_LIMIT,
               SU_M5V_ANA_UPPER_LIMIT))
         {
            Set_SU_Error(self_test_SU_index, LV_LIMIT_ERROR);   
         }

         break;
   }

   /* Result of voltage measurement from SU +50V is compared against */
   /* limits.          						     */

   if (ExceedsLimit(telemetry_data.SU_plus_50,
         SU_P50V_LOWER_LIMIT,
         SU_P50V_UPPER_LIMIT))
   {
      Set_SU_Error(self_test_SU_index, HV_LIMIT_ERROR);
   }

   /* Result of voltage measurement from SU -50V is compared against */
   /* limits.          						     */

   if (ExceedsLimit(telemetry_data.SU_minus_50,
         SU_M50V_LOWER_LIMIT,
         SU_M50V_UPPER_LIMIT))
   {
      Set_SU_Error(self_test_SU_index, HV_LIMIT_ERROR);
   }
}

void SelfTest_SU(sensor_index_t self_test_SU_index)
/* Purpose        : Execute SU self tests                                    */
/* Interface      : inputs      - self_test_SU_index                         */
/*                  outputs     - none                                       */
/*                  subroutines -  LowVoltageCurrent()                       */
/*                                 MeasureVoltage()                          */
/*                                 MeasureTemperature()                      */
/*                                 HighVoltageCurrent()                      */
/* Preconditions  : none                                                     */
/* Postconditions : Part of the Self Test sequence regarding temperatures,   */
/*                  voltages and overcurrents is completed.                  */
/* Algorithm      : - V_DOWN is checked                                      */
/*                  - Voltage channels are checked one by one and in case of */
/*                    an error corresponding error bit is set.               */
/*                  - SU Temperatures and HV Status Register is checked.     */
{
   uint_least8_t EXTERNAL i;
   /* Used in a for loop */

   LowVoltageCurrent();
   /* V_DOWN is checked. */

   HighVoltageCurrent(self_test_SU_index);
   /* HV Status register is checked. */

   /* SU voltages are measured */
   for (i = channel_0_e; i <= channel_6_e; i++)
   {
      MeasureVoltage(i);
      /* All voltage channels are measured. */
   }

   Monitor_SU_Voltage(self_test_SU_index);
   /* Voltage measurement results are monitored against limits. */

   MeasureTemperature(self_test_SU_index);
   /* SU temperatures are measured and monitored. */
}

unsigned char ExceedsLimit(
   unsigned char value,
   unsigned char lower_limit,
   unsigned char upper_limit)
/* Purpose        : Tests given value against given limits.                  */
/* Interface      : inputs      - value                                      */            
/*                                lower_limit                                */
/*                                upper_limit                                */ 
/*                  outputs     - boolean value                              */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : Given value is tested.                                   */
/* Algorithm      : See below, self explanatory.                             */
{
   return (value < lower_limit || value > upper_limit);
}  


void SelfTestChannel(sensor_index_t self_test_SU_index)
/* Purpose        : Starts channel tests                                     */
/* Interface      : inputs      - self_test_SU_index                         */
/*                  outputs     - none                                       */
/*                  subroutines -  SetTestPulseLevel                         */
/*                                 SetTriggerLevel                           */
/*                                 RestoreSettings                           */
/*                                 ExecuteChannelTest                        */
/*                                 DisableAnalogSwitch                       */
/* Preconditions  : none                                                     */
/* Postconditions : SU channels are self tested.                             */
/*                  voltages and overcurrents is completed.                  */
/* Algorithm      : - Threshold level is set high.                           */
/*                  - Test pulse level for a given channel is set high.      */
/*                  - Channels are tested.                                   */
/*                  - Threshold level is set low.                            */
/*                  - Test pulse level for a given channel is set low .      */
/*                  - Channels are tested.                                   */
/*                  - A pseudo trigger is generated in order to reset peak   */
/*                    detector and delay counter in AcquisitionTask.         */
/*                  - Threshold levels are restored to the level prior to    */
/*                    the test. SU state for the SU under test is restored   */
/*                    to ON.                                                 */
{
   trigger_set_t EXTERNAL test_threshold;

   DISABLE_HIT_TRIGGER;

   /* Initial parameters for SetTriggerLevel function. */
   test_threshold.sensor_unit = self_test_SU_number;
   test_threshold.level       = MAX_PLASMA_SELF_TEST_THRESHOLD;
   test_threshold.channel     = PLASMA_1_PLUS;
   SetTriggerLevel(&test_threshold);
   test_threshold.channel     = PLASMA_1_MINUS;
   SetTriggerLevel(&test_threshold);
   test_threshold.channel     = PLASMA_2_PLUS;
   SetTriggerLevel(&test_threshold);

   test_threshold.level       = MAX_PIEZO_SELF_TEST_THRESHOLD;
   test_threshold.channel     = PZT_1_2;
   SetTriggerLevel(&test_threshold);


   /* Set initial test pulse value to 0. Test pulse value is also zeroed    */
   /* before returning from ExecuteChannelTest procedure also.              */
   SetTestPulseLevel(0);

   /* Test threshold level is set before each channel test for every channel*/
   /* and value is set back to the maximum threshold level before returning */
   /* from the following ExecuteChannelTest procedure calls.                */

   test_threshold.channel     = PLASMA_1_PLUS;
   test_threshold.level       = HIGH_PLASMA_1_PLUS_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PLASMA_1_PLUS, PLASMA_1_PLUS_HIGH);

   test_threshold.channel     = PLASMA_1_MINUS;
   test_threshold.level       = HIGH_PLASMA_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PLASMA_1_MINUS, PLASMA_1_MINUS_HIGH);

   ExecuteChannelTest(self_test_SU_index, PLASMA_2_PLUS, PLASMA_2_PLUS_HIGH);

   test_threshold.channel     = PZT_1_2;
   test_threshold.level       = HIGH_PIEZO_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PZT_1, PZT_1_HIGH);

   test_threshold.channel     = PZT_1_2;
   test_threshold.level       = HIGH_PIEZO_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PZT_2, PZT_2_HIGH);

   test_threshold.channel     = PLASMA_1_PLUS;
   test_threshold.level       = LOW_PLASMA_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PLASMA_1_PLUS, PLASMA_1_PLUS_LOW);

   test_threshold.channel     = PLASMA_1_MINUS;
   test_threshold.level       = LOW_PLASMA_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PLASMA_1_MINUS, PLASMA_1_MINUS_LOW);

   ExecuteChannelTest(self_test_SU_index, PLASMA_2_PLUS, PLASMA_2_PLUS_LOW); 

   test_threshold.channel     = PZT_1_2;
   test_threshold.level       = LOW_PIEZO_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PZT_1, PZT_1_LOW);

   test_threshold.channel     = PZT_1_2;
   test_threshold.level       = LOW_PIEZO_SELF_TEST_THRESHOLD;
   SetTriggerLevel(&test_threshold);
   ExecuteChannelTest(self_test_SU_index, PZT_2, PZT_2_LOW);

   ENABLE_HIT_TRIGGER;

   SET_HIT_TRIGGER_ISR_FLAG;
   /* A pseudo trigger is generated in order to reset peak   */
   /* detector and delay counter in AcquisitionTask.         */  
   /* No event is recorded i.e. event processing is disabled */
   /* because SU state is 'self_test_e'.                     */

   RestoreSettings(self_test_SU_index);
}


void ExecuteChannelTest(
   sensor_index_t self_test_SU_index,
   unsigned char test_channel,
   unsigned char test_pulse_level)
/* Purpose        : Execute SU self tests                                    */
/* Interface      : inputs      - self_test_SU_index, test_channel           */
/*                  outputs     - telemetry_data.SU# Status                  */
/*                  subroutines -  SelectSelfTestChannel                     */
/*                                 SelectStartSwitchLevel                    */
/*                                 WaitTimeout                               */
/*                                 DelayAwhile                               */
/*                                 SelectTriggerSwitchLevel                  */
/* Preconditions  : none                                                     */
/* Postconditions : Self test trigger siggnal is generated.                  */
/*                  voltages and overcurrents is completed.                  */
/* Algorithm      : - Self test channel is selected.                         */
/*                  - Analog switch starting level is selected depending on  */
/*                    the channel.                                           */
/*                  - A pseudo trigger is generated in order to reset peak   */
/*                    detector and delay counter in AcquisitionTask.         */
/*                  - Hit trigger processing is waited for 40 ms.            */
/*                  - Hit trigger processing for this self test pulse is     */
/*                    enabled by setting SU state to 'self_test_trigger_e'   */
/*                  - Analog switch triggering level is selected depending   */
/*                    on the channel (rising or falliing edge).              */
/*                  - If self test trigger pulse did not cause an interrupt, */
/*                    set SELF_TEST_ERROR indication in SU status register   */
/*                    and restore SU state to 'self_test_e'.                 */
{
   unsigned short EXTERNAL delay_limit;
   trigger_set_t  EXTERNAL test_threshold;

   if (test_channel == PLASMA_1_PLUS || 
       test_channel == PLASMA_1_MINUS || 
       test_channel == PLASMA_2_PLUS)
   {
      SelectSelfTestChannel(test_channel);

      EnableAnalogSwitch(self_test_SU_index);

      WaitTimeout(1);

      ResetPeakDetector(self_test_SU_index + SU1);

      WaitTimeout(1);

      ResetPeakDetector(self_test_SU_index + SU1);

      WaitTimeout(1);

      CLEAR_HIT_TRIGGER_ISR_FLAG;

      ResetDelayCounters();

      SU_state[self_test_SU_index] = self_test_trigger_e;
      /* Enable hit trigger processing for this self test pulse. */

      ENABLE_HIT_TRIGGER;

      SetTestPulseLevel(test_pulse_level);

      /* Set at least 1ms test pulse    */
      delay_limit = DELAY_LIMIT(1000);
      DelayAwhile(delay_limit);

      /* For plasma 1i channel triggering must take place in 1ms after */
      /* rising edge.                                                  */
      if (test_channel                 == PLASMA_1_MINUS &&
          SU_state[self_test_SU_index] == self_test_trigger_e)
      {
         /* Self test trigger pulse did not cause an interrupt. */    
         Set_SU_Error(self_test_SU_index, SELF_TEST_ERROR);
    
         SU_state[self_test_SU_index] = self_test_e;
         /* Triggering of a self test pulse is disabled by restoring */
         /* the self_test_e state.                                   */
      }

      /* Test pulse is always at least 3ms (=1ms+2ms) */
      delay_limit = DELAY_LIMIT(2000);
      DelayAwhile(delay_limit);

      SetTestPulseLevel(0);

      if (test_channel == PLASMA_2_PLUS)
      {
	 SET_HIT_TRIGGER_ISR_FLAG;
      }

      /* If channel is plasma 1e or 2e then wait at least 1ms after */
      /* falling edge.                                              */
      if (test_channel != PLASMA_1_MINUS)
      {
         delay_limit = DELAY_LIMIT(1000);
         /* Set at least 1ms test pulse    */
         DelayAwhile(delay_limit);
      }

      DISABLE_HIT_TRIGGER;

      if (test_channel != PLASMA_2_PLUS)
      {
         test_threshold.sensor_unit = self_test_SU_index + SU1;
         test_threshold.channel     = test_channel;
         test_threshold.level       = MAX_PLASMA_SELF_TEST_THRESHOLD;
         SetTriggerLevel(&test_threshold);
      }

      DisableAnalogSwitch(self_test_SU_index);
   }
   else
   {
      SelectSelfTestChannel(test_channel);

      SetTestPulseLevel(test_pulse_level);

      WaitTimeout(1);
      
      ResetPeakDetector(self_test_SU_index + SU1);

      WaitTimeout(1);

      ResetPeakDetector(self_test_SU_index + SU1);

      WaitTimeout(1);

      CLEAR_HIT_TRIGGER_ISR_FLAG;

      ResetDelayCounters();

      SU_state[self_test_SU_index] = self_test_trigger_e;
      /* Enable hit trigger processing for this self test pulse. */

      ENABLE_HIT_TRIGGER;

      EnableAnalogSwitch(self_test_SU_index);

      /* Set at least 1ms test pulse    */
      delay_limit = DELAY_LIMIT(1000);
      DelayAwhile(delay_limit);

      DISABLE_HIT_TRIGGER;

      SetTestPulseLevel(0);

      DisableAnalogSwitch(self_test_SU_index);

      test_threshold.sensor_unit = self_test_SU_index + SU1;
      test_threshold.level       = MAX_PIEZO_SELF_TEST_THRESHOLD;
      test_threshold.channel     = PZT_1_2;
      SetTriggerLevel(&test_threshold);
   }

   if (SU_state[self_test_SU_index] == self_test_trigger_e)
   {
      /* Self test trigger pulse did not cause an interrupt. */    
      Set_SU_Error(self_test_SU_index, SELF_TEST_ERROR);
    
      SU_state[self_test_SU_index] = self_test_e;
      /* Triggering of a self test pulse is disabled by restoring */
      /* the self_test_e state.                                   */
   }
}

void RestoreSettings(sensor_index_t self_test_SU_index)
/* Purpose        : Restores settings after SU self tests.                   */
/* Interface      : inputs      - self_test_SU_index,                        */
/*                                telemetry_data, SU threshold levels        */
/*                  outputs     - HW registers, thresholds                   */
/*                                SU state                                   */
/*                  subroutines -  SetTriggerLevel                           */
/*                                 Switch_SU_State                           */
/* Preconditions  : none                                                     */
/* Postconditions : - Threshold levels are restored to the level prior to    */
/*                    the test. SU state for the SU under test is restored   */
/*                    to ON.                                                 */
/* Algorithm      : - Original threshold levels are copied from              */
/*                    telemetry_data and written in to HW registers with     */
/*                    SetTriggerLevel.                                       */
/*                  - SU state is restored to ON with Switch_SU_State.       */
{
   SU_settings_t EXTERNAL *const PROGRAM setting_map_c[] = {
      &telemetry_data.sensor_unit_1,
      &telemetry_data.sensor_unit_2,
      &telemetry_data.sensor_unit_3,
      &telemetry_data.sensor_unit_4
      };
      /* Pointers to Sensor Unit configuration data in telemetry */
      /* data area.                                              */
 
   SU_settings_t EXTERNAL * INDIRECT_INTERNAL SU_setting;
   /* Pointer to configuration data of the Sensor Unit being */
   /* Self Tested.                                           */

   sensor_unit_t EXTERNAL SU_switch;
   trigger_set_t EXTERNAL threshold;
   /* Parameters for subroutines. */
 
   SU_setting = setting_map_c[self_test_SU_index];

   threshold.sensor_unit = self_test_SU_number; 

   threshold.level   = 
      SU_setting -> plasma_1_plus_threshold;
   threshold.channel = PLASMA_1_PLUS;
      SetTriggerLevel(&threshold);
   /* Restore Plasma 1 Plus trigger threshold. */
 
   threshold.level   = 
      SU_setting -> plasma_1_minus_threshold;
   threshold.channel = PLASMA_1_MINUS;
   SetTriggerLevel(&threshold);
   /* Restore Plasma 1 Minus trigger threshold. */
 
   threshold.level   = 
   SU_setting -> piezo_threshold;
   threshold.channel = PZT_1_2;
   SetTriggerLevel(&threshold);
   /* Restore Piezo trigger threshold. */
 
   SU_switch.SU_number             = self_test_SU_number;
   SU_switch.SU_state              = on_e;
   SU_switch.expected_source_state = self_test_e;
   Switch_SU_State(&SU_switch);
   /* Switch SU State back to On. */
                       
}


