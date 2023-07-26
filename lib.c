#include <stdio.h>

int entryPoint(int board[8][8], int qtd, int list_moves[qtd]) {
    // Faça alguma coisa com a matriz e a lista aqui
    printf("Movimentos recebidos:\n");
    for (int i = 0; i < qtd; i++) {
        printf("%d ", list_moves[i]);
    }

    printf("\n\n");

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    int qtd_local = 0;
    int cor = board[qtd_local][qtd_local+1];
    while (qtd_local < qtd){
        board[list_moves[qtd_local]][list_moves[qtd_local+1]] = 0;
        board[list_moves[qtd_local+2]][list_moves[qtd_local+3]] = cor;
        qtd_local += 4;
    }

    return 0;
}