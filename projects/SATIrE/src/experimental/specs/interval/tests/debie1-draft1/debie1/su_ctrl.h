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
 *    Subsystem  :   DNI (DEBIE Null Interface)
 *    Module     :   su_ctrl.h
 *
 * Macros, functions, constants and types for controlling Sensor Units.
 *
 * Based on the SSF DHI file su_ctrl.h, rev 1.28, Tue Sep 14 14:51:26 1999.
 *
 *- * --------------------------------------------------------------------------
 */


#ifndef SU_CTRL_H
#define SU_CTRL_H

/* Sensor Channels */

#define NUM_CH 5
/* Number of recorded measurement channels per sensor unit. */
/* The PZT_1_2 channel is not recorded, as such.            */

#define  PLASMA_1_PLUS  0
#define  PLASMA_1_MINUS 1
#define  PZT_1          2
#define  PZT_2          3
#define  PLASMA_2_PLUS  4
#define  PZT_1_2        5


#define SU_1 1 
#define SU_2 2
#define SU_3 3
#define SU_4 4

#define SU_1_ON 1 
#define SU_2_ON 2
#define SU_3_ON 3
#define SU_4_ON 4

#define SU_1_OFF 1 
#define SU_2_OFF 2
#define SU_3_OFF 3
#define SU_4_OFF 4

#define LOW_PLASMA_SELF_TEST_THRESHOLD         0x15
#define LOW_PIEZO_SELF_TEST_THRESHOLD          0x0D
#define HIGH_PLASMA_1_PLUS_SELF_TEST_THRESHOLD 0xAB
#define HIGH_PLASMA_SELF_TEST_THRESHOLD        0x80
#define HIGH_PIEZO_SELF_TEST_THRESHOLD         0x2B
#define MAX_PLASMA_SELF_TEST_THRESHOLD         0xFF
#define MAX_PIEZO_SELF_TEST_THRESHOLD          0xFF
/* Self test threshold levels. */

#define PLASMA_1_PLUS_LOW          0x13
#define PLASMA_1_MINUS_LOW         0x08
#define PLASMA_2_PLUS_LOW          0x10
#define PZT_1_LOW                  0x36
#define PZT_2_LOW                  0x36
/* Low level test pulses. */

#define PLASMA_1_PLUS_HIGH         0x5A
#define PLASMA_1_MINUS_HIGH        0x2A
#define PLASMA_2_PLUS_HIGH         0x50
#define PZT_1_HIGH                 0xF6
#define PZT_2_HIGH                 0xE8
/* High level test pulses. */

#define SU_NOT_ACTIVATED 0
#define SU_NOT_DEACTIVATED 0

#define CHANNEL_NOT_SELECTED 5
#define SU_NOT_SELECTED 6

#define TRIGGER_SET_OK 1 

#define DEFAULT_THRESHOLD 0x0D
/* Default Trigger threshold is mid-scale value. */

#define DEFAULT_TEST_PULSE_LEVEL 0x00

#define DEFAULT_CLASSIFICATION_LEVEL 0
#define DEFAULT_MAX_TIME             255
#define DEFAULT_MIN_TIME             0
/* These default levels are only temporary */

#define SU_ONOFF_MASK 3
/* Bit mask for SU Status register manipulation when SU is */
/* switched ON or OFF.                                     */

#define SU_STATE_TRANSITION_OK     1
#define SU_STATE_TRANSITION_FAILED 0

#define NO_SU 0

/* Trigger level register base addresses */

#define  SU_1_TRIGGER_BASE    0xFFB0
#define  SU_2_TRIGGER_BASE    0xFFB3
#define  SU_3_TRIGGER_BASE    0xFFC0
#define  SU_4_TRIGGER_BASE    0xFFC3

#define  DELAY_1_2_MSB 0xFF40
#define  DELAY_1_LSB 0xFF30
#define  DELAY_2_LSB 0xFF50

#define SU_1_THRESHOLD1 0xFFB0
#define SU_1_THRESHOLD2 0xFFB1
#define SU_1_THRESHOLD3 0xFFB2

#define SU_2_THRESHOLD1 0xFFB3
#define SU_2_THRESHOLD2 0xFFB4
#define SU_2_THRESHOLD3 0xFFB5

#define SU_3_THRESHOLD1 0xFFC0
#define SU_3_THRESHOLD2 0xFFC1
#define SU_3_THRESHOLD3 0xFFC2

#define SU_4_THRESHOLD1 0xFFC3
#define SU_4_THRESHOLD2 0xFFC4
#define SU_4_THRESHOLD3 0xFFC5

/* Some other register addresses */

#define  TEST_PULSE_LEVEL              0xFFB6
#define  SU_SELF_TEST_CH               0xFFE0

/* Simulation functions */

extern unsigned char Get_MSB_Counter   (void);
extern unsigned char Get_LSB1_Counter  (void);
extern unsigned char Get_LSB2_Counter  (void);
extern unsigned char Rise_Time_Counter (void);

/* Counter access macros */

#define GET_MSB_COUNTER   Get_MSB_Counter ()
#define GET_LSB1_COUNTER  Get_LSB1_Counter ()
#define GET_LSB2_COUNTER  Get_LSB2_Counter ()
#define RISE_TIME_COUNTER Rise_Time_Counter ()

#define SU_CONTROL 0xFFD0


/* Macros */


#define SU_1_MINUS_50   1
#define SU_1_PLUS_50    2
#define SU_2_MINUS_50   4
#define SU_2_PLUS_50    8
#define SU_3_MINUS_50   16
#define SU_3_PLUS_50    32
#define SU_4_MINUS_50   64
#define SU_4_PLUS_50    128

#define  TRIGGER_SOURCE T0 + 2 * T1

#define  HV_STATUS       0xFF70
#define SET_COUNTER_RESET(LEVEL) {}

/* Simulation of trigger signal */

extern unsigned char Trigger_Source_0 (void);
extern unsigned char Trigger_Source_1 (void);
extern unsigned char V_Down           (void);

/* Macros to access trigger signals */

#define TRIGGER_SOURCE_0 Trigger_Source_0 ()
#define TRIGGER_SOURCE_1 Trigger_Source_1 ()
#define V_DOWN           V_Down ()

/* Type definitions */

typedef uint_least8_t sensor_number_t;
/* Sensor Unit number. Valid values SU_1, SU_2, SU_3 and SU_4 */
/* which must be successive integers.                         */
/* As a special case for some variables, the value NO_SU      */
/* indicates "no Sensor Unit". This possibility is always     */
/* shown by a comment on that variable, otherwise only real   */
/* SU numbers are allowed.                                    */

#define NUM_SU 4
/* The number of sensor units. */

typedef uint_least8_t sensor_index_t;
/* Sensor Unit index. Valid values 0 .. NUM_SU - 1.  */
/* Index 0 corresponds to Sensor Unit number SU_1. */

typedef uint_least8_t channel_t;
/* The number of an A/D channel. */

typedef struct {
   unsigned short int FromPlasma1Plus;
   unsigned short int FromPlasma1Minus;
}delays_t ;

typedef struct {
   sensor_number_t sensor_unit;
   channel_t       channel;
   unsigned char   level;
   unsigned char   execution_result;
   unsigned int    base;
}trigger_set_t ;

typedef struct {
   unsigned char V_down_bit;
   unsigned char HV_status;
}voltage_status_t ;


extern unsigned char EXTERNAL SU_ctrl_register;
/* This variable stores values of write-only registers */

/* Function prototypes */

/* Sensor Unit status */


/* Delay and rise time counters */
extern void ReadDelayCounters (delays_t EXTERNAL *delay);
extern unsigned char ReadRiseTimeCounter(void) COMPACT REENTRANT_FUNC;
extern void ResetDelayCounters(void) COMPACT REENTRANT_FUNC;
extern void ResetPeakDetector(sensor_number_t unit);
extern void SignalPeakDetectorReset(
   unsigned char low_reset_value,
   unsigned char high_reset_value);


/* Trigger levels */
extern void SetTriggerLevel(trigger_set_t EXTERNAL *setting)
   COMPACT REENTRANT_FUNC;

/* Test pulse level */
extern void SetTestPulseLevel(unsigned char level)
   COMPACT REENTRANT_FUNC;

extern void GetVoltageStatus(voltage_status_t EXTERNAL *v_status) 
   COMPACT REENTRANT_FUNC;


/* Sensor Unit power control */
extern void Switch_SU_On  (
   sensor_number_t SU_Number,
   unsigned char EXTERNAL *execution_result)
   COMPACT REENTRANT_FUNC;

extern void Switch_SU_Off (
   sensor_number_t SU_Number,
   unsigned char EXTERNAL *execution_result)
   COMPACT REENTRANT_FUNC;               

/* Sensor Unit calibration */

extern void EnableAnalogSwitch(sensor_index_t self_test_SU_index);
extern void DisableAnalogSwitch(sensor_index_t self_test_SU_index);
extern void SelectSelfTestChannel(unsigned char channel);
extern void SelectTriggerSwitchLevel(
           unsigned char  test_channel,
           sensor_index_t self_test_SU_index);
extern void SelectStartSwitchLevel(
           unsigned char  test_channel,
           sensor_index_t self_test_SU_index);


#endif


