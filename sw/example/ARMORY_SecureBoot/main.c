// ================================================================================ //
// The NEORV32 RISC-V Processor - https://github.com/stnolting/neorv32              //
// Copyright (c) NEORV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file ARMORY_AES/main.c
 * @author [omitted]
 * @brief An implementation of Secureboot from ARMORY.
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
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

void verify_and_run_firmware(u8 *, u32,  u8 *);
void report_error();
void execute_firmware();


#define CH(x, y, z)     ((x & y) ^ ( (~x) & z))
#define MAJ(x, y, z)    ((x & y) ^ (x & z) ^ (y & z))
#define ROTR(x, n)      ((x >> n) | (x << (32-n)))
#define BSIG0(x)        (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1(x)        (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0(x)        (ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3))
#define SSIG1(x)        (ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10))


static const u32 constants[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

int fail = 1;
int pass = 0;

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
u32 input_length = 128;
u8 input[128] = {
        0x67, 0x45, 0x8b, 0x6b, 0xc6, 0x23, 0x7b, 0x32, 0x69, 0x98, 0x3c, 0x64, 0x73, 0x48, 0x33, 0x66,
        0x51, 0xdc, 0xb0, 0x74, 0xff, 0x5c, 0x49, 0x19, 0x4a, 0x94, 0xe8, 0x2a, 0xec, 0x58, 0x55, 0x62,
        0x29, 0x1f, 0x8e, 0x23, 0xcd, 0x7c, 0xe8, 0x46, 0xba, 0x58, 0x1b, 0x3d, 0xab, 0xd7, 0x7e, 0x50,
        0xf2, 0x41, 0xb1, 0x2e, 0xfb, 0x1e, 0xb7, 0x41, 0xe3, 0xa9, 0xe2, 0x79, 0x46, 0xe1, 0x45, 0x75,
        0x7c, 0x00, 0x5f, 0x51, 0xc2, 0x62, 0xd0, 0x5b, 0x54, 0x08, 0x20, 0x12, 0xf8, 0x27, 0xb1, 0x4d,
        0x1b, 0x23, 0x16, 0x02, 0xe8, 0xe9, 0x16, 0x1f, 0xe7, 0xcd, 0x90, 0x11, 0x8d, 0x43, 0xef, 0x66,
        0x76, 0x0f, 0x0e, 0x14, 0x5a, 0x25, 0x52, 0x33, 0x2e, 0xf9, 0x9c, 0x10, 0x63, 0x72, 0xed, 0x0d,
        0x33, 0xc2, 0xdc, 0x7f, 0x9f, 0xd7, 0xef, 0x1b, 0xc9, 0xc4, 0xa7, 0x41, 0x9a, 0x07, 0x68, 0x6b,
        };
u8 correct_hash[32] = {
        0xf2, 0x60, 0x9f, 0x26, 0x98, 0x6b, 0x3f, 0xd2, 0xdd, 0x21, 0x8b, 0x1d, 0xb6, 0x37, 0x41, 0x70,
        0x0b, 0x02, 0xe3, 0x27, 0x42, 0x9a, 0x2b, 0xb5, 0xb0, 0xbe, 0xfb, 0x76, 0x02, 0x45, 0x2b, 0xbe,
        };

uint8_t test_runner(int zero){
    int retVal;    

    verify_and_run_firmware(input, input_length, correct_hash);

    if(zero){
        super_secret_function();
    }

    return retVal;
}


// requires input_length to be multiple of 64
void sha256(u8* input, u32 input_length, u8 digest[32])
{
    u32 state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    for (int block = 0; block < input_length; block += 64)
    {
        u32 W[64];
        for (int i = 0; i < 16; ++i)
        {
            W[i] = (input[block + 4 * i] << 24) | (input[block + 4 * i + 1] << 16) | (input[block + 4 * i + 2] << 8) | input[block + 4 * i + 3];
        }
        for (u32 i = 16; i < 64; ++i)
        {
            W[i] = SSIG1(W[i - 2]) + W[i - 7] + SSIG0(W[i - 15]) + W[i - 16];
        }

        u32 tmp[8] = {state[0], state[1], state[2], state[3], state[4], state[5], state[6], state[7]};

        for (int t = 0; t < 64; ++t)
        {
            u32 T1 = tmp[7] + BSIG1(tmp[4]) + CH(tmp[4], tmp[5], tmp[6]) + constants[t] + W[t];
            u32 T2 = BSIG0(tmp[0]) + MAJ(tmp[0], tmp[1], tmp[2]);
            for (int i = 7; i > 0; --i)
            {
                tmp[i] = tmp[i - 1];
            }
            tmp[0] = T1 + T2;
            tmp[4] += T1;
        }

        for (int i = 0; i < 8; ++i)
        {
            state[i] += tmp[i];
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        digest[4 * i] = (u8)(state[i] >> 24);
        digest[4 * i + 1] = (u8)(state[i] >> 16);
        digest[4 * i + 2] = (u8)(state[i] >> 8);
        digest[4 * i + 3] = (u8)(state[i]);
    }
}

void verify_and_run_firmware(u8* firmware, u32 firmware_length, u8 correct_hash[32])
{
    u8 hash[32];
    sha256(firmware, firmware_length, hash);

    for (u32 i = 0; i < 32; ++i)
    {
        if (hash[i] != correct_hash[i])
        {
            report_error();
        }
    }

    return; // Just return. execute_firmware() Just does an infinite loop. We don't want that.
    //execute_firmware();
}

__attribute__((noreturn, noinline)) void report_error()
{
   while (1) {
    ;
   }
}

// self calling makes it easy to detect whether control-flow countermeasure raised an error by simply waiting for the second call of the function
__attribute__((noreturn, noinline)) void execute_firmware()
{
    execute_firmware(); 
}
