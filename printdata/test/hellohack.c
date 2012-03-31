#include <stdlib.h> 
#include <stdio.h>
#include "../printdata.h"

struct bs
{
    unsigned a: 1;
    unsigned b: 3;
    unsigned c: 4;
    unsigned d: 6;
};
int main()
{
    unsigned int iTmp = 12359;
    struct bs bsTmp;
    char str[4];

    printf("hello world!\n");
    PrintRawData("abc", 3);
    printf("\n%d\n", sizeof(iTmp));

    PrintBinaryData((char *)(&iTmp), 4); 
    printf("\n---------------------\n");
    bsTmp.a = 1;
    bsTmp.b = 4;
    bsTmp.c = 8;
    bsTmp.d = 3;
    PrintBinaryData((char *)(&bsTmp), 2);

    printf("\n---------------------\n");

    memset(str, '\0', 4);
    str[0] = 1;
    str[1] = 2;
    str[2] = 3;
    str[3] = 1;
    str[3] = str[3] << 7;
    PrintBinaryData(str, 4);

    memcpy((&iTmp), str, 4);
    printf("iTmp = %d\n", iTmp);
    return 0;
}

