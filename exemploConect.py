#Esse código só tem a funçã ode ser uma forma simple sde mostrar a exata configuração de tipos e wrappers para
#conectar a interface com o back da nossa aplicação

import os
import ctypes
import numpy as np

# Definir o tipo de matriz 8x8 para c_int_Array_8
MatrixRow = ctypes.c_int * 8
MatrixType = MatrixRow * 8

# Carregar a biblioteca compartilhada
path_to_library = os.path.join(os.path.dirname(__file__), "lib.so")
lib = ctypes.CDLL(path_to_library)

# Definir os tipos de retorno e argumentos da função
lib.entryPoint.restype = ctypes.c_int
lib.entryPoint.argtypes = [ctypes.POINTER(MatrixType), ctypes.c_int, ctypes.POINTER(ctypes.c_int)]

# Crie a matriz e a lista de movimentos em Python
board_pos = [[0 for _ in range(8)] for _ in range(8)]
list_of_moves = [1, 2, 3, 4]
qtd_moves = len(list_of_moves)

# Converter a matriz Python para NumPy array
board_np = np.array(board_pos, dtype=np.int32)
'''board_np = np.array(board_pos, dtype=np.int32)
Nesta linha, estamos convertendo a matriz Python board_pos em uma matriz NumPy board_np. Para fazer isso, utilizamos a função np.array(),
que cria um novo array NumPy a partir da matriz Python board_pos. O argumento dtype=np.int32 especifica que queremos que o array NumPy contenha inteiros de 32 bits
(o mesmo tipo usado em MatrixType). Essa conversão é necessária para poder passar a matriz board_np para o código C, pois ele espera um array NumPy como argumento.'''

# Converter cada elemento do array NumPy para c_int_Array_8
board_c = MatrixType(*[MatrixRow(*[ctypes.c_int(x) for x in row]) for row in board_np])
'''board_c = MatrixType(*[MatrixRow(*[ctypes.c_int(x) for x in row]) for row in board_np])
Nesta linha, estamos criando a matriz C board_c a partir do array NumPy board_np. O objetivo é converter cada elemento do array NumPy em uma
c_int_Array_8, que é a estrutura de dados usada para representar cada linha da matriz C.
Vamos entender o loop:
for row in board_np: percorre cada linha da matriz NumPy board_np.
MatrixRow(*[ctypes.c_int(x) for x in row]) cria uma c_int_Array_8 a partir de cada linha da matriz NumPy. ctypes.c_int(x) converte cada elemento da linha em um inteiro
no formato c_int, e * antes de [ctypes.c_int(x) for x in row] desempacota os elementos da lista para que possam ser usados como argumentos para MatrixRow.
O loop externo MatrixType(*[... for row in board_np]) cria a matriz C board_c juntando cada linha convertida em uma c_int_Array_8 para formar a matriz C completa.'''

# Obter o ponteiro C para a matriz
board_ptr = ctypes.pointer(board_c)

# Converter a lista de movimentos para um array de inteiros
moves_c = (ctypes.c_int * qtd_moves)(*list_of_moves)

# Chame a função entryPoint
lib.entryPoint(board_ptr, qtd_moves, moves_c)

# Imprimir a matriz modificada pelo código C
print("Matriz modificada pelo código C:")
for row in board_c:
    print(list(row))

# Obter os valores do ponteiro C e atualizar a matriz Python
for i in range(8):
    for j in range(8):
        board_pos[i][j] = board_c[i][j]

print(board_pos)