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



/**@}*/

// Includes
#include <neorv32.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "header.h"

// prototypes
esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length);
esp_err_t esp_secure_boot_verify_ecdsa_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest);

/*
This main is a simple driver for a single secureboot function, esp_secure_boot_verify_ecdsa_signature_block(), that we will be attacking. The function will be fed some bad data and we want it to succeed. We do not use esp_secure_boot_verify_signature(), since it assumes memory mapped in an esp32's memory space. So, since we are not actually running a full bootloader due to size constraints, nor an esp32, we must only use esp_secure_boot_verify_ecdsa_signature_block().

As a note, this is NOT a novel vulnerability for SecureBoot V1. SecureBoot's attack model, in code, at least, is to assume no more than one fault, which we are breaking by feeding in already bad data and then faulting. SecureBoot's own documentation notes that current measures only protect against one fault (e.g., corrupted data or instruction skipping, but never both). Multiple faults would break these defenses, which we are effectively showing.

- [author of note anonymized for submission review]
*/

int fail = 0x2000 + 2;  // ESP_ERR_IMAGE_INVALID;   standard code for a bad image.
int pass = 0;           // ESP_OK;                  standard code for a good image.

uint8_t test_runner();
int glitch_result = 0;
/// @brief A blank function that may used as a halting symbol; A point in the code to show a fault has occurred. E.g., an instruction skip allowed unreachable code—like this function—to be executed.
uint8_t __attribute__((noinline)) super_secret_function()
{
    glitch_result = 1;
    return (uint8_t)1; // Return PASS_SUCCESS since we "passed" the password check.
}

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

    // Run test function
    test_runner(0);

  return glitch_result;
}

uint8_t test_runner(int zero){
    int retVal;                             // Return value. We consider value of "fail" to be a benign run.
    esp_secure_boot_sig_block_t badObj; // Holds correct values.
    uint8_t verified_digest[32];            // Holds a "verified" copy of the digest, in separate memory.
    const uint8_t* image_digest = verified_digest;         // Holds image digest.

    // Populate structs members.
    badObj.version = ESP_SECURE_BOOT_SCHEME;
    
    // Populate arrays.
    for(int i = 0; i<ARRAY_LENGTH; i++){
        badObj.signature[i] = 0xff;     // All 1's; dummy value.

        // Digests are only 32 long, so, we need to check.
        if ( i < ESP_SECURE_BOOT_DIGEST_LEN){
            // image_digest[i] = 0xff;         // All 1's; dummy value.
            verified_digest[i] = 0xff;      // All 1's; dummy value.
        }
    }

    // Call and return the value from esp_secure_boot_verify_ecdsa_signature_block
    retVal = (esp_err_t)esp_secure_boot_verify_ecdsa_signature_block(&badObj, image_digest, verified_digest);
    // Returns ESP_OK (0) or ESP_ERR_IMAGE_INVALID (0x2000 + 2).

    if(zero){
        super_secret_function();
    }

    return retVal;
}
