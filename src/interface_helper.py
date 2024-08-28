import tkinter as tk
import pygame

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


def play_audio(file_path):
    try:
      pygame.mixer.init()
      pygame.mixer.music.load(file_path)
      pygame.mixer.music.play()
    except:
       print("Impossible to play audio")