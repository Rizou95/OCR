#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "../utils/sdl_utils.h"
#include "load_sudoku.h"
#include <err.h>

double sigmoid(double x) {return 1 / (1 + exp(-x)); }
double sigmoidPrime(double x) {return x * (1-x); }

#define nbInputs 784
#define nbHiddenNodes1 38
#define nbHiddenNodes2 38
#define nbOutputs 10

double hiddenWeights1[nbInputs][nbHiddenNodes1];
double hiddenWeights2[nbHiddenNodes1][nbHiddenNodes2];
double outputWeights[nbHiddenNodes2][nbOutputs];

double hiddenNode1[nbHiddenNodes1];
double hiddenNode2[nbHiddenNodes2];
double outputNode[nbOutputs];

int nbTraining = 1000;
//double trainingInputs[10][784] = { {0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f} }; //les entreés
double trainingInputs[1][784];
double trainingOutputs[10][1] = {{0},{1},{2},{3},{4},{5},{6},{7},{8},{9}}; //  les resultats respectivement aux entrées

int findmax(double tableau[])
{
    double temp = tableau[0];
    int res = 0;
    for (int i = 1; i < 10;i++)
    {
        if(temp < tableau[i])
        {
            temp = tableau[i];
            res = i;
        }
    }    
    return res;
    
}
void findpixel(SDL_Surface *image, double table[1][784])
{
    Uint32 pixel;
    Uint8 r, g, b;
    int width = image->w;
    int height = image->h;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);

            if(r+g+b < 200)
                table[0][y + x * 10] = 1;
            else
                table[0][y + x * 10] = 0;
        }
    }
}

double randWeight() { 
    return (((double)rand())/((double)RAND_MAX)*2)-1; 
}

int initWeight() { 
    for(int i=0; i<nbInputs; i++){
        for(int j=0; j< nbHiddenNodes1; j++){
            hiddenWeights1[i][j] = 0;
        }
    }
    for(int i=0; i<nbHiddenNodes1; i++){
        for(int j=0; j<nbHiddenNodes2; j++){
            hiddenWeights2[i][j] = 0;
        }
    }
    for(int i=0; i<nbHiddenNodes2; i++){
        for(int j=0; j<nbOutputs; j++){
            outputWeights[i][j] = 0;
        }
    }
    return 0;
}

int Training(){
    //initWeight();
    for (int i = 0; i < nbTraining; i++){
        char buffer[100];
        sprintf(buffer, "./src/numbers/%d.jpg",i%10);
        SDL_Surface* chiffre = load_image(buffer);
        findpixel((chiffre),trainingInputs);
        for(int j=0; j<nbHiddenNodes1; j++){
            float temp = 0;
            for(int k=0; k<nbInputs; k++){
                temp += trainingInputs[0][k]*hiddenWeights1[k][j];
            }
            hiddenNode1[j] = sigmoid(temp);
        }
        for(int j=0; j<nbHiddenNodes2; j++){
            float temp = 0;
            for(int k=0; k<nbHiddenNodes1; k++){
                temp += hiddenNode1[k]*hiddenWeights2[k][j];
            }
            hiddenNode2[j] = sigmoid(temp);
            }

        for(int j=0; j<nbOutputs; j++){
            float temp = 0;
            for(int k=0; k<nbHiddenNodes2; k++){
                temp += hiddenNode2[k]*outputWeights[k][j];
            }
            outputNode[j] = sigmoid(temp);
        }
        
        double deltaOutput[nbOutputs];
        for (int j=0; j<nbOutputs; j++) {
            double dError = (trainingOutputs[0][0]-outputNode[j]);
            deltaOutput[j] = dError*sigmoidPrime(outputNode[j]);
            //printf("Input : %d %d -> %f but needed %f\n", (int)trainingInputs[i%10][0],(int)trainingInputs[i%10][1] ,outputNode[j], trainingOutputs[i%10][j]);
        }
        double deltaHidden2[nbHiddenNodes2];
        for(int j=0; j<nbHiddenNodes2; j++){
            double dError = 0;
            for(int k=0; k<nbOutputs; k++) {
                dError+=deltaOutput[k]*outputWeights[j][k];
            }
            deltaHidden2[j] = dError*sigmoidPrime(hiddenNode2[j]);
        }
        double deltaHidden1[nbHiddenNodes1];
        for (int j=0; j<nbHiddenNodes1; j++) {
            double dError = 0;
            for(int k=0; k<nbOutputs; k++) {
                dError+=deltaHidden2[k]*outputWeights[j][k];
            }
            deltaHidden1[j] = dError*sigmoidPrime(hiddenNode1[j]);
        }
        for (int j=0; j<nbOutputs; j++) {
            for (int k=0; k<nbHiddenNodes2; k++) {
                outputWeights[k][j]+=hiddenNode2[k]*deltaOutput[j];
            }
        }
        for (int j=0; j<nbHiddenNodes2; j++) {
            for(int k=0; k<nbHiddenNodes1; k++) {
                hiddenWeights2[k][j] += hiddenNode1[k]*deltaHidden2[j];
            }
        }
        for (int j=0; j<nbHiddenNodes1; j++) {
            for(int k=0; k<nbInputs; k++) {
                hiddenWeights1[k][j] += trainingInputs[0][k]*deltaHidden1[j];
            }
        }
        SDL_FreeSurface(chiffre);
    }
    return 0;
}


void Result(int result[9][9])
{
    //initWeight();
    int x = 0;
    int y = 0;

    for (int i = 1; i < 82; i++){
        char buffer[100];
        sprintf(buffer, "./case/%d.bmp",i);
        findpixel(load_image(buffer),trainingInputs);
        for(int j=0; j<nbHiddenNodes1; j++){
            double temp = 0;
            for(int k=0; k<nbInputs; k++){
                temp += trainingInputs[0][k]*hiddenWeights1[k][j];
            }
            hiddenNode1[j] = sigmoid(temp);
        }
        for(int j=0; j<nbHiddenNodes2; j++){
            double temp = 0;
            for(int k=0; k<nbHiddenNodes1; k++){
                temp += hiddenNode1[k]*hiddenWeights2[k][j];
            }
            hiddenNode2[j] = sigmoid(temp);
            }

        for(int j=0; j<nbOutputs; j++){
            double temp = 0;
            for(int k=0; k<nbHiddenNodes2; k++){
                temp += hiddenNode2[k]*outputWeights[k][j];
            }
            outputNode[j] = sigmoid(temp);
        }
        if (i != 0 && i % 9 == 0){
            x = 0;
            y++;
        }
        else{
            x++;
        }
        result[x][y] = findmax(outputNode);
    }
}

void PrintNetwork()
{
    FILE* file_ptr = fopen("./src/network/PrintNetwork.txt", "w");
    for (int i = 0; i < 784 ; i++){
	    for (int j = 0 ; j<38;j++){
		    fprintf(file_ptr,"hiddenWeights1[%d][%d] = %f\n",i,j,hiddenWeights1[i][j]);
	    }
    }
    for (int i = 0; i < 38 ; i++){
	    for (int j = 0 ; j< 38;j++){
		    fprintf(file_ptr,"hiddenWeights2[%d][%d] = %f\n",i,j,hiddenWeights2[i][j]);
	    }
    }
    for (int i = 0; i < 38 ; i++){
            for (int j = 0 ; j< 10;j++){
                fprintf(file_ptr,"hiddenOutputs[%d][%d] = %f\n",i,j,outputWeights[i][j]);
            }
    }
    fclose(file_ptr);
}

void SaveNetwork()
{
    FILE* file_ptr = fopen("./src/network/SaveNetwork.txt", "w");
    for (int i = 0; i < 784 ; i++){
	    for (int j = 0 ; j< 38;j++){
		    fprintf(file_ptr,"%f\n",hiddenWeights1[i][j]);
	    }
    }
    for (int i = 0; i < 38 ; i++){
	    for (int j = 0 ; j< 38;j++){
		    fprintf(file_ptr,"%f\n",hiddenWeights2[i][j]);
	    }
    }
    for (int i = 0; i < 38 ; i++){
        for (int j = 0 ; j< 10;j++){
            fprintf(file_ptr,"%f\n",outputWeights[i][j]);
        }
    }
    fclose(file_ptr);
}

void LoadNetwork()
{
    FILE* file_ptr = fopen("./src/network/SaveNetwork.txt","r");
    char chaine[1000] = "";
    for (int i = 0; i < 784 ; i++){
	    for (int j = 0 ; j< 38;j++){
            if (fgets(chaine,1000,file_ptr) != NULL)
		        hiddenWeights1[i][j] = atof(chaine);
	    }
    }
    for (int i = 0; i < 38 ; i++){
	    for (int j = 0 ; j< 38;j++){
            if (fgets(chaine,1000,file_ptr) != NULL)
		        hiddenWeights2[i][j] = atof(chaine);
	    }
    }
    for (int i = 0; i < 38 ; i++){
         for (int j = 0 ; j< 10;j++){
            if (fgets(chaine,1000,file_ptr) != NULL)
                outputWeights[i][j] = atof(chaine);
        }
    }
    fclose(file_ptr);
}
    


int main_network(int sudoku[9][9], int image)
{
    load_sudoku(sudoku, image);
    //LoadNetwork();
    //Result(sudoku);
    //SaveNetwork();
    //PrintNetwork();
    return 0;
}

int training()
{
    LoadNetwork();
    for (int i = 0; i < 1; i++) {
        Training();
    }
    SaveNetwork();
    PrintNetwork();
    return 0;
}