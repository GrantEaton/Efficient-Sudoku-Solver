// testing.cpp : Defines the entry point for the console application.
//

#include "sudoku.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm> 
#include <ctime>

using namespace std;

//board holds the sudoku board with "-" as empty locations
vector<vector<char>> board;
//potential numbers to be placed in each spot on the board. Using hashMap for O(1) speed (could have used array of 9 elements, also)
vector<vector<map<int, int>>> boardPotentials;
//size of a given sub-grid in a sudoku (for 9x9 its 3, 4x4 its 2)
int boxSize; 

//a state of the game, which holds a board and its potential values
struct BoardState {
	vector<vector<char>> board;
	vector<vector<map<int, int>>> boardPotentials;
	BoardState(vector<vector<char>> b, vector<vector<map<int, int>>> p) {
		board = b;
		boardPotentials = p;
	}
};

//prints a given board
void printSudoku(vector<vector<char>> board) {
	for (int r = 0; r < board[0].size(); r++) {
		
		cout << '\n';
		for (int c = 0; c < board[0].size(); c++) {

			cout << board[r][c];
		}
		//cout << '\n';
	}
}

//prints a given boards potentials
void printPotentials( vector<vector<map<int, int>>> boardPotentials) {
	typedef map<int, int>::const_iterator MapIterator;
	for (int r = 0; r < boardPotentials[1].size(); r++) {

		cout << '\n';
		for (int c = 0; c < boardPotentials[1].size(); c++) {
			cout << "[";
			for (MapIterator iter = boardPotentials[r][c].begin(); iter != boardPotentials[r][c].end(); iter++)
			{
				cout <<  iter->first;
			}
			cout << "]";
		}
		cout << '\n';
	}
}

// returns an int, that represents which "box" (sub-grid of the board) a given spot on the board is in.
// int is in rc format (ex. 32 = row 3, col 2) where rc represent the bottom right location of the quare
// ex. given (getRxCofSquare(0,1) on the 2x2 grid below
// 12|34
// 56|78
// -- --
// 91|23
// 45|67
// will return 11, (row 1, col 1)
int getRxCofSquare(int row, int col) {
	
	if (row > boardSize - 1 || col > boardSize - 1) {
		return -1;
	}
	else if (row < boxSize) {
		if (col < boxSize) {
			return  (boxSize * 10) - 10 + boxSize - 1;
		}
		else if (col < boxSize * 2) {
			return (boxSize * 10) - 10 + (boxSize * 2) - 1;
		}
		else
			return (boxSize * 10) - 10 + (boxSize * 3) - 1;
	}
	else if (row < boxSize * 2) {
		if (col < boxSize) {
			return (boxSize * 20) - 10 + boxSize - 1;
		}
		else if (col < boxSize * 2) {
			return (boxSize * 20) - 10 + (boxSize * 2) - 1;
		}
		else
			return (boxSize * 20) - 10 + (boxSize * 3) - 1;
	}
	else if (row < boxSize * 3) {
		if (col < boxSize) {
			return (boxSize * 30) - 10 + boxSize - 1;
		}
		else if (col < boxSize * 2) {
			return (boxSize * 30) - 10 + (boxSize * 2) - 1;
		}
		else
			return (boxSize * 30) - 10 + (boxSize * 3) - 1;
	}
	else return -1;
}

// add 1-boardSize to the potentials (ex 9x9 has 1-9 for each spot)
//only used after reading in text file
void fillPotentials() {
	for (int r = 0; r < boardSize; r++) {
		for (int c = 0; c < boardSize; c++) {
			map<int, int> temp;
			for (int k = 1; k <= boardSize; k++) {
				//insert a 1-boardSize into the map for every location
				temp.insert({ k,k });
				
				boardPotentials[r][c] = temp;
			}
			
		}
	}
}


//takes in a number and updates the row, column and box (sub-grid) that the number is in
BoardState updatePotentials(int num, int row, int col, vector<vector<char>> board, vector<vector<map<int, int>>> boardPotentials) {

	//get the box (sub grid) that the given number
	int boxRow = getRxCofSquare(row, col)/10;
	int boxCol = getRxCofSquare(row, col) % 10;
	// update the potentials for the exact spot to nothing
	boardPotentials[row][col].clear();
	//update the row and column
	for (int j = 0; j < boardSize; j++) {
		boardPotentials[row][j].erase(num);
		boardPotentials[j][col].erase(num);

	}
	//update the given box (sub grid) that the number is in
	for (int r = boxRow - (boxSize - 1); r <= boxRow; r++) {
		for (int c = boxCol - (boxSize - 1); c <= boxCol; c++) {
			//cout << "deleting " << num << " From row " << r << " and Col " << c << '\n';
			boardPotentials[r][c].erase(num);
			
		}
	}
	return BoardState(board, boardPotentials);
}

// return if the given state is solveable
// returns false if a spot is not assigned to a number(has a "-") but has 0 potentials
bool isSolveable(vector<vector<char>> board, vector<vector<map<int, int>>> boardPotentials) {
	for (int r = 0; r < boardSize; r++) {
		for (int c = 0; c < boardSize; c++) {
			if (board[r][c] == '-' && boardPotentials[r][c].empty()) {
				//cout << "row: " << r << " col: " << c;
				return false;
			}
		}
	}
	return true;
}

//return the next location we want to search as a rc int (ex. 27 = row 2, colw 7)
int getNextLocation(vector<vector<char>> board, vector<vector<map<int, int>>> boardPotentials) {
	//using MRV (Minimum Remaining (potential) Values to decide which location to select next
	//minimum represents the lowest amount of potential values we have found so far
	int minimum = 100;
	// will return 999 if board is full with #s, meaning we solved it, otherwise an int as rc
	int ret = 999;

	//check if solveable first
	if (isSolveable(board,boardPotentials)){
		for (int r = 0; r < boardSize; r++) {
			for (int c = 0; c < boardSize; c++) {
				int size = boardPotentials[r][c].size();
				//of size is 0, then its just a spot with a value already, skip to the next loop iteration.
				if (size == 0) {
					continue;
				}
				//if size 1, return immediately for efficiency
				else if( size == 1) {
					return r * 10 + c;
				}
				//if we find a new minimum, update it to that location
				else if (size < minimum){

					minimum = size;
					ret = r * 10 + c;
				}
				//Implementation of a random possibility to improve speed.
				// the getNextLocation function normally searches col 0-8 in row 0, then 1, 2 etc...
				// This is inefficient for complex sudokus, because we are constantly searching the same row, over and over
				// this results in updating less potential values, so what were going to do is set a chance for the function
				//to skip that location and set it to a different one, given it has the same MRV
				else if (size == minimum) {
					if ((rand() % 100 + 1) <= 25) { //chance set to 15% for each equal MRV
						minimum = size;
						ret = r * 10 + c;
					}
				}
			}
		}
		return ret;
	}
	else return -1;
}

//solve with Depth-First Search (DFS), backtracking (via recursion) and foreward-checking
bool solveSudoku(vector<vector<char>> board, vector<vector<map<int, int>>> boardPotentials) {
	
	//go until we break
	while (true) {

		//get next location as int rc (ex. rc= 49, so row = 4, col = 9)
		int rc = getNextLocation(board, boardPotentials);
		int r = rc / 10;
		int c = rc % 10;
		// we hit a location where the board is full with #s, thus its solved. 
		if (rc == 999) {
			break;
		}
		//we hit a location where the sudoku is unsolveable, start backtracking
		else if (rc == -1) {
			return false;
		}
		
		//if there is only one potential, it must go in that location 
		if (boardPotentials[r][c].size() == 1) {
			//cout << boardPotentials[r][c].begin()->first << '\n';
			
			//update the board with the number
			board[r][c] = '0'+ boardPotentials[r][c].begin()->first;
			//cout << "adding " << boardPotentials[r][c].begin()->first << " to row " << r << " and Col " << c << '\n';

			//update the potentials
			BoardState updatedState = updatePotentials(boardPotentials[r][c].begin()->first, r, c,board,boardPotentials);
			//update the board and potentials in the function
			board = updatedState.board;
			boardPotentials = updatedState.boardPotentials;
			
		}
		//else, were going to have to try all the potential values to see which is correct
		else {
			//cout << '\n' << '\n' << " PICKING FROM 2 OR MORE " << '\n' << '\n';

			//iterate through the potentials and try each one, via recursion
			typedef map<int, int>::const_iterator MapIterator;
			for (MapIterator iter = boardPotentials[r][c].begin(); iter != boardPotentials[r][c].end(); iter++)
			{

				int num = iter->first;
				//update board and potentials
				board[r][c] = '0' + num;
				BoardState updatedState = updatePotentials(num, r, c,board,boardPotentials);
				
				//cout << "adding " << num << " to row " << r << " and Col " << c << '\n';
				
				//recursively try all the potentials
				if (solveSudoku(updatedState.board, updatedState.boardPotentials)) {
					return true;
				}
				
				
			}
			return false;
		}
		
	}
	
	//if we left the while loop, the board is solved
	//cout << "solved" << '\n';

	printSudoku(board);
	//printPotentials(boardPotentials);
	return true;

}

int main(int argc, char* argv[])
{
	cout << argv[1];
	
	//read in the sodoku file
	fstream sudokuFile;
	ifstream ifile(argv[1]);
	if (!ifile) {
		cout << "\nThe file does not exist";
		return 0;
	}
	sudokuFile.open("9x9a.txt", fstream::in);
	
	//first two numbers on the file represent the row x column
	sudokuFile >> row;
	sudokuFile >> col;
	boardSize = row;

	//9x9 has a 3 box size. 4x4 has a 2 boxsize
	boxSize= sqrt(boardSize);

	//ignore the first line with the row and column numbers
	sudokuFile.ignore(80, '\n');

	// expand and add to the board
	for (int r = 0; r < row; r++) {
		board.resize(row);
		boardPotentials.resize(row);
		for (int c = 0; c < col*col; c++) {
			board[r].resize(col * col);
			boardPotentials[r].resize(col * col);
			sudokuFile >> board[r][c];
			//cout << board[r][i];
		}
	}
		//cout << '\n';
	
	// add 1-boardSize to the potentials (ex 9x9 has 1-9 for each spot)
	fillPotentials();

	//take the initial board and update its potentials
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col*col; c++) {
			int a = (board[r][c] - '0') % 48;
			if (a != -3) {
				BoardState updatedState = updatePotentials(a, r, c,board,boardPotentials);
				board = updatedState.board;
				boardPotentials = updatedState.boardPotentials;
			}
			//cout << board[r][i];
		}
		//cout << '\n';
	}
	
	sudokuFile.close();
	//cout << '\n'+board[1].size();

	//use a clock to time the process
	clock_t begin = clock();
	solveSudoku(board,boardPotentials);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "\nExecution time: " << elapsed_secs << " seconds.\n";
	
	

	return 0;
}


