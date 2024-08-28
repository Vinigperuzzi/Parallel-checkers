def format_moves_to_string(moves):
    columns = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
    x1, y1, x2, y2 = moves[0], moves[1], moves[2], moves[3]
    return f"{columns[y1]}{x1 + 1}-{columns[y2]}{x2 + 1}"

def set_end_board(winner, board_pos):
    for i in range(8):
        for j in range(8):
            if (i == 3 or i == 4) and (j == 3 or j == 4):
                if (winner == 3):
                    board_pos[i][j] = 3
                else:
                    board_pos[i][j] = 4
            else:
                board_pos[i][j] = 0

# Initialization of the piece's positions
# Setting white as 1
def set_initial_pos(board_pos):
    for i in range(3):
        for j in range(8):
            if (i + j) % 2 == 1:
                board_pos[i][j] = 1
            else:
                board_pos[i][j] = 0

    for i in range(3,5):
        for j in range(8):
            board_pos[i][j] = 0

    # Setting black as 2
    for i in range(5, 8):
        for j in range(8):
            if (i + j) % 2 == 1:
                board_pos[i][j] = 2
            else:
                board_pos[i][j] = 0
    return board_pos

def set_pos_show(board_pos_show, board_pos):
    for i in range(8):
        for j in range(8):
            board_pos_show[7-i][7-j] = board_pos[i][j]
    return board_pos_show