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
 *    Module     :   telem.c
 *
 * Telemetry module.
 *
 * Based on the SSF file telem.c, rev 1.28, Wed Oct 13 19:49:34 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#include "keyword.h"
#include "kernobj.h"
#include "tm_data.h"
#include "msg_ctrl.h"
#include "tc_hand.h"
#include "telem.h"
#include "ttc_ctrl.h"
#include "su_ctrl.h"
#include "dpu_ctrl.h"
#include "isr_ctrl.h"
#include "taskctrl.h"
#include "health.h"

EXTERNAL telemetry_data_t    telemetry_data;

EXTERNAL science_data_file_t LOCATION(SCIENCE_DATA_START_ADDRESS) science_data;

uint_least16_t EXTERNAL max_events;
/* This variable is used to speed up certain    */
/* Functional Test by adding the possibility    */
/* to restrict the amount of events.            */
/* It is initialised to value MAX_EVENTS at     */
/* Boot.                                        */

unsigned char EXTERNAL *telemetry_pointer;

unsigned char EXTERNAL *telemetry_end_pointer;

unsigned char EXTERNAL read_memory_checksum;
/* Checksum to be sent at the end of Read Memory sequence. */

#define MAX_QUEUE_LENGTH 10
/* Maximum amount of event records in the queue.  */
/* Is expected to be at most 255 (unsigned char). */

event_record_t EXTERNAL event_queue[MAX_QUEUE_LENGTH];
/* Holds event records before they are copied to the */
/* Science Data memory. Normally there is only data  */
/* from the new event whose data is beign collected, */
/* but during Science Telemetry there can be stored  */
/* several older events which are copied to the      */
/* Science Data memory after telemetry ends.         */

uint_least8_t EXTERNAL event_queue_length;
/* Number of event records stored in the queue.    */
/* These records are stored into event_queue table */
/* in order starting from the first element.       */
/* Initialised to zero on power-up.                */

uint_least16_t EXTERNAL free_slot_index;
/* Index to the first free record in the Science         */
/* Data memory, or if it is full equals to 'max_events'. */
/* Initialised to zero on power-up.                      */


event_record_t EXTERNAL *GetFreeRecord(void)

/* Purpose        : Returns pointer to free event record in event queue.     */
/* Interface      : inputs      - event_queue_length, legnth of the event    */
/*                                queue.                                     */
/*                  outputs     - return value, pointer to the free record.  */
/*                  subroutines - none                                       */
/* Preconditions  : none.                                                    */
/* Postconditions : none.                                                    */
/* Algorithm      : -If the queue is not full                                */
/*                     -return pointer to the next free record               */
/*                  -else                                                    */
/*                     -return pointer to the last record                    */

{
   if (event_queue_length < MAX_QUEUE_LENGTH)
   {
      return &(event_queue[event_queue_length]);
   }

   else
   {
      return &(event_queue[MAX_QUEUE_LENGTH - 1]);
   }
}


void TM_InterruptService (void) INTERRUPT(TM_ISR_SOURCE) USED_REG_BANK(2)
/* Purpose        : This function handles the TM interrupts.                 */
/* Interface      : inputs  - telemetry_pointer                              */
/*                            telemetry_end_pointer                          */
/*                            TC_state                                       */
/*                            telemetry_data                                 */
/*                  outputs - telemetry_pointer                              */
/*                            TM HW reigsiters                               */
/*                            Telemcommand Execution task mailbox            */
/* Preconditions  : telemetry_pointer and telemetry_end_pointer have valid   */
/*                  values (TM interrupts should be enabled only when this   */
/*                  condition is true)                                       */
/* Postconditions : Next two bytes are written to TM HW registers and if they*/
/*                  were the last bytes to be written, a "TM_READY" mail is  */
/*                  sent to the Telecommand Execution task                   */
/* Algorithm      : - if telemetry_pointer < telemetry_end_pointer           */
/*                     - write next two bytes from location pointed by       */
/*                       telemetry_pointer and increase it by two            */
/*                  - else if TC_state == register_TM_e                      */
/*                     - write first two TM data registers and set           */
/*                       telemetry_pointer to point to the third TM data     */
/*                       register                                            */
/*                  - else                                                   */
/*                     - send TM_READY message to Telecommand Execution task */
/*                       mailbox                                             */

{
   unsigned char EXTERNAL tm_byte;

   CLEAR_TM_INTERRUPT_FLAG;
   /*The interrupt flag is put down by setting high bit 3 'INT1' in port 3.  */

   if (telemetry_pointer == (unsigned char *) &telemetry_data.time)
   {
      COPY (telemetry_data.time, internal_time);
   }

   if (telemetry_pointer < telemetry_end_pointer)
   {
      /* There are bytes left to be sent to TM. */

      tm_byte = *telemetry_pointer;
      WRITE_TM_MSB (tm_byte);
      read_memory_checksum ^= tm_byte;

      telemetry_pointer++;

      tm_byte = *telemetry_pointer;
      WRITE_TM_LSB (tm_byte);
      read_memory_checksum ^= tm_byte;

      telemetry_pointer++;
   }
   else if (TC_state == register_TM_e)
   /* Start to send TM data registers starting from the first ones */
   {
      telemetry_pointer = (EXTERNAL unsigned char *)&telemetry_data;
      WRITE_TM_MSB (*telemetry_pointer);
      telemetry_pointer++;
      WRITE_TM_LSB (*telemetry_pointer);
      telemetry_pointer++;  
   }
   else if (TC_state == memory_dump_e)
   {
      WRITE_TM_MSB(0);
      WRITE_TM_LSB(read_memory_checksum);
      /* Last two bytes of Read Memory sequence. */

      Send_ISR_Mail(TCTM_MAILBOX, TM_READY);
   }
   else
   /* It is time to stop sending telemetry */
   {
      Send_ISR_Mail (TCTM_MAILBOX, TM_READY);
   }
}


dpu_time_t GetElapsedTime(unsigned int event_number)
/* Purpose        : Returns the hit time of a given event.                   */
/* Interface      : inputs      - event_number (parameter)                   */
/*                                science_data[event_number].hit_time, hit   */
/*                                time of the given event record.            */
/*                  outputs     - return value, hit time.                    */
/*                  subroutines - none                                       */
/* Preconditions  : none.                                                    */
/* Postconditions : none.                                                    */
/* Algorithm      :    -copy hit time of an event to a local variable hit    */
/*                      time                                                 */
/*                     -return the value of hit time                         */
{
   dpu_time_t INDIRECT_INTERNAL hit_time;
   /* Hit time. */

   COPY (hit_time, science_data.event[event_number].hit_time);

   return hit_time;
}



unsigned int FindMinQualityRecord(void)

/* Purpose        : Finds event with lowest quality from Science Data memory.*/
/* Interface      : inputs      - science_data.event, event records          */
/*                  outputs     - return value, index of event record with   */
/*                                   the lowest quality.                     */
/*                  subroutines - GetElapsedTime                             */
/* Preconditions  : none.                                                    */
/* Postconditions : none.                                                    */
/* Algorithm      : -Select first the first event record.                    */
/*                  -Loop from the second record to the last:                */
/*                     -if the quality of the record is lower than the       */
/*                      quality of the selected one, select the record.      */
/*                     -else if the quality of the record equals the quality */
/*                      of selected one and it is older than the selected    */
/*                      one, select the record.                              */
/*                  -End loop.                                               */
/*                  -return the index of the selected record.                */

{
   unsigned int INDIRECT_INTERNAL min_quality_number;
   /* The quality number of an event which has the lowest quality */
   /* number in the science data.                                 */

   unsigned int INDIRECT_INTERNAL min_quality_location;
   /* The location of an event which has the lowest quality number */
   /* in the science data.                                         */
  
   dpu_time_t DIRECT_INTERNAL min_time;
   /* Elapsed time of the oldest event. */

   dpu_time_t DIRECT_INTERNAL time;
   /* Elapsed time as previously mentioned. */

   uint_least16_t DIRECT_INTERNAL i;
   /* Loop variable. */
   

   min_time             = GetElapsedTime(0);
   min_quality_number   = science_data.event[0].quality_number;
   min_quality_location = 0;
   /* First event is selected and compared against */
   /* the following events in the science_data.    */

   for (i=1; i < max_events; i++)
   {
      time = GetElapsedTime(i);

      if(science_data.event[i].quality_number < min_quality_number)
      {
         min_time = time;
         min_quality_number = science_data.event[i].quality_number;
         min_quality_location = i;
         /* If an event in the science_data has a lower quality number than  */
         /* any of the previous events, its quality_number and location is   */
         /* stored into variables.                                           */
      }

      else if(   (science_data.event[i].quality_number == min_quality_number)
              && (time < min_time))
      {
         min_time = time;
         min_quality_location = i;
         /* If an event in the science_data has an equal quality number with */
         /* any of the previous events and it's older, event's               */
         /* quality_number and location are stored into variables.           */
      }
   }

   return min_quality_location;
}


void IncrementCounters(
   sensor_index_t sensor_unit,
   unsigned char  classification)

/* Purpose        : Increments given event counters.                         */
/* Interface      : inputs      - sensor_unit (parameter)                    */
/*                                classification (parameter)                 */
/*                  outputs     - telemetry_data.SU_hits, counter of hits of */
/*                                   given Sensor Unit                       */
/*                                science_data.event_counter, counter of     */
/*                                events with given classification and SU.   */
/*                  subroutines - none                                       */
/* Preconditions  : none.                                                    */
/* Postconditions : Given counters are incremented, if they had not their    */
/*                  maximum values.                                          */
/* Algorithm      : Increment given counters, if they are less than their    */
/*                  maximum values. Calculate checksum for event counter.    */
/*                                                                           */
/* This function is used by Acquisition and TelecommandExecutionTask.        */
/* However, it does not have to be of re-entrant type because collision      */
/* is avoided through design, as follows.                                    */
/* If Science Telemetry is in progress when Acquisition task is handling     */
/* an event, the event record cannot be written to the Science Data          */
/* memory. Instead it is left to the temporary queue which will be           */
/* copied to the Science Data memory after the Science telemetry is          */
/* completed. For the same reason  call for IncrementCounters is             */
/* disabled.                                                                 */
/* On the other hand, when Acquisition task is handling an event with        */
/* RecordEvent all interrupts are disabled i.e. TelecommandExecutionTask     */
/* cannot use IncrementCounters simultaniously.                              */



{
   unsigned char EXTERNAL counter;
   unsigned char EXTERNAL new_checksum;


   if (telemetry_data.SU_hits[sensor_unit] < 0xFFFF)
   {
      telemetry_data.SU_hits[sensor_unit]++;
      /* SU hit counter is incremented. */
   }

   if (science_data.event_counter[sensor_unit][classification] < 0xFF)
   {

      counter = science_data.event_counter[sensor_unit][classification];

      new_checksum = 
         science_data.counter_checksum ^ counter;
      /* Delete effect of old counter value from the checksum. */

      counter++;

      new_checksum ^= counter;
      /* Add effect of new counter value to the checksum. */

      science_data.event_counter[sensor_unit][classification] = counter;
      /* The event counter is incremented. */

      science_data.counter_checksum = new_checksum;
      /* Event counter checksum is updated. */
   }

}


/*****************************************************************************/
/*                               tm_data.h                                   */
/*****************************************************************************/

void RecordEvent(void)
/* Purpose        : This function increments proper event counter and stores */
/*                  the new event record to the science data memory.         */
/* Interface      : inputs      - free_slot_index, index of next free event  */
/*                                   record in the Science Data memory.      */
/*                                TC_state, state of the TC Execution task.  */
/*                                event_queue_length, length of the event    */
/*                                   record queue.                           */
/*                                event_queue, event record queue.           */
/*                  outputs     - event_queue_length, as above.              */
/*                                science_data.event, event records in       */
/*                                Science Data memory.                       */
/*                                free_slot_index, as above.                 */
/*                  subroutines - FindMinQualityRecord                       */
/*                                IncrementCounters                          */
/* Preconditions  : none.                                                    */
/* Postconditions : If Science telemetry is not in progress, event data is   */
/*                  stored in its proper place in the science data,          */
/*                  otherwise event data is left in the queue and one record */
/*                  is reserved from the queue unless it is already full.    */
/* Algorithm      : If there is room in the Science Data memory, the event   */
/*                  data is tried to be stored there, otherwise the event    */
/*                  with the lowest quality is searched and tried to be      */
/*                  replaced. If the Science telemetry is in progress the    */
/*                  event data is left in the queue and the length of the    */
/*                  queue is incremented unless the queue is already full.   */
/*                  If the Science telemetry is not in progress the event    */
/*                  data is copied to the Science Data to the location       */
/*                  defined earlier as described above.                      */

{
   uint_least16_t INDIRECT_INTERNAL record_index;

   DISABLE_INTERRUPT_MASTER;

   record_index = free_slot_index;

   if (record_index >= max_events && TC_state != SC_TM_e)
   {
      /* Science Data memory was full and Science TM was not in progress */

      ENABLE_INTERRUPT_MASTER;
      record_index = FindMinQualityRecord();
      DISABLE_INTERRUPT_MASTER;
   }

   if (TC_state == SC_TM_e)
   {
      /* Science Telemetry is in progress, so the event record */
      /* cannot be written to the Science Data memory. Instead */
      /* it is left to the temporary queue which will be       */
      /* copied to the Science Data memory after the Science   */
      /* telemetry is completed.                               */

      if (event_queue_length < MAX_QUEUE_LENGTH)
      {
         /* There is still room in the queue. */

         event_queue_length++;
         /* Prevent the event data from being overwritten. */
      }
      ENABLE_INTERRUPT_MASTER;
   }

   else
   {
      if (free_slot_index < max_events)
      {
	 /* Science Data memory was not full */

         record_index = free_slot_index;
         science_data.event[record_index].quality_number = 0;
         free_slot_index++;
      }


      /* Increment event counters. */
      IncrementCounters(
         event_queue[0].SU_number - 1,
         event_queue[0].classification);

      ENABLE_INTERRUPT_MASTER;

      if (event_queue[0].quality_number >=
          science_data.event[record_index].quality_number)

      {
         STRUCT_ASSIGN (
            science_data.event[record_index],
            event_queue[0],
            event_record_t);

         /* In this state the event data is located always to */
         /* the first element of the queue.                   */
      }
   }
}   


void ClearEvents(void)
/* Cleares the event counters and the quality numbers of                     */
/* the event records in the science data memory                              */

{
   DIRECT_INTERNAL uint_least8_t i;
   /* This variable is used in the for-loop which goes through  */
   /* the science data event counter.                           */

   DIRECT_INTERNAL uint_least8_t j;
   /* This variable is used in the for-loop which goes through  */
   /* the science data event counter.                           */

   /* Interrupts does not need to be disabled as long as  */
   /* Telecommand Execution task has higher priority than */
   /* Acquisition task.                                   */

   for(i=0;i<NUM_SU;i++)
   {
      telemetry_data.SU_hits[i] = 0;

      for(j=0;j<NUM_CLASSES;j++)
      {
         science_data.event_counter[i][j] = 0;
      }
      /*event counters are cleared in science_data                           */
   }

   for (i=0; i < event_queue_length; i++)
   {
      /* Events from the event queue are copied to the Science */
      /* Data memory.                                          */

      STRUCT_ASSIGN (
         science_data.event[i],
         event_queue[i],
         event_record_t);

      IncrementCounters(
         event_queue[i].SU_number - 1,
         event_queue[i].classification);

      /* One more event is stored in the Science Data memory. */
      /* NOTE that the event queue should always be smaller   */
      /* than the space reserved for event records in the     */
      /* Science Data memory.                                 */
   }

   free_slot_index    = event_queue_length;

   event_queue_length = 0;
   /* Empty the event queue. */

   science_data.counter_checksum = 0;
   science_data.not_used         = 0;
}   

void ResetEventQueueLength(void)
/* Purpose        : Empty the event queue length.                            */
/* Interface      : inputs      - none                                       */
/*                  outputs     - none                                       */
/*                  subroutines - none                                       */
/* Preconditions  : none.                                                    */
/* Postconditions : none.                                                    */
/* Algorithm      : - reset event queue length.                              */
{
      event_queue_length = 0;
}


   







