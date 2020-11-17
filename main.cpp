#include "core.cpp"
#include <fstream>

const int MAX_PLIES = 300;

vector<pair<int, int>> tourney( Game game, vector<int> options_inds, int move_cnt ) {
	int cur_ply = 0;
	int alive = 4;
	
	while ( cur_ply < move_cnt && alive > 1) {
		
		game.do_ab_play();
		if ( cur_ply % 25 == 0 ) { cerr << cur_ply << endl; }
			
			
			
		++cur_ply;
		
		alive = 0;
		for ( int i = 0; i < game.alive.size() ; ++i ) alive += game.alive[i];
		
	}
	
	
	vector<pair<int, int>> rank ;
	for ( int i = 0; i < options_inds.size(); ++i ) {
		rank.push_back({i, options_inds[i]});
	}
	
	vector<int> vob = game.value_on_board();
	sort(rank.begin(), rank.end(), [&game, &vob](const pair<int, int>& a, const pair<int, int>& b) {
		if ( game.moves[a.first] == game.moves[b.first] ) return game.scores[a.first] > game.scores[b.first];
		return game.moves[a.first] > game.moves[b.first];
	});
	
	return rank;
}



int main(int argc, char**argv) {


	if ( argc < 9 ) {
		cout << "Usage: ./bot optionsfile.txt outfile.txt ID move_cnt ab_depth redtype bluetype yellowtype greentype" << endl;
		return 0;
	}
	
	ifstream options_file(argv[1]);
	
	vector<int> options_inds;
	for ( int i = 0; i < 4; ++i ) options_inds.push_back(stoi(argv[i + 6]));
	
	int ab_depth = stoi(argv[5]);
	int move_cnt = stoi(argv[4]);
	
	vector<PlayerOptions> popts;
	
	int total_option_types;
	options_file >> total_option_types;
	
	for ( int i = 0; i < total_option_types; ++i ) {
		double bpp, lpp , mpm, tpm , cpr, cp;
		
		options_file >> bpp >> lpp >> mpm >> tpm >> cpr >> cp;
		
		popts.push_back(PlayerOptions(bpp, lpp, mpm, tpm, cpr, cp));
	}
	
	vector<PlayerOptions> options_list;
	for ( int i = 0; i < 4; ++i ) {
		options_list.push_back(popts[options_inds[i]]);
	}
	

	cout << "GAME SEED: " << seed << endl;
	Game game(0);
	
	game.options_list = options_list;
	
	game.ab_depth = ab_depth;
	
	
	vector<pair<int, int>> results = tourney(game, options_inds, move_cnt);
	
	
	
	ofstream results_file(argv[2], ios_base::app);
	
	for ( auto r: results ) {
		cout << r.first << ' ' << r.second << endl;
		
		results_file << r.second << "\t";
	}
	results_file << "\n";
	
	
}
