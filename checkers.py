import os
import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import ctypes
import numpy as np
import pygame
import random

# Creation of matrice 8x8 board position
board_pos = [[0 for _ in range(8)] for _ in range(8)]
board_pos_show = [[0 for _ in range(8)] for _ in range(8)]
board_pos_send = [[0 for _ in range(8)] for _ in range(8)]

# Data struct to mantain the moves
qtd_moves = 0
list_of_moves = []
front_end_turn = int(0)
which_movement = 0  #This variable is specifically to tell if is the first square of the move, so it will not be duplicated
difficulty_var = 2

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
    start_game()

    firstMove = random.randint(1, 7)
    if firstMove == 1:
        board_pos[5][7-1] = 0
        board_pos[4][7-0] = 2
    if firstMove == 2:
        board_pos[5][7-1] = 0
        board_pos[4][7-2] = 2
    if firstMove == 3:
        board_pos[5][7-3] = 0
        board_pos[4][7-2] = 2
    if firstMove == 4:
        board_pos[5][7-3] = 0
        board_pos[4][7-4] = 2
    if firstMove == 5:
        board_pos[5][7-5] = 0
        board_pos[4][7-4] = 2
    if firstMove == 6:
        board_pos[5][7-5] = 0
        board_pos[4][7-6] = 2
    if firstMove == 7:
        board_pos[5][7-7] = 0
        board_pos[4][7-6] = 2
    print_board()

    #for i in range(8):
    #    for j in range(8):
    #        board_pos[i][j] = 0
    #board_pos[2][1] = 3
    #board_pos[2][3] = 3
    #board_pos[6][1] = 2
    #board_pos[6][3] = 2
    #print_board()

def format_moves_to_string(moves):
    columns = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
    x1, y1, x2, y2 = moves[0], moves[1], moves[2], moves[3]
    return f"{columns[y1]}{x1 + 1}-{columns[y2]}{x2 + 1}"

def set_end_board(winner):
    global board_pos
    for i in range(8):
        for j in range(8):
            if (i == 3 or i == 4) and (j == 3 or j == 4):
                if (winner == 3):
                    board_pos[i][j] = 3
                else:
                    board_pos[i][j] = 4
            else:
                board_pos[i][j] = 0


def submit_move():
    global list_of_moves, qtd_moves, which_movement
    #print("Submeter Jogada")
    #print(list_of_moves)
    #print(board_pos)
    if (not len(list_of_moves)):
        status_label.config(text="Você não chegou a fazer uma jogada!", fg="red")
        return
    list_of_moves.pop(); list_of_moves.pop()    #Take back the duplication from list_of_moves for the last move in chain
    which_movement = 0
    test = entry_point()
    if test[4] == 0:
        status_label.config(text="Jogada inválida", fg="red")  # Update the status label text
        path_to_audio = os.path.join(path_project, "Invalid.mp3")
        play_audio(path_to_audio)
    elif test[4] == 1:
        status_label.config(text=f"Jogada da máquina: {format_moves_to_string(test)}", fg="green")
        path_to_audio = os.path.join(path_project, "Move.wav")
        play_audio(path_to_audio)
    elif test[4] == 2:
        status_label.config(text=f"Jogada da máquina: {format_moves_to_string(test)}", fg="green")
        path_to_audio = os.path.join(path_project, "Attack.wav")
        play_audio(path_to_audio)
    elif test[4] == 3:
        set_end_board(test[4])
        status_label.config(text="Brancas ganharam", fg="blue")  # Update the status label text
        path_to_audio = os.path.join(path_project, "end.mp3")
        play_audio(path_to_audio)
    elif test[4] == 4:
        set_end_board(test[4])
        status_label.config(text="Negras ganharam", fg="blue")  # Update the status label text
        path_to_audio = os.path.join(path_project, "end.mp3")
        play_audio(path_to_audio)
    else:
        status_label.config(text=f"Jogada da máquina: {format_moves_to_string(test)}", fg="green")
    print_board()

def drop_move():
    global list_of_moves, which_movement
    print_board()
    which_movement = 0
    list_of_moves.clear()

def print_coordinates(event):
    global qtd_moves, list_of_moves, which_movement
    widget = event.widget
    if isinstance(widget, tk.Label) and (which_movement):
        drop_move()
        return
    if isinstance(widget, tk.Label):  # Verify if a piece has been clicked or a square
        col, row = widget.master.grid_info()["column"], str(7 - int(widget.master.grid_info()["row"]))
        #print(f"Coordenadas da peça: {col}{row}")
        list_of_moves.append(row); list_of_moves.append(col)
    else:
        col, row = widget.grid_info()["column"], str(7 - int((widget.grid_info()["row"])))
        #print(f"Coordenadas da square: {col}{row}")
        list_of_moves.append(row); list_of_moves.append(col)
        if (which_movement):
            list_of_moves.append(row); list_of_moves.append(col)
    widget.configure(bg="#06594b")
    status_label.config(text="Faça sua jogada", fg="black")  # Update the status label text
    #print(col, row)
    #print(list_of_moves)
    which_movement += 1

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

def play_audio(file_path):
    pygame.mixer.init()
    pygame.mixer.music.load(file_path)
    pygame.mixer.music.play()

def entry_point():
    global board_pos, board_pos_send ,list_of_moves, qtd_moves, front_end_turn, difficulty_var
    for i in range(8):
        for j in range(8):
            board_pos_send[i][7-j] = board_pos[i][j]

    #print("Board_pos_send:\n\n", board_pos_send)

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

    # print("\n\n\nImpressão do perturbado:\n")
    #for value in moves_c:
        #print(value)

    c_difficulty_var = ctypes.c_int(difficulty_var)

    #print("\n\n\nMandando a lista: ", list_of_moves, "\n\nDe tamanho: ", qtd_moves)

    test = lib.entryPoint(board_ptr, ctypes.c_int(qtd_moves), moves_c, ctypes.byref(front_end_turn_var), c_difficulty_var)

    #print("\n\nfront_end_turn foi como:", front_end_turn)

    front_end_turn = front_end_turn_var.value

    #print("\nfront_end_turn voltou como:", front_end_turn)

    machine_move = []
    for i in range(4):
        machine_move.append(moves_c[i])
    machine_move.append(test)

    # Parse the matrix send to C back to list pf lists in python
    for i in range(8):
        for j in range(8):
            board_pos[i][7-j] = board_c[i][j]
    qtd_moves = 0
    list_of_moves.clear()
    #return test
    return machine_move

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
btn_iniciar = tk.Button(menu, text="Reiniciar Jogo", bg=color_iniciar_button, fg="white", font=("Arial", 12, "bold"), command=start_game, relief=tk.RAISED, borderwidth=5)
btn_iniciar.pack(pady=10, padx=20, ipadx=10)

btn_definir = tk.Button(menu, text="Reiniciar (Máquina inicia jogando)", bg=color_definir_button, fg="white", font=("Arial", 12, "bold"), command=define_initial_pos, relief=tk.RAISED, borderwidth=5)
btn_definir.pack(pady=10, padx=20, ipadx=10)

difficulty_label = tk.Label(menu, text="Dificuldade:", font=("Arial", 12, "bold"), bg=color_secondary)
difficulty_label.pack(pady=10)

difficulty_spinbox = tk.Spinbox(menu, from_=2, to=15, font=("Arial", 12), width=10)
#difficulty_spinbox.set(2)  # Define a dificuldade inicial como 2
difficulty_spinbox.pack(pady=5)

selected_difficulty_label = tk.Label(menu, text="", font=("Arial", 12), bg=color_secondary)
selected_difficulty_label.pack()
selected_difficulty_label.config(text=f"Dificuldade selecionada: {2}")

def get_selected_difficulty():
    global difficulty_var
    selected_difficulty = int(difficulty_spinbox.get())
    selected_difficulty_label.config(text=f"Dificuldade selecionada: {selected_difficulty}")
    difficulty_var = selected_difficulty
    #print(difficulty_var)

def get_selected_difficulty_manually(condition):
    global difficulty_var
    if condition == 1:
        difficulty_var += 1
    else:
        difficulty_var -= 1
    selected_difficulty_label.config(text=f"Dificuldade selecionada: {difficulty_var}")


difficulty_button = tk.Button(menu, text="Definir Dificuldade", bg=color_submeter_button, fg="white", font=("Arial", 12, "bold"), command=get_selected_difficulty, relief=tk.RAISED, borderwidth=5)
difficulty_button.pack(pady=10, padx=20, ipadx=10)

btn_sair = tk.Button(menu, text="Sair do Programa", bg=color_sair_button, fg="white", font=("Arial", 12, "bold"), command=root.quit, relief=tk.RAISED, borderwidth=5)
btn_sair.pack(pady=10, padx=20, ipadx=10)

# Rounding submit button
btn_submeter = tk.Button(root, text="Submeter Jogada", bg=color_submeter_button, fg="white", font=("Arial", 16, "bold"), command=submit_move, relief=tk.RAISED, borderwidth=5)
btn_submeter.grid(row=1, column=0, columnspan=2, pady=0)

def show_information_popup():
    root = tk.Toplevel()
    root.title("Sobre nós")

    text_widget = tk.Text(root, wrap=tk.WORD, font=("Arial", 12))
    text_widget.pack(padx=10, pady=10, fill=tk.BOTH, expand=True)

    information = (
        "Esse trabalho é uma tarefa da disciplina de IPPD do curso de Ciência da Computação da Universidade Federal de Pelotas.\n"
        "Esse trabalho foi realizado pelos alunos Allan N. Schuch e Vinícius G. Peruzzi.\n"
        "Visite nossas páginas...\n"
        "https://github.com/allanschuch\n"
        "https://github.com/Vinigperuzzi\n"
        "https://vinigperuzzi.github.io/PersonalPage/\n"
        "\n\n\n"
        "Dica de atalhos:\n"
        "Os botões 'barra de espaço' e 'enter' submetem a jogada.\n"
        "O botão 'Esc' limpa uma jogada informada.\n"
        "Os botões 'cima' e 'baixo' são atalhos para aumentar ou diminuir a dificuldade, sem necessidade de definir no botão, basta mudar.\n"
        "O botão 'R' reinicia o tabuleiro.\n\n"
        "Esperamos que você goste e se divirta..."
    )

    text_widget.insert(tk.END, information)

btn_info = tk.Button(root, text="?", font=("Arial", 14, "bold"), bg=color_sair_button, fg=color_submeter_button, command=show_information_popup)
btn_info.grid(row=8, column=10, padx=10, pady=(0, 10))

def on_key(event):
    if event.keysym in ["Return", "space"]:
        submit_move()
    if event.keysym in ["Escape"]:
        drop_move()
    if event.keysym in ["r", "R"]:
        start_game()

def on_arrow_up(event):
    global difficulty_var
    if difficulty_var < 15:
        get_selected_difficulty_manually(1)
        difficulty_spinbox.configure(textvariable=difficulty_var)   

def on_arrow_down(event):
    global difficulty_var
    if difficulty_var > 2:
        get_selected_difficulty_manually(0)
        difficulty_spinbox.configure(textvariable=difficulty_var)      


root.bind("<Key>", on_key)
root.bind("<Up>", on_arrow_up)
root.bind("<Down>", on_arrow_down)

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
lib.entryPoint.argtypes = [ctypes.POINTER(MatrixType), ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.c_int]
lib.entryPoint.restype = ctypes.c_int

# Run graphic interface
root.mainloop()



#IDEIA: Clicar no numero do teclado para setar a dificuldade:

# Clicar "1" seta a dificulade para 1
# Clicar "2" seta a dificulade para 2
# ...
# Depois eu vou tentar atualizar dentro do spinbox também