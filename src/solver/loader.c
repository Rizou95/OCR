#include <err.h>
#include <stdio.h>
#include "loader.h"

int file_opening(char path[], int str[])
{
    FILE *fptr;
    fptr = fopen(path,"r");

    if(fptr == NULL)
    {
        errx(1, "Error!");   
    }

    int index = 0;
    char ch;
    while ((ch = fgetc(fptr)) != EOF) // get each char of the file
    {
        if ((ch >= '0' && ch <= '9')  || ch == '.')
        {
            str[index] = ch;
            index += 1;
        }
    }

    fclose(fptr);
    return 0;
}

int load(char path[], int grid[N][N]) // put the grid number in the grid array
{
    int str[81];
    file_opening(path, str);
    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            char c = str[y * 9 + x];
            if (c == '.')
                grid[y][x] = c - '0';
            else if (c < '1' || c > '9')
                errx(1, "Error: Invalid char");
            else
                grid[y][x] = c - '0';
        }
    }
    return 0;
}
