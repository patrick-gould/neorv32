// ================================================================================ //
// The NEORV32 RISC-V Processor - https://github.com/stnolting/neorv32              //
// Copyright (c) NEORV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file tiny_aes/main.c
 * @author [omitted]
 * @brief An implementation of tiny-aes-c for the neorv32. See https://github.com/kokke/tiny-AES-c for original project.
 **************************************************************************/

/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200 // Communication rate

// Includes
#include <neorv32.h>
#include <stdint.h>

uint8_t test_runner(uint8_t zero);
int verifyPIN_main();
int glitch_result = 0;

/**********************************************************************//**
 * Main function; runs a series of test functions for each tiny-aes-c library function.
 *
 * @return 0 on success, integer greater than 0 matching the number of failed function calls.
 **************************************************************************/
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

uint8_t test_runner(uint8_t zero){
    verifyPIN_main();
}

