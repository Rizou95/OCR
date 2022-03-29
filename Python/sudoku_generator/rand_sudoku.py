import random
import sys

def write_random():
    string = ""
    for i in range(81):
        if (random.randrange(100) > 30):
            string += str(random.randrange(1, 10))
        else:
            string += "."
    return string

def random_sudoku(nb):
    string = ""
    for i in range(nb):
        string += write_random() + "\n"
    with open("sudoku_strings", "w") as file:
        file.write(string)

if __name__ == '__main__':
    if len(sys.argv) == 2:
        try:
            number = int(sys.argv[1])
            random_sudoku(number)
        except:
            print("You need to give an integer")
    else:
        print("You need to enter a single integer")