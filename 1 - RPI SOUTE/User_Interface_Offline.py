import tkinter as tk
from tkinter import messagebox



def User_Interface_Offline_FC():
    # Fonction pour simuler un texte LED
    def led_text_simulation(label, text, color="red"):
        label.config(text=text, fg=color)

    # Fonction déclenchée par les boutons
    def handle_general_button():
        messagebox.showinfo("Affond général", "Affond général activé!")

    def customize_text():
        new_text = entry_text.get()
        led_text_simulation(led_display, new_text)

    # Fonction pour activer/désactiver la balance 1
    def toggle_balance_1():
        if button_toggle_1.config('text')[-1] == "Activer":
            # Activer la balance 1
            balance_label_1.config(text="Balance 1", font=("Arial", 12), bg="#e3f2fd")
            beer_label_1.config(text="Nom de la bière : MAES", font=("Arial", 10), bg="#e3f2fd", fg="black")
            balance_display_1.config(text="SOUYE", font=("Texas LED", 20), bg="black", fg="red", width=20)
            button_toggle_1.config(text="Désactiver")
        else:
            # Désactiver la balance 1
            balance_label_1.config(text="Balance 1 inactive", font=("Arial", 12), bg="#e3f2fd")
            beer_label_1.config(text="Balance inactive", font=("Arial", 10), bg="#e3f2fd", fg="red")
            balance_display_1.config(text="", bg="#e3f2fd", fg="black")
            button_toggle_1.config(text="Activer")

    # Fonction pour activer/désactiver la balance 2
    def toggle_balance_2():
        if button_toggle_2.config('text')[-1] == "Activer":
            # Activer la balance 2
            balance_label_2.config(text="Balance 2", font=("Arial", 12), bg="#e3f2fd")
            status_label_2.config(text="Nom de la bière : LUPULUS", font=("Arial", 10), bg="#e3f2fd", fg="black")
            balance_display_2.config(text="SOUYE", font=("Texas LED", 20), bg="black", fg="red", width=20)
            button_toggle_2.config(text="Désactiver")
        else:
            # Désactiver la balance 2
            balance_label_2.config(text="Balance 2 inactive", font=("Arial", 12), bg="#e3f2fd")
            status_label_2.config(text="Balance inactive", font=("Arial", 10), bg="#e3f2fd", fg="red")
            balance_display_2.config(text="", bg="#e3f2fd", fg="black")
            button_toggle_2.config(text="Activer")

    # Fonction pour modifier la balance 1
    def modify_balance_1():
        messagebox.showinfo("Modification", "Modification de la Balance 1")

    # Fonction pour modifier la balance 2
    def modify_balance_2():
        messagebox.showinfo("Modification", "Modification de la Balance 2")

    # Fenêtre principale
    root = tk.Tk()
    root.title("Cercle Info - Balance 2.0")
    root.geometry("600x1024")
    root.config(bg="#e0e0e0")

    # Barre de titre en haut
    title_frame = tk.Frame(root, bg="#3949ab", height=50)
    title_frame.pack(fill=tk.X)

    title_label = tk.Label(title_frame, text="Cercle Info - Balance 2.0", fg="white", bg="#3949ab", font=("Arial", 18))
    title_label.pack(pady=10)

    # Gestion de l'écran LED
    led_frame = tk.Frame(root, bg="#f5f5f5", pady=10)
    led_frame.pack(fill=tk.X, padx=10, pady=10)

    led_label = tk.Label(led_frame, text="Par défaut : affichage des informations des balances", font=("Arial", 12), bg="#f5f5f5")
    led_label.pack()

    led_display = tk.Label(led_frame, text="Infos des Balances", font=("Texas LED", 24), bg="black", fg="red", width=50, height=2)
    led_display.pack(pady=10)

    # Boutons LED
    button_frame = tk.Frame(led_frame, bg="#f5f5f5")
    button_frame.pack()

    general_button = tk.Button(button_frame, text="Affond général !", bg="#3949ab", fg="white", relief="flat", command=handle_general_button)
    general_button.grid(row=0, column=0, padx=10)

    entry_text = tk.Entry(button_frame)
    entry_text.grid(row=0, column=1, padx=10)

    custom_button = tk.Button(button_frame, text="Personnaliser le texte", bg="#3949ab", fg="white", relief="flat", command=customize_text)
    custom_button.grid(row=0, column=2, padx=10)

    # Gestion des balances
    balances_frame = tk.Frame(root, pady=20)
    balances_frame.pack(fill=tk.BOTH, expand=True, padx=10)

    balance_label = tk.Label(balances_frame, text="GESTION DES BALANCES", font=("Arial", 14), anchor="center")
    balance_label.pack()

    # Balance 1
    balance_frame_1 = tk.Frame(balances_frame, bg="#e3f2fd", bd=1)
    balance_frame_1.pack(padx=10, pady=5, anchor="center")

    balance_label_1 = tk.Label(balance_frame_1, text="Balance 1", font=("Arial", 12), bg="#e3f2fd")
    balance_label_1.grid(row=0, column=0, padx=10, pady=10, columnspan=2)

    beer_label_1 = tk.Label(balance_frame_1, text="Nom de la bière : MAES", font=("Arial", 10), bg="#e3f2fd")
    beer_label_1.grid(row=1, column=0, padx=10)

    status_label_1 = tk.Label(balance_frame_1, text="Bières restantes : {Data_Send_Matrice['Nb_beer_1']}", font=("Arial", 10), bg="#e3f2fd")
    status_label_1.grid(row=1, column=1, padx=10)

    balance_display_1 = tk.Label(balance_frame_1, text="SOUYE", font=("Texas LED", 20), bg="black", fg="red", width=20)
    balance_display_1.grid(row=2, column=0, columnspan=2, pady=10)

    button_toggle_1 = tk.Button(balance_frame_1, text="Désactiver", bg="#3949ab", fg="white", relief="flat", command=toggle_balance_1)
    button_toggle_1.grid(row=3, column=0, padx=10, pady=10)

    button_modify_1 = tk.Button(balance_frame_1, text="Modifier la balance", bg="#3949ab", fg="white", relief="flat", command=modify_balance_1)
    button_modify_1.grid(row=3, column=1, padx=10, pady=10)

    # Balance 2 (inactive par défaut)
    balance_frame_2 = tk.Frame(balances_frame, bg="#e3f2fd", bd=1)
    balance_frame_2.pack(padx=10, pady=5, anchor="center")

    balance_label_2 = tk.Label(balance_frame_2, text="Balance 2 inactive", font=("Arial", 12), bg="#e3f2fd")
    balance_label_2.grid(row=0, column=0, padx=10, pady=10, columnspan=2)

    status_label_2 = tk.Label(balance_frame_2, text="Balance inactive", font=("Arial", 10), bg="#e3f2fd", fg="red")
    status_label_2.grid(row=1, column=0, padx=10)

    balance_display_2 = tk.Label(balance_frame_2, text="", bg="#e3f2fd", fg="black", width=20)
    balance_display_2.grid(row=2, column=0, columnspan=2, pady=10)

    button_toggle_2 = tk.Button(balance_frame_2, text="Activer", bg="#3949ab", fg="white", relief="flat", command=toggle_balance_2)
    button_toggle_2.grid(row=3, column=0, padx=10, pady=10)

    button_modify_2 = tk.Button(balance_frame_2, text="Modifier la balance", bg="#3949ab", fg="white", relief="flat", command=modify_balance_2)
    button_modify_2.grid(row=3, column=1, padx=10, pady=10)
    root.mainloop()

User_Interface_Offline_FC()

