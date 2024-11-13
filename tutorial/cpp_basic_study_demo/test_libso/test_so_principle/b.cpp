//
// Created by viruser on 2024/11/12.
//

#include <stdio.h>
int             b = 30;
extern "C" void func_b(void)
{
    printf("in func_b. b = %d \n", b);
}
