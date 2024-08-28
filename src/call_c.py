import os
import ctypes
import numpy as np
import src.so_helper as so

def entry_point(board_pos, board_pos_send ,list_of_moves, qtd_moves, front_end_turn, difficulty_var):
    MatrixRow = ctypes.c_int * 8
    MatrixType = MatrixRow * 8

    path_project = so.obtain_proj_path()
    path_to_library = os.path.join(path_project, "shared/lib.so")
    lib = ctypes.CDLL(path_to_library)

    lib.entryPoint.argtypes = [ctypes.POINTER(MatrixType), ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.c_int]
    lib.entryPoint.restype = ctypes.c_int

    for i in range(8):
        for j in range(8):
            board_pos_send[i][7-j] = board_pos[i][j]

    board_np = np.array(board_pos_send, dtype=np.int32)
    board_c = MatrixType(*[MatrixRow(*[ctypes.c_int(x) for x in row]) for row in board_np])
    board_ptr = ctypes.pointer(board_c)
    front_end_turn_var = ctypes.c_int(front_end_turn)

    qtd_moves = len(list_of_moves)
    moves_c = (ctypes.c_int * qtd_moves)(*map(int, list_of_moves))  #Consider change qtd_moves for the fixed value 48, that the maximun possible chain 
    c_difficulty_var = ctypes.c_int(difficulty_var)
    test = lib.entryPoint(board_ptr, ctypes.c_int(qtd_moves), moves_c, ctypes.byref(front_end_turn_var), c_difficulty_var)
    front_end_turn = front_end_turn_var.value

    machine_move = []
    for i in range(4):
        machine_move.append(moves_c[i])
    machine_move.append(test)

    for i in range(8):
        for j in range(8):
            board_pos[i][7-j] = board_c[i][j]
    qtd_moves = 0
    list_of_moves.clear()

    return machine_move