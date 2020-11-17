#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <random>

using namespace std;

struct Board;

struct Piece;

auto seed = random_device()();

auto rng = std::default_random_engine { seed };

struct MoveEvaluation {
	vector<double> scores;
	bool legal;
	MoveEvaluation( vector<double> s, bool l ) {
		scores = s;
		legal = l;
	}
};

struct Move {
    int row, col;
    Move ( int r, int c) {
    	row = r;
    	 col = c;
    };
    Move () {
        row = -1;
        col = -1;
    }
    bool is_valid() {
    	return row != -1 && col != -1;
    }
    const bool operator==( const Move& m ) const {
    	return row == m.row && col == m.col;
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
    const bool operator==( const MovePair& m ) const {
    	return from == m.from && to == m.to;
    }
};

 
vector<Move> probe ( int row, int col, int delta_row, int delta_col, 
    Board& board, bool is_single_hop = false, bool no_swoggle = false, bool require_enemy = false, 
    bool non_attacking = false );
    
vector<int> compute_pawn_orientation ( int owns, int orientation );

vector<int> compute_pawn_left_attack ( int owns, int orientation );

vector<int> compute_pawn_right_attack( int owns, int orientation );

bool isOpen( Board& board, int row, int col, vector<int> dir );


struct Piece {
    int owns, type, alive, points, value;
    string text;

    int moved = 0;

    int delta_row, delta_col;

    Piece( int _owns ) {
        owns = _owns;
        moved = false;
        
    }
    
    void off() {
        alive = false;
    }

    string toString() {
        if ( owns == -1 ) return text;
        else return text + ":" +  to_string(owns) + ":" + (moved ? "1":"0");
    }

    Piece (  ) {
        owns = -1;
    }



	// we're not going to use pointers, and that means just one thing: Switch statements.
    virtual vector<Move> moves( int row, int col , Board& board, int orientation=0 , bool only_attacks=false  ){ 
    	
    	vector<Move> ret, ret2, ret3;
 		vector<int> dir , left_attack, right_attack ;
 			
    	switch(type) {
    		case 0:
 				return {};
   		
    		break;
    		case 1:
				return {};
    		
    		break;
    		case 2:
				ret = probe( row, col, 1, 0, board, false);
				ret2 = probe( row, col, 0, 1, board, false);
				ret.insert(ret.end(), ret2.begin(), ret2.end());
				return ret;
    		
    		break;
    		case 3:
				ret = probe( row, col, 1, 2, board, true );
				ret2 = probe(row, col, 2, 1, board, true ) ;
				ret.insert(ret.end(), ret2.begin(), ret2.end());
				return ret;
    		
    		break;
    		case 4:
 				return probe( row, col, 1, 1, board, false );    		
    		break;
    		case 5:
				ret = probe( row, col, 1, 1,  board, true );
				ret2 = probe( row, col, 1, 0, board, true );
				ret3 = probe( row, col, 0, 1, board, true ); 
		 
				ret.insert( ret.end(), ret2.begin(), ret2.end());
				ret.insert(ret.end(), ret3.begin(), ret3.end());
				return ret;
    		
    		break;
    		case 6:
				ret = probe ( row, col, 1, 1, board, false );
				ret2 = probe( row, col, 1, 0, board, false );
				ret3 = probe( row, col, 0, 1, board, false );
				ret.insert(ret.end(), ret2.begin(), ret2.end());
				ret.insert(ret.end(), ret3.begin(), ret3.end());
				return ret;
    		
    		break;
    		case 7:
				dir = compute_pawn_orientation ( owns, orientation );
				
				if ( !only_attacks ) {
					ret2 = probe( row, col, dir[0], dir[1], board, true, true, false, true ); 
					ret.insert ( ret.end(), ret2.begin(), ret2.end());
		
					if ( !moved && isOpen( board, row, col, dir ) ) {
						// must take special care not to run into other pieces on the way...
						ret2 = probe ( row, col, dir[0] * 2, dir[1] * 2, board,  true, true, false, true );
						ret.insert ( ret.end(), ret2.begin(), ret2.end());
					}
				}
		
				left_attack = compute_pawn_left_attack( owns, orientation );
				ret2 = probe( row, col, left_attack[0], left_attack[1], board, true, true, true );
				ret.insert ( ret.end(), ret2.begin(), ret2.end());

				right_attack = compute_pawn_right_attack( owns, orientation );
				ret2 = probe( row, col, right_attack[0], right_attack[1], board, true, true, true ); 
				ret.insert ( ret.end(), ret2.begin(), ret2.end());
		
				return ret;
    		
 				   		
    		break;
    		case 8:
				ret = probe ( row, col, 1, 1, board, false );
				ret2 = probe( row, col, 1, 0, board, false );
				ret3 = probe( row, col, 0, 1, board, false );
				ret.insert(ret.end(), ret2.begin(), ret2.end());
				ret.insert(ret.end(), ret3.begin(), ret3.end());
				return ret;
    		
    		break;
    		case 9:
    		break;
    		default:
    		break;
    	}
    
    	return vector<Move>(); 
    
    }

    Piece copy() { 
    	return *this;
    }
};



struct Square {
    int row, col;
    Piece contents; 
    
    int rows() {
    	return row;
    }
    
    int cols() {
    	return col;
    }

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

    

    int rows=0, cols=0;
    Square& at( int x, int y ) {
        return conts[x][y];
    }
    Square& at (Move m ) {
        return conts[m.row][m.col];
    }
    Board( ) {};
    Board ( vector<vector<Square>> c ) {
        conts = c;
        rows = conts.size();
        cols = conts[0].size();
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
    vector<double> scores;
    MoveRecommendation( MovePair m, vector<double> s ) {
        move = m;
        scores = s;
    }
    const bool operator==( const MoveRecommendation& mr ) const {
    	return move == mr.move;
    	// don't care about the scores
    }
};

const MoveRecommendation INVALID ( MovePair(Move(-1, -1), Move(-1, -1)), {-1e9, -1e9, -1e9, -1e9 } );

struct PlayerOptions {
	double banked_point_multiplier,
		lost_point_multiplier,
		my_piece_multiplier,
		threat_point_multiplier,
		potential_point_multiplier,
		check_points_right,
		check_points;
		
	PlayerOptions( double bpp = 0.9, double lpp = 1, double mpm = 1, double tpm = 0.3, double cpr = 0.3, double cp = 1 ) {
		banked_point_multiplier = bpp;
		lost_point_multiplier = lpp;
		my_piece_multiplier = mpm;
		threat_point_multiplier = tpm;
		potential_point_multiplier = mpm;
		check_points_right = cpr;
		check_points = cp;
	}
};
