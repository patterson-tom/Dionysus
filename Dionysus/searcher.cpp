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
	for (Move m : valid_moves) {
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
	for (Move m : valid_moves) {
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
void Searcher::stop_searching(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	stop();
}

void Searcher::stop() {
	searching = false;
}

//uses iterative deepening negamax until milliseconds is up to find the best move in the position
Move Searcher::get_best_move(int milliseconds, Board *board) {
	searching = true;

	//starts timer
	std::thread stop_search([this, milliseconds] { stop_searching(milliseconds); });

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