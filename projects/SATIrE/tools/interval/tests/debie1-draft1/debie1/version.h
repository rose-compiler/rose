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
 *    Module     :   version.h
 *
 * DEBIE SW version and checksum.
 *
 * Based on the SSF file version.h, rev 1.30, Wed Feb 23 12:17:32 2000.
 *      
 *- * --------------------------------------------------------------------------
 */

#ifndef VERSION_H
#define VERSION_H

#define SW_VERSION 24
/* Software version. Unique for each delivered build of the software. */

#define CODE_CHECKSUM 0x9F
/* This must be set so that the checksum calculated from the code */
/* memory becomes zero (this value will then be actually equal to */
/* the code checksum when this value would be zero).              */
/* So the procedure is as follows:                                */
/*   1. Update source code file(s), including SW_VERSION above.   */
/*   2. Set CODE_CHECKSUM to zero.                                */
/*   3. Compile and link DEBIE into a .hex file.                  */
/*   4. Compute the XOR of the .hex data (using e.g. 'hexor').    */
/*   5. Set CODE_CHECKSUM to the value computed in step 3.        */
/*   6. Recompile and relink DEBIE into a new .hex file.          */
/*   7. Verify that XOR of the new .hex file is zero.             */

#endif



