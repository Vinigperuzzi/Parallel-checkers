#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SQUARES_PER_ROW 8
#define TOTAL_SQUARES SQUARES_PER_ROW*SQUARES_PER_ROW

char whiteSquare[9] = "\033[47m";
char blackSquare[9] = "\033[40m";

typedef struct STPosition{
    int col;
    int row;
} Position;

enum PieceType {Standard, King};
enum PieceState {Dead, Alive};
enum PieceColor {White, Black};

typedef struct STPiece{
    int id;
    Position position;
    enum PieceColor color;
    enum PieceType type;
    enum PieceState state;
} Piece;

enum SquareState {Free, Occupied};

typedef struct STSquare{
    Position position;
    Piece piece;
    char color[9]; //(0 == brancas, 1 == negras)
    int id;
    enum SquareState state;
} Square;

typedef struct STBoard{
    Square square[TOTAL_SQUARES];
} Board;

typedef struct STMovement{
    Position origin;
    Position destiny;
} Movement;

typedef struct STPathInfo{
    int piecesInTheWay;
    int distance;
    enum PieceColor lastPieceColor; //Color of the last piece on the way
} PathInfo;

enum Adjacency {NotAdjacent, AdjacentFront, AdjacentBack};

enum MovementType {
    Invalid,
    Move,
    Attack          
};

Board createBoard(){
    Board board;

    for(int i = 0; i<SQUARES_PER_ROW; i++){
        for (int j = 0; j<SQUARES_PER_ROW; j++){
            board.square[(i*SQUARES_PER_ROW)+j].position.row = i;
            board.square[(i*SQUARES_PER_ROW)+j].position.col = j;
            board.square[(i*SQUARES_PER_ROW)+j].id = ((i*SQUARES_PER_ROW)+j);
            board.square[(SQUARES_PER_ROW*i)+j].piece.id = 0;
            board.square[(SQUARES_PER_ROW*i)+j].state = Free;
            if ((i+j)%2 == 0){
                strcpy(board.square[(i*SQUARES_PER_ROW)+j].color, blackSquare); //Brancas
            } else{
                strcpy(board.square[(i*SQUARES_PER_ROW)+j].color, whiteSquare);
            }
        }
    }
    return board;
}


char parseIntToChar(int valor){
    if(valor >= 0 && valor < SQUARES_PER_ROW)
        return 'A' + valor;
    else
        return '@';
}

int parseCharToInt(char character, int option){     //(0 to convert letters (a-h), 1 to convert numbers 0-7)
    if(option == 0){
        int charToInt = character - 'a';
        if(charToInt >=0 && charToInt < SQUARES_PER_ROW){
            return charToInt;
        } else {
            return -1;
        }
    } else{
        int charToInt = character - '0';
        if(charToInt >=0 && charToInt < SQUARES_PER_ROW){
            return charToInt;
        } else {
            return -1;
        }
    }
}

void printBoard(Board board, int option){ //(1 to print index, 2 to print square names)
    system("clear || cls");
    for(int i = SQUARES_PER_ROW - 1; i>=0; i--){
        printf("\t\t\t");
        for (int j = 0; j < SQUARES_PER_ROW; j++){
            printf("\033[0;32m");
            printf("%s", board.square[(i*SQUARES_PER_ROW)+j].color);
            if (option == 0){
                if (board.square[(SQUARES_PER_ROW*i)+j].state == 0){
                    printf("  ");
                } else {
                    if (board.square[(i*SQUARES_PER_ROW)+j].piece.color == 0){
                        printf("🪲 ");
                    } else {
                        printf("🐞");
                    }
                }
            } else if (option == 1){
                printf("%d%d", board.square[(i*SQUARES_PER_ROW)+j].position.row, board.square[(i*SQUARES_PER_ROW)+j].position.col);
            } else {
                printf("%c%d", parseIntToChar(board.square[(i*SQUARES_PER_ROW)+j].position.col), board.square[(i*SQUARES_PER_ROW)+j].position.row + 1);
            }
            printf("\033[0;37m");
        }
        printf("\n");
    }
}

Board setPieces(Board board){
    int id = 1;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < SQUARES_PER_ROW; j++){
            if (strcmp(board.square[(i*SQUARES_PER_ROW)+j].color, blackSquare) == 0){
                board.square[(i*SQUARES_PER_ROW)+j].state = Occupied;
                board.square[(i*SQUARES_PER_ROW)+j].piece.color = 0;
                board.square[(i*SQUARES_PER_ROW)+j].piece.id = id++;
                board.square[(i*SQUARES_PER_ROW)+j].piece.state = Alive;
                board.square[(i*SQUARES_PER_ROW)+j].piece.type = Standard;
                board.square[(i*SQUARES_PER_ROW)+j].piece.position.col = board.square[(i*SQUARES_PER_ROW)+j].position.col;
                board.square[(i*SQUARES_PER_ROW)+j].piece.position.row = board.square[(i*SQUARES_PER_ROW)+j].position.row;
            }
        }
    }
    for (int i = SQUARES_PER_ROW - 3; i < SQUARES_PER_ROW; i++){
        for (int j = 0; j < SQUARES_PER_ROW; j++){
            if (strcmp(board.square[(i*SQUARES_PER_ROW)+j].color, blackSquare) == 0){
                board.square[(i*SQUARES_PER_ROW)+j].state = Occupied;
                board.square[(i*SQUARES_PER_ROW)+j].piece.color = 1;
                board.square[(i*SQUARES_PER_ROW)+j].piece.id = id++;
                board.square[(i*SQUARES_PER_ROW)+j].piece.state = Alive;
                board.square[(i*SQUARES_PER_ROW)+j].piece.type = Standard;
                board.square[(i*SQUARES_PER_ROW)+j].piece.position.col = board.square[(i*SQUARES_PER_ROW)+j].position.col;
                board.square[(i*SQUARES_PER_ROW)+j].piece.position.row = board.square[(i*SQUARES_PER_ROW)+j].position.row;
            }
        }
    }
    return board;
}

Board movePiece(Board board, Movement move){
    Piece piece;
    int index;

    index = move.origin.row * SQUARES_PER_ROW + move.origin.col;
    piece = board.square[index].piece;
    board.square[index].piece.id = 0;
    board.square[index].state = Free;

    index = move.destiny.row * SQUARES_PER_ROW + move.destiny.col;
    board.square[index].piece = piece;
    board.square[index].state = Occupied;

    return board;
}

Movement convertInput(char iniCol, char iniRow, char finCol, char finRow, int *valid){
    Movement move;
    *valid = 1;
    int parsedRow, parsedCol;

    parsedCol = parseCharToInt(iniCol, 0);
    if(parsedCol >= 0){
        move.origin.col = parsedCol;
    } else {
        *valid = 0;
        return move;
    }

    parsedCol = parseCharToInt(finCol, 0);
    if(parsedCol >= 0){
        move.destiny.col = parsedCol;
    } else {
        *valid = 0;
        return move;
    }
    
    parsedRow = parseCharToInt(iniRow, 1);
    if(parsedRow >= 0){
        move.origin.row = parsedRow;
    } else {
        *valid = 0;
        return move;
    }

    parsedRow = parseCharToInt(finRow, 1);
    if(parsedRow >= 0){
        move.destiny.row = parsedRow;
    } else {
        *valid = 0;
        return move;
    }

    return move;
}

Movement getMovementFromUser(){
    char input[10];
    Movement move;
    int valid = 0;

    do{
        printf("\n\n\t\t\tWhat move would you like to make? I.E.(a1 b2): ");
        scanf("%[^\n]%*c", input);

        move = convertInput(input[0], input[1], input[3], input[4], &valid);

        if(valid){
            return move;
        } else {
            printf("\n\t\t\tInvalid input! Try something like \"a1 b2\"");
        }
    }while(valid != 1);
}

int hasPiece(Board board, Position position){
    return board.square[position.row * SQUARES_PER_ROW + position.col].state;
}

enum Adjacency isAdjacent(Position origin, Position destiny){     
    if(origin.row + 1 == destiny.row){   // Is one row above    
        if(origin.col + 1 == destiny.col || origin.col - 1 == destiny.col){     // Is adjacent to the front
            return AdjacentFront;
        } else {    // Is not adjacent
            return NotAdjacent;
        }
    } else if(origin.row - 1 == destiny.row){   // Is one row behind
        if(origin.col + 1 == destiny.col || origin.col - 1 == destiny.col){     // Is adjacent to the back
            return AdjacentBack; 
        } else {
            return NotAdjacent;
        }
    } else {
        return NotAdjacent;
    }
}

Piece getPiece(Board board, Position position){
    return board.square[position.row * SQUARES_PER_ROW + position.col].piece;
}

PathInfo getPathInfo(Board board, Position origin, Position destiny){
    PathInfo pathInfo;
    int piecesInTheWay = 0;
    enum PieceColor lastPieceColor;
    int distanceMovedRow = origin.row - destiny.row;
    int distanceMovedCol = origin.col - destiny.col;

    Position position = origin;
    Piece piece;
    
    if(distanceMovedRow > 0){
        if(distanceMovedCol > 0){
            for(size_t i = 1; i < abs(distanceMovedRow); i++){
                position.row--;
                position.col--;
                if(hasPiece(board, position)){
                    piece = getPiece(board, position);
                    piecesInTheWay++;
                    lastPieceColor = piece.color;
                }
            }
        } else {
            for(size_t i = 1; i < abs(distanceMovedRow); i++){
                position.row--;
                position.col++;
                if(hasPiece(board, position)){
                    piece = getPiece(board, position);
                    piecesInTheWay++;
                    lastPieceColor = piece.color;
                }
            }
        }
    } else {
        if(distanceMovedCol > 0){
            for(size_t i = 1; i < abs(distanceMovedRow); i++){
                position.row++;
                position.col--;
                if(hasPiece(board, position)){
                    piece = getPiece(board, position);
                    piecesInTheWay++;
                    lastPieceColor = piece.color;
                }
            }
        } else {
            for(size_t i = 1; i < abs(distanceMovedRow); i++){
                position.row++;
                position.col++;
                if(hasPiece(board, position)){
                    piece = getPiece(board, position);
                    piecesInTheWay++;
                    lastPieceColor = piece.color;
                }
            }
        }
    }

    pathInfo.distance = abs(distanceMovedRow);
    pathInfo.lastPieceColor = lastPieceColor;
    pathInfo.piecesInTheWay = piecesInTheWay;

    return pathInfo;
}

int isDiagonal(Position origin, Position destiny){
    return abs(origin.row - destiny.row) == abs(origin.col - destiny.col);
}

enum MovementType checkMovement(Board board, Position origin, Position destiny){
    if(!(hasPiece(board, origin) && !hasPiece(board, destiny) && isDiagonal(origin, destiny))){
        return Invalid;
    }

    Piece pieceMoving = getPiece(board, origin);
    PathInfo pathInfo = getPathInfo(board, origin, destiny);

    switch (pieceMoving.type)
    {
    case Standard:
        if(pathInfo.distance > 1){
            if(pathInfo.piecesInTheWay == 1 && pathInfo.lastPieceColor == !pieceMoving.color){  // Exists a piece in the way of the enemy Color
                    return Attack;
            } else {        // Tryng to attack friendly color or trying to move more than 1 house using a Standard piece
                    return Invalid;
            }
        } else {
            return Move;
        }
        break;
    case King:
        if(pathInfo.piecesInTheWay == 1){
            if(pathInfo.lastPieceColor == !pieceMoving.color){   // Exists a piece in the way of the enemy Color
                return Attack;
            } else {            // Tryng to attack friendly color
                return Invalid;
            }
        } else if(pathInfo.piecesInTheWay == 0){    //No pieces on the way, cause its the King this piece is free to move
            return Move;
        } else {        // Last scenario: more than 1 piece in the way, it cant move
            return Invalid;
        }
        break;
    default:
        break;
    }
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
    printBoard(board, 0);

    // while(ini.row != 9){
    //     printf("\n\n\t\t\tQual movimento você gostaria de fazer I.E.(a1 b2): ");
    //     scanf("%1d%1d %1d%1d", &ini.row, &ini.col, &fin.row, &fin.col);
    //     board = movePiece(board, ini, fin);
    //     printBoard(board, 0);
    //     printf("\n\n\t\t\tA ideia é movimentar %d%d para %d%d\n\n", ini.row, ini.col, fin.row, fin.col);
    // }

    Movement move;

    while(1){
        move = getMovementFromUser();
        if(checkMovement(board, move.origin, move.destiny) != Invalid){
            board = movePiece(board, move);
            printBoard(board, 0);
        } else {
            printf("\n\t\t\tThis movement is not allowed. Try another one.");
        }
    }
}