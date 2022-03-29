#include <stdio.h>
#include "print.h"

void print(int arr[N][N])
{
    for (int y = 0; y < 9; y++)
    {
        if (y % 3 == 0)
            printf("-------------\n");
        
        for (int x = 0; x < 9; x++)
        {
            if (x % 3 == 0)
                printf("|");
            
            int c = arr[y][x];
            if (c == 0)
                printf("   ");
            else
                printf("%d", arr[y][x]);
        }
        printf("|\n");
    }
    printf("-------------\n");
}
