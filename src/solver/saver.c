#include <stdio.h>
#include <err.h>
#include <string.h>
#include "saver.h"

void save_grid(int grid[N][N], char path[])
{
    FILE *fptr;
    char* extension = ".result";
    char fileSpec[strlen(path)+strlen(extension)+1];
    snprintf( fileSpec, sizeof( fileSpec ), "%s%s", path, extension ); // add .result to the name of the original file
    fptr = fopen(fileSpec,"w");

    if(fptr == NULL)
   {
      errx(1, "Error!");   
   }

    int index = 0;
    for (int y = 0; y < N; y++)
    {
        for (int x = 0; x < N; x++)
        {
            if (index != 0 && index % 27 == 0) // empty line each 3 lines
                fprintf(fptr, "\n");
            if (index != 0 && index % 9 == 0) // return each 9 char
                fprintf(fptr, "\n");
            else if (index != 0 && index % 3 == 0) // space each 3 char
                fprintf(fptr, " ");
            fprintf(fptr,"%d", grid[y][x]);
            index++;
        }
    }
    fclose(fptr);
}
