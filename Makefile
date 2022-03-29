CC = gcc
CFLAGS = -Wall  -std=c99 -O3 -g
CPPFLAGS= `pkg-config --cflags sdl gtk+-3.0 glib-2.0` -MMD
LDLIBS= `pkg-config --libs sdl gtk+-3.0 glib-2.0` -lSDL_image -lSDL_gfx
EXEC = OCR
OBJ = obj
SRC = src

all : $(EXEC)

$(EXEC) : $(OBJ)/main.o $(OBJ)/color_suppr.o $(OBJ)/detection.o $(OBJ)/segmentation.o $(OBJ)/network.o $(OBJ)/load_sudoku.o $(OBJ)/loader.o $(OBJ)/saver.o $(OBJ)/solver.o $(OBJ)/sudoku.o $(OBJ)/sdl_utils.o
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)/main.o $(OBJ)/color_suppr.o $(OBJ)/detection.o $(OBJ)/segmentation.o $(OBJ)/network.o $(OBJ)/load_sudoku.o $(OBJ)/loader.o $(OBJ)/saver.o $(OBJ)/solver.o $(OBJ)/sudoku.o $(OBJ)/sdl_utils.o -lm ${CPPFLAGS} -lm ${LDLIBS}

# -------------------- Main --------------------------- #

$(OBJ)/main.o : $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(OBJ)/main.o -c $(SRC)/main.c ${CPPFLAGS}

# -------------------- Color --------------------------- #

$(OBJ)/color_suppr.o : $(SRC)/color/color_suppr.c
	$(CC) $(CFLAGS) -o $(OBJ)/color_suppr.o -c $(SRC)/color/color_suppr.c ${CPPFLAGS}

# -------------------- Hough --------------------------- #

$(OBJ)/detection.o : $(SRC)/Hough/detection.c
	$(CC) $(CFLAGS) -o $(OBJ)/detection.o -c $(SRC)/Hough/detection.c ${CPPFLAGS}

$(OBJ)/segmentation.o : $(SRC)/Hough/segmentation.c
	$(CC) $(CFLAGS) -o $(OBJ)/segmentation.o -c $(SRC)/Hough/segmentation.c ${CPPFLAGS}

# -------------------- Network --------------------------- #

$(OBJ)/network.o : $(SRC)/network/network.c
	$(CC) $(CFLAGS) -o $(OBJ)/network.o -c $(SRC)/network/network.c ${CPPFLAGS}

$(OBJ)/load_sudoku.o : $(SRC)/network/load_sudoku.c
	$(CC) $(CFLAGS) -o $(OBJ)/load_sudoku.o -c $(SRC)/network/load_sudoku.c ${CPPFLAGS}

# -------------------- Solver --------------------------- #

$(OBJ)/loader.o : $(SRC)/solver/loader.c
	$(CC) $(CFLAGS) -o $(OBJ)/loader.o -c $(SRC)/solver/loader.c ${CPPFLAGS}

$(OBJ)/saver.o : $(SRC)/solver/saver.c
	$(CC) $(CFLAGS) -o $(OBJ)/saver.o -c $(SRC)/solver/saver.c ${CPPFLAGS}

$(OBJ)/solver.o : $(SRC)/solver/solver.c
	$(CC) $(CFLAGS) -o $(OBJ)/solver.o -c $(SRC)/solver/solver.c ${CPPFLAGS}

$(OBJ)/sudoku.o : $(SRC)/solver/sudoku.c
	$(CC) $(CFLAGS) -o $(OBJ)/sudoku.o -c $(SRC)/solver/sudoku.c ${CPPFLAGS}

# -------------------- Utils --------------------------- #

$(OBJ)/sdl_utils.o : $(SRC)/utils/sdl_utils.c
	$(CC) $(CFLAGS) -o $(OBJ)/sdl_utils.o -c $(SRC)/utils/sdl_utils.c ${CPPFLAGS}

# -------------------- Clear --------------------------- #	

clean : 
	- rm -r $(OBJ)/*.o
	- rm -r $(OBJ)/*.d
	- rm $(EXEC)
	- rm *.result
	- rm case/*.bmp
	- rm images/*.bmp
	- rm network_weight
	- rm tmp/*.bmp
	- rm src/network/PrintNetwork.txt