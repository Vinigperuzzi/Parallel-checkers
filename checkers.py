import os
import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import ctypes
import numpy as np

# Creation of matrice 8x8 board position
board_pos = [[0 for _ in range(8)] for _ in range(8)]
board_pos_show = [[0 for _ in range(8)] for _ in range(8)]
board_pos_send = [[0 for _ in range(8)] for _ in range(8)]

# Data struct to mantain the moves
qtd_moves = 0
list_of_moves = []
front_end_turn = int(0)
which_movement = 0  #This variable is specifically to tell if is the first square of the move, so it will not be duplicated

# Initialization of the piece's positions
# Setting white as 1
def set_initial_pos():
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

def set_pos_show():
    for i in range(8):
        for j in range(8):
            board_pos_show[7-i][7-j] = board_pos[i][j]
            #board_pos_show[i][j] = board_pos[i][j]

# Function to obtain project's path in any SO (I guess?)
def obtain_proj_path():
    path_script = os.path.abspath(__file__)
    path_project = os.path.dirname(path_script)
    return path_project

# Obter o caminho para a pasta do projeto
path_project = obtain_proj_path()

def start_game():
    global qtd_moves, list_of_moves, which_movement, front_end_turn
    print("Iniciar Jogo")
    set_initial_pos()
    qtd_moves = 0
    list_of_moves.clear()
    status_label.config(text="Faça sua jogada", fg="black")  # Update the status label text
    which_movement = 0
    front_end_turn = 0

    print_board()

def define_initial_pos():
    print("Definir Posição Inicial")
    status_label.config(text="Funcionalidade a ser implementada, aguarde...", fg="yellow")  # Update the status label text
    print_board()

def submit_move():
    global list_of_moves, qtd_moves, which_movement
    print("Submeter Jogada")
    global qtd_moves, list_of_moves
    print(list_of_moves)
    print(board_pos)
    if (not len(list_of_moves)):
        status_label.config(text="Você não chegou a fazer uma jogada!", fg="red")
        return
    list_of_moves.pop(); list_of_moves.pop()    #Take back the duplication from list_of_moves for the last move in chain
    which_movement = 0
    test = entry_point()
    if not test:
        status_label.config(text="Jogada inválida", fg="red")  # Update the status label text
    else:
        status_label.config(text="Jogada registrada", fg="green")  # Update the status label text
    print_board()

def print_coordinates(event):
    global qtd_moves, list_of_moves, which_movement
    widget = event.widget
    if isinstance(widget, tk.Label):  # Verify if a piece has been clicked or a square
        col, row = widget.master.grid_info()["column"], str(7 - int(widget.master.grid_info()["row"]))
        print(f"Coordenadas da peça: {col}{row}")
        list_of_moves.append(row); list_of_moves.append(col)
        if (which_movement):
            list_of_moves.append(row); list_of_moves.append(col)
    else:
        col, row = widget.grid_info()["column"], str(7 - int((widget.grid_info()["row"])))
        print(f"Coordenadas da square: {col}{row}")
        list_of_moves.append(row); list_of_moves.append(col)
        if (which_movement):
            list_of_moves.append(row); list_of_moves.append(col)
    status_label.config(text="Faça sua jogada", fg="black")  # Update the status label text
    print(col, row)
    print(list_of_moves)
    which_movement += 1

def turn_on_highlight():
    global list_of_moves

# Function that create the colored squares and set png for pieces
pieces_size = 63
pieces = {
    "black_piece": Image.open(os.path.join(path_project, "black_piece.png")).resize((pieces_size, pieces_size)),
    "white_piece": Image.open(os.path.join(path_project, "white_piece.png")).resize((pieces_size, pieces_size)),
    "black_king": Image.open(os.path.join(path_project, "black_king.png")).resize((pieces_size, pieces_size)),
    "white_king": Image.open(os.path.join(path_project, "white_king.png")).resize((pieces_size, pieces_size)),
}
def print_board():
    set_pos_show()
    for i in range(8):
        for j in range(8):
            square_color = color_secondary if (i + j) % 2 == 0 else color_black_squares
            square = tk.Frame(board, width=67, height=67, bg=square_color)
            square.grid(row=i, column=j)
            square.bind("<Button-1>", print_coordinates)  # Adiciona o evento de clique a cada square

            # adding pieces to squares based on board_pos_show info
            if board_pos_show[i][j] == 1:
                piece = ImageTk.PhotoImage(pieces["white_piece"])
                label_piece = tk.Label(square, image=piece, bg=square_color)
                label_piece.image = piece
                label_piece.pack(pady=0)
                label_piece.bind("<Button-1>", print_coordinates)
            elif board_pos_show[i][j] == 2:
                piece = ImageTk.PhotoImage(pieces["black_piece"])
                label_piece = tk.Label(square, image=piece, bg=square_color)
                label_piece.image = piece
                label_piece.pack(pady=0)
                label_piece.bind("<Button-1>", print_coordinates)
            elif board_pos_show[i][j] == 3:
                piece = ImageTk.PhotoImage(pieces["white_king"])
                label_piece = tk.Label(square, image=piece, bg=square_color)
                label_piece.image = piece
                label_piece.pack(pady=0)
                label_piece.bind("<Button-1>", print_coordinates)
            elif board_pos_show[i][j] == 4:
                piece = ImageTk.PhotoImage(pieces["black_king"])
                label_piece = tk.Label(square, image=piece, bg=square_color)
                label_piece.image = piece
                label_piece.pack(pady=0)
                label_piece.bind("<Button-1>", print_coordinates)

def entry_point():
    global board_pos, board_pos_send ,list_of_moves, qtd_moves, front_end_turn
    for i in range(8):
        for j in range(8):
            board_pos_send[i][7-j] = board_pos[i][j]

    print("Board_pos_send:\n\n", board_pos_send)

    # Convert the matrice Python for NumPy array
    board_np = np.array(board_pos_send, dtype=np.int32)
    '''Como uma matriz em python é uma lista de listas, para ser reconhecida pelo C, é necessário converter
    essa estrutura para um array de arrays, que é reconhecido de forma diferente pelo C.
    O argumento np.int32 força que o array NumPy tenha o tipo de inteiros de 32 bits, que é o mesmo tipo
    usado no MatrixType (ctypes.c_int * 8)'''

    # Convert each element of array NumPy for c_int_Array_8
    board_c = MatrixType(*[MatrixRow(*[ctypes.c_int(x) for x in row]) for row in board_np])
    ''''Aqui já é uma lógica meio complicada, mas é explicada pela própria documentação do NumPy.
    Os dados da matriz estão no formato reconhecido pelo C, mas cada linha ainda precisa ser convertida
    em um c_int_Array_8, que nada mais é do que linhas de 8 elementos para cada coluna da matriz.
    A estrutura do for, apesar de estar na forma iter em uma linha só, é um for dentro de for que percorre
    cada elemento de cada linha.'''

    board_ptr = ctypes.pointer(board_c)

    front_end_turn_var = ctypes.c_int(front_end_turn)


    # Wrapping the array of movements
    qtd_moves = len(list_of_moves)
    moves_c = (ctypes.c_int * qtd_moves)(*map(int, list_of_moves))  #Consider change qtd_moves for the fixed value 48, that the maximun possible chain 

    print("\n\n\nImpressão do perturbado:\n")
    for value in moves_c:
        print(value)

    print("\n\n\nMandando a lista: ", list_of_moves, "\n\nDe tamanho: ", qtd_moves)

    test = lib.entryPoint(board_ptr, ctypes.c_int(qtd_moves), moves_c, ctypes.byref(front_end_turn_var))

    print("\n\nfront_end_turn foi como:", front_end_turn)

    front_end_turn = front_end_turn_var.value

    print("\nfront_end_turn voltou como:", front_end_turn)

    # Parse the matrix send to C back to list pf lists in python
    for i in range(8):
        for j in range(8):
            board_pos[i][7-j] = board_c[i][j]
    qtd_moves = 0
    list_of_moves.clear()
    return test

# Color - for style
color_primary = "#53a08e"  # Verde suave
color_secondary = "#f0f0f0"  # Cinza suave
color_black_squares = "black"  # Cor preta para as squares pretas
color_iniciar_button = "#ebc588"  # Laranja
color_definir_button = "#e0dba4"  # Azul
color_sair_button = "#3b0032"  # Vermelho
color_submeter_button = "#7ebea3"  # Verde mais vibrante
color_highlight = "#0dd1b0"     #verdezinho bonitão

# Principal window creation
root = tk.Tk()
root.title("Checkers - IPPD")
root.geometry("1366x768")
root.configure(bg=color_primary)

# Relative path to background image
path_background = os.path.join(path_project, "fundo_damas.png")

# Load background image
background_image = Image.open(path_background)
width, height = 1366, 768  # Dimensões do board
background_image = background_image.resize((width, height))
background_image = ImageTk.PhotoImage(background_image)

# Widget to show background image
label_background = tk.Label(root, image=background_image)
label_background.place(x=0, y=0, relwidth=1, relheight=1)

# Checkers board
board = ttk.LabelFrame(root, width=480, height=480, borderwidth=0, relief=tk.RAISED)
board.grid(row=0, column=0, padx=150, pady=30)

# Function that creates the colored squares and sets png for pieces
def create_square(row, col):
    square_color = color_secondary if (row + col) % 2 == 0 else color_black_squares
    square = tk.Frame(board, width=67, height=67, bg=square_color)
    square.grid(row=row, column=col)
    square.bind("<Button-1>", print_coordinates)  # Add click event to each square

# Create the board grid and squares
for i in range(8):
    for j in range(8):
        create_square(i, j)

# New label to display the current status of the move
status_label = tk.Label(board, text="Faça sua jogada", font=("Arial", 14, "bold"), bg=color_primary, fg="black")
status_label.grid(row=8, column=0, columnspan=8)

# Action Buttons
menu = ttk.LabelFrame(root, text="Menu de interação", width=200, height=400, borderwidth=5, relief=tk.RIDGE)
menu.grid(row=0, column=1, padx=(0, 10), pady=10)

# Rounding buttons
btn_iniciar = tk.Button(menu, text="Iniciar Jogo", bg=color_iniciar_button, fg="white", font=("Arial", 12, "bold"), command=start_game, relief=tk.RAISED, borderwidth=5)
btn_iniciar.pack(pady=10, padx=20, ipadx=10)

btn_definir = tk.Button(menu, text="Definir Posição Inicial", bg=color_definir_button, fg="white", font=("Arial", 12, "bold"), command=define_initial_pos, relief=tk.RAISED, borderwidth=5)
btn_definir.pack(pady=10, padx=20, ipadx=10)

btn_sair = tk.Button(menu, text="Sair do Programa", bg=color_sair_button, fg="white", font=("Arial", 12, "bold"), command=root.quit, relief=tk.RAISED, borderwidth=5)
btn_sair.pack(pady=10, padx=20, ipadx=10)

# Rounding submit button
btn_submeter = tk.Button(root, text="Submeter Jogada", bg=color_submeter_button, fg="white", font=("Arial", 16, "bold"), command=submit_move, relief=tk.RAISED, borderwidth=5)
btn_submeter.grid(row=1, column=0, columnspan=2, pady=0)

# Stule for labelFrame
s = ttk.Style()
s.configure("board.TLabelframe", background=color_primary)
s.configure("Menu.TLabelframe", background=color_secondary)

set_initial_pos()
print_board()

#--------------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------------
#Conection with C -- setup for call the funcions

# Define a custom type to hold a matrice 8x8 for c_int_array_8
MatrixRow = ctypes.c_int * 8
MatrixType = MatrixRow * 8

# Load lib.so
path_to_library = os.path.join(path_project, "lib.so")
lib = ctypes.CDLL(path_to_library)

#Define types to pass as arguments and the return (int mat[8][8], int qtd, int array[qtd])
lib.entryPoint.argtypes = [ctypes.POINTER(MatrixType), ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
lib.entryPoint.restype = ctypes.c_int

# Run graphic interface
root.mainloop()