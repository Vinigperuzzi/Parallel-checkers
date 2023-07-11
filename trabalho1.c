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

char parseIntToChar(int valor){
    switch(valor){
        case 0:
            return 'A';
        case 1:
            return 'B';
        case 2:
           return 'C';
        case 3:
            return 'D';
        case 4:
            return 'E';
        case 5:
            return 'F';
        case 6:
            return 'G';
        case 7:
            return 'H';
        default:
            return 'Z';
        }
}

void printBoard(Board board, int option){ //(1 to print index, 2 to print square names)
    system("clear || cls");
    for(int i = 7; i>=0; i--){
        printf("\t\t\t");
        for (int j = 0; j < 8; j++){
            printf("\033[0;32m");
            printf("%s", board.casa[(i*8)+j].color);
            if (option == 1){
                printf("%d%d", board.casa[(i*8)+j].position.row, board.casa[(i*8)+j].position.col);
            } else {
                printf("%c%d", parseIntToChar(board.casa[(i*8)+j].position.col), board.casa[(i*8)+j].position.row + 1);
            }
            printf("\033[0;37m");
        }
        printf("\n");
    }
}


int main (void){
    Board board = createBoard();
    printBoard(board, 1);
    getchar();
    printBoard(board, 2);
}