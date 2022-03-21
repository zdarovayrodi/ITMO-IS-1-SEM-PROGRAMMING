#include <stdio.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void printf_value (uint1024_t x)
{
    int index = 35-1;
    while (index >= 0 && x.numBlock[index] == 0)
    {
        index--; // moving pointer to first numblock that is not 000000000
    }

    if (index < 0) / in case x == 0 (all numBlocks == 000000000)
    {
        printf("0"); return;
    }

    printf("%u", x.numBlock[index]); // throw away leading 0s
    index--;

    while (index >= 0)
    {
        printf("%u", x.numBlock[index]);
        index--;
    }
}

void scanf_value(uint1024_t *x)
{
    char str[310]; // string of 310 symbols
    scanf("%s", str);// scan value as string
    memset(x, 0, sizeof(uint32_t)*35); // allocate memory for num blocks
    int position, index;
    char block[10]; block[9]='\0'; // ending zero (for ATOI function)

    int numLen == strlen(str)-9;
    for (position = numLen, index = 0; position >= 0; position -= 9, index++)
    {
        strncpy(block, str+position, 9); // copy from str starting at position to block 9 symbols
        x->numBlock[index] = atoi(block); // put num (9 symbols) in numBlock[i]
    }

    if (position % 9 != 0) // overflow (last block != 0)
    { // put last block in numBlock
        int rest = (position + 9) % 9;
        strncpy(block, str, rest); block[rest] = '\0';
        x->numBlock[index] =  atoi(block);
    }
}
