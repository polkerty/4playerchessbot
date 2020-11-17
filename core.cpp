#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <set>

#include "bot.h"

using namespace std;

int george = 0;

bool inBounds( Board& board, int row, int col ) {
	return row > -1 && col > -1 && row < board.rows && col < board.cols;
}

bool isOpen( Board& board, int row, int col, vector<int> dir ) {
	int x = row + dir[0], y = col + dir[1];
	if ( !inBounds( board, x, y ) ) return false;
	if ( ! (board.at(x, y).contents.type == 0) ) return false;
	
	
	return true;
}	
bool isQueeningMove( Piece& piece, Move from, Move to ) {
	if ( !(piece.type == 7) ) return false;
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
	
	return { 0, 0 };

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
			return {1, -1};
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
	
	return {0, 0};
	
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
	
	return {0, 0};
}


vector<Move> line_probe ( int row, int col, int delta_row, int delta_col, 
    Board& board, bool is_single_hop, bool require_enemy, bool non_attacking ) {
    
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
    Board& board, bool is_single_hop , bool no_swoggle , bool require_enemy , 
    bool non_attacking  ) {
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

    Blank( int owns ) : Piece( owns ) {
        type = 0;
        text = "e";
        alive = false;
        points = 0;
        value = 0;
    }

	vector<Move> moves( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
		return {};
	}
	
	Piece copy (){
		Piece blank = Blank(owns);
		return blank;
	}
};



struct Block : Piece {

    Block (int owns ) : Piece(owns) {
        type = 1;
        text = "c";
        alive = false;
        points = 0;
        value = 0;
    }


	
	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
		return {};
	}
	Block copy () {
		return Block(-1);
	}
};

struct Rook : Piece {

    Rook ( int owns ) : Piece(owns ) {
        type = 2;
        text = "r";
        alive = true;
        points = 5;
        value = 5;
    }
	
	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
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
        text = "n";
        alive = true;
        owns = owns;
        points = 3;
        value = 3;
    }
	
	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
		vector<Move> ret = probe( row, col, 1, 2, board, true );
        vector<Move> ret2 = probe(row, col, 2, 1, board, true ) ;
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
        text = "b";
        alive = true;
        points = 5;
        value = 5;

    }
	
	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
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
        text = "k";
        alive = true;
        points = 20;
        value = 20;
    }

	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
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
        text = "q";
        alive = true;
        points = 9;
        value = 9;
    }


	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
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
        type = 8;
        text = "d";
        alive = true;
        points = 1;
        value = 7;
    }

    vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
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
        text = "p";
        alive = true;
        points = 1;
        value = 1;
    }
	
	vector<Move> moves ( int row, int col, Board& board, int orientation = 0 , bool only_attacks = false  ) {
		vector<int> dir = compute_pawn_orientation ( owns, orientation );
		vector<Move> ret;
				
		if ( !only_attacks ) {
            vector<Move> ret2 = probe( row, col, dir[0], dir[1], board, true, true, false, true ); 
			ret.insert ( ret.end(), ret2.begin(), ret2.end());
		
			if ( !moved && isOpen( board, row, col, dir ) ) {
				// must take special care not to run into other pieces on the way...
                ret2 = probe ( row, col, dir[0] * 2, dir[1] * 2, board,  true, true, false, true );
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
					out_row.push_back( Square ( i, j, Block(-1)));
					break;
				case 'e':
					out_row.push_back( Square ( i, j, Blank(-1)));
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
			
			if ( t.size() > 2 && t[2] == "1" ) {
				out_row[out_row.size()].contents.moved = true;
			}			
			
		}
		 ret.push_back(out_row);

	}
	
	return Board(ret);
}


Board copyBoard(Board& b) {
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
	vector<int> scores, alive, moves;
	vector<PlayerOptions> options_list;
	int orientation;
	bool annotated = false;

	vector<int> ab_width = {3, 2, 1, 1};
	int ab_depth = 5;
	
	
	



	Game ( int _me, string map = start_map, int nm = 0 ) {
		board = boardFromMap(map);

		scores = {0, 0, 0 , 0};
		alive = {1, 1, 1, 1};
		moves = {0, 0, 0, 0};
		
		options_list = { PlayerOptions(), PlayerOptions(), PlayerOptions(), PlayerOptions() };

		me = _me;
		next_move = nm;
		orientation = 3;
	}

	bool isMyTurn () {
		return me == next_move;
	}

	void advance() {
		int cand = next_move;
		moves[cand]++;
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
				if ( j > 0 ) map += "\t";
				map += board.at(i, j).contents.toString();
			}
			map += "\n";
		}
		return map;

	}


	vector<int> value_on_board () {
		vector<int> ret(4);
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0; j < board.cols; ++j ) {
				if ( board.at(i, j).contents.type > 1 && 
					alive[board.at(i, j).contents.owns] ) {
					ret[ board.at(i, j).contents.owns ] += 
					board.at(i, j).contents.value;	
				}
			}
		}
	
		return ret;
	}
	
	void show () {
		vector<string> players = {"Red", "Blue", "Yellow", "Green"};
		vector<int> vob = value_on_board();
		cout << "Cur turn: " << players[next_move] << endl;
		for ( int i = 0 ; i < 4; ++i ) {
			cout << players[i] << ": " << scores[i] << "; Alive? " << 
				(alive[i]? "Yes":"No") << "; Moves: " << moves[i] << 
				"; vob: " << vob[i] << endl;
		}
		cout << toString() << endl;
		
		cout << "------------------" << endl;
	}

	void annotate_board () {
		if ( annotated ) return;
		board.options.permitSelfAttacks = true;
		
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0; j < board.cols; ++j ) {
				board.at(i, j).attacks.clear();
				board.at(i, j).targets.clear();
				board.at(i, j).defenses.clear();
			}
		}
		
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0 ; j < board.cols; ++j ) {
				if ( board.at(i, j).contents.owns > -1 && board.at(i, j).contents.alive ) {
					vector<Move> possible_moves = board.at(i, j).contents.moves( i, j, board, orientation, true );
					//console.log(i, j, _.board[i][j].contents, possible_moves);
					board.at(i, j).targets.clear();
					
					for ( int k = 0; k < possible_moves.size(); ++k ) {
						Move move = possible_moves[k];
						if ( board.at(move).contents.owns == board.at(i, j).contents.owns ) {
							board.at(move).defenses.push_back(Move(i, j));
						} else { 
							board.at(move).attacks.push_back(Move(i, j));
						}
						board.at(i, j).targets.push_back(move);
					}
					
					
				}
			}
		}
		
		board.options.permitSelfAttacks = false;
		annotated = true;
	}	
	
	Game copy_self ( int player ) {
		Game candidate = Game(player);
		candidate.board = copyBoard(board);
		candidate.alive = alive;
		candidate.scores = scores;
		candidate.next_move = next_move;
		candidate.options_list = options_list;
		candidate.moves = moves;
		candidate.ab_depth = ab_depth;
		candidate.ab_width = ab_width;
		return candidate;
	}

	void apply_move ( Move from, Move to, bool upgrade = false ) {
		
		if ( !from.is_valid() || !to.is_valid() ) {
			cerr << "Invalid move, ignoring!" << endl;
			return;
		}
		//debugger;
		Piece piece = board.at(from).contents;
		
		if ( isQueeningMove(piece, from, to )) {
			upgrade = true;
		}

		if ( upgrade ) {
			piece = Queen1Pt(piece.owns);
			piece.alive = alive[piece.owns];
		}
		
		board.at(from).contents = Blank(-1);
		
		piece.moved = true; // pawns need to know
		
		
		if ( board.at(to).contents.owns > -1 &&
			board.at(to).contents.alive ) {
			scores[piece.owns] += 
				board.at(to).contents.points;
		}
		
		board.at(to).contents = piece;

		annotated = false;

		

	}
	
	Game spawn_move ( Move a, Move b, int player ) {
		Game board = copy_self( player ) ;
		board.apply_move(a,b);
		board.annotate_board();
		board.advance();
		return board;
	}
	


	vector<int> checked_players  () {

		if ( !annotated ) annotate_board();

		vector<int> checked;
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0; j < board.cols;++j ) {
				if ( board.at(i, j).contents.type == 5 ) {
					if ( board.at(i, j).attacks.size() ) {
						checked.push_back(board.at(i, j).contents.owns );
					}
				}
			}	
		}
		
		return checked;
	
	}

	double attack_val ( int point_value, vector<Move> attackers, vector<Move> defenders ) {

			if ( !attackers.size() ) return 0;
			if ( !defenders.size() ) return point_value;
			double points = point_value;
			double cost = 0;

			
			auto sorter = [this]( const Move& a, const Move& b ) {
				return this->board.at(a.row, a.col).contents.value < this->board.at(b.row, b.col).contents.value;
			};
			sort( attackers.begin(), attackers.end(), sorter);
			sort( defenders.begin(), defenders.end(), sorter);
			int ind = 0;
			if (defenders.size() >= attackers.size() ) {
				cost += board.at(attackers[0].row,attackers[0].col).contents.value;
				ind ++;

			}
			while ( ind < min(attackers.size(), defenders.size()) && 
					board.at(defenders[ind]).contents.value <=
					board.at(attackers[ind]).contents.value ) {
				points += board.at(defenders[ind]).contents.value;
				cost += board.at(attackers[ind]).contents.value;
				++ind;
			}
				

			
			return points - cost*1.1;

	}



	double evaluate ( int player, bool verbose = false ) {
		
		if ( !alive[player] ) return -1000000;	
		if ( !annotated ) annotate_board();
		
		
		

		PlayerOptions options = options_list[player];
	
		// we want a high score.
		// how many points at risk
		// points - how many points at risk. 
		
		double score = scores[player] * options.banked_point_multiplier;
		// any checks are impossible; subtract infinity to avoid them.
		
		if ( !is_legal(player ) ) {
			score -= 1000000;
		}
		
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0; j < board.cols; ++j ) {
				if ( board.at(i, j).contents.owns == player ) {
				// find the smallest value attacker/defender

					
					double aval = attack_val(board.at(i, j).contents.value,
						 board.at(i, j).attacks,
						 board.at(i, j).defenses);
					
					if (verbose && aval > 0 ) {
						cerr <<"At " << i << ", " << j << " - the total threats against us work out to " <<aval << endl;;
					
					}
					if ( aval <= 0  ) {
						score += board.at(i, j).contents.value * options.my_piece_multiplier;
					} else {
						score -= board.at(i, j).contents.value * options.lost_point_multiplier;
					}
				} else {
					// let's see if we threaten them...

					vector<Move> threatsFromPlayer;
					vector<Move> defenses = board.at(i, j).defenses;
					for ( int k = 0; k < board.at(i ,j).attacks.size(); ++k ) {
						Move from = board.at(i, j).attacks[k];
						if (board.at(from.row, from.col).contents.owns != player ) {
							//defenses.push(from);
						} else {
							threatsFromPlayer.push_back(from);
						} 
					}


					int aval = attack_val(board.at(i, j).contents.value, threatsFromPlayer,
					defenses);


					if ( aval > 0 ) {
						score += options.potential_point_multiplier*aval;
						if ( verbose ) {
						 	cerr <<"At "  << i << "," << j << " our threat points are: " << (options.potential_point_multiplier*aval) << endl;;
						}
					}
					if ( board.at(i, j).contents.type == 5 && threatsFromPlayer.size()) {
						if ( board.at(i, j).contents.owns + 1 == player )
						score += options.check_points_right;
						if ( verbose ) {
							cerr << "Points for checking: " << options.check_points_right << endl;
						}
					} else {
						score += options.check_points;
						if ( verbose ) {
							cerr << "Points for checking: " << options.check_points << endl;
						}
					}

				}
			}
			
		}
		
		return score;
		
	}

	vector<double> evaluate_all () {
		vector<double> results = {0, 0, 0, 0};
		for ( int i = 0; i < results.size(); ++i ) {
			results[i]  = evaluate( i );
		}
		return results;
	}


	bool is_legal ( int player ) {
		// need to see if I'm checked.
		if ( !annotated ) annotate_board();
		
		vector<int> chk = checked_players();
		
		for ( int i = 0; i < chk.size(); ++i ) if ( chk[i] == player ) return false;
		return true;
	}



	MoveEvaluation try_move  ( Move a, Move b, int player ) {
		Game candidate = spawn_move( a, b, player);
		return MoveEvaluation( candidate.evaluate_all(), candidate.is_legal( player ) );
		
	}


	vector<MoveRecommendation> getAllMoves ( int player = -2 ) {
		
		//debugger;
		vector<MoveRecommendation> moves;
		
		player = player == -2 ? me : player;
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0 ; j < board.cols; ++j ) {
				if ( board.at(i, j).contents.owns == player && board.at(i, j).contents.alive ) {
					vector<Move> possible_moves = board.at(i, j).contents.moves( i, j, board, orientation );
					//console.log(i, j, _.board[i][j].contents, possible_moves);
					for ( int k = 0; k < possible_moves.size(); ++k ) {
						MoveEvaluation result = try_move( Move( i, j), possible_moves[k], player );
						if ( result.legal ) {
							moves.push_back( 
								MoveRecommendation(
									MovePair(
										Move(i, j), 
										possible_moves[k] 
									),
									result.scores
								)
							);
						}
					}
				}
			}
		}
		
		return moves;
	}

	vector<MoveRecommendation> ordered_moves ( int player ) {
		vector<MoveRecommendation> moves = getAllMoves ( player );
		
		// be unpredictable!! is first principle
		
		shuffle(moves.begin(), moves.end(), rng);

		// let's keep it simple!!!
		
		sort(moves.begin(), moves.end(), [player]( const MoveRecommendation&a, const MoveRecommendation& b) {
			return a.scores[player] > b.scores[player];
		});
		
		return moves;
	}
	



// 	MoveRecommendation alpha_beta ( int plies_remaining, vector<int> scan_hierarchy, int player = -2) {
// 	
// 		player = player == -2 ? me : player;
// 		
// 		int horizon = scan_hierarchy[player];
// 
// 		vector<MoveRecommendation> possible_moves = ordered_moves ( player );
// 
// 		
// 		vector<MoveRecommendation> move_list ( possible_moves.begin(), possible_moves.begin() + max(possible_moves.size(), (size_t)horizon));
// 
// 		if ( plies_remaining == 0 ) {
// 			return move_list[0];
// 		}
// 		vector<double> scores = {0, 0, 0, 0};
// 		int move_index = -1;
// 		for  ( int i = 0; i < move_list.size(); ++i ) {
// 			Game child = spawn_move(move_list[i].move.from, move_list[i].move.to, player );
// 			
// 			MoveRecommendation ply_result = child.alpha_beta( plies_remaining - 1, scan_hierarchy, child.next_move );
// 			// now let's score this for ourselves...
// 
// 			if ( ply_result.scores[player] > scores[player] || i == 0 ) {
// 				scores = ply_result.scores;
// 				move_index = i;
// 			}
// 		}
// 
// 		return move_list[move_index];
// 
// 
// 	}

	
	MoveRecommendation alpha_beta  ( int  plies_remaining, vector<int> scan_hierarchy, int player = -2 ) {
		
		
		
		//george = george + 1; if ( george % 50 == 0 ) cerr << to_string(george) + " plies examined / " << player << ' ' << plies_remaining << endl;

		player = player == -2 ? me : player;
		
		int horizon = scan_hierarchy[player];
		
		vector<MoveRecommendation> possible_moves = ordered_moves(player);
		
		vector<MoveRecommendation> move_list(possible_moves.begin(), possible_moves.begin() + min(possible_moves.size(), (size_t)horizon));
		
		if ( move_list.size() == 0 ) {
			return INVALID;
		}
		
		if ( plies_remaining == 0 ) {
			return move_list[0];
		}
		vector<double> scores = {0, 0, 0, 0};
		int move_index = -1;
		
		
		for  ( int i = 0; i < move_list.size(); ++i ) {
		
			if ( move_list[i] == INVALID ) {
				continue;
			}
			Game child = spawn_move(move_list[i].move.from, move_list[i].move.to, player );
			
			MoveRecommendation ply_result = child.alpha_beta( plies_remaining - 1, scan_hierarchy, child.next_move );
			// now let's score this for ourselves...

			if ( ply_result.scores[player] > scores[player] || i == 0 ) {
				scores = ply_result.scores;
				move_index = i;
			}
		}
		
		if ( move_index == -1 ) return INVALID;

		return move_list[move_index];


	}



	MovePair ab_recommended_move ( int player){
		MoveRecommendation move = alpha_beta(ab_depth, {3, 2, 1, 1}, player); 
		if ( move == INVALID ) {
			kill(player);
		}
		return move.move;	
	}

	MovePair recommended_move (int player) {
	
		vector<MoveRecommendation> moves = ordered_moves(player);
		return moves[0].move;
	}
	
	void kill (int p) {
		alive[p] = false;
		for ( int i = 0; i < board.rows; ++i ) {
			for ( int j = 0; j < board.cols; ++j ) {
				if ( board.at(i,j).contents.owns == p ) {
					board.at(i, j ).contents.alive = false;
				}
			}
		}
	}

	
	void process () {

		if ( !annotated ) annotate_board();
		
		vector<int> checked = checked_players();
		set<int> clist ( checked.begin(), checked.end() );
		
		for ( int i = 0; i < 4; ++i ) {
			vector<MoveRecommendation> mv = getAllMoves(i);
			if (mv.size() == 0 ) {
				if ( alive[i] ) {
					kill(i);
				}
				
				// checkmate:
				if ( clist.find(i) != clist.end() ) {
					scores[next_move] += 20;
				} else { 
				
					//stalemate
					scores[i] += 10; 
				}
			}
		}
	}
	
	void tally() {
		process();
		advance();
	}	
	
	void do_ab_play(  ) {
		MovePair best_move = ab_recommended_move(next_move);
		apply_move(best_move.from, best_move.to);
		tally();

	}

};

