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
 *    Module     :   hw_if.c
 *
 * Initialization and test of hardware.
 *
 * Based on the SSF file hw_if.c, rev 1.34, Sun Jul 25 15:47:56 1999.
 *
 *-----------------------------------------------------------------------------
 */

#include "dpu_ctrl.h"
#include "su_ctrl.h"
#include "isr_ctrl.h"
#include "version.h"
#include "tc_hand.h"
#include "tm_data.h"
#include "ttc_ctrl.h"


reset_class_t EXTERNAL s_w_reset;
/* The type of the last DPU reset, as recorded in Init_DPU. */
/* Note: this variable must _not_ be initialised here (in   */
/* its declaration), since this would overwrite the value   */
/* set in Init_DPU, which is called from the startup module */
/* before the variable initialisation code.                 */


unsigned char EXTERNAL SU_ctrl_register     = 0;
unsigned char EXTERNAL SU_self_test_channel = 0;
/* These variables store values of these write-only registers. */

unsigned char EXTERNAL LOCATION(0xFF00) forbidden_area[256];
/* Last 256 bytes of the external data memory are reserved for     */
/* memory mapped registers. No variables are allowed in that area. */

                  
/* Results of the RAM test done at boot time: */

code_address_t EXTERNAL failed_code_address;
data_address_t EXTERNAL failed_data_address;
#define NO_RAM_FAILURE 0xFFFF
/* The test records the address of the first failed cell in the */
/* code (program) RAM and the external data RAM.                */
/* If no problem is found, the address is NO_RAM_FAILURE.       */
/* Note: these variables must _not_ be initialised here (at     */
/* declaration), since this initialisation would be done after  */
/* the RAM test and so would destroy its results.               */
/* These variables are set by Init_DPU, which is called from    */
/* the startup module.                                          */


/* Other memory-control variables: */

memory_configuration_t EXTERNAL memory_mode;
/* The current memory mapping mode. */

const unsigned char PROGRAM checksum_nullifier = CODE_CHECKSUM;
/* Sets the checksum of the unpatched program code to zero. */

unsigned char EXTERNAL reference_checksum;
/* Expected code checksum. Zero for unpatched code. */

unsigned char EXTERNAL code_not_patched;
/* Initial value is 1, set in Init_DPU. Value is 1 when code */
/* checksum value is valid, cleared to 0 when code memory is */
/* patched, set to 1 when next checksum calculation          */
/* period is started.                                        */

unsigned char EXTERNAL analog_switch_bit[NUM_SU] = {0x10, 0x20, 0x40, 0x80};
/* This array stores the value to be used when analog switch bit */
/* corresponding to a given SU is set.                           */

/* Function prototypes. */
void CopyProgramCode(void);
unsigned short int InitCode_RAM (reset_class_t reset_class);
void InitData_RAM (
   reset_class_t  reset_class,
   code_address_t code_address);

/*****************************************************************************/
/*                               dpu_ctrl.h                                  */
/*****************************************************************************/

unsigned short int Check_RAM (
   data_address_t start_address,
   data_address_t end_address)
/* Purpose        : Check the working of an area of external RAM.            */
/* Interface      : inputs      - range of memory addresses.                 */
/*                  outputs     - first failed address.                      */
/*                                NO_RAM_FAILURE indicates "no failure".     */
/*                  subroutines - TestMemBits                                */
/*                              - TestMemSeq                                 */
/*                              - TestMemData                                */
/* Preconditions  : - start_address  <=  end_address.                        */
/*                  - end_address < 0xFFFF.                                  */
/*                  - External Data RAM is not initialised.                  */
/* Postconditions : The addresses in the given range are tested up to the    */
/*                  ending address or the first failing address.             */
/*                  The original contents of the memory area are lost.       */
/* Algorithm      : - The first memory address is tested with TestMemBits.   */
/*                  - The address range is traversed in pieces acceptable    */
/*                    to TestMemSeq/Data (max 255 bytes at a time).          */
/*                  - For each piece, TestMemSeq/Data is called.             */
/*                  - If an error is found, the failing address              */
/*                    is returned and the test is interrupted.               */
/*                  - If no failure is found, 0xFFFF is returned.            */
/*                                                                           */
/* This function must not rely on any constants in data RAM, since           */
/* it is called before the C start-up system initialises data.               */
     
{
   /* Note that the local variables of this function should all be */
   /* located in the internal data memory. Otherwise they may be   */
   /* altered unexpectedly as a side effect of testing memory.     */

   data_address_t INDIRECT_INTERNAL start;
   /* The starting address for TestMemSeq and TestMemData. */
   
   uint_least16_t INDIRECT_INTERNAL range;
   /* The remaining range (ending address - starting address). */

   uint_least8_t INDIRECT_INTERNAL bytes;
   /* The number of bytes to check, for TestMemSeq/Data. */
   
   uint_least8_t INDIRECT_INTERNAL bytes_left;
   /* The number of bytes left, returned from TestMemSeq/Data. */
   
   start = start_address;

   if (TestMemBits(start) != 0)
   {
      /* Failure in data bus, probably. */
      return start;
   }

   while (start <= end_address)
   {
      range = end_address - start;
      /* Number of bytes to check, less one. */
      
      if (range < 255)
      {
         /* One call of TestMemSeq/Data covers the remaining range. */
         bytes = (uint_least8_t)(range + 1);
      }
      else
      {
         /* One call cannot cover the remaining range. */
         /* Cover as much as possible for one call.    */
         bytes = 255;
      }
      
      bytes_left = TestMemSeq (start, bytes);
 
      if (bytes_left == 0)
      {
         /* TestMemSeq succeeded. Try TestMemData. */
         bytes_left = TestMemData (start, bytes);
      }
           
      if (bytes_left > 0)
      {
         /* Memory error. Return failing address. */
         return (bytes - bytes_left) + start;
      }
      
      start = start + bytes;
      /* Next address to check, or end_address+1 if all done.  */
      /* Wrap-around cannot happen since end_address < 0xFFFF. */
   }

   /* Memory is OK. */
   return NO_RAM_FAILURE;
}


void Init_DPU (reset_class_t reset_class)
/* Purpose        : DEBIE-specific DPU and I/O initialisation at reset.      */
/* Interface      : inputs      - reset class                                */
/*                  outputs     - s_w_reset                                  */
/*                              - code_not_patched                           */
/*                              - failed_code_address                        */
/*                              - failed_data_address                        */
/*                              - memory_mode                                */
/*                  subroutines - InitCode_RAM                               */
/*                              - InitData_RAM                               */
/* Preconditions  : Basic startup initialisation done.                       */
/*                  Code is being executed from PROM.                        */
/*                  Tasks are not yet running.                               */
/*                  External Data RAM is not initialised.                    */
/* Postconditions : see algorithm                                            */
/* Algorithm      : see below, self-explanatory.                             */

/* This function is called by the startup assembly code.           */
/* This function must not rely on any constants in data RAM, since */
/* it is called before the C start-up system initialises data.     */

{
   /* Note: be careful with use of external Data RAM in this function. */
   /* If InitData_RAM tests the RAM, it will destroy the contents.     */

   volatile reset_class_t INDIRECT_INTERNAL safe_reset_class;
   /* Copy of reset_class in internal RAM, safe from RAM test. */
   
   code_address_t INDIRECT_INTERNAL code_address;
   /* Result of InitCode_RAM. */


   safe_reset_class = reset_class;
   /* Copy to internal RAM. */

   SET_WD_RESET_LOW;

   /* The Watch Dog timer is reset by setting WD_RESET bit low at I/O  */
   /* port 1.                                                          */

   SET_INTERRUPT_PRIORITIES;
   /* Define the high/low priority of each interrupt. */

   SET_INT_TYPE1_EDGE;
   /* The interrupt control type 1 bit is set to 'falling edge' state. */

   SET_INT_TYPE0_EDGE;
   /* The interrupt control type 1 bit is set to 'falling edge' state. */

   STOP_TC_TIMER;
   SET_TC_TIMER_MODE;
   DISABLE_TC_TIMER_ISR;
   SET_TC_TIMER_OVERFLOW_FLAG;
   /* Prepare TC timer. */

   CLEAR_TC_INTERRUPT_FLAG;
   CLEAR_TM_INTERRUPT_FLAG;
   CLEAR_HIT_TRIGGER_ISR_FLAG;
   /* Clear pending interrupts. */

   /* RAM tests and code copying: */

   code_address = InitCode_RAM (safe_reset_class);
   
   InitData_RAM (safe_reset_class, code_address);
   
   /* Record RAM test results in external data as follows. */
   /* They will be safe now, since RAM test is over.       */
   /* Note, InitData_RAM already set failed_data_address,  */
   /* and InitCode_RAM selected the memory mode; here we   */
   /* just record the selection in memory_mode.            */
   /* The failed_code/data_addresses are not yet set in    */
   /* telemetry_data, since the latter will be cleared in  */
   /* a later step of the boot sequence.                   */
   
   s_w_reset           = safe_reset_class;
   code_not_patched    = 1;
   failed_code_address = code_address;
   
   if (code_address == NO_RAM_FAILURE)
   {
      memory_mode = SRAM_e;
   }
   else
   {
      memory_mode = PROM_e;
   }

   #ifdef USE_ALWAYS_PROM
      memory_mode = PROM_e;
   #endif

   SET_WD_RESET_HIGH;
   /* The Watch Dog time out signal state is reset HIGH state, as it is*/
   /* falling edge active.                                             */
}


code_address_t InitCode_RAM (reset_class_t reset_class)
/* Purpose        : Initialise Program Memory at reset.                      */
/* Interface      : inputs      - reset class                                */
/*                              - failed_code_address (if Warm Reset)        */
/*                  outputs     - returns new value for failed_code_address  */
/*                  subroutines - Check_RAM                                  */
/*                              - CopyProgramCode                            */
/* Preconditions  : Basic startup initialisation done.                       */
/*                  Code is being executed from PROM.                        */
/*                  Tasks are not yet running.                               */
/*                  External Data RAM is not initialised (except in case     */
/*                  of a Warm Reset).                                        */
/* Postconditions : For a reset that is not a Warm Reset:                    */
/*                  - Code SRAM tested, result in return value.              */
/*                  - Code copied from PROM to SRAM.                         */
/*                  - Running from SRAM if Code RAM is good.                 */
/*                  - Running from PROM if error found in Code RAM.          */
/*                  For a Warm Reset:                                        */
/*                  - Code in SRAM not modified.                             */
/*                  - Running from SRAM or PROM depending on the recorded    */
/*                    result of an earlier Code RAM test (from the global    */
/*                    variable failed_code_address).                         */
/*                  - failed_code_address unchanged and returned.            */
/* Algorithm      : see below.                                               */

/* Note: This initialisation may use external RAM, but any data it */
/* stores in external RAM will be destroyed by the RAM test,       */
/* which is done after this step in the reset sequence.            */
/* This function must not rely on any constants in data RAM, since */
/* it is called before the C start-up system initialises data.     */

{
   code_address_t INDIRECT_INTERNAL code_address;
   /* Value returned by Check_RAM, now or earlier. */
   
   if (reset_class == warm_reset_e)
   {
      /* Warm Reset: Do not copy PROM code to RAM. */
      /* Use result of memory test from earlier (non-warm) reset. */
      
      code_address = failed_code_address;      
   }
   else
   {
      /* HW Reset, Soft Reset or Checksum Reset. */
      
      code_address = Check_RAM ( BEGIN_SRAM1, END_SRAM1 );
      /* TBC that this does not rely on data RAM constants. */
      
      if (code_address == NO_RAM_FAILURE)
      {
         #ifndef USE_ALWAYS_PROM
            CopyProgramCode();
         #endif
         /* Code RAM is good. Copy code to it. */
         /* Later in the boot sequence, the reference checksum */
         /* must be reset to its initial value, to erase its   */
         /* memory of any code patches, or to initialise it in */
         /* case of a power-up reset or test of data RAM.      */
      }
   }
   
   if (code_address == NO_RAM_FAILURE)
   {
      #ifndef USE_ALWAYS_PROM
         SET_MEM_CONF_SRAM;
      #endif
      /* Code RAM is good. Run program from it.             */
      /* For a Warm Reset, the Code RAM may contain patches */
      /* relative to the PROM code, and the reference       */
      /* checksum should also retain a memory of them.      */
   }

   return code_address;
}


void InitData_RAM (
   reset_class_t  reset_class,
   code_address_t code_address)
/* Purpose        : Initialise Data Memory at reset.                         */
/* Interface      : inputs      - reset class                                */
/*                              - failed code address (in some cases)        */
/*                  outputs     - failed_data_address                        */
/*                  subroutines - Check_RAM                                  */
/* Preconditions  : Basic startup initialisation done.                       */
/*                  Tasks are not yet running.                               */
/*                  External Data RAM is not initialised (except in case     */
/*                  of a Warm Reset).                                        */
/* Postconditions : For a Power-Up Reset:                                    */
/*                  - Data SRAM tested, result in failed_data_address.       */
/*                    If the Code RAM test failed (as shown by code_address) */
/*                    only the upper half of the Data space is tested here.  */
/*                  - The data in the tested Data RAM is garbage.            */
/*                  For other kinds of reset:                                */
/*                  - Data in SRAM not modified.                             */
/* Algorithm      : see below.                                               */

/* This function must not rely on any constants in data RAM, since */
/* it is called before the C start-up system initialises data.     */

{
   if (reset_class == power_up_reset_e)
   {
      if (code_address == NO_RAM_FAILURE)
      {
         /* The Code RAM is good, so we have a fresh lower-half  */
         /* of the Data RAM to check, as well as the upper-half. */
         
         failed_data_address = Check_RAM (BEGIN_DATA_RAM, END_SRAM3);
      }
      else
      {
         /* The Code RAM is bad, and is still mapped to the     */
         /* lower-half of the Data space. Check only the upper  */
         /* half of the data space.                             */
         
         failed_data_address = Check_RAM (BEGIN_SRAM3, END_SRAM3);
      }
   }
}


void CopyProgramCode(void)
/* Purpose        : Copies program code from PROM to SRAM                    */
/* Interface      : -inputs: PROM                                            */
/*                  -outputs: SRAM1                                          */
/* Preconditions  : Program code is executed from PROM.                      */
/*                  External data RAM not initialised.                       */
/* Postconditions : SRAM1 holds same program as PROM.                        */
/* Algorithm      : Bytes are copied from code memory to same address in     */
/*                  external data memory in loop that goes through code      */
/*                  memory                                                   */
/* This function must not rely on any constants in data RAM, since */
/* it is called before the C start-up system initialises data.     */

{
   code_address_t i;
   INDIRECT_INTERNAL unsigned char code_byte;

   for (i = PROGRAM_COPY_START; i < PROGRAM_COPY_END; i++)
   {
      code_byte = GET_CODE_BYTE(i);
      SET_DATA_BYTE((data_address_t)i, code_byte);
   }
}


reset_class_t  GetResetClass(void)
/* Purpose        : Reset class is returned.                                 */
/* Interface      : - inputs:  s_w_reset, type of the occurred reset.        */
/*                  - outputs: s_w_reset                                     */
/* Preconditions  : Valid only when called first time after reset in boot    */
/*                  sequence.                                                */
/* Postconditions : s_w_reset is set to error_e value.                       */
/* Algorithm      : value of s_w_reset is returned and s_w_reset is set to   */
/*                  error value.                                             */
{
   register reset_class_t occurred_reset;
 
   occurred_reset = s_w_reset;
   s_w_reset      = error_e;
   return occurred_reset;    
}


void SignalMemoryErrors (void)
/* Purpose        : Copy results of RAM test to telemetry_data.              */
/* Interface      : - inputs:  failed_code_address, failed_data_address      */
/*                  - outputs: telemetry_data fields:                        */
/*                                failed_code_address                        */
/*                                failed_data_address                        */
/*                                mode_status bits for:                      */
/*                                   PROGRAM_MEMORY_ERROR                    */
/*                                   DATA_MEMORY_ERROR                       */
/* Preconditions  : Init_DPU called since reset.                             */
/* Postconditions : telemetry_data reflects the results of the memory tests  */
/*                  done in Init_DPU, as recorded in failed_code_address     */
/*                  and failed_data_address.                                 */
/*                  Note that the TM addresses are zero for "no failure".    */
/* Algorithm      : see below.                                               */
{
   if (failed_code_address == NO_RAM_FAILURE)
   {
      telemetry_data.mode_status        &= ~PROGRAM_MEMORY_ERROR;
      telemetry_data.failed_code_address = 0x0000;
   }
   else
   {
      telemetry_data.mode_status        |= PROGRAM_MEMORY_ERROR; 
      telemetry_data.failed_code_address = failed_code_address;
   }   
   
   if (failed_data_address == NO_RAM_FAILURE)
   {
      telemetry_data.mode_status        &= ~DATA_MEMORY_ERROR;
      telemetry_data.failed_data_address = 0x0000;
   }
   else
   {
      telemetry_data.mode_status        |= DATA_MEMORY_ERROR;
      telemetry_data.failed_data_address = failed_data_address;
   }
}


void SetMemoryConfiguration (memory_configuration_t memory)
/* Purpose        : External program memory is selected to be either PROM or */
/*                  SRAM1.                                                   */
/* Interface      : Port 1 is used.                                          */
/*                  output: memory_mode                                      */
/* Preconditions  : After power-up reset PROM is always selected.            */
/* Postconditions : External program memory is set depending on the given    */
/*                  parameter.                                               */
/*                  memory_mode contains the selected mode.                  */
/* Algorithm      : Memory configuration is selected with the output at the  */
/*                  I/O port 1.                                              */

{
   switch (memory)
   {
      case PROM_e:
         SET_MEM_CONF_PROM;
         break;
      case SRAM_e:
         SET_MEM_CONF_SRAM;
         break;
   }
   memory_mode = memory;  
}


void PatchCode(memory_patch_variables_t EXTERNAL *patch_variables)
/* Purpose        :  Code memory patching.                                   */
/* Interface      :  Following parameters are given: Address from where to   */
/*                   copy, address where to copy and the amount of bytes to  */
/*                   be copied. Execution result is returned. Variables used */
/*                   in this function are stored in a struct. Pointer to a   */
/*                   variable which stores an execution result of the        */
/*                   function SetMemoryConfiguration is passed on.           */
/* Preconditions  :  Source and destination addresses should be valid.       */
/* Postconditions :  Desired part of the memory is copied.                   */
/* Algorithm      :  Bytes are copied.                                       */

{

   fptr_t patch_function;
   /* Function pointer to the patched memory area. */

   unsigned char INDIRECT_INTERNAL old_checksum;
   /* Checksum calculated from the old contents of the pachted memory. */

   unsigned char INDIRECT_INTERNAL new_checksum;
   /* Checksum calculated from the new conrents of the patched memory. */

   unsigned char INDIRECT_INTERNAL patch_value;
   /* New value of a patched code memory byte. */

   unsigned char EXTERNAL temp_configuration;
   /* Original memory configuration. */

   uint_least8_t INDIRECT_INTERNAL i;
   /* Loop variable. */


   temp_configuration = GetMemoryConfiguration();
   /* State of the current memory configuration is stored. */
   
   DISABLE_INTERRUPT_MASTER;
   /* Disable all interrupts. */

   SetMemoryConfiguration (PROM_e);
   /* Enable code patching. */

   new_checksum = 0;
   old_checksum = 0;

   /* Memory block is copied from SRAM3 to SRAM1. */
        
   for (i=0 ; i < patch_variables -> data_amount ; i++)
   {
      old_checksum ^= GET_DATA_BYTE(patch_variables -> destination + i);
      patch_value   = *(patch_variables -> source + i);
      new_checksum ^= patch_value;

      SET_DATA_BYTE(patch_variables -> destination + i, patch_value);
   }

   reference_checksum ^= (old_checksum ^ new_checksum);

   SetMemoryConfiguration (temp_configuration);
   /* The initial memory configuration is restored. */

   switch (patch_variables -> execution_command)
   {
      case 0:
         /* Continue normally. */

         break;

      case 0x09:
         /* Execute soft reset. */

        Reboot (soft_reset_e);
         /* Function does not return. */
         break;

      case 0x37:
         /* Execute warm reset. */

         Reboot (warm_reset_e);
         /* Function deos not return. */
         break;

      case 0x5A:
         /* Jump to the patched memory. */

         patch_function = (fptr_t)(patch_variables -> destination);

         CALL_PATCH(patch_function);
         /* Called code may or may not return. */

         /* TC_state is selected upon return. */

         break;
   }
   ENABLE_INTERRUPT_MASTER;
   /* Enable all 'enabled' interrupts. */

}


memory_configuration_t GetMemoryConfiguration(void) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : Information about selected program memory is acquired    */
/*                  and returned.                                            */
/* Interface      : input: memory_mode                                       */
/* Preconditions  : none                                                     */
/* Postconditions : none                                                     */
/* Algorithm      : Information about current memory  configuration is       */
/*                  stored in a variable.                                    */

{
   return memory_mode; 
   /*Information about current memory configuration is stored in a global    */
   /*variable, which is returned.                                            */
}


/*****************************************************************************/
/*                                su_ctrl.h                                  */
/*****************************************************************************/

/* Sensor Unit power control                                                 */

void Switch_SU_On  (
   sensor_number_t SU_Number, 
   unsigned char EXTERNAL *execution_result) COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Given Sensor Unit is switched on.                       */
/* Interface      :  Execution result is stored in a variable.               */
/* Preconditions  :  SU_Number should be 1,2,3 or 4                          */
/* Postconditions :  Given Sensor Unit is switced on.                        */
/* Algorithm      :  The respective bit is set high in the SU on/off control */
/*                   register with XBYTE.                                    */
{
   switch (SU_Number)
{
   case SU_1:
      
      SU_ctrl_register |= 0x10;
      *execution_result = SU_1_ON;
      /* Set high bit 4 in the SU on/off control register,                   */
      /* preserves other bits.                                               */
      break;

   case SU_2:
      
      SU_ctrl_register |= 0x20;
      *execution_result = SU_2_ON;
      /* Set high bit 5 in the SU on/off control register,                   */
      /* preserves other bits.                                               */
      break;

   case SU_3:
      
      SU_ctrl_register |= 0x40;
      *execution_result = SU_3_ON;
      /* Set high bit 6 in the SU on/off control register,                   */
      /* preserves other bits.                                               */
      break;

   case SU_4:
      
      SU_ctrl_register |= 0x80;
      *execution_result = SU_4_ON;
      /* Set high bit 7 in the SU on/off control register,                   */
      /* preserves other bits.                                               */
      break;

   default:
       *execution_result = SU_NOT_ACTIVATED;
       /*Incorrect SU number has caused an error.                            */
       break;
   }

   SET_DATA_BYTE(SU_CONTROL,SU_ctrl_register);    

   telemetry_data.SU_status[SU_Number - SU_1] |= SU_ONOFF_MASK;
   /* SU_status register is updated to indicate that SU is switched on. */
   /* Other bits in this register are preserved.                        */
}
   

void Switch_SU_Off (
   sensor_number_t SU_Number, 
   unsigned char EXTERNAL *execution_result) COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Given Sensor Unit is switced off.                       */
/* Interface      :  Execution result is stored in a variable.               */
/* Preconditions  :  SU_Number should be 1,2,3 or 4.                         */
/* Postconditions :  Given Sensor Unit is switced off.                       */
/* Algorithm      :  The respective bit is set low with XBYTE.               */
{
   switch (SU_Number)
{
   case SU_1:
      
      SU_ctrl_register &= ~0x10;
      *execution_result = SU_1_OFF;
      /* Set low bit 4 in the SU on/off control register,                    */
      /* preserves other bits.                                               */
      break;

   case SU_2:
      
      SU_ctrl_register &= ~0x20;
      *execution_result = SU_2_OFF;
      /* Set low bit 5 in the SU on/off control register,                    */
      /* preserves other bits.                                               */
      break;
                                                                             
   case SU_3:
      
      SU_ctrl_register &= ~0x40;
      *execution_result = SU_3_OFF;
      /* Set low bit 6 in the SU on/off control register,                    */
      /* preserves other bits.                                               */
      break;

   case SU_4:
      
      SU_ctrl_register &= ~0x80;
      *execution_result = SU_4_OFF;
      /* Set low bit 7 in the SU on/off control register,                    */
      /* preserves other bits.                                               */
      break;

   default:
       *execution_result = SU_NOT_DEACTIVATED;
       /*Incorrect SU number has caused an error.                            */
       break;
   }

   SET_DATA_BYTE(SU_CONTROL,SU_ctrl_register);       

   telemetry_data.SU_status[SU_Number - SU_1] &= (~SU_ONOFF_MASK);
   /* SU_status register is updated to indicate that SU is switched off. */
   /* Other bits in this register are preserved.                         */
}
   

void EnableAnalogSwitch(sensor_index_t self_test_SU_index)
/* Purpose        : The analog switch output is enabled in the               */
/*                  self test channel register.                              */
/* Interface      : inputs      - self_test_SU_index                         */
/*                  outputs     - SU_self_test_channel, HW register          */
/*                  subroutines -  none                                      */
/* Preconditions  : none                                                     */
/* Postconditions : The analog switch output is enabled for a given          */
/*                  self test SU in the SU_self_test_channel register.       */
/* Algorithm      : - The respective bit is set in the SU_self_test_channel  */
/*                    variable and written to HW.                            */
{
   SU_self_test_channel |= analog_switch_bit[self_test_SU_index];
   /* The respective bit is set in the variable, preserve other bits.  */

   SET_DATA_BYTE(SU_SELF_TEST_CH, SU_self_test_channel);
}

void DisableAnalogSwitch(sensor_index_t self_test_SU_index)
/* Purpose        : The analog switch output is disabled in the              */
/*                  self test channel register.                              */
/* Interface      : inputs      - self_test_SU_index                         */
/*                  outputs     - SU_self_test_channel, HW register          */
/*                  subroutines -  none                                      */
/* Preconditions  : none                                                     */
/* Postconditions : The analog switch output is disabled for a given         */
/*                  self test SU in the SU_self_test_channel register.       */
/* Algorithm      : - The respective bit is reset in the SU_self_test_channel*/
/*                    variable and written to HW.                            */
{
   SU_self_test_channel &= ~analog_switch_bit[self_test_SU_index];
   /* The respective bit is set in the variable, preserve other bits.  */

   SET_DATA_BYTE(SU_SELF_TEST_CH, SU_self_test_channel);
}

void SelectSelfTestChannel(unsigned char channel)
/* Purpose        : A self test channel is selected in the                   */
/*                  self test channel register.                              */
/* Interface      : inputs      - channel                                    */
/*                  outputs     - SU_self_test_channel, HW register          */
/*                  subroutines -  none                                      */
/* Preconditions  : none                                                     */
/* Postconditions : The given self test channel is selected.                 */
/*                  self test SU in the SU_self_test_channel register.       */
/* Algorithm      : - The respective bit is set in the self test channel     */
/*                    register and written to HW.                            */
{
   unsigned char EXTERNAL channel_selector_value[NUM_CH];
   /* This array stores the selector bit states related to a given channel. */

   channel_selector_value[PLASMA_1_PLUS]    = 0x00;
   channel_selector_value[PLASMA_1_MINUS]   = 0x01;
   channel_selector_value[PZT_1]            = 0x02;
   channel_selector_value[PZT_2]            = 0x03;
   channel_selector_value[PLASMA_2_PLUS]    = 0x04;

   SU_self_test_channel = 
      (SU_self_test_channel & 0xF8) | channel_selector_value[channel]; 
   /* Set chosen bits preserve others. */

   SET_DATA_BYTE(SU_SELF_TEST_CH, SU_self_test_channel);
}


void ReadDelayCounters (delays_t EXTERNAL *delay)
/* Purpose        :  Read delay counters.                                    */
/* Interface      :  Results are stored into a given struct.                 */
/* Preconditions  :                                                          */
/* Postconditions :  Counters are read.                                      */
/* Algorithm      :  MSB and LSB are combined to form an 16 bit int.         */
{
   unsigned char msb, lsb;
   
   msb = GET_MSB_COUNTER & 0x0F;
   /* Correct set of four bits are selected in the MSB. */
   lsb = GET_LSB1_COUNTER;

   delay -> FromPlasma1Plus = (msb << 8) | lsb;

   msb = GET_MSB_COUNTER >> 4;
   /* Correct set of four bits are selected in the MSB.  */
   lsb = GET_LSB2_COUNTER;
      
   delay -> FromPlasma1Minus = (msb << 8) | lsb;   
}


unsigned char ReadRiseTimeCounter(void) COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Plasma1+ rise time counter is read from the specified   */
/*                   address.                                                */
/* Interface      :  Result is returned as an unsigned char.                 */
/* Preconditions  :                                                          */
/* Postconditions :  Data is gained.                                         */
/* Algorithm      :  Counter is read with XBYTE.                             */
{

   return GET_DATA_BYTE(RISE_TIME_COUNTER);

}

void ResetDelayCounters(void) COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Delay counters are reset.                               */
/* Interface      :  Port 1 is used.                                         */
/* Preconditions  :  Resetting takes place after acquisition.                */
/* Postconditions :  Delay counters are reset.                               */
/* Algorithm      :  The counter reset output bit at the I/O port 1 is set   */
/*                   first and then high.                                    */

{
   SET_COUNTER_RESET(LOW);
   /* Counters are reset by setting CNTR_RS bit to low in port 1 */

   SET_COUNTER_RESET(HIGH);
   /* The bit is set back to high */
}


void SetTriggerLevel(trigger_set_t EXTERNAL *setting) 
        COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Given trigger level is set.                             */
/* Interface      :  Execution result is stored in a variable.               */
/* Preconditions  :  SU_Number should be 1-4 and channel number 1-5 levels   */
/* Postconditions :  Given level is set for specific unit and channel.       */
/* Algorithm      :  The respective memory address is written into.          */
/*                                                                           */
/* This function is used by TelecomandExecutionTask and                      */
/* HealthMonitoringTask. despite the fact that it is of type re-enrant       */
/* the two tasks should not use it simultaniously. When                      */
/* HealthMonitoringTask is conducting self test and uses                     */
/* SetTriggerLevel, TelecomandExecutionTask is able to interrupt and         */
/* possibly set another trigger levels which would foul up the self          */
/* test. On the other hand when TelecomandExecutionTask is setting           */
/* trigger levels HealthMonitoringTask is disabled due to its lower          */
/* priority.                                                                 */


{
 
   setting -> execution_result = TRIGGER_SET_OK;

   switch (setting -> sensor_unit)
   /*sensor unit is selected*/
   {
      case SU_1:
      {
         setting -> base = SU_1_TRIGGER_BASE;
         break;
      }
      case SU_2:
      {
         setting -> base = SU_2_TRIGGER_BASE;
         break;
      }
      case SU_3:
      {
         setting -> base = SU_3_TRIGGER_BASE;
         break;
      }
      case SU_4:
      {
         setting -> base = SU_4_TRIGGER_BASE;
         break;
      }
      default:
      {
         setting -> execution_result = SU_NOT_SELECTED;
         /*Sensor Unit number is invalid.                                    */
         break;
      }
   }

   if (setting -> execution_result != SU_NOT_SELECTED)
   {
      switch (setting -> channel)
      /*channel is selected*/
      {
         case PLASMA_1_PLUS:
         {
            SET_DATA_BYTE(setting -> base + 0, setting -> level);
            break;
         }
         case PLASMA_1_MINUS:
         {
            SET_DATA_BYTE(setting -> base + 1, setting -> level);
            break;
         }
         case PZT_1_2:
         {
            SET_DATA_BYTE(setting -> base + 2, setting -> level);
            break;
         }
         default:
         {
            setting -> execution_result = CHANNEL_NOT_SELECTED;
            /*Given channel parameter is invalid.                            */
            break;
         }
      } 
   }

}


void SetTestPulseLevel(unsigned char level) COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Testpulse level is set.                                 */
/* Interface      :  input:  - Desired test pulse level.                     */
/* Preconditions  :  none.                                                   */
/* Postconditions :  Level is set.                                           */
/* Algorithm      :  Level is written into memory-mapped port address.       */
{
   SET_DATA_BYTE(TEST_PULSE_LEVEL, level);
}
   

void GetVoltageStatus(voltage_status_t EXTERNAL *v_status) 
        COMPACT_DATA REENTRANT_FUNC
/* Purpose        :  Voltage status data is gained.                          */
/* Interface      :  Port 1 is used.                                         */
/* Preconditions  :                                                          */
/* Postconditions :  Data is acquired.                                       */
/* Algorithm      :  HV status register is read into a struct with XBYTE.    */
/*                   V_DOWN bit is read from port 1.                         */
{
   v_status -> V_down_bit =  V_DOWN;
   v_status -> HV_status = GET_DATA_BYTE(HV_STATUS);
}
   
void ResetPeakDetector(sensor_number_t unit)
/* Purpose        :  Peak detector  is reset.                             */
/* Interface      :  -'Sensor unit on/off control register' is used       */
/* Preconditions  :  Resetting takes place after acquisition.             */       
/* Postconditions :  Peak detector  is reset.                             */
/* Algorithm      :  - Interrupts are disabled                            */
/*                   - SignalPeakDetectorReset function is called         */
/*                   - Interrupts are enabled                             */
/*                                                                        */
/* This function is used by Acquisition and HealthMonitoringTask.         */
/* However, it does not have to be of re-entrant type because collision   */
/* is avoided through design, as follows.                                 */
/* HealthMonitoring task uses ResetPeakDetector when Hit Budget has been  */
/* exeeded. This means that Acquisitiontask is disabled. When Acquisition */
/* task uses  ResetPeakDetector HealthMonitoringTask is disabled because  */
/* it is of lower priority .                                              */


{   
     DISABLE_INTERRUPT_MASTER;
     /* Disable all interrupts */

     SignalPeakDetectorReset(
        SU_ctrl_register & ~(1 << (unit - SU_1)),
        SU_ctrl_register);
     /* Generate reset pulse. */

     ENABLE_INTERRUPT_MASTER;
}

void SelectStartSwitchLevel(
   unsigned char  test_channel,
   sensor_index_t self_test_SU_index)
/* Purpose        : Select analog switch output level.                       */
/* Interface      : inputs      - self_test_SU_index, test_channel           */
/*                  outputs     - none                                       */
/*                  subroutines -  EnableAnalogSwitch                        */
/*                                 DisableAnalogSwitch                       */
/* Preconditions  : none                                                     */
/* Postconditions : Analog switch output level is selected depending on the  */
/*                  given channel.                                           */
/* Algorithm      : - Wanted level is selected.                              */
{

   if (test_channel == PLASMA_1_PLUS || test_channel == PLASMA_2_PLUS)
   {
      /* Channel triggered by falling edge. */
      EnableAnalogSwitch(self_test_SU_index);
      /* Set analog switch output level for the given channel. */
   }

   else
   {
      /* Rest of the channels triggered by rising edge. */
      DisableAnalogSwitch(self_test_SU_index);
      /* Set analog switch output level for the given channel. */
   }
}

void SelectTriggerSwitchLevel(
   unsigned char  test_channel,
   sensor_index_t self_test_SU_index)
/* Purpose        : Select analog switch output level.                       */
/* Interface      : inputs      - self_test_SU_index, test_channel           */
/*                  outputs     - none                                       */
/*                  subroutines -  EnableAnalogSwitch                        */
/*                                 DisableAnalogSwitch                       */
/* Preconditions  : none                                                     */
/* Postconditions : Analog switch output level is selected depending on the  */
/*                  given channel.                                           */
/* Algorithm      : - Wanted level is selected.                              */
/*                  - SW triggering is needed with channel PLASMA_2_PLUS.    */
{

   if (test_channel == PLASMA_1_PLUS)
   {
      /* Channel triggered by falling edge. */

      DisableAnalogSwitch(self_test_SU_index);
      /* Set analog switch output level for the given channel. */
   }

   else if (test_channel == PLASMA_2_PLUS)
   {
       /* Channel triggered by falling edge. SW trigger needed. */

       DisableAnalogSwitch(self_test_SU_index);
       /* Set analog switch output level for the given channel. */

       SET_DATA_BYTE(SU_SELF_TEST_CH, SU_self_test_channel);

       SET_HIT_TRIGGER_ISR_FLAG;
       /* SW trigger required. */
   }
  
   else
   {
       /* Rest of the channels triggered by rising edge. */

       EnableAnalogSwitch(self_test_SU_index);
       /* Set analog switch output level for the given channel. */
   }   
}

