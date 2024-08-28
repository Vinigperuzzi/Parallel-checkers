import os
import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import numpy as np
import random
import src.so_helper as so_h
import src.board as b
import src.interface_helper as ih
import src.call_c as c

# Color - for style
color_primary = "#53a08e"
color_secondary = "#f0f0f0"
color_black_squares = "black"
color_iniciar_button = "#ebc588"
color_definir_button = "#e0dba4"
color_sair_button = "#3b0032"
color_submeter_button = "#7ebea3"
color_highlight = "#0dd1b0"

def start_game():
    global qtd_moves, list_of_moves, which_movement, front_end_turn, board_pos
    print("Iniciar Jogo")
    board_pos = b.set_initial_pos(board_pos)
    qtd_moves = 0
    list_of_moves.clear()
    status_label.config(text="Faça sua jogada", fg="black")  # Update the status label text
    which_movement = 0
    front_end_turn = 0

    print_board()

def define_initial_pos():
    start_game()
    b.define_initial_pos(board_pos)
    print_board()

def submit_move():
    global list_of_moves, qtd_moves, which_movement, board_pos, board_pos_send, front_end_turn, difficulty_var
    if (not len(list_of_moves)):
        status_label.config(text="Você não chegou a fazer uma jogada!", fg="red")
        return
    list_of_moves.pop(); list_of_moves.pop()    #Take back the duplication from list_of_moves for the last move in chain
    which_movement = 0
    test = c.entry_point(board_pos, board_pos_send ,list_of_moves, qtd_moves, front_end_turn, difficulty_var)
    if test[4] == 0:
        status_label.config(text="Jogada inválida", fg="red")  # Update the status label text
        path_to_audio = os.path.join(path_project, "audio/Invalid.mp3")
        ih.play_audio(path_to_audio)
    elif test[4] == 1:
        status_label.config(text=f"Jogada da máquina: {b.format_moves_to_string(test)}", fg="green")
        path_to_audio = os.path.join(path_project, "audio/Move.wav")
        ih.play_audio(path_to_audio)
    elif test[4] == 2:
        status_label.config(text=f"Jogada da máquina: {b.format_moves_to_string(test)}", fg="green")
        path_to_audio = os.path.join(path_project, "audio/Attack.wav")
        ih.play_audio(path_to_audio)
    elif test[4] == 3:
        b.set_end_board(test[4], board_pos)
        status_label.config(text="Brancas ganharam", fg="blue")  # Update the status label text
        path_to_audio = os.path.join(path_project, "audio/end.mp3")
        ih.play_audio(path_to_audio)
    elif test[4] == 4:
        b.set_end_board(test[4], board_pos)
        status_label.config(text="Negras ganharam", fg="blue")  # Update the status label text
        path_to_audio = os.path.join(path_project, "audio/end.mp3")
        ih.play_audio(path_to_audio)
    else:
        status_label.config(text=f"Jogada da máquina: {b.format_moves_to_string(test)}", fg="green")
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
        list_of_moves.append(row); list_of_moves.append(col)
    else:
        col, row = widget.grid_info()["column"], str(7 - int((widget.grid_info()["row"])))
        list_of_moves.append(row); list_of_moves.append(col)
        if (which_movement):
            list_of_moves.append(row); list_of_moves.append(col)
    widget.configure(bg="#06594b")
    status_label.config(text="Faça sua jogada", fg="black")  # Update the status label text
    which_movement += 1

def print_board():
    global board_pos_show
    board_pos_show = b.set_pos_show(board_pos_show, board_pos)
    for i in range(8):
        for j in range(8):
            square_color = color_secondary if (i + j) % 2 == 0 else color_black_squares
            square = tk.Frame(board, width=67, height=67, bg=square_color)
            square.grid(row=i, column=j)
            square.bind("<Button-1>", print_coordinates)

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

# Function that creates the colored squares and sets png for pieces
def create_square(row, col):
    square_color = color_secondary if (row + col) % 2 == 0 else color_black_squares
    square = tk.Frame(board, width=67, height=67, bg=square_color)
    square.grid(row=row, column=col)
    square.bind("<Button-1>", print_coordinates)  # Add click event to each square

def get_selected_difficulty():
    global difficulty_var
    selected_difficulty = int(difficulty_spinbox.get())
    selected_difficulty_label.config(text=f"Dificuldade selecionada: {selected_difficulty}")
    difficulty_var = selected_difficulty

def get_selected_difficulty_manually(condition):
    global difficulty_var
    if condition == 1:
        difficulty_var += 1
    else:
        difficulty_var -= 1
    selected_difficulty_label.config(text=f"Dificuldade selecionada: {difficulty_var}")

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



# Creation of matrice 8x8 board position
board_pos = [[0 for _ in range(8)] for _ in range(8)]
board_pos_show = [[0 for _ in range(8)] for _ in range(8)]
board_pos_send = [[0 for _ in range(8)] for _ in range(8)]

# Data struct to mantain the moves
qtd_moves = 0
list_of_moves = []
front_end_turn = int(0)
which_movement = 0
difficulty_var = 2

path_project = so_h.obtain_proj_path()

# Function that create the colored squares and set png for pieces
pieces_size = 63
pieces_path = path_project + '/img'
pieces = {
    "black_piece": Image.open(os.path.join(pieces_path, "black_piece.png")).resize((pieces_size, pieces_size)),
    "white_piece": Image.open(os.path.join(pieces_path, "white_piece.png")).resize((pieces_size, pieces_size)),
    "black_king": Image.open(os.path.join(pieces_path, "black_king.png")).resize((pieces_size, pieces_size)),
    "white_king": Image.open(os.path.join(pieces_path, "white_king.png")).resize((pieces_size, pieces_size)),
}

# Principal window creation
root = tk.Tk()
root.title("Checkers - IPPD")
root.geometry("1366x768")
root.configure(bg=color_primary)

# Relative path to background image
path_background = os.path.join(path_project, "img/fundo_damas.png")

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
difficulty_spinbox.pack(pady=5)

selected_difficulty_label = tk.Label(menu, text="", font=("Arial", 12), bg=color_secondary)
selected_difficulty_label.pack()
selected_difficulty_label.config(text=f"Dificuldade selecionada: {2}")

difficulty_button = tk.Button(menu, text="Definir Dificuldade", bg=color_submeter_button, fg="white", font=("Arial", 12, "bold"), command=get_selected_difficulty, relief=tk.RAISED, borderwidth=5)
difficulty_button.pack(pady=10, padx=20, ipadx=10)

btn_sair = tk.Button(menu, text="Sair do Programa", bg=color_sair_button, fg="white", font=("Arial", 12, "bold"), command=root.quit, relief=tk.RAISED, borderwidth=5)
btn_sair.pack(pady=10, padx=20, ipadx=10)

# Rounding submit button
btn_submeter = tk.Button(root, text="Submeter Jogada", bg=color_submeter_button, fg="white", font=("Arial", 16, "bold"), command=submit_move, relief=tk.RAISED, borderwidth=5)
btn_submeter.grid(row=1, column=0, columnspan=2, pady=0)

btn_info = tk.Button(root, text="?", font=("Arial", 14, "bold"), bg=color_sair_button, fg=color_submeter_button, command=ih.show_information_popup)
btn_info.grid(row=8, column=10, padx=10, pady=(0, 10))   

root.bind("<Key>", on_key)
root.bind("<Up>", on_arrow_up)
root.bind("<Down>", on_arrow_down)

# Style for labelFrame
s = ttk.Style()
s.configure("board.TLabelframe", background=color_primary)
s.configure("Menu.TLabelframe", background=color_secondary)

board_pos = b.set_initial_pos(board_pos)
print_board()

# Run graphic interface
root.mainloop()