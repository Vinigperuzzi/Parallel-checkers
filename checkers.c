//////////////// -- INCLUDE's -- ////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////// -- DEFINE's -- ////////////////

#define SQUARES_PER_ROW 8
#define TOTAL_SQUARES SQUARES_PER_ROW *SQUARES_PER_ROW
#define PLAYABLE_SQUARES_PER_ROW SQUARES_PER_ROW / 2
#define CENTRAL_SQUARES PLAYABLE_SQUARES_PER_ROW / 2
#define PIECES_PER_PLAYER 12
#define TOTAL_PIECES PIECES_PER_PLAYER * 2
#define MAX_MOVEMENTS_PER_TURN 12
#define MAX_FIRST_CHAIN_MOVE 13
#define WEIGHT_DIFF_PIECES 1.75
#define WEIGHT_QTD_KINGS 0.75
#define WEIGHT_EMINENT_KINGS 0.150 // pieces one square from promotion
#define WEIGHT_POSSIBLE_KINGS 0.5  // pieces two squares from promotion
#define WEIGHT_CENTRAL_PIECES 0.5

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
    int whiteKings;
    int blackPieces;
    int blackKings;
} Board;

typedef struct STMovement
{
    Position origin;
    Position destiny;
} Movement;

enum MovementType
{
    Invalid,
    Move,
    Attack
};

typedef struct STMovementSequence
{
    Movement seqMovements[MAX_MOVEMENTS_PER_TURN];
    int numberOfMovements;
    enum MovementType movementType;
} MovementSequence;

typedef struct STPossibleMovement
{
    MovementSequence possibleMovementList[MAX_FIRST_CHAIN_MOVE];
    int numberOfPossibleMovements;
} PossibleMovements;

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

enum Winner
{
    NoOne,
    WhiteWon = 3,
    BlackWon = 4
};

//////////////// -- GLOBAL VAR's -- ////////////////

char whiteSquare[9] = "\033[47m";
char blackSquare[9] = "\033[40m";
enum PieceColor turn = White;
float globalScore = 0;
MovementSequence computerMovement;

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
    board.blackKings = 0;
    board.whiteKings = 0;
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
    if (board->square[index].piece.color == White)
    {
        board->whiteKings++;
    }
    else
    {
        board->blackKings++;
    }
}

void swapTurn(int *frontEndTurn)
{
    if (turn)
    {
        turn = White;
        *frontEndTurn = 0;
    }
    else
    {
        turn = Black;
        *frontEndTurn = 1;
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

PathInfo getPathInfo(Board *board, Position origin, Position destiny)
{

    // This function get some useful informations about the "path" between origin and destiny positions

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

enum MovementType checkMovement(Board *board, Movement *move, Piece *pieceMoving, enum PieceColor turn, int chainAttackFlag)
{
    if (chainAttackFlag)
    {
        if (hasPiece(board, move->destiny) ||         // checks if the destiny position is free and
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
              turn == pieceMoving->color))
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
        else // trying to move backwards with a piece of type Checker
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

int getNumberOfMaxCapturesFromPosition(
    Board *board,
    Position *originPosition,
    Piece *pieceTested,
    enum PieceColor turn,
    IdStack *idStack)
{

    // Given a certain position and a piece to test, calculates the maximum amount of captures
    // possible starting a capturing chain in that position

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
                testMoveType = checkMovement(board, &testMovement, pieceTested, turn, 1);
            }
            else
            {
                testMoveType = checkMovement(board, &testMovement, pieceTested, turn, 0);
            }

            if (testMoveType == Attack)
            {
                PathInfo testPathInfo = getPathInfo(board, testMovement.origin, testMovement.destiny);
                if (!hasId(idStack, testPathInfo.lastPieceInTheWay.id))
                {
                    pushIdStack(idStack, testPathInfo.lastPieceInTheWay.id);
                    captures++;
                    captures = captures + getNumberOfMaxCapturesFromPosition(board, &testMovement.destiny, pieceTested, turn, idStack);
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

void getPossibleAttackChainsFromPosition(
    Board *board,
    Position *originPosition,
    Piece *pieceTested,
    enum PieceColor turn,
    IdStack *idStack,
    MovementSequence possibleAttackChain,
    PossibleMovements *possibleMovements)
{

    // Given a certain position and a piece to test, this function adds to the possibleMovementsList every
    // possible and valid attack chain beggining in that position

    Movement testMovement;
    enum MovementType testMoveType;
    Square testSquare;
    testMovement.origin = *originPosition; // sets the testMovement.origin to the position being tested

    // iterates over the board looking for black squares free positions. When finds one, test the movement
    // originating in originPosition with destiny in that position. If its an attack, verifies if it hasn't
    // already been made checking the idStack. If it doesn't, adds the movement to the possibleAttackChain
    // and calls this function again, recursively, passing the testMovement.destiny and the actual
    // possibleAttackChain as argument.

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
                testMoveType = checkMovement(board, &testMovement, pieceTested, turn, 1);
            }
            else
            {
                testMoveType = checkMovement(board, &testMovement, pieceTested, turn, 0);
            }

            if (testMoveType == Attack)
            {
                PathInfo testPathInfo = getPathInfo(board, testMovement.origin, testMovement.destiny);
                if (!hasId(idStack, testPathInfo.lastPieceInTheWay.id))
                {
                    pushIdStack(idStack, testPathInfo.lastPieceInTheWay.id);
                    possibleAttackChain.seqMovements[possibleAttackChain.numberOfMovements] = testMovement;
                    possibleAttackChain.numberOfMovements++;
                    getPossibleAttackChainsFromPosition(board, &testMovement.destiny, pieceTested, turn,
                                                        idStack, possibleAttackChain, possibleMovements);
                }
            }
        }

        // When there is no more attack that can be made, verify if possibleAttackChain is valid and if it does,
        // adds it to the possibleMovementsList, cause it means that it reached the end of an valid attack chain

        if (possibleAttackChain.numberOfMovements > 0)
        {
            checkMovementSequence(board, &possibleAttackChain, turn);
            if (possibleAttackChain.movementType == Attack)
            {
                possibleMovements->possibleMovementList[possibleMovements->numberOfPossibleMovements] = possibleAttackChain;
                possibleMovements->numberOfPossibleMovements++;
            }
        }
        if (possibleAttackChain.numberOfMovements > 0)
        {                        // if it achieved to identify one possible capture, the function removes
            popIdStack(idStack); // the possible caputured piece id from the idStack before returning
        }
    }

    return;
}

int getMaxPossibleCaptures(Board *board, enum PieceColor pieceColor)
{

    // This function gets, for a given state of the board, the maximum possible ammount
    // of captures that can be made in one turn by the piece color passaed as argument

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
            possibleCaptures = getNumberOfMaxCapturesFromPosition(board, &board->square[i].position, &board->square[i].piece, pieceColor, &idStack);
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
        if (piece->type == King)
        {
            board->whiteKings--;
        }
        board->whitePieces--;
    }
    else
    {
        if (piece->type == King)
        {
            board->blackKings--;
        }
        board->blackPieces--;
    }
}

void checkMovementSequence(Board *board, MovementSequence *movementSequence, enum PieceColor turn)
{
    enum MovementType movementType;
    Piece pieceMoving = board->square[getIndexOfPosition(movementSequence->seqMovements[0].origin)].piece;
    movementType = checkMovement(board, &movementSequence->seqMovements[0], &pieceMoving, turn, 0);

    // First we need to check the first movement on the sequence. If its a movement, we dont need to check
    // the others, cause only attacks can have more than one movement

    if (movementType == Move)
    {
        if (movementSequence->numberOfMovements > 1 || getMaxPossibleCaptures(board, turn) > 0)
        { // checking if it consists of just one movement AND if has any possible capture movement of the turn's player on the board
            movementSequence->movementType = Invalid;
            return;
        }
        else
        {
            movementSequence->movementType = Move;
            return;
        }
    }

    // If the first movement is Attack, check all the others to verify if it consists of only attacks
    // If it does, check if is the biggest attack chain possible on the turn

    if (movementType == Attack)
    {
        for (size_t i = 1; i < movementSequence->numberOfMovements; i++)
        {
            movementType = checkMovement(board, &movementSequence->seqMovements[i], &pieceMoving, turn, 1);
            printf("\nMoveType: %d", movementType);
            if (movementType != Attack)
            {
                movementSequence->movementType = Invalid;
                return;
            }
        }

        if (getMaxPossibleCaptures(board, turn) > movementSequence->numberOfMovements)
        {
            movementSequence->movementType = Invalid;
            return;
        }
        else
        {
            movementSequence->movementType = Attack;
            return;
        }
    }

    movementSequence->movementType = Invalid;
    return;
}

enum Winner checkWinCondition(Board *board, enum PieceColor turn)
{
    if (board->blackPieces == 0)
    {
        return WhiteWon;
    }
    else if (board->whitePieces == 0)
    {
        return BlackWon;
    }

    // Tests for each piece of the opponent of the current turn player if it has any possible
    // movement to make. If it hasn't, then the player won

    Movement testMove;
    Piece testPiece;

    for (size_t i = 0; i < TOTAL_SQUARES; ++i)
    {
        if (board->square[i].state == Occupied && board->square[i].piece.color != turn)
        {
            testMove.origin = board->square[i].position;
            for (size_t j = 0; j < TOTAL_SQUARES; ++j)
            {
                testPiece = board->square[getIndexOfPosition(testMove.origin)].piece;
                testMove.destiny = board->square[j].position;
                if (checkMovement(board, &testMove, &testPiece, !turn, 0) != Invalid)
                {
                    return NoOne;
                }
            }
        }
    }

    if (turn == Black)
    {
        return BlackWon;
    }
    else if (turn == White)
    {
        return WhiteWon;
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

// void playGame(Board board)
// {
//     MovementSequence movementSequence;
//     enum MovementType moveType;

//     while (1)
//     {
//         printTurn();
//         movementSequence = getMovementFromUser();
//         moveType = checkMovementSequence(&board, &movementSequence);

//         switch (moveType)
//         {
//         case Move:
//             movePiece(&board, movementSequence.seqMovements[0]);
//             printBoard(&board, 0);
//             checkWinCondition(&board);
//             swapTurn(&fron);
//             break;
//         case Attack:
//             makeAttack(&board, &movementSequence);
//             printBoard(&board, 0);
//             checkWinCondition(&board);
//             swapTurn();
//             break;
//         case Invalid:
//             printf("\n\t\t\tThis movement is not allowed. Try another one.\n");
//             break;
//         default:
//             break;
//         }
//     }
// }

Board parseBoardFromMatrix(int matrixBoard[8][8])
{
    Board board;
    board.blackPieces = 0;
    board.whitePieces = 0;
    board.whiteKings = 0;
    board.blackKings = 0;
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
                board.square[squareIndex].state = Free;
                break;
            case 1:
                board.square[squareIndex].piece.color = White;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type = Checker;
                board.square[squareIndex].state = Occupied;
                board.whitePieces++;
                break;
            case 2:
                board.square[squareIndex].piece.color = Black;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type = Checker;
                board.square[squareIndex].state = Occupied;
                board.blackPieces++;
                break;
            case 3:
                board.square[squareIndex].piece.color = White;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type = King;
                board.square[squareIndex].state = Occupied;
                board.whitePieces++;
                board.whiteKings++;
                break;
            case 4:
                board.square[squareIndex].piece.color = Black;
                board.square[squareIndex].piece.id = pieceIdCounter++;
                board.square[squareIndex].piece.position = currentPosition;
                board.square[squareIndex].piece.type = King;
                board.square[squareIndex].state = Occupied;
                board.blackPieces++;
                board.blackKings++;
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

    for (size_t i = 0, j = 0; i < movementSequence.numberOfMovements; i++, j += 4)
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
            if (board->square[squareIndex].state == Occupied)
            {
                if (board->square[squareIndex].piece.color == White)
                {
                    if (board->square[squareIndex].piece.type == Checker)
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
                    if (board->square[squareIndex].piece.type == Checker)
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

int checkQtdPiecesInRank(Board board, enum PieceColor pieceColor, int rank)
{
    int qtdPieces = 0;
    for (int i = 0; i < SQUARES_PER_ROW; i++)
    {
        if (board.square[(rank * SQUARES_PER_ROW) + i].piece.color == pieceColor)
        {
            qtdPieces++;
        }
    }
    return qtdPieces;
}

int checkCentralPieces(Board board, enum PieceColor pieceColor)
{
    int qtdPieces = 0;
    for (int i = 3; i < 5; i++)
    {
        for (int j = 3; j < 5; j++)
        {
            if (board.square[i * SQUARES_PER_ROW + j].piece.color == pieceColor)
            {
                qtdPieces++;
            }
        }
    }
    return qtdPieces;
}

float evaluatePos(Board *board, enum PieceColor turn)
{

    // Ideia: Adicionar valorização de capturar uma dama

    if (checkWinCondition(board, turn) == turn + 2)
    {
        return 10;
    }

    float scoreQtdpieces;
    float scoreKings;
    if (!turn)
    {
        scoreQtdpieces = (board->whitePieces - board->blackPieces) / (PIECES_PER_PLAYER * WEIGHT_DIFF_PIECES);
        scoreKings = (board->whiteKings - board->blackKings) / (PIECES_PER_PLAYER * WEIGHT_QTD_KINGS);
    }
    else
    {
        scoreQtdpieces = (board->blackPieces - board->whitePieces) / (PIECES_PER_PLAYER * WEIGHT_DIFF_PIECES);
        scoreKings = (board->blackKings - board->whiteKings) / (PIECES_PER_PLAYER * WEIGHT_QTD_KINGS);
    }

    float scoreEminentKings = checkQtdPiecesInRank(*board, turn, 6) / (PLAYABLE_SQUARES_PER_ROW * WEIGHT_EMINENT_KINGS);
    float scorePossibleKings = checkQtdPiecesInRank(*board, turn, 5) / (PLAYABLE_SQUARES_PER_ROW * WEIGHT_POSSIBLE_KINGS);
    float scoreCentralPieces = checkCentralPieces(*board, turn) / (CENTRAL_SQUARES * WEIGHT_CENTRAL_PIECES);
    float pesoPosicao = scoreQtdpieces + scoreKings + scoreEminentKings + scorePossibleKings + scoreCentralPieces;
    return pesoPosicao;
}

void getPossibleMovementsFromPosition(

    // This function adds to the possibleMoventsList all valid movements possible to make from a given position
    // and state of the board

    Board *board,
    PossibleMovements *possibleMovements,
    Position *testPosition,
    enum PieceColor turn)
{
    Movement auxMovement;
    Square auxSquare;
    MovementSequence auxMovementSeq;

    possibleMovements->numberOfPossibleMovements = 0;
    auxMovement.origin = *testPosition;

    // checks possible movements of the type Move and adds them to the possibleMovementsList
    // if identifies at least one possible attack, it means that because of the rules of the games, no move
    // can be made. So it calls the getPossibleAttackChainsFromPosition passing a pointer to possibleMovements,
    // so the function can populate it with the possible and valid attacks that can be made

    for (size_t i = 0; i < TOTAL_SQUARES; ++i)
    {
        auxSquare = board->square[i];
        if (auxSquare.state == Free && isDiagonal(*testPosition, auxSquare.position))
        {
            auxMovement.destiny = auxSquare.position;
            auxMovementSeq.numberOfMovements = 1;
            auxMovementSeq.seqMovements[0] = auxMovement;
            checkMovementSequence(board, &auxMovementSeq, turn);
            if (auxMovementSeq.movementType == Move)
            {
                possibleMovements->possibleMovementList[possibleMovements->numberOfPossibleMovements] = auxMovementSeq;
                possibleMovements->numberOfPossibleMovements++;
            }
            else if (auxMovementSeq.movementType == Attack)
            {
                auxMovementSeq.numberOfMovements = 0;
                IdStack idStack;
                initializeIdStack(&idStack);
                getPossibleAttackChainsFromPosition(board, &testPosition,
                                                    &board->square[getIndexOfPosition(*testPosition)].piece,
                                                    turn, &idStack, auxMovementSeq, possibleMovements);

                return;
            }
        }
    }
    return;
}

void generateComputerMovement(
    Board *board,
    MovementSequence *movementSequence,
    int level,
    int depth,
    float *biggestScoreAcc,
    MovementSequence firstMovement)
{

    float localScoreSum = *biggestScoreAcc;
    enum PieceColor thisLevelTurn = level % 2;

    if (level > 1)
    {
        printf("\nDebug: level: %d MoveType: %d", level, movementSequence->movementType);

        switch (movementSequence->movementType)
        {
        case Move:
            movePiece(&board, movementSequence->seqMovements[0]);
            break;
        case Attack:
            makeAttack(&board, &movementSequence);
            break;
        default:
            break;
        }

        localScoreSum += evaluatePos(board, thisLevelTurn);
        printf("\nDebug: LocalScoreSum: %f", localScoreSum);
    }

    PossibleMovements possibleMovements[PIECES_PER_PLAYER];
    Square auxSquare;
    int possibleMovesIndexCounter = 0;

    if (level < depth)
    {
        for (size_t i = 0; i < TOTAL_SQUARES; i++)
        {
            auxSquare = board->square[i];
            if (auxSquare.state == Occupied && auxSquare.piece.color == thisLevelTurn)
            {
                getPossibleMovementsFromPosition(board, &possibleMovements[possibleMovesIndexCounter],
                                                 &auxSquare.position, thisLevelTurn);
                // DEBUG
                Movement debugMove;
                for (size_t k = 0; k < possibleMovements[possibleMovesIndexCounter].numberOfPossibleMovements; k++)
                {
                    debugMove = possibleMovements[possibleMovesIndexCounter].possibleMovementList[k].seqMovements[0];
                    printf("\nDebug 2: Iter %d Origin: %d %d Destiny %d %d", i, debugMove.origin.row,
                           debugMove.origin.col, debugMove.destiny.row,
                           debugMove.destiny.col);
                }

                // DEBUG
                possibleMovesIndexCounter++;
            }
        }

        printf("\nDebug: indexCounter: %d", possibleMovesIndexCounter);

        for (size_t i = 0; i < possibleMovesIndexCounter; ++i)
        {
            for (size_t j = 0; j < possibleMovements[i].numberOfPossibleMovements; ++j)
            {
                if (level == 1)
                {
                    firstMovement = possibleMovements[i].possibleMovementList[j];
                }
                generateComputerMovement(board, &possibleMovements[i].possibleMovementList[j], level + 1,
                                         depth, biggestScoreAcc, firstMovement);
            }
        }
    }
    else
    {
        if (localScoreSum > globalScore)
        {
            globalScore = localScoreSum;
            computerMovement = firstMovement;
            printf("\nDebug: Chegou no if! Computer movement: %d %d - %d %d", computerMovement.seqMovements[0].origin.row, computerMovement.seqMovements[0].origin.col, computerMovement.seqMovements[0].destiny.row, computerMovement.seqMovements[0].destiny.col);
        }
    }
}

void makeComputerMovement(Board *board, MovementSequence *computerMovement, enum PieceColor turn)
{
    checkMovementSequence(board, computerMovement, turn);

    switch (computerMovement->movementType)
    {
    case Move:
        movePiece(board, computerMovement->seqMovements[0]);
        break;

    case Attack:
        makeAttack(board, &computerMovement->seqMovements);
        break;

    default:
        break;
    }
}

int entryPoint(int matrixBoard[8][8], int numberOfItens, int listOfMovements[numberOfItens], int *frontEndTurn)
{

    // Function to establish connection between front and back end. The front end calls this function passing
    // the state of the board and a list of movements represented by a matrix  and a vector, respectively, in
    // addition to the current turn. This function then parses this data, converting it in the data structures
    // used here, then evaluates the movement that is trying to be made and act upon its type

    turn = *frontEndTurn;

    // parse data structures

    Board board = parseBoardFromMatrix(matrixBoard);
    MovementSequence movementSequence = parseMovementSequenceFromArray(numberOfItens, listOfMovements);
    enum Winner winner;
    float biggestScoreAcc = 0;

    checkMovementSequence(&board, &movementSequence, turn);

    //Todo: Sobreescrever as primeiras 4 posições de listOfMovements com a jogada da máquina, só isso já faz aparecer na interface

    switch (movementSequence.movementType)
    {
    case Move:
        movePiece(&board, movementSequence.seqMovements[0]);
        // printBoard(&board, 0);
        winner = checkWinCondition(&board, turn);
        if (winner != NoOne) // check if someone won the game. If no one won, return the move authorization
        {
            return winner;
        }
        swapTurn(frontEndTurn);
        generateComputerMovement(&board, &movementSequence, 1, 2, &biggestScoreAcc, movementSequence);
        globalScore = 0;

        makeComputerMovement(&board, &computerMovement, turn);
        updateMatrixBoard(&board, matrixBoard); // update the front end board data structure
        winner = checkWinCondition(&board, turn);
        if (winner != NoOne) // check if someone won the game. If no one won, return the move authorization
        {
            return winner;
        }
        swapTurn(frontEndTurn);
        return Move;
        break;

    case Attack:
        makeAttack(&board, &movementSequence);
        // printBoard(&board, 0);
        winner = checkWinCondition(&board, turn);
        if (winner != NoOne) // check if someone won the game. If no one won, return the attack authorization
        {
            return winner;
        }
        swapTurn(frontEndTurn);
        generateComputerMovement(&board, &movementSequence, 1, 2, &biggestScoreAcc, movementSequence);
        globalScore = 0;

        makeComputerMovement(&board, &computerMovement, turn);
        updateMatrixBoard(&board, matrixBoard); // update the front end board data structure
        winner = checkWinCondition(&board, turn);
        if (winner != NoOne) // check if someone won the game. If no one won, return the move authorization
        {
            return winner;
        }
        swapTurn(frontEndTurn);
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
    // int matrix[8][8] =
    // {
    //     {1, 0, 1, 0, 1, 0, 1, 0},
    //     {0, 1, 0, 1, 0, 1, 0, 1},
    //     {1, 0, 1, 0, 1, 0, 1, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 2, 0, 2, 0, 2, 0, 2},
    //     {2, 0, 2, 0, 2, 0, 2, 0},
    //     {0, 2, 0, 2, 0, 2, 0, 2}
    // };

    // int numOfItens = 4;
    // int list_moves[4] = {2, 0, 3, 1};

    // printf("\n\nReturn: %d", entryPoint(matrix, numOfItens, list_moves));
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

// TO DO LIST
