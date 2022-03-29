#ifndef SUDOKU_H
#define SUDOKU_H

#define N 9

int sudoku(int grid[N][N]);
void init_sudoku(int grid[9][9]);
int sudoku_from_file(char *argv);

#endif
