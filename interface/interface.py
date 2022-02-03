from tkinter import *
from tkinter.font import Font
from tkinter.messagebox import showinfo
from tkinter import filedialog
import numpy as np
from random import random
from matplotlib import pyplot
from subprocess import PIPE, run

THREADS = [1, 2, 4, 6, 8, 12]
THREADS_STR = '"1 2 4 6 8 12"'
def out(command):
    result = run(command, stdout=PIPE, stderr=PIPE, universal_newlines=True, shell=True)
    return result.stdout

def getFiles():
    global experiment_filename
    experiment_filename = filedialog.askopenfilename(initialdir = ".",
                                      title = "Choose experiment files",
                                      filetypes = (("Text files",
                                                        "*.txt*"),
                                                       ("all files",
                                                        "*.*")))
    if len(experiment_filename) == 0:
        file_list_label.config(text = "No config file selected")
    else:
        file_list_label.config(text = experiment_filename)
    

def generateExperiment(N):
    try:
        insert_prob = float(e11.get())
        find_prob = float(e12.get())
        delete_prob = float(e13.get())
        assert(insert_prob >= 0)
        assert(find_prob >= 0)
        assert(delete_prob >= 0)
        full_prob = insert_prob + find_prob + delete_prob
        insert_prob /= full_prob
        find_prob /= full_prob
        delete_prob /= full_prob
    except (Exception):
        showinfo("Bad probabilities", "Bad format for probabilities. Default values used")
        insert_prob = 0.5
        find_prob = 0.4
        delete_prob = 0.1

    try:
        N = int(N)
    except(Exception):
        showinfo("Bad experiment number", "Bad experiment number. Default value used")
        N = 10
    
    try:
        op_count = int (e05.get())
    except(Exception):
        showinfo("Bad operation count", "Bad operation count. Default value used")
        op_count = 1000
    generated = []
    for file_idx in range(N):
        filename = f"generated_{file_idx}.txt"
        generated.append(filename)
        with open(filename, "w") as file:
            operation = random()
            if operation < insert_prob:
                operation = "INSERT"
            elif operation < insert_prob + find_prob:
                operation = "FIND"
            else:
                operation = "DELETE"
            lines = [f"{operation} {op_count}\n"]
            for i in range(op_count):
                value = int(random() * 1e6)
                lines.append(f"{value}\n")
            lines.append("\n")
            file.writelines(lines)
    return generated

def out_parse(output):
    return np.array(output.split("\n")[:-1], dtype = float)

def plot_runtimes(runtimes):
    for algo, times in runtimes.items():
        print(algo, times / times[0])
        pyplot.plot(THREADS, times / times[0], label = algo)
    pyplot.xlabel("Thread count")
    pyplot.ylabel("Playout Speedup")
    pyplot.legend()
    pyplot.show()

def work():
    operation_files = []
    try:
        with open(experiment_filename, "r") as experiment_file:
            operation_files = experiment_file.read().splitlines()
        if force_number_experiments.get():
            try:
                N = int(e0.get())
            except(Exception):
                showinfo("Bad experiment number", "Bad experiment number. Default value used")
                N = 10
            operation_files = operation_files[:N]
            l = len(operation_files)
            if l < N:
                operation_files += generateExperiment(N - l)
    except(Exception):
        operation_files = generateExperiment(e0.get())
    
    runtimes_dict = {}
    for i in range(len(algorithms)):
        if algs[i].get() is True:
            algorithm_name = algorithms[i][0]
            run_command = algorithms[i][1]
            runtimes_dict[algorithm_name] = np.zeros(len(THREADS))
            
            if run_command != "None":
                for file in operation_files:
                    if ("{filename}" in run_command):
                        current_run_command = run_command.replace("{filename}", file)
                    if ("{threads}" in run_command):
                        current_run_command = current_run_command.replace("{threads}", THREADS_STR)
                    runtimes = out_parse(out(current_run_command))
                    runtimes_dict[algorithm_name] += runtimes / len(operation_files)
    
    plot_runtimes(runtimes_dict)


experiment_filename = None
master = Tk()
Label(master, text = 'Number of experiments').grid(row = 0, ipadx = 10, ipady = 10)
Label(master, text = 'Operation count').grid(row = 1, ipadx = 10, ipady = 10)
Label(master, text = 'Probability of insert, find, delete').grid(row = 2, ipadx = 10, ipady = 10)
e0 = Entry(master)
e0.grid(row = 0, column = 1)
e0.insert(END, 3)
e05 = Entry(master)
e05.grid(row = 1, column = 1)
e05.insert(END, 1000)
e11 = Entry(master)
e11.grid(row = 2, column = 1, padx = 10)
e11.insert(END, 0.33)
e12 = Entry(master)
e12.grid(row = 2, column = 2, padx = 10)
e12.insert(END, 0.33)
e13 = Entry(master)
e13.grid(row = 2, column = 3, padx = 10)
e13.insert(END, 0.34)

algorithms = []
with open("./algorithms.txt", "r") as algofile:
    lines = algofile.read().splitlines()
    for line in lines:
        algorithms.append(line.split(' , '))

algs = []
for i in range(len(algorithms)):
    algs.append(BooleanVar())
    algs[-1].set(True)
    Checkbutton(master, text = algorithms[i][0], variable = algs[-1]).grid(row = 3, column = i, padx = 10, ipady = 10)

force_number_experiments = BooleanVar()
Checkbutton(master, text = "Force experiment count", 
            variable = force_number_experiments).grid(row = 4, column = 0, ipadx = 10, ipady = 10)

button_explore = Button(master, text = "Choose experiment config file", command = getFiles)
button_explore.grid(row = 80, padx = 10, pady = 10)
file_list_label = Label(master, text = "No config file selected")
file_list_label.grid(row = 80, column = 2, padx = 10, pady = 10)

button_run = Button(master, text="Run experiments", command = work).grid(row = 90, padx = 10, pady = 10)

button_exit = Button(master, text = "Close program", command = master.destroy).grid(row = 99, padx = 10, pady = 10)

master.mainloop()