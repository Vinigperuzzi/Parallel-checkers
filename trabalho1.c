#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 64;

typedef struct STPosition{
    int col;
    int row;
} Position;

typedef struct STPiece{
    int color; //(0 == brancas, 1 == negras)
    int id;
    Position position;
} Piece;

typedef struct STCasa{
    Position position;
    Piece piece;
    char color[9]; //(0 == brancas, 1 == negras)
    int id;
} Casa;

typedef struct STBoard{
    Casa casa[64];
} Board;


Board createBoard(){
    Board board;

    for(int i = 0; i<8; i++){
        for (int j = 0; j<8; j++){
            board.casa[(i*8)+j].position.row = i;
            board.casa[(i*8)+j].position.col = j;
            board.casa[(i*8)+j].id = ((i*8)+j);
            if ((i+j)%2 == 0){
                strcpy(board.casa[(i*8)+j].color, "\033[40m"); //Brancas
            } else{
                strcpy(board.casa[(i*8)+j].color, "\033[47m");
            }
        }
    }
    return board;
}

void printBoard(Board board){
    system("clear || cls");
    for(int i = 7; i>=0; i--){
        printf("\t\t\t");
        for (int j = 0; j < 8; j++){
            printf("\033[0;32m");
            printf(board.casa[(i*8)+j].color);
            printf("%d%d", board.casa[(i*8)+j].position.row, board.casa[(i*8)+j].position.col);
            printf("\033[0;37m");
        }
        printf("\n");
    }
}


int main (void){
    Board board = createBoard();
    printBoard(board);
}