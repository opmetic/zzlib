#include <stdlib.h> 
#include <stdio.h>
#include "../printdata.h"

int main()
{
    int iTmp = 12359;
    printf("hello world!\n");
    PrintRawData("abc", 3);
    printf("\n%d\n", sizeof(iTmp));

    PrintBinaryData((char *)(&iTmp), 4); 
    return 0;
}

