#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "loader.h"
#include "solver.h"
#include "print.h"
#include "saver.h"

int sudoku(int grid[N][N])
{    
    if (solveSuduko(grid, 0, 0) == 1)
    {
        //print(grid);
        //save_grid(grid, argv);
        return 0;
    }
    else
        errx(1, "The sudoku is not solvable");
}

void init_sudoku(int grid[9][9])
{
    int i, j;
    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 9; j++)
        {
            grid[i][j] = 0;
        }
    }
}

int sudoku_from_file(char* argv)
{
    int grid[N][N];
    
    if (load(argv, grid) == 0)
    {
        if (solveSuduko(grid, 0, 0) == 1)
        {
            //print(grid);
            save_grid(grid, argv);
            return 0;
        }
        else
            errx(1, "The sudoku is not solvable");
    }
    else
        errx(1, "The string for the sudoku is not valid");
}
