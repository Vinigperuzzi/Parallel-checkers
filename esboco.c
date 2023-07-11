#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define SIZE 64;

typedef struct{
    casa casa;
} board;

typedef struct{
    position position;
    piece peca;
    int id;
} casa;

typedef struct {
    int color; //(0 == brancas, 1 == negras)
    int id;
    position position;
} piece;

typedef struct {
    int col;
    int row;
} position;


//if (brancas)
    //movimentosPossiveisBR = posmatriz[i+1][i-1]
    //movimentosPossiveis2BR = posmatriz[i+1][i+1]

    //movimentosPossiveisBR = posmatriz[i-1][i-1]
    //movimentosPossiveis2BR = posmatriz[i-1][i+1]



//Regras
//Dama é vida loka
//Come para trás
//Sopro existe
