#pragma once

#include "transposition_table.h"
#include "board.h"

class Searcher {

	bool searching = false;
	TranspositionTable trans_table;

	double quiescence(double, double, Board*);
	SearchResult negamax(int, double, double, Board*, Move first = { -1 });
	void stop_searching(int);

public:
	Searcher();

	Move get_best_move(int, Board*);
	Move get_random_move(Board*);

	void stop();

};

