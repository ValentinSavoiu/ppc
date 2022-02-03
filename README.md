# PPC

Interface usage:
1. Write your project in a folder 
2. In interface/algorithms.txt add a line:
{Name of your algorithm} , {shell command for running}
shell command should contain {filename} and {threads}, this will give the program the filename and the list of numbers of threads as command line arguments.
3. From the interface folder run interface.py

Useful tips:
- The file requested by "find experiment list" should contain filenames, one per line, absolute or relative to the interface folder, each pointing to a file with operations to be executed
- The operations files should start with the operation types ("FIND", "INSERT" or "DELETE") and number of operations, and then for each line, they should contain the value for the operation 
- if no files are provided, the algorithm will randomly generate operation files until it reaches the inserted number of experiments.
- force experiment count will truncate the given experiments or add random experiments, ensuring that exactly the inserted number of experiments occur.


