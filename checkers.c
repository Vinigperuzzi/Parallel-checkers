//////////////// -- INCLUDE's -- ////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////// -- DEFINE's -- ////////////////

#define SQUARES_PER_ROW 8
#define TOTAL_SQUARES SQUARES_PER_ROW * SQUARES_PER_ROW
#define PIECES_PER_PLAYER 12
#define TOTAL_PIECES PIECES_PER_PLAYER * 2

//////////////// -- DATA TYPE's -- ////////////////

typedef struct STPosition
{
    int col;
    int row;
} Position;

enum PieceType
{
    Checker,
    King
};
enum PieceColor
{
    White,
    Black
};

typedef struct STPiece
{
    int id;
    Position position;
    enum PieceColor color;
    enum PieceType type;
} Piece;

enum SquareState
{
    Free,
    Occupied
};

typedef struct STSquare
{
    Position position;
    Piece piece;
    char color[9];
    int id;
    enum SquareState state;
} Square;

typedef struct STBoard
{
    Square square[TOTAL_SQUARES];
    int whitePieces;
    int blackPieces;
} Board;

typedef struct STMovement
{
    Position origin;
    Position destiny;
} Movement;

typedef struct STMovementSequence
{
    Movement seqMovements[12];
    int numberOfMovements;
} MovementSequence;

typedef struct STPathInfo
{
    int piecesInTheWay;
    int distance;
    Piece lastPieceInTheWay;
} PathInfo;

enum Adjacency
{
    NotAdjacent,
    AdjacentFront,
    AdjacentBack
};

enum MovementType
{
    Invalid,
    Move,
    Attack
};

enum Winner
{
    NoOne,
    WhiteWon,
    BlackWon
};

//////////////// -- GLOBAL VAR's -- ////////////////

char whiteSquare[9] = "\033[47m";
char blackSquare[9] = "\033[40m";
enum PieceColor turn = White;

//////////////// -- STACK IMPL -- ////////////////

#define MAX_SIZE PIECES_PER_PLAYER

typedef struct STIdStack
{
    int id[MAX_SIZE];
    int top;
    int size;
} IdStack;

void initializeIdStack(IdStack *stack)
{
    stack->top = -1;
    stack->size = 0;
}

int idStackIsEmpty(IdStack *stack)
{
    return stack->top == -1;
}

int idStackIsFull(IdStack *stack)
{
    return stack->top == MAX_SIZE - 1;
}

void pushIdStack(IdStack *stack, int value)
{
    stack->top++;
    stack->id[stack->top] = value;
    stack->size++;
}

void popIdStack(IdStack *stack)
{
    stack->top--;
    stack->size--;
}

int peekIdStack(IdStack *stack)
{
    if (idStackIsEmpty(stack))
    {
        return -1;
    }
    else
    {
        return stack->id[stack->top];
    }
}

int hasId(IdStack *stack, int value)
{
    for (size_t i = 0; i < stack->size; ++i)
    {
        if (stack->id[i] == value)
        {
            return 1;
        }
    }

    return 0;
}

//////////////// -- FUNCTION's -- ////////////////

int getIndexOfPosition(Position position)
{
    return position.row * SQUARES_PER_ROW + position.col;
}

Board createBoard()
{
    Board board;

    for (int i = 0; i < SQUARES_PER_ROW; i++)
    {
        for (int j = 0; j < SQUARES_PER_ROW; j++)
        {
            board.square[(i * SQUARES_PER_ROW) + j].position.row = i;
            board.square[(i * SQUARES_PER_ROW) + j].position.col = j;
            board.square[(i * SQUARES_PER_ROW) + j].id = ((i * SQUARES_PER_ROW) + j);
            board.square[(SQUARES_PER_ROW * i) + j].piece.id = 0;
            board.square[(SQUARES_PER_ROW * i) + j].state = Free;
            if ((i + j) % 2 == 0)
            {
                strcpy(board.square[(i * SQUARES_PER_ROW) + j].color, blackSquare); // White
            }
            else
            {
                strcpy(board.square[(i * SQUARES_PER_ROW) + j].color, whiteSquare);
            }
        }
    }

    board.blackPieces = PIECES_PER_PLAYER;
    board.whitePieces = PIECES_PER_PLAYER;
    return board;
}

char parseIntToChar(int valor)
{
    if (valor >= 0 && valor < SQUARES_PER_ROW)
    {
        return 'A' + valor;
    }
    else
    {
        return '@';
    }
}

int parseCharToInt(char character, int option)
{ //(0 to convert letters (a-z), 1 to convert numbers 0-9)
    if (option == 0)
    {
        int charToInt = character - 'a';
        if (charToInt >= 0 && charToInt < SQUARES_PER_ROW)
        {
            return charToInt;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        int charToInt = character - '0';
        if (charToInt >= 0 && charToInt < SQUARES_PER_ROW)
        {
            return charToInt;
        }
        else
        {
            return -1;
        }
    }
}

void printBoard(Board *board, int option)
{ //(1 to print index, 2 to print square names, 3 to print index)
    system("clear || cls");
    for (int i = SQUARES_PER_ROW - 1; i >= 0; i--)
    {
        printf("\t\t\t%d ", i);
        for (int j = 0; j < SQUARES_PER_ROW; j++)
        {
            printf("\033[0;32m");
            printf("%s", board->square[(i * SQUARES_PER_ROW) + j].color);
            if (option == 0)
            {
                if (board->square[(SQUARES_PER_ROW * i) + j].state == 0)
                {
                    printf("  ");
                }
                else
                {
                    if (board->square[(i * SQUARES_PER_ROW) + j].piece.color == 0)
                    {
                        if (board->square[(i * SQUARES_PER_ROW) + j].piece.type == Checker)
                        {
                            printf("🪲 ");
                        }
                        else
                        {
                            printf("🐛");
                        }
                    }
                    else
                    {
                        if (board->square[(i * SQUARES_PER_ROW) + j].piece.type == Checker)
                        {
                            printf("🐞");
                        }
                        else
                        {
                            printf("🐙");
                        }
                    }
                }
            }
            else if (option == 1)
            {
                printf("%d%d", board->square[(i * SQUARES_PER_ROW) + j].position.row, board->square[(i * SQUARES_PER_ROW) + j].position.col);
            }
            else if (option == 3)
            {
                if ((i * SQUARES_PER_ROW + j) < 10)
                {
                    printf("0%d", (i * SQUARES_PER_ROW + j));
                }
                else
                {
                    printf("%d", (i * SQUARES_PER_ROW + j));
                }
            }
            else
            {
                printf("%c%d", parseIntToChar(board->square[(i * SQUARES_PER_ROW) + j].position.col), board->square[(i * SQUARES_PER_ROW) + j].position.row + 1);
            }
            printf("\033[0;37m");
        }
        printf("\n");
    }
    printf("\t\t\t   ");
    for (size_t i = 0; i < SQUARES_PER_ROW; i++)
    {
        printf("%c ", parseIntToChar(i));
    }
    printf("\n");
}

void setPieces(Board *board)
{
    int id = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < SQUARES_PER_ROW; j++)
        {
            if (strcmp(board->square[(i * SQUARES_PER_ROW) + j].color, blackSquare) == 0)
            {
                board->square[(i * SQUARES_PER_ROW) + j].state = Occupied;
                board->square[(i * SQUARES_PER_ROW) + j].piece.color = 0;
                board->square[(i * SQUARES_PER_ROW) + j].piece.id = id++;
                board->square[(i * SQUARES_PER_ROW) + j].piece.type = Checker;
                board->square[(i * SQUARES_PER_ROW) + j].piece.position.col = board->square[(i * SQUARES_PER_ROW) + j].position.col;
                board->square[(i * SQUARES_PER_ROW) + j].piece.position.row = board->square[(i * SQUARES_PER_ROW) + j].position.row;
            }
        }
    }
    for (int i = SQUARES_PER_ROW - 3; i < SQUARES_PER_ROW; i++)
    {
        for (int j = 0; j < SQUARES_PER_ROW; j++)
        {
            if (strcmp(board->square[(i * SQUARES_PER_ROW) + j].color, blackSquare) == 0)
            {
                board->square[(i * SQUARES_PER_ROW) + j].state = Occupied;
                board->square[(i * SQUARES_PER_ROW) + j].piece.color = 1;
                board->square[(i * SQUARES_PER_ROW) + j].piece.id = id++;
                board->square[(i * SQUARES_PER_ROW) + j].piece.type = Checker;
                board->square[(i * SQUARES_PER_ROW) + j].piece.position.col = board->square[(i * SQUARES_PER_ROW) + j].position.col;
                board->square[(i * SQUARES_PER_ROW) + j].piece.position.row = board->square[(i * SQUARES_PER_ROW) + j].position.row;
            }
        }
    }
}

int checkKingTurningCondition(Board *board, Position destiny, Piece *piece)
{
    if (piece->type == King)
    {
        return 0;
    }

    if (destiny.row == SQUARES_PER_ROW - 1 && piece->color == White)
    {
        return 1;
    }
    else if (destiny.row == 0 && piece->color == Black)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void turnKing(Board *board, Piece *piece)
{
    int index = getIndexOfPosition(piece->position);
    board->square[index].piece.type = King;
}

void swapTurn()
{
    if (turn)
    {
        turn = White;
    }
    else
    {
        turn = Black;
    }
}

Movement convertInput(char iniCol, char iniRow, char finCol, char finRow, int *valid)
{
    Movement move;
    *valid = 1;
    int parsedRow, parsedCol;

    parsedCol = parseCharToInt(iniCol, 0);
    if (parsedCol >= 0)
    {
        move.origin.col = parsedCol;
    }
    else
    {
        *valid = 0;
        return move;
    }

    parsedCol = parseCharToInt(finCol, 0);
    if (parsedCol >= 0)
    {
        move.destiny.col = parsedCol;
    }
    else
    {
        *valid = 0;
        return move;
    }

    parsedRow = parseCharToInt(iniRow, 1);
    if (parsedRow >= 0)
    {
        move.origin.row = parsedRow;
    }
    else
    {
        *valid = 0;
        return move;
    }

    parsedRow = parseCharToInt(finRow, 1);
    if (parsedRow >= 0)
    {
        move.destiny.row = parsedRow;
    }
    else
    {
        *valid = 0;
        return move;
    }

    return move;
}

MovementSequence getMovementFromUser()
{
    MovementSequence movementSequence;
    char input[80];
    Movement move;
    int valid = 0;

    do
    {
        movementSequence.numberOfMovements = 0;
        printf("\n\n\t\t\tWhat moves would you like to make? Use bars between movements and put a \"*\" to"
               "\n\t\t\tinform the end of the sequence.\n\n\t\t\tI.E.(a1 c3|c3 e5|e5 g7*  OR  a2 b3*): ");
        scanf("%[^\n]%*c", input);

        for (size_t i = 0, j = 0; i < 80; i = i + 6, j++)
        {
            movementSequence.seqMovements[j] = convertInput(input[i], input[i + 1], input[i + 3], input[i + 4], &valid);
            movementSequence.numberOfMovements++;
            if (!valid || input[i + 5] == '*')
            {
                break;
            }
        }

        if (valid)
        {
            return movementSequence;
        }
        else
        {
            printf("\n\t\t\tInvalid input! Try something like \"a1 c3|c3 e5|e5 g7*  OR  a2 b3*\"");
        }
    } while (valid != 1);
}

int hasPiece(Board *board, Position position)
{
    int index = getIndexOfPosition(position);
    if (index >= 0 && index < TOTAL_SQUARES)
    {
        return board->square[index].state;
    }
    else
    {
        return 0;
    }
}

enum Adjacency isAdjacent(Position origin, Position destiny)
{
    if (origin.row + 1 == destiny.row)
    { // Is one row above
        if (origin.col + 1 == destiny.col || origin.col - 1 == destiny.col)
        { // Is adjacent to the front
            return AdjacentFront;
        }
        else
        { // Is not adjacent
            return NotAdjacent;
        }
    }
    else if (origin.row - 1 == destiny.row)
    { // Is one row behind
        if (origin.col + 1 == destiny.col || origin.col - 1 == destiny.col)
        { // Is adjacent to the back
            return AdjacentBack;
        }
        else
        {
            return NotAdjacent;
        }
    }
    else
    {
        return NotAdjacent;
    }
}

Piece getPiece(Board *board, Position position)
{
    return board->square[getIndexOfPosition(position)].piece;
}

//This function get some useful informations about the "path" between origin and destiny positions

PathInfo getPathInfo(Board *board, Position origin, Position destiny)
{
    PathInfo pathInfo;
    pathInfo.piecesInTheWay = 0;
    Piece lastPieceInTheWay;
    int distanceMovedRow = origin.row - destiny.row;
    int distanceMovedCol = origin.col - destiny.col;

    Position position = origin;

/*The signal of the int variable distanceMovedRow/Col indicates if the piece is moving towards or backwards 
on the board, and if its to the left or right diagonal. We use that information to correctly increment
or decrement the position.col/row varibles and obtain the squares belonging to the path*/

    if (distanceMovedRow > 0)
    {
        if (distanceMovedCol > 0)
        {
            for (size_t i = 1; i < abs(distanceMovedRow); i++)
            {
                position.row--;
                position.col--;
                if (hasPiece(board, position))
                {
                    pathInfo.lastPieceInTheWay = getPiece(board, position);
                    pathInfo.piecesInTheWay++;
                }
            }
        }
        else
        {
            for (size_t i = 1; i < abs(distanceMovedRow); i++)
            {
                position.row--;
                position.col++;
                if (hasPiece(board, position))
                {
                    pathInfo.lastPieceInTheWay = getPiece(board, position);
                    pathInfo.piecesInTheWay++;
                }
            }
        }
    }
    else
    {
        if (distanceMovedCol > 0)
        {
            for (size_t i = 1; i < abs(distanceMovedRow); i++)
            {
                position.row++;
                position.col--;
                if (hasPiece(board, position))
                {
                    pathInfo.lastPieceInTheWay = getPiece(board, position);
                    pathInfo.piecesInTheWay++;
                }
            }
        }
        else
        {
            for (size_t i = 1; i < abs(distanceMovedRow); i++)
            {
                position.row++;
                position.col++;
                if (hasPiece(board, position))
                {
                    pathInfo.lastPieceInTheWay = getPiece(board, position);
                    pathInfo.piecesInTheWay++;
                }
            }
        }
    }

    pathInfo.distance = abs(distanceMovedRow);
    return pathInfo;
}

int isDiagonal(Position origin, Position destiny)
{
    return abs(origin.row - destiny.row) == abs(origin.col - destiny.col);
}

void endGame(enum Winner winner)
{

    char color[6];
    if (winner == BlackWon)
    {
        strcpy(color, "BLACK");
    }
    else
    {
        strcpy(color, "WHITE");
    }

    printf("\n\n");
    printf("\t\t ---------------------------------------\n");
    printf("\t\t|\t                        \t|\n");
    printf("\t\t|\tENDGAME! %s HAS WON!\t\t|\n", color);
    printf("\t\t|\t                        \t|\n");
    printf("\t\t ---------------------------------------\n\n");

    exit(1);
}

enum MovementType checkMovement(Board *board, Movement *move, Piece *pieceMoving, int chainAttackFlag)
{
    if (chainAttackFlag)
    {
        if (hasPiece(board, move->destiny) ||         // checks if the destiny position is free
            !isDiagonal(move->origin, move->destiny)) // if the destiny position is diagonal in relation to destiny's
        {
            return Invalid;
        }
    }
    else
    {
        if (!(hasPiece(board, move->origin) &&           // checks if the origin position has a piece,
              !hasPiece(board, move->destiny) &&         // if the destiny position is free
              isDiagonal(move->origin, move->destiny) && // if the destiny position is diagonal in relation to destiny's
              turn == board->square[getIndexOfPosition(move->origin)].piece.color))
        { // and if is the turn of the piece in origin to play
            return Invalid;
        }
    }

    PathInfo pathInfo = getPathInfo(board, move->origin, move->destiny);

    switch (pieceMoving->type)
    {
    case Checker:
        if (pathInfo.distance > 1)
        {
            if (pathInfo.piecesInTheWay == 1 && pathInfo.lastPieceInTheWay.color == !pieceMoving->color && pathInfo.distance < 3)
            { // Exists a piece in the way of the enemy Color
                return Attack;
            }
            else
            { // Tryng to attack friendly color or trying to move more than 1 house using a Checker piece
                return Invalid;
            }
        }
        else if (isAdjacent(move->origin, move->destiny) == AdjacentFront && pieceMoving->color == White) 
        {
            return Move;
        }
        else if (isAdjacent(move->origin, move->destiny) == AdjacentBack && pieceMoving->color == Black)
        {
            return Move;
        }
        else        //trying to move backwards with a piece of type Checker
        {
            return Invalid;
        }
        break;
    case King:
        if (pathInfo.piecesInTheWay == 1)
        {
            if (pathInfo.lastPieceInTheWay.color == !pieceMoving->color)
            { // Exists a piece in the way of the enemy Color
                return Attack;
            }
            else
            { // Tryng to attack friendly color
                return Invalid;
            }
        }
        else if (pathInfo.piecesInTheWay == 0)
        { // No pieces on the way, cause its the King this piece is free to move
            return Move;
        }
        else
        { // Last scenario: more than 1 piece in the way, it cant move
            return Invalid;
        }
        break;
    default:
        break;
    }
}

// Given a certain position and a piece to test, calculates the maximum amount of captures
// possible starting a capturing chain in that position

int getMaxCapturesFromPosition(Board *board, Position *originPosition, Piece *pieceTested, IdStack *idStack)
{
    int captures = 0;
    int maxCaptures = 0;
    Movement testMovement;
    enum MovementType testMoveType;
    Square testSquare;
    testMovement.origin = *originPosition; // sets the testMovement.origin to the position being tested

    // iterates over the board looking for black squares free positions. When finds one, test the movement
    // originating in originPosition with destiny in that position. If its an attack, verifies if it hasn't
    // already been made checking the idStack. If it doesn't, increment the captures counter and calls this function
    // again, recursively, passing the testMovement.destiny as argument and adding its return to the captures counter

    for (size_t i = 0; i < TOTAL_SQUARES; ++i)
    {
        testSquare = board->square[i];

        // if the originPosition has no piece in it, it needs to activate the 
        // chainAttack Flag on calling the checkMovement function

        if (testSquare.state == Free && isDiagonal(*originPosition, board->square[i].position))
        {    
            testMovement.destiny = testSquare.position;
            if (board->square[getIndexOfPosition(*originPosition)].state == Free)
            {
                testMoveType = checkMovement(board, &testMovement, pieceTested, 1);
            }
            else
            {
                testMoveType = checkMovement(board, &testMovement, pieceTested, 0);
            }

            if (testMoveType == Attack)
            {
                PathInfo testPathInfo = getPathInfo(board, testMovement.origin, testMovement.destiny);
                if (!hasId(idStack, testPathInfo.lastPieceInTheWay.id))
                {
                    pushIdStack(idStack, testPathInfo.lastPieceInTheWay.id);
                    captures++;
                    captures = captures + getMaxCapturesFromPosition(board, &testMovement.destiny, pieceTested, idStack);
                }
            }
        }
        if (captures > maxCaptures)
        {
            maxCaptures = captures;
        }
        if (captures > 0) // if it achieved to identify one possible capture, the function removes
        {                 // the possible caputured piece id from the idStack before returning
            popIdStack(idStack);
        }
        captures = 0;
    }

    return maxCaptures;
}

// This function gets, for a given state of the board, the maximum possible ammount
// of captures that can be made in one turn by the piece color passaed as argument

int getMaxPossibleCaptures(Board *board, enum PieceColor pieceColor)
{
    IdStack idStack;
    int maxPossibleCaptures = 0;
    int possibleCaptures;

    // tests for each position of the board the max possible captures that can be made
    // by a piece of the informed color standing in that position

    for (size_t i = 0; i < TOTAL_SQUARES; ++i)
    {
        initializeIdStack(&idStack); // Stack that will store id's of pieces that already has been captured
                                     // on the simulated attack chain
        if (board->square[i].state == Occupied && board->square[i].piece.color == pieceColor)
        {
            possibleCaptures = getMaxCapturesFromPosition(board, &board->square[i].position, &board->square[i].piece, &idStack);
            if (possibleCaptures > maxPossibleCaptures)
            {
                maxPossibleCaptures = possibleCaptures;
            }
        }
        possibleCaptures = 0;
    }

    return maxPossibleCaptures;
}

void capturePiece(Board *board, Piece *piece)
{
    int index = getIndexOfPosition(piece->position);
    board->square[index].piece.id = 0;
    board->square[index].state = Free;

    if (piece->color == White)
    {
        board->whitePieces--;
    }
    else
    {
        board->blackPieces--;
    }
}

enum MovementType checkMovementSequence(Board *board, MovementSequence *movementSequence)
{
    enum MovementType movementType;
    Piece pieceMoving = board->square[getIndexOfPosition(movementSequence->seqMovements[0].origin)].piece;
    movementType = checkMovement(board, &movementSequence->seqMovements[0], &pieceMoving, 0);

    // First we need to check the first movement on the sequence. If its a movement, we dont need to check
    // the others, cause only attacks can have more than one movement

    if (movementType == Move)
    {
        if (movementSequence->numberOfMovements > 1 || getMaxPossibleCaptures(board, turn) > 0)
        { // checking if it consists of just one movement AND if has any possible capture movement of the turn's player on the board
            return Invalid;
        }
        else
        {
            return Move;
        }
    }

    // If the first movement is Attack, check all the others to verify if it consists of only attacks
    // If it does, check if is the biggest attack chain possible on the turn

    if (movementType == Attack)
    {
        for (size_t i = 0; i < movementSequence->numberOfMovements; i++)
        {   
            pieceMoving = board->square[getIndexOfPosition(movementSequence->seqMovements[i].origin)].piece;
            movementType == checkMovement(board, &movementSequence->seqMovements[i], &pieceMoving, 0);
            if (movementType != Attack)
            {
                return Invalid;
            }
        }

        if (getMaxPossibleCaptures(board, turn) > movementSequence->numberOfMovements)
        {
            return Invalid;
        }
        else
        {
            return Attack;
        }
    }

    return Invalid;
}

void checkWinCondition(Board *board)
{
    if (board->blackPieces == 0)
    {
        printBoard(board, 0);
        endGame(WhiteWon);
    }
    else if (board->whitePieces == 0)
    {
        printBoard(board, 0);
        endGame(BlackWon);
    }

    // Tests for each piece of the current turn player if it has any possible
    // movement to make. If it hasn't, then the opposite color player won

    Movement testMove;
    Piece testPiece;

    for (size_t i = 0; i < TOTAL_SQUARES; ++i)
    {
        if (board->square[i].state == Occupied && board->square[i].piece.color == turn)
        {
            testMove.origin = board->square[i].position;
            for (size_t j = 0; j < TOTAL_SQUARES; ++j)
            {
                testPiece = board->square[getIndexOfPosition(testMove.origin)].piece;
                testMove.destiny = board->square[j].position;
                if (checkMovement(board, &testMove, &testPiece, 0) != Invalid)
                {
                    return;
                }
            }
        }
    }

    if (turn == Black)
    {
        printBoard(board, 0);
        endGame(WhiteWon);
    }
    else if (turn == White)
    {
        printBoard(board, 0);
        endGame(BlackWon);
    }
}

void movePiece(Board *board, Movement move)
{
    Piece pieceAux;
    int index;

    index = getIndexOfPosition(move.origin);
    pieceAux = board->square[index].piece;
    board->square[index].piece.id = 0;
    board->square[index].state = Free;

    index = getIndexOfPosition(move.destiny);
    board->square[index].piece = pieceAux;
    board->square[index].piece.position = board->square[index].position;
    board->square[index].state = Occupied;

    if (checkKingTurningCondition(board, move.destiny, &pieceAux))
    {
        turnKing(board, &board->square[index].piece);
    }
}

void makeAttack(Board *board, MovementSequence *movementSequence)
{
    Piece pieceToCapture;
    PathInfo pathInfo;

    for (size_t i = 0; i < movementSequence->numberOfMovements; i++)
    {
        pathInfo = getPathInfo(board, movementSequence->seqMovements[i].origin, movementSequence->seqMovements[i].destiny);
        capturePiece(board, &pathInfo.lastPieceInTheWay);
    }

    Movement finalMovement;
    finalMovement.origin = movementSequence->seqMovements[0].origin;
    finalMovement.destiny = movementSequence->seqMovements[movementSequence->numberOfMovements - 1].destiny;

    movePiece(board, finalMovement);
}

void printTurn()
{
    char color[6];
    if (turn)
    {
        strcpy(color, "BLACK");
    }
    else
    {
        strcpy(color, "WHITE");
    }
    printf("\n\t\t\tTURN: %s", color);
}

void playGame(Board board)
{
    MovementSequence movementSequence;
    enum MovementType moveType;

    while (1)
    {
        printTurn();
        movementSequence = getMovementFromUser();
        moveType = checkMovementSequence(&board, &movementSequence);

        switch (moveType)
        {
        case Move:
            movePiece(&board, movementSequence.seqMovements[0]);
            printBoard(&board, 0);
            swapTurn();    
            checkWinCondition(&board);
            break;
        case Attack:
            makeAttack(&board, &movementSequence);
            printBoard(&board, 0);
            swapTurn();
            checkWinCondition(&board);
            break;
        case Invalid:
            printf("\n\t\t\tThis movement is not allowed. Try another one.\n");
            break;
        default:
            break;
        }
    }
}

Board parseBoardFromMatrix(int matrixBoard[8][8])
{   
    Board board;
    board.blackPieces = 0;
    board.whitePieces = 0;
    int squareIdCounter = 0;
    int pieceIdCounter = 0;
    int squareIndex;
    Position currentPosition;

    for (size_t i = 0; i < SQUARES_PER_ROW; i++)
    {
        for (size_t j = 0; j < SQUARES_PER_ROW; j++)
        {   
            squareIndex = i * SQUARES_PER_ROW + j;
            currentPosition.row = i;
            currentPosition.col = j;
            board.square[squareIndex].id = squareIdCounter++;
            board.square[squareIndex].position = currentPosition;

            if ((i + j) % 2 == 0)
            {
                strcpy(board.square[squareIndex].color, blackSquare);
            }
            else
            {
                strcpy(board.square[squareIndex].color, whiteSquare);
            }

            switch (matrixBoard[i][j])
            {
            case 0:
                board.square[squareIndex].piece.id = 0;
                board.square[squareIndex].state == Free;
                break;
            case 1:
                board.square[squareIndex].piece.color == White;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type == Checker;
                board.square[squareIndex].state == Occupied;
                board.whitePieces++;
                break;
            case 2:
                board.square[squareIndex].piece.color == Black;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type == Checker;
                board.square[squareIndex].state == Occupied;
                board.blackPieces++;
                break;
            case 3:
                board.square[squareIndex].piece.color == White;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type == King;
                board.square[squareIndex].state == Occupied;
                board.whitePieces++;
                break;
            case 4:
                board.square[squareIndex].piece.color == Black;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type == King;
                board.square[squareIndex].state == Occupied;
                board.blackPieces++;
                break;
            default:
                break;
            }
        }
    }
    return board;
}

MovementSequence parseMovementSequenceFromArray(int numberOfItens, int movements[numberOfItens])
{
    MovementSequence movementSequence;
    movementSequence.numberOfMovements = numberOfItens / 4;

    for (size_t i = 0, j = 0; i < numberOfItens; i++, j+=4)
    {
        movementSequence.seqMovements[i].origin.row = movements[j];
        movementSequence.seqMovements[i].origin.col = movements[j + 1];
        movementSequence.seqMovements[i].destiny.row = movements[j + 2];
        movementSequence.seqMovements[i].destiny.col = movements[j + 3];
    }
    return movementSequence;
}

void updateMatrixBoard(Board *board, int matrixBoard[8][8])
{
    Position currentPosition;
    int squareIndex;

    for (size_t i = 0; i < SQUARES_PER_ROW; i++)
    {
        currentPosition.row = i;
        for (size_t j = 0; j < SQUARES_PER_ROW; j++)
        {
            currentPosition.col = j;
            squareIndex = getIndexOfPosition(currentPosition);
            if(board->square[squareIndex].state == Occupied)
            {
                if(board->square[squareIndex].piece.color == White)
                {
                    if(board->square[squareIndex].piece.type == Checker)
                    {
                        matrixBoard[i][j] = 1;
                    }
                    else
                    {
                        matrixBoard[i][j] = 3;
                    }
                }
                else
                {
                    if(board->square[squareIndex].piece.type == Checker)
                        {
                            matrixBoard[i][j] = 2;
                        }
                        else
                        {
                            matrixBoard[i][j] = 4;
                        }
                }
            }
            else
            {
                matrixBoard[i][j] = 0;
            }
        }
        
    }
}

int entryPoint(int matrixBoard[8][8], int numberOfItens, int listOfMovements[numberOfItens])
{
    Board board = parseBoardFromMatrix(matrixBoard);
    MovementSequence movementSequence = parseMovementSequenceFromArray(numberOfItens, listOfMovements);
    enum MovementType moveType;

    moveType = checkMovementSequence(&board, &movementSequence);

    switch (moveType)
    {
    case Move:
        movePiece(&board, movementSequence.seqMovements[0]);
        updateMatrixBoard(&board, matrixBoard);
        printBoard(&board, 0);
        swapTurn();    
        checkWinCondition(&board);
        return Move;
        break;
    case Attack:
        makeAttack(&board, &movementSequence);
        updateMatrixBoard(&board, matrixBoard);
        printBoard(&board, 0);
        swapTurn();
        checkWinCondition(&board);
        return Attack;
        break;
    case Invalid:
        printf("\n\t\t\tThis movement is not allowed. Try another one.\n");
        return Invalid;
        break;
    default:
        break;
    }
}

int main(void)
{
    int matrix[8][8] = 
    {
        {1, 0, 1, 0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 2, 0, 2, 0, 2},
        {2, 0, 2, 0, 2, 0, 2, 0},
        {0, 2, 0, 2, 0, 2, 0, 2}
    };  

    int numOfItens = 4;
    int list_moves[4] = {2, 0, 3, 1};

    printf("\n\nReturn: %d", entryPoint(matrix, numOfItens, list_moves));
    // Board board = createBoard();
    // setPieces(&board);
    // printBoard(&board, 0);

    // TEST HARDCODED POSITIONS

    // Position p1;
    // p1.col = 0;
    // p1.row = 2;
    // Position p2;
    // p2.col = 1;
    // p2.row = 3;
    // Position p3;
    // p3.col = 1;
    // p3.row = 5;
    // Position p4;
    // p4.col = 0;
    // p4.row = 4;

    // Movement m1;
    // m1.origin = p1;
    // m1.destiny = p2;
    // Movement m2;
    // m2.origin = p3;
    // m2.destiny = p4;

    // capturePiece(&board, &board.square[43].piece);
    // capturePiece(&board, &board.square[45].piece);
    // capturePiece(&board, &board.square[47].piece);
    // capturePiece(&board, &board.square[48].piece);
    // capturePiece(&board, &board.square[50].piece);
    // capturePiece(&board, &board.square[52].piece);
    // capturePiece(&board, &board.square[54].piece);
    // capturePiece(&board, &board.square[57].piece);
    // capturePiece(&board, &board.square[59].piece);
    // capturePiece(&board, &board.square[61].piece);
    // capturePiece(&board, &board.square[63].piece);

    // movePiece(&board, m1);
    // movePiece(&board, m2);

    // printBoard(&board, 0);

    // END OF TEST HARDCODED POSITIONS

    // playGame(board);
}

