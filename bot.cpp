#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

#include "bot.h";

using namespace std;

struct Move {
    int row, col;
    Move( int r, int c ) {
        row = r;
        col = c;
    }
};


bool inBounds( Board board, int row, int col ) {
	return row > -1 && col > -1 && row < board.rows && col < board.cols;
}

bool isOpen( Board board, int row, int col, vector<int> dir ) {
	int x = row + dir[0], y = col + dir[1];
	if ( !inBounds( board, x, y ) ) return false;
	if ( ! board.at(x, y).contents.type == 0 ) return false;
	
	
	return true;
}	
bool isQueeningMove( Piece piece, Move from, Move to ) {
	if ( !piece.type == 7 ) return false;
	return (piece.owns == 1 && from.row == 6&& to.row == 7) || 
	(piece.owns == 3 && from.row == 7 && to.row == 6) ||
	(piece.owns == 0 && from.col == 6 && to.col == 7 ) ||
	(piece.owns == 2 && from.col == 7 && to.col == 6);
}



vector<int> compute_pawn_orientation ( int owns, int orientation ) {
	if ( orientation == 0 ) {
		if ( owns == 0 ) {
			return { -1, 0 };
		} else if ( owns == 1 ) {
			return { 0, 1 };
		} else if ( owns == 2 ) {
			return { 1, 0 };
		} else if ( owns == 3 ) {
			return { 0, -1 };
		}
	} else if ( orientation == 1 ) {
		if ( owns == 1 ) {
			return { -1, 0 };
		} else if ( owns == 2 ) {
			return {0, 1};
		} else if ( owns == 3 ) {
			return {1, 0};
		} else if ( owns == 0 ) {
			return { 0, -1 };
		}
	} else if ( orientation == 2 ) {
		if ( owns == 2) {
			return { -1, 0 };
		} else if ( owns == 3 ) {
			return {0, 1};
		} else if ( owns == 0 ) {
			return {1, 0};
		} else if ( owns == 1 ) {
			return { 0, -1 };
		}
	} else if ( orientation == 3 ) {
		if ( owns == 3 ) {
			return { -1, 0 };
		} else if ( owns == 0 ) {
			return {0, 1};
		} else if ( owns == 1 ) {
			return {1, 0};
		} else if ( owns == 2 ) {
			return { 0, -1 };
		}
	} 

}

vector<int> compute_pawn_left_attack ( int owns, int orientation ) {
	if ( orientation == 0 ) {
		if ( owns == 0 ) {
			return {-1, -1};
		} else if ( owns == 1 ) {
			return {-1, 1};
		} else if ( owns == 2 ) {
			return {1, 1};
		} else if ( owns == 3 ) {
			return {1, -1};
		}
	} else if ( orientation == 1 ) {
		if ( owns == 1 ) {
			return {-1, -1};
		} else if ( owns == 2 ) {
			return {-1, 1};
		} else if ( owns == 3 ) {
			return {1, 1};
		} else if ( owns == 0 ) {
			return {[1, -1};
		}
	} else if ( orientation == 2 ) {
		if ( owns == 2 ) {
			return {-1, -1};
		} else if ( owns == 3 ) {
			return {-1, 1};
		} else if ( owns == 0 ) {
			return {1, 1};
		} else if ( owns == 1 ) {
			return {1, -1};
		}
	} else if ( orientation == 3 ) {
		if ( owns == 3 ) {
			return {-1, -1};
		} else if ( owns == 0 ) {
			return {-1, 1};
		} else if ( owns == 1 ) {
			return {1, 1};
		} else if ( owns == 2 ) {
			return {1, -1};
		}
	}  
	
	
}


vector<int> compute_pawn_right_attack( int owns, int orientation ) {
	if ( orientation == 0 ) {
		if ( owns == 0 ) {
			return { -1, 1 };
		} else if ( owns == 1 ) {
			return { 1, 1 };
		} else if ( owns == 2 ) {
			return { 1, -1 };
		} else if ( owns == 3 ) {
			return { -1, -1 };
		}
	} else if ( orientation == 1 ) {
		if ( owns == 1 ) {
			return { -1, 1 };
		} else if ( owns == 2 ) {
			return { 1, 1 };
		} else if ( owns == 3 ) {
			return { 1, -1 };
		} else if ( owns == 0 ) {
			return { -1, -1 };
		}
	} else if ( orientation == 2 ) {
		if ( owns == 2 ) {
			return { -1, 1 };
		} else if ( owns == 3 ) {
			return { 1, 1 };
		} else if ( owns == 0 ) {
			return { 1, -1 };
		} else if ( owns == 1 ) {
			return { -1, -1 };
		}
	} else if ( orientation == 3 ) {
		if ( owns == 3 ) {
			return { -1, 1 };
		} else if ( owns == 0 ) {
			return { 1, 1 };
		} else if ( owns == 1 ) {
			return { 1, -1 };
		} else if ( owns == 2 ) {
			return { -1, -1 };
		}
	} 
}



vector<Move> line_probe ( int row, int col, int delta_row, int delta_col, 
    Board board, bool is_single_hop, bool require_enemy, bool non_attacking ) {
    
    vector<Move> ret;
    
    int loc = 1;
    
    int orig_row = row;
    int orig_col = col;

    row += delta_row;
    col += delta_col;
    
    
    while ( row < board.rows && row > -1 && col > -1 && col < board.cols ) {
        
        bool break_next = false;
        
        
        // If it's blocked (an edge chunk), stop
        if ( board.at(row, col).contents.type == 1 ) {
            break;
        }
        
        if ( board.at(row, col).contents.type > 1 ) {
            if ( (board.at(row, col).contents.owns == board.at(orig_row,orig_col).contents.owns && 
                !board.options.permitSelfAttacks) || 
                non_attacking
            ) {
                break;
            } else {
                break_next = true;
            }
        }
        
        if ( board.at(row, col).contents.type <= 1 && require_enemy ) {
            break;
        }
        
        ret.push_back(  Move ( row, col ) );
        
        if ( break_next || is_single_hop ) {
            break;
        }
        
        row += delta_row;
        col += delta_col;
    }
    
    return ret;
    
}
	

 
vector<Move> probe ( int row, int col, int delta_row, int delta_col, 
    Board board, bool is_single_hop = false, bool no_swoggle = false, bool require_enemy = false, 
    bool non_attacking = false ) {
	// depth = -1 => forever
	// depth = 0 => until I run into another object.
	
	vector<Move> ret;

	
	// consider the different directions the row/col can go. 
	if ( !no_swoggle ) {

	
		for ( int i = -delta_row; i < delta_row + 1; i += 2 * delta_row ) {
			for ( int j = -delta_col; j < delta_col + 1; j += 2 * delta_col ) {
                
                vector<Move> probe_results = line_probe( row, col, i, j, board, is_single_hop, require_enemy,
                                                            non_attacking );
                ret.insert(ret.end(), probe_results.begin(), probe_results.end());
			
				if ( delta_col == 0 ) break;
			}
			if ( delta_row == 0 ) break;
		}
	
	} else {
		ret = line_probe ( row, col, delta_row, delta_col , board, is_single_hop, require_enemy, non_attacking ) ;
	}
	
	
	return ret;
	
}




struct Blank : Piece {
	int owns, type, alive, points, value;
    string text;

    Blank( int owns) : Piece(-1) {
        type = 0;
        text = " ";
        alive = false;
        points = 0;
        value = 0;
    }

	vector<Move> moves( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		return {};
	}
	
	Piece copy (){
		return Blank();
	}
};



struct Block : Piece {

    Block (int owns ) : Piece(owns) {
        type = 1;
        text = "X";
        alive = false;
        points = 0;
        value = 0;
    }


	
	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		return {};
	}
	Block copy () {
		return Block(-1);
	}
};

struct Rook : Piece {

    Rook ( int owns ) : Piece(owns ) {
        type = 2;
        text = "R";
        alive = true;
        points = 5;
        value = 5;
    }
	
	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		vector<Move> ret = probe( row, col, 1, 0, board, false);
        vector<Move> ret2 = probe( row, col, 0, 1, board, false);
        ret.insert(ret.end(), ret2.begin(), ret2.end());
		return ret;
	}
	Piece copy() {
		Piece rook = Rook( owns );
		rook.alive = alive;
		return rook;
	}

};

struct Knight : Piece {

    Knight ( int owns ) : Piece (owns ) {
        type =3;
        text = "N";
        alive = true;
        owns = owns;
        points = 3;
        value = 3;
    }
	
	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		vector<Move> ret = probe( row, col, 1, 2, board, true );
        vector<Move> ret2 = probe(row, col, 2, 1, board, true ) 
		ret.insert(ret.end(), ret2.begin(), ret2.end());
        return ret;
	}
	Piece copy() {
		Piece knight =  Knight(owns);
		knight.alive = alive;
		return knight;
	}
};

struct Bishop : Piece {
	

    Bishop( int owns ) : Piece(owns) {
        type = 4;
        text = "B";
        alive = true;
        points = 5;
        value = 5;

    }
	
	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		return probe( row, col, 1, 1, board, false ); 
	}

	Piece copy() {
		Piece bishop =  Bishop(owns);
		bishop.alive = alive;
		return bishop;
	}
};

struct King : Piece {


    King ( int owns ) : Piece ( owns ) {
        type = 5;
        text = "K";
        alive = true;
        points = 20;
        value = 20;
    }

	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		vector<Move> ret = probe( row, col, 1, 1,  board, true );
        vector<Move> ret2 = probe( row, col, 1, 0, board, true );
        vector<Move> ret3 = probe( row, col, 0, 1, board, true ); 
		 
        ret.insert( ret.end(), ret2.begin(), ret2.end());
        ret.insert(ret.end(), ret3.begin(), ret3.end());
		return ret;
	}

	Piece copy() {
		Piece king =  King(owns);
		king.alive = alive;
		return king;
	}
	
};

struct Queen : Piece {

    Queen ( int owns ) : Piece(owns ) {
        type = 6;
        text = "Q";
        alive = true;
        points = 9;
        value = 9;
    }


	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		vector<Move> ret = probe ( row, col, 1, 1, board, false );
        vector<Move> ret2 = probe( row, col, 1, 0, board, false );
        vector<Move> ret3 = probe( row, col, 0, 1, board, false );
		ret.insert(ret.end(), ret2.begin(), ret2.end());
        ret.insert(ret.end(), ret3.begin(), ret3.end());
		return ret;
	}
	Piece copy() {
		Piece queen =  Queen(owns);
		queen.alive = alive;
		return queen;
	}
};

struct Queen1Pt : Piece {


    Queen1Pt ( int owns ) : Piece ( owns ) {
        type = 7;
        text = "D";
        alive = true;
        points = 1;
        value = 7;
    }

    vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		vector<Move> ret = probe ( row, col, 1, 1, board, false );
        vector<Move> ret2 = probe( row, col, 1, 0, board, false );
        vector<Move> ret3 = probe( row, col, 0, 1, board, false );
		ret.insert(ret.end(), ret2.begin(), ret2.end());
        ret.insert(ret.end(), ret3.begin(), ret3.end());
		return ret;
	}
	Piece copy() {
		Piece queen1 =  Queen1Pt(owns);
		queen1.alive = alive;
		return queen1;
	}


};

struct Pawn : Piece {

	Pawn ( int owns ) : Piece ( owns ) {
        type = 7;
        text = "P";
        alive = true;
        points = 1;
        value = 1;
    }
	
	vector<Move> moves ( int row, int col, Board board, int orientation = 0 , bool only_attacks = false  ) {
		vector<int> dir = compute_pawn_orientation ( owns, orientation );
		vector<Move> ret;
				
		if ( !only_attacks ) {
            vector<Move> ret2 = probe( row, col, dir[0], dir[1], board, true, true, false, true ); 
			ret.insert ( ret.end(), ret2.begin(), ret2.end());
		
			if ( !moved && isOpen( board, row, col, dir ) ) {
				// must take special care not to run into other pieces on the way...
                ret2 = probe ( row, col, dir[0] * 2, dir[1] * 2, board,  true, true, false, true )
			    ret.insert ( ret.end(), ret2.begin(), ret2.end());
			}
		}
		
		vector<int> left_attack = compute_pawn_left_attack( owns, orientation );
        vector<Move> ret2 = probe( row, col, left_attack[0], left_attack[1], board, true, true, true );
		ret.insert ( ret.end(), ret2.begin(), ret2.end());

		vector<int> right_attack = compute_pawn_right_attack( owns, orientation );
        ret2 = probe( row, col, right_attack[0], right_attack[1], board, true, true, true ); 
		ret.insert ( ret.end(), ret2.begin(), ret2.end());
		
		return ret;
	}
	Piece copy() {
		Piece pawn = Pawn(owns);
		pawn.alive = alive;
		pawn.moved = moved;
		return pawn;
	}
	
	
	
};




string start_map = 
R"(c	c	c	r:1	n:1	b:1	q:1	k:1	b:1	n:1	r:1	c	c	c
c	c	c	p:1	p:1	p:1	p:1	p:1	p:1	p:1	p:1	c	c	c
c	c	c	e	e	e	e	e	e	e	e	c	c	c
r:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	r:2
n:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	n:2
b:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	b:2
q:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	k:2
k:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	q:2
b:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	b:2
n:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	n:2
r:0	p:0	e	e	e	e	e	e	e	e	e	e	p:2	r:2
c	c	c	e	e	e	e	e	e	e	e	c	c	c
c	c	c	p:3	p:3	p:3	p:3	p:3	p:3	p:3	p:3	c	c	c
c	c	c	r:3	n:3	b:3	k:3	q:3	b:3	n:3	r:3	c	c	c)";



Board boardFromMap( string raw ) {
	vector<string> map = split(raw, '\n');

    vector<vector<Square>> ret;

    
	for ( int i = 0; i < map.size(); ++i ) {
        vector<string> row = split(map[i], '\t');
        vector<Square> out_row;

		for ( int j = 0; j < row.size(); ++j ) {
            int player = -1;
			vector<string> t = split(row[j], ':');
			if ( t.size() == 0 || t[0].size() == 0 ) {
				continue;
			}
			if ( t.size() > 1 ) {
				player = stoi(t[1]);
			}
			switch( t[0][0] ) {
				case 'c':
					out_row.push_back( Square ( i, j, Block(player)));
					break;
				case 'e':
					out_row.push_back( Square ( i, j, Blank(player)));
					break;
				case 'r':
					out_row.push_back( Square ( i, j, Rook(player)));
					break;
				case 'n':
					out_row.push_back( Square ( i, j, Knight(player)));
					break;
				case 'b':
					out_row.push_back( Square ( i, j, Bishop(player)));
					break;
				case 'k':
					out_row.push_back( Square ( i, j, King(player)));
					break;
				case 'q':
					out_row.push_back( Square ( i, j, Queen(player)));
					break;
				case 'd':
					out_row.push_back( Square ( i, j, Queen1Pt(player)));
					break;
				case 'p':
					out_row.push_back( Square ( i, j, Pawn(player)));
					break;
				default:
					break;
			}
		}
        ret.push_back(out_row);
	}
	
	return Board(ret);
}

Board copyBoard(Board b) {
	Board ret ( b.conts);
	for ( int i = 0; i < b.rows; ++i ) {
		for ( int j = 0 ; j < b.cols; ++j ) {
			ret.at(i, j) = b.at(i ,j).copy();
		}
	}
	return ret;
}


struct Game  {

	Board board;
	int me;
	int next_move;
	vector<int> scores, alive;
	int orientation;

	Game ( int _me, string map = start_map, int nm = 0 ) {
		board = boardFromMap(map);

		scores = {0, 0, 0 , 0};
		alive = {1, 1, 1, 1};

		me = _me;
		next_move = nm;
		orientation = 3;
	}

	bool isMyTurn () {
		return me == next_move;
	}

	void advance() {
		int cand = next_move;
		for ( int i = 0; i < alive.size(); ++i ) {
			cand = ( cand + 1) % ( alive.size());
			if ( alive[cand] ) {
				next_move = cand;
				break;
			}
		}
	}




	string toString () {

		string map;

		for ( int i = 0 ; i < board.rows; ++i ){
			for ( int j = 0; j < board.cols; ++j ) {
				map += board.at(i, j).contents.toString();
				map += "\t";
			}
			map += "\n";
		}
		return map;

	}


	MoveRecommendation alpha_beta ( int plies_remaining, vector<int> scan_hierarchy, int player= me) {

		
		int horizon = scan_hierarchy[player];

		vector<MoveRecommendation> possible_moves = ordered_moves ( player );

		
		vector<MoveRecommendation> move_list ( possible_moves.begin(), possible_moves.begin() + max(possible_moves.size(), (size_t)horizon));

		if ( plies_remaining == 0 ) {
			return move_list[0];
		}
		vector<int> scores = {0, 0, 0, 0};
		int move_index = -1;
		for  ( int i = 0; i < move_list.size(); ++i ) {
			Game child = spawn_move(move_list[i].move.from, move_list[i].move.to, player );
			
			MoveRecommendation ply_result = child.alpha_beta( plies_remaining - 1, scan_hierarchy, child.next_move );
			// now let's score this for ourselves...

			if ( ply_result.scores[player] > scores[player] || i == 0 ) {
				scores = ply_result.scores;
				move_index = i;
			}
		}

		return move_list[move_index];


	}

	MovePair ab_recommended_move ( int player){
		return alpha_beta(5, {3, 2, 1, 1}, player).move; 
	}

	
	string indexToCode ( int row, int col ) {
		if ( orientation == 3 ) {
			string rows = "abcdefghijklmn";
			
			return  to_string(rows[row]) + "" +  to_string((col + 1));
		}
		
		 else {
		 	// let's ignore this for now
			 return "";
		 }
	}
	// int codeToIndex  ( string code ) {
	// 	string rows = "abcdefghijklmn";
	// 	int index = rows.size() - 1;
	// 	while ( index >= 0 && rows[index] != code[0] )--index;
	// 	int row = index;
	// 	int col = (+code.slice(1))   - 1;
		
	// 	return new Move(row, col);
	// }
	
	//green on bot; this aligns with the map
	 
	
	// this modifies the board to see how many times a square is attacked,
	// and what pieces it attacks.

	bool annotated = false;


	void annotate_board () {
		if ( annotated ) return;
		board.options = board.options;
		board.allowSelfAttacks();
		

		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0; j < board.cols; ++j ) {
				board.at(i, j).attacks = {};
				board.at(i, j).targets = {};
				board.at(i, j).defenses = {};
			}
		}

		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0 ; j < board.cols; ++j ) {
				if ( board.at(i, j).contents.owns > -1 && board.at(i, j).contents.alive ) {
					vector<Move> possible_moves = board.at(i, j).contents.moves( i, j, board, orientation, true );
					//console.log(i, j, _.board[i][j].contents, possible_moves);
					board.at(i, j).targets = {};
					
					for ( int k = 0; k < possible_moves.size(); ++k ) {
						Move move = possible_moves[k];
						
						if ( board.at(move.row,move.col).contents.owns == board.at(i,j).contents.owns ) {
							board.at(move.row,move.col).defenses.push_back( Move(i, j));
						} else { 
							board.at(move.row,move.col).attacks.push_back(( Move(i, j));
						}
						board.at(i, j).targets.push_back(move);
					}
					
					
				}
			}
		}
		
		board.disallowSelfAttacks();
		annotated = true;
	}	
		
	void apply_move ( Move from, Move to, bool upgrade ) {
		
		//debugger;
		Piece piece = board.at(from.row, from.col).contents;
		
		if ( isQueeningMove(piece, from, to )) {
			upgrade = true;
		}

		if ( upgrade ) {
			piece =  Queen1Pt(piece.owns);
		}
		
		board.at(from.row, from.col).contents = Blank(-1);
		
		piece.moved = true; // pawns need to know
		
		
		if ( board.at(to.row,to.col).contents.owns > -1 &&
			board.at(to.row,to.col).contents.alive ) {
			scores[piece.owns] += 
				board.at(to.row,to.col).contents.points;
		}
		
		board.at(to.row,to.col).contents = piece;

		annotated = false;
		
		//this.cur_move = ( this.cur_move + 1)%4;

	}

	
	// this is stupidly complicated to compute :/
	this.castle = function(kfrom, kto ) {
		//castling always moves king 2
		var xdir = (kto.row - kfrom.row)/2
		var ydir = (kto.col - kfrom.col)/2

		var row, col;
		for ( var i = 3; i < 5; ++i ) {
			row = kfrom.row + xdir * i;
			col = kfrom.col + ydir * i;
			if ( _.board[row][col].contents.type == 2 ) {
				break;
			} 

		}
		var new_row = kto.row - xdir;
		var new_col = kto.col - ydir;
		_.apply_move(kfrom, kto);
		_.apply_move(new Move(row, col), new Move(new_row, new_col));
	}
	
	this.annotated = false;
	
	this.kill_group= function( group ) {
		for ( var i = 0; i < group.length; ++i ) {
			_.kill(group[i]);
		}
	}

	this.evaluate_all = function() {
		var results = [0, 0, 0, 0];
		for ( var i = 0; i < results.length; ++i ) {
			results[i]  = _.evaluate( i );
		}
		return results;
	}
	this.value = function(location){
		return _.board[location.row][location.col].contents.value;
	}
	this.attack_val = function( point_value, attackers, defenders ) {

			if ( !attackers.length ) return 0;
			if ( !defenders.length ) return point_value;
			var points = point_value;
			var cost = 0;


			var smallest = function(a, b) {
				return _.board[a.row][a.col].contents.value <
				_.board[b.row][b.col].contents.value ? -1: 1;
			}
			attackers.sort(smallest);
			defenders.sort(smallest);
			var ind = 0;
			if (defenders.length >= attackers.length ) {
				cost += _.board[attackers[0].row][attackers[0].col].contents.value;
				ind ++;

			}
			while ( ind < Math.min(attackers.length, defenders.length) && 
					_.board[defenders[ind].row][defenders[ind].col].contents.value <=
					_.board[attackers[ind].row][attackers[ind].col].contents.value ){
				points += _.board[defenders[ind].row][defenders[ind].col].contents.value;
				cost += _.board[attackers[ind].row][attackers[ind].col].contents.value;
				++ind;
			}
				

			
			return points - cost*1.1;

		}

	this.evaluate = function( player, verbose ) {
		
		if ( !_.alive[player] ) return -1000000;	
		if ( !_.annotated ) _.annotate_board();

		
		var banked_point_multiplier = 0.9;
		var lost_point_muliplier = 1; // lower values are more aggressive
		var my_piece_multiplier = 1;
		var threat_point_multiplier = 0.3; // a little value for making threats.
		var potential_point_multiplier = 0.3;
		var check_points_right = 2; // we like this
		var check_points = 1;

		// we want a high score.
		// how many points at risk
		// points - how many points at risk. 
		
		var score = _.scores[player] * banked_point_multiplier;
		// any checks are impossible; subtract infinity to avoid them.
		
		if ( !_.is_legal(player ) ) {
			score -= 1000000;
		}
		
		for ( var i = 0; i < _.board.length; ++i ) {
			for ( var j = 0; j < _.board[i].length; ++j ) {
				if ( _.board[i][j].contents.owns == player ) {
				// find the smallest value attacker/defender

					if ( isNaN(score) ) {alert("woops" ); debugger;}

					
					var aval = _.attack_val(_.board[i][j].contents.value,
						 _.board[i][j].attacks,
						 _.board[i][j].defenses);
					
					if (verbose && aval > 0 ) {
						console.log("At " + i + ", " + j + " - the total threats against us work out to " + aval);
					
					}
					if ( aval <= 0  ) {
						score += _.board[i][j].contents.value * my_piece_multiplier;
					} else {
						score -= _.board[i][j].contents.value * lost_point_muliplier;
					}
				} else {
					// let's see if we threaten them...

					var threatsFromPlayer = [];
					var defenses = _.board[i][j].defenses.slice();
					for ( var k = 0; k < _.board[i][j].attacks.length; ++k ) {
						var from = _.board[i][j].attacks[k];
						if (_.board[from.row][from.col].contents.owns != player ) {
							//defenses.push(from);
						} else {
							threatsFromPlayer.push(from);
						}
					}


					var aval = _.attack_val(_.board[i][j].contents.value, threatsFromPlayer,
					defenses);


					if ( aval > 0 ) {
						score += potential_point_multiplier*aval;
						if ( verbose ) {
							console.log ("At "  + i + "," + j + " our threat points are: " + potential_point_multiplier*aval);
						}
					}
					if ( _.board[i][j].contents.type == 5 && threatsFromPlayer.length) {
						if ( _.board[i][j].contents.owns + 1 == player )
						score += check_points_right;
						if ( verbose ) {
							console.log("Points for checking: " + check_points_right)
						}
					} else {
						score += check_points;
						if ( verbose ) {
							console.log("Points for checking: " + check_points)
						}
					}

				}
			}
		}

		
		
		// first see how many points are at risk. It's at risk if a
		// smaller element is threatening it. 

		// now, let's look
		
		return score
		
	}

	this.process = function () {

		if ( !_.annotated ) _.annotate_board();

		for ( var i = 0; i < 4; ++i ) {
			var mv = _.getAllMoves(i);
			if (mv.length == 0 ) {
				if ( _.alive[i] ) {
					_.kill(i);
				}
			}
		}
	}

	this.applyAll = function( fn ) {
		for ( var i = 0; i < _.board.length; ++i ) {
			for ( var j = 0; j < _.board[i].length; ++j ) {
				fn(_.board[i][j], i, j, _.board);
			}
		}
	}

	this.kill = function(p) {
		_.alive[p] = false;
		_.applyAll( function( piece, i, j, board) {
			piece.contents.alive = false;
		})
	}

	
	this.getAllMoves = function( player ) {
		
		//debugger;
		var moves = [];
		
		player == player === undefined ? _.me : player;
		for ( var i = 0; i < _.board.length; ++i ) {
			for ( var j = 0 ; j < _.board[i].length; ++j ) {
				if ( _.board[i][j].contents.owns == player && _.board[i][j].contents.alive ) {
					var possible_moves = _.board[i][j].contents.moves( i, j, _.board, _.orientation );
					//console.log(i, j, _.board[i][j].contents, possible_moves);
					for ( var k = 0; k < possible_moves.length; ++k ) {
						var result = _.try_move( new Move( i, j), possible_moves[k], player );
						if ( result.legal ) {
							moves.push( {"from": new Move(i, j), "to": possible_moves[k] , "score": result.score });
						}
					}
				}
			}
		}
		
		return moves;
	}

	this.copy_self = function( player ) {
		var candidate = new ChessBoard(player);
		candidate.board = copyBoard(_.board);
		candidate.alive = _.alive;
		candidate.scores = _.scores;
		candidate.next_move = _.next_move;
		return candidate;
	}

	this.spawn_move = function( a, b, player ) {
		var board = _.copy_self( player ) ;
		board.apply_move(a,b);
		board.annotate_board();
		board.advance();
		return board;
	}
	
	this.try_move = function ( a, b, player ) {
		var candidate = _.spawn_move( a, b, player);
		return { 
			"legal" :candidate.is_legal( player),
			"score": candidate.evaluate(player)
		};
		
	}


	
	
	this.is_legal = function( player ) {
		// need to see if I'm checked.
		if ( !_.annotated ) _.annotate_board();
		
		var chk = _.checked_players();
		return chk.indexOf(player) == -1;
	}

	this.ordered_moves = function( player ) {
		var moves = _.getAllMoves ( player );
		
		// be unpredictable!! is first principle
		moves.sort(function(a, b){
			return Math.random() < 0.5 ? -1 : 1;
		});

		// let's keep it simple!!!
		
		moves.sort(function(a, b) {
			return a.score > b.score ? -1 : 1;
		})
		return moves;
	}
	
	this.recommended_move = function(player) {
		var moves = _.ordered_moves(player);
		return moves[0];
	}
	
	
	
	this.checked_players = function () {

		if ( !_.annotated ) _.annotate_board();

		var checked = [];
		for ( var i = 0; i < _.board.length; ++i ) {
			for ( var j = 0; j < _.board[i].length;++j ) {
				if ( _.board[i][j].contents.type == 5 ) {
					if ( _.board[i][j].attacks.length ) {
						checked.push(_.board[i][j].contents.owns );
					}
				}
			}	
		}
		
		return checked;
	
	}

	this.render = function() {
		if ( _.binding ) {
			_.binding.innerHTML = _.toString();


			$("body").on("click", ".game_table td", function(e){
				window.board.processClick(+$(this).attr("data-row"), +$(this).attr("data-col"))
				e.stopImmediatePropagation();
			});

		}

	}


	this.render();
	
}



int main() {
     
}