//////////////// -- INCLUDE's -- ////////////////

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//////////////// -- DEFINE's -- ////////////////

#define SQUARES_PER_ROW 8
#define TOTAL_SQUARES SQUARES_PER_ROW * SQUARES_PER_ROW
#define PIECES_PER_PLAYER 12
#define TOTAL_PIECES PIECES_PER_PLAYER * 2

//////////////// -- DATA TYPE's -- ////////////////

typedef struct STPosition{
    int col;
    int row;
} Position;

enum PieceType {Checker, King};
enum PieceColor {White, Black};

typedef struct STPiece{
    int id;
    Position position;
    enum PieceColor color;
    enum PieceType type;
} Piece;

enum SquareState {Free, Occupied};

typedef struct STSquare{
    Position position;
    Piece piece;
    char color[9]; //(0 == white, 1 == black)
    int id;
    enum SquareState state;
} Square;

typedef struct STBoard{
    Square square[TOTAL_SQUARES];
    int whitePieces;
    int blackPieces;
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

//////////////// -- GLOBAL VAR's -- ////////////////

char whiteSquare[9] = "\033[47m";
char blackSquare[9] = "\033[40m";
enum PieceColor turn = White;

//////////////// -- FUNCTION's -- ////////////////

int getIndexOfPosition(Position position){
    return position.row * SQUARES_PER_ROW + position.col;
}

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
                strcpy(board.square[(i*SQUARES_PER_ROW)+j].color, blackSquare); //White
            } else{
                strcpy(board.square[(i*SQUARES_PER_ROW)+j].color, whiteSquare);
            }
        }
    }

    board.blackPieces = PIECES_PER_PLAYER;
    board.whitePieces = PIECES_PER_PLAYER;
    return board;
}


char parseIntToChar(int valor){
    if(valor >= 0 && valor < SQUARES_PER_ROW){
        return 'A' + valor;
    } else {
        return '@';
    }
}

int parseCharToInt(char character, int option){     //(0 to convert letters (a-z), 1 to convert numbers 0-9)
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
                        if(board.square[(i*SQUARES_PER_ROW)+j].piece.type == Checker){
                            printf("🪲 ");
                        } else{
                            printf("🐛");
                        }
                    } else {
                        if(board.square[(i*SQUARES_PER_ROW)+j].piece.type == Checker){
                            printf("🐞");
                        } else{
                            printf("🐙");
                        }
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
                board.square[(i*SQUARES_PER_ROW)+j].piece.type = Checker;
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
                board.square[(i*SQUARES_PER_ROW)+j].piece.type = Checker;
                board.square[(i*SQUARES_PER_ROW)+j].piece.position.col = board.square[(i*SQUARES_PER_ROW)+j].position.col;
                board.square[(i*SQUARES_PER_ROW)+j].piece.position.row = board.square[(i*SQUARES_PER_ROW)+j].position.row;
            }
        }
    }
    return board;
}

int checkKingTurningCondition(Board board, Position destiny, Piece piece){
    if(piece.type == King){
        return 0;
    }

    if(destiny.row == SQUARES_PER_ROW - 1 && piece.color == White){
        return 1;
    } else if(destiny.row == 0 && piece.color == Black){
        return 1;
    } else {
        return 0;
    }
}

Board turnKing(Board board, Piece piece){
    int index = getIndexOfPosition(piece.position);
    board.square[index].piece.type = King;

    return board;
}

void swapTurn(){
    if(turn){
        turn = White;
    } else {
        turn = Black;
    }
}

Board movePiece(Board board, Movement move){
    Piece pieceAux;
    int index;

    index = getIndexOfPosition(move.origin);
    pieceAux = board.square[index].piece;
    board.square[index].piece.id = 0;
    board.square[index].state = Free;

    index = getIndexOfPosition(move.destiny);
    board.square[index].piece = pieceAux;
    board.square[index].state = Occupied;

    if(checkKingTurningCondition(board, move.destiny, pieceAux)){
        board = turnKing(board, pieceAux);
    }

    swapTurn();
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
    return board.square[getIndexOfPosition(position)].state;
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
    return board.square[getIndexOfPosition(position)].piece;
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

enum MovementType checkMovement(Board board, Movement move){

    if(!(hasPiece(board, move.origin) &&                                    //checks if the origin position has a piece,
    !hasPiece(board, move.destiny) &&                                       //if the destiy position is free
    isDiagonal(move.origin, move.destiny) &&                                //if the destiny position is diagonal in relation to destiny's
    turn == board.square[getIndexOfPosition(move.origin)].piece.color)){    //and if is the turn of the piece in origin to play
        return Invalid;
    }

    Piece pieceMoving = getPiece(board, move.origin);
    PathInfo pathInfo = getPathInfo(board, move.origin, move.destiny);

    switch (pieceMoving.type)
    {
    case Checker:
        if(pathInfo.distance > 1){
            if(pathInfo.piecesInTheWay == 1 && pathInfo.lastPieceColor == !pieceMoving.color){  // Exists a piece in the way of the enemy Color
                    return Attack;
            } else {        // Tryng to attack friendly color or trying to move more than 1 house using a Checker piece
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

Board killPiece(Board board, Piece piece){
    int index = getIndexOfPosition(piece.position);
    board.square[index].piece.id = 0;
    board.square[index].state = Free;

    if(piece.color == White){
        board.whitePieces--;
    } else {
        board.blackPieces--;
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
        if(checkMovement(board, move) != Invalid){
            board = movePiece(board, move);
            printBoard(board, 0);
        } else {
            printf("\n\t\t\tThis movement is not allowed. Try another one.");
        }
    }
}