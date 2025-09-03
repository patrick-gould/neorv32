// ================================================================================ //
// The NEORV32 RISC-V Processor - https://github.com/stnolting/neorv32              //
// Copyright (c) NEORV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file password/main.c
 * @author Patrick Gould
 * @brief A simple password checking program.
 **************************************************************************/

#include <neorv32.h>


/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200
/** Password success/failure values */
#define PASS_SUCCESS 1
#define PASS_FAILURE 0
/**@}*/

/// @brief A blank function that may used as a halting symbol; A point in the code to show a fault has occurred.
uint8_t __attribute__ ((noinline)) super_secret_function()
{
    return (uint8_t)PASS_SUCCESS; // Return has no special meaning.
}

/**********************************************************************//**
 * Main function; compares two static strings as a password check.
 *
 * @return 1 passwords match, 0 otherwise.
 **************************************************************************/
int main() {

  // Enables ADC counter. This is how we count clock cycles since the ADC samples 4 times each cycle—by default, anyway. Takes roughly 45 cycles of overhead on the ICE40 with a Neorv32 flashed.
  //trigger_high(); // Make sure ot offset cycles by 45

  char passwd[] = "touch";      // Same as chipwhisperer
  char badPasswd[] = "00000";   // Same as chipwhisperer
  uint8_t passok = PASS_SUCCESS;// Should only hold an unsigned 0/1 value, so a single byte is safe.
  int cnt;

  // Simple test - doesn't check for a too-long password!
  for (cnt = 0; cnt < 5; cnt++)
  {
    if (badPasswd[cnt] != passwd[cnt])
    {
      // This result gets loaded into a temp variable in assembly so this assignment actually only done once
      //  after the end of the loop. That is, we can skip it.
      passok = PASS_FAILURE; // <-- target for skip
    }
  }

  // If the above code somehow fails, we should pass this if-condition.
  if(passok) // <-- generates two targets for skip
  {
    passok = super_secret_function(); // Function returns PASS_SUCCESS.
  }

  //trigger_low(); // Disables ADC counter. Should not care about cycle offset
  //simpleserial_put('r', 1, (uint8_t *)&passok); // Should not care about cycle offset

  return passok; // Should "always" return 0.
}
