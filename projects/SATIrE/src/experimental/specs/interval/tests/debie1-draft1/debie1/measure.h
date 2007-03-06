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
 *    Module     :   measure.h
 *
 * Prototypes etc. for the Measure module.
 *
 * Based on the SSF file measure.h, rev 1.14, Mon Aug 16 17:29:40 1999 .
 *      
 *- * --------------------------------------------------------------------------
 */
   

#ifndef MEASURE_H
#define MEASURE_H


#include "keyword.h"
#include "kernobj.h"
#include "su_ctrl.h"

/*Maximum number of conversion start tries allowed in the HitTriggerTask*/ 
#define ADC_MAX_TRIES 25

#define HIT_BUDGET_DEFAULT 20
/* Default limit for the events handled during one Health Monitoring */
/* period. Valid values 1 .. 255.                                    */

#define PEAK_RESET_MIN_DELAY 1
/* Peak detector reset min delay: 1 * 10ms. */

#define COUNTER_RESET_MIN_DELAY 1
/* Delay counter reset min delay: 1 * 10 ms.   */
/* NOTE that specifications would allow delay  */
/* of 1ms, but minimum delay that is possible  */
/* to be generated with RTX is one tick = 10ms */         

#define SELF_TEST_DELAY 4
/* This delay equals the length of 4 system cycles. */

/*Sensor Unit numbers*/
#define SU1 1
#define SU2 2
#define SU3 3
#define SU4 4

/*type definitions*/
typedef enum {
   off_e,               /* SU off state - power is Off.                  */
   start_switching_e,   /* Transition to On state is starting.           */
   switching_e,         /* Transition to On state is started.            */
   on_e,                /* SU on state - power is On.                    */
   self_test_mon_e,     /* Selt Test, Voltage and Temperature monitoring */
   self_test_e,         /* Selt Test, test pulse setup.                  */
   self_test_trigger_e, /* Self test, test pulse handling                */
   acquisition_e        /* Power is On and Hit Events are accepted.      */

} SU_state_t;
   /* From these only off_e, on_e and self_test_e are actual SU states   */
   /* defined in the User Requirements. Those and 'acquisition_e' are    */
   /* the main states between which the SU state transitions are made.   */

typedef struct {
   sensor_number_t SU_number;        /* Sensor Unit number                   */
   SU_state_t SU_state;              /* Sensor unit states can be either On  */
                                     /* or Off.                              */
   SU_state_t expected_source_state; /* Excpected source state of the SU     */
                                     /* state transition.                    */
   unsigned char execution_result;   /* This variable is used to indicate    */
                                     /* execution results.                   */

} sensor_unit_t;

extern SU_state_t EXTERNAL SU_state[4];

extern sensor_number_t EXTERNAL self_test_SU_number;

typedef enum {high_e, low_e} SU_test_level_t;

extern uint_least8_t EXTERNAL hit_budget;
extern uint_least8_t EXTERNAL hit_budget_left;


/*function prototypes*/
extern void Switch_SU_State(sensor_unit_t EXTERNAL *SU_setting) 
               COMPACT_DATA REENTRANT_FUNC;

extern void Start_SU_SwitchingOn(
               sensor_index_t SU,
               unsigned char EXTERNAL *exec_result) 
               COMPACT_DATA REENTRANT_FUNC;

extern void SetSensorUnitOff(
               sensor_index_t SU,
               unsigned char EXTERNAL *exec_result) 
               COMPACT_DATA REENTRANT_FUNC;

extern SU_state_t ReadSensorUnit(unsigned char SU_number) 
                     COMPACT_DATA REENTRANT_FUNC;

extern void Update_SU_State(sensor_index_t SU_index) 
               COMPACT_DATA REENTRANT_FUNC;

/*pointers to tasks*/

extern void (* EXTERNAL hit_task)(void);
extern void (* EXTERNAL acq_task)(void);

/* Task functions, for testing: */

extern void InitHitTriggerTask (void);
extern void HandleHitTrigger (void);
extern void InitAcquisitionTask (void);
extern void HandleAcquisition (void);

#endif


