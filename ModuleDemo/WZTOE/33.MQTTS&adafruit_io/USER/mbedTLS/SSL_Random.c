/*
 * file: SSL_Random.c
 * description: random generator function
 * author: peter
 * company: wiznet
 * data: 2015.11.26
 */

#include "SSL_Random.h"
#include "w55mh32_trng.h"
#include <stdint.h>
#include <stdio.h>

// int SSLRandomCB(void *p_rng, unsigned char *output, size_t output_len)
// {
//     if (output_len <= 0)
//     {
//         return (1);
//     }

//     uint32_t random_data;
//     uint32_t i = 0;
//     RCC->RCC_SYSCFG_CONFIG = 0x01;
//     SYSCFG->SYSCFG_LOCK = 0xCDED3526;
//     SYSCFG->SSC_CLK_EN = 0x07;
//     /* Start TRNG for random number generation */
//     TRNG_Start();
//     while (i < output_len)
//     {

//         while ((TRNG->RNG_CSR & TRNG_RNG_CSR_S128_TRNG0_Mask) == 0);
//         {
//             for (int j = 0; j < 4 && i < output_len; j++)
//             {
//                 random_data = TRNG->RNG_DATA; /* Read 32-bit random data */
//                 printf("rng%d:%08X\r\n",j,random_data);
//                 /* Break down the 32-bit data into bytes and store in output buffer */
//                 for (int k = 0; k < 4 && i < output_len; k++)
//                 {
//                     output[i] = (random_data >> (k * 8)) & 0xFF;
//                     // printf("output[%d] = 0x%02X\n", i, output[i]); /* Debug print */
//                     i++;
//                 }
//             }
//         }
//         /* TRNG->RNG_DATA register can read up to 4 consecutive 32-bit random values */
//         TRNG_ClearITPendingBit(TRNG_IT_RNG0_S128);
//     }
//     TRNG_Stop();
//     return (0);
// }

int SSLRandomCB(void *p_rng, unsigned char *output, size_t output_len)
{
    if (output_len <= 0)
    {
        return (1);
    }

    uint32_t random_data;
    uint32_t i = 0;
    uint32_t rng_buf[4];
    
 
    /* Start TRNG for random number generation */

    TRNG_Start();
    while (i < output_len)
    {

        // while ((TRNG->RNG_CSR & TRNG_RNG_CSR_S128_TRNG0_Mask) == 0);
        if (TRNG_Get(rng_buf) == 0)
        {
            for (int j = 0; j < 4 && i < output_len; j++)
            {
                random_data = rng_buf[j]; /* Read 32-bit random data */
                /* Break down the 32-bit data into bytes and store in output buffer */
                for (int k = 0; k < 4 && i < output_len; k++)
                {
                    output[i] = (random_data >> (k * 8)) & 0xFF;
                    // printf("output[%d] = 0x%02X\n", i, output[i]); /* Debug print */
                    i++;
                }
            }
        }
        /* TRNG->RNG_DATA register can read up to 4 consecutive 32-bit random values */
        TRNG_ClearITPendingBit(TRNG_IT_RNG0_S128);
    }
    TRNG_Stop();
    return (0);
}
