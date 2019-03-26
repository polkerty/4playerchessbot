#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

struct Board;

struct Move {
    int row, col;
    Move ( int r, int c);
    Move () {
        row = -1;
        col = -1;
    }
};

struct MovePair {
    Move from, to;
    MovePair( Move f, Move t ) {
        from = f;
        to = t;
    }
    MovePair( ) {
        from = Move();
        to = Move();
    }
};

struct Piece {
    int owns, type, alive, points, value;
    string text;

    int moved;

    int delta_row, delta_col;

    Piece( int owns ) {
        owns = owns;
        moved = false;
        
    }

    void off() {
        alive = false;
    }

    string toString() {
        if ( owns == -1 ) return text;
        else return text + to_string(owns);
    }

    Piece () {
        owns = -1;
    }

    vector<Move> moves( int row, int col , Board board, int orientation=0 , bool only_attacks=false  );

    Piece copy();
};



struct Square {
    int row, col;
    Piece contents; 

    vector<Move> targets, attacks, defenses;
    Square( int r, int c, Piece conts ) {
        row = r;
        col = c;
        contents = conts; 
    }

	Square copy () {
		Square s =  Square( row, col, contents.copy() );
		return s;
	}
	
    
};

struct Options {
    bool permitSelfAttacks;
    Options() {
        permitSelfAttacks = false;
    }

};

struct Board {
    vector<vector<Square>> conts;

    Options options;

    

    int rows, cols;
    Square at( int x, int y ) {
        return conts[x][y];
    }
    Square at (Move m ) {
        return conts[m.row][m.col];
    }
    Board( );
    Board ( vector<vector<Square>> c ) {
        conts = c;
        rows = conts.size();
        cols = conts.cols();
    }
    void allowSelfAttacks() {
        options.permitSelfAttacks = true;
    }
    void disallowSelfAttacks() {
        options.permitSelfAttacks = false;
    }
};


vector<string> split ( string str, char tok ) {
    vector<string> ret;
    string buff;
    for ( int i = 0; i < str.size(); ++i ) {
        if ( str[i] == tok ) {
            ret.push_back(buff);
            buff = "";
        } else {
            buff += str[i];
        }
    }
    ret.push_back(buff);
    return ret;
}

struct MoveRecommendation {
    MovePair move;
    vector<int> scores;
    MoveRecommendation( MovePair m, vector<int> s ) {
        move = m;
        scores = s;
    }
};

