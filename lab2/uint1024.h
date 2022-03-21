#include <stdio.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct uint1024_t
{
    uint32_t numBlock[35];
} uint1024_t;

uint1024_t from_uint(unsigned int x)
{
    uint1024_t result;
    int filler = 0; // value to fill num blocks with
    memset(&result, filler, sizeof(uint32_t) * 35); // allocate memory for num blocks

    // fill num blocks with int32 values
    int i = 0;
    while ((x > 0) && (i < 35))
    {
        result.numBlock[i] = x % 1000000000;
        x = x / 1000000000; i++;

    }
    return result;
}

uint1024_t add_op (uint1024_t x, uint1024_t y)
{
    uint1024_t result; // variable to store result
    memset(&result, 0, sizeof(uint32_t) * 35); // set memory for num blocks

    for (int i = 0; i < 35; i++)
    {   // write to numBlock[i] value of sum
        result.numBlock[i] = (result.numBlock[i]) + (y.numBlock[i] + x.numBlock[i]) % 1000000000;
        if (i != 35 - 1) // remember the overflow value for next blocks
        { // only for blocks except last
            result.numBlock[1+i] = (y.numBlock[i] + x.numBlock[i]) / 1000000000 ;
        }
    }
    return result;
}

uint1024_t subtr_op (uint1024_t x, uint1024_t y)
{
    uint1024_t result;
    uint64_t rest;

    int filler = 0; // filler value to fill blocks with
    memset(&result, filler, sizeof(uint32_t)*35); // allocate memory for num blocks

    int j;
    for (int i = 0; i < 35; i++)
    {
        if (x.numBlock[i] < y.numBlock[i])
        {
            rest = (1000000000 + x.numBlock[i] - y.numBlock[i]) % 1000000000;
            result.numBlock[i] = rest;
            if (i != 35-1)
            {
                j = i + 1;
                // if numBlock == 000000000
                while ((x.numBlock[j] == 0) && (j != 35))
                {
                    x.numBlock[j] = 999999999;
                    j++;
                }
                x.numBlock[j] -= 1;
            }
        }
        else
        {result.numBlock[i] = x.numBlock[i] - y.numBlock[i];}
    }
    return result;
}

uint1024_t mult_op (uint1024_t x, uint1024_t y)
{
    uint1024_t result;
    uint64_t mult;
    int index;

    int filler = 0;
    memset(&result, filler, sizeof(uint32_t)*35); // allocate memory for num blocks

    for (int i = 0; i < 35; i++) // every block in x multiply by block in y
    { // y index
        for (int j = 0; i+j < 35; j++)
        { // x index
            if (x.numBlock[j] && y.numBlock[i])
            { // check blocks for 0s
                // 32bit block x 32 bit block
                mult = ((uint64_t)x.numBlock[j])*y.numBlock[i];
                // resulting array index
                index = i + j;
                while (mult != 0)
                { // check mult != 0
                    result.numBlock[index] += mult % 1000000000; // resulting block += mult div 1000000000
                    mult /= 1000000000; // mult is now 1000000000 times less
                    if (result.numBlock[index] >= 1000000000) // overflow
                    {
                        result.numBlock[index] %= 1000000000; mult += 1;
                    }
                    index++;
                }
            }
        }
    }
    return result;
}
