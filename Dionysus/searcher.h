#pragma once

#include "transposition_table.h"
#include "board.h"

class Searcher {

	struct BookEntry {
		unsigned long long key;
		unsigned short move;
		unsigned short weight;
		unsigned int learn;
	};

	BookEntry* entries = nullptr;
	int book_size = 0;
	bool searching = false;
	bool using_opening_book = true;
	int searchID = 0;
	TranspositionTable trans_table;

	void init_opening_book();
	double quiescence(double, double, Board*);
	SearchResult negamax(int, double, double, Board*, Move first = { -1 });
	void stop_searching(int, int);
	Move decipher_polyglot_move_code(unsigned short code, Board* board);

public:
	Searcher();

	Move get_best_move(int, Board*);
	Move get_random_move(Board*);

	void stop();

};

