# Efficient-Sudoku-Solver
This is a c++ Sudoku solver that used recursive backtracking and forward-checking with minimum remaining values to quickly solve the puzzle.



TO RUN:
The program is designed to be run in the terminal by taking arguments. You can, however, run it in an IDE, you will just want to manually change the arguments, or modify the code to run the file you want. I will give a small explanation of how to do so down below in the "RUN IN AN IDE" section.

RUN IN THE TERMINAL:
1. Download the whole repository and drag and drop all the files in a directory you want to run it in. For this example, i will use "Efficient-Sudoku-Solver" as my directory.
2. cd into the directory (ex. "cd desktop/Efficient-Sudoku-Solver")
3. Compile the program with a c++ compiler (you will need to have gcc installed) using "c++ -std=c++11 -o sudoku *.cpp" (without quotes)
4. Run the program with "sudoku 9x9a.txt" (without the quotes, changing the text file to whichever you want).

RUN IN AN IDE:
1. Download the whole repository and drag and drop all the files in a new project.
2. Make sure that the text files are in the same directory as the project.
3. Navigate to line 295, which is "sudokuFile.open(argv[1], fstream::in);" and change "argv[1]" to "sudokuname.txt" or whatever text you want to run. ex. sudokuFile.open("4x4a.txt", fstream::in);
4. Run. If you get a file not found error, go into your files and check that the sudoku files are in the same directory.

