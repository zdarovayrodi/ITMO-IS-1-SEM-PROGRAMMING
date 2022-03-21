#include <stdio.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "uint1024.h"
#include "uint1024_io.h"


int main() {
    uint1024_t x, y;

    printf("1: "); scanf_value(&x);

    printf("2: "); scanf_value(&y);

    printf("Сложение:   "); printf_value(add_op(x, y));

    printf("\n");
    return 0;
}
