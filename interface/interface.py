from tkinter import *
from tkinter.font import Font
from tkinter import filedialog
import numpy as np
from subprocess import PIPE, run

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

def work():
    global experiment_filename
    global algorithms
    print(experiment_filename)
    operation_files = []
    try:
        with open(experiment_filename, "r") as experiment_file:
            operation_files = experiment_file.read().splitlines()
                

    except(Exception):
        operation_files = ["a.txt"]
    
    for i in range(len(algorithms)):
        if algs[i].get() is True:
            algorithm_name = algorithms[i][0]
            compile_command = algorithms[i][1]
            run_command = algorithms[i][2]
            if compile_command != "None":
                out(compile_command)
            
            if run_command != "None":
                runtimes = []
                for file in operation_files:
                   current_run_command = run_command.replace("{filename}", file)
                   runtimes.append(int(out(current_run_command)))
                print(algorithm_name, runtimes)

    


experiment_filename = None
master = Tk()
Label(master, text = 'Number of experiments').grid(row = 0)
Label(master, text = 'Percentages of insert, find, delete').grid(row = 1)
e0 = Entry(master)
e0.grid(row = 0, column = 1)
e0.insert(END, 10)
e11 = Entry(master)
e11.grid(row = 1, column = 1)
e11.insert(END, 0.5)
e12 = Entry(master)
e12.grid(row = 1, column = 2)
e12.insert(END, 0.4)
e13 = Entry(master)
e13.grid(row = 1, column = 3)
e13.insert(END, 0.1)

algorithms = []
with open("./algorithms.txt", "r") as algofile:
    lines = algofile.read().splitlines()
    for line in lines:
        algorithms.append(line.split(' , '))

algs = []
for i in range(len(algorithms)):
    algs.append(BooleanVar())
    algs[-1].set(True)
    Checkbutton(master, text = algorithms[i][0], variable = algs[-1]).grid(row = 2, column = i)

force_number_experiments = Entry(master)
Checkbutton(master, text = "Force experiment count", 
            variable = force_number_experiments).grid(row = 4, column = 0)

button_explore = Button(master, text = "Find file list", command = getFiles).grid(row = 97)

button_run = Button(master, text="Run experiments", command = work).grid(row = 99)


master.mainloop()