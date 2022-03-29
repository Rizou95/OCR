import sys
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw

global sudoku_strings
global sudoku
sudoku = [
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    [0, 1, 2, 3, 4, 5, 6, 7, 8],
    ]
sudoku_strings = [[]]

def load_string(string):
    global sudoku
    i = 0
    j = 0
    for number in string:
        if (number == "."):
            sudoku[j][i] = ""
        else:
            sudoku[j][i] = str(number)
        if (i == 8):
            i = 0
            j += 1
        else:
            i += 1

def get_number(i, j):
    global sudoku
    return sudoku[i][j]

def open_file(filename):
    global sudoku_strings
    with open(filename) as file:
        sudoku_strings = file.readlines()
        sudoku_strings = [line.rstrip() for line in sudoku_strings]

def main(path):
    if path == None:
        open_file("sudoku_strings")
    else:
        open_file(path)
    count = 0
    for s in sudoku_strings:
        if (len(s) != 81):
            print("String not valid")
        else:
            load_string(s)
            img = Image.new('RGB', (302,302), (0,0,0))
            draw = ImageDraw.Draw(img)
            font = ImageFont.truetype("font/OpenSans-Regular.ttf", 20)
            for j in range(9):
                for i in range(9):
                    draw.rectangle([(i*28)+(5*i)+5, (j*28)+(5*j)+5, (i*28)+(5*i)+34, (j*28)+(5*j)+34],(255, 255, 255))
                    draw.text((28*i+5*i+14, 28*j+5*j+8), get_number(i,j),(0,0,0),font=font)
            img.save('exemple_sudoku/sudoku_' + str(count) + '.jpg')
            count += 1

if __name__ == '__main__':
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        main(None)
