// ================================================================================ //
// The NEORV32 RISC-V Processor - https://github.com/stnolting/neorv32              //
// Copyright (c) NEORV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file tiny_ecdh/main.c
 * @author [omitted]
 * @brief An implementation of tiny-ecdh-c for the neorv32. See https://github.com/kokke/tiny-ecdh-c for original project.
 **************************************************************************/

/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200 // Communication rate
/**@}*/

// Includes
#include <neorv32.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re.h"

// Prototypes
void test_runner(uint8_t);

int glitch_result = 0;

/// @brief A blank function that may used as a halting symbol; A point in the code to show a fault has occurred. E.g., an instruction skip allowed unreachable code—like this function—to be executed.
uint8_t __attribute__((noinline)) super_secret_function()
{
  glitch_result = 1;
  return (uint8_t)1; // Return PASS_SUCCESS since we "passed" the password check.
}


int main() {

  // capture all exceptions and give debug info via UART
  // this is not required, but keeps us safe
  neorv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  neorv32_uart0_setup(BAUD_RATE, 0);

  // Run test functions
  test_runner(0);

  return glitch_result;
}

void __attribute__((noinline)) test_runner(uint8_t zero){

  // Call test code
  main_1();

  asm("NOP");

  // run our glitch check
  if(zero){
    super_secret_function();
  }
}