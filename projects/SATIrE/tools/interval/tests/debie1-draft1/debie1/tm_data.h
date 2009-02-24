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
 *    Module     :   tm_data.h
 *
 * The Telemetry Data Structure.
 *
 * Based on the SSF file tm_data.h, rev 1.22, Mon May 31 10:10:12 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#ifndef TM_DATA_H
#define TM_DATA_H

#include "keyword.h"

#include "dpu_ctrl.h"
#include "su_ctrl.h"

#define MAX_EVENTS   1261
/* Size of one event is 26 bytes and MAX_EVENTS should be */ 
/* 32 768 bytes/ 26 bytes = 1261 (rounded up). This       */
/* ensures that at least 32 kB of memory is used.         */


#define NUM_CLASSES  10
#define NUM_TEMP     2
#define NUM_NOT_USED (4 + 0x70 - 0x6A)

#define DPU_SELF_TEST 0
#define STAND_BY      1
#define ACQUISITION   2

#define MODE_BITS_MASK 3

/* Definitions related to error indicating bits in mode status register: */
#define SUPPLY_ERROR                0x80
#define DATA_MEMORY_ERROR           0x40
#define PROGRAM_MEMORY_ERROR        0x20
#define ADC_ERROR                   0x04

/* Definitions related to error indicating bits in SU status register: */
#define HV_SUPPLY_ERROR             0x80
#define LV_SUPPLY_ERROR             0x40
#define TEMPERATURE_ERROR           0x20
#define SELF_TEST_ERROR             0x10
#define HV_LIMIT_ERROR              0x08 
#define LV_LIMIT_ERROR              0x04
#define SUPPLY_VOLTAGE_MASK         0x03   

/* Used when error indiacting bits are cleared. */



/* Definitions related to error indicating bits in error status register: */
#define CHECKSUM_ERROR              0x08
#define WATCHDOG_ERROR              0x04
#define OVERALL_SU_ERROR            0xF0
/* Used to indicate error in all of the SUs. */

#define ERROR_STATUS_OFFSET         0x10
/* Used when SU error indicating bit is selected. */


/* Definitions related to error indicating bits in software error register: */
#define MEASUREMENT_ERROR           0x01 
#define OS_START_SYSTEM_ERROR       0x02
#define OS_WAIT_ERROR               0x04
#define OS_SET_SLICE_ERROR          0x08


#define NUM_QCOEFF 5
/* Number of Quality Coefficients. */

/* Sensor Unit low power and TC settings : */

typedef struct {
   unsigned char plus_5_voltage;                    /* byte  1 */
   unsigned char minus_5_voltage;                   /* byte  2 */
   unsigned char plasma_1_plus_threshold;           /* byte  3 */
   unsigned char plasma_1_minus_threshold;          /* byte  4 */
   unsigned char piezo_threshold;                   /* byte  5 */
   unsigned char plasma_1_plus_classification;      /* byte  6 */
   unsigned char plasma_1_minus_classification;     /* byte  7 */
   unsigned char piezo_1_classification;            /* byte  8 */
   unsigned char piezo_2_classification;            /* byte  9 */
   unsigned char plasma_2_plus_classification;      /* byte 10 */
   unsigned char plasma_1_plus_to_minus_max_time;   /* byte 11 */
   unsigned char plasma_1_plus_to_piezo_min_time;   /* byte 12 */
   unsigned char plasma_1_plus_to_piezo_max_time;   /* byte 13 */
   unsigned char plasma_1_minus_to_piezo_min_time;  /* byte 14 */
   unsigned char plasma_1_minus_to_piezo_max_time;  /* byte 15 */
} SU_settings_t;

/* TM data registers : */

typedef struct {
   unsigned char        error_status;                      /* reg   0       */
   unsigned char        mode_status;                       /* reg   1       */
   uint16_t             TC_word;                           /* reg   2 -   3 */
   dpu_time_t           TC_time_tag;                       /* reg   4 -   7 */
   unsigned char        watchdog_failures;                 /* reg   8       */
   unsigned char        checksum_failures;                 /* reg   9       */
   unsigned char        SW_version;                        /* reg  10       */
   unsigned char        isr_send_message_error;            /* reg  11       */
   unsigned char        SU_status[NUM_SU];                 /* reg  12 -  15 */
   unsigned char        SU_temperature[NUM_SU][NUM_TEMP];  /* reg  16 -  23 */
   unsigned char        DPU_plus_5_digital;                /* reg  24       */
   unsigned char        os_send_message_error;             /* reg  25       */
   unsigned char        os_create_task_error;              /* reg  26       */
   unsigned char        SU_plus_50;                        /* reg  27       */
   unsigned char        SU_minus_50;                       /* reg  28       */
   unsigned char        os_disable_isr_error;              /* reg  29       */
   unsigned char        not_used_1;                        /* reg  30       */
   SU_settings_t        sensor_unit_1;                     /* reg  31 -  45 */
   unsigned char        os_wait_error;                     /* reg  46       */
   SU_settings_t        sensor_unit_2;                     /* reg  47 -  61 */
   unsigned char        os_attach_interrupt_error;         /* reg  62       */
   SU_settings_t        sensor_unit_3;                     /* reg  63 -  77 */
   unsigned char        os_enable_isr_error;               /* reg  78       */
   SU_settings_t        sensor_unit_4;                     /* reg  79 -  93 */
   code_address_t       failed_code_address;               /* reg  94 -  95 */
   data_address_t       failed_data_address;               /* reg  96 -  97 */
   uint16_t             SU_hits[NUM_SU];                   /* reg  98 - 105 */
   tm_dpu_time_t        time;                              /* reg 106 - 109 */
   unsigned char        software_error;                    /* reg 110       */
   unsigned char        hit_budget_exceedings;             /* reg 111       */
   unsigned char        coefficient[NUM_QCOEFF];           /* reg 112 - 116 */
   unsigned char        not_used;                          /* reg 117       */

   /* The last register of telemetry data should be 'not_used'.   */
   /* This is necessary for correct operation of telemetry        */
   /* retrieving TCs i.e. number of bytes should be even.         */

}  telemetry_data_t;

extern EXTERNAL telemetry_data_t telemetry_data;

/* Hit trigger event record : */

typedef struct {
   unsigned char      quality_number;    /* byte  0      */
   unsigned char      classification;    /* byte  1      */
   unsigned char      SU_number;         /* byte  2      */
   tm_dpu_time_t      hit_time;          /* byte  3 -  6 */
   unsigned char      SU_temperature_1;  /* byte  7      */
   unsigned char      SU_temperature_2;  /* byte  8      */
   tm_ushort_t        plasma_1_plus;     /* byte  9 - 10 */
   tm_ushort_t        plasma_1_minus;    /* byte 11 - 12 */
   tm_ushort_t        piezo_1;           /* byte 13 - 14 */
   tm_ushort_t        piezo_2;           /* byte 15 - 16 */
   tm_ushort_t        plasma_2_plus;     /* byte 17 - 18 */
   unsigned char      rise_time;         /* byte 19      */
   signed   char      delay_1;           /* byte 20      */
   tm_ushort_t        delay_2;           /* byte 21 - 22 */
   tm_ushort_t        delay_3;           /* byte 23 - 24 */
   unsigned char      checksum;          /* byte 25      */
} event_record_t;

/* Science Data File : */

typedef struct {
   unsigned short int length;
   unsigned char  event_counter[NUM_SU][NUM_CLASSES];
   unsigned char  not_used;
   unsigned char  counter_checksum;
   event_record_t event[MAX_EVENTS];
}  science_data_file_t;

extern EXTERNAL science_data_file_t
                LOCATION(SCIENCE_DATA_START_ADDRESS) science_data;

extern uint_least16_t EXTERNAL max_events;
/* This variable is used to speed up certain    */
/* Functional Test by adding the possibility    */
/* to restrict the amount of events.            */
/* It is initialised to value MAX_EVENTS at     */
/* Boot.                                        */

extern void RecordEvent(void);
/* This function increments proper event counter and stores   */
/* the new event record to the science data memory, if there  */  
/* is free place or events with lower or equal quality number */


extern void ClearEvents(void);
/* Cleares the event counters and the quality numbers of    */
/* the event records in the science data memory              */

#endif

