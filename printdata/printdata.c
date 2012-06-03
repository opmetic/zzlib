#include <ctype.h>
#include <stdio.h>
#include "printdata.h"

void PrintRawData (char *data, int len)
{
    unsigned int index = 0, startFlag = 1;
    int i, j, start, t = 0;
    unsigned char d;

    for (start = i = 0; i < len; i++)
    {
        if (i && (i % 16) == 0)
        {
            printf(" ");
            for (j = start; j < i; j++)
            {
                if (isprint (data[j]))
                {
                    printf ("%c", data[j]);
                }
                else
                {
                    printf (".");
                }
            }
            start = i;
            printf ("\n");
            startFlag = 1;
            t = 0;
        }
        if ((t > 0) && (t % 8) == 0)
        {
            printf(" ");
        }
        t++;

        if (1 == startFlag)
        {
            printf("%.6d  ", start);
        }

        d = data[i];
        printf ("%.2x ", d);

        startFlag = 0;
    }

    if (i % 16)
    {
        for (j = 0; j < (16 - (i % 16)); j++)
        {
            printf ("   ");
        }
		if ((i % 16) <= 8)
		{
			printf (" ");
		}
    }

    printf(" ");
    for (j = start; j < i; j++)
    {
        if (isprint (data[j]))
        {
            printf ("%c", data[j]);
        }
        else    
        {
            printf (".");
        }
    }
    printf ("\n");
}

void PrintBinaryData(char *data, int len)
{
    int i = 0, j = 0;
    unsigned char temp;
    char d;

    for( i=0; i<len; i++)
    {
        d = data[i];
        printf("%d|\t", (data + i));
        for (j=0; j<8; j++)
        {
            temp = d << j;
            temp = temp >> 7;

            printf("%d ", temp);
        }
        printf(" |%c\n", d);
    }

}


