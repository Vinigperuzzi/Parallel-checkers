#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 64;
char whiteSquare[9] = "\033[47m";
char blackSquare[9] = "\033[40m";

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
            board.casa[(8*i)+j].piece.id = 0;
            if ((i+j)%2 == 0){
                strcpy(board.casa[(i*8)+j].color, blackSquare); //Brancas
            } else{
                strcpy(board.casa[(i*8)+j].color, whiteSquare);
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
            if (option == 0){
                if (board.casa[(8*i)+j].piece.id == 0){
                    printf("  ");
                } else {
                    if (board.casa[(i*8)+j].piece.color == 0){
                        printf("🪲");
                    } else {
                        printf("🐞");
                    }
                }
            } else if (option == 1){
                printf("%d%d", board.casa[(i*8)+j].position.row, board.casa[(i*8)+j].position.col);
            } else {
                printf("%c%d", parseIntToChar(board.casa[(i*8)+j].position.col), board.casa[(i*8)+j].position.row + 1);
            }
            printf("\033[0;37m");
        }
        printf("\n");
    }
}

Board setPieces(Board board){
    int id = 1;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 8; j++){
            if (strcmp(board.casa[(i*8)+j].color, blackSquare) == 0){
                board.casa[(i*8)+j].piece.color = 0;
                board.casa[(i*8)+j].piece.id = id++;
                board.casa[(i*8)+j].piece.position.col = board.casa[(i*8)+j].position.col;
                board.casa[(i*8)+j].piece.position.row = board.casa[(i*8)+j].position.row;
            }
        }
    }
    for (int i = 5; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (strcmp(board.casa[(i*8)+j].color, blackSquare) == 0){
                board.casa[(i*8)+j].piece.color = 1;
                board.casa[(i*8)+j].piece.id = id++;
                board.casa[(i*8)+j].piece.position.col = board.casa[(i*8)+j].position.col;
                board.casa[(i*8)+j].piece.position.row = board.casa[(i*8)+j].position.row;
            }
        }
    }
    return board;
}

Board movePiece(Board board, Position posIni, Position posFin){
    Piece piece;
    for(int i = 0; i < 64; i++){
        if (board.casa[i].position.col == posIni.col && board.casa[i].position.row == posIni.row){
            piece = board.casa[i].piece;
            board.casa[i].piece.id = 0;
        }
        if (board.casa[i].position.col == posFin.col && board.casa[i].position.row == posFin.row){
            board.casa[i].piece = piece;
        }
    }
    return board;
}


int main (void){
    Board board = createBoard();
    board = setPieces(board);
    printBoard(board, 1);
    getchar();
    printBoard(board, 2);
    getchar();
    printBoard(board, 0);
    getchar();
    Position ini, fin;
    ini.col = 2; ini.row = 2; fin.row = 3; fin.col = 1;
    board = movePiece(board, ini, fin);
    printBoard(board, 0);

    while(ini.row != 9){
        printf("\n\n\t\t\tQual movimento você gostaria de fazer I.E.(a1 b2): ");
        scanf("%1d%1d %1d%1d", &ini.row, &ini.col, &fin.row, &fin.col);
        board = movePiece(board, ini, fin);
        printBoard(board, 0);
        printf("\n\n\t\t\tA ideia é movimentar %d%d para %d%d\n\n", ini.row, ini.col, fin.row, fin.col);
    }
}