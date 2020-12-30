#include "searcher.h"
#include "limits.h"
#include "utils.h"

#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <time.h>
#include <iostream>
#include <iomanip>

Searcher::Searcher() {
	trans_table.clear();
	if (using_opening_book) init_opening_book();
}

//used to sort moves before alpah beta pruning
//captures are first, as these are more likely to be good, making ab pruning more effective
bool compare_moves(Move a, Move b) {
	bool a_capture = a.prev_square != EMPTY_SQUARE;
	bool b_capture = b.prev_square != EMPTY_SQUARE;

	if (!a_capture && !b_capture) {
		return false;
	}
	else if (a_capture && !b_capture) {
		return true;
	}
	else if (!a_capture && b_capture) {
		return false;
	}

	return (a.prev_square % 6) > (b.prev_square % 6);
}

//load opening book moves into memory
void Searcher::init_opening_book() {
	FILE* book;
	errno_t res = fopen_s(&book, BOOK_NAME, "r");

	//if opening the file failed
	if (res != 0 || !book) {
		std::cout << "Cant open file" << std::endl;
		using_opening_book = false;
		return;
	}

	//find out how many entries there are
	long num_entries;
	fseek(book, 0, SEEK_END);
	num_entries = ftell(book) / sizeof(BookEntry);
	rewind(book);

	entries = (BookEntry*)malloc(sizeof(BookEntry) * num_entries);

	if (!entries) {
		std::cout << "Something went wrong" << std::endl;
		using_opening_book = false;
		return;
	}

	//actually read the data
	int result = fread(entries, sizeof(BookEntry), num_entries, book);

	using_opening_book = num_entries > 0;
	book_size = num_entries;
}

//quiescence is run at each terminal node in negamax, to stabilise the position
//means we do not stop search halfway through a queen trade, and think we are a queen up/down
double Searcher::quiescence(double alpha, double beta, Board *board) {

	//current eval
	double standing_pat = (board->is_white_to_move() ? 1 : -1) * board->evaluate_position();

	alpha = std::max(alpha, standing_pat);

	if (alpha >= beta) return beta;

	//get all captures and sort them by what they are capturing (higher value targets first)
	std::vector<Move> valid_moves = board->get_valid_captures(board->is_white_to_move() ? WHITE : BLACK);
	std::sort(valid_moves.begin(), valid_moves.end(), compare_moves);

	//iterate through each capture, as in negamax
	for (const Move &m : valid_moves) {
		if (board->make_move(m)) {
			double score = -quiescence(-beta, -alpha, board);
			board->undo_move(m);
			alpha = std::max(alpha, score);
			if (alpha >= beta) return beta;
		}
	}

	//return best score seen
	return alpha;
}

SearchResult Searcher::negamax(int depth, double alpha, double beta, Board *board, Move first) {

	//cancel search is necessary
	if (!searching) return { };

	double alphaOrig = alpha;
	
	
	//check to see if this position has already been calculated to this depth or further
	TransTableEntry* trans_entry = trans_table.get_if_exists(board->get_zobrist_hash());
	if (trans_entry->flag != NOT_PRESENT && trans_entry->depth >= depth) {

		//if we have the exact score, we can just return this
		if (trans_entry->flag == EXACT) {
			return trans_entry->sr;
		}
		//if we only have a lower bound, we can update alpha using this
		else if (trans_entry->flag == LOWER_BOUND) {
			alpha = std::max(alpha, trans_entry->sr.score);
		}
		//likewise with beta
		else if (trans_entry->flag == UPPER_BOUND) {
			beta = std::min(beta, trans_entry->sr.score);
		}

		//normal ab pruning
		if (alpha >= beta) {
			return trans_entry->sr;
		}
	}

	//initial best move seen
	SearchResult value = { {0,0,0,0,0} , (double)INT_MIN - depth - 10 };

	//get all valid moves and sort by captures, increasing ab pruning effectiveness as it is more likely that successful moves are tried earlier
	std::vector<Move> valid_moves = board->get_valid_moves(board->is_white_to_move() ? WHITE : BLACK);
	std::sort(valid_moves.begin(), valid_moves.end(), compare_moves);

	//if we have been told to search a specific move first, put this at the front
	//ab pruning means we do not need to remove it from the rest of the list, as it will barely be searched again
	if (first.player != -1) valid_moves.insert(valid_moves.begin(), first);

	//iterate through each move
	for (const Move &m : valid_moves) {
		if (board->make_move(m)) {
			SearchResult sr;

			//if 50 move rule is up or we have three folded, then this is a draw
			if (board->get_half_move_clock() >= 100 || board->is_three_move_rep()) {
				sr =  { m, 0 };
			}
			//if this is the final move of the search, get the score of the position via quiescence
			else if (depth <= 1) {
				sr = { m, -quiescence(-beta, -alpha, board) };
			}
			//if we have more to go, get the score using negamax
			else {
				sr = negamax(depth - 1, -beta, -alpha, board);
				sr.score *= -1;
			}

			//if new best, update value
			if (sr.score > value.score) {
				value = { m, sr.score };
			}
			
			//undo the move
			board->undo_move(m);

			//ab pruning
			alpha = std::max(alpha, value.score);
			if (alpha >= beta) break;
		}
	}

	//if no possible moves
	if (value.score <= (double)INT_MIN - depth - 10) {
		//if stalemate
		if (!board->in_check(board->is_white_to_move() ? WHITE : BLACK)) {
			value.score = 0;
		}
		//otherwise in checkmate, can leave score as is
	}

	//store this result in the transposition table for the future
	if (searching) {
		TransTableEntry new_trans_entry = { EXACT, value, depth };
		if (value.score <= alphaOrig) {
			new_trans_entry.flag = UPPER_BOUND;
		}
		else if (value.score >= beta) {
			new_trans_entry.flag = LOWER_BOUND;
		}
		trans_table.store(board->get_zobrist_hash(), new_trans_entry);
	}

	return value;
}

//stops the search after milliseconds
//only if we are still doing the same search - could be onto the next one if for example the other search was stopped early to play a book move
void Searcher::stop_searching(int milliseconds, int SID) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	if (SID == searchID) stop();
}

void Searcher::stop() {
	searching = false;
}

//uses iterative deepening negamax until milliseconds is up to find the best move in the position
Move Searcher::get_best_move(int milliseconds, Board *board) {
	searching = true;

	//starts timer
	searchID++;
	std::thread stop_search([this, milliseconds] { stop_searching(milliseconds, searchID); });

	//check for a book move
	//have to swap the endianness of all the fields in each entry, since they are stored in the binary field as big endian
	if (using_opening_book) {
		std::vector<BookEntry*> matches;
		int total_weight = 0;
		//collect all the entries together which fit the current position
		for (int i = 0; i < book_size; i++) {
			if (board->get_zobrist_hash() == endian_swap_u64(entries[i].key)) {
				matches.push_back(&entries[i]);
				entries[i].weight = endian_swap_u16(entries[i].weight);
				total_weight += entries[i].weight;
			}
		}
		//if we found any entries for this position
		//choose a random one based on the weighting of each entry
		if (total_weight > 0) {
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 gen(rd()); // seed the generator
			std::uniform_int_distribution<> distr(0, total_weight); // define the range
			int r = distr(gen); // generate a random integer
			int cum_weight = 0;
			for (const auto entry : matches) {
				cum_weight += entry->weight;
				//return the formatted move which we land on
				if (cum_weight >= r) {
					Move m = decipher_polyglot_move_code(endian_swap_u16(entry->move), board);
					std::cout << "Using book move" << std::endl;
					stop_search.detach();
					return m;
				}
			}
		}
	}

	trans_table.clear();

	int depth = 0;
	SearchResult sr = { {-1}, 0 };

	//stop if we searching is false or we see a guaranteed checkmate for either side
	while (searching && sr.score > (double)INT_MIN && sr.score < INT_MAX) {
		depth++;
		SearchResult tmp = negamax(depth, (double)INT_MIN, INT_MAX, board, sr.move);

		//if search at this depth concluded
		if (searching) {
			sr = tmp;
			std::cout << std::fixed << "Depth " << depth << " move " << create_lan_from_move(sr.move) << " " << sr.move.start << " " << sr.move.end << " " << " score " << sr.score << std::endl;
		}
	}

	stop_search.detach();
	return sr.move;
}

//generate all possible moves, and pick a random one
Move Searcher::get_random_move(Board *board) {
	std::vector<Move> valid_moves = board->get_valid_moves(board->is_white_to_move() ? WHITE : BLACK);
	auto rng = std::default_random_engine(time(NULL));
	std::shuffle(valid_moves.begin(), valid_moves.end(), rng);

	//keep going until we get a move that doesnt leave our king in check
	//GUI should check for stale/checkmate for us so we dont need to worry about running out of moves
	int i = 0;
	while (i < valid_moves.size() && !board->make_move(valid_moves[i])) {
		i++;
	}

	return valid_moves[i];
}

Move Searcher::decipher_polyglot_move_code(unsigned short code, Board *board) {

	//extract eahc piece of information from bit field
	int dst_file = (code >> 0) & 7;
	int dst_row = (code >> 3) & 7;
	int src_file = (code >> 6) & 7;
	int src_row = (code >> 9) & 7;
	int prom_pc = (code >> 12) & 7;

	//convert row/file to the index in board's representation
	int src_code = (7 - src_row) * 8 + src_file;
	int dst_code = (7 - dst_row) * 8 + dst_file;

	int prev_square = board->get_square(dst_code);

	//determine the start and end piece types
	int starting_piece = board->get_square(src_code) % 6;
	int end_piece = starting_piece;
	switch (prom_pc) {
	case 0:
		break;
	case 1:
		end_piece = KNIGHT;
		break;
	case 2:
		end_piece = BISHOP;
		break;
	case 3:
		end_piece = ROOK;
		break;
	case 4:
		end_piece = QUEEN;
		break;
	}

	int player = board->is_white_to_move() ? WHITE : BLACK;

	//if castle, need to change target square to just 2 squares along instead of on the rook
	if (starting_piece == KING) {
		std::vector<std::pair<int, int>> castle_moves = { std::make_pair(4, 7), std::make_pair(4, 0), std::make_pair(60, 63), std::make_pair(60, 56) };

		for (int i = 0; i < 4; i++) {
			if (src_code == castle_moves[i].first && dst_code == castle_moves[i].second) {
				prev_square = EMPTY_SQUARE;
				dst_code = src_code + (dst_code > src_code ? 2 : -2);
				break;
			}
		}
	}

	//return the extracted move
	Move m = { player, src_code, dst_code, starting_piece, end_piece, prev_square };
	return m;

	
}