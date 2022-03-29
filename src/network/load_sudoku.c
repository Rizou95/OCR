#include "../solver/loader.h"

void load_sudoku(int sudoku[9][9], int img_to_load)
{
    switch (img_to_load)
    {
    
    case 2:
        load("./src/network/grid_2", sudoku);
        break;
    
    case 5:
        load("./src/network/grid_5", sudoku);
        break;

    default:
        load("./src/network/grid_bonus", sudoku);
        break;
    }
}