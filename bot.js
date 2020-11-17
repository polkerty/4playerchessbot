{

var Move = function( row, col ) {
	this.row = row;
	this.col = col;
}


var Blank = function() {
	var _ = this;
	this.owns = -1;
	this.type = 0;
	this.text = "e";
	this.alive = false;
	this.points = 0;
	this.value = 0;

	this.toString = function() {
		return _.text;
	}

	this.moves = function( row, col, board ) {
		return [];
	}
	
	this.copy = function() {
		return new Blank();
	}
}


function compute_pawn_orientation ( owns, orientation ) {
	if ( orientation == 0 ) {
		if ( owns == 0 ) {
			return [ -1, 0 ];
		} else if ( owns == 1 ) {
			return [0, 1];
		} else if ( owns == 2 ) {
			return [1, 0];
		} else if ( owns == 3 ) {
			return [ 0, -1 ];
		}
	} else if ( orientation == 1 ) {
		if ( owns == 1 ) {
			return [ -1, 0 ];
		} else if ( owns == 2 ) {
			return [0, 1];
		} else if ( owns == 3 ) {
			return [1, 0];
		} else if ( owns == 0 ) {
			return [ 0, -1 ];
		}
	} else if ( orientation == 2 ) {
		if ( owns == 2) {
			return [ -1, 0 ];
		} else if ( owns == 3 ) {
			return [0, 1];
		} else if ( owns == 0 ) {
			return [1, 0];
		} else if ( owns == 1 ) {
			return [ 0, -1 ];
		}
	} else if ( orientation == 3 ) {
		if ( owns == 3 ) {
			return [ -1, 0 ];
		} else if ( owns == 0 ) {
			return [0, 1];
		} else if ( owns == 1 ) {
			return [1, 0];
		} else if ( owns == 2 ) {
			return [ 0, -1 ];
		}
	} 

}

function compute_pawn_left_attack ( owns, orientation ) {
	if ( orientation == 0 ) {
		if ( owns == 0 ) {
			return [-1, -1];
		} else if ( owns == 1 ) {
			return [-1, 1];
		} else if ( owns == 2 ) {
			return [1, 1];
		} else if ( owns == 3 ) {
			return [1, -1];
		}
	} else if ( orientation == 1 ) {
		if ( owns == 1 ) {
			return [-1, -1];
		} else if ( owns == 2 ) {
			return [-1, 1];
		} else if ( owns == 3 ) {
			return [1, 1];
		} else if ( owns == 0 ) {
			return [1, -1];
		}
	} else if ( orientation == 2 ) {
		if ( owns == 2 ) {
			return [-1, -1];
		} else if ( owns == 3 ) {
			return [-1, 1];
		} else if ( owns == 0 ) {
			return [1, 1];
		} else if ( owns == 1 ) {
			return [1, -1];
		}
	} else if ( orientation == 3 ) {
		if ( owns == 3 ) {
			return [-1, -1];
		} else if ( owns == 0 ) {
			return [-1, 1];
		} else if ( owns == 1 ) {
			return [1, 1];
		} else if ( owns == 2 ) {
			return [1, -1];
		}
	}  
	
	
}


function compute_pawn_right_attack( owns, orientation ) {
	if ( orientation == 0 ) {
		if ( owns == 0 ) {
			return [ -1, 1 ];
		} else if ( owns == 1 ) {
			return [ 1, 1 ];
		} else if ( owns == 2 ) {
			return [ 1, -1 ];
		} else if ( owns == 3 ) {
			return [ -1, -1 ];
		}
	} else if ( orientation == 1 ) {
		if ( owns == 1 ) {
			return [ -1, 1 ];
		} else if ( owns == 2 ) {
			return [ 1, 1 ];
		} else if ( owns == 3 ) {
			return [ 1, -1 ];
		} else if ( owns == 0 ) {
			return [ -1, -1 ];
		}
	} else if ( orientation == 2 ) {
		if ( owns == 2 ) {
			return [ -1, 1 ];
		} else if ( owns == 3 ) {
			return [ 1, 1 ];
		} else if ( owns == 0 ) {
			return [ 1, -1 ];
		} else if ( owns == 1 ) {
			return [ -1, -1 ];
		}
	} else if ( orientation == 3 ) {
		if ( owns == 3 ) {
			return [ -1, 1 ];
		} else if ( owns == 0 ) {
			return [ 1, 1 ];
		} else if ( owns == 1 ) {
			return [ 1, -1 ];
		} else if ( owns == 2 ) {
			return [ -1, -1 ];
		}
	} 
}
 
function probe ( row, col, delta_row, delta_col, board, is_single_hop, no_swoggle, require_enemy, non_attacking ) {
	// depth = -1 => forever
	// depth = 0 => until I run into another object.
	
	var ret = [];

	function line_probe ( row, col, delta_row, delta_col, board, is_single_hop, require_enemy ) {
		
		var ret = [];
		
		var loc = 1;
		
		var orig_row = row;
		var orig_col = col;

		row += delta_row;
		col += delta_col;
		
		
		while ( row < board.length && row > -1 && col > -1 && col < board[row].length ) {
			
			var break_next = false;
			
			
			// If it's blocked (an edge chunk), stop
			if ( board[row][col].contents.type == 1 ) {
				break;
			}
			
			if ( board[row][col].contents.type > 1 ) {
				if ( (board[row][col].contents.owns == board[orig_row][orig_col].contents.owns && 
					!board.options.permitSelfAttacks) || 
					non_attacking
				) {
					break;
				} else {
					break_next = true;
				}
			}
			
			if ( board[row][col].contents.type <= 1 && require_enemy ) {
				break;
			}
			
			ret.push( new Move ( row, col ) );
			
			if ( break_next || is_single_hop ) {
				break;
			}
			
			row += delta_row;
			col += delta_col;
		}
		
		return ret;
		
	}
	
	
	// consider the different directions the row/col can go. 
	if ( !no_swoggle ) {

	
		for ( var i = -delta_row; i < delta_row + 1; i += 2 * delta_row ) {
			for ( var j = -delta_col; j < delta_col + 1; j += 2 * delta_col ) {
			
				ret = ret.concat( line_probe( row, col, i, j, board, is_single_hop, require_enemy ) );
			
				if ( delta_col == 0 ) break;
			}
			if ( delta_row == 0 ) break;
		}
	
	} else {
		ret = line_probe ( row, col, delta_row, delta_col , board, is_single_hop, require_enemy ) ;
	}
	
	
	return ret;
	
}




var Block = function( ) {
	var _ = this;
	this.owns = -1;
	this.type =  1;
	this.text = "c";
	this.alive = false;
	this.points = 0;
	this.value = 0;

	this.toString = function() {
		return _.text ;
	}
	
	this.moves = function( row, col, board ) {
		return [];
	}
	this.copy = function() {
		return new Block();
	}
}

var Rook = function( owns ) {
	var _ = this;
	this.owns = owns;
	this.type = 2;
	this.text = "r";
	this.alive = true;
	this.points = 5;
	this.value = 5;

	this.toString = function() {
		return _.text + ":" + _.owns;
	}
	
	this.moves = function( row, col, board ) {
		var ret = probe( row, col, 1, 0, board, false);
		return ret.concat(probe( row, col, 0, 1, board, false));
	}
	this.copy = function() {
		var rook = new Rook(_.owns );
		rook.alive = _.alive;
		// other deets no change
		return rook;
	}

}

var Knight = function( owns ) {
	var _ = this;
	this.type = 3;
	this.text = "n";
	this.alive = true;
	this.owns = owns;
	this.points = 3;
	this.value = 3;

	this.toString = function() {
		return _.text + ":" + _.owns;
	}
	
	this.moves = function( row, col, board ) {
		var ret = probe( row, col, 1, 2, board, true );
		return ret.concat( probe(row, col, 2, 1, board, true ));
	}
	this.copy = function() {
		var knight = new Knight(_.owns);
		knight.alive = _.alive;
		return knight;
	}
}

var Bishop = function( owns ) {
	var _ = this;
	this.owns = owns;
	this.type = 4;
	this.text = "b";
	this.alive = true;
	this.points = 5;
	this.value = 5;

	this.toString = function() {
		return _.text + ":" + _.owns;
	}
	
	this.moves = function ( row, col, board ) {
		return probe( row, col, 1, 1, board, false ); 
	}
	this.copy = function() {
		var bishop = new Bishop(_.owns);
		bishop.alive = _.alive;
		return bishop;
	}
}

var King = function( owns ) {
	var _ = this;
	this.owns = owns;
	this.type = 5;
	this.text = "k";
	this.alive = true;
	this.points = 20;
	this.value = 20;


	this.toString = function() {
		return _.text + ":" + _.owns;
	}

	this.moves = function ( row, col, board ) {
		var ret = probe( row, col, 1, 1,  board, true );
		ret = ret.concat(probe( row, col, 1, 0, board, true ));
		ret = ret.concat(probe( row, col, 0, 1, board, true ) );
		return ret;
	}
	this.copy = function() {
		var king = new King(_.owns);
		king.alive = _.alive;
		return king;
	}
	
}

var Queen = function( owns ) {
	var _ = this;
	this.owns = owns;
	this.type = 6;
	this.text = "q";
	this.alive = true;
	this.points = 9;
	this.value = 9;


	this.toString = function() {
		return _.text + ":" + _.owns;
	}


	this.moves = function( row, col, board ) {
		var ret = probe ( row, col, 1, 1, board, false );
		ret = ret.concat( probe( row, col, 1, 0, board, false ));
		ret=  ret.concat( probe( row, col, 0, 1, board, false ));
		return ret;
	}
	this.copy = function() {
		var queen = new Queen(_.owns);
		queen.alive = _.alive;
		return queen;
	}
}

var Queen1Pt = function( owns ) {
	var _ = this;
	this.owns = owns;
	this.type = 8;
	this.text = "d";
	this.alive = true;
	this.points = 1;
	this.value = 7;


	this.toString = function() {
		return _.text + ":" + _.owns;
	}

	this.moves = function( row, col, board ) {
		var ret = probe ( row, col, 1, 1, board, false );
		ret = ret.concat( probe( row, col, 1, 0, board, false ));
		ret=  ret.concat( probe( row, col, 0, 1, board, false ));
		return ret;
	}
	this.copy = function() {
		var queen1 = new Queen1Pt(_.owns);
		queen1.alive = _.alive;
		return queen1;
	}


}

var Pawn = function( owns ) {
	var _ = this;
	this.owns = owns;
	this.type = 7;
	this.text = "p";

	this.toString = function() {
		return _.text + ":" + _.owns + ":" + (_.moved?1:0);
	}

	this.alive = true;
	this.points = 1;
	this.value = 1;
	
	
	this.moved = false;
	
	
	this.moves = function( row, col, board, orientation, only_attacks  ) {
		var dir = compute_pawn_orientation ( _.owns, orientation );
		var ret = [];
				
		if ( !only_attacks ) {
			ret = ret.concat (probe( row, col, dir[0], dir[1], board, true, true, false, true ));
		
			if ( !_.moved && isOpen( board, row, col, dir ) ) {
				// must take special care not to run into other pieces on the way...
				ret = ret.concat( probe ( row, col, dir[0] * 2, dir[1] * 2, board,  true, true, false, true ) );
			}
		}
		
		var left_attack = compute_pawn_left_attack( _.owns, orientation );
		ret = ret.concat( probe( row, col, left_attack[0], left_attack[1], board, true, true, true ));
		var right_attack = compute_pawn_right_attack( _.owns, orientation );
		ret = ret.concat( probe( row, col, right_attack[0], right_attack[1], board, true, true, true ));
		
		return ret;
	}
	this.copy = function() {
		var pawn = new Pawn(_.owns);
		pawn.alive = _.alive;
		pawn.moved = _.moved;
		return pawn;
	}
	
	
	
}


function inBounds( board, row, col ) {
	return row > -1 && col > -1 && row < board.length && col < board[0].length;
}

function isOpen( board, row, col, dir ) {
	var x = row + dir[0], y = col + dir[1];
	if ( !inBounds( board, x, y ) ) return false;
	if ( ! board[x][y].contents.type == 0 ) return false;
	
	
	return true;
}	

function isQueeningMove( piece, from, to ) {
	if ( !(piece.type == 7) ) return false;
	return (piece.owns == 1 && from.row == 6&& to.row == 7) || 
	(piece.owns == 3 && from.row == 7 && to.row == 6) ||
	(piece.owns == 0 && from.col == 6 && to.col == 7 ) ||
	(piece.owns == 2 && from.col == 7 && to.col == 6);
}

var Square = function( row, col, contents ) {
	var _ = this;
	this.row = row;
	this.col = col;
	this.contents = contents;

	this.targets = []
	this.attacks = []
	this.defenses = []
	
	this.copy = function() {
		var s = new Square( _.row, _.col, _.contents.copy() );
		return s;
	}
	
}


var start_map = 



`c	c	c	r:1	n:1	b:1	q:1	k:1	b:1	n:1	r:1	c	c	c
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
c	c	c	r:3	n:3	b:3	k:3	q:3	b:3	n:3	r:3	c	c	c`


var test_map = 



`c	c	c	r:1	n:1	b:1	q:1	k:1	b:1	n:1	r:1	c	c	c
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
c	c	c	r:3	n:3	b:3	k:3	q:3	b:3	n:3	r:3	c	c	c`



function boardFromMap( map ) {
	map = map.split("\n");
	for ( var i = 0; i < map.length; ++i ) {
		map[i] = map[i].split("\t");
		for ( var j = 0; j < map[i].length; ++j ) {
			var t = map[i][j].split(":");
			if ( t[1] ) {
				t[1] = +t[1];
			}
			switch( t[0] ) {
				case "c":
					map[i][j] = new Square ( i, j, new Block());
					break;
				case "e":
					map[i][j] = new Square ( i, j, new Blank());
					break;
				case "r":
					map[i][j] = new Square ( i, j, new Rook(t[1]));
					break;
				case "n":
					map[i][j] = new Square ( i, j, new Knight(t[1]));
					break;
				case "b":
					map[i][j] = new Square ( i, j, new Bishop(t[1]));
					break;
				case "k":
					map[i][j] = new Square ( i, j, new King(t[1]));
					break;
				case "q":
					map[i][j] = new Square ( i, j, new Queen(t[1]));
					break;
				case "d":
					map[i][j] = new Square ( i, j, new Queen1Pt(t[1]));
					break;
				case "p":
				
					var moved = t.length >= 3 && t[2] == "1";
					
					map[i][j] = new Square ( i, j, new Pawn(t[1]));
					map[i][j].contents.moved = moved;
					break;
				default:
					break;
			}
		}
	}
	
	map.options = {};
	return map;
}

var george = 0;

function clone (obj) {
	var ret = JSON.parse(JSON.stringify(obj));
	return ret;
	
}

function copyBoard(b) {
	var a = [];
	for ( var i = 0; i < b.length; ++i ) {
		var row = [];
		for ( var j = 0 ; j < b[i].length; ++j ) {
			row.push(b[i][j].copy() );
		}
		a.push(row);
	}
	return a;
}

var ChessBoard = function( me, map, binding ) {

	map = map || start_map

	var _ = this;
	this.board = boardFromMap( map );
	
	this.binding = binding;

	this.scores = [0, 0, 0, 0];
	this.alive = [1, 1, 1, 1];
	
	this.me = me;
	this.next_move = me; //by default

	this.options_list = [{}, {}, {}, {}];

	this.isMyTurn = function() {
		return _.me == _.next_move;
	}
	this.advance = function() {
		var cand = this.next_move;
		for ( var i = 0; i < _.alive.length; ++i ) {
			cand = (cand + 1)%_.alive.length;
			if ( _.alive[cand] ) {
				_.next_move = cand;
				break;
			}
		}
	}

	this.things = null;
	this.processClick = function(row, col ) {
		if ( _.things === null ) {
			_.things = new Move(row, col);
		} else {
			_.apply_move(_.things, new Move(row, col));
			_.things = null;
		}
	}

	this.toMap = function() {
		var ret = "";
		for ( var i = 0; i < _.board.length; ++i ) {
			if ( i > 0 ) ret += "\n";
			for ( var j = 0; j < _.board[i].length; ++j ) {
				if ( j > 0 ) ret += "\t";
				ret += _.board[i][j].contents.toString();
			}
		}
		return ret;
	}

	this.toState = function() {
		var ret = {
			map: _.toMap(),
			scores: _.scores,
			alive: _.alive,
			options: _.options_list,
			next_move: _.next_move
		};
		return ret;
	}

	this.fromState = function(state) {
		_.board = boardFromMap(state.map);
		_.scores = state.scores;
		_.options_list = state.options;
		_.next_move = state.next_move;
		_.alive = state.alive;
	}

	this.statefulMove = function(state) {
		_.fromState(state);
		return _.ab_recommended_move(_.next_move);
	}


	this.toString = function() {
		var html = "";

		html += "<div class='scores' >Scores: ";
		var color_names = ["Red", "Blue", "Yellow", "Green"];
		for ( var i = 0; i < color_names.length; ++i ) {
			html += color_names[i] + ": " + _.scores[i] + "<span style='display:inline-block; width:30px;' ></span>";
		}
		html += "</div>";
		html += "<div class='cur_turn' >";
		html += "Cur turn: " + color_names[_.next_move];
		html += "</div>";


		html += "<table class='game_table'>";

		for ( var i = 0 ; i < _.board.length; ++i ) {
			html += "<tr>";
			for ( var j = 0; j < _.board[i].length; ++j ) {
				html += "<td data-row='" + i + "' data-col='" + j +
				 "' class='" + _.board[i][j].contents.text + 
				 " cc owns" + _.board[i][j].contents.owns + "' >" +
					 " " /*_.board[i][j].contents.toString()*/ + "</td>";
			}
		}

		html += "</table>";

		return html;
	}

	
	this.alpha_beta = function ( plies_remaining, scan_hierarchy, player ) {

		george = george + 1; if ( george % 25 == 0 ) console.log(george + " plies examined", player, scan_hierarchy);

		player == player === undefined ? me : player;
		
		var horizon = scan_hierarchy[player];
		
		var move_list = _.ordered_moves( player ).slice(0, horizon);

		if ( plies_remaining == 0 ) {
			return {
				move: move_list[0],
				scores: _.evaluate_all()
			}
		}
		var scores = [0, 0, 0, 0], move_index = -1;
		for  ( var i = 0; i < move_list.length; ++i ) {
			var child = _.spawn_move(move_list[i].from, move_list[i].to, player );
			
			var ply_result = child.alpha_beta( plies_remaining - 1, scan_hierarchy, child.next_move );
			// now let's score this for ourselves...

			if ( ply_result.scores[player] > scores[player] || i == 0 ) {
				scores = ply_result.scores;
				move_index = i;
			}
		}

		return {
			move: move_list[move_index],
			scores: scores
		};


	}

	_.ab_width = [3, 2, 1, 1];
	_.ab_depth = 10;

	this.ab_recommended_move = function(player){
		var ab_rotated = new Array(_.ab_width.length);
		for ( var i = 0; i < _.ab_width.length; ++i ) {
			ab_rotated[(i + player)%_.ab_width.length] = _.ab_width[i];
		}
		return _.alpha_beta(_.ab_depth, ab_rotated, player).move; 
	}

	
	this.indexToCode = function( row, col ) {
		if ( _.orientation == 3 ) {
			var rows = "abcdefghijklmn";
			
			return rows[row] + "" +  (col + 1);
		}
		
		 else {
		 	// let's ignore this for now
		 }
	}
	this.codeToIndex = function ( code ) {
		var rows = "abcdefghijklmn";
		var row = rows.indexOf(code[0]);
		var col = (+code.slice(1))   - 1;
		
		return new Move(row, col);
	}
	
	//green on bot; this aligns with the map
	this.orientation = 3;
	 
	
	// this modifies the board to see how many times a square is attacked,
	// and what pieces it attacks.
	this.annotate_board = function() {
		if ( _.annotated ) return;
		_.board.options = _.board.options || {}
		_.board.options.permitSelfAttacks = true;
		
		_.applyAll(function( piece, i, j, board) {
			board[i][j].attacks = [];
			board[i][j].targets = [];
			board[i][j].defenses = [];
		});

		for ( var i = 0; i < _.board.length; ++i ) {
			for ( var j = 0 ; j < _.board[i].length; ++j ) {
				if ( _.board[i][j].contents.owns > -1 && _.board[i][j].contents.alive ) {
					var possible_moves = _.board[i][j].contents.moves( i, j, _.board, _.orientation, true );
					//console.log(i, j, _.board[i][j].contents, possible_moves);
					_.board[i][j].targets = [];
					
					for ( var k = 0; k < possible_moves.length; ++k ) {
						var move = possible_moves[k];
						_.board[move.row][move.col].attacks = _.board[move.row][move.col].attacks || [];
						_.board[move.row][move.col].defenses = _.board[move.row][move.col].defenses || [];
						
						if ( _.board[move.row][move.col].contents.owns == _.board[i][j].contents.owns ) {
							_.board[move.row][move.col].defenses.push(new Move(i, j));
						} else { 
							_.board[move.row][move.col].attacks.push(new Move(i, j));
						}
						_.board[i][j].targets.push(move);
					}
					
					
				}
			}
		}
		
		_.board.options.permitSelfAttacks = false;
		_.annotated = true;
	}	
		
	this.apply_move = function( from, to, upgrade ) {
		
		//debugger;
		var piece = _.board[from.row][from.col].contents;
		
		if ( isQueeningMove(piece, from, to )) {
			upgrade = true;
		}

		if ( upgrade ) {
			piece = new Queen1Pt(piece.owns);
		}
		
		_.board[from.row][from.col].contents = new Blank();
		
		piece.moved = true; // pawns need to know
		
		
		if ( _.board[to.row][to.col].contents.owns > -1 &&
			_.board[to.row][to.col].contents.alive ) {
			_.scores[piece.owns] += 
				_.board[to.row][to.col].contents.points;
		}
		
		_.board[to.row][to.col].contents = piece;

		_.annotated = false;

		if ( _.binding ) {
			_.render();
		}
		
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
		

		options = Object.assign({
			banked_point_multiplier: 0.9,
			lost_point_multiplier: 1, // lower values are more aggressive
			my_piece_multiplier: 1,
		 	threat_point_multiplier: 0.3, // a little value for making threats.
			potential_point_multiplier: 0.3,
			check_points_right: 2, // we like this
			check_points: 1
		}, _.options_list[player]);

	
		// we want a high score.
		// how many points at risk
		// points - how many points at risk. 
		
		var score = _.scores[player] * options.banked_point_multiplier;
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
						score += _.board[i][j].contents.value * options.my_piece_multiplier;
					} else {
						score -= _.board[i][j].contents.value * options.lost_point_multiplier;
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

					if ( !_.board[i][j].contents.alive ) aval = 0;
					
					if ( aval > 0 ) {
						score += options.potential_point_multiplier*aval;
						if ( verbose ) {
							console.log ("At "  + i + "," + j + " our threat points are: " + options.potential_point_multiplier*aval);
						}
					}
					if ( _.board[i][j].contents.type == 5 && threatsFromPlayer.length && aval > 0) {
						if ( _.board[i][j].contents.owns + 1 == player )
						score += options.check_points_right;
						if ( verbose ) {
							console.log("Points for checking: " + options.check_points_right)
						}
					} else {
						score += options.check_points;
						if ( verbose ) {
							console.log("Points for checking: " + options.check_points)
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

	this.kill = function(p, keep_king_alive) {
		_.alive[p] = false;
		_.applyAll( function( piece, i, j, board) {
			if ( piece.contents.owns == p  && (piece.contents.type != 5 || !keep_king_alive ) ) {
				piece.contents.alive = false;
			}
		});
	};

	
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
		candidate.scores = _.scores.slice(0);
		candidate.next_move = _.next_move;
		candidate.options_list = _.options_list;
		candidate.ab_depth = _.ab_depth;
		candidate.ab_width = _.ab_width;
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


}

try {
	exports.Board = ChessBoard;
} catch (e) {
	
}
