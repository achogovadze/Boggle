
/*
* File: Boggle.cpp
* ----------------
* Name: [TODO: enter name here]
* Section: [TODO: enter section leader here]
* This file is the main starter file for Assignment #4, Boggle.
* [TODO: extend the documentation]
*/

#include <iostream>
#include <math.h>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include "point.h"

using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;

const string STANDARD_CUBES[16]  = {
	"AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
	"AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
	"DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
	"EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25]  = {
	"AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
	"AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
	"CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
	"DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
	"FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();
void setupBoard(Grid<char>& board);
void displayBoard(Grid<char>& board);
void addLetters(int numRows, Grid<char>& board);
string getRandomString(Vector<string>& strings);
void fillBoard(Grid<char>& board, string toPrint, int numRows);
void askPlayerForLetters(Vector<string>& letters, int numRows);
Vector<Point> points();
void findAllWords(Map<string, Grid<bool> >& map, Grid<char>& board, Grid<bool> used, Lexicon& lex, Set<string>& words, string soFar, int x, int y);
void setupUsedCubes(Grid<bool>& used, int numRows);
void generatePossibleWords(Grid<char>& board, Grid<bool>& used, Map<string, Grid<bool> >& map, Set<string>& words);
void playersTurn(Grid<char>& board, Grid<bool>& used, Map<string, Grid<bool> >& map, Set<string>& words);
void computersTurn(Set<string>& words);
void highlight(Map<string, Grid<bool> >& map, string word);
void highlightSelectedWord(Grid<bool>& used);
void deHighlightBoard(Grid<bool>& used);

/* Main program */

int main() {
	GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
	initGBoggle(gw);
	welcome();
	giveInstructions();
	initGBoggle(gw);
	while (1){
		Grid<char> board;
		Grid<bool> used;
		Map<string, Grid<bool> > map;
		Set<string> words;
		setupBoard(board);
		generatePossibleWords(board, used, map, words);
		playersTurn(board, used, map, words);
		computersTurn(words);
		string askPlayer = getLine("Want to start a new game? yes / no ");
		if (askPlayer == "no") break;
	}
	return 0;
}


/*
* Function: welcome
* Usage: welcome();
* -----------------
* Print out a cheery welcome message.
*/

void welcome() {
	cout << "Welcome!  You're about to play an intense game ";
	cout << "of mind-numbing Boggle.  The good news is that ";
	cout << "you might improve your vocabulary a bit.  The ";
	cout << "bad news is that you're probably going to lose ";
	cout << "miserably to this little dictionary-toting hunk ";
	cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
* Function: giveInstructions
* Usage: giveInstructions();
* --------------------------
* Print out the instructions for the user.
*/

void giveInstructions() {
	cout << endl;
	cout << "The boggle board is a grid onto which I ";
	cout << "I will randomly distribute cubes. These ";
	cout << "6-sided cubes have letters rather than ";
	cout << "numbers on the faces, creating a grid of ";
	cout << "letters on which you try to form words. ";
	cout << "You go first, entering all the words you can ";
	cout << "find that are formed by tracing adjoining ";
	cout << "letters. Two letters adjoin if they are next ";
	cout << "to each other horizontally, vertically, or ";
	cout << "diagonally. A letter can only be used once ";
	cout << "in each word. Words must be at least four ";
	cout << "letters long and can be counted only once. ";
	cout << "You score points based on word length: a ";
	cout << "4-letter word is worth 1 point, 5-letters ";
	cout << "earn 2 points, and so on. After your puny ";
	cout << "brain is exhausted, I, the supercomputer, ";
	cout << "will find all the remaining words and double ";
	cout << "or triple your paltry score." << endl << endl;
	cout << "Hit return when you're ready...";
	getLine();
}


void setupBoard(Grid<char>& board){
	displayBoard(board);
}

// This method asks user if they want to play 5x5 Boggle
void displayBoard(Grid<char>& board){
	cout << endl;
	cout << "You can choose standard Boggle (4x4 grid)" << endl;
	cout << "or Big Boggle (5x5)." << endl << endl;
	string boggleSize = getLine("Would you like Big Boggle? yes / no: ");
	int numRows = 4;
	int numCols = 4;
	if (boggleSize == "yes"){
		numRows = 5;
		numCols = 5;
	}
	drawBoard(numRows, numCols);
	addLetters(numRows, board);
}

// This method fills board with random letters, also asks user to enter chosen letters
void addLetters(int numRows, Grid<char>& board){
	Vector<string> letters;
	cout << endl;
	cout << "I'll give you a chance to set up the board to your specification, which makes it easier to confirm your boggle program is working." << endl;
	string answer = getLine("Do you want to force the board configuration? yes / no: ");
	if (answer == "yes"){
		askPlayerForLetters(letters, numRows);
	} else {
		if (numRows == 4){
			for (int i = 0; i < 16; i++){
				letters.add(STANDARD_CUBES[i]);
			}
		} else if (numRows == 5){
			for (int i = 0; i < 25; i++){
				letters.add(BIG_BOGGLE_CUBES[i]);
			}
		}
	}
	board.resize(numRows, numRows);
	string toPrint = getRandomString(letters);
	fillBoard(board, toPrint, numRows);
}

// This method returns string with randomly distributed letters
string getRandomString(Vector<string>& letters){
	string result = "";
	int rand;
	for (int i = 0; i < letters.size(); i++){
		rand = randomInteger(i, letters.size() - 1);
		string copy = letters[i];
		letters[i] = letters[rand];
		letters[rand] = copy;
	}
	foreach (string s in letters){
		rand = randomInteger(0, 5);
		result += s[rand];
	}
	return result;
}

// This method actually displays characters on the board
void fillBoard(Grid<char>& board, string toPrint, int numRows){
	int counter = 0;
	for (int i = 0; i < numRows; i++){
		for (int j = 0; j < numRows; j++){
			board[i][j] = toPrint[counter];
			labelCube(i,j,toPrint[counter]);
			counter++;
		}
	}

}

// This method allows user to enter chosen letters, 6 letters on each turn total of 16 or 25 turns
void askPlayerForLetters(Vector<string>& letters, int numRows){
	cout << "Enter a ";  
	cout << numRows * numRows;
	cout << "-character string to identify which letters you want on the cubes." << endl;
	cout << "The first ";
	cout << numRows;
	cout << " letters are the cubes on the top row from left to right," << endl;
	cout << "the next ";
	cout << numRows;
	cout << " letters are the second row, and so on." << endl;
	string cubeLetters;
	for (int i = 0; i < numRows * numRows; i++){
		cubeLetters = getLine("Enter the string of size 6: ");
		while (cubeLetters.length() != 6){
			cout << "Enter correct size string " << endl;
			cubeLetters = getLine("Enter the string of size 6: ");
		}
		letters.add(cubeLetters);
	}
}

// This method returns vector with points to coordinate on the board when needed, coordinates around one point
Vector<Point> points(){
	Vector<Point> result;
	Point p1 (-1,-1);
	Point p2 (-1,0);
	Point p3 (-1,1);
	Point p4(0,-1);
	Point p5 (0,1);
	Point p6 (1,-1);
	Point p7 (1,0);
	Point p8 (1,1);
	result += p1,p2,p3,p4,p5,p6,p7,p8;
	return result;
}

// This method initializes all places on the board to be FALSE, this means that they are not used yet, true means used.
void setupUsedCubes(Grid<bool>& used, int numRows){
	used.resize(numRows, numRows);
	for (int i = 0; i < numRows; i++){
		for (int j = 0; j < numRows; j++){
			used[i][j] = false;
		}
	}
}

/* This method finds all possible words and stores them in Set of strings "words", 
	also stores grids of used cubes, so that we can highlight them after,
	the key is a word and the value is a grid of booleans. Base case is if word satisfies our 
	constraints, and then recursively find other words.
*/
void findAllWords(Map<string, Grid<bool> >& map, Grid<char>& board, Grid<bool> used, Lexicon& lex, Set<string>& words, string soFar, int x, int y){
	used[x][y] = true;
	if (soFar.length() > 3 && lex.contains(soFar)){
		words.add(soFar);
		map[soFar] = used;
	}
	string copy;
	Vector<Point> vect = points();
	foreach (Point p in vect){
		if (board.inBounds(x + p.getX(), y + p.getY()) && !used[x + p.getX()][y + p.getY()]){
			copy = soFar;
			copy += board[x + p.getX()][y + p.getY()];
			if (lex.containsPrefix(copy)){
				findAllWords(map, board, used, lex, words, copy, x + p.getX(), y + p.getY());
			}
		}
	}
}

// This method generates words for all points on the board, with method findAllWords, discribed above.
void generatePossibleWords(Grid<char>& board, Grid<bool>& used, Map<string, Grid<bool> >& map, Set<string>& words){
	Lexicon lex("EnglishWords.dat");
	setupUsedCubes(used, board.nRows);
	string soFar = "";
	for (int i = 0; i < board.nRows; i++){
		for (int j = 0; j < board.nRows; j++){
			soFar = board[i][j];
			findAllWords(map, board, used, lex, words, soFar, i, j);
		}
	}
}

// This method allows player to enter all words they can find on the board and score as many points as possible
void playersTurn(Grid<char>& board, Grid<bool>& used, Map<string, Grid<bool> >& map,  Set<string>& words){
	Player p = HUMAN;
	Set<string> usedWords;
	string word;
	while (1){
		word = getLine("Enter your word: ");
		if (word == "") break;
		word = toUpperCase(word);
		if (words.contains(word) && !usedWords.contains(word)){
			usedWords.add(word);
			recordWordForPlayer(word, p);
			highlight(map, word);
		}
	}
	words -= usedWords;
}

// This method displays all words left from player's try and computer gets the points
void computersTurn(Set<string>& words){
	Player p = COMPUTER;
	foreach (string word in words){
		recordWordForPlayer(word, p);
	}
}

// This method highlights cubes which are in use of the given word
void highlight(Map<string, Grid<bool> >& map, string word){
	Grid<bool> used = map[word];
	highlightSelectedWord(used);
	pause(800);
	deHighlightBoard(used);
}

// This method highlights cubes based on Grid of booleans used by the word
void highlightSelectedWord(Grid<bool>& used){
	for (int i = 0; i < used.nRows; i++){
		for (int j = 0; j < used.nRows; j++){
			highlightCube(i, j, used[i][j]);
		}
	}
}

//  This method returns the board to its starting position without any highlighting
void deHighlightBoard(Grid<bool>& used){
	for (int i = 0; i < used.nRows; i++){
		for (int j = 0; j < used.nRows; j++){
			highlightCube(i, j, 0);
		}
	}
}
